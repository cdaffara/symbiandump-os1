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

#include "ECamStubPlugin.h"

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <ecamuids.hrh>

//
// Main stuff
//

static void Panic(TInt aReason)
	{
	_LIT(KDllName, "ECam Stub Plugin");
	User::Panic(KDllName, aReason);
	}

//
// CCameraStub
//

CCameraStub::CCameraStub()
	{
	}
	
CCameraStub::~CCameraStub()
	{
	}
	
CCameraStub* CCameraStub::NewL()
	{
	return new (ELeave) CCameraStub;
	}
	
// 
// 2nd stage ConstructL calls called by framework
//

void CCameraStub::Construct2L(MCameraObserver& aObserver,TInt aCameraIndex)
	{
	iObserver = &aObserver;
	iCameraIndex = aCameraIndex;
	User::Leave(KErrNotSupported);
	}
	
void CCameraStub::Construct2DupL(MCameraObserver& aObserver,TInt aCameraHandle)
	{
	iObserver = &aObserver;
	iCameraHandle = aCameraHandle;
	User::Leave(KErrNotSupported);
	}


void CCameraStub::Construct2L(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority)
	{
	iObserver2 = &aObserver;
	iCameraIndex = aCameraIndex;
	iPriority = aPriority;
	User::Leave(KErrNotSupported);
	}
	
void CCameraStub::Construct2DupL(MCameraObserver2& aObserver,TInt aCameraHandle)
	{
	iObserver2 = &aObserver;
	iCameraHandle = aCameraHandle;
	User::Leave(KErrNotSupported);
	}


	
//
// Dummy versions of virtual functions
//

void CCameraStub::CameraInfo(TCameraInfo& /*aInfo*/) const
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::Reserve()
	{
	Panic(EPanicUnimplemented);
	}


void CCameraStub::Release()
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::PowerOn()
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::PowerOff()
	{
	Panic(EPanicUnimplemented);
	}

TInt CCameraStub::Handle()
	{
	Panic(EPanicUnimplemented);
	return 0;
	}

void CCameraStub::SetZoomFactorL(TInt /*aZoomFactor = 0*/)
	{
	Panic(EPanicUnimplemented);
	}

TInt CCameraStub::ZoomFactor() const
	{
	Panic(EPanicUnimplemented);
	return 0;
	}

void CCameraStub::SetDigitalZoomFactorL(TInt /*aDigitalZoomFactor = 0*/)
	{
	Panic(EPanicUnimplemented);
	}

TInt CCameraStub::DigitalZoomFactor() const
	{
	Panic(EPanicUnimplemented);
	return 0;
	}

void CCameraStub::SetContrastL(TInt /*aContrast*/)
	{
	Panic(EPanicUnimplemented);
	}

TInt CCameraStub::Contrast() const
	{
	Panic(EPanicUnimplemented);
	return 0;
	}

void CCameraStub::SetBrightnessL(TInt /*aBrightness*/)
	{
	Panic(EPanicUnimplemented);
	}

TInt CCameraStub::Brightness() const
	{
	Panic(EPanicUnimplemented);
	return 0;
	}


void CCameraStub::SetFlashL(TFlash /*aFlash = EFlashNone*/)
	{
	Panic(EPanicUnimplemented);
	}

CCamera::TFlash CCameraStub::Flash() const
	{
	Panic(EPanicUnimplemented);
	return EFlashNone;
	}

void CCameraStub::SetExposureL(TExposure /*aExposure = EExposureAuto*/)
	{
	Panic(EPanicUnimplemented);
	}

CCamera::TExposure CCameraStub::Exposure() const
	{
	Panic(EPanicUnimplemented);
	return EExposureAuto;
	}

void CCameraStub::SetWhiteBalanceL(TWhiteBalance /*aWhiteBalance = EWBAuto*/)
	{
	Panic(EPanicUnimplemented);
	}


CCamera::TWhiteBalance CCameraStub::WhiteBalance() const
	{
	Panic(EPanicUnimplemented);
	return EWBAuto;
	}

void CCameraStub::StartViewFinderDirectL(RWsSession& /*aWs*/,CWsScreenDevice& /*aScreenDevice*/,RWindowBase& /*aWindow*/,TRect& /*aScreenRect*/)
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::StartViewFinderDirectL(RWsSession& /*aWs*/,CWsScreenDevice& /*aScreenDevice*/,RWindowBase& /*aWindow*/,TRect& /*aScreenRect*/,TRect& /*aClipRect*/)
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::StartViewFinderBitmapsL(TSize& /*aSize*/)
	{
	Panic(EPanicUnimplemented);
	}
	
void CCameraStub::StartViewFinderBitmapsL(TSize& /*aSize*/,TRect& /*aClipRect*/)
	{
	Panic(EPanicUnimplemented);
	}


void CCameraStub::StartViewFinderL(TFormat /*aImageFormat*/,TSize& /*aSize*/)
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::StartViewFinderL(TFormat /*aImageFormat*/,TSize& /*aSize*/,TRect& /*aClipRect*/)
	{
	Panic(EPanicUnimplemented);
	}


void CCameraStub::StopViewFinder()
	{
	Panic(EPanicUnimplemented);
	}

TBool CCameraStub::ViewFinderActive() const
	{
	Panic(EPanicUnimplemented);
	return EFalse;
	}

void CCameraStub::SetViewFinderMirrorL(TBool /*aMirror*/)
	{
	Panic(EPanicUnimplemented);
	}

TBool CCameraStub::ViewFinderMirror() const
	{
	Panic(EPanicUnimplemented);
	return EFalse;
	}

void CCameraStub::PrepareImageCaptureL(TFormat /*aImageFormat*/,TInt /*aSizeIndex*/)
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::PrepareImageCaptureL(TFormat /*aImageFormat*/,TInt /*aSizeIndex*/,const TRect& /*aClipRect*/)
	{
	Panic(EPanicUnimplemented);
	}


void CCameraStub::CaptureImage()
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::CancelCaptureImage()
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::EnumerateCaptureSizes(TSize& /*aSize*/,TInt /*aSizeIndex*/,TFormat /*aFormat*/) const
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::PrepareVideoCaptureL(TFormat /*aFormat*/,TInt /*aSizeIndex*/,TInt /*aRateIndex*/,TInt /*aBuffersToUse*/,TInt /*aFramesPerBuffer*/)
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::PrepareVideoCaptureL(TFormat /*aFormat*/,TInt /*aSizeIndex*/,TInt /*aRateIndex*/,TInt /*aBuffersToUse*/,TInt /*aFramesPerBuffer*/,const TRect& /*aClipRect*/)
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::StartVideoCapture()
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::StopVideoCapture()
	{
	Panic(EPanicUnimplemented);
	}

TBool CCameraStub::VideoCaptureActive() const
	{
	Panic(EPanicUnimplemented);
	return EFalse;
	}

void CCameraStub::EnumerateVideoFrameSizes(TSize& /*aSize*/,TInt /*aSizeIndex*/,TFormat /*aFormat*/) const
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::EnumerateVideoFrameRates(TReal32& /*aRate*/,TInt /*aRateIndex*/,TFormat /*aFormat*/,TInt /*aSizeIndex*/,TExposure /*aExposure = EExposureAuto*/) const
	{
	Panic(EPanicUnimplemented);
	}

void CCameraStub::GetFrameSize(TSize& /*aSize*/) const
	{
	Panic(EPanicUnimplemented);
	}

TReal32 CCameraStub::FrameRate() const
	{
	Panic(EPanicUnimplemented);
	return 0.0;
	}

TInt CCameraStub::BuffersInUse() const
	{
	Panic(EPanicUnimplemented);
	return 0;
	}

TInt CCameraStub::FramesPerBuffer() const
	{
	Panic(EPanicUnimplemented);
	return 0;
	}

void CCameraStub::SetJpegQuality(TInt /*aQuality*/)
	{
	Panic(EPanicUnimplemented);
	}

TInt CCameraStub::JpegQuality() const
	{
	Panic(EPanicUnimplemented);
	return 0;
	}

TAny* CCameraStub::CustomInterface(TUid /*aInterface*/)
	{
	Panic(EPanicUnimplemented);
	return NULL;
	}


//
// CCameraStubInfo
//

CCameraStubInfo::CCameraStubInfo()
	{
	}
	
CCameraStubInfo::~CCameraStubInfo()
	{
	}
	
CCameraStubInfo* CCameraStubInfo::NewL()
	{
	return new (ELeave) CCameraStubInfo;
	}
	
TInt CCameraStubInfo::CamerasAvailable()
	{
	return 0;
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KUidOnboardCameraStubPlugin,	CCameraStub::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KUidOnboardCameraStubInfo,	CCameraStubInfo::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

