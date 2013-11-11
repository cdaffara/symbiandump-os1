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
 @test
 @internalComponent - Internal Symbian test code
*/

#include "tdrawresource.h"
#include <graphics/sgimage.h>
#include <graphics/sgutils.h>
#include <graphics/directgdidrawablesource.h>
#include <graphics/sgresourceinternal.h>
#include <graphics/wsdrawresource.h>


__WS_CONSTRUCT_STEP__(DrawResource);


#if defined(__X86GCC__)
extern "C" TInt atexit(void (*function)(void))
	{
	return KErrNone;
	}
#endif

//
// class CTestWsGraphicsContext
//
CTestWsGraphicsContext* CTestWsGraphicsContext::NewL(RDirectGdiImageTarget& aTarget)
	{
	CTestWsGraphicsContext* self = new(ELeave) CTestWsGraphicsContext;
	CleanupStack::PushL(self);
	self->ConstructL(aTarget);
	CleanupStack::Pop(self);
	return self;
	}

void CTestWsGraphicsContext::ConstructL(RDirectGdiImageTarget& aTarget)
	{
	CDirectGdiDriver* driver = CDirectGdiDriver::Static();
	User::LeaveIfNull(driver);
	iContext = CDirectGdiContext::NewL(*driver);
	TInt err = iContext->Activate(aTarget);
	User::LeaveIfError(err);
	}

CTestWsGraphicsContext::~CTestWsGraphicsContext()
	{
	delete iContext;
	}

TAny* CTestWsGraphicsContext::ResolveObjectInterface(TUint aTypeId)
	{
	switch(aTypeId)
		{
	case MWsDrawableSourceProvider::EWsObjectInterfaceId:
		return static_cast<MWsDrawableSourceProvider*>(this);
		}
	return NULL;

	}
	
TInt CTestWsGraphicsContext::CreateDrawableSource(const TSgDrawableId& aDrawableId, TAny*& aSource)
	{
	TRAPD(err, DoCreateDrawableSourceL(aDrawableId, aSource));
	return err;
	}

void CTestWsGraphicsContext::DoCreateDrawableSourceL(const TSgDrawableId& aDrawableId, TAny*& aSource)
	{
	CDirectGdiDriver* driver = CDirectGdiDriver::Static();
	if (!driver)
		{
		User::Leave(KErrNotReady);
		}
	RDirectGdiDrawableSource* drawableSource = new(ELeave) RDirectGdiDrawableSource(*driver);
	CleanupStack::PushL(drawableSource);
	RSgDrawable drawable;
	User::LeaveIfError(drawable.Open(aDrawableId, ESgDoNotRestrictUsage));
	CleanupClosePushL(drawable);
	User::LeaveIfError(drawableSource->Create(drawable));
	CleanupStack::PopAndDestroy();
	CleanupStack::Pop(drawableSource);
	aSource = drawableSource;
	}

void CTestWsGraphicsContext::CloseDrawableSource(TAny* aSource)
	{
	RDirectGdiDrawableSource* drawableSource = static_cast<RDirectGdiDrawableSource*>(aSource);
	drawableSource->Close();
	delete drawableSource;
	}

void CTestWsGraphicsContext::DrawResource(const TAny* aSource, const TPoint& aPos, CWindowGc::TGraphicsRotation aRotation)
	{
	const RDirectGdiDrawableSource* drawableSource = static_cast<const RDirectGdiDrawableSource*>(aSource);
	iContext->DrawResource(aPos, *drawableSource, (DirectGdi::TGraphicsRotation)aRotation);
	iPos = aPos;
	iRotation = (DirectGdi::TGraphicsRotation)aRotation;
	}

void CTestWsGraphicsContext::DrawResource(const TAny* aSource, const TRect& aRect, CWindowGc::TGraphicsRotation aRotation)
	{
	const RDirectGdiDrawableSource* drawableSource = static_cast<const RDirectGdiDrawableSource*>(aSource);
	iContext->DrawResource(aRect, *drawableSource, (DirectGdi::TGraphicsRotation)aRotation);
	iDestRect = aRect;
	iRotation = (DirectGdi::TGraphicsRotation)aRotation;
	}

void CTestWsGraphicsContext::DrawResource(const TAny* aSource, const TRect& aRectDest, const TRect& aRectSrc, CWindowGc::TGraphicsRotation aRotation)
	{
	const RDirectGdiDrawableSource* drawableSource = static_cast<const RDirectGdiDrawableSource*>(aSource);
	iContext->DrawResource(aRectDest, *drawableSource, aRectSrc, (DirectGdi::TGraphicsRotation)aRotation);
	iDestRect = aRectDest;
	iSrcRect = aRectSrc;
	iRotation = (DirectGdi::TGraphicsRotation)aRotation;
	}

void CTestWsGraphicsContext::DrawResource(const TAny* aSource, const TRect& aRect, const TDesC8& aParam)
	{
	const RDirectGdiDrawableSource* drawableSource = static_cast<const RDirectGdiDrawableSource*>(aSource);
	iContext->DrawResource(aRect, *drawableSource, aParam);
	iDestRect = aRect;
	}


void CTestWsGraphicsContext::Clear()
	{
	iContext->Clear();
	}

//
// class CTDrawResource
//

CTDrawResource::CTDrawResource(CTestStep* aStep)
	: CTWsGraphicsBase(aStep)
	{
	}

CTDrawResource::~CTDrawResource()
	{
	iWindow.Close();
	delete iRefBitmap;
	delete iRotatedRefBitmap;
	delete iScaledBitmap;
	delete iScaledCroppedBitmap;
	delete iCopyBitmap;
	delete iBitmapWrongScreenNumber;

	delete iWsGrapicResolver;
	delete iGraphicsCon;

	iWsDrawableSource.Close();
	iImage.Close();
	iImageTarget.Close();
	iImageCollection.Close();

	CDirectGdiDriver* dGdiDriver = CDirectGdiDriver::Static();
	if(dGdiDriver)
		{
		dGdiDriver->Close();
		}
	SgDriver::Close();
	}

void CTDrawResource::ConstructL()
	{
	//Constrcut and setup window to be drawn to
	iWindow = RWindow(TheClient->iWs);
    User::LeaveIfError(iWindow.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
    TSize iWinSize=TSize(TheClient->iScreen->SizeInPixels());
    iWindow.SetExtent(TPoint(0,0),iWinSize);
	iWindow.Activate();
	iWindow.BeginRedraw();
	iWindow.EndRedraw();
	
	//Creates all reference and copy bitmaps required for all tests
	CreateReferenceAndCopyBitmapsL();

	TInt err = CDirectGdiDriver::Open();
	User::LeaveIfError(err);
	err = SgDriver::Open();
	User::LeaveIfError(err);
	
	//create image target
	CDirectGdiDriver* theDGdiDriver = CDirectGdiDriver::Static();
	if(!theDGdiDriver)
		{
		User::Leave(KErrNotReady);
		}
	TSgImageInfo info;
	info.iUsage = ESgUsageDirectGdiTarget | ESgUsageDirectGdiSource | ESgUsageCompositionSource;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iSizeInPixels = TSize(200, 200);
	info.iShareable = ETrue;
	const TInt KImageCount = 1;
	err = iImageCollection.Create(info, KImageCount);
	User::LeaveIfError(err);
	
	err = iImageCollection.OpenImage(0, iImage);
	User::LeaveIfError(err);
	iImageTarget = RDirectGdiImageTarget(*theDGdiDriver);
	err = iImageTarget.Create(iImage);
	User::LeaveIfError(err);
	
	// construction of image source
	RSgImage sgImage;
	CreateSgImageFromBitmapL(sgImage);
	CleanupClosePushL(sgImage);
	iWsDrawableSource = RWsDrawableSource(TheClient->iWs);
	User::LeaveIfError(iWsDrawableSource.Create(sgImage, TheClient->iScreen->GetScreenNumber()));
	
    //Create dummy MWsGraphicResolver - required RemoteGc testing
	iWsGrapicResolver = new (ELeave) CWSGraphicsRes();
	
	//Create dummy MWsGraphicsContext - required RemoteGc testing
	iGraphicsCon =  CTestWsGraphicsContext::NewL(iImageTarget);
	
	CleanupStack::PopAndDestroy(&sgImage);
	}

void CTDrawResource::RunTestCaseL(TInt aCurTestCase)
	{	
	((CTDrawResourceStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:		
		((CTDrawResourceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-DRAWRESOURCE-0001"));
		INFO_PRINTF1(_L("DrawResourcePos Test"));		
		TestDrawResourcePos();
		break;
	case 2:
		((CTDrawResourceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-DRAWRESOURCE-0002"));
		INFO_PRINTF1(_L("DrawResourceRect Test"));	
		TestDrawResourceRect();
		break;
	case 3:
		((CTDrawResourceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-DRAWRESOURCE-0003"));
		INFO_PRINTF1(_L("DrawResourceScale Test"));		
		TestDrawResourceScale();
		break;
	case 4:
		((CTDrawResourceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-DRAWRESOURCE-0004"));
		INFO_PRINTF1(_L("RWsDrawableSource Reference Counting Test"));		
		TestRWsDrawableSourceReferenceCountingL();
		break;	
	case 5:
		((CTDrawResourceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-DRAWRESOURCE-0005"));
		INFO_PRINTF1(_L("RemoteGcDrawResourcePos Test"));	
		TestRemoteGcDrawResourcePosL();
		break;
	case 6:
		((CTDrawResourceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-DRAWRESOURCE-0006"));
		INFO_PRINTF1(_L("RemoteGcDrawResourceRect Test"));	
		TestRemoteGcDrawResourceRectL();
		break;
	case 7:
		((CTDrawResourceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-DRAWRESOURCE-0007"));
		INFO_PRINTF1(_L("RemoteGcDrawResourceScale Test"));	
		TestRemoteGcDrawResourceScaleL();
		break;
	case 8:
		((CTDrawResourceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-DRAWRESOURCE-0008"));
		INFO_PRINTF1(_L("DrawResourceScreens Test"));	
		TestDrawResourceScreensL();
		break;
	case 9:
		((CTDrawResourceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-TestCopyScreenToBitmapWithDifferentDisplayModes-0001"));
		INFO_PRINTF1(_L("CopyScreenToBitmapWithDifferentDisplayModes Test\n"));
		TestCopyScreenToBitmapWithDifferentDisplayModesL();
		break;
	case 10:
		((CTDrawResourceStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTDrawResourceStep*)iStep)->CloseTMSGraphicsStep();
		INFO_PRINTF1(_L("Test complete\n"));
		TestComplete();
		break;
		}
	((CTDrawResourceStep*)iStep)->RecordTestResultL();
	}

/**
@SYMTestCaseID           GRAPHICS-WSERV-DRAWRESOURCE-0001
@SYMPREQ                 PREQ2095
@SYMTestPriority         High
@SYMTestCaseDesc         Draw RSgImage using DrawResource(const TPoint&, const RWsDrawableSource&, TGraphicsRotation)
@SYMTestActions          Draw to position (0,0) with default rotation (none)
						 Draw to position (10,10) with 90 degrees rotation 
@SYMTestExpectedResults  Drawn images should match reference CFbsBitmap
*/
void CTDrawResource::TestDrawResourcePos()
	{
	// Draw to TPoint(0,0) with EGraphicsRotationNone
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
	TheGc->Clear();
	MWsDrawResource* dr = static_cast<MWsDrawResource*>(TheGc->Interface(KMWsDrawResourceInterfaceUid));
	TEST(dr != NULL);
	dr->DrawResource(KDestPoint, iWsDrawableSource, CWindowGc::EGraphicsRotationNone);
	TheGc->Deactivate();
	iWindow.EndRedraw();
	TheClient->iWs.Finish();  
	TheClient->WaitForRedrawsToFinish();

	//Copy the screen to the copy bitmap
	TheClient->iScreen->CopyScreenToBitmap(iCopyBitmap, KCopyTestRect);

	//Compare the bitmaps
	INFO_PRINTF1(_L("Draw to TPoint(0,0) with EGraphicsRotationNone"));
	TBool compRes = CompareBitmapsByPixel(iCopyBitmap, iRefBitmap);
	TEST(compRes);

	/*** Draw to position (10,10) with 90 degrees rotation ***/
	TPoint const KDestPoint2(10,10);
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
    TheGc->Clear();
	dr->DrawResource(KDestPoint2, iWsDrawableSource, KTestRotation);
    TheGc->Deactivate();
	iWindow.EndRedraw();
	TheClient->iWs.Finish(); 
	TheClient->WaitForRedrawsToFinish();

	//Copy the screen to the copy bitmap
	TheClient->iScreen->CopyScreenToBitmap(iCopyBitmap, TRect(KDestPoint2, KSourceSize));

	//Compare the bitmaps
	INFO_PRINTF1(_L("Draw to position (10,10) with 90 degrees rotation"));
	compRes = CompareBitmapsByPixel(iCopyBitmap, iRotatedRefBitmap);
	TEST(compRes);
	}

/**
@SYMTestCaseID           GRAPHICS-WSERV-DRAWRESOURCE-0002
@SYMPREQ                 PREQ2095
@SYMTestPriority         High
@SYMTestCaseDesc         Draw RSgImage using DrawResource(const TRect& , const RWsDrawableSource&, TGraphicsRotation aRotation)
@SYMTestActions          Draw to rect pos (10,10), rect size (60,60) with 90 degrees rotation
@SYMTestExpectedResults  Drawn images should match the reference CFbsBitmap
*/
void CTDrawResource::TestDrawResourceRect()
	{
	//Use DrawResource
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
	TheGc->Clear();
	MWsDrawResource* dr = static_cast<MWsDrawResource*>(TheGc->Interface(KMWsDrawResourceInterfaceUid));
	TEST(dr != NULL);
	dr->DrawResource(KDestRect, iWsDrawableSource, KTestRotation);
	TheGc->Deactivate();
	iWindow.EndRedraw();
	TheClient->iWs.Finish();  
	TheClient->WaitForRedrawsToFinish();

	//Copy the screen to the copy bitmap
	TheClient->iScreen->CopyScreenToBitmap(iCopyBitmap, KCopyTestRect);

	//Compare the bitmaps
	INFO_PRINTF1(_L("Draw to rect pos (10,10), rect size (60,60) with 90 degrees rotation"));
	TInt compRes = CompareBitmapsByPixel(iCopyBitmap, iScaledBitmap);
	TEST(compRes);
	}

/**
@SYMTestCaseID           GRAPHICS-WSERV-DRAWRESOURCE-0003
@SYMPREQ                 PREQ2095
@SYMTestPriority         High
@SYMTestCaseDesc         Draw RSgImage using DrawResource(const TRect& , const RWsDrawableSource&, const TRect&, TGraphicsRotation aRotation)
@SYMTestActions          Draw the part of the source image (rect Pos (0,0), rect size(50,50))
						 to rect pos(10,10), rect size(60,60) and and 90 degrees rotation
@SYMTestExpectedResults  Drawn images should match the reference CFbsBitmap
*/
void CTDrawResource::TestDrawResourceScale()
	{
	//Use DrawResource
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
	TheGc->Clear();
	MWsDrawResource* dr = static_cast<MWsDrawResource*>(TheGc->Interface(KMWsDrawResourceInterfaceUid));
	TEST(dr != NULL);
	dr->DrawResource(KDestRect, iWsDrawableSource, KSourceRect, KTestRotation);
	TheGc->Deactivate();
	iWindow.EndRedraw();
	TheClient->iWs.Finish();  
	TheClient->WaitForRedrawsToFinish();

	//Copy the screen to the copy bitmap
	TheClient->iScreen->CopyScreenToBitmap(iCopyBitmap, KCopyTestRect);

	//Compare the bitmaps
	INFO_PRINTF1(_L("Draw the part of the source image to rect pos(10,10), rect size(60,60) and and 90 degrees rotation"));
	TInt compRes = CompareBitmapsByPixel(iCopyBitmap, iScaledCroppedBitmap);
	TEST(compRes);
	}

/**
@SYMTestCaseID           GRAPHICS-WSERV-DRAWRESOURCE-0004
@SYMPREQ                 PREQ2095
@SYMTestPriority         High
@SYMTestCaseDesc         Draw RSgImage using DrawResource(const TPoint&, const RWsDrawableSource&, TGraphicsRotation) after closing the image source
@SYMTestActions          Draw to position (0,0) with default rotation (none) after closing the image source
@SYMTestExpectedResults  Drawn image should match reference CFbsBitmap
*/
void CTDrawResource::TestRWsDrawableSourceReferenceCountingL()
	{	
	// construction of image source
	RSgImage sgImage;
	CreateSgImageFromBitmapL(sgImage);
	CleanupClosePushL(sgImage);
	RWsDrawableSource drawableSource(TheClient->iWs);
	CleanupClosePushL(drawableSource);
	User::LeaveIfError(drawableSource.Create(sgImage, TheClient->iScreen->GetScreenNumber())); 

	//Draw using DrawResource
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
	TheGc->Clear();
	MWsDrawResource* dr = static_cast<MWsDrawResource*>(TheGc->Interface(KMWsDrawResourceInterfaceUid));
	TEST(dr != NULL);
	dr->DrawResource(KDestPoint, drawableSource, CWindowGc::EGraphicsRotationNone);
	TheGc->Deactivate();
	iWindow.EndRedraw();

	//Close the image source but this should not stop the source from being drawn 
	drawableSource.Close();
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();

	//Copy the screen to the copy bitmap
	TheClient->iScreen->CopyScreenToBitmap(iCopyBitmap, KCopyTestRect);

	//Compare the bitmaps
	INFO_PRINTF1(_L("Draw to TPoint(0,0) with EGraphicsRotationNone"));
	TBool compRes = CompareBitmapsByPixel(iCopyBitmap, iRefBitmap);
	TEST(compRes);

	CleanupStack::PopAndDestroy(2, &sgImage);
	}

/**
@SYMTestCaseID           GRAPHICS-WSERV-DRAWRESOURCE-0005
@SYMPREQ                 PREQ2095
@SYMTestPriority         High
@SYMTestCaseDesc         Draw RSgImage using RemotGc->DrawResource(const TPoint&, const RWsDrawableSource&, TGraphicsRotation)
@SYMTestActions          Record the draw resource commands using CRemoteGc and play the recorded commands on a window using
 						 Play(const TPoint&, const TRect&, RWsSession&, CWindowGc&) and then play the recorded commands using . 
						 Play(const TPoint&, const TRect&, const MWsGraphicResolver&, MWsGraphicsContext&).
@SYMTestExpectedResults  Drawn images should match reference CFbsBitmap and also values received by MWsGraphicsContext should 
 						 match what was originally sent to DrawResource.
*/
void CTDrawResource::TestRemoteGcDrawResourcePosL()
	{
	const TRect KRemotGcSourceRect(0, 0, iWindow.Size().iWidth, iWindow.Size().iHeight);
	const TRegionFix<1> KClippingRegion(KRemotGcSourceRect);
	CWindowGc::TGraphicsRotation testRotation= CWindowGc::EGraphicsRotationNone;
	
	//Create remote gc - required RemoteGc testing
	CRemoteGc* remoteGc = CRemoteGc::NewL(TheClient->iScreen);
	CleanupStack::PushL(remoteGc);

	//Record draw messages
	remoteGc->ResetCommandBuffer();
	remoteGc->BeginDraw(KRemotGcSourceRect);
	MWsDrawResource* dr = static_cast<MWsDrawResource*>(remoteGc->Interface(KMWsDrawResourceInterfaceUid));
	TEST(dr != NULL);
	dr->DrawResource(KDestPoint, iWsDrawableSource, CWindowGc::EGraphicsRotationNone);
	remoteGc->EndDraw();
	
	RWsGraphicMsgBuf msgBuf;
	CleanupClosePushL(msgBuf);
	//Externalize the captured commands from remote gc in to a buffer
	remoteGc->ExternalizeL(msgBuf, ETrue);
	//Create command buffer - required RemoteGc testing
	CCommandBuffer* cmdBuffer = CCommandBuffer::NewL();
	CleanupStack::PushL(cmdBuffer);
	//Internalize the buffer with captured commands (from CRemoteGC) in to CCommandBuffer
	cmdBuffer->InternalizeL(msgBuf.Pckg());

	// Play stored commands using Play(const TPoint&, const TRect&, RWsSession&, CWindowGc&) 
	remoteGc->ResetCommandBuffer();
	iWindow.Invalidate();
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
	TheGc->Clear();
	cmdBuffer->Play(KPlayOffset, &KClippingRegion, KRemotGcSourceRect, TheClient->iWs, *TheGc);
	TheGc->Deactivate();
	iWindow.EndRedraw();
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();

	//Copy the screen to the copy bitmap
	TheClient->iScreen->CopyScreenToBitmap(iCopyBitmap, KCopyTestRect);

	//Compare the bitmaps
	INFO_PRINTF1(_L("Using Play(const TPoint&, const TRect&, RWsSession&, CWindowGc&)"));
	TBool compRes = CompareBitmapsByPixel(iCopyBitmap, iRefBitmap);
	TEST(compRes);

	// Play stored commands using Play(const TPoint&, const TRect&, const MWsGraphicResolver&, MWsGraphicsContext&) 
	remoteGc->ResetCommandBuffer();
	iWindow.Invalidate();
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
	TheGc->Clear();
	cmdBuffer->Play(KPlayOffset,&KClippingRegion,KRemotGcSourceRect,*iWsGrapicResolver,*iGraphicsCon);
	TheGc->Deactivate();
	iWindow.EndRedraw();
	TheClient->iWs.Finish();

	//Compare the values received by CTestWsGraphicsContext and the values sent to it
	INFO_PRINTF1(_L("Using Play(const TPoint&, const TRect&, const MWsGraphicResolver&, MWsGraphicsContext&)"));
	TBool valuesSame = EFalse;
	CFbsBitmap* bmp;
	BitmapLC(bmp);
	compRes = CompareBitmapsByPixel(bmp, iRefBitmap);
	CleanupStack::PopAndDestroy(bmp);

	if((iGraphicsCon->iPos == KDestPoint) && (compRes)
									&& (reinterpret_cast<TInt&>(iGraphicsCon->iRotation)==reinterpret_cast<TInt&>(testRotation)))
		valuesSame = ETrue;
	TEST(valuesSame);
	CleanupStack::PopAndDestroy(3, remoteGc);
	}

/**
@SYMTestCaseID           GRAPHICS-WSERV-DRAWRESOURCE-0006
@SYMPREQ                 PREQ2095
@SYMTestPriority         High
@SYMTestCaseDesc         Draw RSgImage using RemotGc->DrawResource(const TRect&, const RWsDrawableSource&, TGraphicsRotation)
@SYMTestActions          Record the draw resource commands using CRemoteGc and play the recorded commands on a window using
 						 Play(const TPoint&, const TRect&, RWsSession&, CWindowGc&) and then play the recorded commands using . 
						 Play(const TPoint&, const TRect&, const MWsGraphicResolver&, MWsGraphicsContext&).
@SYMTestExpectedResults  Drawn images should match reference CFbsBitmap and also values received by MWsGraphicsContext should 
 						 match what was originally sent to DrawResource.
*/
void CTDrawResource::TestRemoteGcDrawResourceRectL()
	{
	const TRect KRemotGcSourceRect(0, 0, iWindow.Size().iWidth, iWindow.Size().iHeight);
	const TRegionFix<1> KClippingRegion(KRemotGcSourceRect);
	
	//Create remote gc - required RemoteGc testing
	CRemoteGc* remoteGc = CRemoteGc::NewL(TheClient->iScreen);
	CleanupStack::PushL(remoteGc);

	//Record draw messages
	remoteGc->BeginDraw(KRemotGcSourceRect);
	MWsDrawResource* dr = static_cast<MWsDrawResource*>(remoteGc->Interface(KMWsDrawResourceInterfaceUid));
	TEST(dr != NULL);
	dr->DrawResource(KDestRect, iWsDrawableSource, KTestRotation);
	remoteGc->EndDraw();
	
	RWsGraphicMsgBuf msgBuf;
	CleanupClosePushL(msgBuf);
	//Externalize the captured commands from remote gc in to a buffer
	remoteGc->ExternalizeL(msgBuf, ETrue);

	//Create command buffer - required RemoteGc testing
	CCommandBuffer* cmdBuffer = CCommandBuffer::NewL();
	CleanupStack::PushL(cmdBuffer);
	//Internalize the buffer with captured commands (from CRemoteGC) in to CCommandBuffer
	cmdBuffer->InternalizeL(msgBuf.Pckg());

	// Play stored commands using Play(const TRect&, const TRect&, RWsSession&, CWindowGc&)
	iWindow.Invalidate();
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
	TheGc->Clear();
	cmdBuffer->Play(KPlayOffset, &KClippingRegion, KRemotGcSourceRect, TheClient->iWs, *TheGc);
	TheGc->Deactivate();
	iWindow.EndRedraw();
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();

	//Copy the screen to the copy bitmap
	TheClient->iScreen->CopyScreenToBitmap(iCopyBitmap, KCopyTestRect);

	//Compare the bitmaps
	INFO_PRINTF1(_L("Using Play(const TPoint&, const TRect&, RWsSession&, CWindowGc&)"));
	TBool compRes = CompareBitmapsByPixel(iCopyBitmap, iScaledBitmap);
	TEST(compRes);

	// Play stored commands using Play(const TPoint&, const TRect&, const MWsGraphicResolver&, MWsGraphicsContext&) 
	iWindow.Invalidate();
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
	TheGc->Clear();
	iGraphicsCon->Clear();
	cmdBuffer->Play(KPlayOffset,&KClippingRegion,KRemotGcSourceRect,*iWsGrapicResolver,*iGraphicsCon);
	TheGc->Deactivate();
	iWindow.EndRedraw();
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();

	//Compare the values received by CTestWsGraphicsContext and the values sent to it
	INFO_PRINTF1(_L("Using Play(const TPoint&, const TRect&, const MWsGraphicResolver&, MWsGraphicsContext&)"));
	TBool valuesSame = EFalse;

	CFbsBitmap* bmp;
	BitmapLC(bmp);
	compRes = CompareBitmapsByPixel(bmp, iScaledBitmap);
	CleanupStack::PopAndDestroy(bmp);

	if((iGraphicsCon->iDestRect == KDestRect) && (compRes)
								&& (reinterpret_cast<TInt&>(iGraphicsCon->iRotation)==reinterpret_cast<const TInt&>(KTestRotation)))
		valuesSame = ETrue;
	TEST(valuesSame);
	CleanupStack::PopAndDestroy(3, remoteGc);
	}

/**
@SYMTestCaseID           GRAPHICS-WSERV-DRAWRESOURCE-0007
@SYMPREQ                 PREQ2095
@SYMTestPriority         High
@SYMTestCaseDesc         Draw RSgImage using RemotGc->DrawResource(const TRect&, const RWsDrawableSource&, const TRect&, TGraphicsRotation)
@SYMTestActions          Record the draw resource commands using CRemoteGc and play the recorded commands on a window using
 						 Play(const TPoint&, const TRect&, RWsSession&, CWindowGc&) and then play the recorded commands using . 
						 Play(const TPoint&, const TRect&, const MWsGraphicResolver&, MWsGraphicsContext&).
@SYMTestExpectedResults  Drawn images should match reference CFbsBitmap and also values received by MWsGraphicsContext should 
 						 match what was originally sent to DrawResource.
*/
void CTDrawResource::TestRemoteGcDrawResourceScaleL()
	{
	const TRect KRemotGcSourceRect(0, 0, iWindow.Size().iWidth, iWindow.Size().iHeight);
	const TRegionFix<1> KClippingRegion(KRemotGcSourceRect);
	
	//Create remote gc - required RemoteGc testing
	CRemoteGc* remoteGc = CRemoteGc::NewL(TheClient->iScreen);
	CleanupStack::PushL(remoteGc);

	//Record draw messages
	remoteGc->BeginDraw(KRemotGcSourceRect);
	MWsDrawResource* dr = static_cast<MWsDrawResource*>(remoteGc->Interface(KMWsDrawResourceInterfaceUid));
	TEST(dr != NULL);
	dr->DrawResource(KDestRect, iWsDrawableSource, KSourceRect, KTestRotation);
	remoteGc->EndDraw();
	
	RWsGraphicMsgBuf msgBuf;
	CleanupClosePushL(msgBuf);
	//Externalize the captured commands from remote gc in to a buffer
	remoteGc->ExternalizeL(msgBuf, ETrue);

	//Create command buffer - required RemoteGc testing
	CCommandBuffer* cmdBuffer = CCommandBuffer::NewL();
	CleanupStack::PushL(cmdBuffer);
	//Internalize the buffer with captured commands (from CRemoteGC) in to CCommandBuffer
	cmdBuffer->InternalizeL(msgBuf.Pckg());

	// Play the stored commands using Play(const TRect&, const TRect&, RWsSession&, CWindowGc&)
	remoteGc->ResetCommandBuffer();
	iWindow.Invalidate();
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
	TheGc->Clear();
	cmdBuffer->Play(KPlayOffset, &KClippingRegion, KRemotGcSourceRect, TheClient->iWs, *TheGc);
	TheGc->Deactivate();
	iWindow.EndRedraw();
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();
	//Copy the screen to the copy bitmap
	TheClient->iScreen->CopyScreenToBitmap(iCopyBitmap, KCopyTestRect);

	//Compare the bitmaps
	INFO_PRINTF1(_L("Using Play(const TPoint&, const TRect&, RWsSession&, CWindowGc&)"));
	TBool compRes = CompareBitmapsByPixel(iCopyBitmap, iScaledCroppedBitmap);
	TEST(compRes);

	// Play the stored commands using Play(const TPoint&, const TRect&, const MWsGraphicResolver&, MWsGraphicsContext&) 	
	iWindow.Invalidate();
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
	TheGc->Clear();
	iGraphicsCon->Clear();
	cmdBuffer->Play(KPlayOffset,&KClippingRegion,KRemotGcSourceRect,*iWsGrapicResolver,*iGraphicsCon);
	TheGc->Deactivate();
	iWindow.EndRedraw();
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();

	//Compare the values received by CTestWsGraphicsContext and the values sent to it
	INFO_PRINTF1(_L("Using Play(const TPoint&, const TRect&, const MWsGraphicResolver&, MWsGraphicsContext&)"));
	TBool valuesSame = EFalse;

	CFbsBitmap* bmp;
	BitmapLC(bmp);
	compRes = CompareBitmapsByPixel(bmp, iScaledCroppedBitmap);
	CleanupStack::PopAndDestroy(bmp);

	if((iGraphicsCon->iDestRect == KDestRect) && (compRes) && (iGraphicsCon->iSrcRect == KSourceRect)
								&& (reinterpret_cast<TInt&>(iGraphicsCon->iRotation)==reinterpret_cast<const TInt&>(KTestRotation)))
		valuesSame = ETrue;
	TEST(valuesSame);
	CleanupStack::PopAndDestroy(3, remoteGc);
	}

/**
@SYMTestCaseID          GRAPHICS-WSERV-DRAWRESOURCE-0008
@SYMPREQ                PREQ2095
@SYMTestPriority        High
@SYMTestCaseDesc        Negative testing. Draw graphics recourses which associated with different screens.
@SYMTestActions         Open RWsDrawableSource associated with the screen which doesn’ exist.  
						Open RWsDrawableSource associated with the screen which exists but differes from drawing target.
						Draw Rectangle and Resource to the screen via CWindowGc
						Draw Rectangle and Resource to the screen via CRemoteGc
@SYMTestExpectedResults  Opening drawable resource on the screen which doesn’t exist must fail with error code KErrArgument 
						Drawing primitives will result only rectangles be drawn as drawable recourses get associated with different screen
*/
void CTDrawResource::TestDrawResourceScreensL()
	{
	TInt numOfScreens = TheClient->iWs.NumberOfScreens();
	if(numOfScreens < 2)
		{
		INFO_PRINTF2(_L("This test case will be running if the number of available screens more than 1, current number is %d"), numOfScreens);
		return;
		}
	TInt screenNumber = TheClient->iScreen->GetScreenNumber();
	TInt differentScreen = (screenNumber == 0) ? 1 : 0;

	RSgImage sgImage;
	CreateSgImageFromBitmapL(sgImage);
	CleanupClosePushL(sgImage);

	RWsDrawableSource drawableSource(TheClient->iWs);
	TInt res = drawableSource.Create(sgImage, differentScreen + 200); //wrong screen number
	TEST(res == KErrArgument);

	res = drawableSource.Create(iImage, differentScreen);
	TEST(res == KErrNotSupported); //in order to succeed the image must be created with flag usage ESgUsageWindowGcSource
	
	TSgImageInfo info; 
	User::LeaveIfError(sgImage.GetInfo(info));

	res = drawableSource.Create(sgImage, differentScreen);
	if(res == KErrNotSupported)
		{
		INFO_PRINTF1(_L("The second screen is not supports drawable source. This test case terminates now."));
		CleanupStack::PopAndDestroy(&sgImage);
		return;
		}
	TEST(res == KErrNone);
	User::LeaveIfError(res);
	CleanupClosePushL(drawableSource);
	
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
	TheGc->Clear();
	TheGc->SetBrushStyle(CFbsBitGc::ESolidBrush);
	TheGc->SetPenStyle(CFbsBitGc::ENullPen);
	TheGc->SetBrushColor(KRgbGreen);
	TheGc->DrawRect(TRect(KDestPoint, info.iSizeInPixels));
	// Draw to TPoint(0,0) with EGraphicsRotationNone but to the different screen
	MWsDrawResource* winDr = static_cast<MWsDrawResource*>(TheGc->Interface(KMWsDrawResourceInterfaceUid));
	TEST(winDr != NULL);
	winDr->DrawResource(KDestPoint, drawableSource, CWindowGc::EGraphicsRotationNone);
	TheGc->Deactivate();
	iWindow.EndRedraw();
	TheClient->iWs.Finish();  
	TheClient->WaitForRedrawsToFinish();

	//Copy the screen to the copy bitmap
	TheClient->iScreen->CopyScreenToBitmap(iCopyBitmap, KCopyTestRect);

	//Compare the bitmaps
	INFO_PRINTF1(_L("Draw to TPoint(0,0) with EGraphicsRotationNone but to different screen"));
	TBool compRes = CompareBitmapsByPixel(iCopyBitmap, iBitmapWrongScreenNumber);
	TEST(compRes);

	//--------------------------- remoteGc 
	const TRect KRemotGcSourceRect(0, 0, iWindow.Size().iWidth, iWindow.Size().iHeight);
	const TRegionFix<1> KClippingRegion(KRemotGcSourceRect);
	
	//Create remote gc - required RemoteGc testing
	CRemoteGc* remoteGc = CRemoteGc::NewL(TheClient->iScreen);
	CleanupStack::PushL(remoteGc);

	//Record draw messages
	remoteGc->BeginDraw(KRemotGcSourceRect);
	remoteGc->SetBrushStyle(CFbsBitGc::ESolidBrush);
	remoteGc->SetPenStyle(CFbsBitGc::ENullPen);
	remoteGc->SetBrushColor(KRgbGreen);
	remoteGc->DrawRect(TRect(KDestPoint, info.iSizeInPixels));
	MWsDrawResource* remDr = static_cast<MWsDrawResource*>(remoteGc->Interface(KMWsDrawResourceInterfaceUid));
	TEST(remDr != NULL);
	remDr->DrawResource(KDestRect, drawableSource, CWindowGc::EGraphicsRotationNone);
	remoteGc->EndDraw();
	
	RWsGraphicMsgBuf msgBuf;
	CleanupClosePushL(msgBuf);
	//Externalize the captured commands from remote gc in to a buffer
	remoteGc->ExternalizeL(msgBuf, ETrue);

	//Create command buffer - required RemoteGc testing
	CCommandBuffer* cmdBuffer = CCommandBuffer::NewL();
	CleanupStack::PushL(cmdBuffer);
	//Internalize the buffer with captured commands (from CRemoteGC) in to CCommandBuffer
	cmdBuffer->InternalizeL(msgBuf.Pckg());
	
	// Play stored commands using Play(const TRect&, const TRect&, RWsSession&, CWindowGc&)
	iWindow.Invalidate();
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
	TheGc->Clear();
	cmdBuffer->Play(KPlayOffset, &KClippingRegion, KRemotGcSourceRect, TheClient->iWs, *TheGc);
	TheGc->Deactivate();
	iWindow.EndRedraw();
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();

	//Copy the screen to the copy bitmap
	TheClient->iScreen->CopyScreenToBitmap(iCopyBitmap, KCopyTestRect);

	INFO_PRINTF1(_L("Draw to TPoint(0,0) with EGraphicsRotationNone but to different screen"));
	TBool compRes1 = CompareBitmapsByPixel(iCopyBitmap, iBitmapWrongScreenNumber);
	TEST(compRes1);

	CleanupStack::PopAndDestroy(5, &sgImage);
	}

/**
@SYMTestCaseID          GRAPHICS-WSERV-TestCopyScreenToBitmapWithDifferentDisplayModes-0001
@SYMPREQ                PREQ2095
@SYMTestPriority        High
@SYMTestCaseDesc        Get bitmap and scanline from the screen.  
@SYMTestActions         Draw bitmap in various display modes to the screen. 
						Retrieve scan line and bitmap from the screen using standard WSERV API.  

@SYMTestExpectedResults Checks that obtained bitmap matches with the reference bitmap.
*/
void CTDrawResource::TestCopyScreenToBitmapWithDifferentDisplayModesL()
	{
	TDisplayMode mode = TheClient->iScreen->DisplayMode();
    INFO_PRINTF2(_L("Screen display mode %d"), mode);
	CFbsBitmap* bitmap = NULL;
	
	TSize bitmapSize(163, 120);
	CreateBitmapLC(bitmap, bitmapSize, mode);
	
	iWindow.BeginRedraw();
	TheGc->Activate(iWindow);
	TheGc->SetBrushStyle(CWindowGc::ESolidBrush);
	TheGc->SetBrushColor(KRgbYellow);
	TheGc->Clear();
	const TPoint ptOffset(10, 15);
	TheGc->BitBlt(KDestPoint + ptOffset, bitmap);

	TheGc->Deactivate();
	iWindow.EndRedraw();
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();

	const TInt length = bitmapSize.iWidth;
	const TInt height = bitmapSize.iHeight;
	const TInt buffersSize = length * 4;
	TUint8 *screenData = (TUint8*) User::AllocL(buffersSize);
	CleanupStack::PushL(screenData);
	TUint8 *bitmapData = (TUint8*) User::AllocL(buffersSize);
	CleanupStack::PushL(bitmapData);
	TPtr8 ptrScreen (screenData, buffersSize, buffersSize);
	TPtr8 ptrBitmap (bitmapData, buffersSize, buffersSize);
	
	//EGray mode uses dithering in BitGdi, wserv doesnt support this, thus skipping the first mode 
	for(TInt ii =2; ; ii++)
		{
		TDisplayMode dispMode = (TDisplayMode)ii; 
		if(dispMode >= EColorLast)
			break;
		if(dispMode == ERgb)
			continue;

	    INFO_PRINTF2(_L("Copy Screen to bitmap, destination mode %d"), dispMode);
		
		CFbsBitmap *bmp = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(bmp);
		User::LeaveIfError(bmp->Create(bitmapSize, dispMode));
		TUidPixelFormat pixelFormat = SgUtils::DisplayModeToPixelFormat(dispMode);
    	const TInt minStride = SgUtils::MinDataStride(length, pixelFormat);
    	const TInt bitmapStride = bmp->DataStride();
    	TEST(minStride <= bitmapStride);
	
	//Copy the screen to the copy bitmap
		TRect rc = bitmapSize;
		rc.Move(ptOffset);
		TheClient->iScreen->CopyScreenToBitmap(bmp, rc);
		CFbsBitmap *bmpRef = NULL;
		CreateReferenceBitmapLC(bmpRef, bitmap, dispMode);
		TEST(bmpRef->DisplayMode() == dispMode);
		
		//Compare the bitmaps
	    TBool compRes = CompareBitmapsByPixel(bmp, bmpRef);
	    TEST(compRes);
	    
	    for(TInt jj = 0; jj < height; jj++)
	    	{
	    	TPoint pt(0, jj);
	    	TPoint ptScreen = pt + ptOffset;
	    	
	    	Mem::Fill(screenData, bitmapStride, 0xff);
	    	Mem::Fill(bitmapData, bitmapStride, 0xff);
	    	
	    	TheClient->iScreen->GetScanLine(ptrScreen, ptScreen, length, dispMode);
	    	bitmap->GetScanLine(ptrBitmap, pt, length, dispMode);
	    	TInt length1 = ptrScreen.Length();
	    	
	    	TInt res = Mem::Compare(screenData, length1, bitmapData, length1);
	    	TEST(res == 0);
	    	}
	    CleanupStack::PopAndDestroy(2, bmp);
		}//screen modes;
		
	CleanupStack::PopAndDestroy(3, bitmap);
	}

//Helper function: Creates reference bitmap with specified display mode
void CTDrawResource::CreateReferenceBitmapLC(CFbsBitmap*& aBmpTarget, CFbsBitmap* aBmpSrc, TDisplayMode aDestMode) 
	{
	TSize size = aBmpSrc->SizeInPixels();
	aBmpTarget = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(aBmpTarget);
	User::LeaveIfError(aBmpTarget->Create(size, aDestMode));
    CFbsBitmapDevice *refBitmapDev = CFbsBitmapDevice::NewL(aBmpTarget);
    CleanupStack::PushL(refBitmapDev);
    CFbsBitGc *originalBitGc;
	User::LeaveIfError(refBitmapDev->CreateContext(originalBitGc));
	CleanupStack::PushL(originalBitGc);
	originalBitGc->BitBlt(TPoint(0,0), aBmpSrc);
	CleanupStack::PopAndDestroy(2, refBitmapDev);
	}

//Helper function: Creates reference bitmap with specified display mode
void CTDrawResource::CreateBitmapLC(CFbsBitmap*& aBmpTarget, const TSize& aSize, TDisplayMode aDispMode) const
	{
	aBmpTarget = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(aBmpTarget);
	User::LeaveIfError(aBmpTarget->Create(aSize, aDispMode));
    CFbsBitmapDevice *refBitmapDev = CFbsBitmapDevice::NewL(aBmpTarget);
    CleanupStack::PushL(refBitmapDev);
    CFbsBitGc *originalBitGc;
	User::LeaveIfError(refBitmapDev->CreateContext(originalBitGc));
	CleanupStack::PushL(originalBitGc);
	
	TRect rect = TRect(aSize); 
	rect.Shrink(21, 15);
	
	originalBitGc->SetBrushStyle(CFbsBitGc::ESolidBrush);
	originalBitGc->SetBrushColor(TRgb(0,150,150));
	originalBitGc->DrawRect(TRect(TPoint(0,0), TSize(aSize.iWidth, aSize.iHeight / 2)));
	originalBitGc->SetBrushColor(TRgb(150,100,150));
	originalBitGc->DrawRect(TRect(TPoint(0,aSize.iHeight / 2), TSize(aSize.iWidth, aSize.iHeight)));

	originalBitGc->SetBrushColor(TRgb(0,0,128));
	originalBitGc->DrawRect(rect);
	
	
	CleanupStack::PopAndDestroy(2, refBitmapDev);
	}

//Helper function: Creates a RSgImage from a bitmap
void CTDrawResource::CreateSgImageFromBitmapL(RSgImage& aSgImage)
	{
	TUint32* data = iRefBitmap->DataAddress();
	TInt stride = iRefBitmap->DataStride();
	TSize size = iRefBitmap->SizeInPixels();

	TSgImageInfo info;
	info.iSizeInPixels = size;
	info.iScreenId = TheClient->iScreen->CurrentScreenMode();
	info.iShareable = ETrue; //must be shareable since wserv is in other process
	info.iUsage = ESgUsageWindowGcSource;
	info.iPixelFormat = SgUtils::DisplayModeToPixelFormat(iRefBitmap->DisplayMode());

	User::LeaveIfError(aSgImage.Create(info, data, stride));
	}

//Helper function: Creates all reference bitmaps required for all tests
void CTDrawResource::CreateReferenceAndCopyBitmapsL()
	{
	//Create reference bitmap
	iRefBitmap = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iRefBitmap->Create(TSize(160,120),EColor64K));
	
	//Setup to draw to original reference bitmap
    CFbsBitmapDevice *refBitmapDev = CFbsBitmapDevice::NewL(iRefBitmap);
    CleanupStack::PushL(refBitmapDev);
    CFbsBitGc *originalBitGc;
	User::LeaveIfError(refBitmapDev->CreateContext(originalBitGc));
	CleanupStack::PushL(originalBitGc);
	
	//Draw to reference bitmap
	originalBitGc->SetBrushStyle(CFbsBitGc::ESolidBrush);
	originalBitGc->SetBrushColor(TRgb(0,150,150));
	originalBitGc->DrawRect(TRect(TPoint(0,0), TSize(160,60)));
	originalBitGc->SetBrushColor(TRgb(150,100,150));
	originalBitGc->DrawRect(TRect(TPoint(0,60), TSize(160,60)));
	
	//create a rotated version of the reference bitmap
	iRotatedRefBitmap = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iRotatedRefBitmap->Create(TSize(120,160),EColor64K));
	
	//Setup to draw to rotated reference bitmap
    CFbsBitmapDevice *rotRefBitmapDev = CFbsBitmapDevice::NewL(iRotatedRefBitmap);
    CleanupStack::PushL(rotRefBitmapDev);
    CFbsBitGc *rotatedBitGc;
	User::LeaveIfError(rotRefBitmapDev->CreateContext(rotatedBitGc));
	CleanupStack::PushL(rotatedBitGc);
	
	//Draw to rotated reference bitmap
	rotatedBitGc->SetBrushStyle(CFbsBitGc::ESolidBrush);
	rotatedBitGc->SetBrushColor(TRgb(0,150,150));
	rotatedBitGc->DrawRect(TRect(TPoint(60,0), TSize(60,160)));
	rotatedBitGc->SetBrushColor(TRgb(150,100,150));
	rotatedBitGc->DrawRect(TRect(TPoint(0,0), TSize(60,160)));
	
	//Prepare a scaled version of the rotated reference bitmap to later compare against
    iScaledBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError(iScaledBitmap->Create(TSize(160,120),EColor64K));
	//Setup to draw to bitmap
    CFbsBitmapDevice *scaledBitDev = CFbsBitmapDevice::NewL(iScaledBitmap);
    CleanupStack::PushL(scaledBitDev);
    CFbsBitGc *scaledBitGc;
	User::LeaveIfError(scaledBitDev->CreateContext(scaledBitGc));
	CleanupStack::PushL(scaledBitGc);
	//Draw the rotated reference bitmap scaled
	scaledBitGc->DrawBitmap(KDestRect, iRotatedRefBitmap); 
	
	//Prepare a scaled version of the rotated reference bitmap to later compare against
	iScaledCroppedBitmap = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iScaledCroppedBitmap->Create(TSize(160,120),EColor64K));
    
	//Setup to draw to bitmap
    CFbsBitmapDevice *scaledCroppedBitDev = CFbsBitmapDevice::NewL(iScaledCroppedBitmap);
    CleanupStack::PushL(scaledCroppedBitDev);
    CFbsBitGc *scaledCroppedBitGc;
	User::LeaveIfError(scaledCroppedBitDev->CreateContext(scaledCroppedBitGc));
	CleanupStack::PushL(scaledCroppedBitGc);
	
	//Draw the rotated reference bitmap scaled
	TInt width = iRotatedRefBitmap->SizeInPixels().iWidth;
	TInt height = iRotatedRefBitmap->SizeInPixels().iHeight; 
	
	TRect rectSrc;
	rectSrc.iTl.iX= width - KSourceRect.Height();
	rectSrc.iTl.iY= 0;
	rectSrc.iBr.iX= width;
	rectSrc.iBr.iY= KSourceRect.Width();

	scaledCroppedBitGc->DrawBitmap(KDestRect, iRotatedRefBitmap, rectSrc); 
	
	//Prepare bitmap for testing drawable which opened with different screen number
	iBitmapWrongScreenNumber = new (ELeave) CFbsBitmap();
    User::LeaveIfError(iBitmapWrongScreenNumber->Create(TSize(160,120),EColor64K));
	//Setup to draw to bitmap
    CFbsBitmapDevice *wrongScreenNumberBitDev = CFbsBitmapDevice::NewL(iBitmapWrongScreenNumber);
    CleanupStack::PushL(wrongScreenNumberBitDev);
    CFbsBitGc *wrongScreenNumberBitGc;
	User::LeaveIfError(wrongScreenNumberBitDev->CreateContext(wrongScreenNumberBitGc));
	CleanupStack::PushL(wrongScreenNumberBitGc);
	//Draw the rotated reference bitmap scaled
	wrongScreenNumberBitGc->SetBrushColor(KRgbGreen);
	wrongScreenNumberBitGc->SetBrushStyle(CFbsBitGc::ESolidBrush);
	wrongScreenNumberBitGc->SetPenStyle(CFbsBitGc::ENullPen);
	wrongScreenNumberBitGc->DrawRect(TRect(0, 0, 160, 120)); //
	
	//Create a bitmap  to copy to with the same display mode as the reference bitmap
	iCopyBitmap = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iCopyBitmap->Create(TSize(640,240),EColor64K));
	
	CleanupStack::PopAndDestroy(10, refBitmapDev);
	}

//Helper function: This function compares two bitmaps on a pixel by pixel basis */
TBool CTDrawResource::CompareBitmapsByPixel(CFbsBitmap* aCandidateBitmap, CFbsBitmap* aReferenceBitmap)
	{
	TBool result = ETrue;

	TSize candidateSize = aCandidateBitmap->SizeInPixels();
	TSize referenceSize = aReferenceBitmap->SizeInPixels();

	TInt mismatchedPixels = 0;
	
	TRgb nativePixel;
	TRgb referencePixel;
	for (TInt x = 0; x < referenceSize.iWidth; x++)
		{
		for (TInt y = 0; y < referenceSize.iHeight; y++)
			{
			TPoint point(x,y);
			nativePixel = TRgb(0,0,0,0);
			referencePixel = TRgb(0,0,0,0);
			aCandidateBitmap->GetPixel(nativePixel, point);
			aReferenceBitmap->GetPixel(referencePixel, point);

			if (nativePixel != referencePixel)
				{
				mismatchedPixels++;
				result = EFalse;
				}
			}
		}
	
	INFO_PRINTF2(_L("Number of different pixels: %i"), mismatchedPixels);
	return result;
	}
//Helper function: This function extracts content of the image associated with the image target and copies it into bitmap
void CTDrawResource::BitmapLC(CFbsBitmap*& aBmp)
	{
	aBmp = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(aBmp);
	
	TSgImageInfo info;
	TInt res = iImage.GetInfo(info);
	User::LeaveIfError(res);
	
	res = aBmp ->Create(info.iSizeInPixels, SgUtils::PixelFormatToDisplayMode(info.iPixelFormat));
	User::LeaveIfError(res);
	TUint32* dataAddressDest = aBmp->DataAddress();
	
	RSgImage image;
	info.iUsage = ESgUsageNone;
	info.iCpuAccess = ESgCpuAccessReadOnly;
	res = image.Create(info, iImage);
	User::LeaveIfError(res);
	CleanupClosePushL(image);
	const TAny* dataAddress = NULL; 
	TInt dataStride;
	res = image.MapReadOnly(dataAddress, dataStride);
	User::LeaveIfError(res);
	Mem::Copy(dataAddressDest, dataAddress, dataStride * info.iSizeInPixels.iHeight);
	image.Unmap();
	CleanupStack::PopAndDestroy(&image);
	}
