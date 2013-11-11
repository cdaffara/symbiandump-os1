// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: DevVideoRecord tests.
// 
//

/**
 @file TestDevVideoCiuRecord.h
*/

#ifndef TESTDEVVIDEOCIURECORD_H
#define TESTDEVVIDEOCIURECORD_H

#include <mmf/devvideo/devvideostandardcustominterfaces.h>
#include "tsu_mmf_devvideociu.h"


/**
 * DevVideoCiuRecord test base class
 *
 * @class CTestDevVideoCiuRecordStep
 *
 */
class CTestDevVideoCiuRecordStep : public CTestDevVideoCiuStep, public MMMFDevVideoRecordObserver
	{
public:
	CTestDevVideoCiuRecordStep(const TDesC& aTestName);
	~CTestDevVideoCiuRecordStep();
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL() = 0;	

	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();

	// MMMFDevVideoRecordObserver
    virtual void MdvroReturnPicture(TVideoPicture* aPicture);
    virtual void MdvroSupplementalInfoSent();
    virtual void MdvroNewBuffers();
    virtual void MdvroFatalError(TInt aError);
    virtual void MdvroInitializeComplete(TInt aError);
    virtual void MdvroStreamEnd();

protected:
	TInt iError;
	THwDeviceId iHwDeviceId;
	CMMFDevVideoRecord* iDevVideoRecord;
	};

//
//							POSITIVE TESTS
//
// MMmfVideoInputSnapshot related tests
// Instantiation
class CTestDevVideoCiuRecCreateVideoInputSnapshotPreProc : public CTestDevVideoCiuRecordStep
	{
public:
	static CTestDevVideoCiuRecCreateVideoInputSnapshotPreProc* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuRecCreateVideoInputSnapshotPreProc(const TDesC& aTestName);
	~CTestDevVideoCiuRecCreateVideoInputSnapshotPreProc() {};
	};

// SetObserver
class CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc : public CTestDevVideoCiuRecordStep, 
																public MMmfVideoInputSnapshotObserver
	{
public:
	static CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
	
	// from MMmfVideoInputSnapshotObserver
	void SnapshotComplete(TInt aError);
private:
	CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc(const TDesC& aTestName);
	~CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc() {};
	};

// GetSnapshot
class CTestDevVideoCiuRecGetSnapshotVideoInputSnapshotPreProc : public CTestDevVideoCiuRecordStep
	{
public:
	static CTestDevVideoCiuRecGetSnapshotVideoInputSnapshotPreProc* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuRecGetSnapshotVideoInputSnapshotPreProc(const TDesC& aTestName);
	~CTestDevVideoCiuRecGetSnapshotVideoInputSnapshotPreProc() {};
	};

// CancelSnapshot
class CTestDevVideoCiuRecCancelSnapshotVideoInputSnapshotPreProc : public CTestDevVideoCiuRecordStep
	{
public:
	static CTestDevVideoCiuRecCancelSnapshotVideoInputSnapshotPreProc* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
	
private:
	CTestDevVideoCiuRecCancelSnapshotVideoInputSnapshotPreProc(const TDesC& aTestName);
	~CTestDevVideoCiuRecCancelSnapshotVideoInputSnapshotPreProc() {};
	};

// Observer SnapshotComplete callback
class CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc : public CTestDevVideoCiuRecordStep, 
																	public MMmfVideoInputSnapshotObserver
	{
public:
	static CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
	
	// from MMmfVideoInputSnapshotObserver
	void SnapshotComplete(TInt aError);
private:
	CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc(const TDesC& aTestName);
	~CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc() {};
	};

//
// MMmfVideoBufferManagement related tests
// Instantiation
class CTestDevVideoCiuRecCreateVideoBufferManagement : public CTestDevVideoCiuRecordStep
	{
public:
	static CTestDevVideoCiuRecCreateVideoBufferManagement* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuRecCreateVideoBufferManagement(const TDesC& aTestName);
	~CTestDevVideoCiuRecCreateVideoBufferManagement() {};
	};

// SetObserver
class CTestDevVideoCiuRecSetObserverVideoBufferManagement : public CTestDevVideoCiuRecordStep, 
															public MMmfVideoBufferManagementObserver
	{
public:
	static CTestDevVideoCiuRecSetObserverVideoBufferManagement* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
	
	// from MMmfVideoBufferManagementObserver
	void MmvbmoNewBuffers();
	void MmvbmoReleaseBuffers();
	
private:
	CTestDevVideoCiuRecSetObserverVideoBufferManagement(const TDesC& aTestName);
	~CTestDevVideoCiuRecSetObserverVideoBufferManagement() {};
	};

// Enable
class CTestDevVideoCiuRecEnableVideoBufferManagement : public CTestDevVideoCiuRecordStep
	{
public:
	static CTestDevVideoCiuRecEnableVideoBufferManagement* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuRecEnableVideoBufferManagement(const TDesC& aTestName);
	~CTestDevVideoCiuRecEnableVideoBufferManagement() {};
	};

// SetBufferOptions
class CTestDevVideoCiuRecSetOptionsVideoBufferManagement : public CTestDevVideoCiuRecordStep
	{
public:
	static CTestDevVideoCiuRecSetOptionsVideoBufferManagement* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuRecSetOptionsVideoBufferManagement(const TDesC& aTestName);
	~CTestDevVideoCiuRecSetOptionsVideoBufferManagement() {};
	};

// GetBufferOptions
class CTestDevVideoCiuRecGetOptionsVideoBufferManagement : public CTestDevVideoCiuRecordStep
	{
public:
	static CTestDevVideoCiuRecGetOptionsVideoBufferManagement* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuRecGetOptionsVideoBufferManagement(const TDesC& aTestName);
	~CTestDevVideoCiuRecGetOptionsVideoBufferManagement() {};
	};

// GetBuffer
class CTestDevVideoCiuRecGetBufferVideoBufferManagement : public CTestDevVideoCiuRecordStep
	{
public:
	static CTestDevVideoCiuRecGetBufferVideoBufferManagement* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuRecGetBufferVideoBufferManagement(const TDesC& aTestName);
	~CTestDevVideoCiuRecGetBufferVideoBufferManagement() {};
	};

// ReleaseBuffer
class CTestDevVideoCiuRecReleaseBufferVideoBufferManagement : public CTestDevVideoCiuRecordStep
	{
public:
	static CTestDevVideoCiuRecReleaseBufferVideoBufferManagement* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuRecReleaseBufferVideoBufferManagement(const TDesC& aTestName);
	~CTestDevVideoCiuRecReleaseBufferVideoBufferManagement() {};
	};
	
// Observer NewBuffers callback
class CTestDevVideoCiuRecNewBuffersVideo : public CTestDevVideoCiuRecordStep, 
											public MMmfVideoBufferManagementObserver
	{
public:
	static CTestDevVideoCiuRecNewBuffersVideo* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
	
	// from MMmfVideoBufferManagementObserver
	virtual void MmvbmoNewBuffers();
	virtual void MmvbmoReleaseBuffers();
private:
	CTestDevVideoCiuRecNewBuffersVideo(const TDesC& aTestName);
	~CTestDevVideoCiuRecNewBuffersVideo() {};
private:
	TBool iCallbackReceived;
	};

// Observer ReleaseBuffers callback
class CTestDevVideoCiuRecReleaseBuffersVideo : public CTestDevVideoCiuRecordStep, 
												public MMmfVideoBufferManagementObserver
	{
public:
	static CTestDevVideoCiuRecReleaseBuffersVideo* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
	
	// from MMmfVideoBufferManagementObserver
	virtual void MmvbmoNewBuffers();
	virtual void MmvbmoReleaseBuffers();
private:
	CTestDevVideoCiuRecReleaseBuffersVideo(const TDesC& aTestName);
	~CTestDevVideoCiuRecReleaseBuffersVideo() {};
private:
	TBool iCallbackReceived;
	};

//
// MMmfVideoH324AnnexKMode related tests
// Instantiation
class CTestDevVideoCiuRecCreateVideoH324AnnexKModeEncoder : public CTestDevVideoCiuRecordStep
	{
public:
	static CTestDevVideoCiuRecCreateVideoH324AnnexKModeEncoder* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuRecCreateVideoH324AnnexKModeEncoder(const TDesC& aTestName);
	~CTestDevVideoCiuRecCreateVideoH324AnnexKModeEncoder() {};
    };

// Set Mode
class CTestDevVideoCiuRecSetVideoH324AnnexKModeEncoder : public CTestDevVideoCiuRecordStep
	{
public:
	static CTestDevVideoCiuRecSetVideoH324AnnexKModeEncoder* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuRecSetVideoH324AnnexKModeEncoder(const TDesC& aTestName);
	~CTestDevVideoCiuRecSetVideoH324AnnexKModeEncoder() {};
	};

//
//							NEGATIVE TESTS
//
class CTestDevVideoCiuRecCreateVideoMediaProtectedPreProc : public CTestDevVideoCiuRecordStep
	{
public:
	static CTestDevVideoCiuRecCreateVideoMediaProtectedPreProc* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuRecCreateVideoMediaProtectedPreProc(const TDesC& aTestName);
	~CTestDevVideoCiuRecCreateVideoMediaProtectedPreProc() {};
	};	

//
class CTestDevVideoCiuRecCreateVideoMediaProtectedEncoder : public CTestDevVideoCiuRecordStep
	{
public:
	static CTestDevVideoCiuRecCreateVideoMediaProtectedEncoder* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuRecCreateVideoMediaProtectedEncoder(const TDesC& aTestName);
	~CTestDevVideoCiuRecCreateVideoMediaProtectedEncoder() {};
	};	

#endif
