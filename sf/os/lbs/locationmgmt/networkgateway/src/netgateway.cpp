// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Client API to the LBS Network Simulator for 
// the (test) LBS protocol module.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include <e32base.h>
#include <e32debug.h>
#include <ecom/ecom.h>
#ifdef SYMBIAN_FEATURE_MANAGER
	#include <featdiscovery.h>
	#include <featureuids.h>
#endif
#include <lbs/lbslocerrors.h>
#include <lbs/lbsadmin.h>
#include <lbs/lbslocclasstypes.h>
#include <lbs/lbsassistancedatabase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsnetcommon.h>
#include "lbscommoninternaldatatypes.h"
#include "LbsExtendModuleInfo.h"
#include "LbsInternalInterface.h"
#include "lbsdevloggermacros.h"
#include "netgateway.h"
#include "netregstatusmonitor.h"
#include <lbs/lbsgpsmeasurement.h>
#include "netgatewayasynccallback.h"
#include "netobserver.h"
#include "lbsnrhngmsgs.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "lbsnetextintversions.h"
#endif

const TInt KLbsLogQueueSize = 10;

// Privacy protocol module UID. This module is only currently used
// in standalone privacy mode, but this may change if this module
// is extended to support more than just privacy requests 
const TUid KPrivacyProtocolModuleUid = { 0x1028373C };

// Pre-defined technology type for autonomous.
const TPositionModuleInfo::TTechnologyType KAutonomous = (TPositionModuleInfo::ETechnologyTerminal);

// Pre-defined technology type for terminal-based (TB).
const TPositionModuleInfo::TTechnologyType KTerminalBased = (TPositionModuleInfo::ETechnologyTerminal
															 | TPositionModuleInfo::ETechnologyAssisted);

// Pre-defined technology type for terminal-assisted (TA).
const TPositionModuleInfo::TTechnologyType KTerminalAssisted = (TPositionModuleInfo::ETechnologyNetwork
																| TPositionModuleInfo::ETechnologyAssisted);

// Convert TLbsNetSessionId to TLbsNetSessionIdInt
#define TLBSNETSESSIONIDINT(sessionId) \
	TLbsNetSessionIdInt(sessionId.SessionOwner(), sessionId.SessionNum())

// Convert TLbsNetSessionIdInt to TLbsNetSessionId
#define TLBSNETSESSIONID(sessionId) \
	TLbsNetSessionId(sessionId.SessionOwner(), sessionId.SessionNum())


//
// CNetworkGateway
//

/**
*/
CNetworkGateway* CNetworkGateway::NewL()
	{
	CNetworkGateway* self = new (ELeave) CNetworkGateway;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

/**
*/
CNetworkGateway::CNetworkGateway()
    {
    for (TInt index = 0; index < EInvalidSession; index++)
       {
       iModuleForSession[index] = NULL;
       }
	}

/**
*/	
CNetworkGateway::~CNetworkGateway()
	{
	LBSLOG(ELogP9, "->S CLbsNetworkProtocolBase::~ClbsNetworkProtocolBase() ProtocolModule\n");

	delete iExternalLogEvent;
	
	iLogger.Close();
	
	DeleteLoadedModules();
	
	delete iNetworkRequestChannel;
	delete iPSYChannel;
	delete iNetworkRegistrationStatusMonitor;
	delete iAgpsChannel;
	delete iNetworkLocationChannel;
	delete iAdmin;
	delete iCloseDownRequestDetector;
	delete iRejectPrivacyRequest;
	
	REComSession::FinalClose();
	#if defined(_DEBUG)
		if(iOomListener)
			{
			iOomListener->Cancel();
			delete iOomListener;
			}
	#endif 
	
	// Close the Subsession with the Location Monitor
	if ( iLocMonSubSession.SubSessionHandle() )
        {
        iLocMonSubSession.Close();
        }
	// Close the session with the Location Monitor
	if ( iLocMonSession.Handle() )
        {
        iLocMonSession.Close();
        }
	}

/**
*/	
void CNetworkGateway::ConstructL()
	{
	RProcess process;
	iCloseDownRequestDetector = CLbsCloseDownRequestDetector::NewL(this, process.SecureId());
	
	iAdmin = CLbsAdmin::NewL();

#if defined __WINSCW__ && defined SYMBIAN_CELLMO_CENTRIC
	TBool locationManagementSupported(EFalse);
#else
#ifdef SYMBIAN_FEATURE_MANAGER
	TBool locationManagementSupported = CFeatureDiscovery::IsFeatureSupportedL(NFeature::KLocationManagement);
#else
	TBool locationManagementSupported(ETrue);
#endif // SYMBIAN_FEATURE_MANAGER
#endif // __WINSCW__ && defined SYMBIAN_CELLMO_CENTRIC

	if(locationManagementSupported)
		{
		iNetworkLocationChannel = CNetworkLocationChannel::NewL(*this);
		iAgpsChannel = CAgpsChannel::NewL(*this);
		iNetworkRegistrationStatusMonitor = CNetworkRegistrationStatusMonitor::NewL(*this);
		}
	
	iNetworkRequestChannel = CNetworkRequestChannel::NewL(*this);
	iPSYChannel = CPsyRequestChannel::NewL(*this, RLbsNetChannel::EChannelNG2SUPLPSY);
	
	User::LeaveIfError(iAdmin->Get(KLbsProtocolModuleLoading, iLoadInfo));

	TLbsAdminProtocolModulesInfo homeModules;
	User::LeaveIfError(iAdmin->Get(KLbsSettingHomeProtocolModule, homeModules));
	TLbsAdminProtocolModulesInfo roamingModules;
	User::LeaveIfError(iAdmin->Get(KLbsSettingRoamingProtocolModule, roamingModules));
	
	homeModules.GetModuleIds(iHomePmUidArray, iHomePmCount);
	roamingModules.GetModuleIds(iRoamingPmUidArray,iRoamingPmCount);
	
	// Use the default protocol modules
	iHomePmUid		= iHomePmUidArray[0];
	iRoamingPmUid	= iRoamingPmUidArray[0];

	// Populate the list of all protocol modules with the home modules
	for(TInt homeIndex = 0; homeIndex < iHomePmCount; homeIndex++)
		{
		CProtocolModuleInfo* info = CProtocolModuleInfo::NewL(iHomePmUidArray[homeIndex], NULL, NULL, ETrue, EFalse);
		iModules[homeIndex] = info;
		}
	iPmCount = iHomePmCount;
	
	// Populate the list of all protocol modules with the roaming modules
	for(TInt roamingIndex = 0; roamingIndex < iRoamingPmCount; roamingIndex++)
		{
		TLbsProtocolModuleId pmId = iRoamingPmUidArray[roamingIndex];
		TBool alreadyPresent = EFalse;
		
		for(TInt index = 0; index < iHomePmCount; index++)
			{
			if(pmId == iModules[index]->ModuleUid())
				{
				alreadyPresent = ETrue;
				iModules[index]->SetRoaming(); // If already present then set it as roaming as well
				}
			}
		
		// If not present create it
		if(!alreadyPresent)
			{
			CProtocolModuleInfo* info = CProtocolModuleInfo::NewL(pmId, NULL, NULL, EFalse, ETrue);
			iModules[iPmCount] = info;
			iPmCount++;
			}
		}

	if (iHomePmUid == KPrivacyProtocolModuleUid)
		{
		TRAPD(err, LoadProtocolModuleL(iHomePmUid));
		iRegistrationStatus = RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork;
		if (err != KErrNone)
			{
			LBSLOG_ERR3(ELogP3, "Error creating privacy protocol module UID 0x%x: %d",iHomePmUid , err);
			}
		}
	else if(iLoadInfo.LoadingStrategy()==TLbsAdminProtocolModuleLoadingInfo::EProtocolModuleLoadingAll)
		{
		for(TUint index = 0; index < iPmCount; index++)
			{
			TRAPD(err, LoadProtocolModuleL(iModules[index]->ModuleUid()));
			if (err != KErrNone)
				{
				LBSLOG_ERR3(ELogP3, "Error creating protocol module, err:%d uid:0x%h", err, (iModules[index]->ModuleUid()).iUid);
				}
			}
		}
	
	iRejectPrivacyRequest = CRejectPrivacyRequestCallback::NewL();
	
	iLogger.Open(KLbsLogQueueSize);
	#if defined(_DEBUG)
		// For OOM testing. The listener will force an error on the next heap 
		// allocation when it is kicked by test code.
		iOomListener = CLbsOomListener::NewL();
		iOomListener->StartGettingRequests();
	#endif  
	
	// Establish a session with the Location Monitor 
	if(KErrNone == iLocMonSession.Connect())
	    {
        // Open subsession  
        iLocMonSubSession.OpenL(iLocMonSession);
	    }
	}

/** Set flag indicating from which protocol module the latest network message originated.
*/
void CNetworkGateway::SetReceivingProtocolModule(TLbsProtocolModuleId aId)
	{
	iReceivingModuleId = aId;
	}

/** Select and load a Network Protocol Module.

Depending on admin options, two protocol modules are loaded at gateway initialisation
or a single module is loaded when the phone registers.
Loading is a 'one-off' process, nothing further is loaded (and nothing is unloaded) till phone reboots
*/
void CNetworkGateway::LoadProtocolModuleL(TLbsProtocolModuleId aModuleId)
	{
	LBSLOG(ELogP2, "CNetworkGateway::LoadProtocolModuleL:");
	if(aModuleId != KLbsProtocolNullModuleId)
	    {
        CProtocolModuleInfo* moduleInfo = ModuleInfo(aModuleId);
        CLbsNetworkProtocolBase* module = NULL;
        
        if(moduleInfo)
            {
            CNetObserver* obs = new (ELeave) CNetObserver(this, aModuleId);
            moduleInfo->SetObserver(obs);
            
            TLbsNetProtocolModuleParams params(*obs);
            LBSLOG(ELogP9, "->S CLbsNetworkProtocolBase::NewL() ProtocolModule\n");
            LBSLOG2(ELogP9, "  > TLbsProtocolModuleId moduleId  = 0x%08X\n", aModuleId);
            module = CLbsNetworkProtocolBase::NewL(aModuleId, params);
            
            moduleInfo->SetProtocolModule(module);
            }
	    }
	}

//---------------------------------------------------------------------------------------------------
// Messages from a Protocol Module (MLbsNetworkProtocolObserver)
//---------------------------------------------------------------------------------------------------
void CNetworkGateway::GetCurrentCapabilities(TLbsNetPosCapabilities& aCapabilities) const
	{
	LBSLOG(ELogP9, "<-A MLbsNetworkProtocolObserver::GetCurrentCapabilities() ProtocolModule\n");
	LBSLOG(ELogP2, "CNetworkGateway::GetCurrentCapabilities:");
	LBS_RDEBUG("ProtMod", "LBS", "GetCurrentCapabilities");

	// This call is not necessarily associated with a session, don't try to filter on current protocol module */

	TPositionModuleInfoExtended::TDeviceGpsModeCapabilities deviceGpsModeCaps(TPositionModuleInfo::ETechnologyUnknown);

	// Get the capabilities information to fill in the position method(s) for aCapabilities.
	TInt err = LbsModuleInfo::GetDeviceCapabilities(KLbsGpsLocManagerUid, deviceGpsModeCaps);

#ifdef ENABLE_LBS_DEV_LOGGER
	if (err != KErrNone)
		{
		LBSLOG_WARN2(ELogP3, "Error reading module info (%d), defaulting to 'unknown' pos method", err);
		}
#endif // ENABLE_LBS_DEV_LOGGER
	
	// Convert the module capabilities into a TLbsNetPosCapabilities..
	CLbsAdmin::TGpsMode adminGpsMode = GetAdminGpsMode();
	switch (deviceGpsModeCaps)
		{
		case TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased:
			{
			TLbsNetPosMethod methods[2];
			methods[0].SetPosMethod(KLbsPositioningMeansGps, KTerminalBased);
			methods[1].SetPosMethod(KLbsPositioningMeansGps, KAutonomous);
			aCapabilities.SetPosMethods(methods, 2);
			
#ifdef ENABLE_LBS_DEV_LOGGER
			// Report a warning if the admin setting contradicts
			// the module capabilities.
			if (adminGpsMode != CLbsAdmin::EGpsPreferTerminalBased
				&& adminGpsMode != CLbsAdmin::EGpsAlwaysTerminalBased
				&& adminGpsMode != CLbsAdmin::EGpsAutonomous)
				{
				LBSLOG_WARN3(ELogP3, 
							 "KLbsSettingXXXGpsMode (%d) contradicts GPS module capabilties (0x%x)!", 
							 adminGpsMode,
							 deviceGpsModeCaps);
				}
#endif // ENABLE_LBS_DEV_LOGGER
			break;
			}
		case TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted:
			{
			TLbsNetPosMethod method;
			method.SetPosMethod(KLbsPositioningMeansGps, KTerminalAssisted);
			aCapabilities.SetPosMethods(&method, 1);
			
#ifdef ENABLE_LBS_DEV_LOGGER
			// Report a warning if the admin setting contradicts
			// the module capabilities.
			if (adminGpsMode != CLbsAdmin::EGpsPreferTerminalAssisted
				&& adminGpsMode != CLbsAdmin::EGpsAlwaysTerminalAssisted)
				{
				LBSLOG_WARN3(ELogP3, 
							 "KLbsSettingXXXGpsMode (%d) contradicts GPS module capabilties (0x%x)!", 
							 adminGpsMode,
							 deviceGpsModeCaps);
				}
#endif // ENABLE_LBS_DEV_LOGGER
			break;
			}
		case TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB:
		case (TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased
			  | TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted):
			{
			// If the module supports both TA and TB, then check the admin
			// settings to find the 'preferred' one to put first in the 
			// list of supported modes.			
			TLbsNetPosMethod methods[3];
			switch (adminGpsMode)
				{
				case CLbsAdmin::EGpsAlwaysTerminalAssisted:
				case CLbsAdmin::EGpsPreferTerminalAssisted:
					{
					methods[0].SetPosMethod(KLbsPositioningMeansGps, KTerminalAssisted);
					methods[1].SetPosMethod(KLbsPositioningMeansGps, KTerminalBased);
					break;
					}
				case CLbsAdmin::EGpsAutonomous:
				case CLbsAdmin::EGpsAlwaysTerminalBased:
				case CLbsAdmin::EGpsPreferTerminalBased:
				default:
					{
					methods[0].SetPosMethod(KLbsPositioningMeansGps, KTerminalBased);
					methods[1].SetPosMethod(KLbsPositioningMeansGps, KTerminalAssisted);
					break;
					}
				}
			methods[2].SetPosMethod(KLbsPositioningMeansGps, KAutonomous);
			aCapabilities.SetPosMethods(methods, 3);
			break;
			}
		case TPositionModuleInfoExtended::EDeviceGpsModeAutonomous:
			{
			TLbsNetPosMethod method;
			method.SetPosMethod(KLbsPositioningMeansGps, KAutonomous);
			aCapabilities.SetPosMethods(&method, 1);

#ifdef ENABLE_LBS_DEV_LOGGER
			// Report a warning if the admin setting contradicts
			// the module capabilities.
			if (adminGpsMode != CLbsAdmin::EGpsAutonomous)
				{
				LBSLOG_WARN3(ELogP3, 
							 "KLbsSettingXXXGpsMode (%d) contradicts GPS module capabilties (0x%x)!", 
							 adminGpsMode,
							 deviceGpsModeCaps);
				}
#endif // ENABLE_LBS_DEV_LOGGER
			break;
			}
		case TPositionModuleInfoExtended::EDeviceGpsModeNone:
		default:
			{
			// If the capabilities are not known or unrecognised, set unknown pos method.
			TLbsNetPosMethod method;
			method.SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyUnknown);
			aCapabilities.SetPosMethods(&method, 1);
			LBSLOG_WARN3(ELogP3, 
						 "Unrecognised GPS module capabilities (%d), defaulting to TPositionModuleInfo::ETechnologyUnknown", 
						 adminGpsMode, 
						 deviceGpsModeCaps);
			break;
			}
		}
	}

void CNetworkGateway::ProcessStatusUpdate(TLbsNetProtocolServiceMask /*aActiveServiceMask*/)
	{
	LBSLOG(ELogP9, "<-A MLbsNetworkProtocolObserver::ProcessStatusUpdate() ProtocolModule\n");
	LBSLOG(ELogP2, "CNetworkGateway::ProcessStatusUpdate:");
	LBS_RDEBUG("ProtMod", "LBS", "ProcessStatusUpdate");
	//LBSLOG2(ELogP2, "Active Service Mask : 0x%x", aActiveServiceMask);
	
	// TODO: It is TBD exactly what/where this update should go.
	// For now, just consume it.
	}

void CNetworkGateway::ProcessPrivacyRequest(const TLbsNetSessionId& aSessionId,
										    TBool aEmergency, 
										    const TLbsNetPosRequestPrivacy& aPrivacy,
										    const TLbsExternalRequestInfo& aRequestInfo)
	{
	LBS_RDEBUG("ProtMod", "LBS", "ProcessPrivacyRequest");
	__ASSERT_ALWAYS((aRequestInfo.ClassType() == EExternalRequestInfoClass)
					|| (aRequestInfo.ClassType() == (EExternalRequestInfoClass 
													| EExternalRequestInfoClass2)),
					User::Panic(_L("LbsNetGateway"), 2));
	
	LBSLOG(ELogP9, "<-A MLbsNetworkProtocolObserver::ProcessPrivacyRequest() ProtocolModule\n");
	LBSLOG(ELogP9, "  > TLbsNetSessionId aSessionId  = \n");
	LBSLOG_SESSIONID(aSessionId);
	LBSLOG2(ELogP9, "  > TBool aEmergency  = %d\n", aEmergency);
	LBSLOG(ELogP9, "  > TLbsNetPosRequestPrivacy aPrivacy  =\n");
	LBSLOG_POSREQUESTPRIVACY(aPrivacy);
	
	/** LBSLOGGER - Start Logging */
	// -------------------------------------------------------------------------

	TInt err = KErrNone;
	if(iExternalLogEvent == NULL)
		TRAP(err, iExternalLogEvent = CLbsExternalLocateLogEvent::NewL(aSessionId.SessionOwner(), NULL, NULL));
	if (err == KErrNone)
		{
		TLbsLoggingPrivacyRequestParams params;
		params.iSessionId = aSessionId;
		// Only TLbsExternalRequestInfo is logged, so if aRequestInfo
		// is a TLbsExternalRequestInfo2, the string(s) will be truncated.
		params.iRequestInfo = aRequestInfo;
		params.iRequestPrivacy =  aPrivacy;
		iExternalLogEvent->SetPrivacyRequestParams(params);
		}
	LBSLOG(ELogP9, "  < TLbsExternalRequestInfo aRequestInfo  =\n");
	//LBSLOG_TLBSEXTERNALREQ(aRequestInfo);
	

	// Convert aPrivacy to TLbsNetPosRequestPrivacyInt
	// because it is used to createTLbsNetMtLrRequestMsg.
	TLbsNetPosRequestPrivacyInt privacyInt;
	privacyInt.SetRequestAdvice((TLbsNetPosRequestPrivacyInt::TLbsRequestAdviceInt)
			aPrivacy.RequestAdvice());
	privacyInt.SetRequestAction((TLbsNetPosRequestPrivacyInt::TLbsRequestActionInt)
			aPrivacy.RequestAction());
		
	TLbsNetMtLrRequestMsg msg(
			TLBSNETSESSIONIDINT(aSessionId),
			aEmergency, privacyInt, aRequestInfo);

	if(ValidPm(aSessionId.SessionOwner()))
		{
		iNetworkRequestChannel->SendNetRequestMessage(msg);
		}
	else
		{
		//Not a valid PM, reject this message(rather than silently ignore??)
		iRejectPrivacyRequest->CallBack((ModuleInfo(aSessionId.SessionOwner()))->ProtocolModule(), aSessionId);	
		}
	// -------------------------------------------------------------------------
	/** LBSLOGGER - End Logging */
	}

void CNetworkGateway::ProcessLocationRequest(const TLbsNetSessionId& aSessionId,
											 TBool aEmergency,
											 TLbsNetProtocolService aService, 
											 const TLbsNetPosRequestQuality& aQuality,
											 const TLbsNetPosRequestMethod& aMethod)
	{
	LBS_RDEBUG("ProtMod", "LBS", "ProcessLocationRequest");
	LBSLOG(ELogP9, "<-A MLbsNetworkProtocolObserver::ProcessLocationRequest() ProtocolModule\n");
	LBSLOG(ELogP9, "  > TLbsNetSessionId aSessionId  = \n");
	LBSLOG_SESSIONID(aSessionId);
	LBSLOG2(ELogP9, "  > TBool aEmergency  = %d\n", aEmergency);
	LBSLOG2(ELogP9, "  > TLbsNetProtocolService aService  = 0x%02X\n", aService);
	LBSLOG(ELogP9, "  > TLbsNetPosRequestQuality aQuality  = \n");
	LBSLOG_POSREQUESTQUALITY(aQuality);	
	LBSLOG(ELogP9, "  > TLbsNetPosRequestMethod aMethod  =\n");
	LBSLOG_POSREQUESTMETHOD(aMethod);
	
	// If the received message is from a protocol module that is not registered reject it
	if(!ValidPm(iReceivingModuleId))
		{
		return;
		}
	// Convert aQuality to TLbsNetPosRequestQualityInt
	// because it is needed to create TLbsNetLocationRequestMsg.
	TLbsNetPosRequestQualityInt qualityInt;
	ConvertToTLbsNetPosRequestQualityInt(aQuality, qualityInt);
	
	// Convert aMethod to TLbsNetPosRequestMethodInt
	// because it is needed to create TLbsNetLocationRequestMsg.
	 TLbsNetPosRequestMethodInt methodInternal;
	 TLbsNetPosMethodInt posMethodInternal[KLbsMaxNetPosMethodsInt];
	 for (int i = 0; i < aMethod.NumPosMethods(); i++)
		{
		TLbsNetPosMethod posMethodTemp;
		aMethod.GetPosMethod(i, posMethodTemp);
		posMethodInternal[i].SetPosMethod(posMethodTemp.PosMeans(),
				 (TPositionModuleInfoInt::TTechnologyTypeInt) posMethodTemp.PosMode());
		}
	 methodInternal.SetPosMethods(posMethodInternal, aMethod.NumPosMethods());
	 methodInternal.SetGpsTimingOfCellFramesRequested(aMethod.GpsTimingOfCellFramesRequested());
	
	// Send message into the NRH.
	TLbsNetLocationRequestMsg msg(
			TLBSNETSESSIONIDINT(aSessionId), 
			aEmergency,
			(TLbsNetworkEnumInt::TLbsNetProtocolServiceInt) aService,
			qualityInt,
			methodInternal);
	iNetworkRequestChannel->SendNetRequestMessage(msg);
	
		
	/** LBSLOGGER - Start Logging */
	// -------------------------------------------------------------------------
	if (iExternalLogEvent == NULL)
		{
		TRAP_IGNORE(iExternalLogEvent = CLbsExternalLocateLogEvent::NewL(aSessionId.SessionOwner(), NULL, NULL));
		}
	else
		{
		iExternalLogEvent->SetRequestOriginator(aSessionId.SessionOwner());
		}
	// -------------------------------------------------------------------------
	/** LBSLOGGER - End Logging */
	}

void CNetworkGateway::ProcessSessionComplete(const TLbsNetSessionId& aSessionId,
											 TInt  aReason)
	{
	LBS_RDEBUG_ARGINT("ProtMod", "LBS", "ProcessSessionComplete",aReason);
	LBSLOG(ELogP9, "<-A MLbsNetworkProtocolObserver::ProcessSessionComplete() ProtocolModule\n");
	LBSLOG(ELogP9, "  > TLbsNetSessionId aSessionId  = \n");
	LBSLOG_SESSIONID(aSessionId);
	LBSLOG2(ELogP9, "  > aReason  = %d\n", aReason);
	
	// If this is a PM that is calling us
	if ( ModuleInfo(aSessionId.SessionOwner()) )
		{
		// If we are registered, silently ignore any message from a now invalid PM.
		// Otherwise, could be temporaily unregistered, so send on message in hope that
		// phone will shortly re-register as home/roaming.
		if(!ValidPm(iReceivingModuleId))
			{
			return;
			}
		}

	// The NRH gets all session complete messages and may ignore
	// them if the Session Owner is NLM or AGPS manager, but
	// no Location Request processing is taking place.
	TLbsNetSessionCompleteMsg msgNRH(TLBSNETSESSIONIDINT(aSessionId), aReason);
	iNetworkRequestChannel->SendNetRequestMessage(msgNRH);
	
	if (aSessionId.SessionOwner() == KLbsGpsLocManagerUid)
		{
		if(iAgpsChannel)
			{
			iAgpsChannel->SendSessionComplete(aReason, TLBSNETSESSIONIDINT(aSessionId));
			}
		}
	else if (aSessionId.SessionOwner() == KLbsNetLocManagerUid)
		{
		if(iNetworkLocationChannel)
			{
			TLbsNetLocNetworkLocationCompleteMsg msgNLM(TLBSNETSESSIONIDINT(aSessionId), aReason);
			iNetworkLocationChannel->SendNetworkLocationMessage(msgNLM);
			}
		}
	else if (aSessionId.SessionOwner() == KLbsSuplPsyUid)
	    {
        if(iPSYChannel)
            {
            iPSYChannel->SendNetworkResponse(msgNRH);
            }
	    }
	// else the session owner is NRH (KLbsNetRequestHandlerUid)
	// or a network-MTLR (owner id may vary) and the msg goes only to the NRH.}
		
		
	/** LBSLOGGER - Start Logging */
	// -------------------------------------------------------------------------
	//ASSERT(iExternalLogEvent != NULL);
	if (iExternalLogEvent != NULL)
		{
		if (!iCancelled)
			{
			switch (aReason)
				{
				case KErrNone:
					{
					iExternalLogEvent->SetRequestOutcome(ELbsRequestOutcomeSuccess);
					break;
					}
				case KErrCancel:
					{
					iExternalLogEvent->SetRequestOutcome(ELbsRequestOutcomeCancel);
					break;
					}
				default:
					{
					iExternalLogEvent->SetRequestOutcome(ELbsRequestOutcomeFail);
					}
				};
			}
		else
			{
			iExternalLogEvent->SetRequestOutcome(ELbsRequestOutcomeCancel);
			}

		iLogger.AddEvent(*iExternalLogEvent);
		
		delete iExternalLogEvent;
		iExternalLogEvent = NULL;
		iSeenLocationUpdate = EFalse;
		iCancelled = EFalse;
		}
	// -------------------------------------------------------------------------
	/** LBSLOGGER - End Logging */
	}

void CNetworkGateway::ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
											const RLbsAssistanceDataBuilderSet& aData,
						   					TInt aReason)
	{
	LBS_RDEBUG_ARGINT("ProtMod", "LBS", "ProcessAssistanceData", aReason);

	if(iAgpsChannel)
		{
		LBSLOG(ELogP9, "<-A MLbsNetworkProtocolObserver::ProcessAssistanceData() ProtocolModule\n");
		LBSLOG2(ELogP9, "  > TLbsAsistanceDataGroup aDataMask  = 0x%08x\n", aDataMask);
		LBSLOG2(ELogP9, "  > TInt aReason  = %d\n", aReason);

		iAgpsChannel->SendAssistanceDataResponse(aReason, 
				(TLbsAsistanceDataGroupInt) aDataMask, aData);
		}
	}
	
void CNetworkGateway::ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
								const RLbsAssistanceDataBuilderSet& aData,
								TInt aReason,
								const TLbsNetSessionIdArray& /*aSessionIdArray*/)
	{
	LBS_RDEBUG_ARGINT("ProtMod", "LBS", "ProcessAssistanceData", aReason);
	LBSLOG(ELogP9, "<-A MLbsNetworkProtocolObserver::ProcessAssistanceData() v2 ProtocolModule\n");

	// We do not attempt to filter out the assistance data based upon whether it came from an
	// invalid protocol module. Assistance data is valid regardless of where it came from and
	// is allowed to arrive at any time.
	
	// The session ID array is currently not used.
	ProcessAssistanceData(aDataMask, aData, aReason);
	}

void CNetworkGateway::ProcessLocationUpdate(const TLbsNetSessionId& aSessionId,
											 const TPositionInfoBase& aPosInfo)
	{
	LBS_RDEBUG("ProtMod", "LBS", "ProcessLocationUpdate");
	LBSLOG(ELogP9, "<-A MLbsNetworkProtocolObserver::ProcessLocationUpdate() ProtocolModule\n");
	LBSLOG(ELogP9, "  > TLbsNetSessionId aSessionId  = \n");
	LBSLOG_SESSIONID(aSessionId);
	LBSLOG(ELogP9, "  > TPositionInfoBase aPosInfo  = \n");
	LBSLOG_TPOSITIONINFOBASE(aPosInfo);

	// If the received message is from a protocol module that is not registered reject it
	if(!ValidPm(iReceivingModuleId))
		{
		return;
		}

	/** LBSLOGGER - Start Logging */
	// -------------------------------------------------------------------------
	TPositionInfoBase* posInfo = const_cast<TPositionInfoBase*>(&aPosInfo);
	if (iSeenLocationUpdate)
		{
		//ASSERT(iExternalLogEvent != NULL);
		// In MOLR we see two reference positions
		
		if(iExternalLogEvent)
		    {
		    iLogger.AddEvent(*iExternalLogEvent);
		    delete iExternalLogEvent;
		    iExternalLogEvent = NULL;
		    }
		}
	
	if (iExternalLogEvent == NULL)
		{
		TRAP_IGNORE(iExternalLogEvent = CLbsExternalLocateLogEvent::NewL(aSessionId.SessionOwner(), posInfo, NULL));
		}
	else
		{
		if(posInfo->PositionClassType() != EPositionGpsMeasurementInfoClass)
			{
			iExternalLogEvent->SetReferenceLocation(posInfo);
			}
		}
	iSeenLocationUpdate = ETrue;
	// -------------------------------------------------------------------------
	/** LBSLOGGER - End Logging */
	
	// Route Ref and FNP positions to the Location monitor if those are not NAN 
	TPosition pos;
	const TPositionInfo& positionInfo = reinterpret_cast <const TPositionInfo&> (aPosInfo);
	positionInfo.GetPosition(pos);
	if (iLocMonSubSession.SubSessionHandle() && !(Math::IsNaN(pos.Latitude()) ||Math::IsNaN(pos.Longitude())))
        {
        iLocMonSubSession.SetLastKnownPosition(aPosInfo);          
        }
		
	// Route only manager Ref and FNP positions to the P&S bus
	if (aSessionId.SessionOwner() == KLbsGpsLocManagerUid ||
		aSessionId.SessionOwner() == KLbsNetLocManagerUid)
		{
		if(iNetworkLocationChannel)
			{
		iNetworkLocationChannel->SetNetworkPosition(TLBSNETSESSIONIDINT(aSessionId), aPosInfo);
			}
		}
	
	if (aSessionId.SessionOwner() == KLbsSuplPsyUid) 
        {
        if(iPSYChannel)
            {
            TLbsCellLocationResponseMsg msg(TLBSNETSESSIONIDINT(aSessionId), KErrNone, aPosInfo);
            iPSYChannel->SendNetworkResponse(msg);
            }
        }
	
	// Route all but AGPS manager Ref and FNP positions to the NRH
	if (aSessionId.SessionOwner() != KLbsGpsLocManagerUid) 
		{
		TTime actualTime;
		actualTime.UniversalTime();

		TLbsNetLocationUpdateMsg msgNRH(TLBSNETSESSIONIDINT(aSessionId), KErrNone, aPosInfo, actualTime);
		
		iNetworkRequestChannel->SendNetRequestMessage(msgNRH);
		}
	}

//---------------------------------------------------------------------------------------------------
// Messages from the Network Location Manager (MNetworkLocationObserver)
//---------------------------------------------------------------------------------------------------
void CNetworkGateway::ProcessNetworkLocationMessage(const TLbsNetLocMsgBase& aMessage)
	{
	LBSLOG(ELogP2, "CNetworkGateway::ProcessNetworkLocationMessage:");
	LBSLOG2(ELogP3, "Type : %d", aMessage.Type());
	
	switch (aMessage.Type())
		{
		case TLbsNetLocMsgBase::ENetLocMsgNetworkLocationRequest:
			{
			const TLbsNetLocNetworkLocationRequestMsg& req = static_cast<const TLbsNetLocNetworkLocationRequestMsg&>(aMessage);
			TLbsNetPosRequestQuality quality;
			ConvertToTLbsNetPosRequestQuality(req.Quality(), quality);
			TLbsNetPosRequestOptions options;
			options.SetRequestQuality(quality);

            // This is first message in network location session, so store the PM used.
            iModuleForSession[ENetworkLocation] = DefaultPm();

			/** LBSLOGGER - Start Logging */
			// -------------------------------------------------------------------------
			if (iExternalLogEvent == NULL)
				{
				TRAP_IGNORE(iExternalLogEvent = CLbsExternalLocateLogEvent::NewL(req.SessionId().SessionOwner(), NULL, NULL));
				}
			// -------------------------------------------------------------------------
			/** LBSLOGGER - End Logging */

			// As this is first message in network location session, we send to the current PM if this exists.
			if (DefaultPm())
				{
				LBSLOG(ELogP9, "->A CLbsNetworkProtocolBase::RequestNetworkLocation() ProtocolModule\n");
				LBSLOG(ELogP9, "  > TLbsNetLocNetworkLocationRequestMsg reg.SessionId()  = \n");
				LBSLOG_SESSIONID(req.SessionId());
				LBSLOG(ELogP9, "  > TLbsNetPosRequestOptions options  =\n");
				// coverity[uninit_use_in_call]
				// We're only logging quality here, so it does not matter that other parts of options are not initialsed
				LBSLOG_POSREQUESTOPTIONS(options);

				LBS_RDEBUG("LBS", "ProtMod", "RequestNetworkLocation");
				DefaultPm()->RequestNetworkLocation(TLBSNETSESSIONID(req.SessionId()), options);
				}
			else
				{
				// No protocol module to send the request to,
				// so send a 'no network available' error back instead!
				ProcessSessionComplete(
						TLBSNETSESSIONID(req.SessionId()), KErrPositionNetworkUnavailable);
				}
			break;
			}
		case TLbsNetLocMsgBase::ENetLocMsgNetworkLocationCancel:
			{
			const TLbsNetLocNetworkLocationCancelMsg& cancel = static_cast<const TLbsNetLocNetworkLocationCancelMsg&>(aMessage);

			// Always send message to the PM with which we initiated the session.
			if (iModuleForSession[ENetworkLocation])
				{
				LBSLOG(ELogP9, "->A CLbsNetworkProtocolBase::CancelNetworkLocation() ProtocolModule\n");
				LBSLOG(ELogP9, "  > TLbsNetLocNetworkLocationRequestMsg cancel.SessionId()  = \n");
				LBSLOG_SESSIONID(cancel.SessionId());
				LBSLOG2(ELogP9, "  > TInt cancel.Reason()  = %d\n", cancel.Reason());

				LBS_RDEBUG_ARGINT("LBS", "ProtMod", "CancelNetworkLocation", cancel.Reason());
				iModuleForSession[ENetworkLocation]->CancelNetworkLocation(TLBSNETSESSIONID(cancel.SessionId()), cancel.Reason());
				}
			else
				{
				// We should only get here if the registration status is ENotRegistered. 
				// Therefore we cannot send the message to a protocol module so discard the Cancel.
				LBSLOG(ELogP9, "CNetworkGateway::ProcessNetworkLocationMessage: LocationCancel but ENotRegistered, discard Cancel");
				}
			iCancelled = ETrue;
			break;
			}
		case TLbsNetLocMsgBase::ENetLocMsgNetworkLocationComplete:
		default:
			{
			break;
			}
		}
	}

//---------------------------------------------------------------------------------------------------
// Messages from the Network PSYs (MPsyRequestObserver)
//---------------------------------------------------------------------------------------------------
void CNetworkGateway::ProcessPsyMessage(const TLbsNetInternalMsgBase& aMessage)
    {
    LBSLOG(ELogP2, "CNetworkGateway::ProcessPsyMessage:");
    LBSLOG2(ELogP2, "Type : %d", aMessage.Type());
            
    switch (aMessage.Type())
        {
        case TLbsNetInternalMsgBase::ECellLocationRequest:
            {
            const TLbsCellLocationRequestMsg& reqMsg = reinterpret_cast<const TLbsCellLocationRequestMsg&>(aMessage);

            TLbsNetPosRequestQuality quality;
            TLbsNetPosRequestQualityInt qualityInt;
            
            reqMsg.Options().GetRequestQuality(qualityInt);
            ConvertToTLbsNetPosRequestQuality(qualityInt, quality);
            
            TLbsNetPosRequestOptions options;
            options.SetRequestQuality(quality);
            options.SetNewClientConnected(reqMsg.Options().NewClientConnected());

            TUid pmUid = reqMsg.ProtocolModule(); 
            CProtocolModuleInfo* modInfo = ModuleInfo(pmUid);
            
            if(modInfo)
                {
                CLbsNetworkProtocolBase* pm = modInfo->ProtocolModule();
                pm->RequestNetworkLocation(TLBSNETSESSIONID(reqMsg.SessionId()), options);
            
                // This is first message in network location session, so store the PM used.
                if(reqMsg.SessionId().SessionOwner() == KLbsNetLocManagerUid)
                    {
                    iModuleForSession[ENetworkLocation] = pm;
                    }
                else
                    {
                    iModuleForSession[EPSYLocation] = pm;
                    }
                }
            else
                {
                // No protocol module to send the request to
                ProcessSessionComplete(TLBSNETSESSIONID(reqMsg.SessionId()), KErrPositionNetworkUnavailable);
                }
            break;
            }

        case TLbsNetInternalMsgBase::ECellLocationCancel:
            {
            const TLbsCellLocationCancelMsg& cancelMsg = reinterpret_cast<const TLbsCellLocationCancelMsg&>(aMessage);

            CLbsNetworkProtocolBase* pm;
            if(cancelMsg.SessionId().SessionOwner() == KLbsNetLocManagerUid)
                {
                pm = iModuleForSession[ENetworkLocation];
                }
            else
                {
                pm = iModuleForSession[EPSYLocation];
                }
            
            // Always send message to the PM with which we initiated the session.
            if (pm)
                {
                pm->CancelNetworkLocation(TLBSNETSESSIONID(cancelMsg.SessionId()), cancelMsg.Reason());
                }
            else
                {
                // We should only get here if the registration status is ENotRegistered. 
                // Therefore we cannot send the message to a protocol module so discard the Cancel.
                LBSLOG(ELogP9, "CNetworkGateway::ProcessPsyMessage: LocationCancel but ENotRegistered, discard Cancel");
                }
            iCancelled = ETrue;
            break;
            }
        default:
            {
             break;
            }
       }
    }

//---------------------------------------------------------------------------------------------------
// Messages from the AGPS Manager (MAgpsObserver)
//---------------------------------------------------------------------------------------------------
void CNetworkGateway::OnAssistanceDataRequest(TLbsAsistanceDataGroupInt aDataMask)
	{
	LBSLOG(ELogP1, "CNetworkGateway::OnAssistanceDataRequest:");
	LBSLOG2(ELogP2, "Assistance Data Request. Data Mask : 0x%x", aDataMask);

	LBSLOG(ELogP9, "->A CLbsNetworkProtocolBase::RequestAssistanceData() ProtocolModule\n");
	LBSLOG2(ELogP9, "  > TLbsAsistanceDataGroupInt aDataMask  = 0x%08X\n", aDataMask);

	// Check there is at least one protocol module before attempting to send assistance data 
	if (DefaultPm())
		{
		CLbsNetworkProtocolBase* pm = NULL;
		CLbsNetworkProtocolBase2* pm2 = NULL;
		
		for(TUint index = 0; index < iPmCount; index++)
			{
			LBS_RDEBUG_ARGINT("LBS", "ProtMod", "RequestAssistanceData", aDataMask);
			pm = (iModules[index])->ProtocolModule();

			if(pm)
				{
				pm2 = reinterpret_cast<CLbsNetworkProtocolBase2*> 
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
				(pm->ExtendedInterface(CLbsNetworkProtocolBase::EExtInterface2,0,0));
#else
				(pm->ExtendedInterface(ELbsNetExtInterface2,0,0));
#endif
				if(pm2 != NULL)
					{
					const TLbsNetSessionIdArray dummySessIdArray;
					pm2->RequestAssistanceData(aDataMask, dummySessIdArray);
					}
				else
					{
					pm->RequestAssistanceData(aDataMask);
					}
				}
			}
		}
	else
		{
		// Shouldn't get here as logic error but gateway does not panic in release mode.
		LBSLOG_ERR(ELogP3, "CNetworkGateway::OnAssistanceDataRequest: Could not identify where to send assistance data request");


		if(aDataMask != EAssistanceDataNone)
			{
			// No protocol module to send the request to, so send
			// a 'no network available' error back!
			RLbsAssistanceDataBuilderSet dummyBuilderSet;
			ProcessAssistanceData(aDataMask, dummyBuilderSet, KErrPositionNetworkUnavailable);
			}
		}
	}
	
/**
From AGPS Manager
*/
void CNetworkGateway::OnSelfLocationRequest(const TLbsNetSessionIdInt& aSessionId, 
											const TLbsNetPosRequestOptionsAssistanceInt& aOptions)
	{
	LBSLOG(ELogP1, "CNetworkGateway::OnSelfLocationRequest");
	LBSLOG4(ELogP2, "Self Location Request. SessionId : (%d, %d). Data Mask : 0x%x", aSessionId.SessionOwner().iUid,
											  										aSessionId.SessionNum(),
											  										aOptions.DataRequestMask());

    // First message in MO-LR session as far as the gateway is concerned, so store the (ptr to) module.
    iModuleForSession[ESelfLocation] = DefaultPm();

	if (DefaultPm())
		{
		// Generate log for RequestSelfLocation message that we are send to the current protocol module
		LBSLOG(ELogP9, "->A CLbsNetworkProtocolBase::RequestSelfLocation() ProtocolModule");
		LBSLOG(ELogP9, "  > TLbsNetSessionId aSessionId  = \n");
		LBSLOG_SESSIONID(aSessionId);
		LBSLOG(ELogP9, "  > TLbsNetPosRequestOptionsAssistance aOptions  =\n");
		LBSLOG_REQUESTOPTIONSASSISTANCE(aOptions);
		LBS_RDEBUG("LBS", "ProtMod", "RequestSelfLocation");

		TLbsNetPosRequestOptionsAssistance optionsAssistance;
		ConvertToTLbsNetPosRequestOptionsAssistance(aOptions, optionsAssistance);

		DefaultPm()->RequestSelfLocation(TLBSNETSESSIONID(aSessionId), optionsAssistance);
		}
	else
		{
		// No protocol module to send the request to...
    	if (TPositionModuleInfo::ETechnologyTerminal == aOptions.PosMode())
	    	{
		    // If the positioning method is autonomous, just complete the session
			ProcessSessionComplete(TLBSNETSESSIONID(aSessionId), KErrNone);
	   		}
		else
 	   		{
 	   		// otherwise send a 'no network available' error  back to the application 
 	   		// (regardless of whether data mask is zero or not). Also, send appropriate 
 	   		// session complete message.
			RLbsAssistanceDataBuilderSet dummyBuilderSet;
			ProcessAssistanceData(EAssistanceDataNone, dummyBuilderSet, KErrPositionNetworkUnavailable);
			ProcessSessionComplete(TLBSNETSESSIONID(aSessionId), KErrPositionNetworkUnavailable);
			}
		}
	}

/**
From AGPS Manager
*/
void CNetworkGateway::OnSelfLocationCancel(const TLbsNetSessionIdInt& aSessionId, TInt aReason)
	{
	LBSLOG(ELogP1, "CNetworkGateway::OnSelfLocationCancel");
	LBSLOG4(ELogP2, "Self Location Cancel. SessionId : (%d, %d). Reason    : %d\n", aSessionId.SessionOwner().iUid, 
											  										aSessionId.SessionNum(),
																					aReason);
	/** LBSLOGGER - Start Logging */
	// -------------------------------------------------------------------------
	if (iExternalLogEvent == NULL)
		{
		TRAP_IGNORE(iExternalLogEvent = CLbsExternalLocateLogEvent::NewL(aSessionId.SessionOwner(), NULL, NULL));
		}
	// -------------------------------------------------------------------------
	/** LBSLOGGER - End Logging */

	if (iModuleForSession[ESelfLocation])
		{
		LBSLOG(ELogP9, "->A CLbsNetworkProtocolBase::CancelSelfLocation() ProtocolModule\n");
		LBSLOG(ELogP9, "  > TLbsNetSessionId aSessionId  = \n");
		LBSLOG_SESSIONID(aSessionId);
		LBSLOG2(ELogP9, "  > TInt aReason  = %d\n", aReason);
		LBS_RDEBUG_ARGINT("LBS", "ProtMod", "CancelSelfLocation", aReason);

		iModuleForSession[ESelfLocation]->CancelSelfLocation(TLBSNETSESSIONID(aSessionId), aReason);
		}
	else
		{
		// Shouldn't get here as logic error but gateway does not panic in release mode.
		LBSLOG_ERR(ELogP3, "CNetworkGateway::OnSelfLocationCancel: Cancel message but no default PM");
		__ASSERT_DEBUG(EFalse, User::Panic(KLbsNGFault, ENGUnexpectedSessionIdOrType));
		}
	}

/**
From AGPS Manager
*/
void CNetworkGateway::OnSystemStatusAdvice(TBool aTracking)
	{
	LBSLOG(ELogP2, "CNetworkGateway::OnSystemStatusAdvice");
	// NG must have a cache if more statuses added
	
	LBSLOG(ELogP9, "->A CLbsNetworkProtocolBase::AdviceSystemStatus() ProtocolModule\n");

	CLbsNetworkProtocolBase::TLbsSystemStatus tracking;
	tracking = (aTracking) ? CLbsNetworkProtocolBase::ESystemStatusClientTracking : CLbsNetworkProtocolBase::ESystemStatusNone;
	LBSLOG2(ELogP9, "  > TLbsSystemStatus sStatus  = 0x%02X\n", tracking);

	CLbsNetworkProtocolBase* pm = NULL;
	for(TUint index = 0; index < iPmCount; index++)
		{
		pm = (iModules[index])->ProtocolModule();
		if(pm)
			{
			pm->AdviceSystemStatus(tracking);
			}
		}
	}

//---------------------------------------------------------------------------------------------------
// Messages from the Network Request Handler (MNetworkRequestObserver)
//---------------------------------------------------------------------------------------------------
/** Process incoming messages from the Network Request Handler.

This function is called for each message that is received from the NRH.
In most cases it determines which protocol module is being used for the session
and passes the message contents to that module by calling the appropriate function
on the api.
*/	
void CNetworkGateway::ProcessNetRequestMessage(const TLbsNetInternalMsgBase& aMessage)
	{
	LBSLOG(ELogP2, "CNetworkGateway::ProcessNetRequestMessage:");
	LBSLOG2(ELogP2, "Type : %d", aMessage.Type());
			
	switch (aMessage.Type())
		{
		case TLbsNetInternalMsgBase::ELocationResponse:
			{
			const TLbsNetLocationResponseMsg& msg = reinterpret_cast<const TLbsNetLocationResponseMsg&>(aMessage);

			/** LBSLOGGER - Start Logging */
			// -------------------------------------------------------------------------
			// log the GPS location response positioninfo
			if (iExternalLogEvent != NULL)
				{
				TPositionInfoBase* posInfo = const_cast<TPositionInfoBase*>(&msg.PositionInfo());
				if(posInfo->PositionClassType() != EPositionGpsMeasurementInfoClass)
					{
					iExternalLogEvent->SetPositionInfo(posInfo);
					}
				}
			// -------------------------------------------------------------------------
			/** LBSLOGGER - End Logging */
				
			// Determine the original protocol module.
			TSessionTypes sessionType;
			CLbsNetworkProtocolBase* originalPM = FindOriginalPMFromID(TLBSNETSESSIONID(msg.SessionId()), sessionType);
			__ASSERT_DEBUG(sessionType != EInvalidSession,
						   User::Panic(KLbsNGFault, ENGUnexpectedSessionIdOrType));
			if (originalPM)
				{
				LBSLOG(ELogP9, "->A CLbsNetworkProtocolBase::RespondLocationRequest() ProtocolModule\n");
				LBSLOG(ELogP9, "  > TLbsNetLocationResponseMsg msg.SessionId()  = \n");
				LBSLOG_SESSIONID(msg.SessionId());
				LBSLOG2(ELogP9, "  > TInt msg.Reason()  = %d\n", msg.Reason());
				LBSLOG(ELogP9, "  > TPositionInfoBase msg.PositionInfo()  =\n");
				LBSLOG_TPOSITIONINFOBASE(msg.PositionInfo());
				LBS_RDEBUG_ARGINT("LBS", "ProtMod", "RespondLocationRequest", msg.Reason());

				originalPM->RespondLocationRequest(TLBSNETSESSIONID(msg.SessionId()), msg.Reason(), msg.PositionInfo());
				}
			else
				{
				// Shouldn't get here, logic error but gateway does not panic in release mode!
				LBSLOG_ERR(ELogP3, "CNetworkGateway::ProcessNetRequestMessage: For RespondLocationRequest could not find original protocol module");
				__ASSERT_DEBUG(EFalse, User::Panic(KLbsNGFault, ENGUnexpectedSessionIdOrType));
				}
			break;

			}
		case TLbsNetInternalMsgBase::ETransmitLocationRequest:
			{
			const TLbsNetTransmitLocationRequestMsg& msg = reinterpret_cast<const TLbsNetTransmitLocationRequestMsg&>(aMessage);

			/** LBSLOGGER - Start Logging */
			// -------------------------------------------------------------------------
			if (iExternalLogEvent == NULL)
				{
				TRAP_IGNORE(iExternalLogEvent = CLbsExternalLocateLogEvent::NewL(msg.SessionId().SessionOwner(), NULL, NULL));
				}
			// -------------------------------------------------------------------------
			/** LBSLOGGER - End Logging */

            // This message is the first part of what must be an x3p session as far as the gateway is concerned,
            // so store the (ptr to) module.
            iModuleForSession[EX3p] = DefaultPm();

			if (DefaultPm())
				{
				LBSLOG(ELogP9, "->A CLbsNetworkProtocolBase::RequestTransmitLocation() ProtocolModule\n");
				LBSLOG(ELogP9, "  > TLbsNetTransmitLocationRequestMsg msg.SessionId()  = \n");
				LBSLOG_SESSIONID(msg.SessionId());
				LBSLOG2(ELogP9, "  > TDesC msg.Destination()  = %S\n", &msg.Destination());
				LBSLOG2(ELogP9, "  > TInt msg.Priority()  = %d\n", msg.Priority());

				// Request extended module interface
				CLbsNetworkProtocolBase2* networkProtocolExt = 
					reinterpret_cast<CLbsNetworkProtocolBase2*>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
					(DefaultPm()->ExtendedInterface(CLbsNetworkProtocolBase::EExtInterface2,0,0));
#else
					(DefaultPm()->ExtendedInterface(ELbsNetExtInterface2,0,0));
#endif
	
				if (networkProtocolExt != NULL)
					{
					TLbsNetPosRequestOptionsTechnology optionsTech;
		
					ConvertToTLbsNetPosRequestOptionsTechnology(msg.Options(), optionsTech);
					LBS_RDEBUG("LBS", "ProtMod", "RequestTransmitLocation");
					networkProtocolExt->RequestTransmitLocation(
															TLBSNETSESSIONID(msg.SessionId()),
															msg.Destination(),
															msg.Priority(),
															optionsTech);
					}
				else 
					{
					// Fall-back to using old API as the module doesn't support the new one
										LBS_RDEBUG("LBS", "ProtMod", "RequestTransmitLocation");
										DefaultPm()->RequestTransmitLocation(TLBSNETSESSIONID(msg.SessionId()),
															msg.Destination(),
															msg.Priority());
					}
				}
			else
				{
				// No network protocol module to send the request to,
				// so send a 'no network available' error back!
				ProcessSessionComplete(TLBSNETSESSIONID(msg.SessionId()), KErrPositionNetworkUnavailable);
				}
			break;
			}
		case TLbsNetInternalMsgBase::ETransmitLocationCancel:
			{
			const TLbsNetTransmitLocationCancelMsg& msg = reinterpret_cast<const TLbsNetTransmitLocationCancelMsg&>(aMessage);

			// Logging
			iCancelled = ETrue;
			
			// Always transmit cancel to PM dealing with the original request (must have been an x3p session).
			if (iModuleForSession[EX3p])
				{
				LBSLOG(ELogP9, "->A CLbsNetworkProtocolBase::CancelTransmitLocation() ProtocolModule\n");
				LBSLOG(ELogP9, "  > TLbsNetTransmitLocationCancelMsg msg.SessionId()  = \n");
				LBSLOG_SESSIONID(msg.SessionId());
				LBSLOG2(ELogP9, "  > TInt msg.Reason()  = %d\n", msg.Reason());

				LBS_RDEBUG_ARGINT("LBS", "ProtMod", "CancelTransmitLocation", msg.Reason());
				iModuleForSession[EX3p]->CancelTransmitLocation(TLBSNETSESSIONID(msg.SessionId()),
																msg.Reason());
				}
			else
				{
				// Shouldn't get here, logic error but gateway does not panic in release mode!
				LBSLOG_ERR(ELogP3, "CNetworkGateway::ProcessNetRequestMessage: No stored Protocol Module for x3p TransmitLocationCancel msg");
				__ASSERT_DEBUG(EFalse, User::Panic(KLbsNGFault, ENGUnexpectedSessionIdOrType));			
				}

			break;
			}
		case TLbsNetInternalMsgBase::ECancelFromPrivacyController:

 			{
 			const TLbsNetCancelFromPrivacyControllerMsg& msg = reinterpret_cast<const TLbsNetCancelFromPrivacyControllerMsg&>(aMessage);
 
   			/** LBSLOGGER - Start Logging */
   			// -------------------------------------------------------------------------
   			if (iExternalLogEvent == NULL)
   				{
   				TRAP_IGNORE(iExternalLogEvent = CLbsExternalLocateLogEvent::NewL(msg.SessionId().SessionOwner(), NULL, NULL));
   				}
   			// -------------------------------------------------------------------------
   			/** LBSLOGGER - End Logging */
			 				
			// Always transmit cancel to PM dealing with the original request. Must have been MT-LR
			TSessionTypes sessionType;
			CLbsNetworkProtocolBase* originalPM = FindOriginalPMFromID(TLBSNETSESSIONID(msg.SessionId()), sessionType);
			__ASSERT_DEBUG(sessionType == EMtlrHome || sessionType == EMtlrRoaming,
						   User::Panic(KLbsNGFault, ENGUnexpectedSessionIdOrType)); 			 
			if (originalPM)
				{

   				// Request extended module interface
   				CLbsNetworkProtocolBase2* networkProtocolExt = 
   					reinterpret_cast<CLbsNetworkProtocolBase2*>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
   					(originalPM->ExtendedInterface(CLbsNetworkProtocolBase::EExtInterface2,0,0));
#else
   					(originalPM->ExtendedInterface(ELbsNetExtInterface2,0,0));
#endif

   				if (networkProtocolExt != NULL)
   					{
   					LBSLOG(ELogP9, "->A CLbsNetworkProtocolBase::CancelExternalLocation() ProtocolModule\n");
   					LBSLOG(ELogP9, "  > TLbsNetTransmitLocationRequestMsg msg.SessionId()  = \n");
   					LBSLOG_SESSIONID(msg.SessionId());
 					LBSLOG2(ELogP9, "  > TInt msg.Reason()  = %d\n", msg.Reason());

   					// Use extended API
   					networkProtocolExt->CancelExternalLocation(TLBSNETSESSIONID(msg.SessionId()),
   															msg.Reason());
   					}
   				else 
   					{
   					// do nothing if its the old type of protocol module 
   					}
   				}
   			else
   				{
				// Shouldn't get here, logic error but gateway does not panic in release mode!
				LBSLOG_ERR(ELogP3, "CNetworkGateway::ProcessNetRequestMessage: Continuation message but no stored session info");
				__ASSERT_DEBUG(EFalse, User::Panic(KLbsNGFault, ENGUnexpectedSessionIdOrType));

   				// No network protocol module to which we can send the request, so send
   				// a 'no network available' error back. This is probably marginally tidier than not doing so.
   				ProcessSessionComplete(TLBSNETSESSIONID(msg.SessionId()), KErrPositionNetworkUnavailable);

   				}
   			break;
   			}
			
		case TLbsNetInternalMsgBase::EPrivacyResponse:
			{
			const TLbsNetMtLrReponseMsg& msg = reinterpret_cast<const TLbsNetMtLrReponseMsg&>(aMessage);

			// Always transmit RespondPrivacyRequest to PM dealing with the original request (must be MTLR session).
			TSessionTypes sessionType;
			CLbsNetworkProtocolBase* originalPM = FindOriginalPMFromID(TLBSNETSESSIONID(msg.SessionId()), sessionType);
			__ASSERT_DEBUG(sessionType == EMtlrHome || sessionType == EMtlrRoaming,
							User::Panic(KLbsNGFault, ENGUnexpectedSessionIdOrType)); 			 

			if (originalPM)
				{
				LBSLOG(ELogP9, "->A CLbsNetworkProtocolBase::RespondPrivacyRequest() ProtocolModule\n");
				LBSLOG(ELogP9, "  > TLbsNetMtLrReponseMsg msg.SessionId()  = \n");
				LBSLOG_SESSIONID(msg.SessionId());
				LBSLOG2(ELogP9, "  > TLbsNetMtLrReponseMsg msg.Response()  = %d", msg.Response());

				LBS_RDEBUG_ARGINT("LBS", "ProtMod", "RespondPrivacyRequest", msg.Response());

   				CLbsNetworkProtocolBase2* networkProtocolExt = 
   					reinterpret_cast<CLbsNetworkProtocolBase2*>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
   					(originalPM->ExtendedInterface(CLbsNetworkProtocolBase::EExtInterface2,0,0));
#else
   					(originalPM->ExtendedInterface(ELbsNetExtInterface2,0,0));
#endif

							
				if (networkProtocolExt != NULL)
					{
					const CLbsNetworkProtocolBase::TLbsPrivacyResponse resp = (const CLbsNetworkProtocolBase::TLbsPrivacyResponse&) msg.Response();
												
					networkProtocolExt->RespondPrivacyRequest(TLBSNETSESSIONID(msg.SessionId()), resp, msg.Reason());
					}
				else 
					{
					originalPM->RespondPrivacyRequest(TLBSNETSESSIONID(msg.SessionId()), (const CLbsNetworkProtocolBase::TLbsPrivacyResponse&) msg.Response());
					}
				}
			else
				{
				// Shouldn't get here, logic error but gateway does not panic in release mode!
				LBSLOG_ERR(ELogP1, "CNetworkGateway::ProcessNetRequestMessage: No stored Protocol Module for Privacy Response msg");
				__ASSERT_DEBUG(EFalse, User::Panic(KLbsNGFault, ENGUnexpectedSessionIdOrType));
				}
												    
			/** LBSLOGGER - Start Logging */
			// -------------------------------------------------------------------------
			if (iExternalLogEvent == NULL)
				{
				TRAP_IGNORE(iExternalLogEvent = CLbsExternalLocateLogEvent::NewL(msg.SessionId().SessionOwner(), NULL, NULL));
				}
			if (iExternalLogEvent != NULL)
				{
				TLbsLoggingPrivacyResponseParams params;
				params.iSessionId.SetSessionOwner(msg.SessionId().SessionOwner());
				params.iSessionId.SetSessionNum(msg.SessionId().SessionNum());
				params.iResult = static_cast<CLbsNetworkProtocolBase::TLbsPrivacyResponse>(msg.Response());
				iExternalLogEvent->SetPrivacyResponseParams(params);
				}
			// -------------------------------------------------------------------------
			/** LBSLOGGER - End Logging */
												    
			break;
			}
		default:
			{
			break;
			}
		}
	}

//---------------------------------------------------------------------------------------------------
// Messages from the Network Registration Status Monitor (MNetworkRegistrationStatusObserver)
//---------------------------------------------------------------------------------------------------
void CNetworkGateway::OnNetworkRegistrationStatusChange(
		RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus aStatus)
	{
	LBSLOG(ELogP2, "CNetworkGateway::OnNetworkRegistrationStatusChange:");
	LBSLOG2(ELogP2, "Status : %d", aStatus);
	
	iRegistrationStatus = aStatus;
	
	// If we're loading by status and there is already one set of protocol
	//  modules loaded do not allow another set to be loaded.
	if( iLoadInfo.LoadingStrategy() == TLbsAdminProtocolModuleLoadingInfo::EProtocolModuleLoadingByStatus )
		{
		if( aStatus == RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork && ModuleInfo(iRoamingPmUid) )
		    {
		    if( ModuleInfo(iRoamingPmUid)->ProtocolModule() )
		        {
		        return;
		        }
		    }
		else if( aStatus == RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork && ModuleInfo(iHomePmUid) )
			{
			if( ModuleInfo(iHomePmUid)->ProtocolModule() )
			    {
                return;
			    }
			}
		}

	switch (aStatus)
		{
		case RLbsNetworkRegistrationStatus::ENotRegistered:
			{
			// Handled by iRegistrationStatus being set to ENotRegistered
			break;
			}

		case RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork:
			{
			for(TUint index = 0; index < iPmCount; index++)
				{
				if( iModules[index]->Home() && (!iModules[index]->ProtocolModule()) )
					{
					TRAPD(err, LoadProtocolModuleL(iModules[index]->ModuleUid()));
					if (err != KErrNone)
						{
						LBSLOG_ERR2(ELogP3, "Error creating protocol module: %d", err);
						}
					}
				}
			break;
			}
		case RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork:
			{
			for(TUint index = 0; index < iPmCount; index++)
				{
				if( iModules[index]->Roaming() && (!iModules[index]->ProtocolModule()) )
					{
					TRAPD(err, LoadProtocolModuleL(iModules[index]->ModuleUid()));
					if (err != KErrNone)
						{
						LBSLOG_ERR2(ELogP3, "Error creating protocol module: %d", err);
						}
					}
				}
			break;
			}
		
		// Should never be at Registration Unknown, nor any state not dealt with above...
		case RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown:
		default:
			{
			// Programming error we are not going to recover from..
			User::Panic(KLbsNGFault, ENGUnexpectedMsgType);
			}
		}
	}

//---------------------------------------------------------------------------------------------------
// Other, private.
//---------------------------------------------------------------------------------------------------

/* Observer that is called when LbsRoot has requested the Network Gateway to shutdown.
*/
void CNetworkGateway::OnProcessCloseDown()
	{
	// For now, simply close down as soon as possible.
	// In future the close down may need to be asynchronous,
	// to allow the buffers in CAgpsChannel and CNetRequestChannel
	// to be processed normally.
	CActiveScheduler::Stop();
	}

/* Called when LbsRoot wants the Lbs system to reset the internal state
   of data buffers, etc. For the Network Gateway, the main thing is to 
   set 'invalid' assistance data in the interface to the agps manager.
*/
void CNetworkGateway::OnProcessReset()
	{
	// Send a set of 'invalid' assistance data to the AGPS manager
	TRAP_IGNORE(ResetAssistanceDataBufferL());
	}

/* Reset the assistance data channel by sending invalid
   assistance data to it.
*/
void CNetworkGateway::ResetAssistanceDataBufferL()
	{
	if(iAgpsChannel)
		{
		TLbsAsistanceDataGroup dataMask = EAssistanceDataAquisitionAssistance |
										  EAssistanceDataBadSatList |
										  EAssistanceDataNavigationModel |
										  EAssistanceDataReferenceTime |
										  EAssistanceDataIonosphericModel |
										  EAssistanceDataDgpsCorrections |
										  EAssistanceDataReferenceLocation |
										  EAssistanceDataAlmanac |
										  EAssistanceDataPositioningGpsUtcModel;
		
		RLbsAssistanceDataBuilderSet builderSet;
		CleanupClosePushL(builderSet);
		builderSet.OpenL();
		
		TTime invalidTimeStamp(0);
		
		RUEPositioningGpsAlmanacBuilder* almanacPtr;
		User::LeaveIfError(builderSet.GetDataBuilder(almanacPtr));
		almanacPtr->SetTimeStamp(invalidTimeStamp);
			
		RUEPositioningGpsIonosphericModelBuilder* ionosphericPtr;
		User::LeaveIfError(builderSet.GetDataBuilder(ionosphericPtr));
		ionosphericPtr->SetTimeStamp(invalidTimeStamp);
		
		RUEPositioningGpsNavigationModelBuilder* navigationPtr;
		User::LeaveIfError(builderSet.GetDataBuilder(navigationPtr));
		navigationPtr->SetTimeStamp(invalidTimeStamp);
		
		RUEPositioningGpsReferenceTimeBuilder* referenceTimePtr;
		User::LeaveIfError(builderSet.GetDataBuilder(referenceTimePtr));
		referenceTimePtr->SetTimeStamp(invalidTimeStamp);
		
		RUEPositioningGpsUtcModelBuilder*  utcPtr;
		User::LeaveIfError(builderSet.GetDataBuilder(utcPtr));
		utcPtr->SetTimeStamp(invalidTimeStamp);
		
		RUEPositioningGpsAcquisitionAssistanceBuilder* acquisitionPtr;
		User::LeaveIfError(builderSet.GetDataBuilder(acquisitionPtr));
		acquisitionPtr->SetTimeStamp(invalidTimeStamp);
		
		RBadSatListBuilder* badSatPtr;
		User::LeaveIfError(builderSet.GetDataBuilder(badSatPtr));
		badSatPtr->SetTimeStamp(invalidTimeStamp);
		
		RReferenceLocationBuilder* referenceLocationPtr;
		User::LeaveIfError(builderSet.GetDataBuilder(referenceLocationPtr));
		referenceLocationPtr->SetTimeStamp(invalidTimeStamp);
	
		// Send this invalid data to the assistance data channel	
		iAgpsChannel->SendAssistanceDataResponse(KErrNotReady,
											 dataMask,
									 		 builderSet);
	
		CleanupStack::PopAndDestroy(&builderSet);
		}
	}

/** Get the preferred GPS mode to use based on admin settings.

There are separate hoem and roaming settings for the preferred GPS mode to use,
so need to check the current network registration status to read the correct
setting. If the status is not home or roaming (i.e. unregistered), use the
roaming setting instead.
*/
CLbsAdmin::TGpsMode CNetworkGateway::GetAdminGpsMode() const
	{
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus netRegStatus(RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown);
	netRegStatus = iNetworkRegistrationStatusMonitor->GetCurrentNetworkRegistrationStatus();
	if (netRegStatus == RLbsNetworkRegistrationStatus::ENotRegistered)
		{
		// Unregistered status may just be temporary, so use the last good
		// registration status instead of the current status.
		netRegStatus = iNetworkRegistrationStatusMonitor->GetLastGoodNetworkRegistrationStatus();
		}

	TLbsAdminSetting setting(KLbsSettingHomeGpsMode);
	switch (netRegStatus)
		{
		case RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork:
			{
			setting = KLbsSettingHomeGpsMode;
			break;
			}
		case RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork:
		default:
			{
			setting = KLbsSettingRoamingGpsMode;
			break;
			}
		}
	
	CLbsAdmin::TGpsMode adminGpsMode(CLbsAdmin::EGpsModeUnknown);
	TInt err = iAdmin->Get(setting, adminGpsMode);

	// If our call to find admin setting was broken, use most likely setting...
	if (err != KErrNone)
		{
		LBSLOG_ERR2(ELogP2, "Error %d with get of Gps Admin Mode\n", err); 
		adminGpsMode = CLbsAdmin::EGpsPreferTerminalBased;
		}

	return adminGpsMode;
	}

CLbsNetworkProtocolBase* CNetworkGateway::DefaultPm()
	{
	if(iRegistrationStatus == RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork)
		return ModuleInfo(iHomePmUid)->ProtocolModule();
	else if(iRegistrationStatus == RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork)
		return ModuleInfo(iRoamingPmUid)->ProtocolModule();
	
	return NULL;
	}

TBool CNetworkGateway::ValidPm(TUid aPmUid)
	{
	if(ModuleInfo(aPmUid))
		{
		if((ModuleInfo(aPmUid))->ProtocolModule())
			{
			if( (iRegistrationStatus == RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork && (ModuleInfo(aPmUid))->Home()) ||
				(iRegistrationStatus == RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork && (ModuleInfo(aPmUid))->Roaming()) )
				{
				return ETrue;
				}
			}
		}
		
	return EFalse;
	}

CLbsNetworkProtocolBase* CNetworkGateway::FindOriginalPMFromID(const TLbsNetSessionId& aId, TSessionTypes& aType)
	{
	TUid id = aId.SessionOwner();

	if (ModuleInfo(id)) // If the request was from a protocol module
		{
		aType = ((ModuleInfo(id))->Home()) ? EMtlrHome : EMtlrRoaming; 
		return  (ModuleInfo(id))->ProtocolModule();
		}
	else if (id == KLbsNetLocManagerUid)
        {
        aType = ENetworkLocation;
        return iModuleForSession[ENetworkLocation];
        }
    else if (id == KLbsSuplPsyUid)
        {
        aType = EPSYLocation;
        return iModuleForSession[EPSYLocation];
        }
    else if (id == KLbsNetRequestHandlerUid)
        {
        aType = EX3p;
        return iModuleForSession[EX3p];
        }
    else if (id == KLbsGpsLocManagerUid)
        {
        aType = ESelfLocation;
        return iModuleForSession[ESelfLocation];
        }
    else
        {
        aType = EInvalidSession;
        return DefaultPm();
        }
	}

CProtocolModuleInfo* CNetworkGateway::ModuleInfo(TLbsProtocolModuleId aUid)
	{
	for(TUint index = 0; index < iPmCount; index++)
		{
		if(iModules[index]->ModuleUid() == aUid)
			{
			return iModules[index];
			}
		}
	
	return NULL; 
	}

void CNetworkGateway::DeleteLoadedModules()
	{
	for(TUint index = 0; index < iPmCount; index++)
		{
		delete(iModules[index]);
		}
	iPmCount = 0;
	}

CProtocolModuleInfo* CProtocolModuleInfo::NewL(TLbsProtocolModuleId aModuleId, CNetObserver* aObs, CLbsNetworkProtocolBase* aNetworkProtocol, TBool aHome, TBool aRoaming)
	{
	CProtocolModuleInfo* self = new (ELeave) CProtocolModuleInfo(aModuleId, aObs, aNetworkProtocol, aHome, aRoaming);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CProtocolModuleInfo::~CProtocolModuleInfo()
	{
	delete iObs;
	delete iNetworkProtocol;
	}

CProtocolModuleInfo::CProtocolModuleInfo(TLbsProtocolModuleId aModuleId, CNetObserver* aObs, CLbsNetworkProtocolBase* aNetworkProtocol, TBool aHome, TBool aRoaming):
	iModuleId(aModuleId),
	iObs(aObs),
	iNetworkProtocol(aNetworkProtocol),
	iHome(aHome),
	iRoaming(aRoaming)
	{
	}

void CProtocolModuleInfo::ConstructL()
	{
	}

TLbsProtocolModuleId CProtocolModuleInfo::ModuleUid()
	{
	return iModuleId;
	}

void CProtocolModuleInfo::SetObserver(CNetObserver* aObserver)
	{
	iObs = aObserver;
	}

CNetObserver* CProtocolModuleInfo::Observer()
	{
	return iObs;
	}

void CProtocolModuleInfo::SetProtocolModule(CLbsNetworkProtocolBase* aProtocolModule)
	{
	iNetworkProtocol = aProtocolModule;
	}

CLbsNetworkProtocolBase* CProtocolModuleInfo::ProtocolModule()
	{
	return iNetworkProtocol;
	}

void CProtocolModuleInfo::SetHome()
	{
	iHome = ETrue;
	}

TBool CProtocolModuleInfo::Home()
	{
	return iHome;
	}
	
void CProtocolModuleInfo::SetRoaming()
	{
	iRoaming = ETrue;
	}

TBool CProtocolModuleInfo::Roaming()
	{
	return iRoaming;
	}
void CNetworkGateway::ConvertToTLbsNetPosRequestQualityInt(
			const TLbsNetPosRequestQuality& aSource,
			TLbsNetPosRequestQualityInt& aDest) const
	
	{
	aDest.SetMaxFixTime(aSource.MaxFixTime());
	aDest.SetMaxFixAge(aSource.MaxFixAge());
	aDest.SetMinHorizontalAccuracy(aSource.MinHorizontalAccuracy());
	aDest.SetMinVerticalAccuracy(aSource.MinVerticalAccuracy());
	}
	
void CNetworkGateway::ConvertToTLbsNetPosRequestQuality(
			const TLbsNetPosRequestQualityInt& aSource,
			TLbsNetPosRequestQuality& aDest) const
	{
	aDest.SetMaxFixTime(aSource.MaxFixTime());
	aDest.SetMaxFixAge(aSource.MaxFixAge());
	aDest.SetMinHorizontalAccuracy(aSource.MinHorizontalAccuracy());
	aDest.SetMinVerticalAccuracy(aSource.MinVerticalAccuracy());
	}

void CNetworkGateway::ConvertToTLbsNetPosRequestOptions(
			const TLbsNetPosRequestOptionsInt& aSource,
			TLbsNetPosRequestOptions& aDest) const
	{
	aDest.SetNewClientConnected(aSource.NewClientConnected());
	
	TLbsNetPosRequestQualityInt qualityInt;
	aSource.GetRequestQuality(qualityInt);
	TLbsNetPosRequestQuality quality;
	ConvertToTLbsNetPosRequestQuality(qualityInt, quality);
	aDest.SetRequestQuality(quality);
	}

void CNetworkGateway::ConvertToTLbsNetPosRequestOptionsAssistance(
			const TLbsNetPosRequestOptionsAssistanceInt& aSource,
			TLbsNetPosRequestOptionsAssistance& aDest) const
	{
	aDest.SetNewClientConnected(aSource.NewClientConnected());
	
	TLbsNetPosRequestQualityInt qualityInt;
	aSource.GetRequestQuality(qualityInt);
	TLbsNetPosRequestQuality quality;
	ConvertToTLbsNetPosRequestQuality(qualityInt, quality);
	aDest.SetRequestQuality(quality);
	
	aDest.SetDataRequestMask((TLbsAsistanceDataGroupInt) aSource.DataRequestMask());
	aDest.SetPosMode((TPositionModuleInfo::TTechnologyType) aSource.PosMode());	
	}

void CNetworkGateway::ConvertToTLbsNetPosRequestOptionsTechnology(
			const TLbsNetPosRequestOptionsTechnologyInt& aSource,
			TLbsNetPosRequestOptionsTechnology& aDest) const
	{
	aDest.SetNewClientConnected(aSource.NewClientConnected());
	
	TLbsNetPosRequestQualityInt qualityInt;
	aSource.GetRequestQuality(qualityInt);
	TLbsNetPosRequestQuality quality;
	ConvertToTLbsNetPosRequestQuality(qualityInt, quality);
	aDest.SetRequestQuality(quality);
	aDest.SetPosMode(aSource.PosMode());
	}

