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

#include <eikenv.h>
#include <testcameraapp.rsg>
#include "TestCameraApp.h"
#include "TestCameraApp.hrh"
#include "TestCamera.h"

const TInt KMaxErrLength=50;
_LIT(KErrorFormat,"Error - %d (%S)");

//
//  CTCamAppUi
//

void CTCamAppUi::ConstructL()
	{
	CEikAppUi::ConstructL();
	iAppView = new(ELeave) CTCamAppView;
	iAppView->ConstructL(ClientRect());
	AddToStackL(iAppView);
	
	// The plugin relies on a specific file existing
	// hence this has to be created.
	CreateDependencyFileL();

	iCamera = CCamera::NewL(*this,0,0);
	
	iCameraAdvSet = CCamera::CCameraAdvancedSettings::NewL(*iCamera);
	iCameraPresets = CCamera::CCameraPresets::NewL(*iCamera);
	iCameraImgProc = CCamera::CCameraImageProcessing::NewL(*iCamera);
	iCameraSnapshot = CCamera::CCameraSnapshot::NewL(*iCamera);
	
	iCamera->Reserve();
	}

CTCamAppUi::~CTCamAppUi()
	{
	if (iCamera!=NULL)
		iCamera->Release();
	
	delete iCameraSnapshot;
	delete iCameraImgProc;	
	delete iCameraPresets;
	delete iCameraAdvSet;
	delete iCamera;

	// as the application is being closed, delete the file
	// so that the plugin will not be reactivated accidentaly
	RemoveDependencyFile();
	
	if (iAppView)
		{
		RemoveFromStack(iAppView);
		delete iAppView;
		}
	}

void CTCamAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
	case ETCamCmdPowerOn:
		iCamera->PowerOn();
		break;
	case ETCamCmdPowerOff:
		iCamera->PowerOff();
		break;
	case ETCamCmdViewFinder:
		ViewFinderL();
		break;
	case ETCamCmdCaptureImage:
		SingleShotL();
		break;
	case ETCamCmdBurstMode:
		BurstModeL();
		break;
	case ETCamCmdCaptureVideo:
		CaptureVideoL();
		break;
	case ETCamCmdIncBrightness:
		iCamera->SetBrightnessL(iCamera->Brightness() + 10);
		break;
	case ETCamCmdIncContrast:
		iCamera->SetContrastL(iCamera->Contrast() + 10);
		break;
	case ETCamCmdDecBrightness:
		iCamera->SetBrightnessL(iCamera->Brightness() - 10);
		break;
	case ETCamCmdDecContrast:
		iCamera->SetContrastL(iCamera->Contrast() - 10);
		break;
	case ETCamCmdResetBrightness:
		iCamera->SetBrightnessL(0);
		break;
	case ETCamCmdResetContrast:
		iCamera->SetContrastL(0);
		break;
	case ETCamCmdIncNumBurstImages:
		iCameraAdvSet->SetBurstImages(iCameraAdvSet->BurstImages() + 1);
		break;
	case ETCamCmdDecNumBurstImages:
		iCameraAdvSet->SetBurstImages(iCameraAdvSet->BurstImages() - 1);
		break;
	case ETCamCmdResetNumBurstImages:
		iCameraAdvSet->SetBurstImages(4);
		break;
		
	case ETCamCmdFactoryPreset:
		iCameraPresets->SetPreset(KUidECamPresetFactoryDefault);
		break;
	
	case ETCamCmdOutdoorPreset:
		iCameraPresets->SetPreset(KUidECamPresetOutdoor);
		break;	
		
	case ETCamCmdOutdoorSportPreset:
		iCameraPresets->SetPreset(KUidECamPresetOutdoorSport);
		break;
	
	case ETCamCmdFogPreset:
		iCameraPresets->SetPreset(KUidECamPresetFog);
		break;	

	case ETCamCmdNightPartyIndoorPreset:
		iCameraPresets->SetPreset(KUidECamPresetNightPartyIndoor);
		break;
	
	case ETCamCmdTextDocumentPreset:
		iCameraPresets->SetPreset(KUidECamPresetTextDocument);
		break;	
		
	case ETCamCmdImgProcIncBrightness:
		IncImgProcAdjustmentL(KUidECamEventImageProcessingAdjustBrightness);
		break;
		
	case ETCamCmdImgProcDecBrightness:
		DecImgProcAdjustmentL(KUidECamEventImageProcessingAdjustBrightness);
		break;
		
	case ETCamCmdImgProcIncContrast:
		IncImgProcAdjustmentL(KUidECamEventImageProcessingAdjustContrast);
		break;
		
	case ETCamCmdImgProcDecContrast:
		DecImgProcAdjustmentL(KUidECamEventImageProcessingAdjustContrast);
		break;
		
	case ETCamCmdEnableSnapshot:
		EnableSnapshotL();
		break;
		
	case ETCamCmdDisableSnapshot:
		DisableSnapshot();
		break;
	
	case EEikCmdExit:
		Exit();
		break;
		}
	}

void CTCamAppUi::ViewFinderL()
	{
	TSize imageSize;
	iCamera->EnumerateCaptureSizes(imageSize,1,CCamera::EFormatFbsBitmapColor16M);
	const TPoint pos = iAppView->DrawBorders(imageSize);
	TRect screenRect(pos,imageSize);
	TRect clipRect;

	if (!iCamera->ViewFinderActive())
		{
		iCamera->StartViewFinderDirectL(iCoeEnv->WsSession(),*iCoeEnv->ScreenDevice(),*iAppView->DrawableWindow(),screenRect,clipRect);		
		}
	else
		{
		iCamera->StopViewFinder();
		}
	}

void CTCamAppUi::BurstModeL()
	{
	if (!iCameraAdvSet)
		{
		User::Leave(KErrNotSupported);
		}
	
	if(CCamera::CCameraAdvancedSettings::EDriveModeBurst & iCameraAdvSet->SupportedDriveModes() )
		{
		iCameraAdvSet->SetDriveMode(CCamera::CCameraAdvancedSettings::EDriveModeBurst);
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}
	
void CTCamAppUi::EnableSnapshotL()
	{
	if(!iCameraSnapshot->IsSnapshotActive())
		{
		TSize size;
		iCamera->EnumerateCaptureSizes(size,1,CCamera::EFormatFbsBitmapColor16M);
		if(iCameraSnapshot->SupportedFormats() & CCamera::EFormatFbsBitmapColor4K)
			{
			iCameraSnapshot->PrepareSnapshotL(CCamera::EFormatFbsBitmapColor4K, TPoint(0,0), size, KRgbWhite, EFalse);
			iCameraSnapshot->StartSnapshot();
			}
		}
	}
	
void CTCamAppUi::DisableSnapshot()
	{
	if(iCameraSnapshot->IsSnapshotActive())
		{
		iCameraSnapshot->StopSnapshot();
		}
	}
	
void CTCamAppUi::SingleShotL()
	{
	if (!iCameraAdvSet)
		{
		User::Leave(KErrNotSupported);
		}
	
	if(CCamera::CCameraAdvancedSettings::EDriveModeSingleShot & iCameraAdvSet->SupportedDriveModes() )
		{
		iCameraAdvSet->SetDriveMode(CCamera::CCameraAdvancedSettings::EDriveModeSingleShot);
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

void CTCamAppUi::CaptureImageL()
	{
	TSize size;
	iCamera->EnumerateCaptureSizes(size,1,CCamera::EFormatFbsBitmapColor16M);
	iCamera->PrepareImageCaptureL(CCamera::EFormatFbsBitmapColor16M,1);
	iCamera->CaptureImage();
	}

void CTCamAppUi::CaptureVideoL()
	{
	if (!iCamera->VideoCaptureActive())
		{
		iCameraSnapshot->StopSnapshot();
		
		TSize size;
		TReal32 rate = 0;
		iCamera->EnumerateVideoFrameSizes(size,0,CCamera::EFormatFbsBitmapColor16M);
		iCamera->EnumerateVideoFrameRates(rate,0,CCamera::EFormatFbsBitmapColor16M,0);
		iCamera->PrepareVideoCaptureL(CCamera::EFormatFbsBitmapColor16M,0,0,2,1);
		iCamera->StartVideoCapture();
		}
	else
		{
		iCamera->StopVideoCapture();
		}	
	}
	
void CTCamAppUi::IncImgProcAdjustmentL(TUid aTransformation)
	{
	if(!iCameraImgProc)	
		{
		User::Leave(KErrNotSupported);
		}
	
	RArray<TUid> supportedTransformations;
	CleanupClosePushL(supportedTransformations);
	iCameraImgProc->GetSupportedTransformationsL(supportedTransformations);
	
	TInt error = supportedTransformations.Find(aTransformation);
	CleanupStack::PopAndDestroy(); 

	if(error == KErrNotFound)
		{
		User::Leave(error);
		}
		
	TInt valuetx;
	User::LeaveIfError(iCameraImgProc->GetTransformationValue(aTransformation, valuetx));
	
	iCameraImgProc->SetTransformationValue(aTransformation, valuetx + 10);
	}
	
void CTCamAppUi::DecImgProcAdjustmentL(TUid aTransformation)
	{
	if(!iCameraImgProc)	
		{
		User::Leave(KErrNotSupported);
		}
	
	RArray<TUid> supportedTransformations;
	CleanupClosePushL(supportedTransformations);
	iCameraImgProc->GetSupportedTransformationsL(supportedTransformations);
	
	TInt error = supportedTransformations.Find(aTransformation);
	CleanupStack::PopAndDestroy(); 

	if(error == KErrNotFound)
		{
		User::Leave(error);
		}
		
	TInt valuetx;
	User::LeaveIfError(iCameraImgProc->GetTransformationValue(aTransformation, valuetx));
	
	iCameraImgProc->SetTransformationValue(aTransformation, valuetx - 10);
	}
	
void CTCamAppUi::ReserveComplete(TInt /*aError*/)
	{
	}

void CTCamAppUi::PowerOnComplete(TInt /*aError*/)
	{
	}

void CTCamAppUi::ViewFinderFrameReady(CFbsBitmap& /*aFrame*/)
	{
	}

void CTCamAppUi::ImageReady(CFbsBitmap* aBitmap,HBufC8* /*aData*/,TInt aError)
	{
	TBuf<KMaxErrLength> msgBuffer;
	_LIT(KErrMessage,"CTCamAppUi::ImageReady");
	if (aBitmap)
		{
		iAppView->DrawImage(aBitmap);
		delete aBitmap;
		}
	if (aError)
		{
		msgBuffer.Format(KErrorFormat,aError,&KErrMessage);
		iEikonEnv->InfoMsg(msgBuffer);
		}
	}

void CTCamAppUi::FrameBufferReady(MFrameBuffer* aFrameBuffer,TInt aError)
	{
	TBuf<KMaxErrLength> msgBuffer;
	_LIT(KErrMessage,"CTCamAppUi::FrameBufferReady");
	if (aError)
		{
		msgBuffer.Format(KErrorFormat,aError,&KErrMessage);
		iEikonEnv->InfoMsg(msgBuffer);
		return;
		}
	TRAPD(error,iAppView->DrawImage(aFrameBuffer->FrameL(0)));
	aFrameBuffer->Release();
	if (error)
		{
		msgBuffer.Format(KErrorFormat,error,&KErrMessage);
		iEikonEnv->InfoMsg(msgBuffer);
		}
	}
	
void CTCamAppUi::HandleEvent(const TECAMEvent& aEvent)
	{
	if(!TECAMEvent2::IsEventEncapsulationValid(aEvent))
		{
		HandleEvent1(aEvent);
		}
	else
		{
		HandleEvent2(static_cast<const TECAMEvent2&>(aEvent));
		}
	}
	
void CTCamAppUi::HandleEvent2(const TECAMEvent2& /*aEvent2*/)
	{
	
	}
	
void CTCamAppUi::HandleEvent1(const TECAMEvent& aEvent)
	{
	TBuf<KMaxErrLength> msgBuffer;
	
	switch(aEvent.iEventType.iUid)
		{
		case KUidECamEventCameraSettingDriveModeUidValue:
			{
			if(!aEvent.iErrorCode)
				{
				TRAPD(error,CaptureImageL());	
				if(error)
					{
					_LIT(KErrMessage,"CaptureImage");
					msgBuffer.Format(KErrorFormat,error,&KErrMessage);
					iEikonEnv->InfoMsg(msgBuffer);
					}
				}
			else
				{
				_LIT(KErrMessage,"SettingDriveMode");
				msgBuffer.Format(KErrorFormat,aEvent.iErrorCode,&KErrMessage);
				iEikonEnv->InfoMsg(msgBuffer);
				}
			break;
			}
		case KUidECamEventBurstImagesUidValue:
			{
			if(aEvent.iErrorCode)
				{
				_LIT(KErrMessage,"SettingNumOfBurstImage");
				msgBuffer.Format(KErrorFormat,aEvent.iErrorCode,&KErrMessage);
				iEikonEnv->InfoMsg(msgBuffer);
				}	
			break;	
			}
		case KUidECamEventSnapshotUidValue:
			{
			if(aEvent.iErrorCode)
				{
				_LIT(KErrMessage,"SnapshotDataNotification");
				msgBuffer.Format(KErrorFormat,aEvent.iErrorCode,&KErrMessage);
				iEikonEnv->InfoMsg(msgBuffer);
				}
			else
				{
				if(iCamera->ViewFinderActive())
					{	
					iCamera->StopViewFinder();
					}
			
				TRAPD(error, SnapshotDataDisplayL());

				if(error != KErrNone)
					{
					_LIT(KErrMessage,"SnapshotDataDisplay");
					msgBuffer.Format(KErrorFormat,error,&KErrMessage);
					iEikonEnv->InfoMsg(msgBuffer);
					}
				}
			break;
			}
			
		case KUidECamPresetFactoryDefaultUidValue:
			{
			if(aEvent.iErrorCode)
				{
				_LIT(KErrMessage,"FactoryPreset");
				msgBuffer.Format(KErrorFormat,aEvent.iErrorCode,&KErrMessage);
				iEikonEnv->InfoMsg(msgBuffer);
				}
			break;
			}
			
		case KUidECamPresetOutdoorUidValue:
			{
			if(aEvent.iErrorCode)
				{
				_LIT(KErrMessage,"OutdoorPreset");
				msgBuffer.Format(KErrorFormat,aEvent.iErrorCode,&KErrMessage);
				iEikonEnv->InfoMsg(msgBuffer);
				}
			break;
			}
			
		case KUidECamPresetOutdoorSportUidValue:
			{
			if(aEvent.iErrorCode)
				{
				_LIT(KErrMessage,"OutdoorSportPreset");
				msgBuffer.Format(KErrorFormat,aEvent.iErrorCode,&KErrMessage);
				iEikonEnv->InfoMsg(msgBuffer);
				}
			break;
			}
			
		case KUidECamPresetFogUidValue:
			{
			if(aEvent.iErrorCode)
				{
				_LIT(KErrMessage,"FogPreset");
				msgBuffer.Format(KErrorFormat,aEvent.iErrorCode,&KErrMessage);
				iEikonEnv->InfoMsg(msgBuffer);
				}
			break;
			}
			
		case KUidECamPresetNightPartyIndoorUidValue:
			{
			if(aEvent.iErrorCode)
				{
				_LIT(KErrMessage,"NightPartyIndoorPreset");
				msgBuffer.Format(KErrorFormat,aEvent.iErrorCode,&KErrMessage);
				iEikonEnv->InfoMsg(msgBuffer);
				}
			break;
			}
			
		case KUidECamPresetTextDocumentUidValue:
			{
			if(aEvent.iErrorCode)
				{
				_LIT(KErrMessage,"TextDocumentPreset");
				msgBuffer.Format(KErrorFormat,aEvent.iErrorCode,&KErrMessage);
				iEikonEnv->InfoMsg(msgBuffer);
				}
			break;
			}
		
		case KUidECamEventImageProcessingAdjustBrightnessUidValue:
			{
			if(aEvent.iErrorCode)
				{
				_LIT(KErrMessage,"ImageProcessingAdjustBrightness");
				msgBuffer.Format(KErrorFormat,aEvent.iErrorCode,&KErrMessage);
				iEikonEnv->InfoMsg(msgBuffer);
				}
			break;
			}
			
		case KUidECamEventImageProcessingAdjustContrastUidValue:
			{
			if(aEvent.iErrorCode)
				{
				_LIT(KErrMessage,"ImageProcessingAdjustContrast");
				msgBuffer.Format(KErrorFormat,aEvent.iErrorCode,&KErrMessage);
				iEikonEnv->InfoMsg(msgBuffer);
				}
			break;
			}
					
		default:
			break;
		}
	}
	
void CTCamAppUi::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	
	}
	
void CTCamAppUi::ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError)
	{
	TBuf<KMaxErrLength> msgBuffer;
	_LIT(KErrMessage,"CTCamAppUi::ImageBufferReady");
	if(aError)
		{
		msgBuffer.Format(KErrorFormat,aError,&KErrMessage);
		iEikonEnv->InfoMsg(msgBuffer);
		return;
		}
	if(!iCameraSnapshot->IsSnapshotActive())
		{
		if (iCamera->ViewFinderActive())
			{	
			iCamera->StopViewFinder();
			}
		TRAP(aError,iAppView->DrawImage(&aCameraBuffer.BitmapL(0)));		
		}

	aCameraBuffer.Release();
	
	if (aError)
		{
		msgBuffer.Format(KErrorFormat,aError,&KErrMessage);
		iEikonEnv->InfoMsg(msgBuffer);
		}
		
	}
	
void CTCamAppUi::VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError)
	{
	TBuf<KMaxErrLength> msgBuffer;
	_LIT(KErrMessage,"CTCamAppUi::FrameBufferReady");
	if (aError)
		{
		msgBuffer.Format(KErrorFormat,aError,&KErrMessage);
		iEikonEnv->InfoMsg(msgBuffer);
		return;
		}
	TRAPD(error,iAppView->DrawImage(&aCameraBuffer.BitmapL(0)));
	aCameraBuffer.Release();
	if (error)
		{
		msgBuffer.Format(KErrorFormat,error,&KErrMessage);
		iEikonEnv->InfoMsg(msgBuffer);
		}		
	}
	
void CTCamAppUi::SnapshotDataDisplayL()
	{
	RArray<TInt> array;
	array.Reset();
	RTestCameraFrameBuffer& buffer = static_cast<RTestCameraFrameBuffer&>(iCameraSnapshot->SnapshotDataL(array));
	iAppView->DrawImage(&buffer.BitmapL(0));
	array.Close();
	buffer.Release();
	}	

void CTCamAppUi::CreateDependencyFileL()
	{
	// enforce dependency
	// create the file the plugin depends on.
	User::LeaveIfError(ifsSession.Connect());
	RFile file;
	TInt err = file.Replace(ifsSession, KTestCameraPluginName, EFileShareAny);
  	file.Close();
  	if(err != KErrNone)
  		{
	    User::LeaveIfError(KErrNotSupported);
  		}
	}
	
void CTCamAppUi::RemoveDependencyFile()
	{
	ifsSession.Delete(KTestCameraPluginName);
	ifsSession.Close();
	}

//
// CTCamAppView
//

CTCamAppView::CTCamAppView():
	CCoeControl()
	{
	}

void CTCamAppView::ConstructL(const TRect& /*aRect*/)
	{
	CreateWindowL();
	EnableDragEvents();
	SetExtentToWholeScreen();
	ActivateL();
	}

CTCamAppView::~CTCamAppView()
	{
	}

void CTCamAppView::DrawImage(CFbsBitmap* aImage) const
	{
	CWindowGc& gc = SystemGc();
	gc.Activate(Window());
	TRect drawRect=Rect();
	TPoint pos;
	pos.iX = (3 * drawRect.iBr.iX / 4) - (aImage->SizeInPixels().iWidth / 2);
	pos.iY = (drawRect.iBr.iY - aImage->SizeInPixels().iWidth) / 2;
	gc.BitBlt(pos,aImage);
	TRect border(pos,aImage->SizeInPixels());
	border.Grow(1,1);
	gc.DrawRect(border);
	gc.Deactivate();
	iCoeEnv->WsSession().Flush();
	}

void CTCamAppView::Draw(const TRect& /*aRect*/) const
	{
	CWindowGc& gc = SystemGc();
	TRect drawRect(Rect());
	drawRect.Shrink(1,1);
	gc.DrawRect(drawRect);
	gc.Clear();
	}

TPoint CTCamAppView::DrawBorders(const TSize& aSize) const
	{
	CWindowGc& gc = SystemGc();
	gc.Activate(Window());
	TRect drawRect(Rect());
	TPoint pos;
	pos.iX = (drawRect.iBr.iX / 4) - (aSize.iWidth / 2);
	pos.iY = (drawRect.iBr.iY - aSize.iWidth) / 2;
	TRect border(pos,aSize);
	border.Grow(1,1);
	gc.SetPenColor(KRgbBlack);
	gc.DrawRect(border);
	gc.Deactivate();
	iCoeEnv->WsSession().Flush();
	return pos;
	}

//
// CTCamDocument
//

CTCamDocument::CTCamDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{
	}

CEikAppUi* CTCamDocument::CreateAppUiL()
	{
    return new(ELeave) CTCamAppUi;
	}

//
// CTCamApp
//

TUid CTCamApp::AppDllUid() const
	{
	return KUidTestCameraApp;
	}

CApaDocument* CTCamApp::CreateDocumentL()
	{
	return new(ELeave) CTCamDocument(*this);
	}



//
// Base factory function
//

#include <eikstart.h>
LOCAL_C CApaApplication* NewApplication()
	{
	return new CTCamApp;
	}

//
// EXE Entry point
//

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}




