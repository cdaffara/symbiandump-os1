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
// This file contains testing steps for the Unit
// test of the MMF Video Clients.
// Tests exercise all exposed memeber functions
// 
//


#ifndef __TEST_STEP_ALLOCFAIL_VID_PLAYER__
#define __TEST_STEP_ALLOCFAIL_VID_PLAYER__

 
/**
 *
 * Initialise
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepOOMVidPlayerNew :	public	CTestStepUnitMMFVidClient,
									public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepOOMVidPlayerNew* NewL(TBool aUseSurface = EFalse);
	CTestStepOOMVidPlayerNew(TBool aUseSurface);
	~CTestStepOOMVidPlayerNew();
	virtual TVerdict DoTestStepL();

	//from MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Open File
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepOOMVidPlayerOpenFile :	public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
										//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepOOMVidPlayerOpenFile* NewL(TBool aUseFileHandle, 
						TInt aSurfaceMode = CTestStepUnitMMFVidClient::ENotSurfaceMode);
	CTestStepOOMVidPlayerOpenFile(TBool aUseFileHandle, TInt aSurfaceMode);
	~CTestStepOOMVidPlayerOpenFile();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

private:
	void OpenFileAndStartSchedulerL();

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iUseFileHandle;
	};

/**
 *
 * Open Desc
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepOOMVidPlayerOpenDesc :	public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
	{
public:
	static CTestStepOOMVidPlayerOpenDesc* NewL(TInt aSurfaceMode = CTestStepUnitMMFVidClient::ENotSurfaceMode);
	CTestStepOOMVidPlayerOpenDesc(TInt aSurfaceMode);
	~CTestStepOOMVidPlayerOpenDesc();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

private:
	void OpenDescAndStartSchedulerL();

private:
	TInt iError;
	TMMFEvent iEvent;
	HBufC8* iBuf;
	};

/**
 *
 * Open Url
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepOOMVidPlayerOpenUrl :	public	CTestStepUnitMMFVidClient,
										public	MVideoPlayerUtilityObserver
										//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepOOMVidPlayerOpenUrl* NewL(TInt aSurfaceMode = CTestStepUnitMMFVidClient::ENotSurfaceMode);
	CTestStepOOMVidPlayerOpenUrl(TInt aSurfaceMode);
	~CTestStepOOMVidPlayerOpenUrl();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

private:
	void OpenUrlAndStartSchedulerL();

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Open File Specific Controller Uid
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepOOMVidPlayerOpenFileCtrlUid :	public	CTestStepUnitMMFVidClient,
												public	MVideoPlayerUtilityObserver
												//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepOOMVidPlayerOpenFileCtrlUid* NewL();
	CTestStepOOMVidPlayerOpenFileCtrlUid();
	~CTestStepOOMVidPlayerOpenFileCtrlUid();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

private:
	void OpenFileAndStartSchedulerL();

private:
	TInt iError;
	TMMFEvent iEvent;
	};

#endif //(__TEST_STEP_ALLOCFAIL_VID_PLAYER__)
