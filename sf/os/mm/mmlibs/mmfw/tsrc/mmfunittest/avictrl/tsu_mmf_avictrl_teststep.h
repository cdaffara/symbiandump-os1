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


#ifndef TSU_MMF_AVICTRL_TESTSTEP_H
#define TSU_MMF_AVICTRL_TESTSTEP_H

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/common/mmfcontroller.h>
#include "tsu_mmf_avictrl_testsuite.h"

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
#include <mmf/common/mmfvideosurfacecustomcommands.h>
#include <mmf/common/mmfvideosubtitlecustomcommands.h>

#define KWindowClipRectFull TRect(0, 0, 640, 240)
#define KWindowClipRectAlt1 TRect(20, 20, 620, 220)
#define KWindowClipRectAlt2 TRect(50, 0, 290, 240)
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

class CTestSuiteAviCtrl;
/**

NOTE: RAsyncTestStepAviCtrlBase/RAsyncTestStepAviCtrlPlayBase should be used instead

RTestStepAviCtrlBase is the (old) base class for all the test steps in this suite,
providing default path functionality for all the teststeps in this suite.

*/
class RTestStepAviCtrlBase : public RTestStep,
						  public MMMFControllerEventMonitorObserver
	{
public:
	RTestStepAviCtrlBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	void SetSuite( CTestSuiteAviCtrl* aPtr );
	CTestSuiteAviCtrl* GetSuite();
protected:
	CTestSuiteAviCtrl* iSuite;
	RFs iFs;
	RMMFController iController;
	TMMFPrioritySettings iPriority;
	CMMFControllerEventMonitor* iControllerEventMonitor;
	CActiveScheduler* iScheduler;
	TMMFFileConfig	iFileConfig;
	TUid iControllerUid;
	TBuf<25> iSectName;	// Section name for retrieving filename
	TBuf<25> iKeyName;	// Key name for retrieving filename
	};

/**

NOTE: RAsyncTestStepAviCtrlBase/RAsyncTestStepAviCtrlPlayBase should be used instead

RTestStepAviCtrlPlayBase is the base class for all the AviPlayController test steps, providing common functionality.
*/
class RTestStepAviCtrlPlayBase : public RTestStepAviCtrlBase
	{
public:
	RTestStepAviCtrlPlayBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict DoTestStepPreambleL();
	TBool SetupReadFileConfigL();
	TMMFFileConfig& ReadFileConfig();
protected:
	//from MMMFControllerEventMonitorObserver
	void HandleEvent(const TMMFEvent& aEvent);
protected:
	TRect iScreenRect;
	TRect iClipRect;
	RRegion iRegion;
	};

/*
RTestStepAviCtrlRecordBase is the base class for all  the AviRecordController test steps, providing common functionality.
*/
class RTestStepAviCtrlRecordBase : 	public RTestStepAviCtrlBase
	{
public:
	RTestStepAviCtrlRecordBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TBool SetupWriteFileConfigL();
	TMMFFileConfig& WriteFileConfig();
protected:
	//from MMMFControllerEventMonitorObserver
	void HandleEvent(const TMMFEvent& aEvent);
	};



/**
RAsyncTestStepAviCtrlBase is the base class for all the test steps in this
suite, providing default path functionality for all the teststeps in this suite
*/
class RAsyncTestStepAviCtrlBase :
	public RAsyncTestStep,
	public MMMFControllerEventMonitorObserver
	{

public:

	RAsyncTestStepAviCtrlBase(const TDesC& aTestName,
							  const TDesC& aSectName,
							  const TDesC& aKeyName,
							  TUid aControllerUid);

	// From RAsyncTestStep
	void KickoffTestL();
	void CloseTest();

	void SetSuite( CTestSuiteAviCtrl* aPtr );
	CTestSuiteAviCtrl* GetSuite();


protected:

	static TInt RAsyncTestStepAviCtrlBaseCallBack(TAny* aPtr);
	void DoRAsyncTestStepAviCtrlBaseCallBack();

protected:

	CTestSuiteAviCtrl* iSuite;
	RFs iFs;
	RMMFController iController;
	TMMFPrioritySettings iPriority;
	CMMFControllerEventMonitor* iControllerEventMonitor;
	TMMFFileConfig	iFileConfig;
	TUid iControllerUid;
	TBuf<25> iSectName;	// Section name for retrieving filename
	TBuf<25> iKeyName;	// Key name for retrieving filename
	CAsyncCallBack* iKickoffAOp;
	};

/**
RTestStepAviCtrlPlayBase is the new base class for all the AviPlayController
test steps, providing common functionality.
*/
class RAsyncTestStepAviCtrlPlayBase :
	public RAsyncTestStepAviCtrlBase
	{

public:

	RAsyncTestStepAviCtrlPlayBase(const TDesC& aTestName,
								  const TDesC& aSectName,
								  const TDesC& aKeyName,
								  TUid aControllerUid);

	// From RAsyncTestStep
	void KickoffTestL();
	void CloseTest();

	TBool SetupReadFileConfigL();
	TMMFFileConfig& ReadFileConfig();


protected:

	static TInt RAsyncTestStepAviCtrlPlayBaseCallBack(TAny* aPtr);
	void DoRAsyncTestStepAviCtrlPlayBaseCallBack();

	//from MMMFControllerEventMonitorObserver
	void HandleEvent(const TMMFEvent& aEvent);


protected:

	TRect iScreenRect;
	TRect iClipRect;
	RRegion iRegion;
	CAsyncCallBack* iKickoffAOp;
	TBool iGceEnabled;
	};

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
class RAsyncTestStepSubtitleBase : public RAsyncTestStepAviCtrlPlayBase
	{
public:
	RAsyncTestStepSubtitleBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);

	// From MMMFControllerEventMonitorObserver
	void HandleEvent(const TMMFEvent& aEvent);
	
	// From RAsyncTestStep
	void KickoffTestL();
		
protected:
	void SetExpectedCrpReadyCount(TInt aCount);
	
	// Common actions to be performed during OpenComplete and PlayComplete handlers
	void CheckAndEnableSubtitlesL();
	void AddDefaultSubtitleConfL();
	void StartPlaybackL();
	void RemoveConfigDisableL();

protected:
	RMMFVideoPlaySurfaceSupportCustomCommands iSurfaceCommands;
	RMMFVideoPlaySubtitleSupportCustomCommands iSubtitleCommands;
	
	static const TInt KWindowIdValid;
	static const TInt KWindowIdValidAlt;
	static const TInt KWindowIdMismatched;

private:
	virtual void OnOpenCompleteL();
	virtual void OnPlayCompleteL();
	
private:
	TInt iCrpReadyEventCount;
	TInt iExpectedCrpReadyCount;
	};
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	
#endif
