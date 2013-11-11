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
#include <TestCameraApp.rsg>
#include "TestCameraApp.h"
#include "TestCameraApp.hrh"

const TInt KMaxErrLength=50;
_LIT(KErrorFormat,"Error - %d (%S)");

//
//  CTCamAppUi
//

void CTCamAppUi::ConstructL()
	{
    BaseConstructL();
	iAppView = new(ELeave) CTCamAppView;
	iAppView->ConstructL(ClientRect());
	AddToStackL(iAppView);

	iCamera = CCamera::NewL(*this,0);
	iCamera->Reserve();
	}

CTCamAppUi::~CTCamAppUi()
	{
	iCamera->Release();
	delete iCamera;
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
		CaptureImageL();
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
	case EAknSoftkeyBack:
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
		iCamera->StartViewFinderDirectL(iCoeEnv->WsSession(),*iCoeEnv->ScreenDevice(),*iAppView->DrawableWindow(),screenRect,clipRect);
	else
		iCamera->StopViewFinder();
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
		TSize size;
		TReal32 rate = 0;
		iCamera->EnumerateVideoFrameSizes(size,0,CCamera::EFormatFbsBitmapColor16M);
		iCamera->EnumerateVideoFrameRates(rate,0,CCamera::EFormatFbsBitmapColor16M,0);
		iCamera->PrepareVideoCaptureL(CCamera::EFormatFbsBitmapColor16M,0,0,2,1);
		iCamera->StartVideoCapture();
		}
	else
		iCamera->StopVideoCapture();
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

_LIT(KContrastDialogTitle,"Contrast");
_LIT(KBrightnessDialogTitle,"Brightness");

//
// CTCamAppView
//

CTCamAppView::CTCamAppView():
	CCoeControl()
	{}

void CTCamAppView::ConstructL(const TRect& aRect)
	{
	CreateWindowL();
    SetRect(aRect);
	EnableDragEvents();
	ActivateL();
	}

CTCamAppView::~CTCamAppView()
	{}

void CTCamAppView::DrawImage(CFbsBitmap* aImage) const
	{
	CWindowGc& gc = SystemGc();
	gc.Activate(Window());
	TRect drawRect(Rect());
	TPoint pos;
	pos.iX = (drawRect.iBr.iX - aImage->SizeInPixels().iWidth) / 2;
	pos.iY = (drawRect.iBr.iY - aImage->SizeInPixels().iHeight) / 2;
	gc.BitBlt(pos,aImage);
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
	pos.iX = (drawRect.iBr.iX - aSize.iWidth) / 2;
	pos.iY = (drawRect.iBr.iY - aSize.iHeight) / 2;
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
		: CAknDocument(aApp)
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





