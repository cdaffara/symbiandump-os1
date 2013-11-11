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
// @file tlbsutils.h
// This is the header file which contains the common utility functions
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef __T_LBS_UTILS_H__
#define __T_LBS_UTILS_H__


// Epoc includes
#include <e32cmn.h>

// LBS includes
#include <lbs.h>
#include <lbscommon.h>
#include <lbspositioninfo.h>
#include <lbs/lbsassistancereferencetime.h>
#include <lbsclasstypes.h>
#include <lbs/lbsadmin.h>
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsgpsmeasurement.h>
#include <lbs/lbslocdatasourcegpsbase.h>

// Forward Declaration

// Includes for Log Comparison
#include <lbs/lbslogevent.h>
#include <lbs/lbsselflocatelogevent.h>
#include <lbs/lbsexternallocatelogevent.h>
#include <lbs/lbstransmitlocationlogevent.h>
#include <lbs/lbsnetworklocatelogevent.h>
#include <lbs/lbsassistancedatalogevent.h>




// Property Keys
enum TLbsTestPropertyKeys
	{
	ELbsTestAGpsModuleIn = 1301,
	ELbsTestAGpsModuleOut = 2301,
	ELbsTestClockPluginOut = 3301,
	ELbsTestAGpsModuleModeChanges = 4301,
	ELbsTestAGpsModuleResetAssistanceDataFlag = 5301,
	ELbsTestAGpsModuleCancelCount = 6301,
	//ELbsTestAGpsModuleAssistanceDataRequestFlag = 7301  //NOTE: This property is defined in tlbsdefs.h - please do not use value 7301 again
	ELbsTestAGpsModuleClientMode = 8301
	};

enum KLbsTestAGpsModuleResetAssistanceDataStates
	{
	ELbsTestAGpsResetAssistanceDataNotReceived,
	ELbsTestAGpsResetAssistanceDataReceived
	};


const TInt KMaxModuleDataBusIniFileName = 100;

// assistance data providers:
const TInt KSimpleAssistanceDataProviderPluginUidValue = 0x10281D77;
const TInt KSuplAssistanceDataProviderPluginUidValue = 0x10285AC2;


/** Update structure to hold both measurement and position (location) information.
*/
class TLbsModuleUpdateItem
	{
public:
	IMPORT_C TLbsModuleUpdateItem();
	
	IMPORT_C const TPositionExtendedSatelliteInfo& Position() const;
	IMPORT_C const TPositionGpsMeasurementInfo& Measurement() const;
	IMPORT_C TInt Error() const;
	IMPORT_C TInt Delay() const;
	
	IMPORT_C void SetPosition(const TPositionExtendedSatelliteInfo& aReport);
	IMPORT_C void SetMeasurement(const TPositionGpsMeasurementInfo& aMeasurement);
	IMPORT_C void SetError(TInt aError);
	IMPORT_C void SetDelay(TInt aDelay);
	

protected:	
	TPositionExtendedSatelliteInfo		iPosition;
	TPositionGpsMeasurementInfo	iMeasurement;
	TInt						iError;
	TInt						iDelay;
	};



// Passing data to test module.
class TModuleDataIn
	{
public:
	/** Supported request types. */
	enum TModuleRequestType
		{
		EModuleRequestConfig,
		EModuleRequestEventKickOff,
		EModuleRequestTestMode,
		EModuleRequestTimeOut,
		EModuleRequestError,
		EModuleRequestTimeStampOffset,
		EModuleRequestOptionsChangeReportMode
		};
	TModuleRequestType iRequestType;

	
	/** Config request data. */
	TBuf<KMaxModuleDataBusIniFileName>	iConfigFileName;
	TBuf<40>							iConfigSection;	// bug if you make it bigger, more data we add, we get a -9 buf overflow on the property read

	
	/** Event kick off request data. */
	enum _TKickOffEventType
		{
		EKickOffEventTypeDevice=0x01, 
		EKickOffEventTypeDataQuality=0x02
		};	
	typedef TUint TKickOffEventType;
	TKickOffEventType iKickOffEventType;

	
	/** Assistance event data. */
	enum TAssDataEventType
		{	
		EAssDataEventExpectSingle,
		EAssDataEventExpectMultiple
		};
	TAssDataEventType iAssDataEventType;	


	/** Module test mode. */
	enum TTestMode	
		{
		EModuleTestModeAssDataOff,
		EModuleTestModeAssDataOn,
		EModuleTestModeAssDataOn_TimeStamp,
		EModuleTestModeAssDataOn_Cancel_Stage1,
		EModuleTestModeAssDataOn_Cancel_Stage2,
		EModuleTestModeAssDataOn_Unsolicited,
		EModuleTestModeAssDataOn_NoNetwork,
		EModuleTestModeAssDataOn_DataNotAvailable,
		EModuleTestModeAssDataOn_SomeDataNotAvailable,
		EModuleTestModeAssDataOn_InvalidDataRequest,
		EModuleTestModeAssDataOn_InvalidDataGet,
		EModuleTestModeAssDataOn_EarlyPosUpdates
		};
	TTestMode iAssDataTestMode;

	enum TGpsOptionsChangeReportMode
		{
		EGpsModuleDoReportOptionsChange,
		EGpsModuleDoNotReportOptionsChange,
		};
	TGpsOptionsChangeReportMode iOptionsChangeReportMode;

	/** Module time out. */
	TTimeIntervalMicroSeconds iTimeOut;

	TBool iOneOffTimeOut;
	
	/** Module Error Code */
	TInt iError;

	/** Used to alter the time stamp of the posinfo. */
	TTimeIntervalMicroSeconds iTimeStampOffset;
	};


// Retriving data from the test module.
class TModuleDataOut 
	{
public:
	
	enum TModuleRequestType
		{
		EModuleResponse,
		EModuleGpsMode,
		};
	TModuleRequestType iRequestType;

    enum TModuleResponseType
		{
		EModuleResponseOk,
		EModuleAssDataRequestedOk,
		EModuleAssDataOk,
		EModuleAssDataCancelledOk, // may not be possible to verify from within module, hence may remove
		EModuleStartupOk,
		EModuleShutdownOk,
		EModuleErr	/** Set if any of the above failed. */
		};

	TModuleResponseType iResponse;
    CLbsAdmin::TGpsMode iGpsMode; /** Used to retrive GPS mode from the module. */
	};	


// Retriving data from the test clock plugin.
class TClockPluginDataOut
	{
public:
	enum TClockPluginResponseType
		{
		EClockPluginResponseOk,
		EClockPluginErr	/** Set if any of the above failed. */
		};

	TClockPluginResponseType iResponse;
	};

// Count of cancels
class TCancelCount
	{
public:
	TUint iCancelCount;
	};

// Assistance Data Simple Provider Default values.
const TUint KAssistanceData_RefTime_Week = 12;
const TUint KAssistanceData_RefTime_Tow1MSec = 567;

// Module Info values
/*
 * the A-GPS value are from testproduct\agpsmodule\group\xtestmodule.ini
 * most the AGPS module module info are the same as the LbsLocDataSourcePlugins\sirf\group\sirfdatasource.ini
 * except "Module name", "Version", "ClassesSupported", "DataSourceId"
 */
//Module ID
#define APGS_MODINFO_MODULE_ID      			271064388
#define NETWORK_MODINFO_MODULE_ID   			271064387

//Module name   
#define AGPS_MODINFO_MOD_NAME                  	_L("TEST_Loc_AGPS_Module")
#define NETWORK_MODINFO_MOD_NAME               	_L("NetLocManager")
//
#define AGPS_MODINFO_IS_AVAILABLE			    (ETrue)
#define NETWORK_MODINFO_IS_AVAILABLE			(ETrue)
//Technology type
#define AGPS_MODINFO_TECHNOLOGY_TYPE            (TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyTerminal | \
																					  TPositionModuleInfo::ETechnologyAssisted))
#define NETWORK_MODINFO_TECHNOLOGY_TYPE         (TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyNetwork))
//version
#define AGPS_MODINFO_VERSION                    (TVersion(1,2,3))
#define NETWORK_MODINFO_VERSION                 (TVersion(0,1,1))
//device location
#define AGPS_MODINFO_DEVICE_LOCATION            (TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceInternal))
#define NETWORK_MODINFO_DEVICE_LOCATION         (TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceExternal))
//capabilities
#define AGPS_MODINFO_CAPABILITIES 				(TPositionModuleInfo::TCapabilities(TPositionModuleInfo::ECapabilityHorizontal | \
																					TPositionModuleInfo::ECapabilityVertical | \
																					TPositionModuleInfo::ECapabilitySpeed | \
																					TPositionModuleInfo::ECapabilityDirection | \
																					TPositionModuleInfo::ECapabilitySatellite))
#define NETWORK_MODINFO_CAPABILITIES			(TPositionModuleInfo::TCapabilities(TPositionModuleInfo::ECapabilityHorizontal| \
																					TPositionModuleInfo::ECapabilityVertical | \
																					TPositionModuleInfo::ECapabilitySpeed | \
																					TPositionModuleInfo::ECapabilityDirection))
//classes supported
#define AGPS_MODINFO_CLASSES_SUPPORTED_POSITION_FAMILY 		(EPositionInfoClass|EPositionGenericInfoClass| \
															 EPositionCourseInfoClass|EPositionSatelliteInfoClass)
#define NETWORK_MODINFO_CLASSES_SUPPORTED_POSITION_FAMILY 	(EPositionInfoClass|EPositionGenericInfoClass| \
															 EPositionCourseInfoClass|EPositionSatelliteInfoClass)
//time to first fix
#define AGPS_MODINFO_TIME_TO_FIRST_FIX         (TTimeIntervalMicroSeconds(90000000))
#define NETWORK_MODINFO_TIME_TO_FIRST_FIX      (TTimeIntervalMicroSeconds(20000000))
//time to next fix
#define AGPS_MODINFO_TIME_TO_NEXT_FIX          (TTimeIntervalMicroSeconds(1000000))
#define NETWORK_MODINFO_TIME_TO_NEXT_FIX       (TTimeIntervalMicroSeconds(20000000))
//horizontal accuracy
#define AGPS_MODINFO_HORIZONTAL_ACCURACY       5.4f
#define NETWORK_MODINFO_HORIZONTAL_ACCURACY    100.0f
//vertical accuracy
#define AGPS_MODINFO_VERTICAL_ACCURACY         5.4f
#define NETWORK_MODINFO_VERTICAL_ACCURACY      100.0f
//cost indication
#define AGPS_MODINFO_COST_INDICATOR            (TPositionQuality::ECostPossible)
#define NETWORK_MODINFO_COST_INDICATOR         (TPositionQuality::ECostCharge)
//power consumption
#define AGPS_MODINFO_POWER_CONSUMPTION         (TPositionQuality::EPowerLow)
#define NETWORK_MODINFO_POWER_CONSUMPTION      (TPositionQuality::EPowerLow)

/**
 Current LBS system has two positioning module: A-GPS and Network Loc manager
 This value might be changed if more positioning modules are added in the future
*/
#define MAX_POSITIONING_MODULES					2

class TCTClientDetails
	{
public:
	inline TCTClientDetails()
	: iSessionIdDelay(0), iSessionStepDelay(0), iCancelRequest(EFalse), iSessionIdCancel(0), iSessionStepCancel(0), iNumberOfNPUDs(1), iUpdateOptions(0), iPsyId(0), iExpectedError(0)
	{}
public:
	TInt iSessionIdDelay;
	TInt iSessionStepDelay;
	TBool iCancelRequest;
	TInt iSessionIdCancel;
	TInt iSessionStepCancel;	
	TInt iNumberOfNPUDs;
	TPositionUpdateOptions iUpdateOptions;
	TInt	iPsyId;
	TInt	iExpectedError;
	};

// Implements a class to contain the utility functions.
class T_LbsUtils
	{	
public:
	enum TComparisonAccuracyType
		{
		ERoughAccuracy, EExactAccuracy
		};

	IMPORT_C T_LbsUtils();
	IMPORT_C ~T_LbsUtils();

	IMPORT_C void CreateTestPropertiesL();
	// 
	IMPORT_C void GetConfigured_ModuleInfoL(const TDesC& aConfigFileName, const TDesC& aConfigSection, TPositionModuleInfo& aModInfo);

	IMPORT_C void GetConfigured_PosInfosL(const TDesC& aConfigFileName, const TDesC& aConfigSection, RPointerArray<TAny>& aPosInfoArr);
	IMPORT_C void GetConfigured_UpdateArrayL(const TDesC& aConfigFileName, const TDesC& aConfigSection, RPointerArray<TLbsModuleUpdateItem>& aUpdateArr);
	IMPORT_C void GetConfigured_ModuleStatusL(const TDesC& aConfigFileName, const TDesC& aConfigSection, TPositionModuleStatus& aModuleStatus, TBool& aDelayReportingModStatus);
	IMPORT_C void GetConfigured_ModuleUpdateOptionsL(const TDesC& aConfigFileName, const TDesC& aConfigSection, TPositionUpdateOptions& aUpdateOpts);
	IMPORT_C void GetConfigured_ClientDetailsL(const TDesC& aConfigFileName, const TDesC& aConfigSection, 
									RArray<TCTClientDetails>& aClientDetailsArray);

	IMPORT_C void GetExpected_ModuleModes(const TDesC& aConfigFileName, const TDesC& aConfigSection, RPointerArray<TLbsGpsOptions>& aModuleModes);

	IMPORT_C void NotifyModuleOfConfigChangeL(const TModuleDataIn& aModuleDataIn);
	IMPORT_C void WaitForModuleToRequestAssistanceDataL();

	IMPORT_C TBool Compare_PosInfo(const TPositionInfoBase& aPosInfoSideA, const TPositionInfoBase& aPosInfoSideB);
	IMPORT_C TBool Compare_PosInfo(const TPositionInfoBase& aPosInfoSideA, const TPositionInfoBase& aPosInfoSideB, TComparisonAccuracyType aCmpAccuracy);
	 
	IMPORT_C TBool Compare_PosInfoArr(RPointerArray<TAny>& aPosInfoArrSideA, RPointerArray<TAny>& aPosInfoArrSideB);
	IMPORT_C TBool Compare_PosInfoArr(RPointerArray<TAny>& aPosInfoArrSideA, RPointerArray<TAny>& aPosInfoArrSideB, TComparisonAccuracyType aCmpAccuracy);

	IMPORT_C void ResetAndDestroy_PosInfoArr(RPointerArray<TAny>& aPosInfoArr);
	
	IMPORT_C TPositionModuleId GetNetworkModuleIdL(RPositionServer& aServer);
	IMPORT_C TPositionModuleId GetAGpsModuleIdL(RPositionServer& aServer);
	IMPORT_C TPositionModuleId GetBadModuleId();
	
	IMPORT_C TBool Compare_ModuleInfo(TPositionModuleInfo& aModInfoSideA, TPositionModuleInfo& aModInfoSideB);
	IMPORT_C TBool Compare_PositionQuality(TPositionQuality& aQualitySideA, TPositionQuality& aQualitySideB);
	IMPORT_C TBool Compare_ModuleStatus(TPositionModuleStatus& aModStatusSideA, TPositionModuleStatus& aModStatusSideB);
	IMPORT_C TBool Compare_ModuleUpdateOptions(TPositionUpdateOptions& aModUpdateOptsSideA, TPositionUpdateOptions& aModUpdateOptsSideB);
	IMPORT_C TBool Verify_PositionIsValid(TPositionInfo& aPosInfo);

	// Assistance data functions
	// IMPORT_C void CreateSimpleAssistanceData(/*TLbsAssistanceDataGroup aDataMask, RDataReaderRootBase& aGpsRefTimeReader*/);
	IMPORT_C TBool VerifySimpleAssistanceData(RUEPositioningGpsReferenceTimeReader& aGpsRefTimeReader);
    IMPORT_C void  Create_ModuleInfoL(TPositionModuleId aModuleId, TPositionModuleInfo& aModInfo);


	IMPORT_C TBool Compare_LogBase(const CLbsLogEvent& aLogEventSideA, const CLbsLogEvent& aLogEventSideB);
	IMPORT_C TBool Compare_SelfLocateLog(const CLbsSelfLocateLogEvent& aSelfLocateLogSideA, const CLbsSelfLocateLogEvent& aSelfLocateLogB);
	IMPORT_C TBool Compare_NetworkLocateLog(const CLbsNetworkLocateLogEvent& aNetworkLocateLogSideA, const CLbsNetworkLocateLogEvent& aNetworkLocateLogB);
	IMPORT_C TBool Compare_ExternalLocateLog(const CLbsExternalLocateLogEvent& aExternalLocateLogSideA, const CLbsExternalLocateLogEvent& aExternalLocateLogSideB);
	IMPORT_C TBool Compare_TransmitLocationLog(const CLbsTransmitLocationLogEvent& aX3PLogSideA, const CLbsTransmitLocationLogEvent& aX3PLogSideB);
	IMPORT_C TBool Compare_AssistanceDataLog(const CLbsAssistanceDataLogEvent& aAssistanceDataLogSideA, const CLbsAssistanceDataLogEvent& aAssistanceDataLogSideB);
	
	// Needed to compare destinations within TransmitLocation Logs
	TBool Compare_Destinations(TDesC16& aDestinationFromRealLog, TDesC16& aDestinationFromExpectedLog);
	

	IMPORT_C TInt SupportedType(const TUint32& aType, const TInt& aSize);
	IMPORT_C TInt CopyPositionTypes(TPositionInfoBase& aTo, const TPositionInfoBase& aFrom);
	
	IMPORT_C void ResetIntegrationModulesCountOfCancelsL();
	IMPORT_C void IncrementIntegrationModulesCountOfCancels();
	IMPORT_C TUint IntegrationModulesCountOfCancelsL();
private:
	TInt GetModIdByTechnology(RPositionServer& aServer, TPositionModuleInfo::TTechnologyType aTechnologyType, TPositionModuleId& aModuleId);
	// Needed to compare external locate logs.
	TBool Compare_ExternalRequestPrivacyRequestParams(const TLbsLoggingPrivacyRequestParams& aLogPrivacyRequestParamsSideA, const TLbsLoggingPrivacyRequestParams& aLogPrivacyRequestParamsSideB);
	TBool Compare_ExternalRequestPrivacyResponseParams(const TLbsLoggingPrivacyResponseParams& aExternalRequestPrivacyResponseSideA, const TLbsLoggingPrivacyResponseParams& aExternalRequestPrivacyResponseSideB);
	TBool Compare_ExternalRequestInfo(const TLbsExternalRequestInfo& aExternalRequestInfoSideA, const TLbsExternalRequestInfo& aExternalRequestInfoSideB);
	TBool Compare_ExternalRequestPrivacy(const TLbsNetPosRequestPrivacy& aExternalRequestPrivacySideA, const TLbsNetPosRequestPrivacy& aExternalRequestPrivacySideB);
	};


/** Possible set of Module options. 
	Held by AGps Hybrid module as bits in a 32 bitmap.
*/
enum TLbsHybridModuleOptions
{
	EHybridModuleOptions_ClearAll = 0x0000,
	ELbsHybridModuleOptions_AssistanceDataOn = 0x0001,
	ELbsHybridModuleOptions_MultiUpdatesOn = 0x0002,
	ELbsHybridModuleOptions_DisableReqAssistData = 0x0004,
	ELbsHybridModuleOptions_DynamicAssistanceData = 0x008,	//Assistance Data delivery variant used in SUPL tests
	ELbsHybridModuleOptions_SetAll = 0x000F
};

#endif //__T_LBS_UTILS_H__
