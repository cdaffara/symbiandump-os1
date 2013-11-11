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
 @file Te_locsrvSuiteStepBase.h
 @internalTechnology
*/

#ifndef __TE_LOCSRV_STEP_BASE__
#define __TE_LOCSRV_STEP_BASE__
#include <test/testexecutestepbase.h>
#include "te_locsrvsuitedefs.h"

#include <lbs.h>
#include <lbserrors.h>
#include <lbscommon.h>
#include <lbssatellite.h>
#include <lbspositioninfo.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <lbs/epos_cposmoduleupdate.h>

#include "dummyvalues.h"
#include "testdummyextclasses.h"

/****************************************************************************
	Copied from lcfpsy6publics.h to remove dependency
*****************************************************************************/
#include <lbsfieldids.h>

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

/** Maximum number of logged events in the PSY 
    The log Can Be Retrieved by KPSY6FieldEventLog command
 */
#define DEF_MAX_EVENT_COUNT             10

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
 End of code from lcfpsy6publics.h
*****************************************************************************/

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
#define DEF_LCFPSY1_MODULEID        0x03133002
#define DEF_LCFPSY2_MODULEID        0x03133004
#define DEF_LCFPSY3_MODULEID        0x03133006
#define DEF_LCFPSYSLOW4_MODULEID    0x03133008
#define DEF_LCFPSY6_MODULEID        0x0313300c

const TInt KLcfPsy1UidValue = DEF_LCFPSY1_MODULEID;
const TInt KLcfPsy3UidValue = DEF_LCFPSY3_MODULEID;
const TInt KLcfPsy6UidValue = DEF_LCFPSY6_MODULEID;

const TInt KTestHeapMinSize = 0x4000;
const TInt KTestHeapMaxSize = 0x8000;

const TInt KFifthOfSecond = 200000;
const TInt KFifthSecond = 200 * 1000;
const TInt KHalfSecond = 500 * 1000;
const TInt KSecondsToMicro = 1000*1000;
const TInt KSecond = KSecondsToMicro;

const TInt KDefaultTestTimeout = 20*1000*1000;  //20 seconds should be more than enough.

#define DO_PANIC_TEST_L(aThreadFunction, aExpectedExitCat, aExpectedExitReason, aTimeoutValue) \
    { \
    TExitCategoryName exitName = TPtrC(aExpectedExitCat).Left(KMaxExitCategoryName); \
    DoPanicTestL(aThreadFunction, exitName, aExpectedExitReason, aTimeoutValue); \
    }
#define CHECK_EQUAL_PROPS(obj1, obj2, prop) \
    CheckExpectedResult(obj1.prop, obj2.prop, KFailedPropertyCheck)

const TReal64 KErrorToleranceValue = KMinTReal64; //0.00001;

struct SPanicThreadFunctionData
{
    TThreadFunction     iPanicFunction;     // thread function that will panic
    TAny*               iPtr;               // data to pass to the function
};

class CTe_locsrvSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_locsrvSuiteStepBase();
	CTe_locsrvSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

protected:
    CActiveScheduler*   iScheduler;

	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	
    RPositionServer iServer;
    RPositioner     iPositioner;
    CPosModules*    iModulesSettings;
    
    TRequestStatus iStatus;
	
	void CheckExpectedResult(TInt aResult, TInt aExpectedResult, const TDesC& aLabel = KFailedEqualCheck);
	void CheckExpectedResult(TUid aResult, TUid aExpectedResult, const TDesC& aLabel = KFailedEqualCheck);
	void CheckExpectedResult(TTime aResult, TTime aExpectedResult, const TDesC& aLabel = KFailedEqualCheck);
	void CheckExpectedResult(const TDesC& aResult, const TDesC& aExpectedResult, const TDesC& aLabel = KFailedEqualCheck);
	void CheckExpectedResult(TTimeIntervalMicroSeconds aResult, TTimeIntervalMicroSeconds aExpectedResult, const TDesC& aLabel = KFailedEqualCheck);
	void CheckExpectedRealResult(TReal32 aResult, TReal32 aExpectedResult, const TDesC& aLabel = KComparedValuesNaN);
	void CheckResult(TBool aResult);
	void PositionRequestWithCheck(TPositionInfo& _aInfo, TInt _aResult, TUid _aModuleId);
	void PositionRequestWithCheck(TPositionInfo& _aInfo, TRequestStatus& aStatus, TInt _aResult, TUid _aModuleId, RPositioner aPositioner);
	TInt DoPanicTestL(TThreadFunction aThreadFunction, TExitCategoryName aExpectedExitCat,
	                  TInt aExpectedExitReason, TTimeIntervalMicroSeconds32 aTimeoutValue);
	
	void StandardPrepareL();
	void CTe_locsrvSuiteStepBase::StandardCleanup();
	void CTe_locsrvSuiteStepBase::SetupPsyPriorityL(TUid aPsyUid, TInt aPriority);
	void DisableAllModulesL();
	void TogglePsyL(TUid aPsyUid, TBool aEnabled);
	
	};
#endif
