// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// HCIChannelMux.cpp
// HCI Channel Multiplexer implementation. 
// 
//

#include <bluetooth/logger.h>
#include <bluetooth/hcicommandqueue.h>
#include "linkmuxer.h"
#include "AclDataQController.h"
#include "linkconsts.h"
#include "hcifacade.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

CLinkMuxer* CLinkMuxer::NewL(CLinkMgrProtocol& aLinkMgrProtocol, CHCIFacade& aHCIFacade)
	{
	LOG_STATIC_FUNC
	CLinkMuxer* self = new (ELeave) CLinkMuxer(aLinkMgrProtocol, aHCIFacade);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CLinkMuxer::ConstructL()
	{
	LOG_FUNC
	// Get pointer to CommandQ
	iCommandController = &iHCIFacade.CommandQController();

#ifdef PROXY_COMMUNICATES
	const TUint16 noBufs = iHCIFacade.ReadACLReportingInterval();
#else
	const TUint16 noBufs = ++iHCIFacade.ReadACLReportingInterval(); // another slot for bc handle
#endif
	const TUint16 framingOverhead = iHCIFacade.ReadACLFramingOverhead();

	iDataController= CACLDataQController::NewL(iLinkMgrProtocol, 
			                                   *this,
											   KHCIACLMinDataBufferSize,
											   framingOverhead,
			                                   noBufs);

	// Tell the limited data Q controller our data credits from HC is 0 
	// when we establish how many HC really has we will notify again
	// that will happen on the reception of the first ReadBufferSize result
	iDataController->InitialDataCredits(0);

	iChannelsFree = iHCIFacade.HCTLState();

	TCallBack cb(TryToSendCallBackStatic, this);
	iSendCallBack = new (ELeave)CAsyncCallBack(cb, EActiveMedPriority);
	}

CLinkMuxer::CLinkMuxer(CLinkMgrProtocol& aLinkMgrProtocol, CHCIFacade& aHCIFacade)
	: iHCIFacade(aHCIFacade)
	, iChannelsFree(KHCITransportNoChannels)
	, iFlowControlMode(EFlowControlToHostControllerOnly) // by design flow control to the controller is always used
	, iLinkMgrProtocol(aLinkMgrProtocol)
/**
	We expect the transport to notify us when the transport channels are ready
**/
	{
	LOG_FUNC
	}

CLinkMuxer::~CLinkMuxer()
	{
	LOG_FUNC
    delete iSendCallBack;
	delete iDataController;
	}

TInt CLinkMuxer::ACLPacketMTU() const
	{
	LOG_FUNC
	__ASSERT_DEBUG(iACLPacketMTU, Panic(ELinkMgrPacketMTUUseBeforeSet));
	return iACLPacketMTU;
	}


TInt CLinkMuxer::TryToSendCallBackStatic(TAny* aCLinkMuxer)
	{
	LOG_STATIC_FUNC
	static_cast<CLinkMuxer*>(aCLinkMuxer)->DoSend();
	return EFalse;
	}

void CLinkMuxer::TryToSend()
	{
	LOG_FUNC
	// fireup async callback
	iSendCallBack->CallBack();
	}

void CLinkMuxer::DoSend()
/**
	This is the method where it is decided whether a command or data packet will be issued next.
	A send may not occur - the signal to send may have occurred through the addition to a Q
	whereas the destination channel for the relevant packet may not be free

	There could have been a 'Strategy' pattern here, for applying different
	scheduling in the future. The likely hood of this is very low, hence the 
	simpler implementation.

	The scheduling is very simple:
	If we have command packets to send and credits to do so ,send one
	otherwise send a data packet if possible/available.
*/
	{
	LOG_FUNC
	LOG1(_L("LinkMuxer: Dosend- free channels 0x%04x"), iChannelsFree);

	if (!iChannelsFree)
		return;  // no channel to send on available

	TUint   theDataQLevel=0;
	TUint16 theDataCredits=0;

	iDataController->GetDataQRecords(theDataQLevel,theDataCredits);
	
	// the assumption here is that a channel is free
	// note the implicit precedence of commands over data
	
	if (iChannelsFree & KHCITransportCommandChannel)
		{
		// have commands and command channel is free
		iCommandController->DoSend();
		}

	if((iChannelsFree & KHCITransportACLDataChannel)
		&& theDataQLevel && theDataCredits)
		{
		// have data, and credits, and data channel is free, try to send some data
		iDataController->IssueNextACLDataFragment(); // may or may not issue!
		}
	}


/**
	When channels become free this method gets called to tell the muxer
	The muxer takes the opportunity to attempt a send
*/

void CLinkMuxer::ChannelsFree(THCITransportChannel aChannel)
	{
	LOG_FUNC
	iChannelsFree |= aChannel;

	if (iChannelsFree != KHCITransportNoChannels)
		{
		// only call async callback - don't call synchronous
		TryToSend();
		}
	}
/**
	When channels become closed this method is called to remove them
	from the "free" list.
	
	Note: The channels that require closing have their respective bits set
		hence the reason for the bit inversion within this method.
*/

void CLinkMuxer::ChannelsClosed(THCITransportChannel aChannel)
	{
	LOG_FUNC
	iChannelsFree &= (~aChannel);
	iChannelsFree &= KHCITransportAllChannels;
	}


#ifdef STACK_SCO_DATA
TBool CLinkMuxer::CanWriteSCOData()
	{
	LOG_FUNC
	return (iChannelsFree & KHCITransportSCODataChannel);
	}
#endif

void CLinkMuxer::ResetFlowControlMode()
	{
	LOG_FUNC
	iFlowControlMode = EFlowControlToHostControllerOnly;
	iCtrlerToHostSet = EFalse;
	}

void CLinkMuxer::RecordHostControllerToHostFlowControl(TBool aFlowFlag)
/**
	Called when HCIFacade successfully configures some level of controller to host
	flow control
	@param	aFlowFlag - true if flow control set-up succeeded, false otherwise
**/
	{
	LOG_FUNC
	
	__ASSERT_DEBUG(!iCtrlerToHostSet, Panic(ELinkMgrFlowControlUnexpectedUpdate));
	
	switch (iFlowControlMode)
		{
	case EFlowControlToHostControllerOnly:
		if(aFlowFlag)
			{
			// Success! We're using using two way flow control
			iFlowControlMode = ETwoWayFlowControlEnabled;
			}
		else
			{
			// Fail! we only allow one shot to set this up on initialisation
			// so inform the data controller that any reserved memory can be
			// be released.
			iDataController->NoExplicitInboundPoolNeeded();
			}
		break;
		
	case ETwoWayFlowControlEnabled:
		// We shouldn't get this twice (we only have one shot of setting it up
		// and to reach this point we've been in this function once before).
		break;
	
	case EFlowControlFromHostControllerOnly:
		// fall-through
	case ENoFlowControl:
		Panic(ELinkMgrNoFlowControlSetInReleaseBuild);
		break;
	
	default:
		Panic(ELinkMgrNoSuchFlowControlMode);
		break;
		}
	
	iCtrlerToHostSet = ETrue;
	}
	



CACLDataQController* CLinkMuxer::HandleLocalReadBufferSizeResult(
			TUint16 aAclMaxLen,
			TUint8 /*aScoMaxLen*/,
			TUint16 aNoACL,
			TUint16 /*aNoSCO*/)
/**
	This method handles the results of the local (intra HCI) inquiry of the HC 
	buffer capabilities.
	The results are then used for setting the HCI MTU (=HC MTU) and for 
	modeling the HC's pool usage.
	This method must be called very early upon start-up in order to set up the 
	data Q and its controller, for the flow control between L2CAP -> HCI -> 
	HC. 

	@param aAclMaxLen Maximum length of each ACLDataPacket.
	@param aScoMaxLen Maximum length of each SCODataPacket.
	@param aNoACL Total no. of ACL Data Packets.
	@param aNoSCO Total no. of SCO Data Packets.
	@return Return address (not ownership) of new/same ACL Data Controller.
*/
    {
	LOG_FUNC
	LOG2(_L("CLinkMuxer::HandleLocalReadBufferSizeResult aAclMaxLen = %d, aNoACL = %d"), 
		aAclMaxLen, aNoACL);

	const TUint preferredNumBuffers = Max(
			static_cast<TInt>(KHCIPreferedNumberOfHCIACLDataBuffers),
			static_cast<TInt>(aNoACL)
			);

	// If the HC supports more buffers than we already allocated (in 
	// ConstructL) then make a new (bigger!) Q controller. NB This may fail, 
	// in which case just go with the earlier one- we always return from this 
	// (non-leaving) function leaving behind a valid iDataController.
	TUint16 bufSize;
	TUint numBufs;
	iDataController->GetBufferInfo(bufSize, numBufs);
	if ( preferredNumBuffers >= numBufs )
		{
		const TUint16 framingOverhead = iHCIFacade.ReadACLFramingOverhead();

		CACLDataQController* tmpDataQController = NULL;
		TRAP_IGNORE(tmpDataQController = 
 					CACLDataQController::NewL(iLinkMgrProtocol,
									*this, 
									aAclMaxLen,
									framingOverhead,
									preferredNumBuffers));
		if ( tmpDataQController )
			{
			delete iDataController;
			iDataController = tmpDataQController;
			}
		}

	// Tell the data Q controller our HC data credits 
	iDataController->InitialDataCredits(aNoACL); 

	iDataController->GetBufferInfo(bufSize, numBufs);
	iACLPacketMTU = bufSize;
 	__ASSERT_DEBUG(numBufs >= iHCIFacade.ReadACLReportingInterval(), 
		Panic(ELinkMgrDataQBufferIsNotSufficient));

	LOG1(_L("CLinkMuxer::HandleLocalReadBufferSizeResult iDataController = 0x%08x"), 
		iDataController);

	return iDataController;
	}

//
// End of file
