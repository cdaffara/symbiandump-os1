// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
*/

#ifndef MMCAMERACLIENTSESSION_H
#define MMCAMERACLIENTSESSION_H


#include "mmcameraclientinterface.h"

#include <ecam/ecamplugin.h>
#include <ecam/ecaminfoplugin.h>
#include <ecam/ecamviewfinderintf.h>
#include <e32hashtab.h>


class CMMDirectViewFinder; // Friend class implementing functionality for CCameraV2DirectViewFinder
class CMMCameraAccessHandler; // Response class used to handle asynchronous reserve of camera device
class CMMCameraPowerHandler; // Response class used to handle asynchronous powering of camera device

_LIT(KMMCameraPluginName, "C:\\mmcameraclientplugin.txt");


/**
* Reference implementation of CCamera.
* 
*/
class CMMCameraClientSession : public CCameraPlugin
	{
friend class CMMCameraAccessHandler;
friend class CMMCameraPowerHandler;
friend class CMMDirectViewFinder;

public:
	static CMMCameraClientSession* NewL();
	~CMMCameraClientSession();

	// from CCameraPlugin
	void Construct2L(MCameraObserver& aObserver, TInt aCameraIndex);
	void Construct2DupL(MCameraObserver& aObserver, TInt aCameraHandle);
	
	void Construct2L(MCameraObserver2& aObserver, TInt aCameraIndex, TInt aPriority);
	void Construct2DupL(MCameraObserver2& aObserver, TInt aCameraHandle);

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
	void StartViewFinderDirectL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindowBase& aWindow, TRect& aScreenRect);
	void StartViewFinderDirectL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindowBase& aWindow, TRect& aScreenRect, TRect& aClipRect);
	void StartViewFinderBitmapsL(TSize& aSize);	
	void StartViewFinderBitmapsL(TSize& aSize, TRect& aClipRect);

	void StartViewFinderL(TFormat aImageFormat, TSize& aSize);
	void StartViewFinderL(TFormat aImageFormat, TSize& aSize, TRect& aClipRect);

	void StopViewFinder();
	TBool ViewFinderActive() const;
	void SetViewFinderMirrorL(TBool aMirror);
	TBool ViewFinderMirror() const;
	void PrepareImageCaptureL(TFormat aImageFormat, TInt aSizeIndex);
	void PrepareImageCaptureL(TFormat aImageFormat, TInt aSizeIndex, const TRect& aClipRect);
	void CaptureImage();
	void CancelCaptureImage();
	void EnumerateCaptureSizes(TSize& aSize, TInt aSizeIndex, TFormat aFormat) const;
	void PrepareVideoCaptureL(TFormat aFormat, TInt aSizeIndex, TInt aRateIndex, TInt aBuffersToUse, TInt aFramesPerBuffer);
	void PrepareVideoCaptureL(TFormat aFormat, TInt aSizeIndex, TInt aRateIndex, TInt aBuffersToUse, TInt aFramesPerBuffer, const TRect& aClipRect);
	void StartVideoCapture();
	void StopVideoCapture();
	TBool VideoCaptureActive() const;
	void EnumerateVideoFrameSizes(TSize& aSize, TInt aSizeIndex, TFormat aFormat) const;
	void EnumerateVideoFrameRates(TReal32& aRate, TInt aRateIndex, TFormat aFormat, TInt aSizeIndex, TExposure aExposure = EExposureAuto) const;
	void GetFrameSize(TSize& aSize) const;
	TReal32 FrameRate() const;
	TInt BuffersInUse() const;
	TInt FramesPerBuffer() const;
	void SetJpegQuality(TInt aQuality);
	TInt JpegQuality() const;
	TAny* CustomInterface(TUid aInterface);

private:
	CMMCameraClientSession();

private:
	MCameraObserver2* 			iCameraObserver2; // not owned
	CMMDirectViewFinder*	 	iDirectViewFinder;
	MCameraViewFinder*			iVF;

	CMMCameraAccessHandler* 	iCameraAccessHandler; // This response class gets created by the plugin class as part of its construction process
	CMMCameraPowerHandler* 		iCameraPowerHandler; // This response class gets created by the plugin class as part of its construction process
	RMMCameraSession 			iCameraSession; // handle to client side session to perform synchronous request of CCamera

	RHashMap<TInt, CCameraV2DirectViewFinder*>					iHashMap;

	// ECam States
	TBool iReserved;
	TBool iPoweredUp;
	TBool iPrepareCompleted;

	TInt iCameraIndex;
	TInt iCameraHandle;
	TInt iPriority;
	TBool iCollaborativeClient;
	};
	
class CMMCameraClientSessionInfo : public CCameraInfoPlugin
	{
public:
	static CMMCameraClientSessionInfo* NewL();  // may need to connect and create session
	
	~CMMCameraClientSessionInfo();
	
	// from CCameraInfoPlugin
	TInt CamerasAvailable();
	
private:
	CMMCameraClientSessionInfo();
	};

class FileDependencyUtil
	{
public:
	static void CheckFileDependencyL();
	};


class CMMCameraAccessHandler : public CActive
    {
public: 
    static CMMCameraAccessHandler* NewL(CMMCameraClientSession* aPlugin, TInt aPriority);
    
    ~CMMCameraAccessHandler();
    
    void Reserve();
    /*void PowerOn();
    void CaptureImage(TInt aNumberOfImages); // aNumberOfImages is 0 for Image Capture and >1 for burst image( future case) 
    void StartVideoCapture();*/
    
protected:
    void RunL();  // void GrabImageData() and GrabVideoData() should be in RRefCameraSession 
    void DoCancel();

private:
	CMMCameraAccessHandler(CMMCameraClientSession* aPlugin, TInt aPriority);

private:
    CMMCameraClientSession* iPlugin;
    
    /*TInt iReqStillImages;
    TInt iCapturedStillImages;
    TInt iVideoFrames;  // initialized as 0 and gets incremented every time new video frame is received 
    */};
    

class CMMCameraPowerHandler : public CActive
    {
public: 
    static CMMCameraPowerHandler* NewL(CMMCameraClientSession* aPlugin, TInt aPriority);
    
    ~CMMCameraPowerHandler();
    
    void PowerOn();
        
protected:
    void RunL();  
    void DoCancel();

private:
	CMMCameraPowerHandler(CMMCameraClientSession* aPlugin, TInt aPriority);

private:
    CMMCameraClientSession* iPlugin;
    };

#endif // MMCAMERACLIENTSESSION_H

