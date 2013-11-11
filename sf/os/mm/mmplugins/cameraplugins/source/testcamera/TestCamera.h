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

#ifndef __TESTCAMERA_H__
#define __TESTCAMERA_H__


#include <ecam/ecamplugin.h>
#include <ecam/ecaminfoplugin.h>
#include <bitdev.h>
#include <ecam.h>
#include <w32std.h>
#include <ecamadvsettings.h>

_LIT(KTestCameraPluginName, "C:\\102070cc.txt");

const TInt KMinTestCameraZoom = -3;
const TInt KMaxTestCameraZoom = 3;
const TReal32 KMaxTestCameraDigitalZoomFactor = 1.0;
const TInt KMaxTestCameraDigitalZoom = 0;
const TInt KFramePosIncrement = 5;
const TInt KFrameFeatureSize = 25;
const TInt KFrameFeatureBorderThickness = 5;
const enum CCamera::TFormat KInitialViewFinderFormat = CCamera::EFormatFbsBitmapColor16M;
const TDisplayMode KViewFinderDisplayMode = EColor4K;

class CTestCamAdvSet;
class CTestCamPresets;
class CTestCamSnapshot;
class CTestCamImgProc;

class RTestCameraFrameBuffer : public MCameraBuffer
	{
public:
	RTestCameraFrameBuffer(CFbsBitmap*& aFrame): iAvailable(ETrue), iFrame(aFrame) {}
	virtual TInt NumFrames() {return 0;}
	virtual TDesC8* DataL(TInt aFrameIndex) {if (aFrameIndex != 0) { User::Leave(KErrArgument); } return NULL; }
	virtual CFbsBitmap& BitmapL(TInt aFrameIndex) {if (aFrameIndex != 0) { User::Leave(KErrArgument); } return *iFrame;}
 	virtual	RChunk& ChunkL() { return iChunk;}
	virtual TInt ChunkOffsetL(TInt aFrameIndex) {if (aFrameIndex != 0) { User::Leave(KErrArgument); } return 0;}
	virtual TInt FrameSize(TInt aFrameIndex) {if (aFrameIndex != 0) { User::Leave(KErrArgument); } return 0; }
	virtual void Release() {iAvailable = ETrue;}
public:	
	volatile TBool iAvailable;
	CFbsBitmap*& iFrame;
	RChunk iChunk;
	};

class RTestFrameBuffer : public MFrameBuffer
	{
public:
	RTestFrameBuffer(CFbsBitmap*& aFrame): iAvailable(ETrue), iFrame(aFrame) {}
	virtual TDesC8* DataL(TInt /*aIndex*/) { User::Leave(KErrNotSupported); return NULL; }
	virtual CFbsBitmap* FrameL(TInt aIndex) { if (aIndex != 0) User::Leave(KErrNotSupported); return iFrame; }
	virtual void Release() { iAvailable = ETrue; }
public:
	TBool iAvailable;
	CFbsBitmap*& iFrame;
	};

class CDirectScreenAccess;

class CTestCamera : public CCameraPlugin, public MDirectScreenAccess
	{
	friend class CTestCamAdvSet;
	friend class CTestCamPresets;
	friend class CTestCamSnapshot;
	friend class CTestCamImgProc;
	
public:
	static CTestCamera* NewL();
	~CTestCamera();
	void ConstructL();
	// from CCameraPlugin
	void Construct2L(MCameraObserver& aObserver,TInt aCameraIndex);
	void Construct2DupL(MCameraObserver& aObserver,TInt aCameraHandle);
	void Construct2L(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority);
	void Construct2DupL(MCameraObserver2& aObserver,TInt aCameraHandle);
private:
	// From CCamera
	virtual void CameraInfo(TCameraInfo& aInfo) const;
	virtual void Reserve();
	virtual void Release();
	virtual void PowerOn();
	virtual void PowerOff();
	virtual TInt Handle();

	virtual void SetZoomFactorL(TInt aZoomFactor = 0);
	virtual TInt ZoomFactor() const;
	virtual void SetDigitalZoomFactorL(TInt aDigitalZoomFactor = 0);
	virtual TInt DigitalZoomFactor() const;

	virtual void SetContrastL(TInt aContrast);
	virtual TInt Contrast() const;
	virtual void SetBrightnessL(TInt aBrightness);
	virtual TInt Brightness() const;
	virtual void SetFlashL(TFlash aFlash = EFlashNone);
	virtual TFlash Flash() const;
	virtual void SetExposureL(TExposure aExposure = EExposureAuto);
	virtual TExposure Exposure() const;
	virtual void SetWhiteBalanceL(TWhiteBalance aWhiteBalance = EWBAuto);
	virtual TWhiteBalance WhiteBalance() const;

	virtual void StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect);
	virtual void StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect,TRect& aClipRect);
	virtual void StartViewFinderBitmapsL(TSize& aSize);
	virtual void StartViewFinderBitmapsL(TSize& aSize,TRect& aClipRect);
	virtual void StartViewFinderL(TFormat aImageFormat,TSize& aSize);
	virtual void StartViewFinderL(TFormat aImageFormat,TSize& aSize,TRect& aClipRect);
	virtual void StopViewFinder();
	virtual TBool ViewFinderActive() const;
	virtual void SetViewFinderMirrorL(TBool aMirror);
	virtual TBool ViewFinderMirror() const;

	virtual void PrepareImageCaptureL(TFormat aImageFormat,TInt aSizeIndex);
	virtual void PrepareImageCaptureL(TFormat aImageFormat,TInt aSizeIndex,const TRect& aClipRect);
	virtual void CaptureImage();
	virtual void CancelCaptureImage();
	virtual void EnumerateCaptureSizes(TSize& aSize,TInt aSizeIndex,TFormat aFormat) const;

	virtual void PrepareVideoCaptureL(TFormat aFormat,TInt aSizeIndex,TInt aRateIndex,TInt aBuffersToUse,TInt aFramesPerBuffer);
	virtual void PrepareVideoCaptureL(TFormat aFormat,TInt aSizeIndex,TInt aRateIndex,TInt aBuffersToUse,TInt aFramesPerBuffer,const TRect& aClipRect);
	virtual void StartVideoCapture();
	virtual void StopVideoCapture();

	virtual TBool VideoCaptureActive() const;
	virtual void EnumerateVideoFrameSizes(TSize& aSize,TInt aSizeIndex,TFormat aFormat) const;
	virtual void EnumerateVideoFrameRates(TReal32& aRate,TInt aRateIndex,TFormat aFormat,TInt aSizeIndex,TExposure aExposure = EExposureAuto) const;
	virtual void GetFrameSize(TSize& aSize) const;
	virtual TReal32 FrameRate() const;
	virtual TInt BuffersInUse() const;
	virtual TInt FramesPerBuffer() const;

	virtual void SetJpegQuality(TInt aQuality);
	virtual TInt JpegQuality() const;

	virtual TAny* CustomInterface(TUid aInterface);

	// From MAbortDirectScreenAccess
	virtual void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);

	// From MDirectScreenAccess
	virtual void Restart(RDirectScreenAccess::TTerminationReasons aReason);

	// New functions
	static TInt TimerCallBack(TAny* aThis);
	void FrameReady();
	void GetFrame(CFbsBitGc& aGc, TPoint& aPos) const;
	void GetImgProcessedFrame(CFbsBitGc& aGc, TPoint& aPos) const;
	void ServiceVideo(TInt aBuferNum);
	void ServiceViewFinder(CFbsBitmap* aSourceFrame);
	void ServiceImageCapture(const CFbsBitGc& aSourceBitmapGc);
	void ServiceBurstImageCapture(const CFbsBitGc& aSourceBitmapGc);
	void DoCaptureImageL();
	void CheckReserveAndPowerL();
	TInt CheckReserveAndPower() const;
	void CustomInterfaceL(TAny*& aIface, TUid aInterface);
	static TInt ReserveCallBack(TAny* aThis);
	static TInt PowerOnCallBack(TAny* aThis);
	static TInt ImageCaptureCallBack(TAny* aThis);
	static TInt VideoCaptureCallBack(TAny* aThis);
	static TInt HandleEventCallBack(TAny* aThis);
	static TInt HandleEvent2CallBack(TAny* aThis);
	
private:
	 CTestCamera();	
private:
	MCameraObserver* iObserver; // not owned
	MCameraObserver2* iObserver2; // not owned
	TBool iReserved;
	TBool iPowerOn;

	TCameraInfo iInfo;
	CPeriodic* iTimer;
	TTimeIntervalMicroSeconds32 iFrameDelay;
	TInt iFrameIndex;
	TTimeIntervalMicroSeconds iElapsedTime;
	TPoint iStillImagePos;
	TPoint iVideoFramePos;

	TReal32 iVideoFrameRate;
	TInt iBuffersInUse;
	TSize iVideoFrameSize;
	TFormat iVideoFrameFormat;

	CFbsBitmap* iVideoFrame1;
	CFbsBitmapDevice* iVideoFrameDev1;
	CFbsBitGc* iVideoFrameGc1;
	CFbsBitmap* iVideoFrame2;
	CFbsBitmapDevice* iVideoFrameDev2;
	CFbsBitGc* iVideoFrameGc2;

	TInt iStillCaptureErr;
	TSize iStillImageSize;
	TFormat iStillImageFormat;
	CFbsBitmap* iStillImage;
	CFbsBitmapDevice* iStillImageDev;
	CFbsBitGc* iStillImageGc;

	TSize iViewFinderSize;
	TFormat iViewFinderFormat;
	CFbsBitmap* iViewFinderBitmap;
	CFbsBitmapDevice* iViewFinderBitmapDev;
	CFbsBitGc* iViewFinderBitmapGc;

	CDirectScreenAccess* iDSA;
	CFbsScreenDevice* iScreenDev;
	CFbsBitGc* iScreenGc;
	TRect iScreenRect;
	TRect iClipRect;

	CFbsBitmap* iImage;
	CFbsBitmapDevice* iDev;
	CFbsBitGc* iGc;

	CAsyncCallBack iReserveAsync;
	CAsyncCallBack iPowerOnAsync;
	CAsyncCallBack iImageCaptureAsync;
	CAsyncCallBack iVideoCaptureAsync;
	CAsyncCallBack iHandleEventAsync;
	CAsyncCallBack iHandleEvent2Async;

	RTestFrameBuffer iFrameBuffer1;
	RTestFrameBuffer iFrameBuffer2;
	
	RTestCameraFrameBuffer iFrameCameraBuffer1;
	RTestCameraFrameBuffer iFrameCameraBuffer2;

	TInt iVideoCaptureErr;
	// Members to store the current optical or digital value set with SetZoomFactorL() and SetDigitalZoomFactorL()
	// Although arguments have trailing Factor in fact they must contain values in the ranges specified by
	//  	iInfo.iMinZoom : iInfo.iMaxZoom for optical zoom and 
	//		0 : iInfo.iMaxDigitalZoom for digital zoom 
	// The factor values cannot be modified using Camera API as it is assumed 
	// that they are determined by the camera hardware.  
	TInt iZoom;
	TInt iDigitalZoom;
	
	TInt iContrast;
	TInt iBrightness;
	
	TInt iImgProcBrightness;
	TInt iImgProcContrast;
	
	volatile TBool iViewFinderActive;
	volatile TBool iVideoCaptureActive;
	volatile TBool iImageCaptureActive;
	volatile TBool iVideoPreparedLast;
	volatile TBool iVideoCapturePrepared;
	volatile TBool iImageCapturePrepared;

	RArray<TSize> iImageSizes;
	RArray<TSize> iVideoFrameSizes;
	RArray<TReal32> iVideoFrameRates;
	TInt iCameraIndex;
	TInt iCameraHandle;
	TInt iPriority;	
	
	TECAMEvent iECamEvent;
	TECAMEvent2 iECamEvent2;
	TInt iEventError;
	
	CTestCamAdvSet* iAdvSettingsImpl;
	CTestCamSnapshot* iSnapshotImpl;
	CTestCamImgProc* iImgProcImpl;
	
	CCamera::CCameraAdvancedSettings::TDriveMode iDriveMode;
	TInt iBurstImages;
	};
	
class CTestCameraInfo : public CCameraInfoPlugin
	{
public:
	static CTestCameraInfo* NewL();
	~CTestCameraInfo();
	// from CCameraInfoPlugin
	TInt CamerasAvailable();
private:
	CTestCameraInfo();	
	};
	
enum TStubPanicReasons
	{
	EPanicUnimplemented=1
	};	

class FileDependencyUtil
	{
public:
	static void CheckFileDependencyL();
	};

class CDataGlobal : public CBase
	{
public:
	CTestCamAdvSet* iTestCamAdvSet;
	CTestCamPresets* iTestCamPresets;
	CTestCamSnapshot* iTestCamSnapshot;
	CTestCamImgProc* iTestCamImgProc;
	TInt 		iAdvSetReferenceCount;	
	TInt 		iPresetsReferenceCount;
	TInt 		iSnapshotReferenceCount;	
	TInt 		iImgProcReferenceCount;
	};
	
#endif
