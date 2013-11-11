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
 @internalComponent
*/

#include "mmcameraserver.h"
#include "mmcameraserversession.h"
#include "mmcameraservercontroller.h"

#include <graphics/surfacemanager.h>

using namespace MMClient;
using namespace MMVideoComponents;
using namespace MMCameraComponents;

static const TInt KMMCamHandleNumberBase = 500;
const TInt KDefaultFrameRate = 30;

CMMCameraServerController* CMMCameraServerController::NewL(TInt aCameraIndex)
	{
	CMMCameraServerController* self = new(ELeave) CMMCameraServerController(aCameraIndex);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CMMCameraServerController::CMMCameraServerController(TInt aCameraIndex): iCameraIndex(aCameraIndex),
																	 iCameraHandle(KMMCamHandleNumberBase + iCameraIndex),
																	 iMMCapability(EFalse),
																	 iViewFinderState(CCamera::CCameraV2DirectViewFinder::EViewFinderInActive),
																	 iFlash(CCamera::EFlashNone),
																	 iExposure(CCamera::EExposureAuto),
																	 iWhiteBalance(CCamera::EWBAuto)
	{
	}

void CMMCameraServerController::ConstructL()
	{	
	User::LeaveIfError(iMMServer.Connect()); // Connect to the MM server

	// Create characterizations for camera and graphics sink
	XMMCameraViewFinderCharacterization* cameraViewFinderChar = XMMCameraViewFinderCharacterization::New();
	XMMVideoGraphicsSinkCharacterization* graphicsSinkChar = XMMVideoGraphicsSinkCharacterization::New();
	XMMCameraSensorCharacterization* cameraSensorChar = XMMCameraSensorCharacterization::New();
//	XMMCameraCaptureCharacterization* cameraCaptureChar = XMMCameraCaptureCharacterization::New();

	// Open stream, stream container and camera/graphics sink components
	User::LeaveIfError(iMMStreamContainer.Open());
	User::LeaveIfError(iMMStream.Open(iMMStreamContainer));

	User::LeaveIfError(iCameraViewFinder.Open(*cameraViewFinderChar));
	User::LeaveIfError(iMMStream.Append(iCameraViewFinder));

	User::LeaveIfError(iGraphicsSink.Open(*graphicsSinkChar));
	User::LeaveIfError(iMMStream.Append(iGraphicsSink));

	User::LeaveIfError(iCameraSensor.Open(*cameraSensorChar));
	User::LeaveIfError(iMMStream.Append(iCameraSensor));
	}

CMMCameraServerController::~CMMCameraServerController()
	{
	iCameraCapture.Close();
	iCameraSensor.Close();
	iGraphicsSink.Close();
	iCameraViewFinder.Close();
	iMMStream.Close();
	iMMStreamContainer.Close();
	iMMServer.Close();

	iCamCntrlLink.Deque();
	iCamCntrlLink.iNext = NULL;
	}

TInt CMMCameraServerController::CameraIndex() const
	{
	return iCameraIndex;
	}

TInt CMMCameraServerController::CameraHandle() const
	{
	return iCameraHandle;
	}

void CMMCameraServerController::PrepareDirectViewFinderL(TDirectViewFinderInfo& aDirectViewFinderInfo)
	{
	// Default mode - One Shot setting off
	User::LeaveIfError(iCameraSensor.SetSensorOneShot(EFalse));

	User::LeaveIfError(iCameraViewFinder.SetFrameDimensions(aDirectViewFinderInfo.iScreenRect.Width(), aDirectViewFinderInfo.iScreenRect.Height()));
	User::LeaveIfError(iCameraViewFinder.SetColorFormat(MMCameraComponents::EColorFormatYUV422Planar));
	// TODO: Replace 2 with calculation to find bytes per pixel depending on color format
	User::LeaveIfError(iCameraViewFinder.SetStride(aDirectViewFinderInfo.iScreenRect.Width() * 2));
	// Default mode - Frame rate = 30 fps
	User::LeaveIfError(iCameraViewFinder.SetFrameRate(KDefaultFrameRate));

	User::LeaveIfError(iMMServer.Commit(iMMStreamContainer));

//	User::LeaveIfError(iGraphicsSink.SetScreenNumber(aDirectViewFinderInfo.iScreenNum)); // Commented out as not yet supported by translators
	User::LeaveIfError(iGraphicsSink.GetSurfaceId(aDirectViewFinderInfo.iSurfaceId));
	iClipRect = aDirectViewFinderInfo.iClipRect;
	}

TInt CMMCameraServerController::StartDirectViewFinder()
	{
	TRequestStatus status;
	iMMStream.Start(status);
	User::WaitForRequest(status);
	if(status.Int() == KErrNone)
		{
		iViewFinderState = CCamera::CCameraV2DirectViewFinder::EViewFinderActive;
		}

	return status.Int();
	}

void CMMCameraServerController::StopDirectViewFinder()
	{
	TRequestStatus status;
	iMMStream.Stop(status);
	User::WaitForRequest(status);
	iViewFinderState = CCamera::CCameraV2DirectViewFinder::EViewFinderInActive;
	}

void CMMCameraServerController::PauseDirectViewFinder()
	{
	TRequestStatus status;
	iMMStream.Pause(status);
	User::WaitForRequest(status);
	iViewFinderState = CCamera::CCameraV2DirectViewFinder::EViewFinderPause;
	}

void CMMCameraServerController::ResumeDirectViewFinder()
	{
	TRequestStatus status;
	iMMStream.Resume(status);
	User::WaitForRequest(status);
	iViewFinderState = CCamera::CCameraV2DirectViewFinder::EViewFinderActive;
	}

CCamera::CCameraV2DirectViewFinder::TViewFinderState CMMCameraServerController::GetDirectViewFinderState() const
	{
	return iViewFinderState;
	}

TInt CMMCameraServerController::SetDirectViewFinderMirror(const TBool aMirror)
	{
	// mismatch here between ECAM API (which only specify whether mirror is on or off)
	// and RMMCameraSensor (which specify horizontal and/or vertical mirror modes)
	if (aMirror)
		{
		return iCameraSensor.SetMirrorType(EMirrorHorizontal);
		}
	else
		{
		return iCameraSensor.SetMirrorType(EMirrorNone);
		}
	}

TInt CMMCameraServerController::GetDirectViewFinderMirror(TBool& aMirror)
	{
	// mismatch here between ECAM API (which only specify whether mirror is on or off)
	// and RMMCameraSensor (which specify horizontal and/or vertical mirror modes)
	TMMMirror mirror;
	TInt error = iCameraSensor.GetMirrorType(mirror);
	if (error != KErrNone)
		{
		return error;
		}

	if (mirror == EMirrorHorizontal)
		{
		aMirror = ETrue;
		}
	else
		{
		aMirror = EFalse;
		}

	return KErrNone;
	}

TInt CMMCameraServerController::DirectViewFinderProperties(TInt& aScreenNum, TRect& aScreenRect, TRect& aClipRect)
	{
	TUint screenNumber = 0;
	TInt error = iGraphicsSink.GetScreenNumber(screenNumber); // will currently just return 0 as screen number Set() not yet supported
	if (error == KErrNone)
		{
		aScreenNum = screenNumber;
		TUint width;
		TUint height;
		TInt error = iCameraViewFinder.GetFrameDimensions(width, height);
		if (error ==KErrNone)
			{
			aScreenRect.SetWidth(width);
			aScreenRect.SetHeight(height);
			aClipRect = iClipRect;
			return KErrNone;
			}
		}
	else
		{
		aScreenNum = -1;
		}

	return error;	
	}

TInt CMMCameraServerController::SetZoom(const TInt aZoom)
	{
	iZoom = aZoom;
	return KErrNone;
	}

TInt CMMCameraServerController::SetDigitalZoom(const TInt aDigitalZoom)
	{
	iDigitalZoom = aDigitalZoom;
	return KErrNone;
	}

TInt CMMCameraServerController::SetContrast(const TInt aContrast)
	{
	iContrast = aContrast;
	return KErrNone;
	}

TInt CMMCameraServerController::SetBrightness(const TInt aBrightness)
	{
	iBrightness = aBrightness;
	return KErrNone;
//	return iCameraSensor.SetBrightness(aBrightness);
	}

TInt CMMCameraServerController::SetFlash(const CCamera::TFlash aFlash)
	{
	iFlash = aFlash;
	return KErrNone;
	}

TInt CMMCameraServerController::SetExposure(const CCamera::TExposure aExposure)
	{
	iExposure = aExposure;
	return KErrNone;

	/*
	// Several CCamera exposure values dont have equivalent TMMExposureControl values
	switch (aExposure)
		{
		case CCamera::EExposureAuto:
			{
			return iCameraSensor.SetExposureControl(EExposureControlAuto);
			}
		case CCamera::EExposureNight:
			{
			return iCameraSensor.SetExposureControl(EExposureControlNight);
			}
		case CCamera::EExposureBacklight:
			{
			return iCameraSensor.SetExposureControl(EExposureControlBackLight);
			}
		case CCamera::EExposureCenter:
			{
			return iCameraSensor.SetExposureControl(EExposureControlSpotLight);
			}
		case CCamera::EExposureSport:
			{
			return iCameraSensor.SetExposureControl(EExposureControlSports);
			}
		case CCamera::EExposureSnow:
			{
			return iCameraSensor.SetExposureControl(EExposureControlSnow);
			}
		case CCamera::EExposureBeach:
			{
			return iCameraSensor.SetExposureControl(EExposureControlBeach);
			}
		default:
			{
			return KErrNotSupported;
			}
		}
*/	}

TInt CMMCameraServerController::SetWhiteBalance(const CCamera::TWhiteBalance aWhiteBalance)
	{
	iWhiteBalance = aWhiteBalance;
	return KErrNone;

/*	// Several CCamera white balance values dont have equivalent TMMWhiteBalControl values
	switch (aWhiteBalance)
		{
		case CCamera::EWBAuto:
			{
			return iCameraSensor.SetWhiteBalanceControl(EWhiteBalControlAuto);
			}
		case CCamera::EWBDaylight:
			{
			return iCameraSensor.SetWhiteBalanceControl(EWhiteBalControlSunLight);
			}
		case CCamera::EWBCloudy:
			{
			return iCameraSensor.SetWhiteBalanceControl(EWhiteBalControlCloudy);
			}
		case CCamera::EWBTungsten:
			{
			return iCameraSensor.SetWhiteBalanceControl(EWhiteBalControlTungsten);
			}
		case CCamera::EWBFluorescent:
			{
			return iCameraSensor.SetWhiteBalanceControl(EWhiteBalControlFluorescent);
			}
		case CCamera::EWBFlash:
			{
			return iCameraSensor.SetWhiteBalanceControl(EWhiteBalControlFlash);
			}
		case CCamera::EWBShade:
			{
			return iCameraSensor.SetWhiteBalanceControl(EWhiteBalControlShade);
			}
		case CCamera::EWBHorizon:
			{
			return iCameraSensor.SetWhiteBalanceControl(EWhiteBalControlHorizon);
			}
		default:
			{
			return KErrNotSupported;
			}
		}
*/	}

TInt CMMCameraServerController::GetZoom(TInt& aZoom)
	{
	aZoom = iZoom;
	return KErrNone;
	}

TInt CMMCameraServerController::GetDigitalZoom(TInt& aDigitalZoom)
	{
	aDigitalZoom = iDigitalZoom;
	return KErrNone;
	}

TInt CMMCameraServerController::GetContrast(TInt& aContrast)
	{
	aContrast = iContrast;
	return KErrNone;
	}

TInt CMMCameraServerController::GetBrightness(TInt& aBrightness)
	{
	aBrightness = iBrightness;
	return KErrNone;
//	return iCameraSensor.GetBrightness(aBrightness);
	}

TInt CMMCameraServerController::GetFlash(CCamera::TFlash& aFlash)
	{
	aFlash = iFlash;
	return KErrNone;
	}

TInt CMMCameraServerController::GetExposure(CCamera::TExposure& aExposure)
	{
	aExposure = iExposure;
	return KErrNone;

	/*
	// Several CCamera exposure values dont have equivalent TMMExposureControl values
	TMMExposureControl exposure;
	TInt error = iCameraSensor.GetExposureControl(exposure);
	if (error != KErrNone)
		{
		return error;
		}

	switch (exposure)
		{
		case EExposureControlAuto:
			{
			aExposure = CCamera::EExposureAuto;
			break;
			}
		case EExposureControlNight:
			{
			aExposure = CCamera::EExposureNight;
			break;
			}
		case EExposureControlBackLight:
			{
			aExposure = CCamera::EExposureBacklight;
			break;
			}
		case EExposureControlSpotLight:
			{
			aExposure = CCamera::EExposureCenter;
			break;
			}
		case EExposureControlSports:
			{
			aExposure = CCamera::EExposureSport;
			break;
			}
		case EExposureControlSnow:
			{
			aExposure = CCamera::EExposureSnow;
			break;
			}
		case EExposureControlBeach:
			{
			aExposure = CCamera::EExposureBeach;
			break;
			}
		default:
			{
			return KErrNotSupported;
			}
		}

	return KErrNone;
*/	}

TInt CMMCameraServerController::GetWhiteBalance(CCamera::TWhiteBalance& aWhiteBalance)
	{
	aWhiteBalance = iWhiteBalance;
	return KErrNone;

	/*
	// Several CCamera white balance values dont have equivalent TMMWhiteBalControl values
	TMMWhiteBalControl whiteBal;
	TInt error = iCameraSensor.GetWhiteBalanceControl(whiteBal);
	if (error != KErrNone)
		{
		return error;
		}

	switch (aWhiteBalance)
		{
		case EWhiteBalControlAuto:
			{
			aWhiteBalance = CCamera::EWBAuto;
			break;
			}
		case EWhiteBalControlSunLight:
			{
			aWhiteBalance = CCamera::EWBDaylight;
			break;
			}
		case EWhiteBalControlCloudy:
			{
			aWhiteBalance = CCamera::EWBCloudy;
			break;
			}
		case EWhiteBalControlTungsten:
			{
			aWhiteBalance = CCamera::EWBTungsten;
			break;
			}
		case EWhiteBalControlFluorescent:
			{
			aWhiteBalance = CCamera::EWBFluorescent;
			break;
			}
		case EWhiteBalControlFlash:
			{
			aWhiteBalance = CCamera::EWBFlash;
			break;
			}
		case EWhiteBalControlShade:
			{
			aWhiteBalance = CCamera::EWBShade;
			break;
			}
		case EWhiteBalControlHorizon:
			{
			aWhiteBalance = CCamera::EWBHorizon;
			break;
			}
		default:
			{
			return KErrNotSupported;
			}
		}

	return KErrNone;
*/	}

void CMMCameraServerController::Reset()
	{
	if (iViewFinderState == CCamera::CCameraV2DirectViewFinder::EViewFinderActive)
		{
		StopDirectViewFinder();
		}
	}




CMMCameraServerControllerQuery* CMMCameraServerControllerQuery::NewL()
	{
	CMMCameraServerControllerQuery* self = new (ELeave) CMMCameraServerControllerQuery();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CMMCameraServerControllerQuery::~CMMCameraServerControllerQuery()
	{
	iServer.Close();
	}

void CMMCameraServerControllerQuery::ConstructL()
	{
	User::LeaveIfError(iServer.Connect());
	}

CMMCameraServerControllerQuery::CMMCameraServerControllerQuery()
	{
	}

TInt CMMCameraServerControllerQuery::GetCamerasAvailable()
	{
	// Code to query MM server TBC
	return 1;
	}
