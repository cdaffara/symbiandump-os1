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
// This implements the CLinkMgrProtocol object that is the contact point from
// BT PRT to the LinkMgr stack
// At present this class also remembers the configuration of the controller
// -- no particular reason - could be encapsulated in a class that the facade
// knows about.
// 
//

#include <bt_sock.h>
#include <bluetooth/logger.h>
#include "debug.h"
#include <utf.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/aclsockaddr.h>

#include "linkmgr.h"
#include "hcifacade.h"
#include "bt.h"
#include "hostresolver.h"
#include "linkmuxer.h"
#include "physicallinksmanager.h"
#include "linkconsts.h"
#include "eirmanserver.h"
#include "eirmanager.h"
#include "eirpublisherlocalname.h"
#include "eirpublishertxpowerlevel.h"
#include "bredrcontrollerconfiguration.h"


#include "ProxySAP.h"
#include "SCOSAP.h"
#include "ACLSAP.h"
#include "VendorSAP.h"
#include "vendorspecific.h"

#include "Subscribers.h"
#include "BTSec.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

#ifdef _DEBUG
PANICCATEGORY("linkmgr");
#endif

const TInt KLengthOfDeviceClass = 11;			// 11 bits in the device class portion of the CoD
const TInt KStartingOffsetOfDeviceClass = 2;	// 2 LSB bits for format type
const TUint32 KDeviceClassReset = 0;			// used to reset persistent data

// Constant which can be ANDed with a complete CoD bitmask to extract the device class portion.
const TInt KDeviceClassBitMask = ((1 << KLengthOfDeviceClass) - 1) << KStartingOffsetOfDeviceClass;


enum TLinkMgrPanic
	{
	ELinkMgrProtocolOpenedAfterClose,
	ELinkMgrProtocolBadAFHHostChannelClassification,
	EUnknownFatalError,
	ELocalDeviceBadHandle,
	ERegistrySessionClosedTooManyTimes,
	ERegistrySessionDeletedWhenOpen,
	};

void Panic(TLinkMgrPanic aPanic)
// Panic L2CAP due to stack bug.
	{
	LOG_STATIC_FUNC
	_LIT(KL2CAPPanicName, "BTLinkMgr");
	User::Panic(KL2CAPPanicName, aPanic);
	}

// Construction & Initialisation

CLinkMgrProtocol::CLinkMgrProtocol(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
: CBluetoothProtocolBase(aSecMan, aControlPlane, aCodMan)
, iDebugModeDefinitionResult(KErrGeneral)
	{
	LOG_FUNC
	TCallBack cb(TryToClose, this);
	iIdleTimerEntry.Set(cb);
	}

CLinkMgrProtocol::~CLinkMgrProtocol()
/**
	Delete all our stuff
**/
   
	{
	LOG_FUNC
	RemoveIdleTimerEntry();
	
	// Delete setter P&S values
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothSetScanningStatus);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothSetLimitedDiscoverableStatus);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothSetDeviceClass);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothSetAFHHostChannelClassification);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothSetAFHChannelAssessmentMode);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothSetDeviceName);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothSetAcceptPairedOnlyMode);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothSetPageScanParameters);

	// Delete standard P&S values (possibly trying to delete already deleted shared set+get values)
	iProperty.Delete(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothGetLocalDeviceAddress);
	iProperty.Delete(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothGetPHYCount);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothGetConnectingStatus);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothGetScanningStatus);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothGetLimitedDiscoverableStatus);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothGetDeviceClass);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothGetDeviceName);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothGetAcceptPairedOnlyMode);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothHostResolverActive);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothSetSimplePairingDebugMode);
	iProperty.Delete(KPropertyUidBluetoothCategory,	KPropertyKeyBluetoothGetPageScanParameters);


	delete iPhysicalLinksMgr;
	delete iInquiryMgr;
	delete iEirManServer;
	delete iACLStateFactory;
	delete iSCOStateFactory;

	SecMan().ClearHCICommandQueue(); // clears up the commands
	SecMan().ClearLinksMgrProtocol();
	SecMan().ClearPhysicalLinksMgr();
	
	delete iLinkMuxer;
	delete iHCIFacade;
	
	delete iRegistryUpdater;

	DeletePublications();
	iSubscribers.ResetAndDestroy();

	iLocalDevice.Close();
	delete iRegSess;
	}

CLinkMgrProtocol* CLinkMgrProtocol::NewL(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
	{
	LOG_STATIC_FUNC
	CLinkMgrProtocol* i=new (ELeave) CLinkMgrProtocol(aSecMan, aControlPlane, aCodMan);
	return i;
	}

void CLinkMgrProtocol::InitL(TDesC& /*aTag*/)
/**
	Pre-binding initialise.
	Allocate any objects we need here.
	ESOCK ensures that this function will only be called once per
	object of this class, regardless of how long it hangs around.
**/
	{
	LOG_FUNC

	__ASSERT_DEBUG(!iClosePending, Panic(ELinkMgrProtocolOpenedAfterClose));
	__ASSERT_DEBUG(!iIdleEntryQueued, Panic(ELinkMgrProtocolOpenedAfterClose));
	__ASSERT_DEBUG(!iHCIFacade, Panic(ELinkMgrProtocolOpenedAfterClose));

	// Get the local device settings from persistent store
	// By keeping a session open for the lifetime of this protocol
	// we prevent RegServ being created during connection time.
	LOG(_L("LinkMgr : Initialising RegistryServer"));
	iRegSess = CRegistrySession::NewL(*this);
	TInt err = iLocalDevice.Open(iRegSess->RegServ());
	
	if (err ==KErrNone)
		{
		// read local device settings (e.g page scan\inquiry scan) from registry
		err = iLocalDevice.Get(iLocalDeviceSettings);	// ignore error - can continue
		__ASSERT_DEBUG(err==KErrNone, Panic(EUnknownFatalError));		
		
		
		//
		// Mask off the service class bits from the CoD -- we're not interested in the
		// registry's values for anything other than the device class part.
		// There's a nice terminology overload here, as the 'Class of Device' contains
		// both a device class and a service class.  It might have been nice if the
		// getter and setter methods were 'ClassOfDevice', but even that wouldn't help
		// much...
		//
		iLocalDeviceSettings.SetDeviceClass(iLocalDeviceSettings.DeviceClass() & KDeviceClassBitMask);
		
		//NB - no (ELeave) here - in OOM we'd rather continue and not update the registry
		//than Leave.  All calls to methods are guarded by "if (iRegistryUpdater)"
		iRegistryUpdater = new CRegistryUpdater(iLocalDevice, iLocalDeviceSettings);
		}

	LOG(_L("LinkMgr : Creating HCIFacade (and HCI)"));
	iHCIFacade = CHCIFacade::NewL(*this); 	// Create HCI/HCIFacade first
	iPhysicalLinksMgr = CPhysicalLinksManager::NewL(*this, *iHCIFacade, *iRegSess, SecMan(), CodMan());
	iPhysicalLinksMgr->SetAcceptPairedOnlyMode(iLocalDeviceSettings.AcceptPairedOnlyMode()); // Tell the physical links manager the persisted value for the accept paired only mode
	iHCIFacade->SetPhysicalLinksMgr(*iPhysicalLinksMgr); 	// Tell HCIFacade of ConnMgr

	SecMan().SetPhysicalLinksMgr(*iPhysicalLinksMgr);
	SecMan().SetLinksMgrProtocol(*this);
	SecMan().SetHCICommandQueue(iHCIFacade->CommandQController());

	LOG(_L("LinkMgr : Initialising Link Muxer"));
	iLinkMuxer=CLinkMuxer::NewL(*this, *iHCIFacade);
	iHCIFacade->SetLinkMuxer(*iLinkMuxer);

	iACLStateFactory = CACLLinkStateFactory::NewL();
	iSCOStateFactory = CSyncLinkStateFactory::NewL();
				
	LOG(_L("LinkMgr : Initialising EirManServer"));
	iEirManServer = CEirManServer::NewL(iHCIFacade->CommandQController(), *this);

	LOG(_L("LinkMgr : Initialising InquiryMgr"));
	iInquiryMgr = CBTInquiryMgr::NewL(*this);
	iInquiryMgr->SetHCICommandQueue(iHCIFacade->CommandQController());


	LOG(_L("LinkMgr : Configuring HCIFacade)"));
	iHCIFacade->InitL(iLocalDeviceSettings);// passes ownership of device
	
	// Inquiry and page scan is read from BT Registry and set it to P&S
	// As there is no listening socket, they are set to OFF on HW yet!
	// P&S value is taken to account as soon we open the first listening socket!
	DefinePublications(iLocalDeviceSettings.ScanEnable());
	
	// set up subscribers
	CSubscriber* subscriber;
	
	subscriber = CDiscoverabilitySubscriber::NewL(*this);
	CleanupStack::PushL(subscriber);
	User::LeaveIfError(iSubscribers.Append(subscriber));
	CleanupStack::Pop(subscriber);

	subscriber = CLimitedSubscriber::NewL(*this);
	CleanupStack::PushL(subscriber);
	User::LeaveIfError(iSubscribers.Append(subscriber));
	CleanupStack::Pop(subscriber);

	subscriber = CDeviceClassSubscriber::NewL(*this);
	CleanupStack::PushL(subscriber);
	User::LeaveIfError(iSubscribers.Append(subscriber));
	CleanupStack::Pop(subscriber);
	
	subscriber = CAFHHostChannelClassificationSubscriber::NewL(*this);
	CleanupStack::PushL(subscriber);
	User::LeaveIfError(iSubscribers.Append(subscriber));
	CleanupStack::Pop(subscriber);
	
	subscriber = CDeviceNameSubscriber::NewL(*this);
	CleanupStack::PushL(subscriber);
	User::LeaveIfError(iSubscribers.Append(subscriber));
	CleanupStack::Pop(subscriber);

	subscriber = CRegistryRemoteTableSubscriber::NewL(*this);
	CleanupStack::PushL(subscriber);
	User::LeaveIfError(iSubscribers.Append(subscriber));
	CleanupStack::Pop(subscriber);
	
	subscriber = CAcceptPairedOnlySubscriber::NewL(*this);
	CleanupStack::PushL(subscriber);
	User::LeaveIfError(iSubscribers.Append(subscriber));
	CleanupStack::Pop(subscriber);
	
	subscriber = CAFHChannelAssessmentModeSubscriber::NewL(*this);
	CleanupStack::PushL(subscriber);
	User::LeaveIfError(iSubscribers.Append(subscriber));
	CleanupStack::Pop(subscriber);
	
	if(iDebugModeDefinitionResult == KErrNone)
		{
		// Only subscribe on debug mode if we have managed to successfully define
		// the key ourselves (this is because it needs to be protected with the very 
		// strong CommDD capability).
		subscriber = CDebugModeSubscriber::NewL(*this);
		CleanupStack::PushL(subscriber);
		User::LeaveIfError(iSubscribers.Append(subscriber));
		CleanupStack::Pop(subscriber);
		}

	subscriber = CPageScanParametersSubscriber::NewL(*this);
	CleanupStack::PushL(subscriber);
	User::LeaveIfError(iSubscribers.Append(subscriber));
	CleanupStack::Pop(subscriber);

	// set CoD - leave if goes wrong, user's device will be "unknown" otherwise
	// NB - In future we should tie this to the SDP Server...(or someother higher API...)
	
	if (iLocalDeviceSettings.DeviceClass() && iLocalDeviceSettings.IsValidDeviceClass())
	// Some phones never set their CoD in s/w, we don't want to blat
	// over their h/w settings
		{
		iPendingLocalDeviceSettings.SetDeviceClass(iLocalDeviceSettings.DeviceClass()); //we expect this after power on
		}

	CodMan().Initialise();	// Cod Service Manager can now get a valid CoD

	if(iLocalDeviceSettings.AFHChannelAssessmentMode() == EFalse)
	// If this feature is supported it will start off switched on
		{
		SetAFHChannelAssessmentModeL(iLocalDeviceSettings.AFHChannelAssessmentMode());
		}

	//Attempt to make sure that controller has the same value as the registry
	//and that the P&S key 'KPropertyKeyBluetoothGetLimitedDiscoverableStatus'
	//is, as a consequence, 'Set' appropriately (when the associated controller 
	//event arrives).
	DoSetLimitedDiscoverableL(iLocalDeviceSettings.LimitedDiscoverable());

	LOG(_L("LinkMgr : Initialising BR/EDR Controller Configuration"));
	iBrEdrControllerConfiguration = CBrEdrControllerConfiguration::NewL(iHCIFacade->CommandQController(), *this);
	
	LOG(_L("LinkMgr : Initialising complete"));
	}

void CLinkMgrProtocol::DefinePublications(THCIScanEnable aHCIScanEnable)
	{
	LOG_FUNC
	
	// 6 properties for setting.
	// Its possible that these are re-defined below, but we are discarding any KErrAlreadyExists.
	// As the setters require more capabilities we ensure that they get defined first.

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothSetScanningStatus,
							RProperty::EInt,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));
	
	// Even if we initialise KPropertyKeyBluetoothSetScanningStatus to Inquiry & 
	// Page scanning enabled, it's not turned on at this point. Only when first
	// listening socket is opened, this value will control the inquiry scan & page scan

	(void)(iProperty.Set(KPropertyUidBluetoothCategory,
						 KPropertyKeyBluetoothSetScanningStatus,
						 aHCIScanEnable));
						 
	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothSetLimitedDiscoverableStatus,
							RProperty::EInt,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothSetDeviceClass,
							RProperty::EInt,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothSetAFHHostChannelClassification,
							RProperty::EByteArray,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothSetAFHChannelAssessmentMode,
							RProperty::EInt,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothSetDeviceName,
							RProperty::EText,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothSetAcceptPairedOnlyMode,
							RProperty::EInt,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));
	
	iDebugModeDefinitionResult = (iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothSetSimplePairingDebugMode,
							RProperty::EInt,
							KLOCAL_SERVICES,
							KCOMMDD
							));

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothSetPageScanParameters,
							RProperty::EInt,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));

	(void) RProperty::Set(KPropertyUidBluetoothCategory,
						  KPropertyKeyBluetoothSetPageScanParameters,
						  EStandardPageScanParameters);


	// Original Get P&S value definitions.

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothGetLocalDeviceAddress,
							RProperty::EByteArray,
							KLOCAL_SERVICES,
						   	KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothGetPHYCount,
							RProperty::EInt,
							KLOCAL_SERVICES,
						   	KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothGetConnectingStatus,
							RProperty::EInt,
							KLOCAL_SERVICES,
						   	KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothGetScanningStatus,
							RProperty::EInt,
							KLOCAL_SERVICES,
						   	KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothGetLimitedDiscoverableStatus,
							RProperty::EInt,
							KLOCAL_SERVICES,
						   	KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothGetDeviceClass,
							RProperty::EInt,
							KLOCAL_SERVICES,
						   	KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothGetDeviceName,
							RProperty::EText,
							KLOCAL_SERVICES,
						   	KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));
							
	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothGetAcceptPairedOnlyMode,
							RProperty::EInt,
							KLOCAL_SERVICES,
						   	KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));
							
 	(void) (iProperty.Define(KPropertyUidBluetoothCategory,
 							KPropertyKeyBluetoothHostResolverActive,
 							RProperty::EInt,
 							KLOCAL_SERVICES,
 						   	KLOCAL_SERVICES_AND_NETWORK_CONTROL
 							));
 	
	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothGetSimplePairingDebugMode,
							RProperty::EInt,
							KLOCAL_SERVICES,
						   	KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));

	(void)(iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothGetPageScanParameters,
							RProperty::EInt,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL,
							KLOCAL_SERVICES_AND_NETWORK_CONTROL
							));
							
	(void) RProperty::Set(KPropertyUidBluetoothCategory,
						  KPropertyKeyBluetoothGetPageScanParameters,
						  EStandardPageScanParameters);
	}

void CLinkMgrProtocol::DeletePublications()
	{
	LOG_FUNC
	//we own them, so they'll get deleted
	iProperty.Close();
	}

void CLinkMgrProtocol::StartL()
/**
	Binding complete.
	Startup call from esock.  Do nothing
	If HCI needs any start-up, do it here.
**/
	{
	LOG_FUNC
	}

// From higher protocol
void CLinkMgrProtocol::BindL(CProtocolBase* protocol, TUint /* id*/)
	{
	LOG_FUNC
	TServerProtocolDesc prtDesc;
	protocol->Identify(&prtDesc);

#ifdef TCI
	if(prtDesc.iAddrFamily!=KBTAddrFamily || prtDesc.iProtocol!=KTCIL2CAP)
#else
	if(prtDesc.iAddrFamily!=KBTAddrFamily || prtDesc.iProtocol!=KL2CAP)
#endif
		{
		User::Leave(KErrBtEskError);
		}
	// Store this for error propogation
	//
	iUpperProtocol = protocol;
	}

// Factory functions

CHostResolvProvdBase* CLinkMgrProtocol::NewHostResolverL()
	{
	LOG_FUNC
	return iInquiryMgr->NewHostResolverL();
	}

// Query functions

void CLinkMgrProtocol::Identify(TServerProtocolDesc* aDesc) const
//
// Identify request from SOCKET server
//
	{
	LOG_FUNC
	CLinkMgrProtocol::ProtocolIdentity(aDesc);
	}

void CLinkMgrProtocol::ProtocolIdentity(TServerProtocolDesc* aDesc)
	{
	LOG_STATIC_FUNC
	_LIT(name,"BTLinkManager");
	aDesc->iProtocol=KBTLinkManager;

	aDesc->iName=name;
	aDesc->iAddrFamily=KBTAddrFamily;
	aDesc->iSockType=KUndefinedSockType;	// various!
	
	aDesc->iVersion=TVersion(KBTMajor,KBTMinor,KBTBuild);
	aDesc->iByteOrder=ELittleEndian;

/*
	The following sockets are exposed as RSockets

	'Proxy' = a SAP attached to a baseband connection that can influence it
	'SCO'	= a SAP providing SCO services

	Higher protocols can additionally ask for ACL SAPs, but these are not 
	available as RSockets

	This guides the direction of the ServiceInfo below

*/

	aDesc->iServiceInfo=KSIDatagram|KSIGracefulClose|KSIBroadcast|KSIQOS|KSICanReconnect|KSIConnectData|KSIDisconnectData; // slightly hijacked connectData
	aDesc->iNamingServices=KNSNameResolution;
	aDesc->iSecurity=KSocketNoSecurity;
	aDesc->iMessageSize=Max(CHctlSynchronousDataFrame::KHCTLMaxSynchronousDataSize, CHctlAclDataFrame::KHCTLMaxACLDataSize);
	aDesc->iServiceTypeInfo=ESocketSupport|ECantProcessMBufChains|ENeedMBufs|ETransport
							|EPreferDescriptors|EUseCanSend;
	aDesc->iNumSockets=100;	// one per baseband - including many parked!
	}

void CLinkMgrProtocol::Error(TInt aErr, CProtocolBase* /*aProt*/)
	{
	LOG_FUNC
	// A fatal error has occurred (Power off hopefully!). Notify the upper protocol
	// (of there is one) and the inquiry manager. 
	__ASSERT_DEBUG(aErr==KErrHardwareNotAvailable, Panic(EUnknownFatalError));
	iInquiryMgr->CompleteCommands(aErr);
	iPhysicalLinksMgr->FatalError(aErr);
	}

void CLinkMgrProtocol::CloseNow()
/**
	Close command from base class.
	Called when ref count reaches 0.
	We don't actually have to close now, but when we finally
	do, we must call CanClose (done within TryToClose). That'll
	actually delete us. In the mean time, if a new client tries
	to connect to LinkMgr, ESOCK will use this existing one and just
	call Open on it.
	What we do here, is queue an idle timer entry. It's expiry will
	call TryToClose, which'll actually do the close.
**/
	{
	LOG_FUNC
	// synchronously update stack state in Registry
	iLocalDevice.Modify(iLocalDeviceSettings); //ignore error		
	
	iClosePending = ETrue;
	QueIdleTimerEntry();
	}

void CLinkMgrProtocol::Open()
/**
	Open LinkMgr protocol.
	Called everytime a new client (of LinkMgr) wants to use it.
**/
	{
	LOG_FUNC
	LocalOpen();
	if(iExternalRef++ == 0)
		{
		ExternalOpenInit();
		}
	}

void CLinkMgrProtocol::Close()
	{
	LOG_FUNC
	if(--iExternalRef == 0)
		{
		ExternalCloseCleanup();
		}
	LocalClose();
	}

void CLinkMgrProtocol::LocalOpen()
	{
	LOG_FUNC
	iClosePending = EFalse;
	RemoveIdleTimerEntry();
	CProtocolBase::Open();
	}

void CLinkMgrProtocol::LocalClose()
	{
	LOG_FUNC
	CProtocolBase::Close();
	}

void CLinkMgrProtocol::ExternalOpenInit()
	{
	LOG_FUNC
	TRAP_IGNORE(iLocalNamePublisher = CEirPublisherLocalName::NewL(*iEirManServer));
	TRAP_IGNORE(iTxPowerLevelPublisher = CEirPublisherTxPowerLevel::NewL(*iEirManServer));
	if(iLocalNamePublisher)
		{
		// Publish the initial name.
		iLocalNamePublisher->UpdateName(iLocalDeviceSettings.DeviceName());
		}
	}

void CLinkMgrProtocol::ExternalCloseCleanup()
	{
	LOG_FUNC
	// Currently just the internal publishers
	delete iLocalNamePublisher;
	iLocalNamePublisher = NULL;
	delete iTxPowerLevelPublisher;
	iTxPowerLevelPublisher = NULL;
	}

void CLinkMgrProtocol::QueIdleTimerEntry()
/**
	Queue idle timer entry.
	When this timer expires, it'll call TryToClose, which actually
	causes the thing to finally close down.
**/
	{
	LOG_FUNC
	RemoveIdleTimerEntry();
	iIdleEntryQueued = ETrue;
	BTSocketTimer::Queue(KLinkMgrProtocolIdleTimer, iIdleTimerEntry);
	}

void CLinkMgrProtocol::RemoveIdleTimerEntry()
/**
	Called whenever we're opened.
	Checks there are no idle timer entries queued.
**/
	{
	LOG_FUNC
	if (!iIdleEntryQueued)
		return;
	BTSocketTimer::Remove(iIdleTimerEntry);
	iIdleEntryQueued = EFalse;
	}

 /*static*/ TInt CLinkMgrProtocol::TryToClose(TAny* aProtocol)
/**
	Actually try to close the protocol.
	Called after the idle timeout period by the BTSocketTimer. If
	we're all set to close down, the thing is closed.
**/
	{
	LOG_STATIC_FUNC
	CLinkMgrProtocol* p=static_cast<CLinkMgrProtocol*>(aProtocol);
	p->iIdleEntryQueued = EFalse;
	if (p->iClosePending)
		{
		p->CanClose();	// deletes this
		}
	return EFalse;
	}

CServProviderBase* CLinkMgrProtocol::NewSAPL(TUint aSockType)
/** 
	Create a new SAP.
	The SAP returned is owned by the caller -- this protocol will not clean it up.
	esock uses this function to create a new SAP, and esock will delete when it
	is finished with it.
**/
	{
	LOG_FUNC

	CServProviderBase* sap = NULL;
	
	switch (aSockType)
		{
		case KSockBluetoothTypeESCO:
			{
			sap = CeSCOLink::NewL(*iPhysicalLinksMgr, NULL);
			break;
			}

		case KSockBluetoothTypeSCO:
			{
			sap = CSCOLink::NewL(*iPhysicalLinksMgr, NULL);
			break;
			}

		case KSockBluetoothTypeACL:
			{
			// an explicit ACL link has been asked for (by L2CAP)
			sap = CACLLink::NewL(*iPhysicalLinksMgr, NULL, *this); // don't know for which PHY at this stage
			break;
			}

		case KSockBluetoothTypeVendorSpecific:
			{
			if (!iVendorSpecificSAP)
				{
				sap = CVendorSAP::NewL(*this);
				}
			else
				{
				User::Leave(KErrInUse); // only support one :o)
				}
			break;
			}

		case KSockBluetoothTypeRawBroadcast:
		default:
			{
			__ASSERT_DEBUG((aSockType > KHCIMinimumHandle && aSockType < KHCIMaximumHandle)||(aSockType==KSockBluetoothTypeRawBroadcast), Panic(EBTProxySAPInvalidCreate));
			// if the connection handle has been specified - find the connection: aSockType *is* a ConnectionHandle in this case
			CPhysicalLink* phySAP = NULL;

			if (aSockType!=KSockBluetoothTypeRawBroadcast)
				{
				// it's a proxy sap which we should try to bind to the PHY
				// 
				phySAP = iPhysicalLinksMgr->FindPhysicalLink(static_cast<THCIConnHandle>(aSockType));
				}
			// create Proxy telling it the possible PHY
			CBTProxySAP* proxySAP = CBTProxySAP::NewLC(*iPhysicalLinksMgr, phySAP);
			CleanupStack::Pop(proxySAP);
			sap = proxySAP;
			}
		}
	ASSERT_DEBUG(sap);
	return sap;
	}

TBTDevHCIVersion CLinkMgrProtocol::GetHWHCIVersion() const
	{
	LOG_FUNC
	return iHWHCIVersion;
	}

void CLinkMgrProtocol::SetLocalVersion(THCIErrorCode aErr, TBTDevHCIVersion aHCIVersion, TBTDevLMPVersion aLMPVersion)
	{
	LOG_FUNC
	// we store this for possibly useful later use :-)
	if (aErr == EOK)
		{
		iHWHCIVersion = aHCIVersion; // not especially useful for us: but maybe to exploit >BT1.1
		iHWLMPVersion = aLMPVersion; // the LM in the HC ;-)
		}
	}

TInt CLinkMgrProtocol::StartProtocolListening()
	{
	LOG_FUNC
	return KickDiscoverabilitySubscriber();
	}

// oh Dear! void again, how to report failure...
void CLinkMgrProtocol::StopProtocolListening()
	{
	LOG_FUNC

	// NB. If 'KickDiscoverabilitySubscriber' returns an error, then
	// we've tried to get the value for ScanStatus, but we've failed.
	// Not a lot we can do about it.  This means that we may be 
	// leaving inquiry and page scan enabled with no listeners.
	(void)KickDiscoverabilitySubscriber();
	}

TInt CLinkMgrProtocol::KickDiscoverabilitySubscriber()
	{
	/*
	 Reset P&S with its current value. This will kick
	 CDiscoverabilitySubscriber's RunL which calls 
	 SetInquiryAndPageScanningL to check for the current 
	 user wishes AND the existance of listeners before 
	 sending the appropriate WriteScanEnable command to 
	 the controller.
	*/
	LOG_FUNC

	TInt scanVal;
	TInt err = iProperty.Get(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetScanningStatus, scanVal);
	
	if (err != KErrNone)
		{
		return err;
		}

	THCIScanEnable scan = static_cast<THCIScanEnable>(scanVal);

	// force CDiscoverabilitySubscriber to try to enable Page and Inquiry scan to HW, when set
	(void)(iProperty.Set(KPropertyUidBluetoothCategory,
						 KPropertyKeyBluetoothSetScanningStatus,
						 scan));

	return err;
	}
	
void CLinkMgrProtocol::LocalSupportedFeaturesAvailable()
	{
	// Set options based on the features
	iInquiryMgr->SetInquiryMode();
	iEirManServer->NotifyFeaturesReady();
	}
	
void CLinkMgrProtocol::SetLocalFeatures(THCIErrorCode aErr, const TBTFeatures& aMask)
	{
	LOG_FUNC
	if (aErr == EOK)
		{
		// we store this for possibly useful later use :-)
		iHWFeatures = aMask;
		}
	// else ignore for now...
	iEirManServer->NotifyFeaturesReady();
	}

void CLinkMgrProtocol::SetLocalCommands(THCIErrorCode aErr, const TBluetoothHciCommands& aMask)
	{
	LOG_FUNC
	if (aErr == EOK)
		{
		// we store this for possibly useful later use :-)
		iHWCommands = aMask;
		
		}
	// else ignore for now...
	}

void CLinkMgrProtocol::SetInquiryAndPageScanningL()
	{
	LOG_FUNC
	
	TInt requestedScanVal = 0; // no Inquiry or Page Scanning as default
	
	TInt err = iProperty.Get(KPropertyUidBluetoothCategory,
						KPropertyKeyBluetoothSetScanningStatus,
						requestedScanVal
						);

	THCIScanEnable requestedScanSetting;						
	if (!err) 
		{
		requestedScanSetting = static_cast<THCIScanEnable>(requestedScanVal);
		}
	else
		{
		
		// If err == KErrNotFound, we have not published this property yet, but in all error cases
		// we will use the current device setting to define the scanning requirements instead.
		requestedScanSetting = iLocalDeviceSettings.ScanEnable();
		}

	// we need to check to see if we have any listeners before we can enable page & inquiry scan
	THCIScanEnable validScanSetting;
	if(!IsListening() || (requestedScanSetting == EInquiryScanOnly))
		{
		validScanSetting = ENoScansEnabled;
		}
	else 
		{
		validScanSetting = requestedScanSetting;
		}
	
	iPendingLocalDeviceSettings.SetScanEnable(validScanSetting);
	
	iHCIFacade->WriteScanEnableL(iPendingLocalDeviceSettings.ScanEnable());
		
	if (!err) 
		{
		// Update registry with the requested settings from the _set_ key
		iLocalDeviceSettings.SetScanEnable(static_cast<THCIScanEnable>(requestedScanVal));			
		UpdateSettings();
		}		
	}

void CLinkMgrProtocol::SetAcceptPairedOnlyMode(TBool aOn)
/*
THIS WORKS DIFFERENTLY: It does not involve the controller - it is
purely a stack setting. It is called directly when the user requests
the stack value to be reset.
*/
	{
	LOG_FUNC
	iLocalDeviceSettings.SetAcceptPairedOnlyMode(aOn);
	
	//update UI
	(void)iProperty.Set(KPropertyUidBluetoothCategory,
						KPropertyKeyBluetoothGetAcceptPairedOnlyMode,
						iLocalDeviceSettings.AcceptPairedOnlyMode()
						);

	UpdateSettings();
	}

void CLinkMgrProtocol::SetSimplePairingDebugMode(TBool aOn)
/*
THIS WORKS DIFFERENTLY: It does not involve the controller - it is
purely a stack setting.
*/
	{
	LOG_FUNC
	//update UI
	(void)iProperty.Set(KPropertyUidBluetoothCategory,
						KPropertyKeyBluetoothGetSimplePairingDebugMode,
						aOn
						);
	}

void CLinkMgrProtocol::SetDeviceClassL(TUint32 aCoD)
	{
	LOG_FUNC
	// This method is called from Subscribers when the P&S is used to write a new CoD
	TInt err = KErrNone;
	err = CodMan().PandSCodHandler(aCoD);	// Cod Service Manager will decide what gets written
		 
	User::LeaveIfError(err);	// P&S CoD failed to write, but at least Codman has a record of it
	}

void CLinkMgrProtocol::WriteClassOfDeviceL(TUint32 aCoD)
	{
	LOG_FUNC
	// Only write the CoD bits if they have changed from the existing setting
	if (iPendingLocalDeviceSettings.DeviceClass() != aCoD)
		{
		TRAPD(err, iHCIFacade->WriteDeviceClassL(aCoD));
		if (err == KErrNone)
			{
			iPendingLocalDeviceSettings.SetDeviceClass(aCoD);
			}
		else
			{
			/* If the function errors, it is probably becuase the power is off, so we'll save the setting for power on
			   If the error is for a different reason, setting this doesn't do any harm anyway - it will get picked
			   up the next time the power is turned on */
			iDesiredLocalDeviceSettings.SetDeviceClass(aCoD);
			User::Leave(err);
			}
		}
	}

TInt CLinkMgrProtocol::SetLocalDeviceName(const TDesC8& aName)
	{
	LOG_FUNC
	// We trap this HCI command and not the others in Set... to keep the 
	// error path up to CBTHostResolver::SetHostName
	TRAPD(err, iHCIFacade->WriteLocalNameL(aName));
	if (err == KErrNone)
		{
		iPendingLocalDeviceSettings.SetDeviceName(aName);
		}
	else
		{
		/* If the function leaves, it is probably becuase the power is off, so we'll save the setting for power on
		   If the leave is for a different reason, setting this doesn't do any harm anyway - it will get picked
		   up the next time the power is turned on */
		iDesiredLocalDeviceSettings.SetDeviceName(aName);
		}
	return err;
	}

void CLinkMgrProtocol::SetAFHHostChannelClassificationL(const TBTAFHHostChannelClassification& aChannelClassification)
	{
	LOG_FUNC
	iHCIFacade->SetAFHHostChannelClassificationL(aChannelClassification);
	}
	
void CLinkMgrProtocol::SetAFHChannelAssessmentModeL(TBool aMode)
	{
	LOG_FUNC
	TRAPD(err, iHCIFacade->WriteAFHChannelAssessmentModeL(aMode));
	if (err == KErrNone)
		{
		iPendingLocalDeviceSettings.SetAFHChannelAssessmentMode(aMode);
		}
	else
		{
		/* If the function leaves, it is probably becuase the power is off, so we'll save the setting for power on
		   If the leave is for a different reason, setting this doesn't do any harm anyway - it will get picked
		   up the next time the power is turned on */
		iDesiredLocalDeviceSettings.SetAFHChannelAssessmentMode(aMode);
		User::Leave(err);
		}
	}

void CLinkMgrProtocol::SetLimitedDiscoverableIfChangedL(TBool aOn)
	{
	LOG_FUNC
	// Set limited discoverable if value differs from cached (= registry) value
	// and previous change going the opposite way is not pending completion
	// See also DoSetLimitedDiscoverableL
	if ((aOn == iLocalDeviceSettings.LimitedDiscoverable()) &&
		(iPendingLocalDeviceSettings.LimitedDiscoverable() == iLocalDeviceSettings.LimitedDiscoverable()))
		return;

	DoSetLimitedDiscoverableL(aOn);
	}

void CLinkMgrProtocol::DoSetLimitedDiscoverableL(TBool aOn)
	{
	LOG_FUNC
	// HCI spec says we can always send >1 IAC! Controllers will take first
	// so we can do what GAP says easily

	TUint8 numIACs =0;
	TUint iacs[2];

	if (aOn)
		{
		// turn on LIAC
		TRAPD(err, WriteClassOfDeviceL(iPendingLocalDeviceSettings.DeviceClass() | (EMajorServiceLimitedDiscoverableMode << 
				(KLengthOfDeviceClass+KStartingOffsetOfDeviceClass))));
		if (err == KErrNone)
			{
			iPendingLocalDeviceSettings.SetLimitedDiscoverable(ETrue);
			}
		else
			{
			/* If the function leaves, it is probably becuase the power is off, so we'll save the setting for power on
			   If the leave is for a different reason, setting this doesn't do any harm anyway - it will get picked
			   up the next time the power is turned on */
			iDesiredLocalDeviceSettings.SetLimitedDiscoverable(ETrue);
			User::Leave(err);
			}
		numIACs = 2;
		iacs[0] = KLIAC;
		iacs[1] = KGIAC;
		}
	else
		{
		// turn off LIAC - could do the 1 minute GAP timer?
		TRAPD(err, WriteClassOfDeviceL(iPendingLocalDeviceSettings.DeviceClass() & ~(EMajorServiceLimitedDiscoverableMode << 
				(KLengthOfDeviceClass+KStartingOffsetOfDeviceClass))));
		if (err == KErrNone)
			{
			iPendingLocalDeviceSettings.SetLimitedDiscoverable(EFalse);
			}
		else
			{
			/* If the function leaves, it is probably becuase the power is off, so we'll save the setting for power on
			   If the leave is for a different reason, setting this doesn't do any harm anyway - it will get picked
			   up the next time the power is turned on */
			iDesiredLocalDeviceSettings.SetLimitedDiscoverable(EFalse);
			User::Leave(err);
			}
		numIACs = 1;
		iacs[0] = KGIAC;
		}
	
	iHCIFacade->WriteIACLAPL(numIACs, iacs);
	}

void CLinkMgrProtocol::UpdateLocalDevicePower(TBTPowerState aState)
	{
	LOG_FUNC
	iLocalDeviceSettings.SetPowerSetting(static_cast<TUint8>(aState));
	UpdateSettings();
	}
	
// This is an update method for dealing with the result of asking what the device
// name is. It is used to keep iLocalDeviceSettings.DeviceName(), the P&S device 
// name value and the h/w value in sync
void CLinkMgrProtocol::UpdateLocalDeviceName(const TDesC8& aName)
   	{
	LOG_FUNC
  	iLocalDeviceSettings.SetDeviceName(aName);

	// Update the UI with the unicode DeviceName
	TBuf16<KHCILocalDeviceNameMaxLength> unicodeName;
	(void) CnvUtfConverter::ConvertToUnicodeFromUtf8(unicodeName, iLocalDeviceSettings.DeviceName());
	// The return from the above should never be > 0 since aName should have a length <= KHCILocalDeviceNameMaxLength
	// hence we shouldn't have any unconverted chars left over. If we get another error then we just have to publish  
	// the resulting unicode name anyway.
		
	(void)iProperty.Set(KPropertyUidBluetoothCategory,
				    	KPropertyKeyBluetoothGetDeviceName,
				    	unicodeName
						);
  	UpdateSettings();					
   	}
   	
// This is an update method for dealing with the result of telling the h/w what the device
// name should be. It is used to keep iLocalDeviceSettings.DeviceName(), the P&S device 
// name value and the h/w value in sync
void CLinkMgrProtocol::UpdateLocalDeviceName(TBool aSucceeded)
	{
	LOG_FUNC
	if (aSucceeded)
		{
		TBool isDifferentName = 
				(iLocalDeviceSettings.DeviceName() != iPendingLocalDeviceSettings.DeviceName());
		iLocalDeviceSettings.SetDeviceName(iPendingLocalDeviceSettings.DeviceName());
		if(iLocalNamePublisher && isDifferentName)
			{
			iLocalNamePublisher->UpdateName(iLocalDeviceSettings.DeviceName());
			}
		}
		
	// Update the UI with the unicode DeviceName
	TBuf16<KHCILocalDeviceNameMaxLength> unicodeName;
	(void) CnvUtfConverter::ConvertToUnicodeFromUtf8(unicodeName, iLocalDeviceSettings.DeviceName());
	// The return from the above should never be > 0 since unicodeName is the same size as utf8Name 
	// hence we shouldn't have any unconverted chars left over. Similarly ret should not be < 0 
	// because when we received DeviceName in Unicode we where able to convert it to UTF-8
	// without a problem so reversing the conversion should be okay.
	
	(void)iProperty.Set(KPropertyUidBluetoothCategory,
				    	KPropertyKeyBluetoothGetDeviceName,
				    	unicodeName
						);
	UpdateSettings();
	}

void CLinkMgrProtocol::UpdateLocalDeviceScanEnable(TBool aSucceeded)
	{
	LOG_FUNC
	TInt scan;
	TInt err = KErrNone;
	
	// Set scan to be the actual scan enable state of the hardware
	if(aSucceeded)
		{
		// We've set it to the value in iLocalDeviceSettings
		scan = iPendingLocalDeviceSettings.ScanEnable();
		}
	else
		{
		// We haven't changed it, current value is in the _get_ key
		err = iProperty.Get(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothGetScanningStatus,
							scan);
		}
	
	if(!err)
		{	
		//update UI with the hardware's settings using the _get_ key
		(void)iProperty.Set(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothGetScanningStatus,
							scan
							);	
		}					
	}

void CLinkMgrProtocol::UpdateLimitedDiscoverable(TBool aSucceeded)
	{
	LOG_FUNC
	if (aSucceeded)
		{
		iLocalDeviceSettings.SetLimitedDiscoverable(
			iPendingLocalDeviceSettings.LimitedDiscoverable());
		}
	
	//update UI
	(void)iProperty.Set(KPropertyUidBluetoothCategory,
						KPropertyKeyBluetoothGetLimitedDiscoverableStatus,
						iLocalDeviceSettings.LimitedDiscoverable()
						);

	UpdateSettings();
	}

void CLinkMgrProtocol::UpdateDeviceClass(TBool aSucceeded)
	{
	LOG_FUNC
	if (aSucceeded)
		{
		iLocalDeviceSettings.SetDeviceClass(
			iPendingLocalDeviceSettings.DeviceClass());
		}
	
	//update UI
	(void)iProperty.Set(KPropertyUidBluetoothCategory,
						KPropertyKeyBluetoothGetDeviceClass,
						iLocalDeviceSettings.DeviceClass()
						);

	UpdateSettings();
	}
	
void CLinkMgrProtocol::UpdateDeviceClass(TBool aSucceeded, TUint aCoD)
	{
	LOG_FUNC
	if (aSucceeded)
		{
		iLocalDeviceSettings.SetDeviceClass(aCoD);
		
		//update UI
		(void)iProperty.Set(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothGetDeviceClass,
							aCoD
							);
							
		UpdateSettings();							
		}
	}

void CLinkMgrProtocol::UpdateAFHChannelAssessmentMode(TBool aSucceeded)
	{
	LOG_FUNC
	if (aSucceeded)
		{
		iLocalDeviceSettings.SetAFHChannelAssessmentMode(
			iPendingLocalDeviceSettings.AFHChannelAssessmentMode());
		}

	UpdateSettings();
	}

void CLinkMgrProtocol::UpdateSettings()
	{
	LOG_FUNC
	//Careful! - we new'd it without ELeave
	if (iRegistryUpdater)
		{
		iRegistryUpdater->Update();
		}
	}

void CLinkMgrProtocol::UpdateInquiryResponseTxPowerLevel(TInt8 aTxPowerLevel)
	{
	LOG_FUNC
	if(iTxPowerLevelPublisher)
		{
		iTxPowerLevelPublisher->UpdateTxPowerLevel(aTxPowerLevel);
		}
	}


void CLinkMgrProtocol::SetUIConnecting(TBool aConnecting)
	{
	LOG_FUNC
	(void)iProperty.Set(KPropertyUidBluetoothCategory,
						KPropertyKeyBluetoothGetConnectingStatus,
						aConnecting);
	}


void CLinkMgrProtocol::SetUINumPhysicalLinks(TUint aNum)
	{
	LOG_FUNC
	(void)iProperty.Set(KPropertyUidBluetoothCategory,
						KPropertyKeyBluetoothGetPHYCount,
						aNum);
	}


void CLinkMgrProtocol::SetLocalBTAddress(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	iLocalDeviceAddress = aAddr;

	// Also update registry with local device address
	iLocalDeviceSettings.SetAddress(iLocalDeviceAddress);
	UpdateSettings();

	// publish this number - might be useful
	const TDesC8& des = aAddr.Des();
	(void)iProperty.Set(KPropertyUidBluetoothCategory,
						KPropertyKeyBluetoothGetLocalDeviceAddress,
						des);
	}

void CLinkMgrProtocol::SetUIDiscovering(TInt aDiscoveringState)
 	{
	LOG_FUNC
 	(void)iProperty.Set(KPropertyUidBluetoothCategory,
 						KPropertyKeyBluetoothHostResolverActive,
 						aDiscoveringState);
 	}
 
TInt CLinkMgrProtocol::ACLPacketMTU() const
	{
	LOG_FUNC
	return iLinkMuxer->ACLPacketMTU();
	}

TInt CLinkMgrProtocol::ControlPlaneMessage(TBTControlPlaneMessage aMessage, TAny* aParam)
	{
	LOG_FUNC
	TInt rerr = KErrNotFound;
	CPhysicalLink* con = NULL;
	
	if ((aMessage == ESubscribePhysicalLink) || (aMessage == EUnsubscribePhysicalLink)) 
		{
		TPhysicalLinkSubscriptionInfo& subscriptionInfo = *reinterpret_cast<TPhysicalLinkSubscriptionInfo*>(aParam);
		con = iPhysicalLinksMgr->FindPhysicalLink(subscriptionInfo.iAddr);
		}
	else if ((aMessage == ETryToAndThenPreventHostEncryptionKeyRefresh))
		{
		// For EPreventAutoEncryptionKeyRefresh we pass the address as a TBTDevAddr**
		const TBTDevAddr& addr = **reinterpret_cast<const TBTDevAddr**>(aParam);
		con = iPhysicalLinksMgr->FindPhysicalLink(addr);
		}
	else
		{
		const TBTDevAddr& addr = *reinterpret_cast<const TBTDevAddr*>(aParam);
		con = iPhysicalLinksMgr->FindPhysicalLink(addr);
		}

	if (con)
		{
		switch (aMessage)
			{
			case EOverridePark:
				rerr = con->OverridePark();
				break;
			case EUndoOverridePark:
				rerr = con->UndoOverridePark();
				break;
			case EOverrideLPM:
				rerr = con->OverrideLPM();
				break;
			case EUndoOverrideLPM:
				rerr = con->UndoOverrideLPM();
				break;
			case ESubscribePhysicalLink:
				{
				const TPhysicalLinkSubscriptionInfo& subscriptionInfo = *reinterpret_cast<TPhysicalLinkSubscriptionInfo*>(aParam);
				con->SubscribeLinkObserver(subscriptionInfo.iMPhysicalLinkObserver);
				break;
				}
			case EUnsubscribePhysicalLink:
				{
				const TPhysicalLinkSubscriptionInfo& subscriptionInfo = *reinterpret_cast<TPhysicalLinkSubscriptionInfo*>(aParam);				
				con->UnsubscribeLinkObserver(subscriptionInfo.iMPhysicalLinkObserver);
				break;
				}
			case ETryToAndThenPreventHostEncryptionKeyRefresh:
				{
				rerr = con->TryToAndThenPreventHostEncryptionKeyRefresh(aParam);
				break;
				}
			default:
				rerr = KErrNotSupported;
				break;
			};
		}
	return rerr;
	}
	
	
void CLinkMgrProtocol::ClearPendingLocalDeviceSettingsCod()
	{
	LOG_FUNC
	// If we know that the h/w has been reset, then we need to ensure that our persistent
	// value reflects this. That way we will force the CoD re-write to the hardware.
	iPendingLocalDeviceSettings.SetDeviceClass(KDeviceClassReset);
	}

void CLinkMgrProtocol::SetPageScanParameters(TPageScanParameterSettings aPageScanParameters)
	{
	LOG_FUNC
	iBrEdrControllerConfiguration->SetPageScanParameters(aPageScanParameters);
	}


//
// CRegistrySession
//

CRegistrySession* CRegistrySession::NewL(CLinkMgrProtocol& aLinkMgrProtocol)
	{
	LOG_STATIC_FUNC
	CRegistrySession* self = new(ELeave) CRegistrySession(aLinkMgrProtocol);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CRegistrySession::Open()
	{
	LOG_FUNC
	if(iAccessCount++ == 0)
		{
		iLinkMgrProtocol.LocalOpen();
		}
	}

void CRegistrySession::Close()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iAccessCount > 0, Panic(ERegistrySessionClosedTooManyTimes));
	if(--iAccessCount == 0)
		{
		iLinkMgrProtocol.LocalClose();
		}
	}
	
CRegistrySession::CRegistrySession(CLinkMgrProtocol& aLinkMgrProtocol)
	: CBase()
	, iAccessCount(0)
	, iLinkMgrProtocol(aLinkMgrProtocol)
	{
	LOG_FUNC
	}

void CRegistrySession::ConstructL()
	{
	LOG_FUNC
	User::LeaveIfError(iRegServ.Connect());
	}

CRegistrySession::~CRegistrySession()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iAccessCount == 0, Panic(ERegistrySessionDeletedWhenOpen));
	iRegServ.Close();
	}

RBTRegServ& CRegistrySession::RegServ()
	{
	LOG_FUNC
	return iRegServ;
	}


//class CRegistryUpdater
CRegistryUpdater::~CRegistryUpdater()
	{
	LOG_FUNC
	Cancel();
	}
	

CRegistryUpdater::CRegistryUpdater(RBTLocalDevice& aLocalDevice,TBTTrackedLocalDevice& aSettings) :
CActive(CActive::EPriorityStandard),iLocalDevice(aLocalDevice),iStackSettings(aSettings),iRepeatUpdate(EFalse)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

void CRegistryUpdater::Update()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iLocalDevice.SubSessionHandle(), Panic(ELocalDeviceBadHandle));
	
	if (!IsActive())
		{
		TInt err;
		err = iLocalDevice.Get(iRegistrySettings); 
		
		// if we can't get settings from registry, we won't be able to update them
		if ( err == KErrNone )
			{
			iStackSettings.Modify(iRegistrySettings);
			iStackSettings.ResetChangesMask();
			iLocalDevice.Modify(iRegistrySettings,iStatus);
			SetActive();
			iRepeatUpdate = EFalse;	
			}
		}
	else
		{
		// Even if multiple requests come in while this object is active we only need to
		// perform one update since iLocalDevice will accumulated required new settings.
		LOG(_L(" -- CRegistryUpdaterAO already active so will repeat the update later."));
		iRepeatUpdate = ETrue;
		}
	}

void CRegistryUpdater::RunL()
	{
	LOG_FUNC
	if (iRepeatUpdate)
		{
		Update();
		}		
	}
	
void CRegistryUpdater::DoCancel()
	{
	LOG_FUNC
	iLocalDevice.CancelRequest(iStatus);
	iRepeatUpdate = EFalse;	
	}
	

//class TBTTrackedLocalDevice
void TBTTrackedLocalDevice::SetAddress(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	if (aAddr != Address())
		{
		TBTLocalDevice::SetAddress(aAddr);
		StoreChange(EAddress);
		}
	}
	
void TBTTrackedLocalDevice::SetDeviceClass(TUint32 aCod)
	{
	LOG_FUNC
	if (aCod != DeviceClass())
		{
		TBTLocalDevice::SetDeviceClass(aCod);
		StoreChange(ECoD);
		}
	}

void TBTTrackedLocalDevice::SetDeviceName(const TDesC8& aName)
	{
	LOG_FUNC
	if (aName != DeviceName())
		{
		TBTLocalDevice::SetDeviceName(aName);
		StoreChange(EDeviceName);
		}
	}

void TBTTrackedLocalDevice::SetScanEnable(THCIScanEnable aEnable)
	{
	LOG_FUNC
	if (aEnable != ScanEnable())
		{
		TBTLocalDevice::SetScanEnable(aEnable);
		StoreChange(EScanEnable);
		}
	}

void TBTTrackedLocalDevice::SetLimitedDiscoverable(TBool aOn)
	{
	LOG_FUNC
	if (aOn != LimitedDiscoverable())
		{
		TBTLocalDevice::SetLimitedDiscoverable(aOn);
		StoreChange(ELimitedDiscoverable);
		}
	}

void TBTTrackedLocalDevice::SetPowerSetting(TUint8 aPowerSetting)
	{
	LOG_FUNC
	if (aPowerSetting != PowerSetting())
		{
		TBTLocalDevice::SetPowerSetting(aPowerSetting);
		StoreChange(EPowerSetting);
		}
	}

void TBTTrackedLocalDevice::SetAFHChannelAssessmentMode(TBool aOn)
	{
	LOG_FUNC
	if (aOn != AFHChannelAssessmentMode())
		{
		TBTLocalDevice::SetAFHChannelAssessmentMode(aOn);
		StoreChange(EAFHChannelAssessmentMode);
		}
	}

void TBTTrackedLocalDevice::SetAcceptPairedOnlyMode(TBool aOn)
	{
	LOG_FUNC
	if (aOn != AcceptPairedOnlyMode())
		{
		TBTLocalDevice::SetAcceptPairedOnlyMode(aOn);
		StoreChange(EAcceptPairedOnlyMode);
		}
	}

void TBTTrackedLocalDevice::StoreChange(TUint8 aChange)
	{
	LOG_FUNC
	iChangedSettings |= aChange;
	}
	
void TBTTrackedLocalDevice::ResetChangesMask()
	{
	LOG_FUNC
	iChangedSettings = 0;
	}
	
void TBTTrackedLocalDevice::Modify(TBTLocalDevice& aLocalDeviceSettings)
	{
	LOG_FUNC
	if ( iChangedSettings & EAddress )
		{
		aLocalDeviceSettings.SetAddress(Address());
		}

	if ( iChangedSettings & ECoD )
		{
		aLocalDeviceSettings.SetDeviceClass(DeviceClass());
		}

	if ( iChangedSettings & EDeviceName )
		{
		aLocalDeviceSettings.SetDeviceName(DeviceName());
		}

	if ( iChangedSettings & EPowerSetting )
		{
		aLocalDeviceSettings.SetPowerSetting(PowerSetting());
		}

	if ( iChangedSettings & EScanEnable )
		{
		aLocalDeviceSettings.SetScanEnable(ScanEnable());
		}

	if ( iChangedSettings & ELimitedDiscoverable )
		{
		aLocalDeviceSettings.SetLimitedDiscoverable(LimitedDiscoverable());
		}

	if ( iChangedSettings & EAFHChannelAssessmentMode )
		{
		aLocalDeviceSettings.SetAFHChannelAssessmentMode(AFHChannelAssessmentMode());
		}

	if ( iChangedSettings & EAcceptPairedOnlyMode )
		{
		aLocalDeviceSettings.SetAcceptPairedOnlyMode(AcceptPairedOnlyMode());
		}
	}

