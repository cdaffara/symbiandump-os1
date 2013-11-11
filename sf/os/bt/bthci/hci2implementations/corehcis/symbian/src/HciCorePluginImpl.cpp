// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "HciCorePluginImpl.h"
#include "hciserver.h"
#include "hciserverrequestmgr.h"
#include "corehciutil.h"

#include <bluetooth/hci/hctlbase.h>
#include <bluetooth/hci/hctlinterface.h>
#include <bluetooth/hci/controllerinitialisationplugin.h>
#include <bluetooth/hci/controllerinitialisationinterface.h>
#include <bluetooth/hci/hctlpowerinterface.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/aclpacketconsts.h>
#include <bluetooth/hci/hcidataobserver.h>
#include <bluetooth/hci/hciutil.h>
#include <bluetooth/hci/hciipc.h>
#include <bluetooth/hci/event.h>

#include <bluetooth/hcicommandqueue.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COREHCI_SYMBIAN);
#endif

_LIT(KHciUtilComponentName, "corehci_symbian");

/*static*/ CCoreHCIPluginImpl* CCoreHCIPluginImpl::NewL()
	{
	LOG_STATIC_FUNC
	
	CCoreHCIPluginImpl* p = new(ELeave) CCoreHCIPluginImpl;
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();
	return p;
	}

void CCoreHCIPluginImpl::ConstructL()
/*
Start up the HCTL plugin, then the initialisation plugin, as per the information
in section 2.3 of PREQ750_Improvements document.
*/
	{
	LOG_FUNC
	
	// Create HCI Utility library
	iHciUtil = CHciUtil::NewL(KHciUtilComponentName);

	// If we can't open the ini file then this will be treated in the same way
	// as not reading a valid UID from the ini file.
	TRAP_IGNORE(iHciUtil->OpenIniFileL());
	
	// Create Core HCI plugin
	_LIT(KSection, "Plugins");
	_LIT(KHCTLUid, "hctl_uid");

	TUid hctlImplUid = TUid::Null();
	TRAPD(err, hctlImplUid = iHciUtil->GetUidFromFileL(KSection, KHCTLUid));

	if (err == KErrNone)
		{
		// Valid UID found, load it
		iHCTLPlugin = CHCTLBase::NewL(hctlImplUid);
		}
	else
		{
		// No UID found in ini file, attempt to load single instance of 
		// implementation
		iHCTLPlugin = CHCTLBase::NewL();
		}

	// Add this as the event observer.
	MHCTLInterface* hctl = static_cast<MHCTLInterface*>(iHCTLPlugin->Interface(TUid::Uid(KHCTLInterfaceUid)));
	__ASSERT_ALWAYS(hctl, PANIC(KCoreHciPanicCat, EUninitialisedInterface));
	
	iHCTLHardResetPerformer = static_cast<MHardResetInitiator*>(iHCTLPlugin->Interface(TUid::Uid(KHCHardResetUid)));
	__ASSERT_ALWAYS(iHCTLHardResetPerformer, PANIC(KCoreHciPanicCat, EUninitialisedInterface));				

	// Setup observers on the HCTL.
	hctl->MhiSetEventObserver(*this);
	hctl->MhiSetChannelObserver(*this);
	hctl->MhiSetDataObserver(*this);
	hctl->MhiSetControllerStateObserver(*this);
	
	// Create the initialisor. The initialisor is optional so don't worry if
	// we can't load it.
	_LIT(KInitialisorUid, "initialisation_uid");

	TUid initImplUid = TUid::Null();
	TRAP(err, initImplUid = iHciUtil->GetUidFromFileL(KSection, KInitialisorUid));

	if (err == KErrNone)
		{
		// Valid UID found, load it
		iInitialisationPlugin = CControllerInitialisationPlugin::NewL(initImplUid);
		}
	else
		{
		// No UID found in ini file, attempt to load single instance of 
		// implementation
		TRAP_IGNORE(iInitialisationPlugin = CControllerInitialisationPlugin::NewL());
		}

	if(iInitialisationPlugin)
		{
		MControllerInitialisationInterface* initialisor = 
			static_cast<MControllerInitialisationInterface*>
				(iInitialisationPlugin->Interface(TUid::Uid(KControllerInitialisationInterfaceUid)));
		
		__ASSERT_ALWAYS(initialisor, PANIC(KCoreHciPanicCat, EUninitialisedInterface));

		initialisor->MciiSetCoreHci(*this);
		}

	iHCIServer = CHCIServer::NewL(*this);

	// The power interface is not required so iHCTLPowerInterface can be NULL after
	// the following call. Check for NULL before using iHCTLPowerInterface elsewhere.
	iHCTLPowerInterface = static_cast<MHCTLPowerInterface*>(iHCTLPlugin->Interface(TUid::Uid(KHCTLPowerInterfaceUid)));

	// Get the HCTL specific frame header and trailer octet sizes.
	hctl->MhiGetCommandTransportOverhead(iHCTLCommandHeaderSize, iHCTLCommandTrailerSize);
	hctl->MhiGetAclDataTransportOverhead(iHCTLACLHeaderSize, iHCTLACLTrailerSize);
	hctl->MhiGetSynchronousDataTransportOverhead(iHCTLSyncDataHeaderSize, iHCTLSyncDataTrailerSize);
	}

CCoreHCIPluginImpl::~CCoreHCIPluginImpl()
	{
	LOG_FUNC
	
	//delete in the order given in section 2.4 of PREQ750_Improvements document
	delete iInitialisationPlugin;
	delete iHCTLPlugin;	
	delete iHCIServer;
	delete iHciUtil;
	}
	
//Request hard reset from the stack - stack will do all the required preparation - such as notify QDP and so on, and call us back via MHardResetInitiator interface
void CCoreHCIPluginImpl::RequestHardResetFromStack()
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iStackHardResetRequester, PANIC(KCoreHciPanicCat, EUninitialisedInterface));	
	iStackHardResetRequester->MhriStartHardReset();
	}
	
// Set Power received from the HCI server
TInt CCoreHCIPluginImpl::SetPower(TBTPowerState aState, TDesC8* /*aBuf*/)
	{
	LOG_FUNC
	
	if(iHCTLPowerInterface)
		{
		return iHCTLPowerInterface->MhpiSetPower(aState);
		}
	else
		{
		return KErrNotSupported;
		}
	}
	
// Get Power received from the HCI server
TInt CCoreHCIPluginImpl::GetPower(TBTPowerState& aState, TDesC8* /*aBuf*/)
	{
	LOG_FUNC
	
	if(iHCTLPowerInterface)
		{
		return iHCTLPowerInterface->MhpiGetPower(aState);
		}
	else
		{
		return KErrNotSupported;
		}
	}
	
TAny* CCoreHCIPluginImpl::Interface(TUid aUid)
	{
	LOG_FUNC
	
	TAny* ret = NULL;
	
	switch(aUid.iUid)
		{		
	case KCoreHciInterfaceUid:
		{		
		ret = reinterpret_cast<TAny*>(static_cast<MCoreHci*>(this));
		break;
		}
	
	case KHCIDataFramerInterfaceUid:
		{		
		ret = reinterpret_cast<TAny*>(static_cast<MHCIDataFramer*>(this));
		break;
		}
		
	case KHCICommandAllocatorInterfaceUid:
		{		
		ret = reinterpret_cast<TAny*>(static_cast<MHCICommandAllocator*>(this));
		break;
		}

	case KHCHardResetUid:
		{		
		ret = reinterpret_cast<TAny*>(static_cast<MHardResetInitiator*>(this));
		break;
		}
	
	case KHCTLInterfaceUid:
		{		
		ret = iHCTLPlugin->Interface(aUid);
		break;
		}
		
	case KControllerInitialisationInterfaceUid:
		{
		if (iInitialisationPlugin != NULL)
			{
			ret = iInitialisationPlugin->Interface(aUid);
			}
		break;
		}
					
	case KHCIClientUsageCallbackUid:
		{
		ret = reinterpret_cast<TAny*>(static_cast<MHCIClientUsageCallback*>(this));
		break;
		}

	default:	
		break;
		}
	
	return ret;
	}
	
void CCoreHCIPluginImpl::MhriStartHardReset()
	{
	LOG_FUNC	

	iHCTLHardResetPerformer->MhriStartHardReset();
	}
	
CHctlCommandFrame* CCoreHCIPluginImpl::MhcaNewFrameL()
	{
	LOG_FUNC
	
	return (CHctlCommandFrame::NewL(iHCTLCommandHeaderSize, iHCTLCommandTrailerSize));
	}


CHctlAclDataFrame* CCoreHCIPluginImpl::MhdfNewAclDataFrameL(TUint16 aSize)
	{
	LOG_FUNC
	
	return CHctlAclDataFrame::NewL(aSize, iHCTLACLHeaderSize, iHCTLACLTrailerSize);
	}

CHctlSynchronousDataFrame* CCoreHCIPluginImpl::MhdfNewSynchronousDataFrameL(TUint8 aSize)
	{
	LOG_FUNC
	
	return CHctlSynchronousDataFrame::NewL(aSize, iHCTLSyncDataHeaderSize, iHCTLSyncDataTrailerSize);
	}

/*virtual*/ void CCoreHCIPluginImpl::MhdfFormatAclData(CHctlAclDataFrame& aFrame, THCIConnHandle aConnH,
									TAclPacketBoundaryFlag aBoundaryFlag, TAclPacketBroadcastFlag aBroadcastFlag, 
									const TDesC8& aData)
	{
	LOG_FUNC
	
	THCIConnHandle handle = aConnH;

	if(aBroadcastFlag != EPointToPointOnly)
		{
		handle = 0xbca; //TODO KHCIBroadcastHandle;
		}

	aFrame.SetConnectionHandle(handle);
	aFrame.SetFlags(aBoundaryFlag, aBroadcastFlag);
	aFrame.SetDataPayload(aData); // also sets DataLength
	}

void CCoreHCIPluginImpl::MhdfFormatSynchronousData(CHctlSynchronousDataFrame& aFrame, 
												   THCIConnHandle aConnH, const TDesC8& aData)
	{
	LOG_FUNC
	
	THCIConnHandle handle = aConnH;
	aFrame.SetConnectionHandle(handle);
	aFrame.SetDataPayload(aData); // also sets DataLength
	}

void CCoreHCIPluginImpl::MhdoProcessAclData(const TDesC8& aData)
	{
	LOG_FUNC

	TAclPacketBoundaryFlag pbFlag = CHctlAclDataFrame::PacketBoundaryFlag(aData);
    TAclPacketBroadcastFlag bcFlag = CHctlAclDataFrame::PacketBroadcastFlag(aData);
	THCIConnHandle handle = CHctlDataFrameBase::ConnectionHandle(aData);
	iHCIDataObserver->MhdoProcessAclData(handle, pbFlag, bcFlag,
						aData.Right(aData.Length() - CHctlAclDataFrame::KHCIACLDataPacketHeaderLength));
	}

void CCoreHCIPluginImpl::MhdoProcessSynchronousData(const TDesC8& aData)
	{
	LOG_FUNC
	
	THCIConnHandle handle = CHctlDataFrameBase::ConnectionHandle(aData);
	iHCIDataObserver->MhdoProcessSynchronousData(handle, 0,
						aData.Right(aData.Length() - CHctlSynchronousDataFrame::KHCISynchronousDataPacketHeaderLength));
	}

void CCoreHCIPluginImpl::MheoProcessEvent(const TDesC8& aEvent)
	{
	LOG_FUNC

	if (THCIEventBase::CreateAndSendEvent(aEvent, *iCommandEventObserver, *iDataEventObserver) != KErrNone)
		{
		// If we got an error processing the event, reset the controller
		RequestHardResetFromStack();
		}
	}

void CCoreHCIPluginImpl::McsoProcessPowerChange(TInt aError, TControllerChangeType aChangeType,
												TBTPowerState aState)
	{
	LOG_FUNC
	
	iHCIServer->PowerControlManager().CompleteRequest(aError);
	iControllerStateObserver->McsoProcessPowerChange(aError, aChangeType, aState); //pass on up
	}

void CCoreHCIPluginImpl::McsoProcessHardResetPhaseChange(TInt aError, TControllerChangeType aChangeType,
														 TBTHardResetState aState)
	{
	LOG_FUNC
	
	iControllerStateObserver->McsoProcessHardResetPhaseChange(aError, aChangeType, aState); //pass on up
	}

TAny* CCoreHCIPluginImpl::MqpifQdpPluginInterface(const TUid aUid)
	{
	LOG_FUNC
	
	if(iHCICommandQueue)
		{
		return (iHCICommandQueue->MhcqQdpPluginInterface(aUid));
		}
		
	return NULL;
	}

TInt CCoreHCIPluginImpl::MchSetOption(TUint /*aName*/, const TDesC8& /*aData*/)
	{
	LOG_FUNC
	
	return KErrNotSupported;
	}

TInt CCoreHCIPluginImpl::MchGetOption(TUint /*aName*/, TDes8& /*aData*/)
	{
	LOG_FUNC
	
	return KErrNotSupported;
	}

void CCoreHCIPluginImpl::MchIoctl(TUint /*aLevel*/,TUint /*aName*/, TDes8* /*aOption*/, TAny* /*aStackSAP*/)
	{
	LOG_FUNC
	}

void CCoreHCIPluginImpl::MchCancelIoctl(TUint /*aLevel*/,TUint /*aName*/, TAny* /*aStackSAP*/)
	{
	LOG_FUNC
	}

void CCoreHCIPluginImpl::MchSetHCICommandQueue(MHCICommandQueue& aHCICommandQueue)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iHCICommandQueue, PANIC(KCoreHciPanicCat, EInterfaceAlreadyInitialised));

	iHCICommandQueue = &aHCICommandQueue;
	iHCIServer->DirectAccessManager().SetHCICommandQueue(*iHCICommandQueue);
	}

void CCoreHCIPluginImpl::MchSetPhysicalLinksState(MPhysicalLinksState& aPhysicalLinksState)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iPhysicalLinksState, PANIC(KCoreHciPanicCat, EInterfaceAlreadyInitialised));
	
	iPhysicalLinksState = &aPhysicalLinksState;
	}

void CCoreHCIPluginImpl::MchSetHardResetInitiator(MHardResetInitiator& aHardResetInitiator)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iStackHardResetRequester, PANIC(KCoreHciPanicCat, EInterfaceAlreadyInitialised));
	
	iStackHardResetRequester = &aHardResetInitiator;
	}

void CCoreHCIPluginImpl::MchSetDataEventObserver(MHCIDataEventObserver& aHCIDataEventObserver)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iDataEventObserver, PANIC(KCoreHciPanicCat, EInterfaceAlreadyInitialised));
	
	iDataEventObserver = &aHCIDataEventObserver;
	}

void CCoreHCIPluginImpl::MchSetDataObserver(MHCIDataObserver& aHCIDataObserver)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iHCIDataObserver, PANIC(KCoreHciPanicCat, EInterfaceAlreadyInitialised));
	
	iHCIDataObserver = &aHCIDataObserver;
	}

void CCoreHCIPluginImpl::MchSetCommandEventObserver(MHCICommandEventObserver& aHCICommandEventObserver)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iCommandEventObserver, PANIC(KCoreHciPanicCat, EInterfaceAlreadyInitialised));
	
	iCommandEventObserver = &aHCICommandEventObserver;
	}

void CCoreHCIPluginImpl::MchSetChannelObserver(MHCTLChannelObserver& aHCIChannelObserver)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iHCTLChannelObserver, PANIC(KCoreHciPanicCat, EInterfaceAlreadyInitialised));
	
	iHCTLChannelObserver = &aHCIChannelObserver;
	}

void CCoreHCIPluginImpl::MchSetControllerStateObserver(MControllerStateObserver& aControllerStateObserver)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iControllerStateObserver, PANIC(KCoreHciPanicCat, EInterfaceAlreadyInitialised));
	
	iControllerStateObserver = &aControllerStateObserver;
	}

TAny* CCoreHCIPluginImpl::MchHctlPluginInterface(const TUid aUid)
	{
	LOG_FUNC
	
	return iHCTLPlugin->Interface(aUid);
	}

void CCoreHCIPluginImpl::MhcoChannelOpen(THCITransportChannel aChannels)
	{
	LOG_FUNC
	
	// Just pass on to the upper HCLT channel observer. 
	iHCTLChannelObserver->MhcoChannelOpen(aChannels);
	}
	
void CCoreHCIPluginImpl::MhcoChannelClosed(THCITransportChannel aChannels)
	{
	LOG_FUNC
	
	// Just pass on to the upper HCTL channel observer. 
	iHCTLChannelObserver->MhcoChannelClosed(aChannels);
	}

void CCoreHCIPluginImpl::MhcucSetClientUsage(MHCIClientUsage& aClientUsage)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iClientUsage, PANIC(KCoreHciPanicCat, EInterfaceAlreadyInitialised));
	
	iClientUsage = &aClientUsage;
	}

void CCoreHCIPluginImpl::OpenClientReference()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iClientUsage, PANIC(KCoreHciPanicCat, EUninitialisedInterface));
	
	if (iClientUsage)
		{
		iClientUsage->MhcuOpenClientReference();
		}
	}

void CCoreHCIPluginImpl::CloseClientReference()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iClientUsage, PANIC(KCoreHciPanicCat, EUninitialisedInterface));

	if (iClientUsage)
		{
		iClientUsage->MhcuCloseClientReference();
		}
	}

// EOF

