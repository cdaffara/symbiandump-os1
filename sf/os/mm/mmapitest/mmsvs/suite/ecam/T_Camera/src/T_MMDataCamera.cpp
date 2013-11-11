/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



#include "T_MMDataCamera.h"
#include "T_CameraParams.h"

const TInt						KDefaultBrightness		= 66;
const TInt						KDefaultContrast		= 53;
const CCamera::TExposure		KDefaultExposure		= CCamera::EExposureNight;
const CCamera::TFlash			KDefaultFlash			= CCamera::EFlashManual;
const TInt						KDefaultJpgQuality		= 56;
const TInt						KDefaultZf				= 0;
const CCamera::TWhiteBalance	KDefaultWhiteBalance	= CCamera::EWBDaylight;
const CCamera::TFormat			KDefaultFormat			= CCamera::EFormatFbsBitmapColor64K;
const TInt						KDefaultBuffersToUse	= 2;
const TInt						KDefaultViewFinderWidth	= 100;
const TInt						KDefaultViewFinderHeight= 100;

/*@{*/
_LIT(KTrue,							"TRUE");
_LIT(KFalse,						"FALSE");
_LIT(KClipTag,						"clipRect");
_LIT(KScreenTag,					"screenRect");
_LIT(KViewFinderTag,				"viewfinder");
_LIT(KCamerasAvailable,				"camerasAvailable");
_LIT(KPriorityPcnt,					"ppcnt");
_LIT(KCameraIndex, 					"camIndex");
_LIT(KCustomUid, 					"customUid");
_LIT(KCameraFormat, 				"cameraFormat");
_LIT(KObserverInterface, 			"observerInterface");
_LIT(KMObserver2, 					"MObserver2");
_LIT(KRectTop,						"%STop");
_LIT(KRectBottom,					"%SBottom");
_LIT(KRectLeft,						"%SLeft");
_LIT(KRectRight,					"%SRight");
_LIT(KSizeWidth,					"%SWidth");
_LIT(KSizeHeight,					"%SHeight");
_LIT(KWsSession,					"wsSession");
_LIT(KInvalidWsSession,				"InvalidWsSession");
_LIT(KWsScreenDevice,				"wsScreenDevice");
_LIT(KInvalidScrDev,				"InvalidScreenDevice");
_LIT(KWindow,						"window");
_LIT(KInvalidWindow,				"InvalidWindow");
_LIT(KRateIndex, 					"rateIndex");
_LIT(KSizeIndex, 					"sizeIndex");
_LIT(KBuffersInUse, 				"buffersToUse");
_LIT(KBrightness,					"brightness");
_LIT(KContrast,						"contrast");
_LIT(KWhiteBalance,					"whiteBalance");
_LIT(KDigitalZoomFactor,			"digitalZoomFactor");
_LIT(KZoomFactor,					"zoomFactor");
_LIT(KExposure,						"exposure");
_LIT(KJpegQuality,					"jpegQuality");
_LIT(KViewFinderMirror,				"viewFinderMirror");
_LIT(KFlash,						"flash");
_LIT(KAmount,						"amount");
_LIT(KFrameRate,					"frameRate");
_LIT(KFramesPerBuffer, 				"framesPerBuffer");
_LIT(KWidth,						"width");
_LIT(KHeight, 						"height");
_LIT(KHandle, 						"handle");
_LIT(KVideoCaptureActive,			"videoCaptureActive");
_LIT(KViewFinderActive,				"viewFinderActive");
_LIT(KOrientation, 					"orientation");
_LIT(KBrightnessSupported, 			"brightnessSupported");
_LIT(KContrastSupported,			"contrastSupported");
_LIT(KVfDirectSupported,			"vfDirectSupported");
_LIT(KVfBitmapsSupported,			"vfBitmapsSupported");
_LIT(KImageCaptureSupported, 		"imageCaptureSupported");
_LIT(KVideoCaptureSupported, 		"videoCaptureSupported");
_LIT(KVfMirrorSupported, 			"vfMirrorSupported");
_LIT(KVfClippingSupported, 			"vfClippingSupported");
_LIT(KImageClippingSupported, 		"imageClippingSupported");
_LIT(KVideoClippingSupported, 		"videoClippingSupported");
_LIT(KBitmapFile,					"bitmapfile");
_LIT(KCamera,						"camera");

_LIT(KCmdCamerasAvailable,			"CamerasAvailable");
_LIT(KCmdCameraInfo,				"CameraInfo");
_LIT(KCmdSetBrightnessL,			"SetBrightnessL");
_LIT(KCmdBrightness,				"Brightness");
_LIT(KCmdSetContrastL,				"SetContrastL");
_LIT(KCmdContrast,					"Contrast");
_LIT(KCmdSetDigitalZoomFactorL,		"SetDigitalZoomFactorL");
_LIT(KCmdDigitalZoomFactor,			"DigitalZoomFactor");
_LIT(KCmdSetExposureL,				"SetExposureL");
_LIT(KCmdExposure,					"Exposure");
_LIT(KCmdSetFlashL,					"SetFlashL");
_LIT(KCmdFlash,						"Flash");
_LIT(KCmdSetJpegQuality,			"SetJpegQuality");
_LIT(KCmdJpegQuality,				"JpegQuality");
_LIT(KCmdSetViewFinderMirrorL,		"SetViewFinderMirrorL");
_LIT(KCmdViewFinderMirror,			"ViewFinderMirror");
_LIT(KCmdSetWhiteBalanceL,			"SetWhiteBalanceL");
_LIT(KCmdWhiteBalance,				"WhiteBalance");
_LIT(KCmdSetZoomFactorL,			"SetZoomFactorL");
_LIT(KCmdZoomFactor,				"ZoomFactor");
_LIT(KCmdCaptureImage,				"CaptureImage");
_LIT(KCmdCancelCaptureImage,		"CancelCaptureImage");
_LIT(KCmdPrepareVideoCaptureL,		"PrepareVideoCaptureL"); // 2 overloads _LIT(PrepareVideoCaptureL(),
_LIT(KCmdEnumerateVideoFrameRates,	"EnumerateVideoFrameRates");
_LIT(KCmdEnumerateVideoFrameSizes,	"EnumerateVideoFrameSizes");
_LIT(KCmdStartVideoCapture,			"StartVideoCapture");
_LIT(KCmdVideoCaptureActive,		"VideoCaptureActive");
_LIT(KCmdStopVideoCapture,			"StopVideoCapture");
_LIT(KCmdCustomInterface,			"CustomInterface");
_LIT(KCmdEnumerateCaptureSizes,		"EnumerateCaptureSizes");
_LIT(KCmdFrameRate,					"FrameRate");
_LIT(KCmdBuffersInUse,				"BuffersInUse");
_LIT(KCmdFramesPerBuffer,			"FramesPerBuffer");
_LIT(KCmdGetFrameSize,				"GetFrameSize");
_LIT(KCmdHandle,					"Handle");
_LIT(KCmdNewDuplicateL,				"NewDuplicateL"); // 2 overloads_LIT(NewDuplicateL(),
_LIT(KCmdNewL,						"NewL"); // 2 overloads_LIT(NewL(),
_LIT(KCmdPowerOff,					"PowerOff");
_LIT(KCmdPowerOn,					"PowerOn");
_LIT(KCmdPrepareImageCaptureL,		"PrepareImageCaptureL"); // 2 overloads _LIT(PrepareImageCaptureL(),
_LIT(KCmdRelease,					"Release");
_LIT(KCmdReserve,					"Reserve");
_LIT(KCmdStartViewFinderBitmapsL,	"StartViewFinderBitmapsL"); // 2 overloads _LIT(KStartViewFinderBitmapsL, "StartViewFinderBitmapsL");
_LIT(KCmdStartViewFinderDirectL,	"StartViewFinderDirectL"); // 2 overloads _LIT(StartViewFinderDirectL(),
_LIT(KCmdStartViewFinderL,			"StartViewFinderL"); // 2 overloads _LIT(StartViewFinderL(),
_LIT(KCmdStopViewFinder,			"StopViewFinder");
_LIT(KCmdViewFinderActive,			"ViewFinderActive");
_LIT(KCmdDestructor,				"~");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_MMDataCamera* CT_MMDataCamera::NewL()
/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
	{
	CT_MMDataCamera*	ret=new (ELeave) CT_MMDataCamera();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_MMDataCamera::CT_MMDataCamera()
/**
 * Protected constructor. First phase construction
 */
:	iCamera(NULL)
,	iPrepareCaptureOk(EFalse)
,	iPrepareCaptureErr(KErrNone)
,	iCapturing(EFalse)
,	iHasBitmapFile(EFalse)
,	iAsyncErrorIndex(0)
,	iFbsSessionConnected(EFalse)
,	iWsSessionConnected(EFalse)
,	iScrDevice(NULL)
,	iWindowGroup(NULL)
,	iWindow(NULL)
	{
	}

void CT_MMDataCamera::ConstructL()
/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
	{
	User::LeaveIfError(iFbsSession.Connect()); // Will leave with KErrNotFound if TextShell in epoc.ini
	iFbsSessionConnected=ETrue;

	User::LeaveIfError(iWsSession.Connect());
	iWsSessionConnected=ETrue;

	iScrDevice = new (ELeave) CWsScreenDevice(iWsSession);
	User::LeaveIfError(iScrDevice->Construct());

	iWindowGroup = new (ELeave) RWindowGroup(iWsSession);
	User::LeaveIfError(iWindowGroup->Construct(reinterpret_cast<TUint32>(iWindowGroup), EFalse));

	iWindow = new (ELeave) RWindow(iWsSession);
	User::LeaveIfError(iWindow->Construct(*iWindowGroup, reinterpret_cast<TUint32>(iWindowGroup+1)));
    iWindow->SetExtent(TPoint(0,0), TSize(100,100));
    iWindow->SetVisible(ETrue);
    iWindow->Activate();
    iWsSession.Flush();
	}

CT_MMDataCamera::~CT_MMDataCamera()
/**
 * Public destructor
 */
	{
	DestroyData();

	if ( iWindow!=NULL )
		{
		iWindow->Close();
		delete iWindow;
		iWindow = NULL;
		}

	if ( iWindowGroup!=NULL )
		{
		iWindowGroup->Close();
		delete iWindowGroup;
		}

	delete iScrDevice;
	iScrDevice = NULL;

	if ( iWsSessionConnected )
		{
		iWsSession.Close();
		iWsSessionConnected=EFalse;
		}

	if ( iFbsSessionConnected )
		{
		iFbsSession.Disconnect();
		iFbsSessionConnected=EFalse;
		}
	}

void CT_MMDataCamera::DestroyData()
	{
	delete iCamera;
	iCamera=NULL;
	iCameraInfo=TCameraInfo();
	iAsyncErrorIndex = 0;
	}

TAny* CT_MMDataCamera::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return iCamera;
	}

void CT_MMDataCamera::SetObjectL(TAny* aObject)
	{
	DestroyData();
	iCamera	= static_cast<CCamera*> (aObject);
	}

void CT_MMDataCamera::DisownObjectL()
	{
	iCamera = NULL;
	}

TBool CT_MMDataCamera::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
	{
	TBool	ret=ETrue;

	if(aCommand == KCmdBrightness)
		{
		DoCmdBrightness(aSection);
		}
	else if(aCommand == KCmdSetBrightnessL)
		{
		DoCmdSetBrightness(aSection);
		}
	else if(aCommand == KCmdContrast)
		{
		DoCmdContrast(aSection);
		}
	else if(aCommand == KCmdSetContrastL)
		{
		DoCmdSetContrastL(aSection);
		}
	else if(aCommand == KCmdDigitalZoomFactor)
		{
		DoCmdDigitalZoomFactor(aSection);
		}
	else if(aCommand == KCmdSetDigitalZoomFactorL)
		{
		DoCmdSetDigitalZoomFactorL(aSection);
		}
	else if(aCommand == KCmdExposure)
		{
		DoCmdExposure(aSection);
		}
	else if(aCommand == KCmdSetExposureL)
		{
		DoCmdSetExposureL(aSection);
		}
	else if(aCommand == KCmdFlash)
		{
		DoCmdFlash(aSection);
		}
	else if(aCommand == KCmdSetFlashL)
		{
		DoCmdSetFlashL(aSection);
		}
	else if(aCommand == KCmdViewFinderMirror)
		{
		DoCmdViewFinderMirror(aSection);
		}
	else if(aCommand == KCmdSetViewFinderMirrorL)
		{
		DoCmdSetViewFinderMirrorL(aSection);
		}
	else if(aCommand == KCmdWhiteBalance)
		{
		DoCmdWhiteBalance(aSection);
		}
	else if(aCommand == KCmdSetWhiteBalanceL)
		{
		DoCmdSetWhiteBalanceL(aSection);
		}
	else if(aCommand == KCmdZoomFactor)
		{
		DoCmdZoomFactor(aSection);
		}
	else if(aCommand == KCmdSetZoomFactorL)
		{
		DoCmdSetZoomFactorL(aSection);
		}
	else if(aCommand == KCmdJpegQuality)
		{
		DoCmdJpegQuality(aSection);
		}
	else if(aCommand == KCmdSetJpegQuality)
		{
		DoCmdSetJpegQuality(aSection);
		}
	else if(aCommand == KCmdBuffersInUse)
		{
		DoCmdBuffersInUse(aSection);
		}
	else if(aCommand == KCmdCameraInfo)
		{
		DoCmdCameraInfo(aSection);
		}
	else if(aCommand == KCmdCamerasAvailable)
		{
		DoCmdCamerasAvailable(aSection);
		}
	else if(aCommand == KCmdCancelCaptureImage)
		{
		DoCmdCancelCaptureImage();
		}
	else if(aCommand == KCmdCaptureImage)
		{
		DoCmdCaptureImage(aSection, aAsyncErrorIndex);
		}
	else if(aCommand == KCmdCustomInterface)
		{
		DoCmdCustomInterface(aSection);
		}
	else if(aCommand == KCmdEnumerateCaptureSizes)
		{
		DoCmdEnumerateCaptureSizes(aSection);
		}
	else if(aCommand == KCmdEnumerateVideoFrameRates)
		{
		DoCmdEnumerateVideoFrameRates(aSection);
		}
	else if(aCommand == KCmdEnumerateVideoFrameSizes)
		{
		DoCmdEnumerateVideoFrameSizes(aSection);
		}
	else if(aCommand == KCmdFrameRate)
		{
		DoCmdFrameRate(aSection);
		}
	else if(aCommand == KCmdFramesPerBuffer)
		{
		DoCmdFramesPerBuffer(aSection);
		}
	else if(aCommand == KCmdGetFrameSize)
		{
		DoCmdGetFrameSize(aSection);
		}
	else if(aCommand == KCmdHandle)
		{
		DoCmdHandle();
		}
	else if(aCommand == KCmdNewDuplicateL)
		{
		DoCmdNewDuplicateL(aSection);
		}
	else if ( aCommand==KCmdNewL )
		{
		DoCmdNewL(aSection);
		}
	else if(aCommand == KCmdPowerOff)
		{
		DoCmdPowerOff();
		}
	else if(aCommand == KCmdPowerOn)
		{
		DoCmdPowerOn(aAsyncErrorIndex);
		}
	else if(aCommand == KCmdPrepareImageCaptureL)
		{
		DoCmdPrepareImageCaptureL(aSection);
		}
	else if(aCommand == KCmdPrepareVideoCaptureL)
		{
		DoCmdPrepareVideoCaptureL(aSection);
		}
	else if(aCommand == KCmdRelease)
		{
		DoCmdRelease();
		}
	else if(aCommand == KCmdReserve)
		{
		DoCmdReserve(aAsyncErrorIndex);
		}
	else if(aCommand == KCmdStartVideoCapture)
		{
		DoCmdStartVideoCapture(aSection, aAsyncErrorIndex);
		}
	else if(aCommand == KCmdStartViewFinderBitmapsL)
		{
		DoCmdStartViewFinderBitmapsL(aSection);
		}
	else if(aCommand == KCmdStartViewFinderDirectL)
		{
		DoCmdStartViewFinderDirectL(aSection);
		}
	else if(aCommand == KCmdStartViewFinderL)
		{
		DoCmdStartViewFinderL(aSection);
		}
	else if(aCommand == KCmdStopVideoCapture)
		{
		DoCmdStopVideoCapture();
		}
	else if(aCommand == KCmdStopViewFinder)
		{
		DoCmdStopViewFinder();
		}
	else if(aCommand == KCmdVideoCaptureActive)
		{
		DoCmdVideoCaptureActive(aSection);
		}
	else if(aCommand == KCmdViewFinderActive)
		{
		DoCmdViewFinderActive(aSection);
		}
	else if(aCommand == KCmdDestructor)
		{
		DoCmdDestructor();
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

void CT_MMDataCamera::DoCmdBrightness(const TDesC& aSection)
	{
	TInt	actualBrightness = iCamera->Brightness();
	INFO_PRINTF2(_L("Brightness=%d"), actualBrightness);

	TInt	expectedBrightness = KDefaultBrightness;
	GetIntFromConfig(aSection, KBrightness(), expectedBrightness);
	if ( actualBrightness!=expectedBrightness )
		{
		//	Is brightness supported ?
		if ( iCameraInfo.iOptionsSupported&TCameraInfo::EBrightnessSupported )
			{
			//	Brigtness supported. Report error.
			ERR_PRINTF3(_L("Brightness actual(%d)!=expected(%d)"), actualBrightness, expectedBrightness);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdSetBrightness(const TDesC& aSection)
	{
	TInt	brightness = KDefaultBrightness;
	GetIntFromConfig(aSection, KBrightness(), brightness);

	TInt	expected=KErrNone;
	if ( !(iCameraInfo.iOptionsSupported&TCameraInfo::EBrightnessSupported) )
		{
		expected=KErrNotSupported;
		}

	INFO_PRINTF2(_L("Setting brightness to %d"), brightness);

	TRAPD(err, iCamera->SetBrightnessL(brightness));
	if ( err!=expected )
		{
		ERR_PRINTF3(_L("CCamera::SetBrightnessL failed with error %d. Expected %d"), err, expected);
		SetError(err);
		}
	}

void CT_MMDataCamera::DoCmdContrast(const TDesC& aSection)
	{

	TInt	actualContrast = iCamera->Contrast();
	INFO_PRINTF2(_L("contrast=%d"), actualContrast);

	TInt	expectedContrast = KDefaultContrast;
	GetIntFromConfig(aSection, KContrast(), expectedContrast);
	if(actualContrast != expectedContrast)
		{
		//	Is contrast supported ?
		if ( iCameraInfo.iOptionsSupported&TCameraInfo::EContrastSupported )
			{
			//	Contrast supported. Report error.
			ERR_PRINTF3(_L("Contrast actual(%d)!=expected(%d)"), actualContrast, expectedContrast);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdSetContrastL(const TDesC& aSection)
	{
	TInt	contrast = KDefaultContrast;
	GetIntFromConfig(aSection, KContrast(), contrast);

	TInt	expected=KErrNone;
	if ( !(iCameraInfo.iOptionsSupported&TCameraInfo::EContrastSupported) )
		{
		expected=KErrNotSupported;
		}

	INFO_PRINTF2(_L("Setting contrast to %d"), contrast);

	TRAPD(err,iCamera->SetContrastL(contrast));
	if ( err!=expected )
		{
		ERR_PRINTF3(_L("CCamera::SetContrastL failed with error %d. Expected %d"), err, expected);
		SetError(err);
		}
	}

void CT_MMDataCamera::DoCmdDigitalZoomFactor(const TDesC& aSection)
	{
	TInt	actualDigitalZf = iCamera->DigitalZoomFactor();
	INFO_PRINTF2(_L("digital zoom factor=%d"), actualDigitalZf);

	TInt	expectedDigitalZf = GetDefaultDigitalZf();
	GetIntFromConfig(aSection, KDigitalZoomFactor(), expectedDigitalZf);
	if(actualDigitalZf != expectedDigitalZf)
		{
		ERR_PRINTF3(_L("Digital Zoom Factor actual(%d)!=expected(%d)")
					, actualDigitalZf, expectedDigitalZf);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataCamera::DoCmdSetDigitalZoomFactorL(const TDesC& aSection)
	{
	TInt	digitalZf = GetDefaultDigitalZf();
	GetIntFromConfig(aSection, KZoomFactor(), digitalZf);

	INFO_PRINTF2(_L("Setting digital zoom factor to %d"), digitalZf);

	TRAPD(err, iCamera->SetDigitalZoomFactorL(digitalZf));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CCamera::SetDigitalZoomFactorL failed with error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataCamera::DoCmdExposure(const TDesC& aSection)
	{
	CCamera::TExposure	actualExposure = iCamera->Exposure();
	const TDesC&		actualExpDes = GetParamString(actualExposure);
	INFO_PRINTF2(_L("exposure %S"), &actualExpDes);

	CCamera::TExposure	expectedExposure = KDefaultExposure;
	GetParam(expectedExposure, aSection);
	if ( actualExposure != expectedExposure )
		{
		//	Is exposure supported ?
		if ( iCameraInfo.iExposureModesSupported&expectedExposure )
			{
			//	Exposure supported. Report error.
			ERR_PRINTF3(_L("Actual(%S) and expected(%S) exposures do not match!")
						, &actualExpDes, &GetParamString(expectedExposure));
			SetBlockResult(EFail);
			}
		}
	else
		{
		if ( !(iCameraInfo.iExposureModesSupported&actualExposure) && (actualExposure!=CCamera::EExposureAuto) )
			{
			//	Exposure not supported. Report error.
			ERR_PRINTF2(_L("Actual(%S) exposure not supported ???"), &actualExpDes);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdSetExposureL(const TDesC& aSection)
	{

	CCamera::TExposure	exposure = KDefaultExposure;
	GetParam(exposure, aSection);
	INFO_PRINTF3(_L("Setting exposure to %S (%d)"), &GetParamString(exposure), exposure);

	TInt	expected=KErrNone;
	if ( !(iCameraInfo.iExposureModesSupported&exposure) && (exposure!=CCamera::EExposureAuto) )
		{
		expected=KErrNotSupported;
		}
	TRAPD(err, iCamera->SetExposureL(exposure));
	if ( err!=expected )
		{
		ERR_PRINTF3(_L("CCamera::SetExposureL failed with error %d. Expected %d"), err, expected);
		SetError(err);
		}
	}

void CT_MMDataCamera::DoCmdFlash(const TDesC& aSection)
	{
	CCamera::TFlash	actualFlash = iCamera->Flash();
	const TDesC&	actualFlashDes = GetParamString(actualFlash);
	INFO_PRINTF2(_L("Flash Actual(%S)"), &actualFlashDes);

	CCamera::TFlash	expectedFlash = KDefaultFlash;
	GetParam(expectedFlash, aSection);

	if ( actualFlash != expectedFlash )
		{
		//	Is flash supported ?
		if ( iCameraInfo.iFlashModesSupported&expectedFlash )
			{
			//	Flash supported. Report error.
			ERR_PRINTF3(_L("Actual(%S) and expected(%S) flash do not match!")
					, &actualFlashDes, &GetParamString(expectedFlash));
			SetBlockResult(EFail);
			}
		}
	else
		{
		if ( !(iCameraInfo.iFlashModesSupported&actualFlash) && (actualFlash!=CCamera::EFlashNone) )
			{
			//	Flash not supported. Report error.
			ERR_PRINTF2(_L("Actual(%S) flash not supported ???"), &actualFlashDes);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdSetFlashL(const TDesC& aSection)
	{
	CCamera::TFlash	flash = KDefaultFlash;
	GetParam(flash, aSection);
	INFO_PRINTF2(_L("Setting flash to %S"), &GetParamString(flash));

	TInt	expected=KErrNone;
	if ( !(iCameraInfo.iFlashModesSupported&flash) && (flash!=CCamera::EFlashNone) )
		{
		expected=KErrNotSupported;
		}
	TRAPD(err, iCamera->SetFlashL(flash));
	if ( err!=expected )
		{
		ERR_PRINTF3(_L("CCamera::SetFlashL failed with error %d. Expected %d"), err, expected);
		SetError(err);
		}
	}

void CT_MMDataCamera::DoCmdViewFinderMirror(const TDesC& aSection)
	{
	TBool	actualVFM = iCamera->ViewFinderMirror();

	if(actualVFM)
		{
		INFO_PRINTF1(_L("ViewFinderMirror = true"));
		}
	else
		{
		INFO_PRINTF1(_L("ViewFinderMirror = false"));
		}
	TBool	expectedVFM = EFalse;
	if(GetBoolFromConfig(aSection, KViewFinderMirror(), expectedVFM))
		{
		if(actualVFM != expectedVFM)
			{
			ERR_PRINTF3(_L("Actual VFM(%d) != expectedVFM(%D)"), actualVFM, expectedVFM);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdSetViewFinderMirrorL(const TDesC& aSection)
	{
	TBool	mirror = EFalse;
	GetBoolFromConfig(aSection, KViewFinderMirror(), mirror);

	TInt	err = KErrNone;
	TRAP(err, iCamera->SetViewFinderMirrorL(mirror));
	if ( (err!=KErrNone) )
		{
		ERR_PRINTF2(_L("CCamera::SetViewFinderMirrorL failed with error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataCamera::DoCmdWhiteBalance(const TDesC& aSection)
	{
	CCamera::TWhiteBalance	actualWb = iCamera->WhiteBalance();
	const TDesC&			wbDes = GetParamString(actualWb);
	INFO_PRINTF2(_L("Actual White Balance = %S"), &wbDes);
	CCamera::TWhiteBalance	expectedWb = KDefaultWhiteBalance;
	GetParam(expectedWb, aSection);

	if(actualWb != expectedWb)
		{
		//	Is white balance supported ?
		if ( iCameraInfo.iWhiteBalanceModesSupported&expectedWb )
			{
			//	White balance supported. Report error.
			ERR_PRINTF3(_L("Actual(%S) and expected(%S) white balance do not match!")
					, &wbDes, &GetParamString(expectedWb));
			SetBlockResult(EFail);
			}
		}
	else
		{
		if ( !(iCameraInfo.iWhiteBalanceModesSupported&actualWb) && (actualWb!=CCamera::EWBAuto) )
			{
			//	White balance not supported. Report error.
			ERR_PRINTF2(_L("Actual(%S) white balance not supported ???"), &wbDes);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdSetWhiteBalanceL(const TDesC& aSection)
	{
	CCamera::TWhiteBalance	whiteBalance = CCamera::EWBAuto;
	GetParam(whiteBalance, aSection);
	INFO_PRINTF3(_L("Setting white balance to %S (%d)"), &GetParamString(whiteBalance), whiteBalance);

	TInt	expected=KErrNone;
	if ( !(iCameraInfo.iWhiteBalanceModesSupported&whiteBalance) && (whiteBalance!=CCamera::EWBAuto) )
		{
		expected=KErrNotSupported;
		}
	TRAPD(err, iCamera->SetWhiteBalanceL(whiteBalance));
	if ( err!=expected )
		{
		ERR_PRINTF3(_L("CCamera::SetWhiteBalanceL failed with error %d. Expected %d"), err, expected);
		SetError(err);
		}
	}

void CT_MMDataCamera::DoCmdZoomFactor(const TDesC& aSection)
	{
	TInt	actualZf = iCamera->ZoomFactor();
	INFO_PRINTF2(_L("Zoom Factor = %d"), actualZf);

	TInt	expectedZf = KDefaultZf;
	GetIntFromConfig(aSection, KZoomFactor(), expectedZf);
	if(actualZf != expectedZf)
		{
		ERR_PRINTF3(_L("Actual zoom factor(%d)!=Expected zoom factor(%d)")
					, actualZf, expectedZf);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataCamera::DoCmdSetZoomFactorL(const TDesC& aSection)
	{
	TInt	zoomFactor = KDefaultZf;
	GetIntFromConfig(aSection, KZoomFactor(), zoomFactor);
	INFO_PRINTF2(_L("Setting zoom factor to %d"), zoomFactor);
	TRAPD(err, iCamera->SetZoomFactorL(zoomFactor));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CCamera::SetZoomFactorL failed with error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataCamera::DoCmdJpegQuality(const TDesC& aSection)
	{
	TInt	actualJpq = iCamera->JpegQuality();
	INFO_PRINTF2(_L("Actual Jpeg Quality = %d"), actualJpq);

	TInt	expectedJpq = KDefaultJpgQuality;
	GetIntFromConfig(aSection, KJpegQuality(), expectedJpq);
	if(actualJpq != expectedJpq)
		{
		//	Check jpeg supported
		if ( iCameraInfo.iImageFormatsSupported&CCamera::EFormatJpeg )
			{
			//	Jpeg supported
			ERR_PRINTF3(_L("Actual Jpeg Quality(%d)!=Expected Jpeg Quality(%d)")
						, actualJpq, expectedJpq);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdSetJpegQuality(const TDesC& aSection)
	{
	TInt	quality = KDefaultJpgQuality;
	GetIntFromConfig(aSection, KJpegQuality(), quality);
	INFO_PRINTF2(_L("Setting Jpeg Quality to %d"), quality);
	// how can we fail this step - returns void and doesn't leave !
	iCamera->SetJpegQuality(quality);
	}

void CT_MMDataCamera::DoCmdBuffersInUse(const TDesC& aSection)
	{
	TInt	actualBIU = iCamera->BuffersInUse();
	INFO_PRINTF2(_L("Buffers In Use=%d"), actualBIU);

	TInt	expectedBIU = 0;
	if(GetIntFromConfig(aSection, KBuffersInUse(), expectedBIU))
		{
		if(expectedBIU != actualBIU)
			{
			ERR_PRINTF3(_L("Actual buffers(%d)!= Expected buffers(%d)")
						, actualBIU, expectedBIU);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdCameraInfo(const TDesC& aSection)
	{
	iCamera->CameraInfo(iCameraInfo);
	INFO_PRINTF1(_L("CCamera::CameraInfo() called ok"));
	INFO_PRINTF2(_L("TCameraInfo.iOptionsSupported            = %x"), iCameraInfo.iOptionsSupported);
	INFO_PRINTF2(_L("TCameraInfo.iFlashModesSupported         = %x"), iCameraInfo.iFlashModesSupported);
	INFO_PRINTF2(_L("TCameraInfo.iExposureModesSupported      = %x"), iCameraInfo.iExposureModesSupported);
	INFO_PRINTF2(_L("TCameraInfo.iWhiteBalanceModesSupported  = %x"), iCameraInfo.iWhiteBalanceModesSupported);
	INFO_PRINTF2(_L("TCameraInfo.iMinZoom                     = %d"), iCameraInfo.iMinZoom);
	INFO_PRINTF2(_L("TCameraInfo.iMaxZoom                     = %d"), iCameraInfo.iMaxZoom);
	INFO_PRINTF2(_L("TCameraInfo.iMaxDigitalZoom              = %d"), iCameraInfo.iMaxDigitalZoom);
	INFO_PRINTF2(_L("TCameraInfo.iMinZoomFactor               = %f"), iCameraInfo.iMinZoomFactor);
	INFO_PRINTF2(_L("TCameraInfo.iMaxZoomFactor               = %f"), iCameraInfo.iMaxZoomFactor);
	INFO_PRINTF2(_L("TCameraInfo.iMaxDigitalZoomFactor        = %f"), iCameraInfo.iMaxDigitalZoomFactor);
	INFO_PRINTF2(_L("TCameraInfo.iNumImageSizesSupported      = %d"), iCameraInfo.iNumImageSizesSupported);
	INFO_PRINTF2(_L("TCameraInfo.iImageFormatsSupported       = %x"), iCameraInfo.iImageFormatsSupported);
	INFO_PRINTF2(_L("TCameraInfo.iNumVideoFrameSizesSupported = %d"), iCameraInfo.iNumVideoFrameSizesSupported);
	INFO_PRINTF2(_L("TCameraInfo.iNumVideoFrameRatesSupported = %d"), iCameraInfo.iNumVideoFrameRatesSupported);
	INFO_PRINTF2(_L("TCameraInfo.iVideoFrameFormatsSupported  = %x"), iCameraInfo.iVideoFrameFormatsSupported);
	INFO_PRINTF2(_L("TCameraInfo.iMaxFramesPerBufferSupported = %d"), iCameraInfo.iMaxFramesPerBufferSupported);
	INFO_PRINTF2(_L("TCameraInfo.iMaxBuffersSupported         = %d"), iCameraInfo.iMaxBuffersSupported);

	//EViewFinderDirectSupported View finder display direct-to-screen flag
	TBool	flag=(iCameraInfo.iOptionsSupported & TCameraInfo::EViewFinderDirectSupported) != 0;
	INFO_PRINTF2(_L("VF direct supported = %S"), (flag ? &KTrue() : &KFalse()));
	TBool	supported;
	if(GetBoolFromConfig(aSection, KVfDirectSupported(), supported))
		{
		if(supported != flag)
			{
			ERR_PRINTF3(_L("Vf direct supported does not match actual(%d) expected(%d)"), flag, supported);
			SetBlockResult(EFail);
			}
		}

	//EViewFinderBitmapsSupported View finder bitmap generation flag
	flag = (iCameraInfo.iOptionsSupported & TCameraInfo::EViewFinderBitmapsSupported) != 0;
	INFO_PRINTF2(_L("VF bitmaps supported = %S"), (flag ? &KTrue() : &KFalse()));
	if(GetBoolFromConfig(aSection, KVfBitmapsSupported(), supported))
		{
		if(supported != flag)
			{
			ERR_PRINTF3(_L("Vf direct supported does not match actual(%d) expected(%d)"), flag, supported);
			SetBlockResult(EFail);
			}
		}

	//EImageCaptureSupported Still image capture flag
	flag = (iCameraInfo.iOptionsSupported & TCameraInfo::EImageCaptureSupported) != 0;
	INFO_PRINTF2(_L("Image Capture supported = %S"), (flag ? &KTrue() : &KFalse()));
	if(GetBoolFromConfig(aSection, KImageCaptureSupported(), supported))
		{
		if(supported != flag)
			{
			ERR_PRINTF3(_L("Image Capture supported does not match actual(%d) expected(%d)"), flag, supported);
			SetBlockResult(EFail);
			}
		}

	//EVideoCaptureSupported Video capture flag
	flag = (iCameraInfo.iOptionsSupported & TCameraInfo::EVideoCaptureSupported) != 0;
	INFO_PRINTF2(_L("Video Capture supported = %S"), (flag ? &KTrue() : &KFalse()));
	if(GetBoolFromConfig(aSection, KVideoCaptureSupported(), supported))
		{
		if(supported != flag)
			{
			ERR_PRINTF3(_L("Video Capture supported does not match actual(%d) expected(%d)"), flag, supported);
			SetBlockResult(EFail);
			}
		}

	//EViewFinderMirrorSupported View finder display mirroring flag
	flag = (iCameraInfo.iOptionsSupported & TCameraInfo::EViewFinderMirrorSupported) != 0;
	INFO_PRINTF2(_L("Vf mirror Capture supported = %S"), (flag ? &KTrue() : &KFalse()));
	if(GetBoolFromConfig(aSection, KVfMirrorSupported(), supported))
		{
		if(supported != flag)
			{
			ERR_PRINTF3(_L("Vf mirror Capture supported does not match actual(%d) expected(%d)"), flag, supported);
			SetBlockResult(EFail);
			}
		}

	//EContrastSupported Contrast setting flag
	flag = (iCameraInfo.iOptionsSupported & TCameraInfo::EContrastSupported) != 0;
	INFO_PRINTF2(_L("Contrast supported = %S"), (flag ? &KTrue() : &KFalse()));
	if(GetBoolFromConfig(aSection, KContrastSupported(), supported))
		{
		if(supported != flag)
			{
			ERR_PRINTF3(_L("Contrast supported does not match actual(%d) expected(%d)"), flag, supported);
			SetBlockResult(EFail);
			}
		}

	//EBrightnessSupported Brightness setting flag
	flag = (iCameraInfo.iOptionsSupported & TCameraInfo::EBrightnessSupported) != 0;
	INFO_PRINTF2(_L("Brightness supported = %S"), (flag ? &KTrue() : &KFalse()));
	if(GetBoolFromConfig(aSection, KBrightnessSupported(), supported))
		{
		if(supported != flag)
			{
			ERR_PRINTF3(_L("Brightness supported does not match actual(%d) expected(%d)"), flag, supported);
			SetBlockResult(EFail);
			}
		}

	//EViewFinderClippingSupported View finder clipping flag
	flag = (iCameraInfo.iOptionsSupported & TCameraInfo::EViewFinderClippingSupported) != 0;
	INFO_PRINTF2(_L("vf clipping supported = %S"), (flag ? &KTrue() : &KFalse()));
	if(GetBoolFromConfig(aSection, KVfClippingSupported(), supported))
		{
		if(supported != flag)
			{
			ERR_PRINTF3(_L("Vf clipping supported does not match actual(%d) expected(%d)"), flag, supported);
			SetBlockResult(EFail);
			}
		}

	//EImageClippingSupported Still image capture clipping flag
	flag = (iCameraInfo.iOptionsSupported & TCameraInfo::EImageClippingSupported) != 0;
	INFO_PRINTF2(_L("image clipping supported = %S"), (flag ? &KTrue() : &KFalse()));
	if(GetBoolFromConfig(aSection, KImageClippingSupported(), supported))
		{
		if(supported != flag)
			{
			ERR_PRINTF3(_L("Image clipping supported does not match actual(%d) expected(%d)"), flag, supported);
			SetBlockResult(EFail);
			}
		}

	//EVideoClippingSupported Video capture clipping flag
	flag = (iCameraInfo.iOptionsSupported & TCameraInfo::EVideoClippingSupported) != 0;
	INFO_PRINTF2(_L("video clipping supported = %S"), (flag ? &KTrue() : &KFalse()));
	if(GetBoolFromConfig(aSection, KVideoClippingSupported(), supported))
		{
		if(supported != flag)
			{
			ERR_PRINTF3(_L("Video clipping supported does not match actual(%d) expected(%d)"), flag, supported);
			SetBlockResult(EFail);
			}
		}

	// orientation
	INFO_PRINTF2(_L("Camera Info - orientation(%S)"), &GetParamString(iCameraInfo.iOrientation));
	TCameraInfo::TCameraOrientation expectedOrientation;
	if(GetParam(expectedOrientation, aSection))
		{
		if(expectedOrientation != iCameraInfo.iOrientation)
			{
			ERR_PRINTF3(_L("Camera info orientation(%S) does not match expected value(%S)")
						, &GetParamString(iCameraInfo.iOrientation)
						, &GetParamString(expectedOrientation));
			SetBlockResult(EFail);
			}
		}

	//Number of capture sizes
	TInt	expectedCaptureSizes = 0;
	if(GetIntFromConfig(aSection, KAmount(), expectedCaptureSizes))
		{
		if(expectedCaptureSizes != iCameraInfo.iNumImageSizesSupported)
			{
			ERR_PRINTF3(_L("Number of capture sizes do not match actual(%d) expected(%d)")
						, iCameraInfo.iNumImageSizesSupported, expectedCaptureSizes);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdCamerasAvailable(const TDesC& aSection)
	{
	TInt	camerasExpected = 1;
	TInt	camerasAvailable = CCamera::CamerasAvailable();
	GetIntFromConfig(aSection, KCamerasAvailable(), camerasExpected);
	INFO_PRINTF2(_L("Cameras available=%d"), camerasAvailable);

	if(camerasAvailable != camerasExpected)
		{
		ERR_PRINTF3(_L("Cameras Available actual(%d)!=expected(%d)")
					, camerasAvailable, camerasExpected);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataCamera::DoCmdCancelCaptureImage()
	{
	iCamera->CancelCaptureImage();
	if ( iCapturing )
		{
		DecOutstanding();
		iCapturing=EFalse;
		}
	}

void CT_MMDataCamera::DoCmdCaptureImage(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	iAsyncErrorIndex = aAsyncErrorIndex;

	iHasBitmapFile=GetStringFromConfig(aSection, KBitmapFile(), iBitmapFile);
	iCamera->CaptureImage();
	iCapturing=ETrue;
	IncOutstanding();
	// callback in MCameraObserver::ImageReady interface
	// that is where this test can fail !!
	}

void CT_MMDataCamera::DoCmdCustomInterface(const TDesC& aSection)
	{
	TInt	uidParam = 0;

	if ( GetHexFromConfig(aSection, KCustomUid(), uidParam) )
		{
		TUid	customUid=TUid::Uid(uidParam);
		TAny*	customInterface = iCamera->CustomInterface(customUid);
		if(customInterface)
			{
			// cast the interface pointer to the desired type
			}
		else
			{
			ERR_PRINTF2(_L("CCamera::CustomInterface(0x%X) returned NULL"), customUid.iUid);
			SetError(KErrNotFound);
			}
		}
	else
		{
		ERR_PRINTF1(_L("CCamera::CustomInterface param not found"));
		SetError(KErrNotFound);
		}
	}

void CT_MMDataCamera::DoCmdEnumerateCaptureSizes(const TDesC& aSection)
	{
	CCamera::TFormat	format = KDefaultFormat;
	GetParam(format, aSection);
	INFO_PRINTF3(_L("Camera format is %S (%d)"), &GetParamString(format), format);

	TInt	sizeIndex;
	if (!GetIntFromConfig(aSection, KSizeIndex(), sizeIndex))
		{
		for ( TInt i=0; i<iCameraInfo.iNumImageSizesSupported; i++ )
			{
			TSize	sz;
			iCamera->EnumerateCaptureSizes(sz, i, format);
			INFO_PRINTF3(_L("Capture size w[%d] h[%d]"), sz.iWidth, sz.iHeight);
			}
		}
	else
		{
		TSize	sz;
		iCamera->EnumerateCaptureSizes(sz, sizeIndex, format);
		INFO_PRINTF3(_L("Capture size w[%d] h[%d]"), sz.iWidth, sz.iHeight);

		TInt	expectedWidth=0;
		if (GetIntFromConfig(aSection, KWidth(), expectedWidth))
			{
			if (expectedWidth != sz.iWidth)
				{
				ERR_PRINTF3(_L("Widths do not match actual(%d) expected(%d)"), sz.iWidth, expectedWidth);
				SetBlockResult(EFail);
				}
			}
		TInt	expectedHeight=0;
		if (GetIntFromConfig(aSection, KHeight(), expectedHeight))
			{
			if (expectedHeight != sz.iHeight)
				{
				ERR_PRINTF3(_L("Heights do not match actual(%d) expected(%d)"), sz.iHeight, expectedHeight);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataCamera::DoCmdEnumerateVideoFrameRates(const TDesC& aSection)
	{
	CCamera::TFormat	format = KDefaultFormat;
	GetParam(format, aSection);
	INFO_PRINTF3(_L("Camera format is %S (%d)"), &GetParamString(format), format);

	CCamera::TExposure exposure = CCamera::EExposureAuto;
	GetParam(exposure, aSection);
	INFO_PRINTF3(_L("exposure %S (%d)"), &GetParamString(exposure), exposure);

	TInt	sizeIndex = 0;
	TBool	sizeIndexGiven = GetIntFromConfig(aSection, KSizeIndex(), sizeIndex);

	TInt	rateIndex = 0;
	TBool	rateIndexGiven = GetIntFromConfig(aSection, KRateIndex(), rateIndex);

	if ( sizeIndexGiven || rateIndexGiven )
		{
		TReal32	actualRate = .0;
		iCamera->EnumerateVideoFrameRates(actualRate, rateIndex, format, sizeIndex, exposure);
		INFO_PRINTF4(_L("Frame Rate[%d], Size[%d] = %f"), rateIndex, sizeIndex, actualRate);

		TInt	expectedRate = 0;
		if ( GetIntFromConfig(aSection, KFrameRate(), expectedRate) )
			{
			TInt	actualRateInt=(TInt)actualRate;
			if (expectedRate != actualRateInt)
				{
				ERR_PRINTF3(_L("Frame rates do not match actual(%d) expected(%d)"), actualRateInt, expectedRate);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		TInt	actualFrameRates = 0;
		for(TInt i=0;i<iCameraInfo.iNumVideoFrameRatesSupported;i++)
			{
			for(TInt j=0;j<iCameraInfo.iNumVideoFrameSizesSupported;j++)
				{
				TReal32	actualRate = .0;
				iCamera->EnumerateVideoFrameRates(actualRate, i, format, j, exposure);
				INFO_PRINTF4(_L("Frame Rate[%d], Size[%d] = %f"), i, j, actualRate);
				if ( actualRate != 0 )
					{
					actualFrameRates++;
					}
				}
			}

		TInt	expectedFrameRates = 0;
		if ( GetIntFromConfig(aSection, KAmount(), expectedFrameRates) )
			{
			if( expectedFrameRates != actualFrameRates )
				{
				// expected number of frame rates does not match
				ERR_PRINTF3(_L("Number of frame rates do not match actual(%d) expected(%d)"), actualFrameRates, expectedFrameRates);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataCamera::DoCmdEnumerateVideoFrameSizes(const TDesC& aSection)
	{
	CCamera::TFormat	format = KDefaultFormat;
	GetParam(format, aSection);
	INFO_PRINTF3(_L("Camera format is %S (%d)"), &GetParamString(format), format);

	TInt	sizeIndex;
	if (!GetIntFromConfig(aSection, KSizeIndex(), sizeIndex))
		{
		TInt	actualFrameSizes = 0;
		for(TInt i=0; i<iCameraInfo.iNumVideoFrameSizesSupported; i++)
			{
			TSize	size;
			iCamera->EnumerateVideoFrameSizes(size, i, format);
			INFO_PRINTF4(_L("Size[%d] %d X %d"), i, size.iWidth, size.iHeight);
			if ( (size.iWidth > 0) && (size.iHeight > 0) )
				{
				actualFrameSizes++;
				}
			}

		TInt	expectedFrameSizes = 0;
		if ( GetIntFromConfig(aSection, KAmount(), expectedFrameSizes) )
			{
			if( expectedFrameSizes != actualFrameSizes )
				{
				// expected number of frame rates does not match
				ERR_PRINTF3(_L("Number of frame sizes do not match actual(%d) expected(%d)"), actualFrameSizes, expectedFrameSizes);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		TSize	size;
		iCamera->EnumerateVideoFrameSizes(size, sizeIndex, format);
		INFO_PRINTF4(_L("Size[%d] %d X %d"), sizeIndex, size.iWidth, size.iHeight);

		TInt	expectedWidth=0;
		if (GetIntFromConfig(aSection, KWidth(), expectedWidth))
			{
			if (expectedWidth != size.iWidth)
				{
				ERR_PRINTF3(_L("Widths do not match actual(%d) expected(%d)"), size.iWidth, expectedWidth);
				SetBlockResult(EFail);
				}
			}
		TInt	expectedHeight=0;
		if (GetIntFromConfig(aSection, KHeight(), expectedHeight))
			{
			if (expectedHeight != size.iHeight)
				{
				ERR_PRINTF3(_L("Heights do not match actual(%d) expected(%d)"), size.iHeight, expectedHeight);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataCamera::DoCmdFrameRate(const TDesC& aSection)
	{
	TReal32	actualFrameRate = iCamera->FrameRate();
	INFO_PRINTF2(_L("frame rate = %f"), actualFrameRate);

	TInt	expectedFrameRate = 0;
	if ( GetIntFromConfig(aSection, KFrameRate(), expectedFrameRate) )
		{
		if ( (TInt)actualFrameRate != expectedFrameRate )
			{
			ERR_PRINTF3(_L("FrameRate(%f) != expectedFrameRate(%f)"), (TReal32)expectedFrameRate, actualFrameRate);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdFramesPerBuffer(const TDesC& aSection)
	{
	TInt	actualFramesPerBuffer = iCamera->FramesPerBuffer();
	INFO_PRINTF2(_L("frames per buffer = %d"), actualFramesPerBuffer);

	TInt	expectedFramesPerBuffer = 0;
	if ( GetIntFromConfig(aSection, KFramesPerBuffer(), expectedFramesPerBuffer) )
		{
		if ( actualFramesPerBuffer != expectedFramesPerBuffer )
			{
			ERR_PRINTF3(_L("FramesPerBuffer(%d) != expectedFramesPerBuffer(%d)")
					, expectedFramesPerBuffer, actualFramesPerBuffer);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdGetFrameSize(const TDesC& aSection)
	{
	TSize	actualSize;
	iCamera->GetFrameSize(actualSize);
	INFO_PRINTF3(_L("Frame Size width = %d height = %d per buffer = %d"), actualSize.iWidth, actualSize.iHeight);

	TInt	expectedWidth = 0;
	if ( GetIntFromConfig(aSection, KWidth(), expectedWidth) )
		{
		if ( actualSize.iWidth != expectedWidth )
			{
			ERR_PRINTF3(_L("width(%d) != expectedWidth(%d)"), actualSize.iWidth, expectedWidth);
			SetBlockResult(EFail);
			}
		}

	TInt	expectedHeight = 0;
	if ( GetIntFromConfig(aSection, KHeight(), expectedHeight) )
		{
		if ( actualSize.iHeight != expectedHeight )
			{
			ERR_PRINTF3(_L("height(%d) != expectedHeight(%d)"), actualSize.iHeight, expectedHeight);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdHandle()
	{
	TInt	handle = iCamera->Handle();
	INFO_PRINTF2(_L("Camera handle = %d"), handle);
	}

void CT_MMDataCamera::DoCmdNewDuplicateL(const TDesC& aSection)
	{
	//	Get the camera object data dictionary name that needs duplicating
	TPtrC		originalCameraName;
	CCamera*	originalCamera=NULL;
	if ( !GetStringFromConfig(aSection, KCamera(), originalCameraName) )
		{
		ERR_PRINTF2(_L("No %S"), &KCamera());
		SetBlockResult(EFail);
		}
	else
		{
		originalCamera=static_cast<CCamera*>(GetDataObjectL(originalCameraName));
		}

	if ( originalCamera!=NULL )
		{
		// read the parameter that will tell us which overload to use
		TPtrC		observerDes;
		TBool		observerSpecified = GetStringFromConfig(aSection, KObserverInterface(), observerDes);
		TInt		err = KErrNone;
		CCamera*	camera=NULL;
		TInt		cameraHandle=originalCamera->Handle();
		GetIntFromConfig(aSection, KHandle(), cameraHandle);
		if ( observerSpecified && (observerDes==KMObserver2) )
			{
			MCameraObserver2*	observer2 = static_cast<MCameraObserver2*>(this);
			TRAP(err, camera = originalCamera->NewDuplicateL(*observer2, cameraHandle));
			}
		else // no parameter or param not MObserver2 - assume MObserver overload
			{
			MCameraObserver*	observer = static_cast<MCameraObserver*>(this);
			TRAP(err, camera = originalCamera->NewDuplicateL(*observer, cameraHandle));
			}
		CleanupStack::PushL(camera);
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CCamera::NewDuplicateL failed with error %d"), err);
			SetError(err);
			}
		else
			{
			INFO_PRINTF1(_L("CCamera::NewDuplicateL passed"));
			}
		delete iCamera;
		iCamera=camera;
		CleanupStack::Pop(camera);
		}
	}

void CT_MMDataCamera::DoCmdNewL(const TDesC& aSection)
	{

	delete iCamera;
	iCamera = NULL;

	TInt	cameraIndex = 0;
	GetIntFromConfig(aSection, KCameraIndex(), cameraIndex);
	TInt	priority=0;
	TPtrC	priorityDes;
	if(GetIntFromConfig(aSection, KPriorityPcnt(), priority))
		{
		TRAPD(err, iCamera = CCamera::NewL(*this, cameraIndex, priority));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CCamera::NewL failed with error %d"), err);
			SetError(err);
			}
		else
			{
			INFO_PRINTF1(_L("CCamera::NewL passed"));
			}
		}
	else // no priority specified use other overload
		{
		TRAPD(err, iCamera=CCamera::NewL(*this, cameraIndex));
		if(err!=KErrNone)
			{
			ERR_PRINTF2(_L("CCamera::NewL failed with error %d"), err);
			SetError(err);
			}
		}
	}

void CT_MMDataCamera::DoCmdPowerOff()
	{
	iCamera->PowerOff();
	INFO_PRINTF1(_L("PowerOff called ok"));
	}

void CT_MMDataCamera::DoCmdPowerOn(const TInt aAsyncErrorIndex)
	{
	iAsyncErrorIndex = aAsyncErrorIndex;
	iCamera->PowerOn();
	IncOutstanding();
	// callback completes in PowerOnComplete
	}

void CT_MMDataCamera::DoCmdPrepareImageCaptureL(const TDesC& aSection)
	{

	CCamera::TFormat	format = KDefaultFormat;
	GetParam(format, aSection);
	INFO_PRINTF3(_L("Camera format is %S (%d)"), &GetParamString(format), format);

	TInt	expected = KErrNone;
	if (!(iCameraInfo.iImageFormatsSupported & format))
		{
		expected=KErrNotSupported;
		INFO_PRINTF2(_L("Image format not supported. Expected = %d"), expected);
		}

	TInt	sizeIndex = 0; // maybe set to max - 1
	GetIntFromConfig(aSection, KSizeIndex(), sizeIndex);

	TRect	clipRect;
	TInt	err = KErrNone;
	// if a clip rect has been specified use it

	if(GetParam(clipRect, KClipTag, aSection))
		{
		if (!(iCameraInfo.iOptionsSupported & TCameraInfo::EImageClippingSupported))
			{
			expected=KErrNotSupported;
			INFO_PRINTF2(_L("Image clipping supported = FALSE!, Excpected = %d"), expected);
			}
		INFO_PRINTF1(_L("iCamera->PrepareImageCaptureL(format, sizeIndex, rect)"));
		TRAP(err, iCamera->PrepareImageCaptureL(format, sizeIndex, clipRect));
		}
	else
		{
		INFO_PRINTF1(_L("iCamera->PrepareImageCaptureL(format, sizeIndex)"));
		TRAP(err, iCamera->PrepareImageCaptureL(format, sizeIndex));
		}
	iPrepareCaptureOk=(err==KErrNone);

	if ( err!=expected )
		{
		ERR_PRINTF3(_L("CCamera::PrepareImageCaptureL failed with error %d. Expected %d"), err, expected);
		SetError(err);
		}
	else
		{
		iPrepareCaptureErr=err;
		}
	}

void CT_MMDataCamera::DoCmdPrepareVideoCaptureL(const TDesC& aSection)
	{

	CCamera::TFormat	format = KDefaultFormat;
	GetParam(format, aSection);
	INFO_PRINTF3(_L("Camera format is %S (%d)"), &GetParamString(format), format);

	TInt	sizeIndex = iCameraInfo.iNumVideoFrameSizesSupported-1;
	GetIntFromConfig(aSection, KSizeIndex(), sizeIndex);

	TInt	rateIndex = iCameraInfo.iNumVideoFrameRatesSupported-1;
	GetIntFromConfig(aSection, KRateIndex(), rateIndex);

	TInt	buffersToUse = KDefaultBuffersToUse;
	GetIntFromConfig(aSection, KBuffersInUse(), buffersToUse);

	TInt	framesPerBuffer = iCameraInfo.iMaxFramesPerBufferSupported;
	GetIntFromConfig(aSection, KFramesPerBuffer(), framesPerBuffer);

	TInt	err = KErrNone;
	TInt	expected = KErrNone;
	TRect	clipRect;
	// if a clip rect has been specified use it

	if(GetParam(clipRect, KClipTag, aSection))
		{
		if (!(iCameraInfo.iOptionsSupported & TCameraInfo::EVideoClippingSupported))
			{
			expected = KErrNotSupported;
			INFO_PRINTF2(_L("Video clipping supported = FALSE!, Excpected = %d"), expected);
			}
		INFO_PRINTF1(_L("iCamera->PrepareVideoCaptureL(format, sizeIndex, rateIndex, buffersToUse, framesPerBuffer, rect)"));
		TRAP(err, iCamera->PrepareVideoCaptureL(format, sizeIndex, rateIndex
											 , buffersToUse, framesPerBuffer, clipRect));
		}
	else
		{
		INFO_PRINTF1(_L(""));
		TRAP(err, iCamera->PrepareVideoCaptureL(format, sizeIndex, rateIndex, buffersToUse, framesPerBuffer));
		}

	if(err != expected)
		{
		ERR_PRINTF2(_L("CCamera::PrepareVideoCaptureL failed with error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataCamera::DoCmdRelease()
	{
	iCamera->Release();
	INFO_PRINTF1(_L("Release called ok"));
	}

void CT_MMDataCamera::DoCmdReserve(const TInt aAsyncErrorIndex)
	{
	iAsyncErrorIndex = aAsyncErrorIndex;
	iCamera->Reserve();
	IncOutstanding();
	// call back in ReserveComplete
	}

void CT_MMDataCamera::DoCmdStartVideoCapture(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	iAsyncErrorIndex = aAsyncErrorIndex;
	// start capturing video - calls FrameBufferReady when each buffer
	// has been filled with required number of frames set in PrepareVideoCaptureL
	iHasBitmapFile=GetStringFromConfig(aSection, KBitmapFile(), iBitmapFile);
	iCamera->StartVideoCapture();
	IncOutstanding();
	}

void CT_MMDataCamera::DoCmdStartViewFinderBitmapsL(const TDesC& aSection)
	{

	TInt	err = KErrNone;
	TSize	viewFinderSize(KDefaultViewFinderWidth, KDefaultViewFinderHeight);
	GetParam(viewFinderSize, KViewFinderTag, aSection);

	TRect	clipRect;
	if ( GetParam(clipRect, KClipTag, aSection) )
		{
		INFO_PRINTF1(_L("iCamera->StartViewFinderBitmapsL(viewFinderSize, rect)"));
		TRAP(err, iCamera->StartViewFinderBitmapsL(viewFinderSize, clipRect));
		}
	else
		{
		INFO_PRINTF1(_L("iCamera->StartViewFinderBitmapsL(viewFinderSize)"));
		TRAP(err, iCamera->StartViewFinderBitmapsL(viewFinderSize));
		}
	INFO_PRINTF2(_L("View Finder Width  %d"), viewFinderSize.iWidth);
	INFO_PRINTF2(_L("View Finder Height %d"), viewFinderSize.iHeight);
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CCamera::StartViewFinderBitmapsL failed with error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataCamera::DoCmdStartViewFinderDirectL(const TDesC& aSection)
	{
	TInt								err = KErrNone;

	// wsSession parameter
	TPtrC	wsSession;
	GetStringFromConfig(aSection, KWsSession(), wsSession);

	if ((wsSession == KInvalidWsSession) && iWsSessionConnected)
		{
		iWsSession.Close();
		iWsSessionConnected=EFalse;
		}

	// wsScreenDevice parameter
	TPtrC	wsScreenDevice;
	GetStringFromConfig(aSection, KWsScreenDevice(), wsScreenDevice);

	CWsScreenDevice*	scrDevice = NULL;
	if (wsScreenDevice != KInvalidScrDev)
		{
		scrDevice = iScrDevice;
		}

	// window parameter
	TPtrC	windowParam;
	GetStringFromConfig(aSection, KWindow(), windowParam);

	RWindow*	window = NULL;
	if (windowParam != KInvalidWindow)
		{
		window = iWindow;
		}

	TRect	screenRect(TSize(KDefaultViewFinderWidth, KDefaultViewFinderHeight));

	GetParam(screenRect, KScreenTag, aSection);

	TRect	clipRect;
	if(GetParam(clipRect, KClipTag, aSection))
		{
		INFO_PRINTF1(_L("iCamera->StartViewFinderDirectL(iWsSession, *scrDevice, *window, screenRect, clipRect)"));
		TRAP(err, iCamera->StartViewFinderDirectL(iWsSession, *scrDevice, *window, screenRect, clipRect));
		}
	else
		{
		INFO_PRINTF1(_L("iCamera->StartViewFinderDirectL(iWsSession, *scrDevice, *window, screenRect)"));
		TRAP(err, iCamera->StartViewFinderDirectL(iWsSession, *scrDevice, *window, screenRect));
		}

	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CCamera::StartViewFinderDirectL failed with error %d"), err);
		SetError(err);
		}

	// restore wsSession
	if (!iWsSessionConnected)
		{
		User::LeaveIfError(iWsSession.Connect());
		iWsSessionConnected=ETrue;
		}
	}

void CT_MMDataCamera::DoCmdStartViewFinderL(const TDesC& aSection)
	{

	CCamera::TFormat	format = KDefaultFormat;
	GetParam(format, aSection);
	INFO_PRINTF3(_L("Camera format is %S (%d)"), &GetParamString(format), format);
	TSize	viewFinderSize(KDefaultViewFinderWidth, KDefaultViewFinderHeight);
	GetParam(viewFinderSize, KViewFinderTag, aSection);

	TInt	err = KErrNone;
	TRect	clipRect;
	if(GetParam(clipRect, KClipTag, aSection))
		{
		INFO_PRINTF1(_L("iCamera->StartViewFinderL(format, viewFinderSize, clipRect)"));
		TRAP(err, iCamera->StartViewFinderL(format, viewFinderSize, clipRect));
		}
	else
		{
		INFO_PRINTF1(_L("iCamera->StartViewFinderL(format, viewFinderSize)"));
		TRAP(err, iCamera->StartViewFinderL(format, viewFinderSize));
		}

	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CCamera::StartViewFinderL failed with error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataCamera::DoCmdStopVideoCapture()
	{
	// stop the video capture
	iCamera->StopVideoCapture();
	}

void CT_MMDataCamera::DoCmdStopViewFinder()
	{
	// stop the view finder
	iCamera->StopViewFinder();
	}

void CT_MMDataCamera::DoCmdVideoCaptureActive(const TDesC& aSection)
	{

	TBool	actualVideoActive = iCamera->VideoCaptureActive();
	if(actualVideoActive)
		{
		INFO_PRINTF1(_L("VideoCaptureActive = true"));
		}
	else
		{
		INFO_PRINTF1(_L("VideoCaptureActive = false"));
		}

	TInt	expectedVideoActive;
	if(GetIntFromConfig(aSection, KVideoCaptureActive(), expectedVideoActive))
		{
		if((TBool)expectedVideoActive != actualVideoActive)
			{
			ERR_PRINTF3(_L("Video Active actual(%d) != expected(%d)")
						, (TInt)actualVideoActive, expectedVideoActive);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdViewFinderActive(const TDesC& aSection)
	{
	TBool	actualVfActive = iCamera->ViewFinderActive();
	if(actualVfActive)
		{
		INFO_PRINTF1(_L("ViewFinderActive = true"));
		}
	else
		{
		INFO_PRINTF1(_L("ViewFinderActive = false"));
		}
	TBool	expectedVfActive;
	if(GetBoolFromConfig(aSection, KViewFinderActive(), expectedVfActive))
		{
		if(expectedVfActive != actualVfActive)
			{
			ERR_PRINTF3(_L("View Finder Active actual(%d) != expected(%d)")
						, (TInt)actualVfActive, expectedVfActive);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataCamera::DoCmdDestructor()
	{
	DestroyData();
	}

TInt CT_MMDataCamera::GetDefaultDigitalZf()
/**
Utility helper function GetDefaultDigitalZf - Gets default digital zoom factor
@return TInt Default zoom factor(dzf)
@param aCamera - Camera for which the dzf is to be retrieved
@pre N/A
@post N/A
*/
	{
	TCameraInfo	cameraInfo;
	iCamera->CameraInfo(cameraInfo);
	return cameraInfo.iMaxDigitalZoom;
	}

void CT_MMDataCamera::ReserveComplete(TInt aError)
/**
aAsyncErrorIndex callback called when CCamera::Reserve completes
@internalComponent
@return void
@param aError Symbian error code on failure, KErrNone if camera reserved ok
@pre N/A
@post N/A
*/
	{
	if(aError == KErrNone )
		{
		INFO_PRINTF1(_L("ReserveComplete OK"));
		}
	else
		{
		ERR_PRINTF2(_L("ReserveComplete Error %d"), aError);
		SetAsyncError(iAsyncErrorIndex, aError);
		}

	DecOutstanding();
	}

void CT_MMDataCamera::PowerOnComplete(TInt aError)
/**
aAsyncErrorIndex callback called when CCamera::PowerOn completes
@internalComponent
@return void
@param aError Symbian error code on failure, KErrNone if camera powered on ok
@pre N/A
@post N/A
*/
	{
	if(aError == KErrNone )
		{
		INFO_PRINTF1(_L("PowerOnComplete OK"));
		}
	else
		{
		ERR_PRINTF2(_L("PowerOnComplete Error %d"), aError);
		SetAsyncError(iAsyncErrorIndex, aError);
		}

	DecOutstanding();
	}

void CT_MMDataCamera::ViewFinderFrameReady(CFbsBitmap& /*aFrame*/)
/**
Callback called periodically after a call to CCamera::StartViewFinderBitmapsL
@internalComponent
@return void
@param aFrame - a referenc to a CFbsBitmap object containing the viewfinder frame
@pre N/A
@post N/A
*/
	{
	// called periodically in response to calls to CCamera::StartViewFinderBitmaps
	}

void CT_MMDataCamera::ImageReady(CFbsBitmap* aBitmap, HBufC8* /*aData*/, TInt aError)
/**
Callback in response to a call to CCamera::CaptureImage
@internalComponent
@return void
@param aBitmap -  a pointer to a CFbsBitmap object in the format specified in the
previous call to CCamera::PrepareImageCapture
@param aData -  a pointer to a HBufC8 descriptor containing data in the format
specified in the previous call to CCaera::PrepareImageCapture
@param aError -  On failure contains a standard symbian error code KErrNone on success
@pre N/A
@post N/A
*/
	{
	if ( aBitmap!=NULL )
		{
		if ( iHasBitmapFile )
			{
			TInt	saveError = aBitmap->Save(iBitmapFile);
			if ( saveError != KErrNone )
				{
				ERR_PRINTF2(_L("aBitmap->Save() error %d"), saveError);
				SetBlockResult(EFail);
				}
			iHasBitmapFile=EFalse;
			}
		delete aBitmap;
		}
	if(aError == KErrNone )
		{
		INFO_PRINTF1(_L("ImageReady OK"));
		}
	else
		{
		ERR_PRINTF2(_L("ImageReady Error %d"), aError);
		//	If the capture failed with not supported then we should get a not ready error.
		if ( (iPrepareCaptureErr!=KErrNotSupported) && (aError!=KErrNotReady) )
			{
			SetAsyncError(iAsyncErrorIndex, aError);
			}
		}
	iCapturing=EFalse;
	DecOutstanding();
	}

void CT_MMDataCamera::FrameBufferReady(MFrameBuffer* aFrameBuffer, TInt aError)
/**
Passes a filled frame buffer to the client
@internalComponent
@released callback - called asynchronously when the buffer has been filled with the
required number of video frames by CCamera::StartVideoCapture
@return void
@param aFrameBuffer, pointer to a MFrameBuffer if successful otherwise NULL
@param aError - KErrNone if successful, standard symbian error code otherwise
@pre None
@post CClassExample object is now fully initialised
*/
	{
	// called asynchronously when a buffer has been filled by the required number of
	// frames by CCamera::StartVideoCapture

	if( (aError==KErrNone) && (aFrameBuffer!=NULL) )
		{
		INFO_PRINTF1(_L("FrameBufferReady OK"));
		if ( iHasBitmapFile )
			{
			CFbsBitmap*	bitmap = NULL;
			TRAPD(err, bitmap = aFrameBuffer->FrameL(aFrameBuffer->iIndexOfFirstFrameInBuffer));
			if ( err == KErrNone )
				{
				TInt	saveError = bitmap->Save(iBitmapFile);
				if ( saveError != KErrNone )
					{
					ERR_PRINTF2(_L("bitmap->Save() error %d"), saveError);
					SetBlockResult(EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("aFrameBuffer->FrameL() error %d"), err);
				SetAsyncError(iAsyncErrorIndex, aError);
				}
			delete bitmap;
			iHasBitmapFile=EFalse;
			}
		}
	else
		{
		if ( (aFrameBuffer==NULL) && (iPrepareCaptureOk) )
			{
			ERR_PRINTF1(_L("aFrameBuffer is NULL"));
			SetBlockResult(EFail);
			}
		if ( aError!=KErrNone )
			{
			ERR_PRINTF2(_L("FrameBufferReady Error %d"), aError);
			SetAsyncError(iAsyncErrorIndex, aError);
			}
		}
	if ( aFrameBuffer!=NULL )
		{
		aFrameBuffer->Release();
		}
	DecOutstanding();
	}

///////////////////////////////////////////////////////////////////
//from the MCameraObserver2 interface / / / / / / / / / / / / / / /
///////////////////////////////////////////////////////////////////

void CT_MMDataCamera::HandleEvent(const TECAMEvent &aEvent)
/**
Callback called when a camera event has completed.
@internalComponent
@return void
@param aParam1 A reference to a TECAMEvent. This can be completion of a
call to CCamera::Reserve() or a call to CCamera::PowerOn() or a notification
that the client has lost control of the camera. The event contains a uid
identifying the event and an accompanying error code
(KErrNone for the successful completion of a request).
The error will be KErrNotReady for a request that was made
out of the correct sequence. The error will be KErrAccessDenied for a Reserve()
request that failed because a higher priority client already controls the camera.
@pre N/A
@post N/A
*/
	{
	if(aEvent.iErrorCode == KErrNone )
		{
		INFO_PRINTF1(_L("HandleEvent OK"));
		}
	else
		{
		ERR_PRINTF2(_L("HandleEvent Error %d"), aEvent.iErrorCode);
		SetAsyncError(iAsyncErrorIndex, aEvent.iErrorCode);
		}

	DecOutstanding();
	}

void CT_MMDataCamera::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/, TInt aError)
/**
Callback - Notifies client of new view finder data. Called periodically in
response to the use of CCamera::StartViewFinderL().
@internalComponent
@return void
@param aCameraBuffer A reference to an MCameraBuffer if successful,
or NULL if not successful.
@param aParam2 KErrNone if successful, or an error code if not successful.
@pre N/A
@post N/A
*/
	{
	if(aError == KErrNone )
		{
		INFO_PRINTF1(_L("ViewFinderReady OK"));
		}
	else
		{
		ERR_PRINTF2(_L("ViewFinderReady Error %d"), aError);
		SetError(aError);
		}
	}

void CT_MMDataCamera::ImageBufferReady(MCameraBuffer& aCameraBuffer, TInt aError)
/**
Callback - Notifies the client of a new captured camera image.
Called asynchronously when CCamera::CaptureImage() completes.
@internalComponent
@return void
@param aCameraBuffer A reference to an MCameraBuffer if successful,
or NULL if not successful.
@param aParam2 KErrNone if successful, or an error code if not successful.
@pre N/A
@post N/A
*/
	{
	if(aError == KErrNone )
		{
		INFO_PRINTF1(_L("ImageBufferReady OK"));
		if ( iHasBitmapFile )
			{
			CFbsBitmap*	bitmap=NULL;
			TRAPD(err, CFbsBitmap& bitmapRef = aCameraBuffer.BitmapL(aCameraBuffer.iIndexOfFirstFrameInBuffer); bitmap=&bitmapRef);
			if ( err == KErrNone )
				{
				TInt	saveError = bitmap->Save(iBitmapFile);
				if ( saveError != KErrNone )
					{
					ERR_PRINTF2(_L("bitmap->Save() error %d"), saveError);
					SetBlockResult(EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("aCameraBuffer.BitmapL() error %d"), err);
				SetBlockResult(EFail);
				}
			iHasBitmapFile=EFalse;
			}
		}
	else
		{
		ERR_PRINTF2(_L("ImageBufferReady Error %d"), aError);
		SetAsyncError(iAsyncErrorIndex, aError);
		}
	DecOutstanding();
	}

void CT_MMDataCamera::VideoBufferReady(MCameraBuffer& aCameraBuffer, TInt aError)
/**
Callback - Notifies the client of new captured video. Called asynchronously and
periodically after CCamera::StartVideoCapture() is called. The buffer has been
filled with the required number of video frames specified by PrepareVideoCaptureL().
@internalComponent
@return void
@param aCameraBuffer A reference to an MCameraBuffer if successful,
or NULL if not successful.
@param aError KErrNone if successful, or an error code if not successful.
@pre N/A
@post N/A
*/
	{
	if(aError == KErrNone )
		{
		INFO_PRINTF1(_L("VideoBufferReady OK"));
		if ( iHasBitmapFile )
			{
			CFbsBitmap*	bitmap=NULL;
			TRAPD(err, CFbsBitmap& bitmapRef = aCameraBuffer.BitmapL(aCameraBuffer.iIndexOfFirstFrameInBuffer); bitmap=&bitmapRef);
			if ( err == KErrNone )
				{
				TInt	saveError = bitmap->Save(iBitmapFile);
				if ( saveError != KErrNone )
					{
					ERR_PRINTF2(_L("bitmap->Save() error %d"), saveError);
					SetBlockResult(EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("aCameraBuffer.BitmapL() error %d"), err);
				SetBlockResult(EFail);
				}
			iHasBitmapFile=EFalse;
			}
		}
	else
		{
		ERR_PRINTF2(_L("VideoBufferReady Error %d"), aError);
		SetAsyncError(iAsyncErrorIndex, aError);
		}
	DecOutstanding();
	}

TBool CT_MMDataCamera::GetParam(
		CCamera::TFormat&					aFormat,
		const TDesC&						aSection)
/**
Utility function - GetParam - Retrieves a ini file CCamera::TFormat parameter
@internalComponent
@return ETrue if parameter retrieved successfully EFalse otherwise
@param aFormat - CCamera::TFormat to be filled with data
@param aSection descriptor containing test step data used for formatting test store query
@pre None
@post When successfull, aFormat contains the camera format parameter
*/
	{
	TBool	returnValue = EFalse;
	TPtrC	formatDes;
	if(GetStringFromConfig(aSection, KCameraFormat(), formatDes))
		{
		if(ConvertToCameraFormat(formatDes, aFormat))
			{
			returnValue = ETrue;
			}
		else
			{
			TInt	tempValue;
			if(GetHexFromConfig(aSection, KCameraFormat(), tempValue))
				{
				aFormat = CCamera::TFormat(tempValue);
				returnValue = ETrue;
				}
			}
		}
	return returnValue;
	}

TBool CT_MMDataCamera::GetParam(
		CCamera::TExposure&					aExposure,
		const TDesC&						aSection)
/**
Utility function GetParam - Retrieves a ini file CCamera::TExposure parameter
@internalComponent
@return ETrue if parameter retrieved successfully EFalse otherwise
@param aExposure - CCamera::TExposure to be filled with data
@param aSection descriptor containing test step data used for formatting test store query
@pre None
@post When successfull, aExposure contains the camera exposure parameter
*/
	{
	TBool	returnValue = EFalse;
	TPtrC	exposureString;
	if(GetStringFromConfig(aSection, KExposure(), exposureString))
		{
		if(ConvertToCameraExposure(exposureString, aExposure))
			{
			returnValue = ETrue;
			}
		else
			{
			TInt	tempValue;
			if(GetIntFromConfig(aSection, KExposure(), tempValue))
				{
				aExposure = CCamera::TExposure(tempValue);
				returnValue = ETrue;
				}
			}
		}
	return returnValue;
	}

TBool CT_MMDataCamera::GetParam(
		CCamera::TFlash&					aFlash,
		const TDesC&						aSection)
/**
Utility function GetParam - Retrieves a ini file CCamera::TFlash parameter
@internalComponent
@return ETrue if parameter retrieved successfully EFalse otherwise
@param aFlash - CCamera::TFlash to be filled with data
@param aSection descriptor containing test step data used for formatting test store query
@pre None
@post When successfull, aFlash contains the camera flash parameter
*/
	{
	TBool	returnValue = EFalse;
	TPtrC	flashString;
	if(GetStringFromConfig(aSection, KFlash(), flashString))
		{
		if(ConvertToCameraFlash(flashString, aFlash))
			{
			returnValue = ETrue;
			}
		else
			{
			TInt	tempValue;
			if(GetIntFromConfig(aSection, KFlash(), tempValue))
				{
				aFlash = CCamera::TFlash(tempValue);
				returnValue = ETrue;
				}
			}
		}
	return returnValue;
	}

TBool CT_MMDataCamera::GetParam(
		CCamera::TWhiteBalance&				aWhiteBalance,
		const TDesC&						aSection)
/**
Utility function GetParam - Retrieves a ini file CCamera::TWhiteBalance parameter
@internalComponent
@return ETrue if parameter retrieved successfully EFalse otherwise
@param aWhiteBalance - CCamera::TWhiteBalance to be filled with data
@param aSection descriptor containing test step data used for formatting test store query
@pre None
@post When successfull, aWhiteBalance contains the camera white balance parameter
*/
	{
	TBool	returnValue = EFalse;
	TPtrC	whiteBalanceString;
	if(GetStringFromConfig(aSection, KWhiteBalance(), whiteBalanceString))
		{
		if(ConvertToCameraWhiteBalance(whiteBalanceString, aWhiteBalance))
			{
			returnValue = ETrue;
			}
		else
			{
			TInt	tempValue;
			if(GetIntFromConfig(aSection, KWhiteBalance(), tempValue))
				{
				aWhiteBalance = CCamera::TWhiteBalance(tempValue);
				returnValue = ETrue;
				}
			}
		}
	return returnValue;
	}

TBool CT_MMDataCamera::GetParam(
		TRect&								aRect,
		const TDesC&						aTag,
		const TDesC&						aSection)
/**
Utility function GetParam - Retrieves a ini file TRect parameter
@internalComponent
@return ETrue if parameter retrieved successfully EFalse otherwise
@param aRect - TRect to be filled with data
@param aTag - descriptor tag for rectangle
@param aSection descriptor containing test step data used for formatting test store query
@pre None
@post When successfull, aRect contains the rect parameter
*/
	{
	TPoint topLeft;
	TPoint bottomRight;

	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TRect	tmp(aRect);
	tempStore.Format(KRectTop(), &aTag);
	GetIntFromConfig(aSection, tempStore, aRect.iTl.iY);

	tempStore.Format(KRectBottom(), &aTag);
	GetIntFromConfig(aSection, tempStore, aRect.iBr.iY);

	tempStore.Format(KRectLeft(), &aTag);
	GetIntFromConfig(aSection, tempStore, aRect.iTl.iX);

	tempStore.Format(KRectRight(), &aTag);
	GetIntFromConfig(aSection, tempStore, aRect.iBr.iX);

	return tmp != aRect;
	}

TBool CT_MMDataCamera::GetParam(
		TSize&								aSize,
		const TDesC&						aTag,
		const TDesC&						aSection)
/**
Utility function GetParam - Retrieves a ini file TRect parameter
@internalComponent
@return ETrue if parameter retrieved successfully EFalse otherwise
@param aSize - TSize to be filled with data
@param aTag - descriptor tag for size
@param aSection descriptor containing test step data used for formatting test store query
@pre None
@post When successfull, aRect contains the rect parameter
*/
	{

	TBuf<KMaxTestExecuteCommandLength> tempStore;

	TSize	tmp(aSize);
	tempStore.Format(KSizeWidth(), &aTag);
	GetIntFromConfig(aSection, tempStore, aSize.iWidth);

	tempStore.Format(KSizeHeight(), &aTag);
	GetIntFromConfig(aSection, tempStore, aSize.iHeight);

	return tmp != aSize;
	}

TBool CT_MMDataCamera::GetParam(
		TCameraInfo::TCameraOrientation&	aOrientation,
		const TDesC&						aSection)
/**
Utility function - GetParam - Retrieves a ini file TCameraInfo::TCameraOrientation parameter
@internalComponent
@return ETrue if parameter retrieved successfully EFalse otherwise
@param aOrientation - CameraInfo::TCameraOrientation to be filled with data
@param aSection descriptor containing test step data used for formatting test store query
@pre None
@post When successfull, aOrientation contains the camera format parameter
*/
	{
	TPtrC	orientationDes;
	TBool	ret = GetStringFromConfig(aSection, KOrientation(), orientationDes);
	if(ret)
		{
		ret = ConvertToOrientation(orientationDes, aOrientation);
		}
	return ret;
	}

TBool CT_MMDataCamera::ConvertToCameraFormat(const TDesC& aFormatDes, CCamera::TFormat& aFormat)
/**
Retrieves a CCamera::TFormat value represented by a human readable string
@internalComponent
@released static helper function
@return ETrue if value retreieved ok, otherwise EFalse
@param aFormatDes a const descriptor containing the human readable string
@param aFormat a CCamera::TFormat reference in which the relevant value will be placed
@pre N/A (static)
@post N/A (static)
*/
	{
	TBool	ret = ETrue;
	if(aFormatDes == KFormatMonochrome)
		{
		aFormat = CCamera::EFormatMonochrome;
		}
	else if(aFormatDes == KFormat16bitRGB444)
		{
		aFormat = CCamera::EFormat16bitRGB444;
		}
	else if(aFormatDes == KFormat16BitRGB565)
		{
		aFormat = CCamera::EFormat16BitRGB565;
		}
	else if(aFormatDes == KFormat32BitRGB888)
		{
		aFormat = CCamera::EFormat32BitRGB888;
		}
	else if(aFormatDes == KFormatJpeg)
		{
		aFormat = CCamera::EFormatJpeg;
		}
	else if(aFormatDes == KFormatExif)
		{
		aFormat = CCamera::EFormatExif;
		}
	else if(aFormatDes == KFormatFbsBitmapColor4K)
		{
		aFormat = CCamera::EFormatFbsBitmapColor4K;
		}
	else if(aFormatDes == KFormatFbsBitmapColor64K)
		{
	 	aFormat = CCamera::EFormatFbsBitmapColor64K;
		}
	else if(aFormatDes == KFormatFbsBitmapColor16M)
		{
		aFormat = CCamera::EFormatFbsBitmapColor16M;
		}
	else if(aFormatDes == KFormatUserDefined)
		{
		aFormat = CCamera::EFormatUserDefined;
		}
	else if(aFormatDes == KFormatYUV420Interleaved)
		{
		aFormat = CCamera::EFormatYUV420Interleaved;
		}
	else if(aFormatDes == KFormatYUV420Planar)
		{
		aFormat = CCamera::EFormatYUV420Planar;
		}
	else if(aFormatDes == KFormatYUV422)
		{
		aFormat = CCamera::EFormatYUV422;
		}
	else if(aFormatDes == KFormatYUV422Reversed)
		{
		aFormat = CCamera::EFormatYUV422Reversed;
		}
	else if(aFormatDes == KFormatYUV444)
		{
		aFormat = CCamera::EFormatYUV444;
		}
	else if(aFormatDes == KFormatYUV420SemiPlanar)
		{
		aFormat = CCamera::EFormatYUV420SemiPlanar;
		}
	else if(aFormatDes == KFormatFbsBitmapColor16MU)
		{
		aFormat = CCamera::EFormatFbsBitmapColor16MU;
		}
	else
		{
		ret = EFalse;
		}
	return ret;
	}

TBool CT_MMDataCamera::ConvertToCameraExposure(const TDesC& aExposureDes, CCamera::TExposure& aExposure)
/**
Retrieves a CCamera::TExposure value represented by a human readable string
@internalComponent
@released static helper function
@return ETrue if value retreieved ok, otherwise EFalse
@param aExposureDes a const descriptor containing the human readable string
@param aExposure a CCamera::TExposure reference in which the relevant value will be placed
@pre N/A (static)
@post N/A (static)
*/
	{
	TBool	ret = ETrue;
	if(aExposureDes == KExposureAuto)
		{
		aExposure = CCamera::EExposureAuto;
		}
	else if(aExposureDes == KExposureNight)
		{
		aExposure = CCamera::EExposureNight;
		}
	else if(aExposureDes == KExposureBacklight)
		{
		aExposure = CCamera::EExposureBacklight;
		}
	else if(aExposureDes == KExposureCenter)
		{
		aExposure = CCamera::EExposureCenter;
		}
	else if(aExposureDes == KExposureSport)
		{
		aExposure = CCamera::EExposureSport;
		}
	else if(aExposureDes == KExposureVeryLong)
		{
		aExposure = CCamera::EExposureVeryLong;
		}
	else if(aExposureDes == KExposureSnow)
		{
		aExposure = CCamera::EExposureSnow;
		}
	else if(aExposureDes == KExposureBeach)
		{
		aExposure = CCamera::EExposureBeach;
		}
	else if(aExposureDes == KExposureProgram)
		{
		aExposure = CCamera::EExposureProgram;
		}
	else if(aExposureDes == KExposureAperturePriority)
		{
		aExposure = CCamera::EExposureAperturePriority;
		}
	else if(aExposureDes == KExposureShutterPriority)
		{
		aExposure = CCamera::EExposureShutterPriority;
		}
	else if(aExposureDes == KExposureManual)
		{
		aExposure = CCamera::EExposureManual;
		}
	else if(aExposureDes == KExposureSuperNight)
		{
		aExposure = CCamera::EExposureSuperNight;
		}
	else if(aExposureDes == KExposureInfra)
		{
		aExposure = CCamera::EExposureInfra;
		}
	else
		{
		ret = EFalse;
		}
	return ret;
	}

TBool CT_MMDataCamera::ConvertToCameraFlash(const TDesC& aFlashDes, CCamera::TFlash& aFlash)
/**
Retrieves a CCamera::TFlash value represented by a human readable string
@internalComponent
@released static helper function
@return ETrue if value retreieved ok, otherwise EFalse
@param aFlashDes a const descriptor containing the human readable string
@param aFlash a CCamera::TFlash reference in which the relevant value will be placed
@pre N/A (static)
@post N/A (static)
*/
	{
	TBool	ret = ETrue;
	if(aFlashDes == KFlashNone)
		{
	 	aFlash = CCamera::EFlashNone;
		}
	else if(aFlashDes == KFlashAuto)
		{
		aFlash = CCamera::EFlashAuto;
		}
	else if(aFlashDes == KFlashForced)
		{
	 	 aFlash = CCamera::EFlashForced;
		}
	else if(aFlashDes == KFlashFillIn)
		{
		aFlash = CCamera::EFlashFillIn;
		}
	else if(aFlashDes == KFlashRedEyeReduce)
		{
		aFlash = CCamera::EFlashRedEyeReduce;
		}
	else if(aFlashDes == KFlashSlowFrontSync)
		{
		aFlash = CCamera::EFlashSlowFrontSync;
		}
	else if(aFlashDes == KFlashSlowRearSync)
		{
		aFlash = CCamera::EFlashSlowRearSync;
		}
	else if(aFlashDes == KFlashManual)
		{
	 	aFlash = CCamera::EFlashManual;
		}
	else
		{
		ret = EFalse;
		}
	return ret;
	}

TBool CT_MMDataCamera::ConvertToCameraWhiteBalance(const TDesC& aWhiteBalanceDes, CCamera::TWhiteBalance& aWhiteBalance)
/**
Retrieves a CCamera::TWhiteBalance value represented by a human readable string
@internalComponent
@released static helper function
@return ETrue if value retreieved ok, otherwise EFalse
@param aWhiteBalenceDes a const descriptor containing the human readable string
@param aWhiteBalance a CCamera::TWhiteBalance reference in which the relevant value will be placed
@pre N/A (static)
@post N/A (static)
*/
	{
	TBool	ret = ETrue;
	if(aWhiteBalanceDes == KWBAuto)
		{
		aWhiteBalance = CCamera::EWBAuto;
		}
	else if(aWhiteBalanceDes == KWBCloudy)
		{
		aWhiteBalance = CCamera::EWBCloudy;
		}
	else if(aWhiteBalanceDes == KWBTungsten)
		{
		aWhiteBalance = CCamera::EWBTungsten;
		}
	else if(aWhiteBalanceDes == KWBFluorescent)
		{
		aWhiteBalance = CCamera::EWBFluorescent;
		}
	else if(aWhiteBalanceDes == KWBFlash)
		{
		aWhiteBalance = CCamera::EWBFlash;
		}
	else if(aWhiteBalanceDes == KWBSnow)
		{
		aWhiteBalance = CCamera::EWBSnow;
		}
	else if(aWhiteBalanceDes == KWBBeach)
		{
		aWhiteBalance = CCamera::EWBBeach;
		}
	else if(aWhiteBalanceDes == KWBManual)
		{
		aWhiteBalance = CCamera::EWBManual;
		}
	else
		{
		ret = EFalse;
		}
	return ret;
	}

const TDesC& CT_MMDataCamera::GetParamString(const CCamera::TWhiteBalance aWb)
/**
Retrieves the human readable equivalent for a CCamera::TWhiteBalance value
@internalComponent
@released static helper function
@return an unmodifiable descriptor containing the human readable string
@param aWb a CCamera::TWhiteBalance value
@pre N/A (static)
@post N/A (static)
*/
	{
	switch ( aWb )
		{
	case CCamera::EWBAuto:
		return KWBAuto();
	case CCamera::EWBCloudy:
		return KWBCloudy();
	case CCamera::EWBTungsten:
		return KWBTungsten();
	case CCamera::EWBFluorescent:
		return KWBFluorescent();
	case CCamera::EWBFlash:
		return KWBFlash();
	case CCamera::EWBSnow:
		return KWBSnow();
	case CCamera::EWBBeach:
		return KWBBeach();
	case CCamera::EWBManual:
		return KWBManual();
	default:
		break;
		};
	return KUnknown();
	}

const TDesC& CT_MMDataCamera::GetParamString(const CCamera::TFlash aFlash)
/**
Retrieves the human readable equivalent for a CCamera::TFlash value
@internalComponent
@released static helper function
@return an unmodifiable descriptor containing the human readable string
@param aFlash a CCamera::TFlash value
@pre N/A (static)
@post N/A (static)
*/
	{
	switch ( aFlash )
		{
	case CCamera::EFlashNone:
		return KFlashNone();
	case CCamera::EFlashAuto:
		return KFlashAuto();
	case CCamera::EFlashForced:
		return KFlashForced();
	case CCamera::EFlashFillIn:
		return KFlashFillIn();
	case CCamera::EFlashRedEyeReduce:
		return KFlashRedEyeReduce();
	case CCamera::EFlashSlowFrontSync:
		return KFlashSlowFrontSync();
	case CCamera::EFlashSlowRearSync:
		return KFlashSlowRearSync();
	case CCamera::EFlashManual:
		return KFlashManual();
	default:
		break;
		};
	return KUnknown();
	}

const TDesC& CT_MMDataCamera::GetParamString(const CCamera::TExposure aExposure)
/**
Retrieves the human readable equivalent for a CCamera::TExposure value
@internalComponent
@released static helper function
@return an unmodifiable descriptor containing the human readable string
@param aExposure a CCamera::TExposure value
@pre N/A (static)
@post N/A (static)
*/
	{
	switch ( aExposure )
		{
	case CCamera::EExposureAuto:
		return KExposureAuto();
	case CCamera::EExposureNight:
		return KExposureNight();
	case CCamera::EExposureBacklight:
		return KExposureBacklight();
	case CCamera::EExposureCenter:
		return KExposureCenter();
	case CCamera::EExposureSport:
		return KExposureSport;
	case CCamera::EExposureVeryLong:
		return KExposureVeryLong();
	case CCamera::EExposureSnow:
		return KExposureSnow();
	case CCamera::EExposureBeach:
		return KExposureBeach();
	case CCamera::EExposureProgram:
		return KExposureProgram();
	case CCamera::EExposureAperturePriority:
		return KExposureAperturePriority;
	case CCamera::EExposureShutterPriority:
		return KExposureShutterPriority;
	case CCamera::EExposureManual:
		return KExposureManual();
	case CCamera::EExposureSuperNight:
		return KExposureSuperNight();
	case CCamera::EExposureInfra:
		return KExposureInfra;
	default:
		break;
		};
	return KUnknown();
	}

const TDesC& CT_MMDataCamera::GetParamString(const CCamera::TFormat aFormat)
/**
Retrieves the human readable equivalent for a CCamera::TFormat value
@internalComponent
@released static helper function
@return an unmodifiable descriptor containing the human readable string
@param aExposure a CCamera::TFormat value
@pre N/A (static)
@post N/A (static)
*/
	{
	switch ( aFormat )
		{
	case CCamera::EFormatMonochrome:
		return KFormatMonochrome;
	case CCamera::EFormat16bitRGB444:
		return KFormat16bitRGB444;
	case CCamera::EFormat16BitRGB565:
		return KFormat16BitRGB565;
	case CCamera::EFormat32BitRGB888:
		return KFormat32BitRGB888;
	case CCamera::EFormatJpeg:
		return KFormatJpeg;
	case CCamera::EFormatExif:
		return KFormatExif;
	case CCamera::EFormatFbsBitmapColor4K:
		return KFormatFbsBitmapColor4K;
	case CCamera::EFormatFbsBitmapColor64K:
		return KFormatFbsBitmapColor64K;
	case CCamera::EFormatFbsBitmapColor16M:
		return KFormatFbsBitmapColor16M;
	case CCamera::EFormatUserDefined:
		return KFormatUserDefined;
	case CCamera::EFormatYUV420Interleaved:
		return KFormatYUV420Interleaved;
	case CCamera::EFormatYUV420Planar:
		return KFormatYUV420Planar;
	case CCamera::EFormatYUV422:
		return KFormatYUV422;
	case CCamera::EFormatYUV422Reversed:
		return KFormatYUV422Reversed;
	case CCamera::EFormatYUV444:
		return KFormatYUV444;
	case CCamera::EFormatYUV420SemiPlanar:
		return KFormatYUV420SemiPlanar;
	case CCamera::EFormatFbsBitmapColor16MU:
		return KFormatFbsBitmapColor16MU;
	default:
		break;
		};
	return KUnknown();
	}

TBool CT_MMDataCamera::ConvertToOrientation(
		const TDesC&						aOrientationDes,
		TCameraInfo::TCameraOrientation&	aOrientation)
/**
Retrieves a TCameraOrientation value represented by a human readable string
@internalComponent
@released static helper function
@return ETrue if value retreieved ok, otherwise EFalse
@param aOrientationDes a const descriptor containing the human readable string
@param aOrientation a TCameraOrientation reference in which the relevant value will be placed
@pre N/A (static)
@post N/A (static)
*/
	{
	TBool ret = ETrue;
	if ( aOrientationDes == KOrientationOutwards )
		{
		aOrientation = TCameraInfo::EOrientationOutwards;
		}
	else if(aOrientationDes == KOrientationInwards)
		{
		aOrientation = TCameraInfo::EOrientationInwards;
		}
	else if(aOrientationDes == KOrientationMobile)
		{
		aOrientation = TCameraInfo::EOrientationMobile;
		}
	else if(aOrientationDes == KOrientationUnknown)
		{
		aOrientation = TCameraInfo::EOrientationUnknown;
		}
	else
		{
		ret = EFalse;
		}
	return ret;
	}

const TDesC& CT_MMDataCamera::GetParamString(TCameraInfo::TCameraOrientation aOrientation)
/**
Retrieves the human readable equivalent for a TCameraInfo::TCameraOrientation value
@internalComponent
@released static helper function
@return an unmodifiable descriptor containing the human readable string
@param aOrientation CameraInfo::TCameraOrientation
@pre N/A (static)
@post N/A (static)
*/
	{
	switch ( aOrientation )
		{
	case TCameraInfo::EOrientationOutwards:
		return KOrientationOutwards;
	case TCameraInfo::EOrientationInwards:
		return KOrientationInwards;
	case TCameraInfo::EOrientationMobile:
		return KOrientationMobile;
	case TCameraInfo::EOrientationUnknown:
		return KOrientationUnknown;
	default:
		break;
		};
	return KUnknown();
	}
