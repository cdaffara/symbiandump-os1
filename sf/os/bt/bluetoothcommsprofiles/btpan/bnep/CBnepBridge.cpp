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

#include <e32std.h>
#include <bluetooth/logger.h>
#include <bttypes.h>
#include <comms-infras/eintsock.h>
#include <elements/nm_interfaces.h>
#include "CBnepBridge.h"
#include "CBnepChannelController.h"
#include "RBnepFrame.h"
#include "bneputils.h"
#include "CBnepLocalDevice.h"
#include "CBnepLink.h"
#include <networking/pktdrv.h>
#include "pancommon.h"

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include <elements/nm_node.h>
#include <comms-infras/ss_common.h>
#include "panmessages.h"
#include <bt_sock.h>
#include <bttypes.h>

using namespace ESock;
using namespace Messages;

#ifdef ESOCK_EXTLOG_ACTIVE
_LIT8(KBnepSubTag, "bnep");
#endif

#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

#ifdef _DEBUG
PANICCATEGORY("panbnep");
#endif

// Class CBnepBridge 
/**
   Create the bridge and attach the local device to it.
*/
CBnepBridge::CBnepBridge (MPanDevice& aLocalDevice) : iDataTransferred(KMaxPanConnections)
    {
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    NM_LOG_NODE_CREATE(KBnepSubTag, CBnepBridge);
#endif

    CONNECT_LOGGER
    LOG_FUNC
    iLinks[KBnepLocalDeviceArrayId] = &aLocalDevice; 
    iLinkCount = 1;
    LOG2(_L8("CBnepBridge::iLinks[%d] = 0x%08x"),KBnepLocalDeviceArrayId,iLinks[KBnepLocalDeviceArrayId]);
    }

/**
   Destroy the bridge.  Does not delete the devices as they are not owned by
   the bridge. The links will be deleted when the CBnepChannelControllers are deleted.
*/
CBnepBridge::~CBnepBridge()
    {
    LOG_STATIC_FUNC
    /**
       Destroy the bridge.  Does not delete the devices as they are not owned by
       the bridge. The links will be deleted when the CBnepChannelControllers are deleted.
       @internalComponent
    */

    delete iDataCounterTimer;

    if (iBnepPacketNotifier)
    	{
    	iBnepPacketNotifier->MbpnbRelease();
    	iBnepPacketNotifier = NULL;
    	}
   

    iLib.Close();
    iDataTransferred.Close();

    LOG1(_L8("CBnepBridge holds %d links on destruction - Invalidating links..."),iLinkCount);
    for (TUint index = 0; index < KMaxPanConnections; ++index)
        {
        if((index != KBnepLocalDeviceArrayId) && (iLinks[index]))
            {
			// Invalidate the link as it is no longer bound to a bridge.
            static_cast<CBnepLink*>(iLinks[index])->Invalidate();
            }
        }
        
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    // Deregister custom messages with the comms transport
    TPanMessage::DeRegister();

    NM_LOG_NODE_DESTROY(KBnepSubTag, CBnepBridge);

	delete iPanLinkControl;
#endif

	CLOSE_LOGGER
    }

/**
   Add a new device to the bridge
   @return KErrOverflow The maximum number of supported devices are already
   in the network.
*/
TInt CBnepBridge::AttachDevice (MPanDevice& aDevice)
    {
    LOG_FUNC
    TUint spareSlot;
    TInt err = GetVacantSlot(spareSlot);
    if(KErrNone == err)
        {
        iLinks[spareSlot] = &aDevice;
        iLinkCount++;
        LOG2(_L8("CBnepBridge attaching iLinks[%d] = 0x%08x"),spareSlot,iLinks[spareSlot]);
        LOG1(_L8("CBnepBridge holds %d links"),iLinkCount);
        if (iBnepPacketNotifier) 
        	{
        	TBnepBytesTransferred counter;
        	counter.iBytesReceived=0;
        	counter.iBytesSent=0;
        	counter.iBTDevAddr=aDevice.iAddr;
        	
	      	__ASSERT_ALWAYS(iDataTransferred.Append(counter)==KErrNone,BnepUtils::Panic(Bnep::EAppendFailed)); // Shouldn't fail as the granularity of the array is the maximum number of devices available.
	      	
 	      	TInt pos = iDataTransferred.Find(counter);
			__ASSERT_ALWAYS(pos != KErrNotFound, BnepUtils::Panic(Bnep::EAppendFailed));
			TBnepBytesTransferred* countPtr = &iDataTransferred[pos];
 			aDevice.SetOwnerCountPtr(countPtr);
	  
	      	iBnepPacketNotifier->MbpnDeviceAdded(aDevice.iAddr);
	      	__ASSERT_ALWAYS(iDataCounterTimer,BnepUtils::Panic(Bnep::ENullPointer));
    	    if (!iDataCounterTimer->IsActive())
    			{
    				iIdleCount=0;
    				iDataCounterTimer->Start(KBnepPacketNotifyInterval,KBnepPacketNotifyInterval,TCallBack(DataCounterCallBack,this)); // Start the timer if it's not already started
    			}
        	}

        }
    return err; 
    }

/**
   Get the default (local) address for this bridge (i.e. address of it's local device).
*/
const TBTDevAddr& CBnepBridge::DefaultAddress () const
    {
    LOG_FUNC
    __ASSERT_ALWAYS(iLinks[KBnepLocalDeviceArrayId],BnepUtils::Panic(Bnep::ENullPointer));
    LOG_BT(_L8("CBnepBridge::DefaultAddress: %S"),iLinks[KBnepLocalDeviceArrayId]->iAddr);
    return iLinks[KBnepLocalDeviceArrayId]->iAddr; 
    }

/**
   Notify the bridge of the imminent destruction of a link.. 
   This is normally called by a CBnepLink destructor, to notify the bridge that 
   it is going down. However, it is possible for a CBnepLink instance to exist 
   for a time without being attached to the bridge. This instance is silently managed.
*/
void CBnepBridge::LinkGoingDown (MPanDevice& aDevice)
    {
    LOG_FUNC
    // We may safely ignore the return value here.
    RemoveDevice(aDevice);
    }

void CBnepBridge::RemoteDeviceReady ()
	{
    LOG_FUNC
	// Inform the local device that a remote device is ready
	static_cast<CBnepLocalDevice*> (iLinks[KBnepLocalDeviceArrayId])->RemoteDeviceReady();
	}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
   Create a new bnep channel as a result of an incoming request
   @param aMessage The message that requested the new channel controller
*/
void CBnepBridge::AttemptNewBnepConnection(const Messages::TNodeId& aSender, const Messages::TSignatureBase& aMessage, TUint16 aActivityId)
    {
    LOG_FUNC
    CBnepChannelController* bnepChannelControl = NULL;

	// If this assert fails check your commdb. It probably means that a non-399 commdb
	// has been imported and is using the basic Ethernet MCPR rather than the Pan
	// Ethernet MCPR
	ASSERT_DEBUG(iPktDrvOwner);

    // The RInternalSocket pointer in the message is owned by PAN and must
    // not be deleted or used. The internal socket implementation must be
    // transfered to a local RInternalSocket (see CBnepLink)
    const TPanMessage::TCreateChannelController& msg = message_cast<const TPanMessage::TCreateChannelController>(aMessage);
    
    // msg.iSender in this case should be an instance of the CPanRemoteDeviceStateMachine
    TRAPD(err, bnepChannelControl = CBnepChannelController::NewL(*this, *msg.iSocket, aSender, *iPktDrvOwner));
    if (err != KErrNone)
        {
        // Report failure to create
        TPanMessage::TChannelControllerCreated failMsg(err);
        RClientInterface::OpenPostMessageClose(TNodeCtxId(aActivityId,Id()),aSender,failMsg);
        return;
        }

	// Same timing as pre-399, give the notifier something to call into
    if (iBnepPacketNotifier) 
    	{
    	iBnepPacketNotifier->MbpnSetLinkControl(*iPanLinkControl);
    	}

    // Send the ack on behalf of the new channel controller
    TPanMessage::TChannelControllerCreated successMsg(KErrNone);
    RClientInterface::OpenPostMessageClose(TNodeCtxId(aActivityId,bnepChannelControl->Id()),aSender,successMsg);
    }

#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

/**
   Create a new bnep channel as a result of an incoming request
   @param aConnectedSocket A connected L2CAP socket, ready for BNEP signalling
   @param aNotify Object to inform of events
*/
MBnepChannelControl& CBnepBridge::NewBnepConnectionL(RInternalSocket& aConnectedSocket, MPanConnectionNotify& aNotify)
    {
    LOG_FUNC
    MBnepChannelControl& bnepChannelControl = *CBnepChannelController::NewL(*this, aConnectedSocket, aNotify);
    if (iBnepPacketNotifier) 
    	{
    	iBnepPacketNotifier->MbpnSetLinkControl(aNotify.BnepConnectionController());
    	}
    return bnepChannelControl;
    }
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CBnepBridge* CBnepBridge::NewL (MPanDevice& aLocalDevice)
    {
    LOG_STATIC_FUNC
    CBnepBridge* self = new(ELeave) CBnepBridge(aLocalDevice);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self; 
    }

void CBnepBridge::ConstructL() 
	{
    LOG_FUNC
	        
    _LIT(KBnepPacketNotifier,"bnepPacketNotifier.dll");
    
    if (iLib.Load(KBnepPacketNotifier)==KErrNone) 
    	{
    	TLibraryFunction func=iLib.Lookup(1);
    	MBnepPacketNotifierBase* notifier;
    	notifier = (MBnepPacketNotifierBase*) (*func)();
    	iBnepPacketNotifier = (MBnepPacketNotifier*) notifier->MbpnbGetInterface(KUidBnepPacketNotifierV1);
    	__ASSERT_DEBUG(iBnepPacketNotifier, BnepUtils::Panic(Bnep::ENullPointer));
    	
    	if (iBnepPacketNotifier) 
    		{
    		iDataCounterTimer=CPeriodic::NewL(CActive::EPriorityStandard);
    		}
 		}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    // Register with the comms transport so we can receive custom messages
    TPanMessage::RegisterL();
	// Create our own MPanLinkControl implementation
	iPanLinkControl = CPanLinkControl::NewL();
#endif    
	}

TInt CBnepBridge::DataCounterCallBack(TAny* aThis) 
	{
    LOG_STATIC_FUNC
	CBnepBridge* self = static_cast<CBnepBridge*>(aThis);
	self->DataCounterNotify();
	return KErrNone;
	}

void CBnepBridge::DataCounterNotify()
	{
    LOG_FUNC
	TBool dataTransferred=EFalse;
	iBnepPacketNotifier->MbpnBnepDataTransferred(iDataTransferred.Array()); // Notify the number of bytes transferred
	for (TUint dev=0;dev<iDataTransferred.Count();++dev)
		{
		if (iDataTransferred[dev].iBytesSent!=0 || iDataTransferred[dev].iBytesReceived!=0)
			{
			iDataTransferred[dev].iBytesSent=0;
			iDataTransferred[dev].iBytesReceived=0;
			dataTransferred=ETrue;
			}
		} // Work out whether any bytes have been transferred
	if (!dataTransferred)  // If not
		{
		iIdleCount++; // Increase the idle count by 1
		if (iIdleCount >= KBnepPacketNotifySleepTicks) // If the idle count has been hit
			{
			__ASSERT_ALWAYS(iDataCounterTimer,BnepUtils::Panic(Bnep::ENullPointer));
			iDataCounterTimer->Cancel(); // Cancel the timer
			}
		}
	else  // If data has been transferred
		{
		iIdleCount=0; // Reset the idle counter
		}
	}

/**
   Pass a BNEP frame onto the required device(s).
   Possible options are: local device, multicast address, single remote device.
   @return KReadyForMoreData in all cases.
*/
TInt CBnepBridge::Process (RBnepFrame& aFrame, const TBTDevAddr& aSourceIfAddr)
    {  
    LOG_FUNC
    TBool foundDevice = EFalse;
    LOG(_L8("CBnepBridge: Delivering packet:"));
    LOG_BT(_L8("CBnepBridge: Source address: %S"), aFrame.SrcAddr());
    LOG_BT(_L8("CBnepBridge: Destination address: %S"), aFrame.DestAddr());
    TUint link;
    switch (aFrame.DestinationType())
        {
        case RBnepFrame::ELocal:
            LOG(_L8("CBnepBridge: Frame type: RBnepFrame::ELocal"));	
            iLinks[KBnepLocalDeviceArrayId]->Process(aFrame);
            break;

        case RBnepFrame::EMulticast:
            LOG(_L8("CBnepBridge: Frame type: RBnepFrame::EMulticast"));
            LOG_BT(_L8("Source address %S"),aSourceIfAddr);
            for (link = 0; link < KMaxPanConnections; link++)
                {
                LOG1(_L8("iLinks[%d]"),link);
                if ((iLinks[link] != NULL) &&
                    (!(aSourceIfAddr == (iLinks[link]->iAddr))) &&
                    (!(aFrame.SrcAddr() == (iLinks[link]->iAddr))) )
                    {
                    LOG_BT(_L8("Multicasting to %S"), iLinks[link]->Addr());
                    iLinks[link]->Process(aFrame);
                    }
                }
            break;

        case RBnepFrame::ENormal:
            LOG(_L8("CBnepBridge: Frame type: RBnepFrame::ENormal"));
            for (link = 0; link < KMaxPanConnections; link++)
                {
                if (iLinks[link])
                    {
                    MPanDevice* linkDev = iLinks[link];
                    if (linkDev->ForThisDevice(aFrame.DestAddr()))
                        {
                        LOG_BT(_L8("Found match, delivering to %S"), linkDev->Addr());
                        linkDev->Process(aFrame);
                        foundDevice = ETrue;
                        break;
                        }
                    }
                }
            // unicast frame to a MAC address we haven't found.
            // We must be in U role connected to a GN/NAP.
            // In which case this bridge's link count must be EXACTLY 2 
            // - this device and the GN. 
            // Otherwise it is a GN and therefore has no
            //  sensible destination for this frame.
            if(!foundDevice && (2 == iLinkCount) )
                {
                // Never assume thak iLinks[0] is always going to be the local device.
                // iLinks[KLocalDeviceArrayId] is the local device.
                for (link = 0; link < KMaxPanConnections; ++link) 
                    {
                    // skip the local connection
                    if(KBnepLocalDeviceArrayId != link)
                        {
                        if(iLinks[link])
                            {
                            iLinks[link]->Process(aFrame);	
                            }
                        }
                    }
                }
            break;	

        default:	
            // Obviously we shouldn't get here under normal circumstances.
            // In a UDEB build we'll log the event.
            // Otherwise, just drop the frame.
            LOG(_L8("CBnepBridge: Invalid destination type."));
        }
    aFrame.Reset();
    
   	return (KReadyForMoreData);

    }


void CBnepBridge::DataCountUpdated()
	{
    LOG_FUNC
	__ASSERT_ALWAYS(iDataCounterTimer,BnepUtils::Panic(Bnep::ENullPointer));
	if (!iDataCounterTimer->IsActive()) // If the timer is not active
		{
	    iIdleCount = 0; // Reset the idle counter
 	    iDataCounterTimer->Start(KBnepPacketNotifyInterval,KBnepPacketNotifyInterval,TCallBack(DataCounterCallBack,this)); // Start the timer
 	    }
 	}


/**
	Set a flag to enable a connection retry to occur - this means that Nifman will not be signalled when we disconnect all devices
*/
void CBnepBridge::SetRetryConnect () 
	{
    LOG_FUNC
	iRetryConnect=ETrue;
	}


/**
   Remove a device from the bridge.
*/
void CBnepBridge::RemoveDevice (MPanDevice& aDevice)
    {
    LOG_FUNC
    for (TUint index = 0; index < KMaxPanConnections; ++index)
        {
        if(iLinks[index] == &aDevice)
            {
            iLinks[index] = NULL;
            LOG1(_L8("CBnepBridge removed iLink[%d]"),index);
            iLinkCount--;
            LOG1(_L8("CBnepBridge has %d links"),iLinkCount);
			if (iBnepPacketNotifier) 
        		{
        		for (TUint dataIndex=0; dataIndex < iDataTransferred.Count(); ++dataIndex)
        			{
        			if (iDataTransferred[dataIndex].iBTDevAddr==aDevice.iAddr) 
        				{
        				iDataTransferred.Remove(dataIndex);
        				}
        			}
        		iBnepPacketNotifier->MbpnDeviceRemoved(aDevice.iAddr);
        		}

			// If no more remote devices, tell the local device
        	if (iLinkCount == 1 && !iRetryConnect)
        		{
	            LOG1(_L8("CBnepBridge::RemoveDevice - Only local link remaining [0x%08x]"), iLinks[KBnepLocalDeviceArrayId]);
        		static_cast<CBnepLocalDevice*> (iLinks[KBnepLocalDeviceArrayId])->AllRemoteDevicesDisconnected ();
        		}
            
            iRetryConnect = EFalse;
            
            break;
            }
        }
    }

/**
   Forces a disconnect on all links.
*/
void CBnepBridge::StopLinks ()
    {
    LOG_FUNC
    for (TUint index = 0; index < KMaxPanConnections; ++index)
        {
        if((KBnepLocalDeviceArrayId !=index) &&
           (NULL != iLinks[index]))
            {
            iLinks[index]->Stop(); // Stops all pending reads.
            }
        }
    }

/**
   Attempts to find the index of a vacant slot in the bridge device array.
   @param aSlotIndex holds the array index of the vacant slot on success
   @return KErrNone on success
   @return KErrOverflow if there is no vacant slot
*/
TInt CBnepBridge::GetVacantSlot (TUint& aSlotIndex)
    {
    LOG_FUNC
    
    for(TUint index = 0; index < KMaxPanConnections; ++index)
        {
        if(NULL == iLinks[index])
            {
            aSlotIndex = index;
            return KErrNone;
            }
        }
    return KErrOverflow;
    }


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
Entry point from the comms transport
*/
void CBnepBridge::ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& /* aRecipient */, Messages::TSignatureBase& aCFMessage)
    {
    LOG_FUNC
	if (aCFMessage.MessageId().Realm() == TPanMessage::ERealmId)
    	{
        switch (aCFMessage.MessageId().MessageId())
            {
            case TPanMessage::TCreateChannelController::EId:
                {
                const TNodeCtxId& ctxId = address_cast<TNodeCtxId> (aSender);
                TUint activityId = ctxId.NodeCtx();
                AttemptNewBnepConnection(ctxId.Node().NodeId(), aCFMessage, activityId);
                }
                break;            
            
            case TPanMessage::TSetRetryConnect::EId:
                {
                // iSender in this case should be the CPanAgent
                SetRetryConnect();
                TPanMessage::TSetRetryConnectComplete msg(KErrNone);
                RClientInterface::OpenPostMessageClose(Id(),aSender,msg);	        
                }
                break;
            
            default:
                __ASSERT_DEBUG(EFalse, BnepUtils::Panic(Bnep::EUnexpectedMessage));
            }
    	}
    else
        {
        __ASSERT_DEBUG(EFalse, BnepUtils::Panic(Bnep::EUnexpectedMessage));
        }
    
    // Absorb messages
    aCFMessage.ClearMessageId();
    }

CBnepBridge::CPanLinkControl* CBnepBridge::CPanLinkControl::NewL()
    {
    LOG_STATIC_FUNC
    CBnepBridge::CPanLinkControl* self = new(ELeave) CPanLinkControl();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self; 
    }

CBnepBridge::CPanLinkControl::CPanLinkControl()
	{
    LOG_FUNC
	}
		
void CBnepBridge::CPanLinkControl::ConstructL()
	{
    LOG_FUNC
	User::LeaveIfError(iSockServ.Connect());
	}

CBnepBridge::CPanLinkControl::~CPanLinkControl()
	{
    LOG_FUNC
	iSockServ.Close();
	}

TAny* CBnepBridge::CPanLinkControl::MplcbGetInterface(const TUid & aInterface)
	{
    LOG_FUNC
    if (aInterface ==KUidPanLinkControlV1) 
        {    
        return this;
        }    
    return NULL;
	}

TInt CBnepBridge::CPanLinkControl::MplcRequestSniff(const TBTDevAddr &aAddr)
	{
    LOG_FUNC
    RBTPhysicalLinkAdapter physicalLink;
    TInt err = physicalLink.Open(iSockServ, const_cast<TBTDevAddr&>(aAddr));
	if(err == KErrNone)
		{
    	err = physicalLink.ActivateSniffRequester();
    	physicalLink.Close();
		}
	return err;
	}

TInt CBnepBridge::CPanLinkControl::MplcRequestActive(const TBTDevAddr &aAddr) 
	{
    LOG_FUNC
    RBTPhysicalLinkAdapter physicalLink;
    TInt err = physicalLink.Open(iSockServ, const_cast<TBTDevAddr&>(aAddr));
	if(err == KErrNone)
		{
    	physicalLink.ActivateActiveRequester();
    	physicalLink.Close();
		}
	return err;
	}
#endif

