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
 @file Te_LbsBtGpsPsySuiteStepBase.h
 @internalTechnology
*/

#ifndef __TE_LBSBTGPSPSY_STEP_BASE__
#define __TE_LBSBTGPSPSY_STEP_BASE__
#include <test/TestExecuteStepBase.h>

#include <e32math.h>
#include <badesca.h>

#include <lbs.h>
#include <lbssatellite.h>

#include "te_lbsbtgpspsydefs.h"
#include "btgpssim.h"

#include <centralrepository.h>

//const TUid KDevNotifCenrepUid = { 0x10285AA7 };
const TUint32  KDevNotifCenrepHwKey = 0x0000;
const TUint32  KDevNotifCenrepClassKey = 0x0001;
const TUint32  KDevNotifCenrepHwAttempts = 0x0002;
const TUint32  KDevNotifCenrepNotifierType = 0x0003;
const TUint32  KDevNotifCenrepSecondaryHwKey = 0x0004;
const TUint32  KDevNotifCenrepSecondaryClassKey = 0x0005;
const TUint32  KDevNotifCenrepUsageStatus = 0x0006;

const TPositionModuleId KbtGPSPsyId = { 0x101FE999 };

const TInt KMaxLogBuffer=200;

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_LbsBtGpsPsyStepBase : public CTestStep
	{
public:
	virtual ~CTe_LbsBtGpsPsyStepBase();
	CTe_LbsBtGpsPsyStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	
	void CheckModuleInfoL( TPositionModuleInfo aInfo );
	
	TInt QueryOnePositionL(TPositionInfoBase & aPosInfo,TTimeIntervalMicroSeconds aTimeOut, TTimeIntervalMicroSeconds aUpdateInterval);
	
	void PrintPositionInformationL( TPosition aPosition );
    void PrintPositionInformationL( TCourse aCourse );
    void PrintPositionInformationL( TSatelliteData aSatelliteData );
	
	void PrintPositionInformationL( TPositionInfo aInfo );
    void PrintPositionInformationL( TPositionCourseInfo aInfo );
    void PrintPositionInformationL( TPositionSatelliteInfo aInfo );
    void PrintPositionInformationL( HPositionGenericInfo *aInfo, TPositionFieldId aId );
    
    static void PrintPositionInformationForThreadL( TPositionInfo aInfo);
    static void PrintPositionInformationForThreadL( TPositionInfo aInfo, const TDesC& aFile );
    static void PrintPositionInformationForThreadL( TPositionSatelliteInfo aInfo);
    static void PrintPositionInformationForThreadL( TPositionSatelliteInfo aInfo, const TDesC& aFile );
    static void PrintForThreadL( const TDesC& aMessage);
    static void PrintForThreadL( const TDesC& aMessage, const TDesC& aFile );
    
	static TInt StartThreadL( TAny* aData );
	
    static void MakeTenPeriodicUpdatesFromBTGPSPsyL( TInt /*forLogging*/ ){};
    
protected:
	void AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode);
	void AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode1, TInt aErrorCode2);
	void AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TReal aErrorCode1, TReal aErrorCode2);
	
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	CActiveScheduler* iScheduler;
    TTime       iTime;
    RBtGpsSim	iBtGpsSim;
	};

#endif	//__TE_LBSBTGPSPSY_STEP_BASE__
