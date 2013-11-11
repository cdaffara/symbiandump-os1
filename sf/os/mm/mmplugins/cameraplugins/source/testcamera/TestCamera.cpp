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

#include <w32std.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <f32file.h> 
#include "TestCamera.h"
#include "test_advanced_settings.h"
#include "test_snapshot.h"
#include "test_image_processing.h"
#include "TestCameraUids.hrh"
#include <ecam.h>

//
// Main stuff
//
// 
// 2nd stage ConstructL calls called by framework
//
CTestCamera* CTestCamera::NewL()
	{
	FileDependencyUtil::CheckFileDependencyL();
   	CTestCamera* self = new(ELeave) CTestCamera;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestCamera::Construct2L(MCameraObserver& aObserver,TInt aCameraIndex)
	{
	iObserver = &aObserver;
	iCameraIndex = aCameraIndex;
	}
	
void CTestCamera::Construct2DupL(MCameraObserver& aObserver,TInt aCameraHandle)
	{
	iObserver = &aObserver;
	iCameraHandle = aCameraHandle;
	}

void CTestCamera::ConstructL()	
	{
	
	TInt err;
	TRAP(err, iScreenDev = CFbsScreenDevice::NewL(_L(""),EColor16MA));
	if (err == KErrNotSupported)
		{
		TRAP(err, iScreenDev = CFbsScreenDevice::NewL(_L(""),EColor16M));
		}
	if (err == KErrNotSupported)
		{
		TRAP(err, iScreenDev = CFbsScreenDevice::NewL(_L(""),EColor64K));
		}
	if (err == KErrNotSupported)
		{
		TRAP(err, iScreenDev = CFbsScreenDevice::NewL(_L(""),EColor4K));
		}
	if (err == KErrNotSupported)
		{
		TRAP(err, iScreenDev = CFbsScreenDevice::NewL(_L(""),EColor256));
		}
	if (err == KErrNotSupported)
		{
		iScreenDev = CFbsScreenDevice::NewL(_L(""),EColor16MAP);
		}		
	else
		{
		User::LeaveIfError(err);		
		}

	User::LeaveIfError(iScreenDev->CreateContext(iScreenGc));
	User::LeaveIfError(iImageSizes.Append( TSize(640, 480) )); // VGA
	User::LeaveIfError(iImageSizes.Append( TSize(160, 120) )); // QQVGA
	iInfo.iNumImageSizesSupported = iImageSizes.Count();
	iStillImageSize = iImageSizes[1]; // Defaults to QQVGA

	User::LeaveIfError(iVideoFrameSizes.Append( TSize(176, 144) )); // QCIF
	User::LeaveIfError(iVideoFrameSizes.Append( TSize(128, 96) ));  // subQCIF
	iInfo.iNumVideoFrameSizesSupported = iVideoFrameSizes.Count();
	User::LeaveIfError(iVideoFrameRates.Append( 10.00 ));
	User::LeaveIfError(iVideoFrameRates.Append(  5.00 ));
	User::LeaveIfError(iVideoFrameRates.Append(  1.00 ));
	iInfo.iNumVideoFrameRatesSupported = iVideoFrameRates.Count();
	iVideoFrameRate = iVideoFrameRates[0];
	// Still image used as view finder source if not defined otherwise
	iStillImage = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iStillImage->Create(iStillImageSize, KViewFinderDisplayMode));
	iStillImageDev = CFbsBitmapDevice::NewL(iStillImage);
	User::LeaveIfError(iStillImageDev->CreateContext(iStillImageGc));
	iStillImageGc->SetPenColor(KRgbBlack);
	iStillImageGc->SetPenSize(TSize(KFrameFeatureBorderThickness, KFrameFeatureBorderThickness));
	iStillImageGc->SetBrushColor(KRgbWhite);
	iStillImageGc->SetFaded(ETrue);
	iVideoPreparedLast = EFalse;
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	}


void CTestCamera::Construct2L(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority)
	{
	iObserver2 = &aObserver;
	iCameraIndex = aCameraIndex;
	iPriority = aPriority;
	}
	
void CTestCamera::Construct2DupL(MCameraObserver2& aObserver,TInt aCameraHandle)
	{
	iObserver2 = &aObserver;
	iCameraHandle = aCameraHandle;
	}
	

CTestCamera::CTestCamera():
	iStillImageFormat(KInitialViewFinderFormat),
	iReserveAsync(CActive::EPriorityStandard),
	iPowerOnAsync(CActive::EPriorityStandard),
	iImageCaptureAsync(CActive::EPriorityStandard),
	iVideoCaptureAsync(CActive::EPriorityStandard),
	iHandleEventAsync(CActive::EPriorityStandard),
	iHandleEvent2Async(CActive::EPriorityStandard),	
	iFrameBuffer1(iVideoFrame1),
	iFrameBuffer2(iVideoFrame2),
	iFrameCameraBuffer1(iVideoFrame1),
	iFrameCameraBuffer2(iVideoFrame2),
	iContrast(0),
	iBrightness(0),
	iImgProcBrightness(0),
	iImgProcContrast(0),
	iVideoCaptureActive(EFalse),
	iImageCaptureActive(EFalse),
	iVideoPreparedLast(EFalse),
	iVideoCapturePrepared(EFalse),
	iImageCapturePrepared(EFalse),
	iECamEvent(KNullUid, KErrNone),
	iECamEvent2(KNullUid, KErrNone, 0),
	iAdvSettingsImpl(NULL),
	iSnapshotImpl(NULL),
	iImgProcImpl(NULL),
	iDriveMode(KDefaultDriveMode),
	iBurstImages(KBurstImages)
	{
	TCallBack reserveCallBack(ReserveCallBack,this);
	iReserveAsync.Set(reserveCallBack);
	TCallBack powerOnCallBack(PowerOnCallBack,this);
	iPowerOnAsync.Set(powerOnCallBack);
	TCallBack imageCaptureCallBack(ImageCaptureCallBack,this);
	iImageCaptureAsync.Set(imageCaptureCallBack);
	TCallBack videoCaptureCallBack(VideoCaptureCallBack,this);
	iVideoCaptureAsync.Set(videoCaptureCallBack);
	TCallBack handleEventCallBack(HandleEventCallBack,this);
	iHandleEventAsync.Set(handleEventCallBack);
	TCallBack handleEvent2CallBack(HandleEvent2CallBack,this);
	iHandleEvent2Async.Set(handleEvent2CallBack);

	iInfo.iHardwareVersion.iMajor = 0;
	iInfo.iHardwareVersion.iMinor = 0;
	iInfo.iHardwareVersion.iBuild = 0;
	iInfo.iSoftwareVersion.iMajor = 0;
	iInfo.iSoftwareVersion.iMinor = 0;
	iInfo.iSoftwareVersion.iBuild = 0;
	iInfo.iOrientation = TCameraInfo::EOrientationOutwards;

	iInfo.iOptionsSupported = 0;
	iInfo.iOptionsSupported |= TCameraInfo::EViewFinderDirectSupported;
	iInfo.iOptionsSupported |= TCameraInfo::EViewFinderBitmapsSupported;
	iInfo.iOptionsSupported |= TCameraInfo::EImageCaptureSupported;
	iInfo.iOptionsSupported |= TCameraInfo::EVideoCaptureSupported;
	iInfo.iOptionsSupported |= TCameraInfo::EContrastSupported;
	iInfo.iOptionsSupported |= TCameraInfo::EBrightnessSupported;
	iInfo.iOptionsSupported |= TCameraInfo::EViewFinderClippingSupported;

	iInfo.iFlashModesSupported = 0; // Bitfield of TFlash values
	iInfo.iExposureModesSupported = 0; // Bitfield of TExposure values
	iInfo.iWhiteBalanceModesSupported = 0; // Bitfield of TWhiteBalance values

	iInfo.iMinZoom = KMinTestCameraZoom;
	// KMinTestCameraZoom is zero or negative
	// note the algorithm for creating of zoom factor
	iInfo.iMinZoomFactor = TReal32(1)/TReal32(1 << -KMinTestCameraZoom);
	iInfo.iMaxZoom = KMaxTestCameraZoom;
	iInfo.iMaxZoomFactor = 1 << KMaxTestCameraZoom;

	iInfo.iMaxDigitalZoom = KMaxTestCameraDigitalZoom;
	iInfo.iMaxDigitalZoomFactor = KMaxTestCameraDigitalZoomFactor;

	iInfo.iImageFormatsSupported = 0;
	iInfo.iImageFormatsSupported |= EFormatFbsBitmapColor4K;
	iInfo.iImageFormatsSupported |= EFormatFbsBitmapColor64K;
	iInfo.iImageFormatsSupported |= EFormatFbsBitmapColor16M;

	iInfo.iVideoFrameFormatsSupported = 0;
	iInfo.iVideoFrameFormatsSupported |= EFormatFbsBitmapColor4K;
	iInfo.iVideoFrameFormatsSupported |= EFormatFbsBitmapColor64K;
	iInfo.iVideoFrameFormatsSupported |= EFormatFbsBitmapColor16M;
	iInfo.iMaxFramesPerBufferSupported = 1;
	iInfo.iMaxBuffersSupported = 2;
	
	iScreenRect = TRect(TPoint(0,0), TPoint(0,0));
	
	iEventError = KErrNone;
	}
	
CTestCamera::~CTestCamera()
	{
	delete iTimer;
	delete iStillImage;
	delete iStillImageGc;
	delete iStillImageDev;
	delete iVideoFrame1;
	delete iVideoFrameGc1;
	delete iVideoFrameDev1;
	delete iVideoFrame2;
	delete iVideoFrameGc2;
	delete iVideoFrameDev2;
	delete iViewFinderBitmapGc;
	delete iViewFinderBitmapDev;
	delete iViewFinderBitmap;
	delete iDSA;
	delete iScreenGc;
	delete iScreenDev;
	delete iGc;
	delete iDev;
	delete iImage;
	delete iAdvSettingsImpl;
	delete iSnapshotImpl;
	delete iImgProcImpl;
	iImageSizes.Reset();
	iVideoFrameSizes.Reset();
	iVideoFrameRates.Reset();
	}

void CTestCamera::CameraInfo(TCameraInfo& aInfo) const
	{
	aInfo = iInfo;
	}

void CTestCamera::Reserve()
	{
	iReserved = ETrue;
	iReserveAsync.CallBack();
	}

void CTestCamera::Release()
	{
	iReserved = EFalse;
	iTimer->Cancel();
	}

void CTestCamera::PowerOn()
	{
	if (!iReserved || iPowerOn)
		{
		iPowerOnAsync.CallBack();
		return;
		}

	iPowerOn = ETrue;
	TCallBack callback(TimerCallBack,this);
	iTimer->Start(0,TInt(TReal32(1000000) / iVideoFrameRate), callback);
	iPowerOnAsync.CallBack();
	}

void CTestCamera::PowerOff()
	{
	iPowerOn = EFalse;
	iTimer->Cancel();
	}

TInt CTestCamera::Handle()
	{
	return 0;
	}

void CTestCamera::SetZoomFactorL(TInt aZoomFactor)
	{
	// it is equivalent to comparing with iInfo members as they are initialised with the same constants
	// if (aZoomFactor < iInfo.iMinZoom || aZoomFactor > iInfo.iMaxZoom)
	if (aZoomFactor < KMinTestCameraZoom || aZoomFactor > KMaxTestCameraZoom)
		{
		User::Leave(KErrNotSupported);	
		}

	iZoom = aZoomFactor;
	}

TInt CTestCamera::ZoomFactor() const
	{
	// default value is 0
	return iZoom;
	}

void CTestCamera::SetDigitalZoomFactorL(TInt aDigitalZoomFactor)
	{
	CheckReserveAndPowerL();
	// it is equivalent to comparing with iInfo member as it is initialised with the same constant
	// if (aDigitalZoomFactor < 0 || aDigitalZoomFactor > iInfo.iMaxDigitalZoom)
	if (aDigitalZoomFactor < 0 || aDigitalZoomFactor > KMaxTestCameraDigitalZoom)
		{
		User::Leave(KErrNotSupported);	
		}

	iDigitalZoom = aDigitalZoomFactor;
	}

TInt CTestCamera::DigitalZoomFactor() const
	{
	// default value is 0
	return iDigitalZoom;
	}

void CTestCamera::SetContrastL(TInt aContrast)
	{
	CheckReserveAndPowerL();

	iContrast = Min(Max(aContrast,-100),100);
	}

TInt CTestCamera::Contrast() const
	{
	return iContrast;
	}

void CTestCamera::SetBrightnessL(TInt aBrightness)
	{
	CheckReserveAndPowerL();

	iBrightness = Min(Max(aBrightness,-100),100);
	}

TInt CTestCamera::Brightness() const
	{
	return iBrightness;
	}

void CTestCamera::SetFlashL(TFlash aFlash)
	{
	CheckReserveAndPowerL();

	if (aFlash != EFlashNone)
		{
		User::Leave(KErrNotSupported);	
		}
	}

CCamera::TFlash CTestCamera::Flash() const
	{
	return EFlashNone;
	}

void CTestCamera::SetExposureL(TExposure aExposure)
	{
	CheckReserveAndPowerL();

	if (aExposure != EExposureAuto)
		{
		User::Leave(KErrNotSupported);
		}
	}

CCamera::TExposure CTestCamera::Exposure() const
	{
	return EExposureAuto;
	}

void CTestCamera::SetWhiteBalanceL(TWhiteBalance aWhiteBalance)
	{
	CheckReserveAndPowerL();

	if (aWhiteBalance != EWBAuto)
		{
		User::Leave(KErrNotSupported);
		}
	}

CCamera::TWhiteBalance CTestCamera::WhiteBalance() const
	{
	return EWBAuto;
	}

void CTestCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect)
	{
	TRect emptyRect;
	StartViewFinderDirectL(aWs,aScreenDevice,aWindow,aScreenRect,emptyRect);
	}

void CTestCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect,TRect& aClipRect)
	{
	if (!iReserved)
		{
		User::Leave(KErrInUse);		
		}
		
	if (!iPowerOn || iViewFinderActive)
		{
		User::Leave(KErrNotReady);	
		}

	iScreenRect = aScreenRect;
	iClipRect = aClipRect;

	delete iDSA;
	iDSA = NULL;
	iViewFinderActive = EFalse;
	iDSA = CDirectScreenAccess::NewL(aWs,aScreenDevice,aWindow,*this);
	iDSA->StartL();
	iViewFinderActive = ETrue;
	iDSA->Gc()->SetOrigin(); // Set Origin to top left of screen (0,0)

	if (!iClipRect.IsEmpty())
		{
		iDSA->Gc()->SetClippingRect(iClipRect);	
		}
	}

void CTestCamera::StartViewFinderBitmapsL(TSize& aSize)
	{
	TRect emptyRect;
	StartViewFinderBitmapsL(aSize,emptyRect);
	}

void CTestCamera::StartViewFinderBitmapsL(TSize& aSize, TRect& aClipRect)
	{
	if (!iReserved)
		{
		User::Leave(KErrInUse);
		}
	if (!iPowerOn || iViewFinderActive)
		{
		User::Leave(KErrNotReady);
		}

	iViewFinderActive = ETrue;
	iScreenRect = TRect(TPoint(0,0), aSize);
	iClipRect = aClipRect;
	iClipRect.Intersection(iScreenRect);

	delete iViewFinderBitmapGc;
	iViewFinderBitmapGc = NULL;
	delete iViewFinderBitmapDev;
	iViewFinderBitmapDev = NULL;
	delete iViewFinderBitmap;
	iViewFinderBitmap = NULL;

	iViewFinderBitmap = new(ELeave) CFbsBitmap;
	if (!iClipRect.IsEmpty())
		{
		User::LeaveIfError(iViewFinderBitmap->Create(iClipRect.Size(), KViewFinderDisplayMode));		
		}
	else
		{
		User::LeaveIfError(iViewFinderBitmap->Create(aSize, KViewFinderDisplayMode));		
		}
	iViewFinderBitmapDev = CFbsBitmapDevice::NewL(iViewFinderBitmap);
	User::LeaveIfError(iViewFinderBitmapDev->CreateContext(iViewFinderBitmapGc));
	}
	
void CTestCamera::StartViewFinderL(TFormat aImageFormat,TSize& aSize)
       {
       TRect emptyRect;
       StartViewFinderL(aImageFormat,aSize,emptyRect);
       }
	
void CTestCamera::StartViewFinderL(TFormat aImageFormat,TSize& /*aSize*/,TRect& aClipRect)	
       {
     	if(!iReserved)
     		{
     		User::Leave(KErrInUse);
     		}
		if(!iPowerOn || iViewFinderActive)
			{
			User::Leave(KErrNotReady);
			}
		
		iViewFinderActive = ETrue;
		iClipRect = aClipRect;
		if(!(aImageFormat & iInfo.iImageFormatsSupported))
			{
			 User::Leave(KErrNotSupported);
			}
       }
       

void CTestCamera::StopViewFinder()
	{
	iViewFinderActive = EFalse;
	delete iDSA;
	iDSA = NULL;

	delete iViewFinderBitmapGc;
	iViewFinderBitmapGc = NULL;
	delete iViewFinderBitmapDev;
	iViewFinderBitmapDev = NULL;
	delete iViewFinderBitmap;
	iViewFinderBitmap = NULL;
	}

TBool CTestCamera::ViewFinderActive() const
	{
	return iViewFinderActive;
	}

void CTestCamera::SetViewFinderMirrorL(TBool aMirror)
	{
	if (aMirror)
		{
		User::Leave(KErrNotSupported);	
		}
	}

TBool CTestCamera::ViewFinderMirror() const
	{
	return EFalse;
	}

void CTestCamera::PrepareImageCaptureL(TFormat aImageFormat,TInt aSizeIndex)
	{
	if (!iReserved)
		{
		User::Leave(KErrInUse);
		}
		
	if (!iPowerOn || iVideoCaptureActive || iImageCaptureActive)
		{
		User::Leave(KErrNotReady);
		}
		

	if (!(aImageFormat & iInfo.iImageFormatsSupported) || aSizeIndex < 0
		|| aSizeIndex >= iInfo.iNumImageSizesSupported)
		{
		User::Leave(KErrNotSupported);
		}

	iVideoPreparedLast = EFalse;

	if (iImageCapturePrepared
		&& iStillImageSize == iImageSizes[aSizeIndex]
		&& iStillImageFormat == aImageFormat)
		{
		return;
		}

	iStillImageSize = iImageSizes[aSizeIndex];
	iStillImageFormat = aImageFormat;

	TDisplayMode displayMode = ENone;
	switch (iStillImageFormat)
		{
		case EFormatFbsBitmapColor4K :
			displayMode = EColor4K;
			break;
		case EFormatFbsBitmapColor64K :
			displayMode = EColor64K;
			break;
		case EFormatFbsBitmapColor16M :
			displayMode = EColor16M;
			break;
		default :
			User::Leave(KErrNotSupported);
			break;
		}

	delete iStillImageGc;
	iStillImageGc = NULL;
	delete iStillImageDev;
	iStillImageDev = NULL;
	delete iStillImage;
	iStillImage = NULL;

	iStillImage = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iStillImage->Create(iStillImageSize, displayMode));
	iStillImageDev = CFbsBitmapDevice::NewL(iStillImage);
	User::LeaveIfError(iStillImageDev->CreateContext(iStillImageGc));
	iStillImageGc->SetPenColor(KRgbBlack);
	iStillImageGc->SetPenSize(TSize(KFrameFeatureBorderThickness, KFrameFeatureBorderThickness));
	iStillImageGc->SetBrushColor(KRgbWhite);
	iStillImageGc->SetFaded(ETrue);
	iImageCapturePrepared = ETrue;
	}

void CTestCamera::PrepareImageCaptureL(TFormat /*aImageFormat*/,TInt /*aSizeIndex*/,const TRect& /*aClipRect*/)
	{
	User::Leave(KErrNotSupported);
	}

void CTestCamera::CaptureImage()
	{
	if (!iReserved)
		{
		iStillCaptureErr = KErrInUse;
		}
		
	if (!iPowerOn || iImageCaptureActive || iVideoCaptureActive || !iImageCapturePrepared)
		{
		iStillCaptureErr = KErrNotReady;
		}

	if (iStillCaptureErr == KErrNone)
		{
		TRAP(iStillCaptureErr,DoCaptureImageL());
		}

	if (iStillCaptureErr == KErrNone)
		{
		iImageCaptureActive = ETrue;
		}
	else
		{
		iImageCaptureAsync.CallBack();
		}
	}

void CTestCamera::DoCaptureImageL()
	{
	delete iGc;
	iGc = NULL;
	delete iDev;
	iDev = NULL;
	delete iImage;
	iImage = NULL;

	CFbsBitmap* image = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(image);
	User::LeaveIfError(image->Create(iStillImageSize, iStillImage->DisplayMode()));
	CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(image);
	CleanupStack::PushL(dev);
	CFbsBitGc* gc = NULL;
	User::LeaveIfError(dev->CreateContext(gc));
	CleanupStack::Pop(dev);
	CleanupStack::Pop(image);

	iImage = image;
	iDev = dev;
	iGc = gc;
	}

void CTestCamera::CancelCaptureImage()
	{
	iImageCaptureActive = EFalse;
	}

void CTestCamera::EnumerateCaptureSizes(TSize& aSize,TInt aSizeIndex,TFormat aFormat) const
	{
	if (aSizeIndex < 0 || aSizeIndex >= iInfo.iNumImageSizesSupported ||
		!(aFormat & iInfo.iImageFormatsSupported) )
		{
		aSize = TSize(0,0);
		}
	else
		{
		aSize = iImageSizes[aSizeIndex];
		}
	}

void CTestCamera::PrepareVideoCaptureL(TFormat aFormat,TInt aSizeIndex,TInt aRateIndex,TInt aBuffersToUse,TInt aFramesPerBuffer)
	{
	if (!iReserved)
		{
		User::Leave(KErrInUse);
		}
		
	if (!iPowerOn || iVideoCaptureActive || iImageCaptureActive)
		{
		User::Leave(KErrNotReady);
		}

	if (!(aFormat & iInfo.iVideoFrameFormatsSupported) 
		|| aSizeIndex < 0 || aSizeIndex >= iInfo.iNumVideoFrameSizesSupported
		|| aRateIndex < 0 || aRateIndex >= iInfo.iNumVideoFrameRatesSupported
		|| aBuffersToUse != 2 || aFramesPerBuffer != 1)
		{
		User::Leave(KErrNotSupported);
		}

	iVideoPreparedLast = ETrue;

	if (iVideoCapturePrepared 
		&& iVideoFrameSize == iVideoFrameSizes[aSizeIndex]
		&& iVideoFrameFormat == aFormat)
		{
		return;
		}

	TDisplayMode displayMode = ENone;
	switch (aFormat)
		{
		case EFormatFbsBitmapColor4K :
			displayMode = EColor4K;
			break;
		case EFormatFbsBitmapColor64K :
			displayMode = EColor64K;
			break;
		case EFormatFbsBitmapColor16M :
			displayMode = EColor16M;
			break;
		default :
			User::Leave(KErrNotSupported);
			break;
		}

	iVideoFrameSize = iVideoFrameSizes[aSizeIndex];
	iVideoFrameRate = iVideoFrameRates[aRateIndex];
	iVideoFrameFormat = aFormat;
	iBuffersInUse = aBuffersToUse;

	delete iVideoFrameGc1;
	iVideoFrameGc1 = NULL;
	delete iVideoFrameDev1;
	iVideoFrameDev1 = NULL;
	delete iVideoFrame1;
	iVideoFrame1 = NULL;

	delete iVideoFrameGc2;
	iVideoFrameGc2 = NULL;
	delete iVideoFrameDev2;
	iVideoFrameDev2 = NULL;
	delete iVideoFrame2;
	iVideoFrame2 = NULL;

	iVideoFrame1 = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iVideoFrame1->Create(iVideoFrameSize, displayMode));
	iVideoFrameDev1 = CFbsBitmapDevice::NewL(iVideoFrame1);
	User::LeaveIfError(iVideoFrameDev1->CreateContext(iVideoFrameGc1));
	iVideoFrameGc1->SetPenColor(KRgbBlack);
	iVideoFrameGc1->SetPenSize(TSize(KFrameFeatureBorderThickness, KFrameFeatureBorderThickness));
	iVideoFrameGc1->SetBrushColor(KRgbWhite);
	iVideoFrameGc1->SetFaded(ETrue);

	if (iBuffersInUse == 2)
		{
		iVideoFrame2 = new(ELeave) CFbsBitmap;
		User::LeaveIfError(iVideoFrame2->Create(iVideoFrameSize, displayMode));
		iVideoFrameDev2 = CFbsBitmapDevice::NewL(iVideoFrame2);
		User::LeaveIfError(iVideoFrameDev1->CreateContext(iVideoFrameGc2));
		iVideoFrameGc2->SetPenColor(KRgbBlack);
		iVideoFrameGc2->SetPenSize(TSize(KFrameFeatureBorderThickness, KFrameFeatureBorderThickness));
		iVideoFrameGc2->SetBrushColor(KRgbWhite);
		iVideoFrameGc2->SetFaded(ETrue);		
		}

	iVideoCapturePrepared = ETrue;
	}

void CTestCamera::PrepareVideoCaptureL(TFormat /*aFormat*/,TInt /*aSizeIndex*/,TInt /*aRateIndex*/,TInt /*aBuffersToUse*/,TInt /*aFramesPerBuffer*/,const TRect& /*aClipRect*/)
	{
	User::Leave(KErrNotSupported);
	}

void CTestCamera::StartVideoCapture()
	{
	if (!iReserved)
		{
		iVideoCaptureErr = KErrInUse;
		}
		
	if (!iPowerOn || iImageCaptureActive || iVideoCaptureActive || !iVideoCapturePrepared)
		{
		iVideoCaptureErr = KErrNotReady;
		}
		
	if (iVideoCaptureErr == KErrNone)
		{
		iVideoCaptureActive = ETrue;
		}
		
	else
		{
		iVideoCaptureAsync.CallBack();
		}
	}

void CTestCamera::StopVideoCapture()
	{
	iVideoCaptureActive = EFalse;
	iFrameIndex = 0;
	iElapsedTime = 0;
	}

TBool CTestCamera::VideoCaptureActive() const
	{
	return iVideoCaptureActive;
	}

void CTestCamera::EnumerateVideoFrameSizes(TSize& aSize,TInt aSizeIndex,TFormat aFormat) const
	{
	if (aSizeIndex < 0 || aSizeIndex >= iInfo.iNumVideoFrameSizesSupported ||
		!(aFormat & iInfo.iVideoFrameFormatsSupported))
		{
		aSize = TSize(0,0);
		}
	else
		{
		aSize = iVideoFrameSizes[aSizeIndex];
		}
	}

void CTestCamera::EnumerateVideoFrameRates(TReal32& aRate,TInt aRateIndex,TFormat aFormat,TInt aSizeIndex,TExposure aExposure) const
	{
	if (aRateIndex < 0 || aRateIndex >= iInfo.iNumVideoFrameRatesSupported ||
		aSizeIndex < 0 || aSizeIndex >= iInfo.iNumVideoFrameSizesSupported ||
		!(aFormat & iInfo.iVideoFrameFormatsSupported) ||
		(!(aExposure & iInfo.iExposureModesSupported) && aExposure != CCamera::EExposureAuto))
		{
		aRate = 0.0;
		}
	else
		{
		aRate = iVideoFrameRates[aRateIndex];
		}
	}

void CTestCamera::GetFrameSize(TSize& aSize) const
	{
	aSize = iVideoFrameSize;
	}

TReal32 CTestCamera::FrameRate() const
	{
	return iVideoFrameRate;
	}

TInt CTestCamera::BuffersInUse() const
	{
	return iBuffersInUse;
	}

TInt CTestCamera::FramesPerBuffer() const
	{
	return 1;
	}

void CTestCamera::SetJpegQuality(TInt /*aQuality*/)
	{
	}

TInt CTestCamera::JpegQuality() const
	{
	return 0;
	}

TAny* CTestCamera::CustomInterface(TUid aInterface)
	{
	TAny* ptrIface = NULL;
	TRAPD(error,CustomInterfaceL(ptrIface, aInterface));
	
	if(error == KErrNone)
		{
		switch(aInterface.iUid)
			{
			// advanced settings interface pointers
			case KECamMCameraAdvancedSettingsUidValue:
				{
				return static_cast<MCameraAdvancedSettings*>(ptrIface);
				}
			case KECamMCameraAdvancedSettings2UidValue:
				{
				return static_cast<MCameraAdvancedSettings2*>(ptrIface);			
				}
			case KECamMCameraAdvancedSettings3UidValue:
				{
				return static_cast<MCameraAdvancedSettings3*>(ptrIface);
				}
			
			// snapshot interface pointers	
			case KECamMCameraSnapshotUidValue:
				{
				return static_cast<MCameraSnapshot*>(ptrIface);
				}
				
			// preset interface pointers	
			case KECamMCameraPresetsUidValue:
				return static_cast<MCameraPresets*>(ptrIface);
				
			// image processing interface pointers
			case KECamMCameraImageProcessingUidValue:
				{
				return static_cast<MCameraImageProcessing*>(ptrIface);
				}
				
			case KECamMCameraImageProcessing2UidValue:
				{
				return static_cast<MCameraImageProcessing2*>(ptrIface);
				}
				
			default:
				return NULL;
			}
		}
	else
		{
		return NULL;		
		}
	}

void CTestCamera::CustomInterfaceL(TAny*& aIface, TUid aInterface)
	{
	switch(aInterface.iUid)
		{
		// advanced settings interface pointers
		case KECamMCameraAdvancedSettingsUidValue:
			{
			iAdvSettingsImpl = CTestCamAdvSet::NewL(*this);
			aIface = static_cast<MCameraAdvancedSettings*>(iAdvSettingsImpl);
			break;
			}
		case KECamMCameraAdvancedSettings2UidValue:
			{
			iAdvSettingsImpl = CTestCamAdvSet::NewL(*this);
			aIface = static_cast<MCameraAdvancedSettings2*>(iAdvSettingsImpl);			
			break;
			}
		case KECamMCameraAdvancedSettings3UidValue:
			{
			iAdvSettingsImpl = CTestCamAdvSet::NewL(*this);
			aIface = static_cast<MCameraAdvancedSettings3*>(iAdvSettingsImpl);
			break;
			}
		
		// snapshot interface pointers	
		case KECamMCameraSnapshotUidValue:
			{
			iSnapshotImpl = CTestCamSnapshot::NewL(*this);
			aIface = static_cast<MCameraSnapshot*>(iSnapshotImpl);
			break;
			}
		// preset interface pointers	
		case KECamMCameraPresetsUidValue:
			{
			aIface = static_cast<MCameraPresets*>(CTestCamPresets::NewL(*this));
			break;
			}
			
		// image processing interface pointers
		case KECamMCameraImageProcessingUidValue:
			{
			iImgProcImpl = CTestCamImgProc::NewL(*this);	
			aIface = static_cast<MCameraImageProcessing*>(iImgProcImpl);
			break;
			}
			
		case KECamMCameraImageProcessing2UidValue:
			{
			iImgProcImpl = CTestCamImgProc::NewL(*this);	
			aIface = static_cast<MCameraImageProcessing2*>(iImgProcImpl);
			break;
			}
			
		default:
			aIface = NULL;
		}
	}

void CTestCamera::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	iViewFinderActive = EFalse;
	}

void CTestCamera::Restart(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	TRAPD(error,iDSA->StartL());
	if (error != KErrNone)
		{
		return;
		}
	iDSA->Gc()->SetOrigin(); // Set Origin to top left of screen (0,0)

	if (!iClipRect.IsEmpty())
		{
		iDSA->Gc()->SetClippingRect(iClipRect);
		}
	else
		{
		iDSA->Gc()->CancelClippingRect();
		}

	iViewFinderActive = ETrue;
	}

TInt CTestCamera::TimerCallBack(TAny* aThis)
	{
	static_cast<CTestCamera*>(aThis)->FrameReady();
	return KErrNone;
	}

void CTestCamera::FrameReady()
	{
	if (iVideoPreparedLast)
		{
		iVideoFramePos.iX += KFramePosIncrement;
		iVideoFramePos.iY += KFramePosIncrement;

		if (iVideoFramePos.iX >= iVideoFrameSize.iWidth)
			{
			iVideoFramePos.iX = 0;
			}
			
		if (iVideoFramePos.iY >= iVideoFrameSize.iHeight)
			{
			iVideoFramePos.iY = 0;
			}
		}
	else
		{
		iStillImagePos.iX += KFramePosIncrement;
		iStillImagePos.iY += KFramePosIncrement;

		if (iStillImagePos.iX >= iStillImageSize.iWidth)
			{
			iStillImagePos.iX = 0;
			}
			
		if (iStillImagePos.iY >= iStillImageSize.iHeight)
			{
			iStillImagePos.iY = 0;
			}
		}

	if (iImageCaptureActive)
		{
		if (!iImageCapturePrepared)
			{
			iImageCaptureActive = EFalse;
			if (iObserver2)
				{
				iObserver2->ImageBufferReady(iFrameCameraBuffer1,KErrNotReady);
				}
			else
				{
				iObserver->ImageReady(NULL,NULL,KErrNotReady);
				}
			}
		else
			{
			if(iDriveMode == CCamera::CCameraAdvancedSettings::EDriveModeBurst)
				{
				for (TInt index =0; index < iBurstImages -1; index++)
					{
					if(iSnapshotImpl != NULL)
						{
						if(iSnapshotImpl->iSnapshotActive)
							{
							GetFrame(*(iSnapshotImpl->iSnapshotImageGc), iStillImagePos);
							iSnapshotImpl->ServiceBurstImageSnapshot(*(iSnapshotImpl->iSnapshotImageGc));
							}
						}
					
					GetImgProcessedFrame(*iStillImageGc, iStillImagePos);
					ServiceBurstImageCapture(*iStillImageGc);
					}
				}
				
			// normal case, single shot and last image from burst mode
			if(iSnapshotImpl != NULL)
				{
				if(iSnapshotImpl->iSnapshotActive)
					{
					GetFrame(*iSnapshotImpl->iSnapshotImageGc, iStillImagePos);
					iSnapshotImpl->ServiceSnapshot(*(iSnapshotImpl->iSnapshotImageGc));
					}
				}
				
			GetImgProcessedFrame(*iStillImageGc, iStillImagePos);
			ServiceImageCapture(*iStillImageGc);	
							
			if (iVideoPreparedLast)
				{
				GetFrame(*iVideoFrameGc1, iVideoFramePos);
				if (iViewFinderActive)
					{
					ServiceViewFinder(iVideoFrame1);
					}	
				}
			else
				{
				if (iViewFinderActive)
					{
					ServiceViewFinder(iStillImage);
					}
				}
			}
		}

	else if (iVideoCaptureActive)
		{
		if (!iVideoCapturePrepared)
			{
			iVideoCaptureActive = EFalse;
			if (iObserver2)
				{
				iObserver2->VideoBufferReady(iFrameCameraBuffer1,KErrNotReady);
				}
			else
				{
				iObserver->FrameBufferReady(NULL,KErrNotReady);
				}
			}
		else
			{
			if (iObserver2)
				{
				if (iFrameCameraBuffer1.iAvailable)
					{
					GetFrame(*iVideoFrameGc1, iVideoFramePos);
					ServiceVideo(1);
					if (iViewFinderActive)
						{
						ServiceViewFinder(iVideoFrame1);
						}
					}
				else if (iFrameCameraBuffer2.iAvailable && iBuffersInUse == 2)
					{
					GetFrame(*iVideoFrameGc2, iVideoFramePos);
					ServiceVideo(2);
					if (iViewFinderActive)
						{
						ServiceViewFinder(iVideoFrame2);
						}
					}
				}
			else
				{
				if (iFrameBuffer1.iAvailable)
					{
					GetFrame(*iVideoFrameGc1, iVideoFramePos);
					ServiceVideo(1);
					if (iViewFinderActive)
						{
						ServiceViewFinder(iVideoFrame1);
						}
					}
				else if (iFrameBuffer2.iAvailable && iBuffersInUse == 2)
					{
					GetFrame(*iVideoFrameGc2, iVideoFramePos);
					ServiceVideo(2);
					if (iViewFinderActive)
						{
						ServiceViewFinder(iVideoFrame2);
						}
					}
				}
			}
	}

	else if (iViewFinderActive)
		{
		if (iVideoPreparedLast)
			{
			GetFrame(*iVideoFrameGc1, iVideoFramePos);
			ServiceViewFinder(iVideoFrame1);
			}
			else
			{
			GetFrame(*iStillImageGc, iStillImagePos);
			ServiceViewFinder(iStillImage);
			}
		}
	}

void CTestCamera::GetFrame(CFbsBitGc& aGc, TPoint& aPos) const
	{
	TInt black = (100 - iContrast) * 127 / 200;
	TInt white = ((100 + iContrast) * 128 / 200) + 127;

	const TInt brightAdjust = iBrightness * 255 / 100;
	black += brightAdjust;
	white += brightAdjust;

	black = Min(Max(black,0),255);
	white = Min(Max(white,0),255);

	aGc.SetFadingParameters(TUint8(black),TUint8(white));
	aGc.Clear();
	aGc.DrawRect(TRect(aPos,TSize(KFrameFeatureSize,KFrameFeatureSize)));
	}
	
void CTestCamera::GetImgProcessedFrame(CFbsBitGc& aGc, TPoint& aPos) const
	{
	TInt contrast = iContrast;
	TInt brightness = iBrightness;
	
	if(iImgProcImpl != NULL)
		{
		for(TInt index=0; index<iImgProcImpl->iActiveTransformations.Count(); index++)
			{
			switch(iImgProcImpl->iActiveTransformations[index].iUid)
				{
				case KUidECamEventImageProcessingAdjustBrightnessUidValue:
					{
					brightness = iImgProcBrightness;
					break;
					}
				case KUidECamEventImageProcessingAdjustContrastUidValue:
					{
					contrast = iImgProcContrast;
					break;
					}
				default:
					{
					break;
					}
				}
			}
		}
	
	TInt black = (100 - contrast) * 127 / 200;
	TInt white = ((100 + contrast) * 128 / 200) + 127;

	const TInt brightAdjust = brightness * 255 / 100;
	black += brightAdjust;
	white += brightAdjust;

	black = Min(Max(black,0),255);
	white = Min(Max(white,0),255);

	aGc.SetFadingParameters(TUint8(black),TUint8(white));
	aGc.Clear();
	aGc.DrawRect(TRect(aPos,TSize(KFrameFeatureSize,KFrameFeatureSize)));
	}
	
void CTestCamera::ServiceVideo(TInt aBufferNum)
	{
	iFrameIndex++;
	iElapsedTime = iElapsedTime.Int64() + (TInt64(1000000) / TInt64(iVideoFrameRate));

	if (aBufferNum == 1)
		{
		if (iObserver2)
			{
			iFrameCameraBuffer1.iIndexOfFirstFrameInBuffer = iFrameIndex;
			iFrameCameraBuffer1.iElapsedTime = iElapsedTime;
			iFrameCameraBuffer1.iAvailable = EFalse;
			}
		else
			{
			iFrameBuffer1.iIndexOfFirstFrameInBuffer = iFrameIndex;
			iFrameBuffer1.iElapsedTime = iElapsedTime;
			iFrameBuffer1.iAvailable = EFalse;
			}
			
		if (iObserver2)
			{
			iObserver2->VideoBufferReady(iFrameCameraBuffer1,KErrNone);
			}
		else
			{
			iObserver->FrameBufferReady(&iFrameBuffer1,KErrNone);
			}
		}
	else if (aBufferNum == 2)
		{
		if (iObserver2)
			{
			iFrameCameraBuffer2.iIndexOfFirstFrameInBuffer = iFrameIndex;
			iFrameCameraBuffer2.iElapsedTime = iElapsedTime;
			iFrameCameraBuffer2.iAvailable = EFalse;
			}
		else
			{
			iFrameBuffer2.iIndexOfFirstFrameInBuffer = iFrameIndex;
			iFrameBuffer2.iElapsedTime = iElapsedTime;
			iFrameBuffer2.iAvailable = EFalse;
			}
			
		if (iObserver2)
			{
			iObserver2->VideoBufferReady(iFrameCameraBuffer2,KErrNone);
			}
		else
			{
			iObserver->FrameBufferReady(&iFrameBuffer2,KErrNone);
			}
		}
	}

void CTestCamera::ServiceViewFinder(CFbsBitmap* aSourceFrame)
	{
	if (iDSA)
		{
		iDSA->Gc()->DrawBitmap(iScreenRect,aSourceFrame);
		iDSA->ScreenDevice()->Update();
		}
	else if (iViewFinderBitmap)
		{
		if (iViewFinderBitmapGc)
			{
			if (!iClipRect.IsEmpty())
				{
				TReal32 xRatio = static_cast<TReal32>(aSourceFrame->SizeInPixels().iWidth) / 
								 static_cast<TReal32>(iClipRect.Size().iWidth);

				TReal32 yRatio = static_cast<TReal32>(aSourceFrame->SizeInPixels().iHeight) / 
								 static_cast<TReal32>(iClipRect.Size().iHeight);

				TRect sourceRect(TPoint(static_cast<TInt>(xRatio*iClipRect.iTl.iX),
										static_cast<TInt>(yRatio*iClipRect.iTl.iY)),
								 TPoint(static_cast<TInt>(xRatio*iClipRect.iBr.iX),
										static_cast<TInt>(yRatio*iClipRect.iBr.iY)));

				TRect destRect(TPoint(0,0), iClipRect.Size());

				iViewFinderBitmapGc->DrawBitmap(destRect,
												aSourceFrame,
												sourceRect);
				}
			else
				{
				TRect destRect(TPoint(0,0), iViewFinderBitmap->SizeInPixels());
				iViewFinderBitmapGc->DrawBitmap(destRect, aSourceFrame);
				}
				
			if (iObserver2)
				{
				iObserver2->ViewFinderReady(iFrameCameraBuffer1,KErrNone);
				}
			else
				{
				iObserver->ViewFinderFrameReady(*iViewFinderBitmap);
				}
		
			}
		}
	else
		{
		iScreenGc->DrawBitmap(iClipRect,aSourceFrame);
		iScreenDev->Update();
		}
	}

void CTestCamera::ServiceImageCapture(const CFbsBitGc& aSourceBitmapGc)
	{
	iImageCaptureActive = EFalse;
	iGc->BitBlt(TPoint(0,0), aSourceBitmapGc);
	CFbsBitmap* image = iImage;
	RTestCameraFrameBuffer frameBuffer(image);
	delete iGc;
	iGc = NULL;
	delete iDev;
	iDev = NULL;
	if (iObserver2 != NULL)
		{
		iObserver2->ImageBufferReady(frameBuffer,KErrNone);
		}
	else
		{
		iImage = NULL;
		iObserver->ImageReady(image,NULL,KErrNone);
		}
	}
	
void CTestCamera::ServiceBurstImageCapture(const CFbsBitGc& aSourceBitmapGc)
	{
	iGc->BitBlt(TPoint(0,0), aSourceBitmapGc);
	if (iObserver2 != NULL)
		{
		CFbsBitmap* image = iImage;
		RTestCameraFrameBuffer frameBuffer(image);
		iObserver2->ImageBufferReady(frameBuffer,KErrNone);
		}
	else//not for MCameraObserver
		{
		return;
		}
	
	if(iSnapshotImpl)
		{
		if(iSnapshotImpl->iSnapshotActive)
			{
			return;
			}
		}
	User::After(500000);
	iStillImagePos.iX += KFramePosIncrement;
	iStillImagePos.iY += KFramePosIncrement;

	if (iStillImagePos.iX >= iStillImageSize.iWidth)
		{
		iStillImagePos.iX = 0;
		}
			
	if (iStillImagePos.iY >= iStillImageSize.iHeight)
		{
		iStillImagePos.iY = 0;
		}
	}

void CTestCamera::CheckReserveAndPowerL()
	{
	if (!iReserved)
		{
		User::Leave(KErrInUse);
		}
		
	if (!iPowerOn)
		{
		User::Leave(KErrNotReady);
		}	
	}

TInt CTestCamera::CheckReserveAndPower() const
	{
	if (!iReserved)
		{
		return KErrInUse;
		}
		
	if (!iPowerOn)
		{
		return KErrNotReady;
		}	
	
	return KErrNone;
	}

TInt CTestCamera::ReserveCallBack(TAny* aThis)
	{
	const TECAMEvent event(KUidECamEventReserveComplete,KErrNone);
	CTestCamera* testCamera = static_cast<CTestCamera*>(aThis);
	if (testCamera->iObserver2)
		{
		testCamera->iObserver2->HandleEvent(event);
		}
	else
		{
		testCamera->iObserver->ReserveComplete(KErrNone);
		}
	return KErrNone;
	}

TInt CTestCamera::PowerOnCallBack(TAny* aThis)
	{
	TECAMEvent event(KUidECamEventPowerOnComplete,KErrNone);
	CTestCamera* testCamera = static_cast<CTestCamera*>(aThis);
	if (testCamera->iObserver2)
		{
		testCamera->iObserver2->HandleEvent(event);
		}
	else
		{
		testCamera->iObserver->PowerOnComplete(testCamera->iPowerOn ? KErrNone : KErrInUse);
		}
	return KErrNone;
	}

TInt CTestCamera::ImageCaptureCallBack(TAny* aThis)
	{
	CTestCamera* testCamera = static_cast<CTestCamera*>(aThis);
	if (testCamera->iObserver2)
		{
		testCamera->iObserver2->ImageBufferReady((static_cast<CTestCamera*>(aThis))->iFrameCameraBuffer1,testCamera->iStillCaptureErr);
		}
	else
		{
		testCamera->iObserver->ImageReady(NULL,NULL,testCamera->iStillCaptureErr);
		}
	testCamera->iStillCaptureErr = KErrNone;
	return KErrNone;
	}

TInt CTestCamera::VideoCaptureCallBack(TAny* aThis)
	{
	CTestCamera* testCamera = static_cast<CTestCamera*>(aThis);
	if (testCamera->iObserver2)
		{
		testCamera->iObserver2->VideoBufferReady((static_cast<CTestCamera*>(aThis))->iFrameCameraBuffer1,testCamera->iVideoCaptureErr);
		}
	else
		{
		testCamera->iObserver->FrameBufferReady(NULL,testCamera->iVideoCaptureErr);
		}
	testCamera->iVideoCaptureErr = KErrNone;
	return KErrNone;
	}
	
TInt CTestCamera::HandleEventCallBack(TAny* aThis)
	{
	CTestCamera* testCamera = static_cast<CTestCamera*>(aThis);
	if (testCamera->iObserver2)
		{
		testCamera->iObserver2->HandleEvent(testCamera->iECamEvent);
		}
	
	return KErrNone;
	}

TInt CTestCamera::HandleEvent2CallBack(TAny* aThis)
	{
	CTestCamera* testCamera = static_cast<CTestCamera*>(aThis);
	if (testCamera->iObserver2)
		{
		testCamera->iObserver2->HandleEvent(testCamera->iECamEvent2);
		}
	
	return KErrNone;
	}

//
// CTestCameraInfo
//

CTestCameraInfo::CTestCameraInfo()
	{
	}
	
CTestCameraInfo::~CTestCameraInfo()
	{
	}
	
CTestCameraInfo* CTestCameraInfo::NewL()
	{
	FileDependencyUtil::CheckFileDependencyL();
	return new (ELeave) CTestCameraInfo;
	}
	
TInt CTestCameraInfo::CamerasAvailable()
	{
	return 1;
	}

//void CTestCamera::CheckFileDependencyL()
void FileDependencyUtil::CheckFileDependencyL()
	{	
	RFs fsSession;
  	RFile file;
    CleanupClosePushL(fsSession);
  	User::LeaveIfError(fsSession.Connect());
    TInt err = file.Open(fsSession, KTestCameraPluginName, EFileRead);
  	file.Close();
  	if(err != KErrNone)
  		{
	    User::LeaveIfError(KErrNotSupported);
  		}
  	CleanupStack::PopAndDestroy(); //fsSession 
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KUidOnboardCameraTestPlugin,	CTestCamera::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidOnboardCameraTestInfo,	CTestCameraInfo::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

