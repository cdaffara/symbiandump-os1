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

#ifndef ECAMSTUBPLUGIN_H
#define ECAMSTUBPLUGIN_H

#include <ecam/ecamplugin.h>
#include <ecam/ecaminfoplugin.h>

// Implementations of the ECam plugin classes

class CCameraStub : public CCameraPlugin
	{
public:
	static CCameraStub* NewL();
	~CCameraStub();
	
protected:
	// from CCameraPlugin
	void Construct2L(MCameraObserver& aObserver,TInt aCameraIndex);
	void Construct2DupL(MCameraObserver& aObserver,TInt aCameraHandle);
	
	void Construct2L(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority);
	void Construct2DupL(MCameraObserver2& aObserver,TInt aCameraHandle);

	// from CCamera itself
	void CameraInfo(TCameraInfo& aInfo) const;
	void Reserve();
	void Release();
	void PowerOn();
	void PowerOff();
	TInt Handle();
	void SetZoomFactorL(TInt aZoomFactor = 0);
	TInt ZoomFactor() const;
	void SetDigitalZoomFactorL(TInt aDigitalZoomFactor = 0);
	TInt DigitalZoomFactor() const;
	void SetContrastL(TInt aContrast);
	TInt Contrast() const;
	void SetBrightnessL(TInt aBrightness);
	TInt Brightness() const;
	void SetFlashL(TFlash aFlash = EFlashNone);
	TFlash Flash() const;
	void SetExposureL(TExposure aExposure = EExposureAuto);
	TExposure Exposure() const;
	void SetWhiteBalanceL(TWhiteBalance aWhiteBalance = EWBAuto);
	TWhiteBalance WhiteBalance() const;
	void StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect);
	void StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect,TRect& aClipRect);
	void StartViewFinderBitmapsL(TSize& aSize);	
	void StartViewFinderBitmapsL(TSize& aSize,TRect& aClipRect);
	void StartViewFinderL(TFormat aImageFormat,TSize& aSize);
	void StartViewFinderL(TFormat aImageFormat,TSize& aSize,TRect& aClipRect);
	void StopViewFinder();
	TBool ViewFinderActive() const;
	void SetViewFinderMirrorL(TBool aMirror);
	TBool ViewFinderMirror() const;
	void PrepareImageCaptureL(TFormat aImageFormat,TInt aSizeIndex);
	void PrepareImageCaptureL(TFormat aImageFormat,TInt aSizeIndex,const TRect& aClipRect);
	void CaptureImage();
	void CancelCaptureImage();
	void EnumerateCaptureSizes(TSize& aSize,TInt aSizeIndex,TFormat aFormat) const;
	void PrepareVideoCaptureL(TFormat aFormat,TInt aSizeIndex,TInt aRateIndex,TInt aBuffersToUse,TInt aFramesPerBuffer);
	void PrepareVideoCaptureL(TFormat aFormat,TInt aSizeIndex,TInt aRateIndex,TInt aBuffersToUse,TInt aFramesPerBuffer,const TRect& aClipRect);
	void StartVideoCapture();
	void StopVideoCapture();
	TBool VideoCaptureActive() const;
	void EnumerateVideoFrameSizes(TSize& aSize,TInt aSizeIndex,TFormat aFormat) const;
	void EnumerateVideoFrameRates(TReal32& aRate,TInt aRateIndex,TFormat aFormat,TInt aSizeIndex,TExposure aExposure = EExposureAuto) const;
	void GetFrameSize(TSize& aSize) const;
	TReal32 FrameRate() const;
	TInt BuffersInUse() const;
	TInt FramesPerBuffer() const;
	void SetJpegQuality(TInt aQuality);
	TInt JpegQuality() const;
	TAny* CustomInterface(TUid aInterface);

private:
	CCameraStub();
private:
	MCameraObserver* iObserver; // not owned
	MCameraObserver2* iObserver2; // not owned
	TInt iCameraIndex;
	TInt iCameraHandle;
	TInt iPriority;
	};
	
class CCameraStubInfo : public CCameraInfoPlugin
	{
public:
	static CCameraStubInfo* NewL();
	
	~CCameraStubInfo();
	
	// from CCameraInfoPlugin
	TInt CamerasAvailable();
private:
	CCameraStubInfo();	
	};
	
enum TStubPanicReasons
	{
	EPanicUnimplemented=1
	};
	
#endif // ECAMSTUBPLUGIN_H
