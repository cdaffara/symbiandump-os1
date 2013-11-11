
// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TESTRECORDER.h
// Header file: Basic recorder tests.
// 
//

/**
 @file TESTRECORDER.h
*/

#ifndef TESTRECORDER_H__
#define TESTRECORDER_H__

#include "TSI_MMFVCLNT.h"

const TInt KMMFVideoRecordTimeAvailable = 10000000;		// 10 seconds

/**
 * Load and initialise a video file for recording.
 *
 * @class CTestVclntVideoFile
 *
 * Req. under test REQ172.16.3    REQ172.16.6    REQ172.16.9
 */
class CTestVclntRecordVideoFile : public CTestMmfVclntStep, public MVideoRecorderUtilityObserver
	{
public:
	CTestVclntRecordVideoFile(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec);
	static CTestVclntRecordVideoFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec);
	static CTestVclntRecordVideoFile* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
	// MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);  
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError);  
	virtual void MvruoEvent(const TMMFEvent& aEvent);


protected:
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TInt iError;
	const TBool iRec;					// Does this test require recording of a video
	TTimeIntervalMicroSeconds iDuration;// Stores duration of video
	};

/**
 * Load and initialise an video descriptor for recording.
 *
 * @class CTestVclntRecorderVideoDes
 *
 * Req. under test REQ172.16.3     REQ172.16.7     REQ172.16.9
 */
class CTestVclntRecorderVideoDes : public CTestMmfVclntStep, public MVideoRecorderUtilityObserver
	{
public:
	CTestVclntRecorderVideoDes(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec) ;
	static CTestVclntRecorderVideoDes* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec);
	static CTestVclntRecorderVideoDes* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
	// MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);  
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError);  
	virtual void MvruoEvent(const TMMFEvent& aEvent);

private:
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	HBufC8* iVideo;						// Buffer for holding video descriptor data
	TTimeIntervalMicroSeconds iDuration;// Stores duration of video
	TInt iError;						// Stores error values for testing
	const TBool iRec;					// Does this test require recording of a video
	};

/**
 * Load and initialise a video URL for recording.
 *
 * @class CTestVclntVideoUrl
 *
 * Req. under test REQ172.16.3    REQ172.16.6    REQ172.16.9
 */
class CTestVclntRecorderVideoUrl : public CTestMmfVclntStep, public MVideoRecorderUtilityObserver
	{
public:
	CTestVclntRecorderVideoUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec);
	static CTestVclntRecorderVideoUrl* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec);
	static CTestVclntRecorderVideoUrl* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
	// MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);  
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError);  
	virtual void MvruoEvent(const TMMFEvent& aEvent);


protected:
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TInt iError;
	const TBool iRec;					// Does this test require recording of a video
	TTimeIntervalMicroSeconds iDuration;// Stores duration of video
	};
/**
 * Configure the recorded file and record.
 *
 * @class CTestVclntRecordConfig
 *
 * Req. under test REQ172.16.11    REQ172.16.9
 */
class CTestVclntRecordConfig : public CTestMmfVclntStep, public MVideoRecorderUtilityObserver
	{
public:
	static CTestVclntRecordConfig* NewL(const TDesC& aTestName);
	static CTestVclntRecordConfig* NewLC(const TDesC& aTestName);
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
	virtual TVerdict DoTestStepL();
	CTestVclntRecordConfig(const TDesC& aTestName);
	// MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);  
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError);  
	virtual void MvruoEvent(const TMMFEvent& aEvent);


protected:
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TInt iError;
	TTimeIntervalMicroSeconds iDuration;// Stores duration of video
	};


/**
 * Open file-based clip. Close the clip and then open the clip again.
 *
 * @class CTestVclntRecordCloseOpen
 *
 * Req. under test REQ172.16.3   REQ172.16.6  REQ172.16.4   REQ172.16.5      
 */
class CTestVclntRecordCloseOpen : public CTestVclntRecordVideoFile
	{
public:
	static CTestVclntRecordCloseOpen* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
	CTestVclntRecordCloseOpen(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);

private:
	TInt iFrameRate;
	};


/**
 * Start recording then stop on request.
 *
 * @class CTestVclntRecordPause
 *
 * Req. under test REQ172.16.9           REQ172.16.10    
 */
class CTestVclntRecordPause : public CTestVclntRecordVideoFile
	{
public:
	static CTestVclntRecordPause* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
	CTestVclntRecordPause(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	};


/**
 * Check duration.
 *
 * @class CTestVclntRecordDuration
 *
 * Req. under test REQ172.16.14   
 */
class CTestVclntRecordDuration : public CTestVclntRecordVideoFile
	{
public:
	static CTestVclntRecordDuration* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aDuration);
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
	CTestVclntRecordDuration(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aDuration);
	};


/**
 * Query record time
 *
 * @class CTestVclntRecordTime
 *
 * Req. under test REQ172.16.15  
 */
class CTestVclntRecordTime : public CTestVclntRecordVideoFile
	{
public:
	static CTestVclntRecordTime* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aTime);
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
	CTestVclntRecordTime(const TDesC& aTestName, const TDesC& SectName,const TDesC& aKeyName, const TInt aTime);

private:
	TInt iTime;
	};



/**
 * Crop clip
 *
 * @class CTestVclntRecordCrop
 *
 * Req. under test REQ172.16.16
 */
class CTestVclntRecordCrop : public CTestVclntRecordVideoFile
	{
public:
	static CTestVclntRecordCrop* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TBool aToEnd);
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
	CTestVclntRecordCrop(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TBool aToEnd);

private:
	TBool iToEnd;
	};

/**
 * Set gain to maximum and enquire
 *
 * @class CTestVclntRecordGain
 *
 * Req. under test REQ172.16.17      REQ172.16.18       REQ172.16.19
 */
class CTestVclntRecordGain : public CTestVclntRecordVideoFile
	{
public:
	static CTestVclntRecordGain* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aGain);
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
	CTestVclntRecordGain(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aGain);

private:
	TInt iGain;
	};

/**
 * Set meta-information.
 *
 * @class CTestVclntRecordMeta
 *
 * Req. under test REQ172.16.20
 */
class CTestVclntRecordMeta : public CTestVclntRecordVideoFile
	{
public:
	static CTestVclntRecordMeta* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
	CTestVclntRecordMeta(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	};


/**
 * Set priority.
 *
 * @class CTestVclntRecordPriority
 *
 * Req. under test REQ172.16.22
 */

class CTestVclntRecordPriority : public CTestVclntRecordVideoFile
	{
public:
	static CTestVclntRecordPriority* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aPriority);
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
	CTestVclntRecordPriority(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aPriority);

private:
	TInt iPriority;
	};



/**
 * Set balance and enquire the balance
 *
 * @class CTestVclntRecordBalance
 *
 * Req. under test REQ172.16.23       REQ172.16.24
 */
class CTestVclntRecordBalance : public CTestVclntRecordVideoFile
	{
public:
	static CTestVclntRecordBalance* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aBalance);
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
	CTestVclntRecordBalance(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aBalance);

private:
	TInt iBalance;
	};


/**
 * Specify maximum clip size in bytes.
 *
 * @class CTestVclntRecordSize
 *
 * Req. under test REQ172.16.26
 */
class CTestVclntRecordSize : public CTestVclntRecordVideoFile
	{
public:
	static CTestVclntRecordSize* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
	CTestVclntRecordSize(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	};


/**
 * Set / get audio / video types
 *
 * @class CTestVclntRecordTypes
 *
 * Req. not mapped
 */
class CTestVclntRecordTypes : public CTestVclntRecordVideoFile
	{
public:
	static CTestVclntRecordTypes* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
	CTestVclntRecordTypes(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	};

/**
 *  Record utility - Delete object before record operation has completed.
 *
 * @class CTestVclntRecordDelete
 *
 * Req. under test REQ172.16.11         REQ172.16.9
 */
class CTestVclntRecordDelete : public CTestVclntRecordVideoFile
	{
public:
	static CTestVclntRecordDelete* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
	virtual TVerdict DoTestStepL();

private:
	CTestVclntRecordDelete(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	};


#endif // TESTRECORDER_H__
