// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: DevVideo alloc tests.
// 
//

/**
 @file TestStepDevVideoAllocFailNew.h
*/

#ifndef __TEST_STEP_DEVVIDEO_ALLOCFAIL_NEW__
#define __TEST_STEP_DEVVIDEO_ALLOCFAIL_NEW__

#include "TestDevVideoPlay.h"
#include "TestDevVideoRecord.h"

/**
 * DevVideoPlay alloc test base class
 *
 * @class CTestStepDevVideoPlayAllocFail
 *
 * Req. under test REQ1931.3
 */

// inherits from CTestDevVideoStep because CTestDevVideoPlayStep instantiates
// the CMMFDevVideoPlay class which we want to do ourselves
class CTestStepDevVideoPlayAllocFail : public CTestDevVideoStep, public MMMFDevVideoPlayObserver
	{
public:
	static CTestStepDevVideoPlayAllocFail* NewL(const TDesC& aTestName);
	CTestStepDevVideoPlayAllocFail(const TDesC& aTestName);
	~CTestStepDevVideoPlayAllocFail();
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL() = 0;

	// MMMFDevVideoPlayObserver
    virtual void MdvpoNewBuffers();
    virtual void MdvpoReturnPicture(TVideoPicture* aPicture);
    virtual void MdvpoSupplementalInformation(const TDesC8& aData, 
											  const TTimeIntervalMicroSeconds& aTimestamp, 
											  const TPictureId& aPictureId);
	virtual void MdvpoPictureLoss();
	virtual void MdvpoPictureLoss(const TArray<TPictureId>& aPictures);
	virtual void MdvpoSliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture);
	virtual void MdvpoReferencePictureSelection(const TDesC8& aSelectionData);
    virtual void MdvpoTimedSnapshotComplete(TInt aError, 
											TPictureData* aPictureData, 
											const TTimeIntervalMicroSeconds& aPresentationTimestamp, 
											const TPictureId& aPictureId);
    virtual void MdvpoNewPictures();
    virtual void MdvpoFatalError(TInt aError);
    virtual void MdvpoInitComplete(TInt aError);
    virtual void MdvpoStreamEnd();

protected:
	TInt iError;
	CMMFDevVideoPlay* iDevVideoPlay;
	};



class CTestStepDevVideoPlayAllocFailNew : public CTestStepDevVideoPlayAllocFail
	{
public:
	static CTestStepDevVideoPlayAllocFailNew* NewL(const TDesC& aTestName);
	CTestStepDevVideoPlayAllocFailNew(const TDesC& aTestName);
	~CTestStepDevVideoPlayAllocFailNew();
	virtual TVerdict DoTestL();
	};


class CTestStepDevVideoPlayAllocFailFindPlugins : public CTestStepDevVideoPlayAllocFail
	{
public:
	static CTestStepDevVideoPlayAllocFailFindPlugins* NewL(const TDesC& aTestName);
	CTestStepDevVideoPlayAllocFailFindPlugins(const TDesC& aTestName);
	~CTestStepDevVideoPlayAllocFailFindPlugins();
	virtual TVerdict DoTestL();
private:
	void GetDecodersL();
	void GetPostProcessorsL();
	void FindDecodersL();
	void FindPostProcessorsL();
	void CheckPluginsL(const RArray<TUid>& aFoundPlugins, const RArray<TUid>& aExpectedPlugins, const RArray<TUid>& aUnExpectedPlugins);
	};

class CTestStepDevVideoPlayAllocFailInfoDecoder : public CTestStepDevVideoPlayAllocFail
	{
public:
	static CTestStepDevVideoPlayAllocFailInfoDecoder* NewL(const TDesC& aTestName);
	CTestStepDevVideoPlayAllocFailInfoDecoder(const TDesC& aTestName);
	~CTestStepDevVideoPlayAllocFailInfoDecoder();
	virtual TVerdict DoTestL();
	};

class CTestStepDevVideoPlayAllocFailInfoPostProc : public CTestStepDevVideoPlayAllocFail
	{
public:
	static CTestStepDevVideoPlayAllocFailInfoPostProc* NewL(const TDesC& aTestName);
	CTestStepDevVideoPlayAllocFailInfoPostProc(const TDesC& aTestName);
	~CTestStepDevVideoPlayAllocFailInfoPostProc();
	virtual TVerdict DoTestL();
	};

class CTestStepDevVideoPlayAllocFailGetPictures : public CTestStepDevVideoPlayAllocFail
	{
public:
	static CTestStepDevVideoPlayAllocFailGetPictures* NewL(const TDesC& aTestName);
	CTestStepDevVideoPlayAllocFailGetPictures(const TDesC& aTestName);
	~CTestStepDevVideoPlayAllocFailGetPictures();
	virtual TVerdict DoTestL();
	void PerformTestL();
	virtual void MdvpoInitComplete(TInt aError);
    virtual void MdvpoNewPictures();
	void CheckNewPictureInfoL(TUint aNumPictures, const TTimeIntervalMicroSeconds& aFirst, const TTimeIntervalMicroSeconds& aLast);
	void DoNewPicturesL();
private:
	TInt iCurrentPictureNumber;
	TInt iTestError;
	};


// DEVVIDEORECORD //

/**
 * DevVideoRecord alloc test base class
 *
 * @class CTestStepDevVideoRecordAllocFail
 *
 * Req. under test REQ1931.3
 */

// inherits from CTestDevVideoStep because CTestDevVideoRecordStep instantiates
// the CMMFDevVideoRecord class which we want to do ourselves
class CTestStepDevVideoRecordAllocFail : public CTestDevVideoStep, public MMMFDevVideoRecordObserver
	{
public:
	static CTestStepDevVideoRecordAllocFail* NewL(const TDesC& aTestName);
	CTestStepDevVideoRecordAllocFail(const TDesC& aTestName);
	~CTestStepDevVideoRecordAllocFail();
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL() = 0;

	// MMMFDevVideoRecordObserver
    virtual void MdvroReturnPicture(TVideoPicture* aPicture);
    virtual void MdvroSupplementalInfoSent();
    virtual void MdvroNewBuffers();
    virtual void MdvroFatalError(TInt aError);
    virtual void MdvroInitializeComplete(TInt aError);
    virtual void MdvroStreamEnd();

protected:
	TInt iError;
	CMMFDevVideoRecord* iDevVideoRecord;
	};


class CTestStepDevVideoRecordAllocFailNew : public CTestStepDevVideoRecordAllocFail
	{
public:
	static CTestStepDevVideoRecordAllocFailNew* NewL(const TDesC& aTestName);
	CTestStepDevVideoRecordAllocFailNew(const TDesC& aTestName);
	~CTestStepDevVideoRecordAllocFailNew();
	virtual TVerdict DoTestL();
	};

class CTestStepDevVideoRecordAllocFailInfoEncoder : public CTestStepDevVideoRecordAllocFail
	{
public:
	static CTestStepDevVideoRecordAllocFailInfoEncoder* NewL(const TDesC& aTestName);
	CTestStepDevVideoRecordAllocFailInfoEncoder(const TDesC& aTestName);
	~CTestStepDevVideoRecordAllocFailInfoEncoder();
	virtual TVerdict DoTestL();
	};

class CTestStepDevVideoRecordAllocFailInfoPreProc : public CTestStepDevVideoRecordAllocFail
	{
public:
	static CTestStepDevVideoRecordAllocFailInfoPreProc* NewL(const TDesC& aTestName);
	CTestStepDevVideoRecordAllocFailInfoPreProc(const TDesC& aTestName);
	~CTestStepDevVideoRecordAllocFailInfoPreProc();
	virtual TVerdict DoTestL();
	};

class CTestStepDevVideoRecordAllocFailFindPlugins : public CTestStepDevVideoRecordAllocFail
	{
public:
	static CTestStepDevVideoRecordAllocFailFindPlugins* NewL(const TDesC& aTestName);
	CTestStepDevVideoRecordAllocFailFindPlugins(const TDesC& aTestName);
	~CTestStepDevVideoRecordAllocFailFindPlugins();
	virtual TVerdict DoTestL();
private:
	void GetEncodersL();
	void GetPreProcessorsL();
	void FindEncodersL();
	void FindPreProcessorsL();
	void CheckPluginsL(const RArray<TUid>& aFoundPlugins, const RArray<TUid>& aExpectedPlugins, const RArray<TUid>& aUnExpectedPlugins);
	};

class CTestStepDevVideoRecordAllocFailGetBuffers : public CTestStepDevVideoRecordAllocFail
	{
public:
	static CTestStepDevVideoRecordAllocFailGetBuffers* NewL(const TDesC& aTestName);
	CTestStepDevVideoRecordAllocFailGetBuffers(const TDesC& aTestName);
	~CTestStepDevVideoRecordAllocFailGetBuffers();
	virtual TVerdict DoTestL();
	void PerformTestL();
	virtual void MdvpoInitComplete(TInt aError);
    virtual void MdvpoNewBuffers();
	void CheckNewBufferInfoL(TUint aNumBuffers);
	void DoNewBuffersL();
private:
	TInt iCurrentBufferNumber;
	TInt iTestError;
	};

#endif //(__TEST_STEP_DEVVIDEO_ALLOCFAIL_NEW__)
