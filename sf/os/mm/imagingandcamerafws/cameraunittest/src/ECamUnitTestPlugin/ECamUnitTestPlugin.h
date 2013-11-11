
// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ECAMUNITTESTPlUGIN_H
#define ECAMUNITTESTPlUGIN_H

#include <ecam/ecamplugin.h>
#include <ecam/ecaminfoplugin.h>
#include <ecamadvsettings.h>
#include <f32file.h> 

_LIT(KECamUnitTestPluginName, "C:\\102070CF.txt");
_LIT(KTestCameraPluginName, "C:\102070cc.txt");

#ifdef SYMBIAN_MULTIMEDIA_THREEPLANEARCH
_LIT(KMMCameraPluginName, "C:\\mmcameraclientplugin.txt");
#endif //SYMBIAN_MULTIMEDIA_THREEPLANEARCH

const TInt KECamSetAvailableCameras = 3; 

// Implementations of the ECam plugin classes
class CCamUnitTestPlugin : 	public CCameraPlugin
	{
	friend class CCamPresets;
	friend class CCamAdvSet;
	friend class CCamImgProc;
	friend class CCamManagement;
	
public:
	static CCamUnitTestPlugin* NewL();
	~CCamUnitTestPlugin();
	
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
		
public:
    void Notify(TUid aMessageId, TInt aError);
    void Notify2(TUid aMessageId, TInt aError, TInt aParam);
    
    void GenerateVFHandle(TInt& aVFHandle);
    // set presets
    //void InitPresetsL(RArray<TUid>& aPresets);
private:
	CCamUnitTestPlugin();
	
private:
	MCameraObserver* iObserver; // not owned
	MCameraObserver2* iObserver2; // not owned

	TInt iCameraIndex;
	TInt iCameraHandle;
	TInt iPriority;
	
	RArray<TInt> iSupportedISORates;
	TInt iIsoRate;
	TInt iVFHandleGenerater;
	
public:	
	// additional info
	RArray<TUid> iPresets;
	TBool iCameras[KECamSetAvailableCameras];
	CCamera::CCameraAdvancedSettings::TCameraType iCameraTypes[KECamSetAvailableCameras];
	};
	
class CCamUnitTestPluginInfo : public CCameraInfoPlugin
	{
public:
	static CCamUnitTestPluginInfo* NewL();
	
	~CCamUnitTestPluginInfo();
	
	// from CCameraInfoPlugin
	TInt CamerasAvailable();
private:
	CCamUnitTestPluginInfo();	
	};
	
class CCamUnitTestPluginSecureId : public MSecureIdPlugin
	{
public:
	static CCamUnitTestPluginSecureId* NewL();
	
	~CCamUnitTestPluginSecureId();
	
	// from CCameraInfoPlugin
	void Release();
	void GetSecureIdL(TInt& aSecureId) const;
	
private:
	CCamUnitTestPluginSecureId();	
	};
	
enum TSetPanicReasons
	{
	EPanicUnimplemented=1
	};
	
class FileDependencyUtil
	{
public:
	static void CheckFileDependencyL();
	};	

#endif // ECAMUNITTESTPlUGIN_H
