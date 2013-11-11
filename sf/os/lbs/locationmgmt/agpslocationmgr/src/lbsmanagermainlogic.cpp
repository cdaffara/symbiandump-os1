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

//************************************************************************************************************
// System
#include <bacline.h>
#include <e32cons.h>
#include <ecom/ecom.h> 

// Project
#include "lbsmanagermainlogic.h"
#include "lbstimer.h"
#include <lbs.h>
#include <lbs/lbsassistancedatabase.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include "LbsInternalInterface.h"
#include "lbsnetworkgatewayhandler.h"
#include "lbsautoclockadjust.h"
#include "lbsprocessuiddefs.h"
#include "utilfunctions.h"
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsgpsmeasurement.h>
#include "lbsdevloggermacros.h"
#include <lbs/lbslocerrors.h>
#include <lbs/lbsgpsmeasurement.h>
#include "lbsqualityprofile.h"
#include <lbs/lbslocdatasourceclasstypes.h>
#include <lbs/epos_intgpshwstatus.h>
#include "lbscommoninternaldatatypes.h"
#include "lbsreffnpint.h"
#include "lbsnetregstatusint.h"

#ifdef AGPS_MODULE_TESTING
#include "lbslocsourcegps.h"
#endif

#ifdef AGPS_MANAGER_TESTING
#include "ctestgpsmodule.h"
#endif

class RLbsPositionUpdates;

const TInt64 KTrackingOffTimeout = 30000000;//30s

const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierLS = 
	{
		{KLbsGpsLocManagerUidValue},{KLbsLocServerUidValue}
	};
const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierNRH = 
	{
		{KLbsGpsLocManagerUidValue},{KLbsNetRequestHandlerUidValue}
	};
	

#include "LbsAssistanceDataLogEvent.h"

const TInt KLbsLogQueueSize = 10;


// Definitions of constants to represent positioning methods (definitions are local
// to this file, not LBS-wide)
static const TPositionModuleInfo::TTechnologyType KLbsMethodNone =
												TPositionModuleInfo::ETechnologyUnknown;
												
static const TPositionModuleInfo::TTechnologyType KLbsMethodAutonomous =
												TPositionModuleInfo::ETechnologyTerminal;
												
static const TPositionModuleInfo::TTechnologyType KLbsMethodTerminalBased =
											 	TPositionModuleInfo::ETechnologyTerminal |
									 	    	TPositionModuleInfo::ETechnologyAssisted;
									 	    	
static const TPositionModuleInfo::TTechnologyType KLbsMethodTerminalAssisted = 
												TPositionModuleInfo::ETechnologyNetwork |
										        TPositionModuleInfo::ETechnologyAssisted;
										        										        
static const TPositionModuleInfo::TTechnologyType KLbsMethodDual = 
											    TPositionModuleInfo::ETechnologyNetwork |
												TPositionModuleInfo::ETechnologyTerminal |
												TPositionModuleInfo::ETechnologyAssisted;

// Gps Options Combination Table entry
struct TLbsGpsCombinedModeTableEntry
	{
	TPositionModuleInfo::TTechnologyType iLSPreferredMode;
	TPositionModuleInfo::TTechnologyType iLSAlternativeMode;
	TPositionModuleInfo::TTechnologyType iNRHPreferredMode;
	TPositionModuleInfo::TTechnologyType iNRHAlternativeMode;
	TPositionModuleInfo::TTechnologyType iResultingMode;
	};

// Module supports Autonomous only.
static const TLbsGpsCombinedModeTableEntry KLbsAutonomousTable[] = 
{
//LS Preferred				//LS Altenative		//NRH Preferred				//NRH Alternative		// Result
{KLbsMethodAutonomous, 		KLbsMethodNone, 	KLbsMethodAutonomous,		KLbsMethodNone,		KLbsMethodAutonomous}
};
static const TInt KLbsAutonomousTableCount(sizeof(KLbsAutonomousTable) / sizeof(TLbsGpsCombinedModeTableEntry));

// Module supports Terminal-Based only.
static const TLbsGpsCombinedModeTableEntry KLbsTBTable[] = 
{
//LS Preferred				//LS Altenative		//NRH Preferred				//NRH Alternative		// Result
{KLbsMethodAutonomous, 		KLbsMethodNone, 	KLbsMethodAutonomous,		KLbsMethodNone,		KLbsMethodAutonomous},
{KLbsMethodAutonomous,		KLbsMethodNone,		KLbsMethodTerminalBased,	KLbsMethodNone,	    KLbsMethodTerminalBased},

{KLbsMethodTerminalBased, 	KLbsMethodNone, 	KLbsMethodAutonomous, 		KLbsMethodNone,		KLbsMethodTerminalBased},
{KLbsMethodTerminalBased, 	KLbsMethodNone, 	KLbsMethodTerminalBased, 	KLbsMethodNone,		KLbsMethodTerminalBased},
};
static const TInt KLbsTBTableCount(sizeof(KLbsTBTable) / sizeof(TLbsGpsCombinedModeTableEntry));

// Module supports Terminal-Assisted only.
static const TLbsGpsCombinedModeTableEntry KLbsTATable[] = 
{
//LS Preferred				//LS Altenative		//NRH Preferred					// NRH Alternative	// Result
{KLbsMethodTerminalAssisted, KLbsMethodNone, 	KLbsMethodTerminalAssisted,		KLbsMethodNone,		KLbsMethodTerminalAssisted},
};
static const TInt KLbsTATableCount(sizeof(KLbsTATable) / sizeof(TLbsGpsCombinedModeTableEntry));

// Module supports either Terminal-Assisted or Terminal-Based (but not both at the same time).
static const TLbsGpsCombinedModeTableEntry KLbsTBOrTATable[] = 
{
//LS Preferred					//LS Altenative		//NRH Preferred				//NRH Alternative		// Result
{KLbsMethodAutonomous, 	    	KLbsMethodNone, 	KLbsMethodAutonomous,		KLbsMethodNone,		KLbsMethodAutonomous},
{KLbsMethodAutonomous,			KLbsMethodNone,		KLbsMethodTerminalBased,	KLbsMethodNone,	    KLbsMethodTerminalBased},
{KLbsMethodAutonomous,			KLbsMethodNone,		KLbsMethodTerminalAssisted,	KLbsMethodNone,	    KLbsMethodTerminalAssisted},

{KLbsMethodTerminalBased, 		KLbsMethodNone, 	KLbsMethodAutonomous, 		KLbsMethodNone,		KLbsMethodTerminalBased},
{KLbsMethodTerminalBased,   	KLbsMethodNone, 	KLbsMethodTerminalBased, 	KLbsMethodNone,		KLbsMethodTerminalBased},
{KLbsMethodTerminalBased, 		KLbsMethodNone, 	KLbsMethodTerminalAssisted, KLbsMethodNone,		KLbsMethodTerminalAssisted},

{KLbsMethodTerminalAssisted,	KLbsMethodNone, 	KLbsMethodAutonomous, 		KLbsMethodNone,		KLbsMethodAutonomous},
{KLbsMethodTerminalAssisted,	KLbsMethodNone,		KLbsMethodTerminalBased,	KLbsMethodNone,	    KLbsMethodTerminalBased},
{KLbsMethodTerminalAssisted,	KLbsMethodNone, 	KLbsMethodTerminalAssisted, KLbsMethodNone,		KLbsMethodTerminalAssisted},
};
static const TInt KLbsTBOrTATableCount(sizeof(KLbsTBOrTATable) / sizeof(TLbsGpsCombinedModeTableEntry));

// Module supports Terminal-Assisted and Terminal-Base simultaneously.
static const TLbsGpsCombinedModeTableEntry KLbsTBAndTATable[] = 
{
//LS Preferred					//LS Altenative			//NRH Preferred				//NRH Alternative			// Result
{KLbsMethodAutonomous, 	    	KLbsMethodNone, 		KLbsMethodAutonomous,		KLbsMethodNone,				KLbsMethodAutonomous},
{KLbsMethodAutonomous,			KLbsMethodNone,			KLbsMethodTerminalBased,	KLbsMethodNone,	    		KLbsMethodTerminalBased},
{KLbsMethodAutonomous, 	    	KLbsMethodNone, 		KLbsMethodTerminalAssisted,	KLbsMethodNone,				KLbsMethodDual},
{KLbsMethodAutonomous,			KLbsMethodNone,			KLbsMethodTerminalAssisted,	KLbsMethodAutonomous,		KLbsMethodDual},
{KLbsMethodAutonomous,			KLbsMethodNone,			KLbsMethodAutonomous,		KLbsMethodTerminalAssisted,	KLbsMethodDual},
{KLbsMethodAutonomous, 	    	KLbsMethodNone, 		KLbsMethodTerminalBased,	KLbsMethodTerminalAssisted,	KLbsMethodDual},
{KLbsMethodAutonomous, 	    	KLbsMethodNone, 		KLbsMethodTerminalAssisted,	KLbsMethodTerminalBased,	KLbsMethodDual},


{KLbsMethodTerminalBased, 		KLbsMethodNone, 		KLbsMethodAutonomous, 		KLbsMethodNone,				KLbsMethodTerminalBased},
{KLbsMethodTerminalBased,   	KLbsMethodNone, 		KLbsMethodTerminalBased, 	KLbsMethodNone,				KLbsMethodTerminalBased},
{KLbsMethodTerminalBased, 		KLbsMethodNone, 		KLbsMethodTerminalAssisted, KLbsMethodNone,				KLbsMethodDual},
{KLbsMethodTerminalBased,   	KLbsMethodNone, 		KLbsMethodTerminalAssisted, KLbsMethodAutonomous,		KLbsMethodDual},
{KLbsMethodTerminalBased,   	KLbsMethodNone, 		KLbsMethodAutonomous,		KLbsMethodTerminalAssisted,	KLbsMethodDual},
{KLbsMethodTerminalBased,   	KLbsMethodNone, 		KLbsMethodTerminalBased, 	KLbsMethodTerminalAssisted,	KLbsMethodDual},
{KLbsMethodTerminalBased,   	KLbsMethodNone, 		KLbsMethodTerminalAssisted, KLbsMethodTerminalBased	,	KLbsMethodDual},

{KLbsMethodTerminalAssisted, 	KLbsMethodNone, 		KLbsMethodAutonomous, 		KLbsMethodNone,				KLbsMethodDual},
{KLbsMethodTerminalAssisted,   	KLbsMethodNone, 		KLbsMethodTerminalBased, 	KLbsMethodNone,				KLbsMethodDual},
{KLbsMethodTerminalAssisted, 	KLbsMethodNone, 		KLbsMethodTerminalAssisted, KLbsMethodNone,				KLbsMethodTerminalAssisted},
{KLbsMethodTerminalAssisted,   	KLbsMethodNone, 		KLbsMethodTerminalAssisted, KLbsMethodAutonomous,		KLbsMethodDual},
{KLbsMethodTerminalAssisted,   	KLbsMethodNone, 		KLbsMethodAutonomous, 		KLbsMethodTerminalAssisted,	KLbsMethodDual},
{KLbsMethodTerminalAssisted,   	KLbsMethodNone, 		KLbsMethodTerminalBased, 	KLbsMethodTerminalAssisted,	KLbsMethodDual},
{KLbsMethodTerminalAssisted,   	KLbsMethodNone, 		KLbsMethodTerminalAssisted, KLbsMethodTerminalBased,	KLbsMethodDual},
};
static const TInt KLbsTBAndTATableCount(sizeof(KLbsTBAndTATable)/sizeof(TLbsGpsCombinedModeTableEntry));

//************************************************************************************************************
//	CManagerMainLogic
//************************************************************************************************************

#define __ASSERT_ALWAYSX(c,p) (void)((c)||(RDebug::Printf("Assert at line %d in file %s ",__LINE__,__FILE__),p,0));

CManagerMainLogic* CManagerMainLogic::NewL()
	{
	LBSLOG(ELogP1, "CManagerMainLogic::NewL() Begin\n");
	CManagerMainLogic* self = new(ELeave) CManagerMainLogic;
	CleanupStack::PushL(self);
	self->ConstructL();
	
	CleanupStack::Pop(self);
	LBSLOG(ELogP1, "CManagerMainLogic::NewL() End\n");
	return self;
	}

//------------------------------------------------------------------------------------------------------------
CManagerMainLogic::CManagerMainLogic() :
	iClosingDown(EFalse),
	iLSReqState(EReqNone),
	iSessionId(KLbsGpsLocManagerUid, 0),
	iLastTrackingFlagSentToNG(EFalse),
	iTrackingStateUnknown(ETrue),
	iPowerModeLS(CLbsLocationSourceGpsBase::EPowerModeUnknown),
	iPowerModeNRH(CLbsLocationSourceGpsBase::EPowerModeUnknown)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::CManagerMainLogic()\n");
	}

//------------------------------------------------------------------------------------------------------------
void CManagerMainLogic::ConstructL()
	{
	LBSLOG(ELogP1, "CManagerMainLogic::ConstructL() Begin\n");
	iQuietus = CQuietus::NewL();
		
    // Create the monitor which detects a closedown signal from
    // the LBS Root Process.
    iCloseDownRequestDetector = CLbsCloseDownRequestDetector::NewL(this, KLbsGpsLocManagerUid);

    iPositionUpdates.OpenL(KLbsGpsLocManagerUid);
    iMeasurementUpdates.OpenL();
    iModuleStatus.OpenL(KLbsGpsLocManagerUid);

    iNetworkGatewayHandler = CNetworkGatewayHandler::NewL(this);

    iNRHLocationRequestHandler = CLocationRequestHandler::NewL(*this,KChannelIdentifierNRH);
    iLocSvrLocationRequestHandler = CLocationRequestHandler::NewL(*this,KChannelIdentifierLS);
    
    iEarlyCompletionUpdateHandler = CEarlyCompletionUpdateHandler::NewL(this);
    iEarlyCompletionUpdateHandler->ListenForEarlyCompletionUpdates();
    
    TPositionModuleStatus moduleStatus;
    // set initial dynamic module status and tell interested parties
    moduleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceInactive);
    moduleStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityUnknown);

    TPositionModuleStatusEventBase::TModuleEvent occurredEventsSinceLastSet = 
                    TPositionModuleStatusEventBase::EEventDeviceStatus | TPositionModuleStatusEventBase::EEventDataQualityStatus;
    iModuleStatus.SetModuleStatus(&moduleStatus,sizeof(moduleStatus),occurredEventsSinceLastSet);
    
    LBSLOG(ELogP9, "->S CLbsLocationSourceGpsBase::NewL() AGPSModule\n");
    LBSLOG2(ELogP9, "  > CManagerMainLogic this  = 0x%08X\n", this);

	iPosIntGpsHwStatus = CPosIntGpsHwStatus::NewL();
	
#ifdef 	AGPS_MODULE_TESTING
    // ECOM plug-in not used in unit test
    iLocationSource = CLbsLocationSourceGps::NewL(*this);
    
    // When module testing there is no root process so the device capabilities
    // are not available from LbsModuleInfo
    iCapabilities = TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB;
		
#elif defined(AGPS_MANAGER_TESTING)
    // ECOM plug-in not used in unit test
    iLocationSource = CTestGpsModule::NewL(*this);

    // When unit-testing there is no root process so the device capabilities
    // are not available from LbsModuleInfo...read capabilities from the 
    // corresponding test property
    TInt capabilities = 0;
    TUid KSuiteExeUid = {0x102869CB};
    TUint KGpsModeCapabilities = 0;
    TInt err = RProperty::Get(KSuiteExeUid, KGpsModeCapabilities, capabilities);
    ASSERT(err == KErrNone);
    iCapabilities = capabilities;		
#else
    // Instantiate Ecom plugin
    TUid dataSourceId;
    TInt result = LbsModuleInfo::GetDataSourceId(KLbsGpsLocManagerUid, dataSourceId);
    LBSLOG2(ELogP9, "  > TUid dataSourceId  = 0x%08X\n", dataSourceId.iUid);
    iLocationSource = CLbsLocationSourceGpsBase::NewL(*this, dataSourceId);

    // Get capabilities from  module info
    User::LeaveIfError(LbsModuleInfo::GetDeviceCapabilities(KLbsGpsLocManagerUid, iCapabilities));
#endif
		
    iAutoClockAdjust = CAutoClockAdjust::NewL();
    
    // Used to send Tracking Off if the Loc Server doesn't keep sending location requests.
    iTimer = CLbsCallbackTimer::NewL(*this);
    
    // Disable tracking if no messages received from the Loc Server before timeout
    RestartTimerIfTracking();
    
    // Open logger
    iLogger.Open(KLbsLogQueueSize);
    
    iSessionClosureData.iNotify = EFalse;		

    // Read the admin setting for the lag timer
    iAdmin = CLbsAdmin::NewL();
    GetFinalNetPositionLagFromAdminProfileL();
    
    iNetRegStatus.OpenL();
		
		#if defined(_DEBUG)
			// For OOM testing. The listener will force an error on the next heap 
			// allocation when it is kicked by test code.
			iOomListener = CLbsOomListener::NewL();
			iOomListener->StartGettingRequests();
		#endif  
			
	
		    
	LBSLOG(ELogP1, "CManagerMainLogic::ConstructL() End\n");
	}


//------------------------------------------------------------------------------------------------------------
CManagerMainLogic::~CManagerMainLogic()
	{
	LBSLOG(ELogP1, "CManagerMainLogic::~CManagerMainLogic() Begin\n");
	
	LBSLOG(ELogP9, "->S CLbsLocationSourceGpsBase::~CLbsLocationSourceGpsBase() AGPSModule\n");
	delete iLocationSource;
#ifndef 	AGPS_MODULE_TESTING
	REComSession::FinalClose();
#endif
	
	delete iPosIntGpsHwStatus;
	delete iCloseDownRequestDetector;
	
	iLocationSource = NULL;
	
	delete iQuietus;
	iQuietus = NULL;

	iLocSvrLocationRequestHandler->Cancel();
	delete iLocSvrLocationRequestHandler;
	iLocSvrLocationRequestHandler = NULL;
	
	
	iEarlyCompletionUpdateHandler->Cancel();
	delete iEarlyCompletionUpdateHandler;
	iEarlyCompletionUpdateHandler = NULL;
	
	iNetworkGatewayHandler->Cancel();
	delete iNetworkGatewayHandler; 
	iNetworkGatewayHandler = NULL;
	
	iNRHLocationRequestHandler->Cancel();
	delete iNRHLocationRequestHandler;
	iNRHLocationRequestHandler = NULL;
	
	delete iAutoClockAdjust;
	iAutoClockAdjust = NULL;
	
	if (iTimer)	
		{
	iTimer->Cancel();
		}

	delete iTimer;
    delete iAdmin;
    
	// Close logger
	iLogger.Close();
    #if defined(_DEBUG)
		if(iOomListener)
			{
			iOomListener->Cancel();
			delete iOomListener;
			}
	#endif 
	
	LBSLOG(ELogP1, "CManagerMainLogic::~CManagerMainLogic() End\n");
	}


void CManagerMainLogic::GetFinalNetPositionLagFromAdminProfileL()
	{

	// Get the profile Id for self locate requests
	TLbsQualityProfileId profileId;
	TInt err = iAdmin->Get(KLbsSettingQualityProfileSelfLocate, profileId);
	if (err == KErrNone)
		{
		// Retrieve the quality profile that belongs to the given Id
		TQualityProfile quality;
		err = LbsQualityProfile::GetQualityProfileById(profileId, quality);
		if (err == KErrNone)
			{
			iFinalNetPositionLag = quality.FinalNetPositionLag();
			}
		else
			{
			iFinalNetPositionLag = 0;
			}
		}
	
	}
	
/*
 * Publish a dummy position on the position bus for the Loc Server, 
 * note that the NRH will ignore this update since the 'conflict control' flag (overloaded for this purpose) is set to TRUE 
 */
void CManagerMainLogic::PublishDummyPosition(TInt aReason)
	{
	TPositionExtendedSatelliteInfo dummySatInfo;
	TTime dummyTime = 0;
	iPositionUpdates.SetPositionInfo(aReason, ETrue, &dummySatInfo, sizeof(TPositionExtendedSatelliteInfo), dummyTime, dummyTime);
	}

/*
 * Publish a dummy Assistance Data Event with the given error, for the integration module.
 * Note: we use 0xFFFFFFFF when informing the module of an error
 */
void CManagerMainLogic::PublishDummyAssistDataEvent(TInt aError)
	{
	iLocationSource->AssistanceDataEvent(aError, KLbsDummyAssistanceDataMaskWithError);
	}
		
//-----------------------------------------------------------------------------

TVersion CManagerMainLogic::Version()
	{
	LBSLOG(ELogP1, "CManagerMainLogic::Version()\n");
	LBSLOG(ELogP9, "<-S MLbsLocationSourceGpsObserver::Version() AGPSModule\n");
	LBSLOG(ELogP9, "  Return TVersion  = 1, 0, 0\n");
	return TVersion(1,0,0);
	}


//-----------------------------------------------------------------------------

// New location arrives from integration module ...
// from MLbsLocationSourceGpsObserver
//
void CManagerMainLogic::UpdateLocation
	(
	TInt aStatus,
	const TPositionInfoBase* aPosInfoArray[],
	TInt aNumItems,
	const TTime& aTargetTime
	)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::UpdateLocation() Begin\n");
	LBSLOG(ELogP9, "<-A MLbsLocationSourceGpsObserver::UpdateLocation() AGPSModule\n");
	LBSLOG2(ELogP9, "  > TInt aStatus  = %d\n", aStatus);
	LBSLOG2(ELogP9, "  > TInt aNumItems  = %d\n", aNumItems);
	LBSLOG5(ELogP9, "  > TTime TargetTime  = %02d:%02d:%02d.%06d\n", aTargetTime.DateTime().Hour(), 
																aTargetTime.DateTime().Minute(),
																aTargetTime.DateTime().Second(),
																aTargetTime.DateTime().MicroSecond());
	LBS_RDEBUG_ARGINT("AGPSMod", "LBS", "UpdateLocation", aStatus);

#ifdef ENABLE_LBS_DEV_LOGGER
	//Loop through the PositionInfoArray
	if(aPosInfoArray != NULL)
		{
		for(TInt posNum=0; posNum<aNumItems; posNum++)
			{
			const TPositionInfoBase& positionPtrTemp = (reinterpret_cast<const TPositionInfoBase&>(*(aPosInfoArray[posNum])));
			LBSLOG2(ELogP9, "  > TPositionInfoBase aPosInfoArray[%d]  =\n", posNum);
			LBSLOG_TPOSITIONINFOBASE(positionPtrTemp);
			(void) positionPtrTemp; // avoid a compiler warning
			}
		}
#endif

	(void)aNumItems;
	__ASSERT_ALWAYSX(aPosInfoArray != NULL,User::Panic(KLbsAGPSManFault, KErrArgument));
    TInt positionCalculationIndex; // index where the position calculation is in the array
    TInt measurementIndex; // index where the satellite measurement is in the array
    TBool positionCalculationPossible = EFalse;
	TTime timeNow;
	timeNow.UniversalTime(); //done here so that position and measurement are published with same time.

	// If the update contains an error then this error will be passed on the location bus, 
    // and if it's the case it will be sent with the measurements as well
    if(aStatus < 0)
    	{
    	LBSLOG(ELogP1,"There was an error returned");

		GetGpsUpdateIndexes(aPosInfoArray, aNumItems, positionCalculationIndex, measurementIndex);
    	if(measurementIndex >= 0)
    		{
    		// There is some measurement information so publish it together with the error
			const TPositionGpsMeasurementInfo* measurementPtr = (reinterpret_cast<const TPositionGpsMeasurementInfo*>(aPosInfoArray[measurementIndex]));
			
			iMeasurementUpdates.SetGpsMeasurementInfo(aStatus, measurementPtr, sizeof(TPositionGpsMeasurementInfo), timeNow);
			
			// Note: currently only doing this for any GPS mode except hybrid, so as not to adversely affect existing hybrid customers 
			// Since there's an error in the measurement, and if there is no position returned, publish it in a dummy position for the LS since he doesn't monitor measurement bus:
			if((positionCalculationIndex < 0) && (iCurrentPositioningMethod != KLbsMethodDual))
				{
				LBSLOG(ELogP1, "Publishing dummy position");
				PublishDummyPosition(aStatus);
				
				iLocSvrLocationRequestHandler->InvalidateRequest();
				iLSReqState = EReqNone;
				iNRHLocationRequestHandler->InvalidateRequest();
				RestartTimerIfTracking();				
				}
    		}

		if (positionCalculationIndex >= 0)
			{
			const TPositionModuleId id = {KLbsGpsLocManagerUidValue};
			TPosition pos;
			TUint attributes = 0; // only relevant when aStatus == KPositionCalculationFutile (>0).
			const TPositionExtendedSatelliteInfo* satPositionPtr = 
					(reinterpret_cast<const TPositionExtendedSatelliteInfo*>(aPosInfoArray[positionCalculationIndex]));
			TPositionExtendedSatelliteInfo position(*satPositionPtr);
			position.GetPosition(pos);
			LBSLOG3(ELogP2, "Lat=%3.4g. Long=%3.4g.\n", pos.Latitude(), pos.Longitude());
			position.SetModuleId(id);

			// Invalidate all the requests. No need to call CombineLocationRequests().	
			iLocSvrLocationRequestHandler->InvalidateRequest();
			iLSReqState = EReqNone;
			iNRHLocationRequestHandler->InvalidateRequest();

			// If the network did not request ETechnologyTerminal then it doesn't need to know about
			// position updates. Set the 'conflict flag' to make the NRH ignore the position update.
			//
			TBool nrhToIgnoreUpdate = EFalse;
			if (iNRHLocationRequestHandler->IsRequestActive() && 
				!(GetTechnologyTypeFromNrhRequest(0) & TPositionModuleInfo::ETechnologyTerminal) &&
				!(GetTechnologyTypeFromNrhRequest(1) & TPositionModuleInfo::ETechnologyTerminal))
				{
				nrhToIgnoreUpdate = ETrue;
				}
		
			// The position update is about to be published for the Location Server...
			// ...here we take the opportunity to let the LS know if we are collecting 
			// measurements and sending them to the network (so the LS can apply a network lag timer
			// for waiting for the final network position after he client timer has expired).
			// The LS will watch out for transitions in the mode from non-TA mode to a TA mode.
			TUint gpsModeForLocationServer = RLbsPositionUpdates::EGpsModeTerminalBased; // default value
			if (iCurrentPositioningMethod & TPositionModuleInfo::ETechnologyNetwork)
				{
				gpsModeForLocationServer = RLbsPositionUpdates::EGpsModeTerminalAssisted;
				}

			LBSLOG(ELogP1, "CManagerMainLogic::UpdateLocation() Set position info\n");
			LBSLOG2(ELogP1, "\t aStatus = %d\n", aStatus);
			iPositionUpdates.SetPositionInfo(aStatus, nrhToIgnoreUpdate, &position, sizeof(TPositionExtendedSatelliteInfo),aTargetTime, timeNow, attributes, gpsModeForLocationServer);
			RestartTimerIfTracking();
			}
    	}
    else if(IsUpdateAsExpected(aPosInfoArray, aNumItems, positionCalculationIndex, measurementIndex))
	// The update will be ignored if it is of an unexpected type
		{
		LBSLOG(ELogP1,"Update is valid");

		if (measurementIndex >= 0 &&
		   (iCurrentPositioningMethod & TPositionModuleInfo::ETechnologyNetwork) )
			{
			LBSLOG(ELogP1, "Measurement index is positive; network expects measurements");
			// There is a new Measurement data update when the network is expecting measurements. Publish it.
			const TPositionGpsMeasurementInfo* measurementPtr = (reinterpret_cast<const TPositionGpsMeasurementInfo*>(aPosInfoArray[measurementIndex]));
			positionCalculationPossible = measurementPtr->PositionCalculationPossible();
			iMeasurementUpdates.SetGpsMeasurementInfo(aStatus, measurementPtr, sizeof(TPositionGpsMeasurementInfo), timeNow);
			}

		if (positionCalculationPossible && positionCalculationIndex < 0)
				{
				// here, we have had a measurement good enough for the network to calculate a position of
				// the required accuracy and also we have NOT had a agps position
			    LBSLOG(ELogP2, "Invalidating NRG request and re-combining requests\n");

				iNRHLocationRequestHandler->InvalidateRequest();
				TUid dummyUid = {0};
				CombineLocationRequests(dummyUid);
				}
		else if ((positionCalculationIndex >= 0 &&
		    (iCurrentPositioningMethod & TPositionModuleInfo::ETechnologyTerminal || iFallbackFromPTAToAutonomousPossible)))
			{
			LBSLOG(ELogP1, "Position index is positive");
            const TPositionModuleId id = {KLbsGpsLocManagerUidValue};
            TPosition pos;
            TReal32 horAccuracy;
            TReal32 verAccuracy;
            TBool updateCorrect = EFalse;
			TUint attributes = 0; // only relevant when aStatus == KPositionCalculationFutile (>0).
			const TPositionExtendedSatelliteInfo* satPositionPtr = 
				(reinterpret_cast<const TPositionExtendedSatelliteInfo*>(aPosInfoArray[positionCalculationIndex]));
			TPositionExtendedSatelliteInfo position(*satPositionPtr);
            position.GetPosition(pos);
            horAccuracy = pos.HorizontalAccuracy();
            verAccuracy = pos.VerticalAccuracy();
            LBSLOG5(ELogP2, "Lat=%3.4g. Long=%3.4g. HorAcc=%3.4g. VerAcc=%3.4g.\n", pos.Latitude(), pos.Longitude(), horAccuracy, verAccuracy);
            position.SetModuleId(id);
			if (aStatus == 0)
			    {
			    LBSLOG(ELogP1, "Status is 0");
			    if ((horAccuracy != 0.0) && (verAccuracy != 0.0)) //IF3
			        {
			        LBSLOG(ELogP1, "Update is correct for status 0");

			        updateCorrect = ETrue;

					// Both handler's IsAccuracyRequirementMet() MUST be called to ensure both handlers change state
					// if required.
					TBool isLSSatisfied = iLocSvrLocationRequestHandler->IsAccuracyRequirementMet(horAccuracy,verAccuracy);
					TBool isNRHSatisfied = iNRHLocationRequestHandler->IsAccuracyRequirementMet(horAccuracy,verAccuracy);
				
					if (positionCalculationPossible)
						{
						// we have had a measurement from which the network can
						// calculate an accurate enough position
						// so, now in Hybrid mode (we have just recieved both a measuremnt and position!)
						// the request from the NRH is now deemed over.
						iNRHLocationRequestHandler->InvalidateRequest();
						isNRHSatisfied = ETrue;
						}
					
					if (isLSSatisfied || isNRHSatisfied)
					    {
					    LBSLOG(ELogP2, "NRH and/or LS satisfied with update\n");
						// Recombine requests without specifying an originating
						// channel or reason since we are not recombining after a change
						// in the LS or NRH request handlers.
						TUid dummyUid = {0};
						CombineLocationRequests(dummyUid);
					    }

					if ((iLSReqState != EReqSessionStarted) && (iLSReqState != EReqReceivedDuringSession) && 
					    !iLocSvrLocationRequestHandler->IsRequestActive()) 
					    {
					    LBSLOG(ELogP2, "Reseting session status\n");
					    iLSReqState = EReqNone;
					    }
					
					// Pass the update to the clock adjuster. 
					// It will adjust the system clock and update the TPosition::iTime if required.
					iAutoClockAdjust->LocationUpdate(aStatus, position);
					}
				}
	
			else if (aStatus > 0)
			    {
			    LBSLOG(ELogP1, "Status is greater than 0");
			    updateCorrect = ETrue;
			
				if (iLSReqState == EReqSessionStarted || iLSReqState == EReqReceivedDuringSession)
				    {	
				    attributes |= RLbsPositionUpdates::ESelfLocateSessionInProgress;
				    }
			
                // Check if we have to remember this position update for re-publishing it on the
                // GPS Location Bus when the session is closed. Currently this is only true
                // for KPositionCalculationFutile. 
                if ((KPositionCalculationFutile == aStatus) && (attributes & RLbsPositionUpdates::ESelfLocateSessionInProgress))
                    {
					iSessionClosureData.iNotify = ETrue;
					iSessionClosureData.iLastPosInfo = position;
					iSessionClosureData.iLastStatus = aStatus;
					iSessionClosureData.iLastTargetTime = aTargetTime;
                    }
			    }
			
			if (updateCorrect)
                {
                LBSLOG(ELogP1, "Update is correct");

				// If the network did not request Terminal Based or Autonomous positioning modes, 
				// it doesn't need to know about positions. If that is the case, set the
				// 'conflict/ignore-update flag' to true so that the NRH ignores the position update.
				// (Note: Terminal Based and Autonomous are the only two positioning methods with the bit
				// TPositionModuleInfo::ETechnologyTerminal set to 1.
				// Terminal Based is "TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted";
				// Autonomous mode is just "TPositionModuleInfo::ETechnologyTerminal").
				TBool nrhToIgnoreUpdate = EFalse;
				if (iNRHLocationRequestHandler->IsRequestActive() && 
				!(GetTechnologyTypeFromNrhRequest(0) & TPositionModuleInfo::ETechnologyTerminal) &&
				!(GetTechnologyTypeFromNrhRequest(1) & TPositionModuleInfo::ETechnologyTerminal))
				    {
					nrhToIgnoreUpdate = ETrue;
				    }
			
				// The position update is about to be published for the Location Server...
				// ...here we take the opportunity to let the LS know if we are collecting 
				// measurements and sending them to the network (so the LS can apply a network lag timer
				// for waiting for the final network position after he client timer has expired).
				// The LS will watch out for transitions in the mode from non-TA mode to a TA mode.
				TUint gpsModeForLocationServer = RLbsPositionUpdates::EGpsModeTerminalBased; // default value
				if (iCurrentPositioningMethod & TPositionModuleInfo::ETechnologyNetwork)
					{
					gpsModeForLocationServer = RLbsPositionUpdates::EGpsModeTerminalAssisted;
					}

				LBSLOG(ELogP1, "CManagerMainLogic::UpdateLocation() Set position info\n");
				LBSLOG2(ELogP1, "\t aStatus = %d\n", aStatus);
				iPositionUpdates.SetPositionInfo(aStatus, nrhToIgnoreUpdate, &position, sizeof(TPositionExtendedSatelliteInfo),aTargetTime, timeNow, attributes, gpsModeForLocationServer);
				RestartTimerIfTracking();
                }
			}
		}
	LBSLOG(ELogP1, "CManagerMainLogic::UpdateLocation() End\n");
	}


// This private method checks the correctness of an update from GPS Module.
// The method returns ETrue if the array with the updates reported by the GPS Module
// contains at least one of the types of updates (positions or measurements) expected
// according to the current positioning method. 
// This method returns in aPositionCalculationIndex the index of the array where the
// position calculation is (it is set to -1 if there is none).
// It returns in aMeasurementIndex the index of the array where the
// position calculation is (it is set to -1 if there is none). 
// 
TBool CManagerMainLogic::IsUpdateAsExpected(const TPositionInfoBase* aPosInfoArray[],const TInt& aNumItems, TInt& aPositionCalculationIndex, TInt& aMeasurementIndex)
	{
	TBool posInfoArrayIsValid = ETrue;
	aPositionCalculationIndex = -1;		
	aMeasurementIndex = -1;
	
	LBSLOG(ELogP1, "CManagerMainLogic::IsUpdateAsExpected() Begin");
	// Check if positions and/or measurements are present in the array and record the indexes
	GetGpsUpdateIndexes(aPosInfoArray, aNumItems, aPositionCalculationIndex, aMeasurementIndex);
		
	switch (iCurrentPositioningMethod)
		{
		case KLbsMethodAutonomous:
		case KLbsMethodTerminalBased:
			// If positions were received then the update is valid.
			posInfoArrayIsValid = (aPositionCalculationIndex >= 0);
		break;
				
		case KLbsMethodTerminalAssisted:
			posInfoArrayIsValid = (aMeasurementIndex >= 0) || 
                    (iFallbackFromPTAToAutonomousPossible && aPositionCalculationIndex >= 0);
		break;
		
		case KLbsMethodDual:
			// Both positions and/or measurements are valid updates
			posInfoArrayIsValid = ETrue;
		break;
		
		case KLbsMethodNone:
			// No request is any longer expecting this update
			posInfoArrayIsValid = EFalse;
		break;
		
		default:
			ASSERT(EFalse);
		break;
		}
	
	LBSLOG2(ELogP1, "CManagerMainLogic::IsUpdateAsExpected() End with %d", posInfoArrayIsValid);
	return posInfoArrayIsValid;
	}


/**
 This method returns where in the array the positions are stored and where the measurements.
 
 @aPosInfoArray The array with information sent from the GPS module
 @aNumItems The number of items in the array
 @aPositionCalculationIndex Used to return the index where positions are stored
 @aMeasurementIndex Used to return the index where measurements are stored
 */
void CManagerMainLogic::GetGpsUpdateIndexes(const TPositionInfoBase* aPosInfoArray[], const TInt& aNumItems, TInt& aPositionCalculationIndex, TInt& aMeasurementIndex)
	{
	aPositionCalculationIndex = -1;		
	aMeasurementIndex = -1;
	
	LBSLOG(ELogP1, "CGPSModeHandlerBase::GetGpsUpdateIndexes() Begin");

	// Check if positions and/or measurements are present in the array
	for (TInt index = 0; index < aNumItems; index++)
		{
		if ((aPosInfoArray[index]->PositionClassType() & EPositionSatelliteInfoClass) == EPositionSatelliteInfoClass)
			{
			LBSLOG2(ELogP1, "position present at index %d", index);
			aPositionCalculationIndex = index;					
			}
		else if ((aPosInfoArray[index]->PositionClassType() & EPositionGpsMeasurementInfoClass) == EPositionGpsMeasurementInfoClass)
			{
			LBSLOG2(ELogP1, "measurement present at index %d", index);
			aMeasurementIndex = index;
			}
		}
	LBSLOG(ELogP1, "CGPSModeHandlerBase::GetGpsUpdateIndexes() End");
	}

//-----------------------------------------------------------------------------

// convert TDeviceStatus to TIntGpsHwStatus
// indexed with TPositionModuleStatus::TDeviceStatus 
// returns one of the four values:  
// 	EStatusUnknown, EStatusOff, EStatusOn, EStatusActive
static const CPosIntGpsHwStatus::TIntGpsHwStatus KConvertStatusTable[8] = 
	{
	CPosIntGpsHwStatus::EStatusUnknown, // EDeviceUnknown,
	CPosIntGpsHwStatus::EStatusOff, 	// EDeviceError 	 	-> EStatusOff
	CPosIntGpsHwStatus::EStatusOff, 	// EDeviceDisabled 		-> EStatusOff
	CPosIntGpsHwStatus::EStatusOff, 	// EDeviceInactive 		-> EStatusOff
	CPosIntGpsHwStatus::EStatusOn, 		// EDeviceInitialising  -> EStatusOn
	CPosIntGpsHwStatus::EStatusOn, 		// EDeviceStandBy		-> EStatusOn
	CPosIntGpsHwStatus::EStatusOn, 		// EDeviceReady			-> EStatusOn
	CPosIntGpsHwStatus::EStatusActive	// EDeviceActive
	};

//-----------------------------------------------------------------------------

// from MLbsLocationSourceGpsObserver
// from the module
void CManagerMainLogic::UpdateDeviceStatus
	(
	TPositionModuleStatus::TDeviceStatus aDeviceStatus
	)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::UpdateDeviceStatus() Begin\n");
	LBSLOG(ELogP9, "<-A MLbsLocationSourceGpsObserver::UpdateDeviceStatus() AGPSModule\n");
	LBSLOG2(ELogP9, "  > TPositionModuleStatus::TDeviceStatus aDeviceStatus  = %d\n", aDeviceStatus);
	LBS_RDEBUG_ARGINT("AGPSMod", "LBS", "UpdateDeviceStatus", aDeviceStatus);
	TPositionModuleStatus moduleStatus;
	
	// publish the HW status 
	CPosIntGpsHwStatus::TIntGpsHwStatus hwStatus = CPosIntGpsHwStatus::EStatusUnknown;
	hwStatus = KConvertStatusTable[aDeviceStatus]; 
	
	TRAPD(error, iPosIntGpsHwStatus->SetStatusL(hwStatus));
	if(error != KErrNone)
	    {
        User::Panic(KLbsAGPSManFault, error);
	    }
	
	iModuleStatus.GetModuleStatus(&moduleStatus,sizeof(moduleStatus),KLbsGpsLocManagerUid);

	moduleStatus.SetDeviceStatus(aDeviceStatus);
	
	TPositionModuleStatusEventBase::TModuleEvent occurredEventsSinceLastSet = 
								TPositionModuleStatusEventBase::EEventDeviceStatus;

	iModuleStatus.SetModuleStatus(&moduleStatus,sizeof(moduleStatus),occurredEventsSinceLastSet);
	
	LBSLOG(ELogP1, "CManagerMainLogic::UpdateDeviceStatus() End\n");
	}
	 
// from MLbsLocationSourceGpsObserver	
void CManagerMainLogic::UpdateDataQualityStatus
	(
	TPositionModuleStatus::TDataQualityStatus aDataQuality
	)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::UpdateDataQualityStatus() Begin\n");
	LBSLOG(ELogP9, "<-A MLbsLocationSourceGpsObserver::UpdateDataQualityStatus() AGPSModule\n");
	LBSLOG2(ELogP9, "  > TPositionModuleStatus::TDataQualityStatus aDataQuality  = %d\n", aDataQuality);
	LBS_RDEBUG_ARGINT("AGPSMod", "LBS", "UpdateDataQualityStatus", aDataQuality);

	TPositionModuleStatus moduleStatus;
	iModuleStatus.GetModuleStatus(&moduleStatus,sizeof(moduleStatus),KLbsGpsLocManagerUid);

	moduleStatus.SetDataQualityStatus(aDataQuality);
	
	TPositionModuleStatusEventBase::TModuleEvent occurredEventsSinceLastSet = 
								TPositionModuleStatusEventBase::EEventDataQualityStatus;

	iModuleStatus.SetModuleStatus(&moduleStatus,sizeof(moduleStatus),occurredEventsSinceLastSet);
	LBSLOG(ELogP1, "CManagerMainLogic::UpdateDataQualityStatus() End\n");
	}

// from MLbsLocationSourceGpsObserver
// Integration module informs us that we may now delete it 
void CManagerMainLogic::Shutdown()
	{
	LBSLOG(ELogP1, "CManagerMainLogic::Shutdown() Begin\n");
	LBSLOG(ELogP9, "<-A MLbsLocationSourceGpsObserver::Shutdown() AGPSModule\n");
	LBS_RDEBUG("AGPSMod", "LBS", "Shutdown");

	 // tell the Root Process we are closed.
	__ASSERT_ALWAYSX(iClosingDown,User::Panic(KLbsAGPSManFault, KErrArgument));
	
	iQuietus->CoupDeGrace();
	LBSLOG(ELogP1, "CManagerMainLogic::Shutdown() End\n");
	}

//	New assistnce data has arrived from network. 
//Pass mask dowm to integration module
void CManagerMainLogic::OnAssistanceDataResponse(TInt aError, TLbsAsistanceDataGroup aMask)
 	{
	LBSLOG(ELogP1, "CManagerMainLogic::OnAssistanceDataResponse() Begin\n");
	LBSLOG(ELogP9, "->S CLbsLocationSourceGpsBase::AssistanceDataEvent() AGPSModule\n");
	LBSLOG2(ELogP9, "  > TInt aError  = %d\n", aError);
	LBSLOG2(ELogP9, "  > TLbsAsistanceDataGroup aMask  = 0x%08X\n", aMask);
	
	// assistance data received from network - // pass mask onto integration module
	iLocationSource->AssistanceDataEvent(aError,aMask);

	if ((iCurrentPositioningMethod == KLbsMethodTerminalAssisted) && (aError < 0)) 
        {
		CLbsAdmin::TGpsMode gpsMode;
        // Get from Admin the flavour ("Preferred" or "Always") of Terminal Assisted
        // that is in progress
        GetTaGpsModeFromAdmin(gpsMode);
        if(gpsMode == CLbsAdmin::EGpsPreferTerminalAssisted)
            {
            // In PTA we send the module a dummy assistance data event to tell him to switch to autonomous if he can (or fail if not)
            iFallbackFromPTAToAutonomousPossible = ETrue;
            }
        }
    
	/** LBSLOGGER - Start Logging */
	// -------------------------------------------------------------------------
	CLbsAssistanceDataLogEvent* logEvent = NULL;
	TRAPD(err, logEvent = CLbsAssistanceDataLogEvent::NewL(iLogAssitaceDataGroup, aMask));
	if (err == KErrNone)
		{
		iLogger.AddEvent(*logEvent);
		
		delete logEvent;
		}
	// -------------------------------------------------------------------------
	/** LBSLOGGER - End Logging */

	LBSLOG(ELogP1, "CManagerMainLogic::OnAssistanceDataResponse() End\n");
	}

/** 
Add handling other errors when implementing Always Assisted modes.
*/
void CManagerMainLogic::OnSessionComplete(TInt aReason, const TLbsNetSessionIdInt& aSessionId)
 	{
	LBSLOG(ELogP1, "CManagerMainLogic::OnSessionComplete() Begin\n");
	CLbsAdmin::TGpsMode gpsMode = CLbsAdmin::EGpsModeUnknown;

	iCurrentNetworkMode = KLbsMethodNone;
	
	// only do this when get explicit cancel from NRH iActiveLSMode = KLbsMethodNone;
	
	if ((iCurrentPositioningMethod == KLbsMethodTerminalAssisted) && (aReason < 0)) 
		{
		// Get from Admin the flavour ("Preferred" or "Always") of Terminal Assisted
		// that is in progress
		 GetTaGpsModeFromAdmin(gpsMode);
		}
	
	// These two errors (KErrServerBusy, KErrPositionHighPriorityReceive) have a special meaning 
	// and cancel any outstanding Self Location request from the Location Server
	// Similarly if we're in Terminal Assisted mode (expecting the network to calculate the position),
	// any outstanding Loc Server request will need to be cancelled 	
	if ((aReason == KErrServerBusy) ||
		(aReason == KErrPositionHighPriorityReceive) ||
		((aReason < 0) && (gpsMode == CLbsAdmin::EGpsAlwaysTerminalAssisted)))
		{
		iLocSvrLocationRequestHandler->InvalidateRequest();
		iLSReqState = EReqNone;
		
		RestartTimerIfTracking();
		
		PublishDummyPosition(aReason);
		
		// Recombine requests as if the NRH had cancelled to allow
		// a cancel sent to the module (if there are no more requests left).
		CombineLocationRequests(KLbsNetRequestHandlerUid, EReasonCancel);
		iIsNrhCancelDue = EFalse;
		}
	else if(((aReason < 0) && (gpsMode == CLbsAdmin::EGpsPreferTerminalAssisted)))
		{
			// In PTA we send the module a dummy assistance data event to tell him to switch to autonomous if he can (or fail if not)
			PublishDummyAssistDataEvent(aReason);
			iFallbackFromPTAToAutonomousPossible = ETrue;
		}
	else
		{
		// Other errors do not cancel LS requests.
		// NRH Location requests should get cancelled by NRH.
		switch (iLSReqState)
			{
			case EReqNone:
			case EReqReceived:
			case EReqSessionCompleted:
				// Ignore
				break;
				
			case EReqSessionStarted:
			case EReqReceivedDuringSession:
				if ((iSessionId.SessionOwner() == aSessionId.SessionOwner()) &&
					(iSessionId.SessionNum() == aSessionId.SessionNum()))
					{
					// In HYBRID and TAP modes we may need to initiate a new request for a FNP 
					// This would happen if a request was received from LS during the current ongoing session
					// and it hadn't been satisfied by any arrival of a FNP or AGPS position
					// We need to do this to instruct  the network to start calculating a FNP.
					// Note that, If its Futile then we dont re-issue a new request
					if( (iCurrentPositioningMethod & TPositionModuleInfo::ETechnologyNetwork)
							&& (iLSReqState == EReqReceivedDuringSession)
							&& (iLocSvrLocationRequestHandler->IsRequestActive())
							&& (!iSessionClosureData.iNotify))
						{
						RLbsNetworkPositionUpdates netPosUpdates;

						TRAPD(error, netPosUpdates.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionFinal));
						if (error!=KErrNone)
						 {
						 User::Panic(KLbsAGPSManFault, error);
						 }
						
						TLbsNetSessionIdInt sessionId;
						TPositionInfo netPosInfo;
						TTime targetTime;
						TTime actualTime;					
						TInt err = netPosUpdates.GetPositionInfo(sessionId, netPosInfo, targetTime, actualTime);
						// Make sure we passed in the right position class type
						__ASSERT_ALWAYSX(err != KErrNotSupported, User::Invariant());
						if((err != KErrNone) || 
							((netPosInfo.PositionMode() & TPositionModuleInfo::ETechnologyNetwork) != TPositionModuleInfo::ETechnologyNetwork) || 
							(actualTime < iLocSvrLocationRequestHandler->GetTimeActivated()))
							{ // if either there is no FNP, or the FNP didn't originate from the network, 
							  // or the activation time of the request is after the FNP was returned  
							  // then re-issue the request
							iLSReqState = EReqReceived;
							CombineLocationRequests(KLbsLocServerUid, MLocationRequestHandlerObserver::EReasonRequest);
							}
						

						}

					if(iLSReqState != EReqReceived)
						{
					iLSReqState = EReqSessionCompleted;
						}
					
					// Check if we need to re-publish the cached position update when
					// completing the session (currently this should only happen if an
					// update with KPositionCalculationFutile was received).
					// Also invalidate Loc Server's request in this case.
					if(iSessionClosureData.iNotify)
						{
						TTime timeNow;
						timeNow.UniversalTime();
						iLocSvrLocationRequestHandler->InvalidateRequest();
						
						// The position update is about to be re-published for the Location Server...
						// ...here we take the opportunity to let the LS know if we are collecting 
						// measurements and sending them to the network (so the LS can apply a network lag timer
						// for waiting for the final network position after he client timer has expired).
						// The LS will watch out for transitions in the mode from non-TA mode to a TA mode.
						TUint gpsModeForLocationServer = RLbsPositionUpdates::EGpsModeTerminalBased; // default value
						if (iCurrentPositioningMethod & TPositionModuleInfo::ETechnologyNetwork)
							{
							gpsModeForLocationServer = RLbsPositionUpdates::EGpsModeTerminalAssisted;
							}
						
						iPositionUpdates.SetPositionInfo(iSessionClosureData.iLastStatus,
														EFalse,
														&iSessionClosureData.iLastPosInfo,
														sizeof(TPositionExtendedSatelliteInfo),
														iSessionClosureData.iLastTargetTime,
														timeNow,
														RLbsPositionUpdates::ESelfLocateSessionClosed,
														gpsModeForLocationServer);
						iSessionClosureData.iNotify = EFalse;
						}
					}
				else
					{
					LBSLOG_WARN(ELogP2, "SessionNum or SessionOwner does not match\n");
					}
				break;

			default:
				User::Panic(KLbsAGPSManFault, EAGPSManUnknownFSMState);
				break;
			}
		
		}	
	LBSLOG(ELogP1, "CManagerMainLogic::OnSessionComplete() End\n");
	}


// from MLbsLocationSourceGpsObserver
// request from integration module
TInt CManagerMainLogic::GetAssistanceDataItem
	(
	TLbsAssistanceDataItem aItem,
	RDataReaderRootBase& aDataRoot,
	TTime& aTimeStamp
	)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::GetAssistanceDataItem() Begin\n");
	LBSLOG(ELogP9, "<-A MLbsLocationSourceGpsObserver::GetAssistanceDataItem() AGPSModule\n");
	LBSLOG2(ELogP9, "  > TLbsAssistanceDataItem aItem  = 0x%08X\n", aItem);
	
	TInt err = iNetworkGatewayHandler->GetAssistanceDataItem(aItem,aDataRoot,aTimeStamp);
	
	LBSLOG5(ELogP9, "  < TTime aTimeStamp  = %02d:%02d:%02d.%06d\n", aTimeStamp.DateTime().Hour(), 
																		aTimeStamp.DateTime().Minute(),
																		aTimeStamp.DateTime().Second(),
																		aTimeStamp.DateTime().MicroSecond());
	LBSLOG2(ELogP9, "  Return  = %d", err);
	return err;
	}


// from MLbsLocationSourceGpsObserver
// request from integration module
TInt CManagerMainLogic::GetAssistanceDataItemTimeStamp(TLbsAssistanceDataItem aItem, TTime& aTimeStamp)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::GetAssistanceDataItemTimeStamp() Begin\n");
	LBSLOG(ELogP9, "<-A MLbsLocationSourceGpsObserver::GetAssistanceDataItemTimeStamp() AGPSModule");
	LBSLOG2(ELogP9, "  > TLbsAssistanceDataItem aItem  = 0x%08X\n", aItem);
	
	TInt err = iNetworkGatewayHandler->GetAssistanceDataItemTimeStamp(aItem, aTimeStamp);
	
	LBSLOG5(ELogP9, "  < TTime aTimeStamp  = %02d:%02d:%02d.%06d\n", aTimeStamp.DateTime().Hour(), 
																		aTimeStamp.DateTime().Minute(),
																		aTimeStamp.DateTime().Second(),
																		aTimeStamp.DateTime().MicroSecond());
	LBSLOG2(ELogP9, "  Return  = %d", err);
	return err;
	}


// from MLbsLocationSourceGpsObserver
// Integration module requests assistance data from network
void CManagerMainLogic::RequestAssistanceData
	(
	TLbsAsistanceDataGroup aDataItemMask
	)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::RequestAssistanceData() Begin\n");
	LBSLOG(ELogP9, "<-A MLbsLocationSourceGpsObserver::RequestAssistanceData() AGPSModule\n");
	LBSLOG2(ELogP9, "  > TLbsAsistanceDataGroup aDataItemMask  = 0x%08X\n", aDataItemMask);
	LBS_RDEBUG_ARGINT("AGPSMod", "LBS", "RequestAssistanceData", aDataItemMask);

	// LBSLOGGER - Store for later
	iLogAssitaceDataGroup = aDataItemMask;
	
	if(iLSReqState == EReqReceived)
		{
		TTime now;
		now.UniversalTime();
		
		//If there is a NRH request outstanding and the LocSrv request is in the future we do not start 
		//the self-locate session. Instead of that, we send the assistance data request and start the timer
		//for the LocSrv. When the timer fires, the LocSrv handler will reissue the request, the request will
		//be reissued to the Integration Module, it will call RequestAssistanceData again, and then we start
		//the self-locate session. 
		//This functionality is to prevent early self-locate requests when there are both NRH and LocSrv
		//requests outstanding and the LocSrv request TT is in the future.
		if(iNRHLocationRequestHandler->IsRequestActive() && iLocSvrLocationRequestHandler->GetStartTime() > now)
			{
			iLocSvrLocationRequestHandler->RestartOnTargetTime();
			iNetworkGatewayHandler->SendAssistanceDataRequest(aDataItemMask);
			}
		else
			{
			iLocSvrLocationRequestHandler->CancelRestartOnTargetTime();
			// Build Self Location Request
		    TLbsLocRequestQualityInt reqQuality = iLocSvrLocationRequestHandler->GetQuality();

		    TLbsNetPosRequestQualityInt ngQuality;
			ngQuality.SetMaxFixAge(0);
			ngQuality.SetMaxFixTime(reqQuality.MaxFixTime());
			ngQuality.SetMinHorizontalAccuracy(reqQuality.MinHorizontalAccuracy());
			ngQuality.SetMinVerticalAccuracy(reqQuality.MinVerticalAccuracy());
								
		    TLbsNetPosRequestOptionsAssistanceInt options;
			options.SetRequestQuality(ngQuality);
			options.SetDataRequestMask(aDataItemMask);
						
			switch (iActiveLSMode)
				{
				case KLbsMethodNone:
					options.SetPosMode(TPositionModuleInfo::ETechnologyUnknown);
					break;
					
				case KLbsMethodAutonomous:
					options.SetPosMode(TPositionModuleInfo::ETechnologyTerminal);
					break;
									
				case  KLbsMethodTerminalBased:
				case  KLbsMethodDual: 
					options.SetPosMode(TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
					break;
														
				case KLbsMethodTerminalAssisted:
					options.SetPosMode(TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
					break;
								
				default:
					options.SetPosMode(TPositionModuleInfo::ETechnologyUnknown);
					__ASSERT_ALWAYSX(EFalse,User::Panic(KLbsAGPSManFault, KErrArgument));
					break;
				}
												
			options.SetNewClientConnected(iLocSvrLocationRequestHandler->GetNewClient());
			iSessionId.IncrSession();
			iCurrentNetworkMode = iCurrentPositioningMethod;
			iNetworkGatewayHandler->SendSelfLocationRequest(iSessionId, options);
			iLSReqState = EReqSessionStarted;	
			}
		}
	else  //iLSReqState != EReqReceive
		{
		iNetworkGatewayHandler->SendAssistanceDataRequest(aDataItemMask);
		}
	
	LBSLOG(ELogP1, "CManagerMainLogic::RequestAssistanceData() End\n");
	}
	
	
void CManagerMainLogic::GetTimeAndQuality(const CLocationRequestHandler& aRequestHandler,
				TTime& aTargetTime, TLbsLocRequestQualityInt& aQuality)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::GetTimeAndQuality() Begin\n");
	aTargetTime	= aRequestHandler.GetStartTime();
	aQuality= aRequestHandler.GetQuality();
	LBSLOG(ELogP1, "CManagerMainLogic::GetTimeAndQuality() End\n");
	}

TPositionModuleInfo::TTechnologyType CManagerMainLogic::GetTechnologyTypeFromNrhRequest(const TInt& aIndex)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::GetTechnologyTypeFromNrhRequest() Begin\n");
	LBSLOG(ELogP1, "CManagerMainLogic::GetTechnologyTypeFromNrhRequest() End\n");
	return iNRHLocationRequestHandler->GetTechnologyTypeFromRequest(aIndex);
	}
	
TPositionModuleInfo::TTechnologyType CManagerMainLogic::GetTechnologyTypeFromLsRequest(const TInt& aIndex)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::GetTechnologyTypeFromLsRequest() Begin\n");
	LBSLOG(ELogP1, "CManagerMainLogic::GetTechnologyTypeFromLsRequest() End\n");
	return iLocSvrLocationRequestHandler->GetTechnologyTypeFromRequest(aIndex);
	}
	
/** This method implements the logic for combining location requests from the LS and NRH
request handlers following a change in the status of either. That logic combines the 
quality and start and end times of both requests.

Before sending a new combined request to the GPS Module, this method invokes ConfigureGpsMode()
to ensure that the GPS module is configured to operate in a positioning method that suites
both the LS and NRH requests.

@aChannelId The channel (LS or NRH) whose status last changed (to active or inactive)
@aReason The reason of the change in the channel status (new request, cancel or timeout)
*/
void CManagerMainLogic::CombineLocationRequests(const TUid& aChannelId, const TRequestStateChangeReason& aReason)
 	{ 	
	LBSLOG(ELogP1, "CManagerMainLogic::CombineLocationRequests() Begin\n");
 	TTime targetTime;
	TLbsLocRequestQuality quality;
	TTimeIntervalMicroSeconds maxFixTime = 0;
	// If in Futile case and GPS producing measurements,
	// do not send a further request to data source to stop AGPS
	// manager re-issuing a request after Futile.
	if (iSessionClosureData.iNotify &&
	    (iCurrentPositioningMethod & TPositionModuleInfo::ETechnologyNetwork))
		{
		LBSLOG(ELogP1, "Invalidating request from LS to avoid reissue after futile measurement received\n");
		iLocSvrLocationRequestHandler->InvalidateRequest();
		}	
		
	// The fix here to get the start time for LS requests, of using the activation time if 
	// the target time is 0, otherwise the target time, might not be needed when DEF117381 gets fixed
	if (!iLocSvrLocationRequestHandler->IsRequestActive() &&
		!iNRHLocationRequestHandler->IsRequestActive() ) 
		{
		if (aReason == EReasonCancel && !iLastTrackingFlagSentToNG)
			{
			LBSLOG(ELogP9, "->S CLbsLocationSourceGpsBase::CancelLocationRequest() AGPSModule");
			LBS_RDEBUG("LBS", "AGPSMod", "CancelLocationRequest");		
			iLocationSource->CancelLocationRequest();
			}
		}
	else
		if (iLocSvrLocationRequestHandler->IsRequestActive() &&
			iNRHLocationRequestHandler->IsRequestActive())

		{
		TTime targetTimeNRH;
		TLbsLocRequestQualityInt qualityNRH;
		TTime targetTimeLS;
		TLbsLocRequestQualityInt qualityLS;
		GetTimeAndQuality(*iNRHLocationRequestHandler,targetTimeNRH,qualityNRH);
		GetTimeAndQuality(*iLocSvrLocationRequestHandler,targetTimeLS,qualityLS);

 		TTime startTimeNRH = iNRHLocationRequestHandler->GetTimeActivated();
 		TTimeIntervalMicroSeconds fixTimeNRH = qualityNRH.MaxFixTime();
 		TTime endTimeNRH;
		if (targetTimeNRH == 0)
			{
			endTimeNRH = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(startTimeNRH,fixTimeNRH);
			}
		else
			{
			endTimeNRH = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(targetTimeNRH,fixTimeNRH);
			}
 
		if (targetTimeLS > endTimeNRH) // no overlap
		{
		// if no overlap then issue earlier one (the NRH request)
		// This could be optimised and only send NRH request once
			targetTime = targetTimeNRH;
			maxFixTime = fixTimeNRH;
			ConvertToTLbsLocRequestQuality(qualityNRH, quality);

			// the request is entirely from NRH, so set the flag to false
			quality.SetAssistedClientRequestPresent(EFalse);

		}
		else
		{
		// requests overlap since NRH always starts now instead of in the future! so issue a combined request
 		targetTime = targetTimeNRH; // NRH requests are always "NOW"
 		// For a combined request, need to take account of the time the
 		// initial request has been active
 		TTime startTimeLS = targetTimeLS.Int64() == 0 ? iLocSvrLocationRequestHandler->GetTimeActivated() : targetTimeLS;
 		TTime endTimeLS = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(startTimeLS, qualityLS.MaxFixTime());
		TTime endTime = (endTimeNRH >= endTimeLS) ? endTimeNRH : endTimeLS;
		// now have an end time. Convert it to a max fix time
  		TTime timeNow;
 		timeNow.UniversalTime();
 		// since at least NRH request is being processed now modify the max fix time
 		maxFixTime = LbsTimerUtils::SubtractInt64TimersWithUnderflowCheck(endTime, timeNow);
	
		// set Quality to most stringent
		TReal32	minHorizontalAccuracy;
		TReal32	minVerticalAccuracy;
 		
		TReal32	minHorizontalAccuracyNRH = qualityNRH.MinHorizontalAccuracy();
		TReal32 minVerticalAccuracyNRH = qualityNRH.MinVerticalAccuracy();

		TReal32	minHorizontalAccuracyLS	= qualityLS.MinHorizontalAccuracy();
		TReal32 minVerticalAccuracyLS = qualityLS.MinVerticalAccuracy();

		minHorizontalAccuracy = (minHorizontalAccuracyNRH >= minHorizontalAccuracyLS)
								?minHorizontalAccuracyLS:minHorizontalAccuracyNRH;
		
		minVerticalAccuracy = (minVerticalAccuracyNRH >= minVerticalAccuracyLS)
								?minVerticalAccuracyLS:minVerticalAccuracyNRH;
	
		quality.SetMinHorizontalAccuracy(minHorizontalAccuracy);
		quality.SetMinVerticalAccuracy(minVerticalAccuracy);
		if(GetTechnologyTypeFromLsRequest(0) != KLbsMethodAutonomous)
		    {
            // this request is partially from the client and the client didn't request 
		    // Autonomous, so set the flag to true
            quality.SetAssistedClientRequestPresent(ETrue);
		    }
		else
		    {
            // client requested Autonomous
            quality.SetAssistedClientRequestPresent(EFalse);		    
		    }
		}

	}
	else
	if (iLocSvrLocationRequestHandler->IsRequestActive()) 
 	{
		// only a Location Server request is active
 		TLbsLocRequestQualityInt qualityInt;
		GetTimeAndQuality(*iLocSvrLocationRequestHandler,targetTime,qualityInt);
		ConvertToTLbsLocRequestQuality(qualityInt, quality);
		// At this point, this could be just a standard LS Request.
		// However, it could also be a resend of an older request after a
		// combined request was issued and the NRH request timed out.
		// So, we have to calculate the fix time remaining rather than using
		// the fix time from the original request
 		TTime startTime = targetTime.Int64() == 0 ? iLocSvrLocationRequestHandler->GetTimeActivated() : targetTime;
 		TTime endTime = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(startTime, quality.MaxFixTime());

  		TTime timeNow;
 		timeNow.UniversalTime();
 		if(targetTime < timeNow)
	 		{ // if the request is being processed then readjust the max fix time
	 		maxFixTime = LbsTimerUtils::SubtractInt64TimersWithUnderflowCheck(endTime, timeNow);
	        }
	    else
 			{ // otherwise leave it as it was
 			maxFixTime = quality.MaxFixTime();
 			}
        if(GetTechnologyTypeFromLsRequest(0) != KLbsMethodAutonomous)
            {
            // this request is entirely from the client API and the client didn't request 
            // Autonomous, so set the flag to true
            quality.SetAssistedClientRequestPresent(ETrue);
            }
        else
            {
            // client requested Autonomous
            quality.SetAssistedClientRequestPresent(EFalse);            
            }
	}
	else
	{
	// only an NRH request is active
		TLbsLocRequestQualityInt qualityInt;
	    GetTimeAndQuality(*iNRHLocationRequestHandler,targetTime,qualityInt);
		ConvertToTLbsLocRequestQuality(qualityInt, quality);
		// At this point, this could be just a standard NRH Request.
		// However, it could also be a resend of an older request after a
		// combined request was issued and the LS request timed out.
		// So, we have to calculate the fix time remaining rather than using
		// the fix time from the original request
		TTime startTime = targetTime.Int64() == 0 ? iNRHLocationRequestHandler->GetTimeActivated() : targetTime;
 		TTime endTime = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(startTime,quality.MaxFixTime());
  		TTime timeNow;
 		timeNow.UniversalTime();
 		if(targetTime < timeNow)
 			{ // if the request is being processed then readjust the max fix time
		    maxFixTime = LbsTimerUtils::SubtractInt64TimersWithUnderflowCheck(endTime, timeNow);
	        }
 		else
 			{ // otherwise leave it as it was
 			maxFixTime = quality.MaxFixTime();
 			}
 		
        // the request is entirely from NRH, so set the flag to false
        quality.SetAssistedClientRequestPresent(EFalse);
	}
	
	// Check if GPS module's mode needs reconfiguring
    ConfigureGpsMode(aChannelId, aReason);
    
	if (maxFixTime != 0) // if we need to send a new request
		{
		// If we are in a GPS mode that sends measurements to the network
		// and a LocServer request is active then add on the 
		// lag time to allow the loc server to receive GPS positions from the 
		// integration module until the session complete comes in.
		if ((iCurrentPositioningMethod & TPositionModuleInfo::ETechnologyNetwork) &&
			 (!iSessionClosureData.iNotify) && 
			(iLocSvrLocationRequestHandler->IsRequestActive() || iLSReqState == EReqSessionStarted 
			|| iLSReqState == EReqReceivedDuringSession))
			{
			TTime extendedMaxFixTime = maxFixTime.Int64();
			extendedMaxFixTime = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(extendedMaxFixTime, iFinalNetPositionLag);
			maxFixTime = extendedMaxFixTime.Int64();
			}

		quality.SetMaxFixTime(maxFixTime);
		LBSLOG(ELogP9, "->A CLbsLocationSourceGpsBase::RequestLocationUpdate() AGPSModule\n");
		LBSLOG5(ELogP9, "  > TargetTime  = %02d:%02d:%02d.%06d\n", targetTime.DateTime().Hour(), 
																	targetTime.DateTime().Minute(),
																	targetTime.DateTime().Second(),
																	targetTime.DateTime().MicroSecond());
		LBSLOG(ELogP9, "  > TLbsLocRequestQuality quality  =\n");
		LBSLOG2(ELogP9, "    - Max Fix Time  = %ld\n", quality.MaxFixTime().Int64());
		LBSLOG2(ELogP9, "    - Horizontal Accuracy  = %f\n", quality.MinHorizontalAccuracy());
		LBSLOG2(ELogP9, "    - Vertical Accuracy  = %f\n", quality.MinVerticalAccuracy());
		LBS_RDEBUG("LBS", "AGPSMod", "RequestLocationUpdate");		

		iLocationSource->RequestLocationUpdate(targetTime, quality);
		}
}

/**
This method works out if a new GPS Options message has to be sent to the GPS module to request
a new positioning method to be used.

The method must first call CalculateActiveLSMode to calculate the contribution of the Location
Server request to the desired GPS mode.

*/ 
void CManagerMainLogic::ConfigureGpsMode(const TUid& aChannelId, const TRequestStateChangeReason& aReason)
		{
	TBool forcedSelfLocate;

	CalculateActiveLSMode(aChannelId, aReason, forcedSelfLocate);
		
	if(KLbsNetRequestHandlerUid == aChannelId)
		{
		if(MLocationRequestHandlerObserver::EReasonCancel == aReason)
			{
			iIsNrhCancelDue = EFalse;
			}
		else if(MLocationRequestHandlerObserver::EReasonRequest == aReason)
			{
			iIsNrhCancelDue = ETrue;
			}
		}
	
	
	TPositionModuleInfo::TTechnologyType newPositioningMethod = KLbsMethodNone;
	newPositioningMethod = CalculatePositioningMethod();

	// A change in the combined GPS mode implies a new configuration
	// message has to be sent to the module
	if (newPositioningMethod != iCurrentPositioningMethod 
			|| (!iCurrentGpsTimingOfCellFramesRequested != !iNRHLocationRequestHandler->GpsTimingOfCellFramesRequested()))		
		{
		SetGpsOptions(newPositioningMethod);
		iCurrentPositioningMethod = newPositioningMethod;
		iCurrentGpsTimingOfCellFramesRequested = iNRHLocationRequestHandler->GpsTimingOfCellFramesRequested();
			
		// The GPS module has been configured with a new mode...
		// check if a SelfLocate has to be sent to the NG next time
		// the GPM module requests assistance data.
		if (forcedSelfLocate)
			{
			iLSReqState = EReqReceived;	
			}
		}

	}


/* This method determines whether the GPS mode received in a Location Request from the Location
Server should be considered to configure the GPS mode or if the currently active mode provided
by an outstanding Location Server request must continue.
The method also determines whether a new selflocation request is to be sent to the Network Gateway
(only necessary when the request has not originated in a new client and a session is already in progress)
*/
void CManagerMainLogic::CalculateActiveLSMode(const TUid& aChannelId, const TRequestStateChangeReason& aReason, TBool& aForceSelfLocate)
	{
        aForceSelfLocate = EFalse;

        if (KLbsLocServerUid == aChannelId && MLocationRequestHandlerObserver::EReasonRequest == aReason)
		{
		//The Location Server sent a location request (only one posmethod allowed in the request)
		__ASSERT_ALWAYSX(GetTechnologyTypeFromLsRequest(1) == KLbsMethodNone, User::Invariant());
		__ASSERT_ALWAYSX((GetTechnologyTypeFromLsRequest(0) == KLbsMethodAutonomous ||
			   			 GetTechnologyTypeFromLsRequest(0) == KLbsMethodTerminalBased ||
			   			 GetTechnologyTypeFromLsRequest(0) == KLbsMethodTerminalAssisted),
			   			 User::Invariant());

		// The GPS mode in new clients' requests is always used
		if (iLocSvrLocationRequestHandler->GetNewClient())
			{
			iActiveLSMode = GetTechnologyTypeFromLsRequest(0);
			// No need to force a new self-locate request as one will be sent
			// by the usual means due to this being a new-client request.
			}
		else
			{
			// For "old" clients, the mode in the new LS request is always used when
			// it is NOT autonomous.
			TPositionModuleInfo::TTechnologyType oldActiveLSMode = iActiveLSMode;
			if((KLbsMethodAutonomous != GetTechnologyTypeFromLsRequest(0)))
				{
				iActiveLSMode = GetTechnologyTypeFromLsRequest(0);
				
				// If the active LS mode has changed AND a self-location session is in progress,
				// a new session has to be forced to start (if it is not in progress already
				// it will start by the usual means)
				aForceSelfLocate = ((oldActiveLSMode != iActiveLSMode) && (KLbsMethodNone != iCurrentNetworkMode));										
				}
			else
				{
				// For "old" Autonomous clients, set them to Autonomous if there is no network method
				if(KLbsMethodNone == iCurrentNetworkMode)
				    {
				    iActiveLSMode = KLbsMethodAutonomous;
				    }
				// Otherwise set them to the network method
				else
				    {
				    iActiveLSMode = iCurrentNetworkMode;
				    }
				
				}
			}				
		}
	}


TPositionModuleInfo::TTechnologyType CManagerMainLogic::LookUpPositioningMethodInCombinationTables()
    {
    // Look up positioning method in combination tables
        const TLbsGpsCombinedModeTableEntry* ptrToTableEntry = NULL;
        TInt entryCount = 0;
        
        // Select the correct table according to h/w capabilities
        switch (iCapabilities)
            {
            case TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased:
                ptrToTableEntry = KLbsTBTable;
                entryCount =  KLbsTBTableCount;
            break;
            
            case TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted:
                ptrToTableEntry = KLbsTATable;
                entryCount = KLbsTATableCount;
            break;
            
            case (TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased|
                  TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted):
              ptrToTableEntry = KLbsTBOrTATable;
              entryCount = KLbsTBOrTATableCount;
            break;
            
            case TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB:
                ptrToTableEntry = KLbsTBAndTATable;
                entryCount = KLbsTBAndTATableCount;
            break;
            
            case TPositionModuleInfoExtended::EDeviceGpsModeAutonomous:
                ptrToTableEntry = KLbsAutonomousTable;
                entryCount = KLbsAutonomousTableCount;
            break;
            
            default:
                ASSERT(EFalse); // incorrect capabilities set
            break;
            }
        
        // Find an entry of the table matching the positioning methods
        // requested by LS and NRH 
        TPositionModuleInfo::TTechnologyType newPositioningMethod = KLbsMethodNone;

        for (TInt index = 0; index < entryCount; index++)
            {
            if (ptrToTableEntry->iLSPreferredMode == iActiveLSMode &&
                ptrToTableEntry->iLSAlternativeMode == KLbsMethodNone &&
                ptrToTableEntry->iNRHPreferredMode == GetTechnologyTypeFromNrhRequest(0) &&
                ptrToTableEntry->iNRHAlternativeMode == GetTechnologyTypeFromNrhRequest (1))
            {
                newPositioningMethod = ptrToTableEntry->iResultingMode;
                break;
                }
            ptrToTableEntry++;
            }

        __ASSERT_ALWAYSX(KLbsMethodNone != newPositioningMethod, User::Invariant()); // no match found in combination table
        return newPositioningMethod;
    }

/* This method determines the positioning method that would be sent in a GPS Options
message to the GPS module. It takes into account the positioning methods requested by
both Location Server and NRH.
*/
TPositionModuleInfo::TTechnologyType CManagerMainLogic::CalculatePositioningMethod()
	{

	#if defined(AGPS_MANAGER_TESTING)
	TInt capabilities;
	TUid KSuiteExeUid = {0x102869CB};
	TUint KGpsModeCapabilities = 0;
	TInt error;
	error = RProperty::Get(KSuiteExeUid, KGpsModeCapabilities, capabilities);
	if(error != KErrNone)
	    {
        User::Panic(KLbsAGPSManFault, error);
	    }
	iCapabilities = capabilities;
	#endif
	
	TPositionModuleInfo::TTechnologyType newPositioningMethod = KLbsMethodNone;
	// Calculate positioning method when there are both
	// LS and NRH requests active
	if(iLocSvrLocationRequestHandler->IsRequestActive() && iIsNrhCancelDue)
		{
		newPositioningMethod = LookUpPositioningMethodInCombinationTables();	
		}
	// Positioning method when only the LS has an active request
	else if(iLocSvrLocationRequestHandler->IsRequestActive())
		{
		if (KLbsMethodNone == iActiveLSMode)
		    {
		    // leave mode as it was - cancel after session comepletes
		    newPositioningMethod = iCurrentPositioningMethod;
		    }
		else		
		switch (iCapabilities)
			{
			case TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased:
				__ASSERT_ALWAYSX((iActiveLSMode == KLbsMethodAutonomous ||
								 iActiveLSMode == KLbsMethodTerminalBased), User::Invariant());
				newPositioningMethod = iActiveLSMode;
			break;
			
			case TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted:
				__ASSERT_ALWAYSX(iActiveLSMode == KLbsMethodTerminalAssisted, User::Invariant());
				newPositioningMethod = iActiveLSMode;
			break;
						
			case TPositionModuleInfoExtended::EDeviceGpsModeAutonomous:
				__ASSERT_ALWAYSX(iActiveLSMode == KLbsMethodAutonomous, User::Invariant());
				newPositioningMethod = iActiveLSMode;
			break;
			
			case TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB:
			case (TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased|
				  TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted):
				  __ASSERT_ALWAYSX((iActiveLSMode == KLbsMethodAutonomous ||
				  				   iActiveLSMode == KLbsMethodTerminalBased ||
				  				   iActiveLSMode == KLbsMethodTerminalAssisted), User::Invariant());
				newPositioningMethod = iActiveLSMode;
			break;
			
			default:
				ASSERT(EFalse); // incorrect capabilities set
			break;
			}
		}
	// Positioning method when only the NRH has an active request
	else if (iIsNrhCancelDue)
		{
		if (iActiveLSMode ==KLbsMethodNone)
		    {
		      switch (iCapabilities)
		            {   
		            case TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased:
		                __ASSERT_ALWAYSX(GetTechnologyTypeFromNrhRequest(1) == KLbsMethodNone,
		                                User::Invariant());
		                newPositioningMethod = GetTechnologyTypeFromNrhRequest(0);
		                __ASSERT_ALWAYSX(newPositioningMethod & TPositionModuleInfo::ETechnologyTerminal,
		                                User::Invariant());
		            break;

		            case TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted:
		                __ASSERT_ALWAYSX(GetTechnologyTypeFromNrhRequest(1) == KLbsMethodNone,
		                                User::Invariant());
		                newPositioningMethod = GetTechnologyTypeFromNrhRequest(0);
		                __ASSERT_ALWAYSX(newPositioningMethod == KLbsMethodTerminalAssisted,
		                                User::Invariant());
		            break;
		            
		            case (TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased|
		                  TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted):
		                __ASSERT_ALWAYSX(GetTechnologyTypeFromNrhRequest(1) == KLbsMethodNone,
		                                User::Invariant());
		                newPositioningMethod = GetTechnologyTypeFromNrhRequest(0);
		                __ASSERT_ALWAYSX((newPositioningMethod == KLbsMethodAutonomous ||
		                                 newPositioningMethod == KLbsMethodTerminalBased ||
		                                 newPositioningMethod == KLbsMethodTerminalAssisted),
		                                User::Invariant());
		                
		            break;
		            
		            case TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB:
		                if (GetTechnologyTypeFromNrhRequest(1) != KLbsMethodNone)
		                    {
		                    __ASSERT_ALWAYSX((
		                        (GetTechnologyTypeFromNrhRequest(0) == KLbsMethodTerminalAssisted && GetTechnologyTypeFromNrhRequest(1) == KLbsMethodAutonomous) ||
		                        (GetTechnologyTypeFromNrhRequest(0) == KLbsMethodAutonomous  && GetTechnologyTypeFromNrhRequest(1) == KLbsMethodTerminalAssisted) ||
		                        (GetTechnologyTypeFromNrhRequest(0) == KLbsMethodTerminalBased && GetTechnologyTypeFromNrhRequest(1) == KLbsMethodTerminalAssisted) ||
		                        (GetTechnologyTypeFromNrhRequest(0) == KLbsMethodTerminalAssisted && GetTechnologyTypeFromNrhRequest(1) == KLbsMethodTerminalBased)),
		                         User::Invariant());
		                         
		                    newPositioningMethod = KLbsMethodDual;
		                    }
		                else
		                    {
		                    newPositioningMethod = GetTechnologyTypeFromNrhRequest(0);
		                    __ASSERT_ALWAYSX((newPositioningMethod == KLbsMethodAutonomous ||
		                                 newPositioningMethod == KLbsMethodTerminalBased ||
		                                 newPositioningMethod == KLbsMethodTerminalAssisted),
		                                User::Invariant());
		                    }
		            break;
		            
		            case TPositionModuleInfoExtended::EDeviceGpsModeAutonomous:
		                __ASSERT_ALWAYSX(GetTechnologyTypeFromNrhRequest(1) == KLbsMethodNone,
		                                User::Invariant());
		                newPositioningMethod = GetTechnologyTypeFromNrhRequest(0);
		                __ASSERT_ALWAYSX(newPositioningMethod == KLbsMethodAutonomous,
		                                User::Invariant());
		            break; 
		            
		            default:
		                ASSERT(EFalse); // incorrect capabilities set
		            break;
		            }
		    }
		else
		    {
	        newPositioningMethod = LookUpPositioningMethodInCombinationTables();  
		    }
		}
	else
		{
		// No activity. Make newPositioningMethod equal to current
		// positioning method so that GPS Options are not set.
		newPositioningMethod = iCurrentPositioningMethod;
		}
			
	return newPositioningMethod;
	}
			
			
/*
This method sends a GPS Options configuration message to the GPS module
*/
void CManagerMainLogic::SetGpsOptions(const TPositionModuleInfo::TTechnologyType aNewPositioningMethod)
    {
	if (aNewPositioningMethod == KLbsMethodDual)
		{
		// Use an object of class class TLbsGpsOptionsArray
		// to set gps options to Terminal Based and request
		// that both measurements and position calcuations
		// are returned by the GPS module.
		TLbsGpsOptionsArray optionsArray;
		optionsArray.ClearOptionItems();
		optionsArray.SetGpsMode(CLbsAdmin::EGpsPreferTerminalBased); 

		TLbsGpsOptionsItem optionsItem;
		optionsItem.SetLocUpdateType(TLbsGpsOptionsItem::EPosUpdateCalculation);
		optionsArray.AppendOptionItem(optionsItem);
		optionsItem.SetLocUpdateType(TLbsGpsOptionsItem::EPosUpdateMeasurement);
		optionsArray.AppendOptionItem(optionsItem);
		optionsArray.SetGpsTimeRelationReq(iNRHLocationRequestHandler->GpsTimingOfCellFramesRequested());
			
        LBSLOG(ELogP9, "->S CLbsLocationSourceGpsBase::SetGpsOptions() AGPSModule\n");
        LBSLOG(ELogP9, "  > TLbsGpsOptions gpsOptions  =\n");
		LBSLOG_TLBSGPSOPTIONS(optionsArray);
		iLocationSource->SetGpsOptions(optionsArray);
		iFallbackFromPTAToAutonomousPossible = EFalse;
		}
	else
		{
		// Use an object of class TLbsGpsOptions
		// to set gps options (an object of class 
		// TLbsGpsOptionsArray could also be used, but 
		// using the base class TLbsGpsOptions ensures
		// minimum impact on GPS Integration Modules
		// already getting the base class when there is
		// only one item in the array)
		TLbsGpsOptions options;
		
		switch (aNewPositioningMethod)
			{
			case KLbsMethodTerminalBased:
				options.SetGpsMode(CLbsAdmin::EGpsPreferTerminalBased);
			break;
				
			case KLbsMethodTerminalAssisted:
				CLbsAdmin::TGpsMode mode;
				GetTaGpsModeFromAdmin(mode); // Prefer or Always Terminal Assisted
				options.SetGpsMode(mode);
			break;
				
			case KLbsMethodAutonomous:
				options.SetGpsMode(CLbsAdmin::EGpsAutonomous);
			break;
			}
		options.SetGpsTimeRelationReq(iNRHLocationRequestHandler->GpsTimingOfCellFramesRequested());
				
		LBSLOG(ELogP9, "->S CLbsLocationSourceGpsBase::SetGpsOptions() AGPSModule\n");
		LBSLOG(ELogP9, "  > TLbsGpsOptions gpsOptions  =\n");
		LBSLOG_TLBSGPSOPTIONS(options);
		iLocationSource->SetGpsOptions(options);
        iFallbackFromPTAToAutonomousPossible = EFalse;
		}
    }


// from Mixin class MUpdateRequestObserver
// Manager receives request for a position update (or cancel) 
// from either NRH or location server
// or, on manager startup, this is called whenever there is an outstanding request 
void CManagerMainLogic::OnLocationRequestStateChanged
	(
	const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel,
	const TRequestStateChangeReason& aReason
	)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::OnLocationRequestStateChanged()\n");
	
	if (!iClosingDown)
		{
		if (aChannel.iClientId == KLbsLocServerUid) 
			{
			LBSLOG(ELogP2, "   LS Location Request, Cancel or Timeout\n");
			
			if (iLocSvrLocationRequestHandler->IsRequestActive())
				{
				// New request received from the Location Server.
				// Always switch to EReqReceived state if the request has come from a new client.
				// If it hasn't, never leave a EReqSessionStarted or EReqReceivedDuringSession state.
				if ((iLocSvrLocationRequestHandler->GetNewClient()) ||
				    (iLSReqState != EReqSessionStarted && iLSReqState != EReqReceivedDuringSession))
					{
					iLSReqState = EReqReceived;
					}
				if(iLSReqState == EReqSessionStarted)
					{
					iLSReqState = EReqReceivedDuringSession;
					}
				}
			else
				{
				if (iLSReqState == EReqSessionStarted || iLSReqState == EReqReceivedDuringSession)
					{
					// if we are in the process of cancelling then don't issue another!
					// stay in EReqSessionStarted state!
					__ASSERT_ALWAYSX(((aReason == MLocationRequestHandlerObserver::EReasonTimeout) || (aReason == MLocationRequestHandlerObserver::EReasonCancel)),User::Panic(KLbsAGPSManFault, KErrArgument));
					TInt reason;
					switch (aReason)
						{
						case MLocationRequestHandlerObserver::EReasonTimeout:
							reason = KErrTimedOut;
							break;
						
						case MLocationRequestHandlerObserver::EReasonCancel:
							reason = KErrCancel;
							break;
					
						default:
							// Change this to a big scream after beta2 release of PREQ1624
							reason = KErrCancel;
							break;
						}
					iNetworkGatewayHandler->SendSelfLocationCancel(iSessionId, reason);
					}
				else
					{
					// leave any state except EReqSessionStarted
					iLSReqState = EReqNone;
					}
				}
			
			RestartTimerIfTracking();
			}
		else
			{
			__ASSERT_ALWAYSX(aChannel.iClientId == KLbsNetRequestHandlerUid, User::Panic(KLbsAGPSManFault, KErrArgument));
			
			if (aReason == MLocationRequestHandlerObserver::EReasonCancel)
			    {
				iIsNrhCancelDue = EFalse; // only an explicit cancel can do this.
			    //iActiveLSMode = KLbsMethodNone;
			    }
			
			LBSLOG(ELogP2, "   NRH Location Request, Cancel or Timeout\n");
			}

		// Combine LS and NRH requests and send a request to GPS module
		// after setting the correct GPS options.
		CombineLocationRequests(aChannel.iClientId, aReason);
		}
	else
		{
		LBSLOG_WARN(ELogP2, "   Location Request ignored because closing down");
		}
	}

/** Pass the status to the NG */
void CManagerMainLogic::OnSystemStatusAdvice(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel, TBool aTracking)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::OnSystemStatusAdvice() Begin\n");
	if (aChannel.iClientId == KLbsLocServerUid)
		{
		SendSystemStatusAdvice(aTracking);
		}
	LBSLOG(ELogP1, "CManagerMainLogic::OnSystemStatusAdvice() End\n");
	}
	
	// power mode advice arrives from location server via a loc request
void CManagerMainLogic::OnPowerModeAdvice(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel, CLbsLocationSourceGpsBase::TPowerMode& aPowerMode)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::PowerModeAdvice() Begin\n");
	LBSLOG(ELogP9, "->S CLbsLocationSourceGpsBase::AdvisePowerMode() AGPSModule\n");
	LBSLOG2(ELogP9, "  > CLbsLocationSourceGpsBase::TPowerMode aMode  = %d\n", aPowerMode);
	LBSLOG2(ELogP9, "  > RLbsPositionUpdateRequests::TChannelIdentifer aChannel  = 0x%08X\n", aChannel.iClientId);

	if (!iClosingDown)
		{
		CLbsLocationSourceGpsBase::TPowerMode powerMode = CLbsLocationSourceGpsBase::EPowerModeUnknown;
		CLbsLocationSourceGpsBase::TPowerMode otherChannelPowerMode = CLbsLocationSourceGpsBase::EPowerModeUnknown;
		
		//Find out who is sending the power advice and update the correct memory variable
		if(aChannel.iClientId == KLbsLocServerUid)
			{
			//Power Advice from LocServer
			iPowerModeLS = aPowerMode;
			otherChannelPowerMode = iPowerModeNRH;
			}
		else
			{
			//Power Advice from NRH
			iPowerModeNRH = aPowerMode;
			otherChannelPowerMode = iPowerModeLS;
			}
		
		//Need to decide what powerMode to send to the module.  We need to ensure that we are not 
		// overwriting a "higher" priority power advice from the other channel
		//(i.e. If the NRH previously sent an ON and the LS is now sending an OFF, we should not send
		// the OFF, since the NRH still requires the hardware to be on, so the LS power mode is ignored)
		switch(aPowerMode)
			{
			case CLbsLocationSourceGpsBase::EPowerModeOn:
				{
				//Always send an ON power advice irrespective of what the other advice is saying
				powerMode = aPowerMode;
				break;
				}
			
			case CLbsLocationSourceGpsBase::EPowerModeStandby:
				{
				//Send standby Power mode only if the other channel is not currently "sending" On
				if(otherChannelPowerMode != CLbsLocationSourceGpsBase::EPowerModeOn)
					{
					powerMode = aPowerMode;
					}
				break;
				}
	
			case CLbsLocationSourceGpsBase::EPowerModeOff:
				{
				//Send Off power mode only if the other channel is not "sending" On or Standby
				if((otherChannelPowerMode != CLbsLocationSourceGpsBase::EPowerModeOn) &&
						(otherChannelPowerMode != CLbsLocationSourceGpsBase::EPowerModeStandby))
					{
					powerMode = aPowerMode;
					}
				break;
				}
		
			case CLbsLocationSourceGpsBase::EPowerModeClose:
				{
				//Send Off power mode only if the other channel is not "sending" On, Standby or Off
				if((otherChannelPowerMode != CLbsLocationSourceGpsBase::EPowerModeOn) &&
						(otherChannelPowerMode != CLbsLocationSourceGpsBase::EPowerModeStandby) &&
						(otherChannelPowerMode != CLbsLocationSourceGpsBase::EPowerModeOff))
					{
					powerMode = aPowerMode;
					}
				break;
				}
	
			case CLbsLocationSourceGpsBase::EPowerModeUnknown:
			default:
				{
				//Do nothing, will send no powerMode to the module
				break;
				}
		}
	
		//Check to see whether we actually made a decision about which powerMode to send
		if(powerMode != CLbsLocationSourceGpsBase::EPowerModeUnknown)
			{
			LBSLOG(ELogP1, "PowerAdvice sent to Module\n");
			iLocationSource->AdvisePowerMode(powerMode); // inform integration module
			}
		
		}
	else
		{
		LBSLOG_WARN(ELogP2, "   Power advice ignored because closing down");
		}


	LBSLOG(ELogP1, "CManagerMainLogic::OnPowerModeAdvice() End\n");
	}

// from MLbsCloseDownObserver
void CManagerMainLogic::OnProcessCloseDown()
	{
	LBSLOG(ELogP1, "CManagerMainLogic::OnProcessCloseDown() Begin\n");
	iClosingDown = ETrue;
	
	// Cancel any outstanding request
	LBSLOG(ELogP9, "->S CLbsLocationSourceGpsBase::CancelLocationRequest() AGPSModule\n");
	LBS_RDEBUG("LBS", "AGPSMod", "CancelLocationRequest");		
	iLocationSource->CancelLocationRequest();

	// instruct integration module to shutdown by sending it Power Mode Advise 
	LBSLOG(ELogP9, "->S CLbsLocationSourceGpsBase::AdvisePowerMode() AGPSModule\n");
	LBSLOG(ELogP9, "  > CLbsLocationSourceGpsBase::TPowerMode aMode  = EPowerModeClose\n");
	LBS_RDEBUG_ARGINT("LBS", "AGPSMod", "AdvisePowerMode", CLbsLocationSourceGpsBase::EPowerModeClose);		
	iLocationSource->AdvisePowerMode(CLbsLocationSourceGpsBase::EPowerModeClose);
	delete iCloseDownRequestDetector;
	iCloseDownRequestDetector = NULL;
	
	LBSLOG(ELogP1, "CManagerMainLogic::OnProcessCloseDown() End\n");	
	}


/** Disables tracking if the Loc Server stopps reissuing requests */
void CManagerMainLogic::OnTimerEventL(TInt /*aTimerId*/)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::OnTimerEventL() Begin\n");
	SendSystemStatusAdvice(EFalse);
	LBSLOG(ELogP1, "CManagerMainLogic::OnTimerEventL() End\n");
	}
	
TInt CManagerMainLogic::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	LBSLOG(ELogP1, "CManagerMainLogic::OnTimerError() Begin\n");
	User::Panic(KLbsAGPSManFault, aError);
	LBSLOG(ELogP1, "CManagerMainLogic::OnTimerError() End\n");
	return aError;
	}

void CManagerMainLogic::SendSystemStatusAdvice(TBool aTracking)
	{
	// if we are sending the state for the first time then just send it
	// if it's not the first time then only send the state when it changes
	if ((iTrackingStateUnknown) || (iLastTrackingFlagSentToNG != aTracking))
		{
		iLastTrackingFlagSentToNG = aTracking;
		iNetworkGatewayHandler->SendSystemStatusAdvice(iLastTrackingFlagSentToNG);
		}
	
	iTrackingStateUnknown = EFalse; // the tracking state is now known!

	RestartTimerIfTracking();
	}

void CManagerMainLogic::RestartTimerIfTracking()
	{
	LBSLOG(ELogP1, "CManagerMainLogic::RestartTimerIfTracking() Begin\n");
	
	iTimer->Cancel();
	
	// If tracking is on the location server must send another message before the timeout
	// Failing to do so will send a Tracking Off message to the NG
	if ( (iTrackingStateUnknown) || ((iLastTrackingFlagSentToNG == TRUE) && !iLocSvrLocationRequestHandler->IsRequestActive()))
		{
		iTimer->EventAfter(static_cast<TTimeIntervalMicroSeconds>(KTrackingOffTimeout), 0);
		}
	LBSLOG(ELogP1, "CManagerMainLogic::RestartTimerIfTracking() End\n");
	}


void CManagerMainLogic::EarlyCompletionUpdate(TPositionInfo& aEarlyCompletionUpdate, TTime& aTargetTime, TTime& aActualTime)
	{
	// Cancel the location request with the integration module if the MO-LR (selflocate session) is still in progress.
	// If the MO-LR (selflocate session) is no longer in progress (EReqSessionCompleted or EReqNone) then only 
	// cancel the location request if there isn't an MT-LR in progress (inactive NRH request) to prevent cancelling a
	// potential emergency request.
	if ((iLSReqState == EReqSessionStarted) || (iLSReqState == EReqReceivedDuringSession) ||
		(((iLSReqState == EReqSessionCompleted)||(iLSReqState == EReqNone)) && (!iNRHLocationRequestHandler->IsRequestActive())))
		{
		TInt posSize = aEarlyCompletionUpdate.PositionClassSize();
		// Just publish this position for the NRH (since it is published with KPositionEarlyComplete code
		// the LocServer will ignore it). The position is of TPositionExtendedSatelliteInfo type.
		// iPositionUpdates.SetPositionInfo(KPositionEarlyComplete, EFalse, &aEarlyCompletionUpdate, sizeof(TPositionExtendedSatelliteInfo), aTargetTime, aActualTime);
		iPositionUpdates.SetPositionInfo(KPositionEarlyComplete, EFalse, &aEarlyCompletionUpdate, sizeof(TPositionExtendedSatelliteInfo), aTargetTime, aActualTime);
		
		// invalidate requests
		iLocSvrLocationRequestHandler->InvalidateRequest();
		iNRHLocationRequestHandler->InvalidateRequest();
		
		// cancel the location request
		LBSLOG(ELogP9, "->S CLbsLocationSourceGpsBase::CancelLocationRequest() AGPSModule\n");
		LBS_RDEBUG("LBS", "AGPSMod", "CancelLocationRequest");		
		iLocationSource->CancelLocationRequest();
		}
	}


/*
This method is only called when it is expected (for consistency with the
hardware capabilities) that a Terminal Assisted positioning method is set
in Admin DB. The reason this method is called is to determine what variety
of Terminal Assisted ("Prefer" or "Always") was set.
*/
void CManagerMainLogic::GetTaGpsModeFromAdmin(CLbsAdmin::TGpsMode& aGpsMode)
{
	LBSLOG(ELogP1, "CManagerMainLogic::GetTaGpsModeFromAdmin() Begin\n");
	TLbsAdminSetting adminSetting;
	TInt err;
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus netRegStatus;

	// Read the current network registration status
	err = iNetRegStatus.GetNetworkRegistrationStatus(netRegStatus);
	
	if (err != KErrNone)
		{
		ASSERT(EFalse);
		// Assume roaming network to err on the safe side as nothing better can be done
		netRegStatus = RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork;	
		}
	
	// Determine precise admin setting to be read (home or roaming gps mode)
	switch (netRegStatus)
		{
			case RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork:
				adminSetting = KLbsSettingRoamingGpsMode;
			break;
			
			default: // roaming and unknown (assume roaming)
			 	adminSetting = KLbsSettingRoamingGpsMode;
			break;
		}

	// Read value of admin setting
	if (KErrNone !=  iAdmin->Get(adminSetting, aGpsMode))
		{	
		// Assume PreferTerminalAssisted
		ASSERT(EFalse);
		aGpsMode = CLbsAdmin::EGpsPreferTerminalAssisted;	
		}
	else
		{
		// Verify that the mode is truly a TA variety
		if ( (aGpsMode != CLbsAdmin::EGpsAlwaysTerminalAssisted) &&
			 (aGpsMode != CLbsAdmin::EGpsPreferTerminalAssisted))
			{
			// Assume PreferTerminalAssisted (nothing else can be done)
			aGpsMode = CLbsAdmin::EGpsPreferTerminalAssisted;
			}
		}
	
	LBSLOG(ELogP1, "CManagerMainLogic::GetTaGpsModeFromAdmin() End\n");
}

void CManagerMainLogic::ConvertToTLbsLocRequestQuality(const TLbsLocRequestQualityInt& aSource,
			TLbsLocRequestQuality& aDest) const
	{
	aDest.SetMaxFixTime(aSource.MaxFixTime());
	aDest.SetMinHorizontalAccuracy(aSource.MinHorizontalAccuracy());
	aDest.SetMinVerticalAccuracy(aSource.MinVerticalAccuracy());	
	}




