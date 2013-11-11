// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalComponent
*/
#include <bluetooth/logger.h>
#include "bt_sock.h"
#include "CBnepLink.h"
#include "CSocketReader.h"
#include "CSocketWriter.h"
#include "CMultiAddrFilterTable.h"
#include "CNetTypeFilterTable.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

// Class CBnepLink 

/**
Construct a link.
@param aBridge The CBnepBridge that this link may eventually be attached to to.
@param aSocket The socket handed up to the agent from L2CAP which this link will use.
@param aParser The CBnepChannelController for this link.
@internalComponent
*/
CBnepLink::CBnepLink (MPanDeviceOwner& aBridge, RInternalSocket& aSocket, MLinkManager& aParser)
    : iLinkManager(aParser), iNegotiating(ETrue)
    {
    LOG_FUNC
    
    // Take ownership of the socket and acquire the remote end's BT address.                                                                                      	
    iSocket.Transfer(aSocket);
    TBTSockAddr btSockAddr;
    iSocket.RemoteName(btSockAddr);
    iAddr = btSockAddr.BTAddr();
    
    // We tell the link who it's owner is likely to be.
    // Note that the link is NOT yet attached, though.
    SetOwner(aBridge); 
    SetOwnerCountPtr(NULL);    
    }

/**
Remove from bridge, delete reader and writer.
@internalComponent
*/
CBnepLink::~CBnepLink()
    {
    LOG_FUNC
    
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    // With 399 we null iOwner if the CBnepBridge is deleted before all the
    // links go down (see Invalidate())
    
    // If we're not actually attached, the bridge will ignore the request.
    __ASSERT_DEBUG(iOwner,BnepUtils::Panic(Bnep::ENullPointer));
#endif
    if(iOwner)
        {
        iOwner->LinkGoingDown(*this);
        }
    
    delete iReader;
    delete iWriter;

	TRequestStatus status;
	iSocket.Shutdown(RSocket::ENormal, status);
	User::WaitForRequest(status);
	iSocket.Close();
	
    delete iMultiAddrFilters;
    delete iNetTypeFilters;
    
    iOutgoing.Reset();
    iIncoming.Reset();
    }


/**
Create the reader and writer objects, and 
attempt to attach this device to the bridge.
@internalComponent
*/
void CBnepLink::ConstructL ()
    {
    LOG_FUNC
    iReader = CSocketReader::NewL(iSocket, *this);
    iWriter = CSocketWriter::NewL(iSocket, *this);
    
    // If this leaves, we have an unattached link that will be deleted during cleanup. 
    // This is the scenario in which we don't want to attempt to notify the 
    // bridge of our demise.
    User::LeaveIfError(iOwner->AttachDevice(*this));
    
    // Start the reader running
    iReader->Read();
    }

/**
Create a BNEP link object.
@param aBridge The CBnepBridge that this link may eventually be attached to to.
@param aSocket The socket handed up to the agent from L2CAP which this link will use.
@param aParser The CBnepChannelController for this link.
@internalComponent
*/
CBnepLink* CBnepLink::NewL (MPanDeviceOwner& aBridge, RInternalSocket& aSocket, MLinkManager& aParser)
    {
    LOG_STATIC_FUNC
    CBnepLink* self = new(ELeave) CBnepLink(aBridge, aSocket, aParser);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return(self);
    }

/**
Call from the bridge to send a packet.
@param aFrame The RBnepFrame from which a packet will be created.
@internalComponent
*/
void CBnepLink::Process (RBnepFrame& aFrame)
    {
    LOG_FUNC
    if(iNegotiating)
        {
        LOG(_L8("Dropping data frame from bridge during role negotiation"));
		return;
        }

    TInt err = SendFrame(aFrame);
        
    if (err != KErrNone)
      	{
      	// Log error but don't send it to the bridge as the IP stack
       	// should not be flowed off if there is a problem with one link
        LOG1(_L8("Sending data frame returned error: %d"), err);    	
       	}
 	else
 		{
 		UpdateOwnerCount(0, aFrame.PayloadLength());
 		}
    
    }

/**
Call from a control processor to cache a control for 
later output processing. This call transfers ownership of the 
RBnepControl object down into the RBnepFrame on which it is queued.
@param aCommand The control to be cached.
@internalComponent
*/
void CBnepLink::QueueOnOutput (RBnepControl* aCommand)
    {
    
    LOG_FUNC
    iOutgoing.InsertControl(aCommand);
    }

/**
This is called from within the RunL of the SocketReader 
when new data has been presented to the incoming side of the link.
@internalComponent
*/
void CBnepLink::ReadComplete (RMBufChain& aChain)
    {
    LOG_FUNC
    iOutgoing.Reset();  // The output side is set to a known state
    // The input side of the link attempts to parse the 
    // input packet into something meaningful.          
    TRAPD(err, iIncoming.ParseBnepFrameL(aChain, LocalAddr(), iAddr));
    aChain.Free(); // we're finished with this now, regardless.

    if (!err)
        {
        Proceed(); // Start processing the contents of the input side of the link.
        }
    else
        {
        iIncoming.Reset(); // Discard the contents of the input side: they are unusuable.
        }
    }

/**
The remote device has disconnected.
@internalComponent
*/
void CBnepLink::RemoteDeviceDisconnect (TInt aError)
    {
    
    LOG_FUNC
    iReader->Cancel();
    iWriter->Cancel();
    iLinkManager.RemoteDeviceDisconnect(aError);
    }

/**
(Re)start data transfer
@internalComponent
*/
void CBnepLink::ResumeDataTransfer ()
    {
    LOG_FUNC
    iNegotiating = EFalse;
	}

/**
Request to send a frame from either the bridge or the channel controller
@param aFrame the frame to be sent
@internalComponent
*/
TInt CBnepLink::SendFrame (RBnepFrame& aFrame)
    {
    LOG_FUNC
	TBool filterEthernetPayload = EFalse;
	TInt rerr = KErrNone;
	
    if(!aFrame.Includes(RBnepFrame::EControl)) // Don't filter controls
        {
        LOG(_L8("Not a control message"));
        if(iMultiAddrFilters) // If no Multicast Address filters have been set, pass this regardless
            {
            LOG(_L8("We have Multicast Address filters set"));
            if (RBnepFrame::EMulticast == aFrame.DestinationType()) // Is the destination a valid multicast address?
                { 
                LOG(_L8("We have a Multicast destination type"));   	
                TBTDevAddr dest = aFrame.DestAddr();
                if(iMultiAddrFilters->FilteredOut(dest)) // // Are we within a permissible range?
                    {
					filterEthernetPayload = ETrue;
                    }
                }
            }

        if(iNetTypeFilters) // If no Network Type Protocol filters have been set, pass this regardless.
            {
            LOG(_L8("We have Network Protocol Type filters set"));
            TUint16 protocol = aFrame.Protocol();
            LOG1(_L8("Frame protocol = %04x"),protocol);
            if(iNetTypeFilters->FilteredOut(protocol))// Are we within a permissible range?
                {
				filterEthernetPayload = ETrue;                
				}
            }
        }

	// Check there is something to send.
	if(aFrame.Includes(RBnepFrame::EControl) ||
	   (aFrame.Includes(RBnepFrame::EDataFrame) && (!filterEthernetPayload || aFrame.Includes(RBnepFrame::EExtensionHeaders))))
	   	{
		// there is something valid to send.
	    RMBufChain chain; // Beware the potential memory leak.
	    TRAP(rerr, aFrame.BuildBnepFrameL(chain, LocalAddr(), iAddr, filterEthernetPayload));
	    if (!rerr)
	        {
	        iWriter->Write(chain); // SocketWriter will free the chain for us.
	        }
	    else
	        {
	        chain.Free(); // Otherwise we need to plug the potential leak.
	        }
	   	}
	// ELSE: Nothing to send.  Return KErrNone.

    return rerr;
    }

/**
Suspend data transfer whilst new roles are negotiated
@internalComponent
*/
void CBnepLink::SuspendDataTransfer ()
    {
    LOG_FUNC
    iNegotiating = ETrue;
    }

void CBnepLink::RemoteDeviceReady()
	{
    iOwner->RemoteDeviceReady();
	}

/**
Install a table of Multicast Address filters on this link. 
Any existing table will be deleted.
@param aFilterTable The filter table to be installed. Can be NULL, which disables the filter.
@internalComponent
*/
void CBnepLink::Install (CMultiAddrFilterTable* aFilterTable)
    {
    delete iMultiAddrFilters;
    iMultiAddrFilters = aFilterTable;
    }

/**
Install a table of Network Protocol Type filters on this link.
Any existing table will be deleted.
@param aFilterTable The filter table to be installed. Can be NULL, which disables the filter.
@internalComponent
*/
void CBnepLink::Install (CNetTypeFilterTable* aFilterTable)
    {
    delete iNetTypeFilters;
    iNetTypeFilters = aFilterTable;
    }

/**
The control processing method of the link. This method supports re-entrant calls 
because of the ambiguity of the interface between BNEP and the PAN Agent.
Input side processing has priority over output side. When all input side 
processing has finished, the output side will be addressed if there is 
anything outstanding.
@internalComponent
*/
void CBnepLink::Proceed ()
    {
    LOG_FUNC
    
    if(iInvalid)
    	{
    	// The link is only ever put into an invalid state when the
    	// CBnepBridge it is attached to is deleted.
    	// As the CBnepChannelControllers (and therefore CBnepLinks) are owned
    	// by Pan Agent, the CBnepBridge ignores tidying them up, leaving the
    	// job upto Pan Agent. The design of Bnep is such that it is assumed
    	// that Pan Agent will tidy up these links in the same operation that 
    	// deletes the packet driver (which is our local device bnep
    	// representation).
    	// As this behaviour is determined by the NifMan implementation, we get
    	// Bnep to "invalidate" the links attached to it on deletion, such that
    	// if we find they are left dangling in debug builds we will be 
    	// informed by a panic, and otherwise we will handle the strange
    	// situation as gracefully as possible by dropping packets on this
    	// rogue link.
    	__ASSERT_DEBUG(EFalse, BnepUtils::Panic(Bnep::EProceedingOnInvalidLink));
    	iIncoming.Reset();
    	return;
    	}

    if(iIncoming.Includes(RBnepFrame::EControl))
        {
        LOG(_L8("Proceeding with a control message"));
        iIncoming.ExecuteControl(iLinkManager);
        }

	UpdateOwnerCount(iIncoming.PayloadLength(), 0);
	
    if(iIncoming.Includes(RBnepFrame::EDataFrame))
        {
        // Only send if we are enabled for data transfer. 
        // Otherwise the packet is dropped.
        if(!iNegotiating)
            {
            iIncoming.SetUplinkAccessAllowedForBnepLink(iUplinkAccessAllowed);
            iOwner->Process(iIncoming, iAddr);
            }
        else
            {
            LOG(_L8("Negotiating roles: inbound traffic data not allowed: dropping inbound frame."));
            }
        }
    
    
    // All input side processing is now 'finished'. 
    
	// If we've finished with the incoming packet...
    if (iIncoming.IsEmpty())
    	{
    	// We can now queue a new read.
    	iReader->Read();
    	
    	// Should we be sending a response yet?
		if (!iOutgoing.IsEmpty() && iOutgoing.Includes(RBnepFrame::EControl) &&	// We have controls to respond with
			!iLinkManager.AwaitingResponse())									// We're not still waiting for something else
	        {
	        TInt err = SendFrame(iOutgoing);
	        if (err)
	            {
	            LOG1(_L8("Dropped a packet: SendFrame error %d"),err);
	            }
			else
				{
				UpdateOwnerCount(0, iOutgoing.PayloadLength());
				}            
	        }
    	}
    }

/**
Stops all pending reads on a link..
*/
void CBnepLink::Stop ()
    {
    iReader->Cancel();
    }
    

/**
Invalidates the link when it is no longer connected to a bridge.
*/
void CBnepLink::Invalidate ()
	{
	iInvalid = ETrue;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY	
    iOwner = NULL; // No longer valid
#endif
	}

void CBnepLink::UpdateOwnerCount(TInt aReceivedCount, TInt aSentCount)
	{
	if(iOwnerCountPtr && (aReceivedCount>0 || aSentCount>0)) // iOwnerCountPtr will be NULL if CBnepBridge::iBnepPacketNotifier is NULL
 		{
  		iOwnerCountPtr->iBytesReceived += aReceivedCount;
 		iOwnerCountPtr->iBytesSent += aSentCount;
  		iOwner->DataCountUpdated();
  		}
  	}

void CBnepLink::SetUplinkAccessAllowedForBnepLink(TBool aAllowed)
	{
	iUplinkAccessAllowed = aAllowed;
	}

		
