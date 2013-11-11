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


#ifndef __TEST_STEP_VID_RECORDER__
#define __TEST_STEP_VID_RECORDER__

 #include <ecam.h>
 #include <mdaaudiosampleplayer.h>
 
/**
 *
 * Test New Recorder utility
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderNew :	public	CTestStepUnitMMFVidClient,
								public	MVideoRecorderUtilityObserver
								//public	MMMFControllerEventMonitorObserver // is it really usefull?
	{
public:
	static CTestStepVidRecorderNew* NewL();
	CTestStepVidRecorderNew();
	~CTestStepVidRecorderNew();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError);
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

protected:
	TInt iError;
	};


class CTestStepAllocFailVidRecorderNew : public CTestStepVidRecorderNew
	{
public:
	CTestStepAllocFailVidRecorderNew();
	static CTestStepAllocFailVidRecorderNew* NewL();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 * Initialise Priority Preference
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderNewPriorPref :	public	CTestStepUnitMMFVidClient,
											public	MVideoRecorderUtilityObserver
											//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderNewPriorPref* NewL();
	CTestStepVidRecorderNewPriorPref();
	~CTestStepVidRecorderNewPriorPref();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TMMFPrioritySettings iPrioritySettings;
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
class CTestStepVidRecorderOpenFile :public	CTestStepUnitMMFVidClient,
									public	MVideoRecorderUtilityObserver
									//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderOpenFile* NewL(TBool aUseFileHandle);
	CTestStepVidRecorderOpenFile(TBool aUseFileHandle);
	~CTestStepVidRecorderOpenFile();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

protected:
	TInt iError;
	TMMFEvent iEvent;
	TBool iUseFileHandle;
	};

class CTestStepAllocFailVidRecorderOpenFile : public CTestStepVidRecorderOpenFile
	{
public:
	static CTestStepAllocFailVidRecorderOpenFile* NewL(TBool aUseFileHandle);
	virtual TVerdict DoTestStepL();
	
protected:
	CTestStepAllocFailVidRecorderOpenFile(TBool aUseFileHandle);
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
class CTestStepVidRecorderOpenDesc :	public	CTestStepUnitMMFVidClient,
									public	MVideoRecorderUtilityObserver
									//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderOpenDesc* NewL();
	CTestStepVidRecorderOpenDesc();
	~CTestStepVidRecorderOpenDesc();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

protected:
	void OpenDescAndStartSchedulerL();

	TInt iError;
	TMMFEvent iEvent;
	HBufC8* iBuf;
	};

class CTestStepAllocFailVidRecorderOpenDesc : public CTestStepVidRecorderOpenDesc
	{
public:
	CTestStepAllocFailVidRecorderOpenDesc();
	static CTestStepAllocFailVidRecorderOpenDesc* NewL();
	virtual TVerdict DoTestStepL();
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
class CTestStepVidRecorderOpenUrl :	public	CTestStepUnitMMFVidClient,
									public	MVideoRecorderUtilityObserver
									//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderOpenUrl* NewL();
	CTestStepVidRecorderOpenUrl();
	~CTestStepVidRecorderOpenUrl();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

protected:
	void OpenUrlAndStartSchedulerL();

	TInt iError;
	TMMFEvent iEvent;
	};


class CTestStepAllocFailVidRecorderOpenUrl : public CTestStepVidRecorderOpenUrl
	{
public:
	CTestStepAllocFailVidRecorderOpenUrl();
	static CTestStepAllocFailVidRecorderOpenUrl* NewL();
	virtual TVerdict DoTestStepL();
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
class CTestStepVidRecorderOpenFileCtrlUid :	public	CTestStepUnitMMFVidClient,
											public	MVideoRecorderUtilityObserver
											//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderOpenFileCtrlUid* NewL(TBool aUseFileHandle);
	CTestStepVidRecorderOpenFileCtrlUid(TBool aUseFileHandle);
	~CTestStepVidRecorderOpenFileCtrlUid();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

protected:
	void OpenFileAndStartSchedulerL();

	TInt iError;
	TMMFEvent iEvent;
	TBool iUseFileHandle;
	};

class CTestStepAllocFailVidRecorderOpenFileCtrlUid : public CTestStepVidRecorderOpenFileCtrlUid
	{
public:
	CTestStepAllocFailVidRecorderOpenFileCtrlUid(TBool aUseFileHandle);
	static CTestStepAllocFailVidRecorderOpenFileCtrlUid* NewL(TBool aUseFileHandle);
	virtual TVerdict DoTestStepL();
	};

/**
 *
 * Prepare
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderPrepare :	public	CTestStepUnitMMFVidClient,
									public	MVideoRecorderUtilityObserver
	{
public:
	static CTestStepVidRecorderPrepare* NewL();
	CTestStepVidRecorderPrepare();
	~CTestStepVidRecorderPrepare();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};
	
	
/**
 *
 * Close
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderClose :	public	CTestStepUnitMMFVidClient,
								public	MVideoRecorderUtilityObserver
								//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderClose* NewL();
	CTestStepVidRecorderClose();
	~CTestStepVidRecorderClose();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

protected:
	TInt iError;
	TMMFEvent iEvent;
	};


	
/**
 *
 * Play
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderRecord :	public	CTestStepUnitMMFVidClient,
									public	MVideoRecorderUtilityObserver
									//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderRecord* NewL();
	CTestStepVidRecorderRecord();
	~CTestStepVidRecorderRecord();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasRecorded;
	};

/**
 *
 * Stop
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderStop :public	CTestStepUnitMMFVidClient,
								public	MVideoRecorderUtilityObserver
								//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderStop* NewL();
	CTestStepVidRecorderStop();
	~CTestStepVidRecorderStop();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError);
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Pause
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderPause :	public	CTestStepUnitMMFVidClient,
									public	MVideoRecorderUtilityObserver
									//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderPause* NewL();
	CTestStepVidRecorderPause();
	~CTestStepVidRecorderPause();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get/Set Priority
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderPriority :	public	CTestStepUnitMMFVidClient,
									public	MVideoRecorderUtilityObserver
									//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderPriority* NewL();
	CTestStepVidRecorderPriority();
	~CTestStepVidRecorderPriority();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get/Set Vid Frame Rate
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderVFrameRate :	public	CTestStepUnitMMFVidClient,
											public	MVideoRecorderUtilityObserver
											//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderVFrameRate* NewL();
	CTestStepVidRecorderVFrameRate();
	~CTestStepVidRecorderVFrameRate();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& /*aEvent*/);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get/Set Video frame size
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderVFrameSize :	public	CTestStepUnitMMFVidClient,
										public	MVideoRecorderUtilityObserver
										//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderVFrameSize* NewL();
	CTestStepVidRecorderVFrameSize();
	~CTestStepVidRecorderVFrameSize();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& /*aEvent*/);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get/Set Video bit rate
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderVBitRate :	public	CTestStepUnitMMFVidClient,
										public	MVideoRecorderUtilityObserver
										//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderVBitRate* NewL();
	CTestStepVidRecorderVBitRate();
	~CTestStepVidRecorderVBitRate();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& /*aEvent*/);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get/Set Audio bit rate
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderABitRate :	public	CTestStepUnitMMFVidClient,
										public	MVideoRecorderUtilityObserver
										//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderABitRate* NewL();
	CTestStepVidRecorderABitRate();
	~CTestStepVidRecorderABitRate();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& /*aEvent*/);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get/Set Audio enabled
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderAudEnabled :	public	CTestStepUnitMMFVidClient,
										public	MVideoRecorderUtilityObserver
										//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderAudEnabled* NewL();
	CTestStepVidRecorderAudEnabled();
	~CTestStepVidRecorderAudEnabled();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& /*aEvent*/);

private:
	TInt iError;
	TMMFEvent iEvent;
	};


/**
 *
 * Get record time available
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderRecordTime :	public	CTestStepUnitMMFVidClient,
										public	MVideoRecorderUtilityObserver
										//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderRecordTime* NewL();
	CTestStepVidRecorderRecordTime();
	~CTestStepVidRecorderRecordTime();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& /*aEvent*/);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Get duration of the clip
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderDuration :	public	CTestStepUnitMMFVidClient,
									public	MVideoRecorderUtilityObserver
										//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderDuration* NewL();
	CTestStepVidRecorderDuration();
	~CTestStepVidRecorderDuration();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& /*aEvent*/);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Set Max Clip Size
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderMaxClipSize :	public	CTestStepUnitMMFVidClient,
										public	MVideoRecorderUtilityObserver
										//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderMaxClipSize* NewL();
	CTestStepVidRecorderMaxClipSize();
	~CTestStepVidRecorderMaxClipSize();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& /*aEvent*/);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Gain functions
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderGain :	public	CTestStepUnitMMFVidClient,
									public	MVideoRecorderUtilityObserver
									//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderGain* NewL();
	CTestStepVidRecorderGain();
	~CTestStepVidRecorderGain();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& /*aEvent*/);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Metadata functions
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderMeta :	public	CTestStepUnitMMFVidClient,
									public	MVideoRecorderUtilityObserver
									//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderMeta* NewL();
	CTestStepVidRecorderMeta();
	~CTestStepVidRecorderMeta();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& /*aEvent*/);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Set/Get audio codecs
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderAudioType :	public	CTestStepUnitMMFVidClient,
										public	MVideoRecorderUtilityObserver
										//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderAudioType* NewL();
	CTestStepVidRecorderAudioType();
	~CTestStepVidRecorderAudioType();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& /*aEvent*/);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Set video codecs
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderVideoType :	public	CTestStepUnitMMFVidClient,
										public	MVideoRecorderUtilityObserver
										//public	MMMFControllerEventMonitorObserver // is it usefull here?
	{
public:
	static CTestStepVidRecorderVideoType* NewL();
	CTestStepVidRecorderVideoType();
	~CTestStepVidRecorderVideoType();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	//from MMMFControllerEventMonitorObserver
	//virtual void HandleEvent(const TMMFEvent& /*aEvent*/);

private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * Video recorder HandleEvent test
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderHandleEvent :	public	CTestStepUnitMMFVidClient,
										public	MVideoRecorderUtilityObserver
	{
public:
	static CTestStepVidRecorderHandleEvent* NewL();
	CTestStepVidRecorderHandleEvent();
	~CTestStepVidRecorderHandleEvent();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
private:
	TInt iError;
	TMMFEvent iEvent;
	};

/**
 *
 * GetImplementationInfo
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderGetImplementationInfo : public	CTestStepUnitMMFVidClient,
								public	MVideoRecorderUtilityObserver
	{
public:
	static CTestStepVidRecorderGetImplementationInfo* NewL();
	CTestStepVidRecorderGetImplementationInfo();
	~CTestStepVidRecorderGetImplementationInfo();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	};

/**
 *
 * SendCustomCommandSync
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderSendCustomCommandSync : public CTestStepUnitMMFVidClient,
								public	MVideoRecorderUtilityObserver
	{
public:
	static CTestStepVidRecorderSendCustomCommandSync* NewL();
	CTestStepVidRecorderSendCustomCommandSync();
	~CTestStepVidRecorderSendCustomCommandSync();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	};

/**
 *
 * SendCustomCommandAsync
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderSendCustomCommandAsync : public CTestStepUnitMMFVidClient,
								public	MVideoRecorderUtilityObserver
	{
public:
	static CTestStepVidRecorderSendCustomCommandAsync* NewL();
	CTestStepVidRecorderSendCustomCommandAsync();
	~CTestStepVidRecorderSendCustomCommandAsync();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	
	class CAsyncObserver : public CActive
		{
	public:
		CAsyncObserver() : CActive(EPriorityStandard) {CActiveScheduler::Add(this);}
		void RunL()
			{
			CActiveScheduler::Stop();
			}
		void DoCancel(){;}
		TRequestStatus& ActiveStatus(){SetActive();return iStatus;}
		};
	

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	CAsyncObserver* iAsyncObserver;
	TMMFMessageDestinationPckg iDestinationPckg;
	};
	
/**
 *
 * CTestStepVidRecorderGeneralEvent
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderGeneralEvent : public	CTestStepUnitMMFVidClient,
								public	MVideoRecorderUtilityObserver
	{
public:
	static CTestStepVidRecorderGeneralEvent* NewL();
	CTestStepVidRecorderGeneralEvent();
	~CTestStepVidRecorderGeneralEvent();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError);
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);

private:
	TBool iCorrectEventReceived;
	TBool iEventReceived;
	TInt iError;
	};


/**
 *
 * CheckImplementationInfo
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepVidRecorderCheckImplementationInfo : public	CTestStepUnitMMFVidClient,
								public	MVideoRecorderUtilityObserver
	{
public:
	static CTestStepVidRecorderCheckImplementationInfo* NewL();
	CTestStepVidRecorderCheckImplementationInfo();
	~CTestStepVidRecorderCheckImplementationInfo();
	enum TVerdict DoTestStepPreambleL();
	enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError); 
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);

private:
	TInt iError;
	TMMFEvent iEvent;
	TBool iHasPlayed;
	};


/*
 */
NONSHARABLE_CLASS(CTimedRecording) : public CTimer
  	{
public:
	// First phase Construction
	CTimedRecording();
	// Destruction
	~CTimedRecording();

public:
	  // Static construction
	static CTimedRecording* NewLC( TInt aTicksRequested, TInt aTicksInterval );
	static CTimedRecording* NewL( TInt aTicksRequested, TInt aTicksInterval );

public:
	void IssueRequest(CVideoRecorderUtility* aRecorder, CMdaAudioPlayerUtility* aAudioPlayer);

	void DoCancel();

	void RunL();

private:
	  // Second phase construction
	void ConstructL( TInt aTicksRequested, TInt aSeconds );

protected:
	TInt iTicksRequested;  // Total number of ticks CTimedRecording
	TInt iSeconds;   // Number of seconds between each tick.
	TInt iTicksDone;       // Number of ticks done so far.
	CVideoRecorderUtility* iRecorder;
	CMdaAudioPlayerUtility* iAudioPlayer;
	};


/*
 */
class CTestStepVidRecordError :	public	CTestStepUnitMMFVidClient,
								public	MVideoRecorderUtilityObserver,
								public  MCameraObserver2,
								public  MMdaAudioPlayerCallback  
	{
public:
	static CTestStepVidRecordError* NewL();
	CTestStepVidRecordError();
	~CTestStepVidRecordError();
	virtual TVerdict DoTestStepL();

	// From CTestStepUnitMMFVidClient
	void RecOpenAndStartSchedulerL();
	enum TVerdict DoTestStepPreambleL(void);
	enum TVerdict DoTestStepPostambleL(void);

	//from MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError);
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoEvent(const TMMFEvent& aEvent);
	
	// From MCameraObserver2
	void HandleEvent(const TECAMEvent& aEvent);
	void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	
	// From MMdaAudioPlayerCallback
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration);
	void MapcPlayComplete(TInt aError);


protected:
	void CreateDependencyFileL();
	void RemoveDependencyFileL();

protected:
	CCamera* iCamera;
	TInt iError;
	CTimedRecording *iTimedRecording;
	TPtrC iFileNamePlay;
	CMdaAudioPlayerUtility *iAudioPlayer;
	
	TTimeIntervalMicroSeconds iDuration;
	};

#endif //(__TEST_STEP_VID_RECORDER__)
