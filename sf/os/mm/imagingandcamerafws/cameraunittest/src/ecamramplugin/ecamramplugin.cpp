// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <ecam.h>
#include "ecamramplugin.h"
#include "ecamrampluginuids.hrh"

	
//
// CCameraRamPlugin
//
CCameraRamPlugin::CCameraRamPlugin()
	{
	}
	
CCameraRamPlugin::~CCameraRamPlugin()
	{
	}
	
CCameraRamPlugin* CCameraRamPlugin::NewL()
	{
	FileDependencyUtil::CheckFileDependencyL();
	return new (ELeave) CCameraRamPlugin;
	}
	
// 
// 2nd stage ConstructL calls called by framework
//

void CCameraRamPlugin::Construct2L(MCameraObserver& aObserver,TInt aCameraIndex)
	{
	iObserver = &aObserver;
	iCameraIndex = aCameraIndex;
	}
	
void CCameraRamPlugin::Construct2DupL(MCameraObserver& aObserver,TInt aCameraHandle)
	{
	iObserver = &aObserver;
	iCameraHandle = aCameraHandle;
	}


void CCameraRamPlugin::Construct2L(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority)
	{
	iObserver2 = &aObserver;
	iCameraIndex = aCameraIndex;
	iPriority = aPriority;
	}
	
void CCameraRamPlugin::Construct2DupL(MCameraObserver2& aObserver,TInt aCameraHandle)
	{
	iObserver2 = &aObserver;
	iCameraHandle = aCameraHandle;
	}


	
//
// Dummy versions of virtual functions
//

void CCameraRamPlugin::CameraInfo(TCameraInfo& /*aInfo*/) const
	{
	}

void CCameraRamPlugin::Reserve()
	{
	}


void CCameraRamPlugin::Release()
	{
	}

void CCameraRamPlugin::PowerOn()
	{
	}

void CCameraRamPlugin::PowerOff()
	{
	}

TInt CCameraRamPlugin::Handle()
	{
	return 0;
	}

void CCameraRamPlugin::SetZoomFactorL(TInt /*aZoomFactor = 0*/)
	{
	User::Leave(KErrNotSupported);
	}

TInt CCameraRamPlugin::ZoomFactor() const
	{
	return 0;
	}

void CCameraRamPlugin::SetDigitalZoomFactorL(TInt /*aDigitalZoomFactor = 0*/)
	{
	User::Leave(KErrNotSupported);
	}

TInt CCameraRamPlugin::DigitalZoomFactor() const
	{
	return 0;
	}

void CCameraRamPlugin::SetContrastL(TInt /*aContrast*/)
	{
	User::Leave(KErrNotSupported);
	}

TInt CCameraRamPlugin::Contrast() const
	{
	return 0;
	}

void CCameraRamPlugin::SetBrightnessL(TInt /*aBrightness*/)
	{
	User::Leave(KErrNotSupported);
	}

TInt CCameraRamPlugin::Brightness() const
	{
	return 0;
	}


void CCameraRamPlugin::SetFlashL(TFlash /*aFlash = EFlashNone*/)
	{
	User::Leave(KErrNotSupported);
	}

CCamera::TFlash CCameraRamPlugin::Flash() const
	{
	return EFlashNone;
	}

void CCameraRamPlugin::SetExposureL(TExposure /*aExposure = EExposureAuto*/)
	{
	User::Leave(KErrNotSupported);
	}

CCamera::TExposure CCameraRamPlugin::Exposure() const
	{
	return EExposureAuto;
	}

void CCameraRamPlugin::SetWhiteBalanceL(TWhiteBalance /*aWhiteBalance = EWBAuto*/)
	{
	}

CCamera::TWhiteBalance CCameraRamPlugin::WhiteBalance() const
	{
	return EWBAuto;
	}

void CCameraRamPlugin::StartViewFinderDirectL(RWsSession& /*aWs*/,CWsScreenDevice& /*aScreenDevice*/,RWindowBase& /*aWindow*/,TRect& /*aScreenRect*/)
	{
	User::Leave(KErrNotSupported);
	}

void CCameraRamPlugin::StartViewFinderDirectL(RWsSession& /*aWs*/,CWsScreenDevice& /*aScreenDevice*/,RWindowBase& /*aWindow*/,TRect& /*aScreenRect*/,TRect& /*aClipRect*/)
	{
	User::Leave(KErrNotSupported);
	}

void CCameraRamPlugin::StartViewFinderBitmapsL(TSize& /*aSize*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CCameraRamPlugin::StartViewFinderBitmapsL(TSize& /*aSize*/,TRect& /*aClipRect*/)
	{
	User::Leave(KErrNotSupported);
	}


void CCameraRamPlugin::StartViewFinderL(TFormat /*aImageFormat*/,TSize& /*aSize*/)
	{
	User::Leave(KErrNotSupported);
	}

void CCameraRamPlugin::StartViewFinderL(TFormat /*aImageFormat*/,TSize& /*aSize*/,TRect& /*aClipRect*/)
	{
	User::Leave(KErrNotSupported);
	}


void CCameraRamPlugin::StopViewFinder()
	{
	}

TBool CCameraRamPlugin::ViewFinderActive() const
	{
	return EFalse;
	}

void CCameraRamPlugin::SetViewFinderMirrorL(TBool /*aMirror*/)
	{
	}

TBool CCameraRamPlugin::ViewFinderMirror() const
	{
	return EFalse;
	}

void CCameraRamPlugin::PrepareImageCaptureL(TFormat /*aImageFormat*/,TInt /*aSizeIndex*/)
	{
	User::Leave(KErrNotSupported);
	}

void CCameraRamPlugin::PrepareImageCaptureL(TFormat /*aImageFormat*/,TInt /*aSizeIndex*/,const TRect& /*aClipRect*/)
	{
	User::Leave(KErrNotSupported);
	}

void CCameraRamPlugin::CaptureImage()
	{
	}

void CCameraRamPlugin::CancelCaptureImage()
	{
	}

void CCameraRamPlugin::EnumerateCaptureSizes(TSize& /*aSize*/,TInt /*aSizeIndex*/,TFormat /*aFormat*/) const
	{
	}

void CCameraRamPlugin::PrepareVideoCaptureL(TFormat /*aFormat*/,TInt /*aSizeIndex*/,TInt /*aRateIndex*/,TInt /*aBuffersToUse*/,TInt /*aFramesPerBuffer*/)
	{
	User::Leave(KErrNotSupported);
	}

void CCameraRamPlugin::PrepareVideoCaptureL(TFormat /*aFormat*/,TInt /*aSizeIndex*/,TInt /*aRateIndex*/,TInt /*aBuffersToUse*/,TInt /*aFramesPerBuffer*/,const TRect& /*aClipRect*/)
	{
	User::Leave(KErrNotSupported);
	}

void CCameraRamPlugin::StartVideoCapture()
	{
	}

void CCameraRamPlugin::StopVideoCapture()
	{
	}

TBool CCameraRamPlugin::VideoCaptureActive() const
	{
	return EFalse;
	}

void CCameraRamPlugin::EnumerateVideoFrameSizes(TSize& /*aSize*/,TInt /*aSizeIndex*/,TFormat /*aFormat*/) const
	{
	}

void CCameraRamPlugin::EnumerateVideoFrameRates(TReal32& /*aRate*/,TInt /*aRateIndex*/,TFormat /*aFormat*/,TInt /*aSizeIndex*/,TExposure /*aExposure = EExposureAuto*/) const
	{
	}

void CCameraRamPlugin::GetFrameSize(TSize& /*aSize*/) const
	{
	}

TReal32 CCameraRamPlugin::FrameRate() const
	{
	return 0.0;
	}

TInt CCameraRamPlugin::BuffersInUse() const
	{
	return 0;
	}

TInt CCameraRamPlugin::FramesPerBuffer() const
	{
	return 0;
	}

void CCameraRamPlugin::SetJpegQuality(TInt /*aQuality*/)
	{
	}

TInt CCameraRamPlugin::JpegQuality() const
	{
	return 0;
	}

TAny* CCameraRamPlugin::CustomInterface(TUid /*aInterfaceUid*/)
	{
	return NULL;
	}
	
//
// CCameraRamPluginInfo
//

CCameraRamPluginInfo::CCameraRamPluginInfo()
	{
	}
	
CCameraRamPluginInfo::~CCameraRamPluginInfo()
	{
	}
	
CCameraRamPluginInfo* CCameraRamPluginInfo::NewL()
	{
	FileDependencyUtil::CheckFileDependencyL();
	return new (ELeave) CCameraRamPluginInfo;
	}
	
TInt CCameraRamPluginInfo::CamerasAvailable()
	{
	return 1;
	}

void FileDependencyUtil::CheckFileDependencyL()
	{
	RFs fsSession;
  	RFile file;
    
  	User::LeaveIfError(fsSession.Connect());
  	CleanupClosePushL(fsSession);
    TInt err = file.Open(fsSession, KECamRamPluginName, EFileRead);
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
	IMPLEMENTATION_PROXY_ENTRY(KUidCameraRamTestPlugin, CCameraRamPlugin::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidCameraRamTestInfo, CCameraRamPluginInfo::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

