// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsportedstepbase.h
// 
//

#ifndef __CT_LBS_PORTED_STEP_BASE_H__
#define __CT_LBS_PORTED_STEP_BASE_H__

//  INCLUDES
#include <w32std.h>
#include <lbspositioninfo.h>
#include <lbs.h>
#include <lbscommon.h>
#include <lbsrequestor.h>
#include <centralrepository.h>
#include "ctlbsstep.h"
#include "ctlbsserver.h"

// CONSTANTS
enum TPosSetupPsyDirective
    {
    EPosDisableTheRest = 0
    };

// UID:s for PSY:s
const TInt32 KUidExternalGps        = 0x101f7a7d;
const TInt32 KUidManualInput        = 0x101f7a7e;
const TInt32 KUidDefault            = 0x101f7a7f;
const TInt32 KUidStrategy           = 0x101f7a80;
const TInt32 KUidSimulationPsy      = 0x101f7a81;
const TInt32 KUidAGPSPsy			= 0x101fe98c;
const TInt32 KUidNETPsy				= 0x101fe98e;
const TInt32 KUidStubPositioner     = 0x01000019;
const TInt32 KUidDynDbTestPsy       = 0x01000016;
const TInt32 KUidInstallTestPsy     = 0x01000015;
const TInt32 KUidTestPsySimulateIsa = 0x01000018;
const TInt32 KUidTestPsy1           = 0x01000001;
const TInt32 KUidTestPsy2           = 0x01000002;
const TInt32 KUidTestPsy3           = 0x01000003;
const TInt32 KUidTestPsy4           = 0x01000004;
const TInt32 KUidTestPsy5           = 0x01000005;
const TInt32 KUidTestPsy6           = 0x01000006;
const TInt32 KUidTestPsy7           = 0x01000007;
const TInt32 KUidTestPsy8           = 0x01000008;
const TInt32 KUidTestPsy9           = 0x01000009;
const TInt32 KUidTestPsy10          = 0x01000010;
const TInt32 KUidTestPsy11          = 0x01000011;
const TInt32 KUidTestPsy12          = 0x01000012;
const TInt32 KUidTestPsy13          = 0x01000013;
const TInt32 KUidTestPsy256         = 0x01000017;
const TInt32 KUidTestPsyTp176       = 0x01000176;
const TInt32 KUidMultiPsy           = 0x01010176;
const TInt32 KUidTestStatusPsy      = 0x01000191;
const TInt32 KUidTestTimerPsy       = 0x01000192;
const TInt32 KUidExamplePsy         = 0x10001234; // TBD not impl UID
const TInt32 KUidPanicPsy           = 0x01010210;
const TInt32 KUidTestPsyPartialUpdate = 0x01000021;
const TInt32 KUidPsySwitching       = 0x00000000; // TBD fill in when there ever is a switching psy
const TInt32 KUidTestPsyMaxAge      = 0x01000020;
const TInt32 KUidTestRangePsy       = 0x01000022;
const TInt32 KUidTestSingPsy        = 0x01f0ff23;
const TInt32 KUidTestTrackingPsy    = 0x01f0ff24;

const TInt32 KTestProxyPsy1         = 0x01f0e001;
const TInt32 KTestProxyPsy2         = 0x01f0e002;
const TInt32 KTestProxyPsy3         = 0x01f0e003;
const TInt32 KTestProxyPsy4         = 0x01f0e004;
const TInt32 KTestProxyPsy5         = 0x102869dd;

const TInt32 KInstallPsyTp273         = 0x01f0e005;

const TInt32 KUidTestPsyPartialUpdate2 = 0x01000023;
const TInt32 KUidSatInfoPsy         =    0x01010177;

const TInt32 KUidBtgPsy         =    0x101FE999;
const TInt32 KUidIgPsy          =    0x101FE999;

const TInt32 KUidInstallableTrackingPsy	= 0x102869D9;
const TInt32 KUidDelayedResponsePsy	= 0x102869DB;

const TInt KEspectedErrorCodePSY1 = KErrNone;
const TInt KEspectedErrorCodePSY2 = KPositionPartialUpdate;
const TInt KEspectedErrorCodePSY3 = KErrPositionBufferOverflow;
const TInt KEspectedErrorCodePSY4 = KErrNone;

// Central repository constants for Default Proxy.
const TUid KCRUidDefaultProxyConfiguration = { 0x101f7a7f };
const TUint32 KDefaultProxyLastWorkingGpsPsy = 0x10000001;


// CLASS DECLARATION
class CT_LbsClientLog;

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CT_LbsPortedStepBase : public CT_LbsStep
	{
public:
	virtual ~CT_LbsPortedStepBase();
	CT_LbsPortedStepBase(CT_LbsServer& aParent);
	virtual TVerdict doTestStepPreambleL(); 
    virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

//Please add your class members which will be common to all individual test steps:
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
    
public: // New functions
    
    /**
    * Starts the test
    */
    virtual void StartL() = 0;
    
public: // Functions from base classes
    
    /**
    * Runs the test in the module
    * @param aIndex Indicates which internal test to run.
    */
    void StartL(TInt aIndex);    
    
protected: // New functions
    
    /**
    * Setup privacy and notification override settings
    * @param aPsyName   the name of the psy
    * @param aDirective how to setup the PSY:s
    *                   Enum def in this file.
    */
    void SetupPsyL(const TUid aPsyUid,
                   TPosSetupPsyDirective aDirective = EPosDisableTheRest);
    
    /**
    * Initialises the test.
    * By default does nothing.
    */
    virtual void InitTestL();
    
    /**
    * Cleans up after a test.
    * Always called, even if the test leaves.
    * By default does nothing.
    */
    virtual void CloseTest();
    
    /**
    * Connect to epos server.
    */
    void ConnectL();
    
    /**
    * Open a PSY by name.
    */
    TInt OpenPositionerByName(const TUid aPsyUid);
    
    /**
    * Open default PSY.
    */
    TInt OpenPositioner();
    
    /**
    * Close PSY by calling "iPositioner.Close()"
    */
    void ClosePositioner();
    
    /**
    * Close connection with epos server.
    */
    void Disconnect();
    
    /**
    * Perform a request to epos.
    * Use this methods when requesting from 
    * both a service and another requestor source, ie a Contact
    */
    void PerformRequestL(const TDesC& aService,
                         const TDesC& aContact, 
                         CRequestor::_TRequestorType aServiceIdType,
                         CRequestor::_TRequestorType aContactIdType,
                     // Set default values
                     CRequestor::_TRequestorFormat aServiceFormat = CRequestor::EFormatApplication,
                     CRequestor::_TRequestorFormat aContactFormat = CRequestor::EFormatTelephone
     );
    
    /**
    * Perform a request to epos. 
    */
    void PerformRequestL(const TDesC& aRequestor,
        CRequestor::_TRequestorType aRequestorType,
      CRequestor::_TRequestorFormat aRequestorFormat);
    
    /**
    * Perform a synchronous request to epos and return errorcode. 
    * Position info is returned in OUT parameter
    */
    TInt PerformSyncRequest(const TDesC& aServiceName,
                   /* OUT */TPositionInfoBase* aModuleInfo);
    
    /**
    * Check the result of a request to epos.
    * Leaves if the error code is not the expected one.
    * The log file is used to print the result from the request
    */
    void CheckRequestResultL(TInt aExpectedErrorCode, TBool aLogFlag = ETrue);
    
    /**
    * Leaves if aCondition is false.
    * @param aCondition something that should be true
    */
    void AssertTrueL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode);

    /**
     * Leaves if aCondition is false.
     * @param aCondition something that should be true
     */
     void AssertTrueL(TBool aCondition, const TDesC& aErrorMsg);
    
    /**
    * Get the last updated position info from positioner
    * Uses GetLastKnownPosition() method.
    * @return position info.
    */
    TPositionInfo AllocPositionInfoL();
    
    /**
    * Verifies that a PSY has been loaded by getting its last reported status.
    * Requires that the PSY report active in its constructor.
    * @param aUid the PSYs Uid 
    */ 
    void VerifyPsyLoadedL(TPositionModuleId aUid); 
    
    /**
    * Verifies that a PSY has been unloaded by getting its last reported status.
    * Requires that the PSY report inactive in its destructor.
    * @param aUid the PSYs Uid 
    */ 
    void VerifyPsyUnloadedL(TPositionModuleId aUid); 
    
    /**
    * Request a position from the default psy
    */ 
    void RequestL(TPositionInfo& aPosInfo, const TInt& aRequestIndex, 
    		const TInt& aExpectedErrorCode = 999);
    
    void LogErrorAndLeaveL(const TDesC& aErrorInfo,
            const TInt   aReason = KErrGeneral);
    
    void SetSimDataFileL(const TDesC& aFileName);
    
    void ResetDefaultPSYLastWorkingExtGPSPSYL();

    void VerifyPositionFromL(TPositionInfo& aPosInfo, TPositionModuleId& aModuleId);

    void VerifyPositionL(TPositionInfo& aPosInfo, TReal64 aLat, 
    		TReal64 aLong, TReal64 aAlt);

    void VerifyRequestTimeLessThanL(TInt aExpectedReqTime);
    
    void ConstructL();
    
    /**
     * Set the update options in LBS
     */
    TInt SetUpdateOptions(TPositionUpdateOptions aUpdateOptions);
    
protected:  // Data

    TInt                    iNrOfRequests;
    TInt 					iRequestTime;
    
    TInt                    iModuleId;
    TUid                    iUidExternalGps;         
    TUid                    iUidManualInput;
    TUid                    iUidDefault;
    TUid                    iUidStrategy;
    TUid                    iUidSimulationPsy;
    TUid                    iUidAGPSPsy;
    TUid                    iUidNETPsy;
    TUid                    iUidStubPositioner;
    TUid                    iUidDynDbTestPsy;      
    TUid                    iUidInstallTestPsy;
    TUid                    iUidTestPsySimulateIsa;
    TUid                    iUidTestPsy1;
    TUid                    iUidTestPsy2;        
    TUid                    iUidTestPsy3;        
    TUid                    iUidTestPsy4;        
    TUid                    iUidTestPsy5;        
    TUid                    iUidTestPsy6;         
    TUid                    iUidTestPsy7;         
    TUid                    iUidTestPsy8;         
    TUid                    iUidTestPsy9;         
    TUid                    iUidTestPsy10;         
    TUid                    iUidTestPsy11;         
    TUid                    iUidTestPsy12;
    TUid                    iUidTestPsy13;        
    TUid                    iUidTestPsy256;
    TUid                    iUidTestPsyTp176;
    TUid                    iUidMultiPsy;
    TUid                    iUidExamplePsy;
    TUid                    iUidTestStatusPsy;
    TUid                    iUidTestTimerPsy;
    TUid                    iUidPsySwitching;
    TUid                    iUidPanicPsy;
    TUid                    iUidTestPsyPartialUpdate;
    TUid                    iUidTestPsyMaxAge;
    TUid                    iUidTestRangePsy;
    TUid                    iUidTestSingPsy;
    TUid                    iUidTestTrackingPsy;
    TUid                    iUidTestProxyPsy1;
    TUid                    iUidTestProxyPsy2;        
    TUid                    iUidTestProxyPsy3;        
    TUid                    iUidTestProxyPsy4;
	TUid                    iUidTestProxyPsy5;
    TUid                    iUidInstallPsyTp273;
    TUid                    iUidPartialUpdate2;
    TUid                    iUidSatInfoPsy;
    TUid                    iUidBtgPsy;
    TUid                    iUidIgPsy;
	TUid					iUidInstallableTrackingPsy;
	TUid					iUidDelayedResponsePsy;

    RPositionServer         iPosServer;
    RPositioner             iPositioner;
    
    CActiveScheduler* 		iScheduler;
    TBool    				iErrors;
    RFs                 	iFileServer;
    
    CT_LbsClientLog*		iLog;
    
private:    // Data
   
    TRequestStatus          iStatus;
    TPositionInfo           iModuleInfo;
	};

#endif // __CT_LBS_PORTED_STEP_BASE_H__
