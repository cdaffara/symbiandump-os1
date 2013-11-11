// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __UNIT_MMF_VID_CLNT_TESTSTEP_H__)
#define __UNIT_MMF_VID_CLNT_TESTSTEP_H__

// EPOC includes
#include <e32base.h>
//#include <eikenv.h>
#include <videoplayer.h>

#ifdef SYMBIAN_BUILD_GCE
#include <videoplayer2.h>
#endif


const TInt KUidTestVideoController = 0x101F88D9;

/**
 *
 * CActiveListener provides the asynchronous operation
 * of an active object
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 *
 */
class CActiveListener : public CActive
	{
public:
	CActiveListener() : CActive(CActive::EPriorityIdle) { CActiveScheduler::Add(this); }
	void InitialiseActiveListener() { iStatus = KRequestPending; SetActive(); }
	// From CActive
	virtual void RunL() { CActiveScheduler::Stop(); }
	virtual void DoCancel() {};
	};

class CTestSuite;
class CTestSuiteUnitMMFVidClient;
class CVideoPlayerUtility;
class CVideoRecorderUtility;

/**
 *
 * CTestStepUnitMMFVidClient, which is the base class for all 
 * the UnitMMFVidClient suite test steps.
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepUnitMMFVidClient : public CTestStep
	{
public:
	enum TTestMode
		{
		ENotSurfaceMode,
		ENotSurfaceModeSupported,
		ESurfaceMode,
		ESurfaceModeNotSupported,
		};
	CTestStepUnitMMFVidClient();
	~CTestStepUnitMMFVidClient();

	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	
	// accessors
	void SetUnitMMFVidClientSuite(CTestSuiteUnitMMFVidClient* aSuite);
	inline CTestSuiteUnitMMFVidClient* UnitMMFVidClientSuite() const;

protected:
	virtual void OpenAndStartSchedulerL(TBool aUseFileHandle = EFalse);

	virtual void OpenAndStartSchedulerL(const TMMSource& aSource);
	
	virtual void PlayAndStartSchedulerL();
	virtual void RecOpenAndStartSchedulerL(TBool aUseFileHandle = EFalse);
	virtual void RecordAndStartScheduler();
	virtual void RecorderPrepareAndStartScheduler();
	virtual void PlayerPrepareAndStartScheduler();
	TBool PlayerStateOpening();
	TBool PlayerStatePaused();
	TBool PlayerStateStopped();
	TBool PlayerStatePlaying();
#ifdef SYMBIAN_BUILD_GCE
	void SurfaceSupported();
	TBool SurfaceCreated();
#endif
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	TInt SubtitleCrpDataCount();
	TBool CheckSubtitleData(TInt aWindowId, TInt aCrpId, const TRect& aCrpRect);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

	TBool RecorderStateOpening();
	TBool RecorderStatePaused();
	TBool RecorderStateStopped();
	TBool RecorderStateRecording();
	
	TInt GetPlayerPriority(TMMFPrioritySettings&);
	TInt GetRecorderPriority(TMMFPrioritySettings&);
	TInt GetPlayerClipRect(TRect& aRect);
	TInt GetPlayerWindowRect(TRect& aRect);
	TInt GetPlayerController(RMMFController*& aController);
	TInt GetRecorderController(RMMFController*& aController);
	TInt GetInitScreenNumber();
	TVerdict ControllerCalled(const TDesC8& aText, TBool aExpected = ETrue);
	TVerdict ControllerCalled2(const TDesC8& aText, TBool aExpected = ETrue);
	TVerdict DoControllerCalled(TInt aFunction, const TDesC8& aText, TBool aExpected = ETrue);
	
	void InitWservL();
	void DeInitWserv();
	void InitEikonL();
	void DeInitEikon();

private:
	// pointer to suite which owns this test 
	CTestSuiteUnitMMFVidClient* iUnitMMFVidClientSuite;
	CActiveScheduler*           iScheduler ;
	

protected:
	CVideoPlayerUtility* iPlayer;
#ifdef SYMBIAN_BUILD_GCE
	CVideoPlayerUtility2* iPlayer2;
#endif
	CVideoRecorderUtility* iRecorder;
	TPtrC iFileName;
	TPtrC8 iMimeType;
	TPtrC* iRes;

	RFs iFs;
	RFile iFile;

	CWsScreenDevice* iScreen;
	RWindow* iWindow;
	RWsSession iWs;
	RWindowGroup iRootWindow;

	//CEikonEnv* iEikEnv;
	
	TBool iSurfaceSupported;
	TBool iSurfaceMode;
	TBool iVpu2;
	};

#endif //__UNIT_MMF_VID_CLNT_TESTSTEP_H__
