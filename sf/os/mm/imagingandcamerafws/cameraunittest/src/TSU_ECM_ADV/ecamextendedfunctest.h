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
//

#ifndef ECAMEXTENDEDFUNCTEST_H
#define ECAMEXTENDEDFUNCTEST_H

#include <testframework.h>
#include "ECamObserverTest.h"
#include "extended_functionality.h"

class RECamClientViewFinderTest : public RTestStep
	{
public:
	static RECamClientViewFinderTest* NewL(const TDesC& aTestStep);
	
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	
	// new
	TVerdict DoTestStep_50L();
	
	TVerdict DoClientVFTestStepL();
	
private:
	RECamClientViewFinderTest(const TDesC& aTestStep);
	};
	
class RECamClientVFNotificationTest : public RECamObserverTest, public MClientViewFinderObserver
	{
public:
	static RECamClientVFNotificationTest* NewL(TBool aAllocTest);
	
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoTestStepL_51L();
	void ConstructL();
	
private:
	RECamClientVFNotificationTest(TBool aAllocTest);
	
	//MClientViewFinderObserver
	void ViewFinderBufferReady(CCamera::CCameraClientViewFinder& aClientViewFinderHandle, TInt aErrorCode);
	void DirectHistogramDisplayed(CCamera::CCameraClientViewFinder& aClientViewFinderHandle, CCamera::CCameraV2Histogram& aDirectHistogramDisplayed, TInt aErrorCode);
	void ClientHistogramReady(CCamera::CCameraClientViewFinder& aClientViewFinderHandle, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode);
	void ImageProcessingFailed(CCamera::CCameraClientViewFinder& aClientViewFinderHandle, TInt aErrorCode);
	TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface);

	void CheckViewFinderNegNotification(TInt aVFHandle, TVerdict& aResult);
	
private:

	TInt iVFHandle;
	TInt iError;
	TUid iInputEventUid;
	};
	
class RECamV2DirectViewFinderTest : public RTestStep
	{
public:
	static RECamV2DirectViewFinderTest* NewL(const TDesC& aTestStep);
	
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	
	// new
	TVerdict DoTestStep_52L();
	
	TVerdict DoV2DirectVFTestStepL();
	
private:
	RECamV2DirectViewFinderTest(const TDesC& aTestStep);
	};
	
class RECamV2DirectVFNotificationTest : public RECamObserverTest, public MDirectViewFinderObserver
	{
public:
	static RECamV2DirectVFNotificationTest* NewL(TBool aAllocTest);
	
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoTestStep_53L();
	
	void ConstructL();
	
private:
	RECamV2DirectVFNotificationTest(TBool aAllocTest);
	
	//MDirectViewFinderObserver
	void DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	void DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraV2Histogram& aDirectHistogramDisplayed, TInt aErrorCode);
	void ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode);
	void DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	void DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode);
	TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface);

	void CheckViewFinderNegNotification(TInt aVFHandle, TVerdict& aResult);
	
private:

	TInt iVFHandle;
	TInt iError;
	TUid iInputEventUid;
	};
	
class RECamPreImgCaptureNotificationTest : public RECamObserverTest, public MPreImageCaptureControlObserver
	{
public:
	static RECamPreImgCaptureNotificationTest* NewL(TBool aAllocTest);
	
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoPreImgCaptureTestStepL();

private:
	RECamPreImgCaptureNotificationTest(TBool aAllocTest);
	
	//MPreImageCaptureControlObserver
	void PrepareImageComplete(CCamera::CCameraImageCapture* aCaptureImageHandle, TInt aErrorCode);
	TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface);

	void CheckCaptureNotification(CCamera::CCameraImageCapture*& aImageCapture, TVerdict& aResult);
	
private:
	CCamera::CCameraImageCapture* iCaptureImageHandle;
	TInt iError;
	TUid iInputEventUid;
	};
	
class RECamImgCaptureNotificationTest : public RECamObserverTest, public MPreImageCaptureControlObserver, MCaptureImageObserver
	{
public:
	static RECamImgCaptureNotificationTest* NewL(TBool aAllocTest);
	
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoImgCaptureAdvFuncConstruction();
	
private:
	RECamImgCaptureNotificationTest(TBool aAllocTest);
	
	//MPreImageCaptureControlObserver
	void PrepareImageComplete(CCamera::CCameraImageCapture* aCaptureImageHandle, TInt aErrorCode);
	TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface);

	void CheckPreCaptureNotification(CCamera::CCameraImageCapture*& aImageCapture, TVerdict& aResult);
	
	//MCaptureImageObserver
	void IndividualImageControlHandle(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId);
	void ImageBufferReady(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode);
	void ClientSnapshotForImageReady(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, MCameraBuffer2* aSnapshotBuffer, TInt aErrorCode);
	void DirectSnapshotForImageDisplayed(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode);
	void CutDownImageDirectSavingCompleted(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode);
	void ImageDirectSavingCompleted(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode);
	void ImageCaptureComplete(CCamera::CCameraImageCapture& aCaptureImageHandle, TInt aErrorCode);
	void ClientHistogramForImageReady(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode);
	void ClientHistogramForSnapshotReady(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode);
	void DirectHistogramForSnapshotDisplayed(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, CCamera::CCameraV2Histogram& aDirectHistogramDisplayed, TInt aErrorCode);
	void ProcessingFailed(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TUint aProcessingTypes, TInt aErrorCode);
	
	void CheckCaptureNotification(CCamera::CCameraImageCapture*& aImageCapture, TPostCaptureControlId& aPostCaptureControlId, TVerdict& aResult);
	void CheckCaptureCompletion(CCamera::CCameraImageCapture*& aImageCapture, TVerdict& aResult);
	
private:
	CCamera::CCameraImageCapture* iCaptureImageHandle;
	TPostCaptureControlId iPostCaptureControlId;
	TUint iProcessingTypes;
	TInt iError;
	TUid iInputEventUid;
	};
	
class RECamVideoCaptureControlTest : public RTestStep
	{
public:
	static RECamVideoCaptureControlTest* NewL(const TDesC& aTestStep);
	
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	
	// new
	TVerdict DoTestStep_56L();
	
	TVerdict DoVideoCaptureTestStepL();
	
private:
	RECamVideoCaptureControlTest(const TDesC& aTestStep);
	};

class RECamVideoCaptureNotificationTest : public RECamObserverTest, public MCaptureVideoObserver
	{
public:
	static RECamVideoCaptureNotificationTest* NewL(TBool aAllocTest);
	
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	void ConstructL();
	
private:
	RECamVideoCaptureNotificationTest(TBool aAllocTest);
	
	//MCaptureVideoObserver
	void VideoBufferReady(MCameraBuffer2* aVideoBuffer, TInt aErrorCode);
	void ClientSnapshotReady(MCameraBuffer2* aSnapshotBuffer, TInt aErrorCode);
	void ClientHistogramReady(MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode);
	void ImageProcessingFailed(TInt aErrorCode);
	void DirectVideoCaptureFailed(TInt aErrorCode);
	TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface);

	void CheckVideoCaptureNegNotification(TInt aErrorCode, TVerdict& aResult);
	
private:

	TInt iError;
	TUid iInputEventUid;
	};
	
#endif // ECAMEXTENDEDFUNCTEST_H

