// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TEST_STEP_MIDICLNT_ALLOCFAIL_OPEN__
#define __TEST_STEP_MIDICLNT_ALLOCFAIL_OPEN__

/**
 *
 * Test Open
 *
 * @lib "TSU_MMFMIDICLIENT.lib"
 *
 * @xxxx
 *
 */
class CTestStepMidiClntAllocFailOpenFile :	public	CTestMmfMidiClntStep
	{
public:
	static CTestStepMidiClntAllocFailOpenFile* NewL(const TDesC& aTestName);
	CTestStepMidiClntAllocFailOpenFile(const TDesC& aTestName);
	~CTestStepMidiClntAllocFailOpenFile();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();
	void OpenFileAndStartSchedulerL();
	//For MS 3.4 Access File By Handle
	void OpenFileByHandleAndStartSchedulerL();

	// from MMidiClientUtilityObserver
	virtual void MmcuoStateChanged(TMidiState aOldState,TMidiState aNewState,const TTimeIntervalMicroSeconds& aTime,TInt aError);
	virtual void MmcuoTempoChanged(TInt aMicroBeatsPerMinute);
	virtual void MmcuoVolumeChanged(TInt aChannel,TReal32 aVolumeInDecibels);
	virtual void MmcuoMuteChanged(TInt aChannel,TBool aMuted);
	virtual void MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats);
	virtual void MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& aPosition);
	virtual void MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& aEntry);
	virtual void MmcuoPolyphonyChanged(TInt aNewPolyphony);
	virtual void MmcuoInstrumentChanged(TInt aChannel,TInt aBankId,TInt aInstrumentId);

private:
	CMidiClientUtility* iMidiClnt;
	TBuf<KNameBufSize> iSectName;
	TBuf<KNameBufSize> iKeyName;
	TPtrC iFileName;
	RFs iFs;
	RFile iFile;
	};
 
/**
 *
 * Test Open
 *
 * @lib "TSU_MMFMIDICLIENT.lib"
 *
 * @xxxx
 *
 */
class CTestStepMidiClntAllocFailOpenDes :	public	CTestMmfMidiClntStep
	{
public:
	static CTestStepMidiClntAllocFailOpenDes* NewL();
	CTestStepMidiClntAllocFailOpenDes();
	~CTestStepMidiClntAllocFailOpenDes();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();
	void OpenDesAndStartSchedulerL();

	// from MMidiClientUtilityObserver
	virtual void MmcuoStateChanged(TMidiState aOldState,TMidiState aNewState,const TTimeIntervalMicroSeconds& aTime,TInt aError);
	virtual void MmcuoTempoChanged(TInt aMicroBeatsPerMinute);
	virtual void MmcuoVolumeChanged(TInt aChannel,TReal32 aVolumeInDecibels);
	virtual void MmcuoMuteChanged(TInt aChannel,TBool aMuted);
	virtual void MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats);
	virtual void MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& aPosition);
	virtual void MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& aEntry);
	virtual void MmcuoPolyphonyChanged(TInt aNewPolyphony);
	virtual void MmcuoInstrumentChanged(TInt aChannel,TInt aBankId,TInt aInstrumentId);

private:
	CMidiClientUtility* iMidiClnt;
	TBuf<KNameBufSize> iSectName;
	TBuf<KNameBufSize> iKeyName;
	HBufC8* iAudio;
	TPtrC iFilename;
	};


/**
 *
 * Test Open
 *
 * @lib "TSU_MMFMIDICLIENT.lib"
 *
 * @xxxx
 *
 */
class CTestStepMidiClntAllocFailOpenUrl :	public	CTestMmfMidiClntStep
	{
public:
	static CTestStepMidiClntAllocFailOpenUrl* NewL();
	CTestStepMidiClntAllocFailOpenUrl();
	~CTestStepMidiClntAllocFailOpenUrl();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();
	void OpenUrlAndStartSchedulerL();

	// from MMidiClientUtilityObserver
	virtual void MmcuoStateChanged(TMidiState aOldState,TMidiState aNewState,const TTimeIntervalMicroSeconds& aTime,TInt aError);
	virtual void MmcuoTempoChanged(TInt aMicroBeatsPerMinute);
	virtual void MmcuoVolumeChanged(TInt aChannel,TReal32 aVolumeInDecibels);
	virtual void MmcuoMuteChanged(TInt aChannel,TBool aMuted);
	virtual void MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats);
	virtual void MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& aPosition);
	virtual void MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& aEntry);
	virtual void MmcuoPolyphonyChanged(TInt aNewPolyphony);
	virtual void MmcuoInstrumentChanged(TInt aChannel,TInt aBankId,TInt aInstrumentId);

private:
	CMidiClientUtility* iMidiClnt;
	TBuf<KNameBufSize> iSectName;
	TBuf<KNameBufSize> iKeyName;
	TPtrC iUrlname;
	};


#endif //(__TEST_STEP_MIDICLNT_ALLOCFAIL_OPEN__)
