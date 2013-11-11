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
//

#ifndef TSU_MMF_OGGCTRL_TESTSTEP_H
#define TSU_MMF_OGGCTRL_TESTSTEP_H

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/common/mmfcontroller.h>
#include "tsu_mmf_oggctrl_testsuite.h"

const TUid KOggPlayControllerUid = { 0x01004304 };
const TUid KOggDecodeUid = {0x01004305};
const TUid KOggRecordControllerUid = {0x10273771};
const TUid KOggEncodeUid = {0x10273772};
const TInt KDeviationInTime	= 500000;
const TInt KTwoSeconds		= 2000000;
const TInt KThreeSeconds	= 3000000;
const TInt KFiveSeconds		= 5000000;
const TInt KNameBufSize		= 25;

class CTestSuite;
class CTestSuiteOggCtrl;
/**
 *
 * RTestStepOggCtrlBase is the base class for all 
 * the test steps in this suite, providing default path functionality
 * for all the teststeps in thsi suite
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlBase : 	public RTestStep,
								public MMMFControllerEventMonitorObserver
	{
public:
	RTestStepOggCtrlBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
	void SetSuite( CTestSuiteOggCtrl* aPtr );
	CTestSuiteOggCtrl* GetSuite();
	TBool TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation);
	TInt CopyFile(TPtrC aFileNameSrc, TPtrC aFileNameDst);
protected:
	CTestSuiteOggCtrl* iSuite;
	RFs iFs;
	RMMFController iController;
	TMMFPrioritySettings iPriority;
	CMMFControllerEventMonitor* iControllerEventMonitor;
	CActiveScheduler* iScheduler;
	TMMFFileConfig	iFileConfig;
	TUid iControllerUid;
	TBuf<KNameBufSize> iSectName;					// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;					// Key name for retrieving filename
	};
/**
 *
 * RTestStepOggCtrlPlayBase is the base class for all 
 * the OggPlayController test steps, providing common functionality.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlPlayBase : 	public RTestStepOggCtrlBase
	{
public:
	RTestStepOggCtrlPlayBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict DoTestStepPreambleL(void);
	TBool SetupReadFileConfigL();
	TMMFFileConfig& ReadFileConfig();
protected:
	//from MMMFControllerEventMonitorObserver
	void HandleEvent(const TMMFEvent& aEvent);
	};

/**
 *
 * RTestStepOggCtrlRecordBase is the base class for all 
 * the OggRecordController test steps, providing common functionality.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlRecordBase : 	public RTestStepOggCtrlBase
	{
public:
	RTestStepOggCtrlRecordBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TBool SetupWriteFileConfigL();
	TMMFFileConfig& WriteFileConfig();
protected:
	//from MMMFControllerEventMonitorObserver
	virtual void HandleEvent(const TMMFEvent& aEvent);
	};
#endif
