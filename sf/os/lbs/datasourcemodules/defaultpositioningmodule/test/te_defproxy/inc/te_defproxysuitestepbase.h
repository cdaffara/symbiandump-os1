/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file Te_defproxySuiteStepBase.h
 @internalTechnology
*/

#ifndef __TE_DEFPROXY_STEP_BASE__
#define __TE_DEFPROXY_STEP_BASE__
#include <test/testexecutestepbase.h>
#include <e32base.h>
#include <lbs.h>
#include <lbssatellite.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_mposmodulesobserver.h>
#include "te_defproxysuitedefs.h"

#include <centralrepository.h>
#include <lbsfieldids.h>

/****************************************************************************
	Copied from test psy .h files to remove dependency on them
*****************************************************************************/
/** HPositioneGenericInfo Field IDs used to request behavior
    or information from the PSY */
enum TIntGpsPsy1PositionCommandFieldIds
{
    KIntGpsPsy1FieldStart =                   EPositionFieldProprietaryFieldsBegin + 300,
    KIntGpsPsy1ConfigPsyUid,                 /** The UID of PSY to be configured. if the
                                                 UID dose not match, PSY shall complete request
                                                 with KErrNotSupported*/
                                        
    KIntGpsPsy1ConfigItemsNumber,            /** Number of config items after this field */
};

struct TPsyLRResponseConfig
    {
    TInt iNumOfResponse; //0 indicate for ever with same response
    TInt iErr; //Error code
    TInt iResponseTime; //In seconds, 0 means imediately
    TReal iLat;
    TReal iLon;
    TReal iAlt;
    TInt iNmeaDataSize; //If this is not zero, then returned 
    //HGenericInfo contains some NMEA data
    };
    
struct TPsyModuleStatusConfig
    {
    TInt iDeviceStatus;
    TInt iDataQuality;
    TInt iResponseTime; //Delay when change the module status. In seconds.
    };

struct TPsyConfig
    {
    enum
        {
        EConfigLRResponse,
        EConfigModuleStatus
        };
        
    TInt iType;
    union
        {
        TPsyLRResponseConfig iLRConfig;
        TPsyModuleStatusConfig iStatusConfig;
        } iData;
    };

/** HPositioneGenericInfo Field IDs used to request behavior
    or information from the PSY */
enum TPSY6PositionCommandFieldIds
{
    KPSY6FieldStart =                   EPositionFieldProprietaryFieldsBegin + 100,
    KPSY6FieldDelayProcessing,          /** PSY delays processing of the request 
                                            for timeout, defined in 
                                            TTimeIntervalMicroSeconds */
    KPSY6FieldGetMaxAge,                /** PSY returns result of GetMaxAge function.
                                            type of the field is TTime */
    KPSY6FieldEventLog,                 /** PSY returns list of last-logged events in
                                            a form of TQueryEvent[] as TDesC field */
    KPSY6FieldEventLogSize,             /** The Size of Event Log is return in the field */
    KPSY6FieldEventLogReset,            /** PSY will start event log from scratch */
    KPSY6FieldProcessingStartTime,      /** PSY sets home time when the request processing
                                            has been started */
    KPSY6FieldRequestedResult,          /** PSY will complete with request code. Use to get
                                            KPositionQualityLoss or 
                                            KPositionPartialUpdate */
    KPSY6FieldRequestedUid,             /** PSY will set requested Module Id. TUint32 */
    KPSY6FieldLeaveOnStartTracking,     /** PSY will leave on next StartTrackingL call */
    KPSY6FieldReportStatus,             /** PSY will ReportStatus before request 
                                            handling. Field must contain prepared
                                            TPositionModuleStatus object */
};

/** Event types */
enum EEventType 
{
    EUpdate,                            /** when NotifyPositionUpdate called */
    ECancel,                            /** when CancelNotifyPositionUpdate called */
    ETrackingStart,                     /** when StartTracking called */
    ETrackingStop                       /** when StopTracking called */
};

/** Event properties */
struct TQueryEvent
{
    EEventType      iEventType;
    TTime           iTime;
};

/****************************************************************************
 End of psy .h copied code
*****************************************************************************/

// Location request config
//LRResponse, NumOfResponse, Err, ResponseTime, Lat, Lon, Alt, NmeaSize
//One LR completed with KErrNone
const struct TPsyConfig KConfigLRNoError = 
    {TPsyConfig::EConfigLRResponse, {1, 0, 1, 1.23, 2.34, 10, 0}};
const struct TPsyConfig KConfigLRNoError10s = 
    {TPsyConfig::EConfigLRResponse, {1, 0, 10, 1.23, 2.34, 10, 0}};
const struct TPsyConfig KConfigLRNoError20s = 
    {TPsyConfig::EConfigLRResponse, {1, 0, 20, 1.23, 2.34, 10, 0}};
const struct TPsyConfig KConfigLRNoError32s = 
    {TPsyConfig::EConfigLRResponse, {1, 0, 32, 1.23, 2.34, 10, 0}};
const struct TPsyConfig KConfigLRNoError35s = 
    {TPsyConfig::EConfigLRResponse, {1, 0, 35, 1.23, 2.34, 10, 0}};
const struct TPsyConfig KConfigLRNoError45s = 
    {TPsyConfig::EConfigLRResponse, {1, 0, 45, 1.23, 2.34, 10, 0}};
const struct TPsyConfig KConfigLRNoError1s = 
    {TPsyConfig::EConfigLRResponse, {1, 0, 1, 1.23, 2.34, 10, 0}};
const struct TPsyConfig KConfigLRNoError5s = 
    {TPsyConfig::EConfigLRResponse, {1, 0, 5, 1.23, 2.34, 10, 0}};

//One LR completed with KErrGeneral
const struct TPsyConfig KConfigLRErrGeneral1s = 
    {TPsyConfig::EConfigLRResponse, {1, KErrGeneral, 1, 1.23, 2.34, 10, 0}};
const struct TPsyConfig KConfigLRErrGeneral5s = 
    {TPsyConfig::EConfigLRResponse, {1, KErrGeneral, 5, 1.23, 2.34, 10, 0}};
const struct TPsyConfig KConfigLRErrGeneral35s = 
    {TPsyConfig::EConfigLRResponse, {1, KErrGeneral, 35, 1.23, 2.34, 10, 0}};
const struct TPsyConfig KConfigLRErrNoMemory = 
    {TPsyConfig::EConfigLRResponse, {1, KErrNoMemory, 1, 1.23, 2.34, 10, 0}};


//KErrPositionBufferOverflow
const struct TPsyConfig KConfigLRErrBufferOverflow = 
    {TPsyConfig::EConfigLRResponse, {1, KErrPositionBufferOverflow, 1, 1.23, 2.34, 10, 0}};
    
const struct TPsyConfig KConfigLRHGeneric10s = 
    {TPsyConfig::EConfigLRResponse, {1, KErrNone, 10, 1.23, 2.34, 10, 1}};

//EConfigModuleStatus, device status, data quality, response time
const struct TPsyConfig KConfigStatusReady = 
    {TPsyConfig::EConfigModuleStatus, 
        {TPositionModuleStatus::EDeviceReady, TPositionModuleStatus::EDataQualityNormal,0.01}};
const struct TPsyConfig KConfigStatusActive = 
    {TPsyConfig::EConfigModuleStatus, 
        {TPositionModuleStatus::EDeviceReady, TPositionModuleStatus::EDataQualityNormal,0.01}};

//Partial update
const struct TPsyConfig KConfigLRPartial = 
    {TPsyConfig::EConfigLRResponse, {0, KPositionPartialUpdate, 1, 1.23, 2.34, 10, 0}};
const struct TPsyConfig KConfigLRPartial5s = 
    {TPsyConfig::EConfigLRResponse, {1, KPositionPartialUpdate, 5, 1.23, 2.34, 10, 0}};

#define CLOSE_AND_OPEN_POSITIONER                                               \
{                                                                               \
    iPositioner.Close();                                                        \
    User::After(KSmallDelay);                                                         \
    TInt res = iPositioner.Open(iServer);                                            \
    CheckExpectedResult(res, KErrNone, KFailedConnectDefProxy);                \
    res = iPositioner.SetRequestor(                                             \
            CRequestor::ERequestorService,                                      \
            CRequestor::EFormatApplication,                                     \
            KTdDefProxyTestName);                                               \
    CheckExpectedResult(res, KErrNone, KFailedSetRequestor);                      \
}

#define SET_TIME                                                                \
    {                                                                           \
    iTime1.UniversalTime();                                                     \
    }
    
#define CHECK_TIME(_s)                                                          \
    {                                                                           \
    iTime2.UniversalTime();                                                     \
    TTimeIntervalSeconds seconds;                                               \
    iTime2.SecondsFrom(iTime1,seconds);                                         \
    CheckExpectedResult(seconds.Int(), _s, KResponseTimeDoesNotMatch);           \
    iTime1.UniversalTime();                                                     \
    }


#define DEF_LCFPSY1_MODULEID        0x03133002
#define DEF_LCFPSY2_MODULEID        0x03133004
#define DEF_LCFPSY3_MODULEID        0x03133006
#define DEF_LCFPSYSLOW4_MODULEID    0x03133008
#define DEF_LCFPSY6_MODULEID        0x0313300c

const TInt KLcfPsy1UidValue = DEF_LCFPSY1_MODULEID;
const TInt KLcfPsy3UidValue = DEF_LCFPSY3_MODULEID;
const TInt KLcfPsy6UidValue = DEF_LCFPSY6_MODULEID;

const TInt KSecondsToMicro = 1000*1000;

const TInt KFifthSecond = 200 * 1000;
const TInt KHalfSecond = 500 * 1000;
const TInt KSecond = KSecondsToMicro;

const TInt KTinyDelay = KFifthSecond; //KFifthSecond;
const TInt KSmallDelay = KHalfSecond; //KHalfSecond;
const TInt KDelay = KSecond; // KSecond;

const TInt KLeavingPsyUidValue = 0x03133026;
const TInt KInstaPsyUidValue = 0x03133028;

//PSY List
const TUid KLeavingPsyUid = {KLeavingPsyUidValue};
const TUid KIntGpsPsy1 = {0x03138001};
const TUid KIntGpsPsy2 = {0x03138003};
const TUid KExtGpsPsy1 = {0x03138005};
const TUid KExtGpsPsy2 = {0x03138007};
const TUid KNetworkPsy1 = {0x03138009};
const TUid KNetworkPsy2 = {0x0313800b};

//BT GPS PSY is never used in this test driver
const TUid KNppPsy = {0x0313302a};

// Name of this set
// Max length of this name is 80.
_LIT(KTdDefProxyTestName, "TdDefProxyUnit2 Test");

// Fail log format: "index : Fail/Pass : TestCase Name : ErrorCode"
_LIT(KLogFailFormat, "%d : Fail : %S : %d");

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_defproxySuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_defproxySuiteStepBase();
	CTe_defproxySuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	
    /**
    * Init PSY settings. This function shall be used like
    * InitPsySettingsL( KPsy1, availability, KPsy2, availability, ...);
    * "availability" shall be 0 or 1. 
    */
    void InitPsySettingsL(TInt aCount, ...);
    void ConfigPsyL(TPositionModuleId aModuleId, TInt aCount, ...);
    void ConfigPsyL(RPositioner& aPositioner, TPositionModuleId aModuleId, TInt aCount, ...);
    void InitPsyListInDefaultProxyL();
	void StandardPrepareL();
	void StandardCleanup();
	void DisableAllModulesL();
	void RestoreModulesL();
	void SetModulePriorityL(TPositionModuleId aModuleId, TInt aPriority);
	void CheckExpectedResult(TInt aResult, TInt aExpectedResult, const TDesC& aLabel);
	void CheckExpectedResult(TUid aResult, TUid aExpectedResult, const TDesC& aLabel);
	void ToggleModuleL(TPositionModuleId aModuleId, TBool aEnabled);
	void ToggleModuleVisibleL(TPositionModuleId aModuleId, TBool aVisible);
    void SetupModuleL(TPositionModuleId aModuleId, TBool aEnabled, TInt aPriority);
    void PositionRequestWithCheck(TPositionInfo& _aInfo, TInt _aResult, TUid _aModuleId);
    TQueryEvent* GetPsy6EventLogL(HPositionGenericInfo& aGenInfo);
    void SetPsy6Result(HPositionGenericInfo& aGenInfo, TInt32 aResult);
    void SetPsy6Delay(HPositionGenericInfo& aGenInfo, TTimeIntervalMicroSeconds aDelay);

protected:
    CActiveScheduler*   iScheduler;
    
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	
    RPositionServer iServer;
    RPositioner     iPositioner;
    CPosModules*    iModulesSettings;
    CPosModuleIdList* iBackupList;
    
    TRequestStatus iStatus;
    
    // Member variable used to set test time
    TTime iTime1;
    TTime iTime2;
    
    TUid    KUidLcfPsy1;
    TUid    KUidLcfPsy3;
    TUid    KUidLcfPsy6;

    TUid    KLeavingPsy;
    TUid    KInstaPsy;
	};

#endif
