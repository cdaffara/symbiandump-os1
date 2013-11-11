// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include "t_wservintegstepbasic.h"
#include <test/tefunit.h>
#include <e32const.h>	//KNullUidValue
#include <pixelformats.h>

namespace t_wservintegstepbasic
	{
	static const TUint delay=100000;	//0.5 sec

	const TInt KSurfaceWidth = 50;
	const TInt KSurfaceHeight = 50;
	const TUidPixelFormat KSurfaceFormat = EUidPixelFormatXRGB_8888;
	const TInt KBytesPerPixel = 4;	// Four bytes per pixel for the format above.

	// Events delivered to this handle are thrown away
	const TUint32 ENullWsHandle = 0xFFFFFFFF;
	_LIT(KT_WServIntegStepBasicTestId,"testid");
	_LIT(KT_BasicStepPanicTxt, "t_wservintegstepbasic");
	}

using namespace t_wservintegstepbasic;

CT_WServIntegStepBasic::CT_WServIntegStepBasic()
	{
	SetTestStepName(KT_WServIntegStepBasic);
	}


CT_WServIntegStepBasic::~CT_WServIntegStepBasic()
	{
	delete iGc;
	delete iScreen;
	iWinGroup.Close();
	iWsSession.Flush();
	iWsSession.Close();
	}


enum TVerdict CT_WServIntegStepBasic::doTestStepPreambleL()
	{
	TVerdict ret=CTestStep::doTestStepPreambleL();

	if ( !GetIntFromConfig( ConfigSection(), KT_WServIntegStepBasicTestId, iTestId ) )
		{
		User::Leave(KErrNotFound);
		}
	else
		{
		User::LeaveIfError(iWsSession.Connect());
		iScreen = new (ELeave) CWsScreenDevice(iWsSession);
		User::LeaveIfError(iScreen->Construct());
		iWinGroup  = RWindowGroup(iWsSession);
		User::LeaveIfError(iWinGroup.Construct(ENullWsHandle) );
		iWinGroup.AutoForeground(ETrue);
		iGc = new (ELeave) CWindowGc(iScreen);
		User::LeaveIfError(iGc->Construct());
		iWsSession.Flush();
		}
	return ret;
	}

enum TVerdict CT_WServIntegStepBasic::doTestStepPostambleL()
	{
	return TestStepResult();
	}

/**
Starts test step
More detail on each test step can be
found in the respective script file.
@internalComponent
@return TVerdict pass / fail
@pre N/A
@post N/A
*/
enum TVerdict CT_WServIntegStepBasic::doTestStepL()
	{
	__UHEAP_MARK;

	RWindow win;
	RBlankWindow blankWin;

	switch( iTestId )
		{
		case 1:
			Graphics_Wserv_Gce_1L(win);
			break;
		case 2:
			Graphics_Wserv_Gce_2L(blankWin);
			break;
		case 3:
			Graphics_Wserv_Gce_3L();
			break;
		case 4:
			Graphics_Wserv_Gce_4L(win);
			break;
		case 5:
			Graphics_Wserv_Gce_5L(win);
			break;
		case 6:
			Graphics_Wserv_Gce_6L(win);
			break;
		case 7:
			//Test removed in CR1489
			break;
		case 8:
			//Test removed in CR1489
			break;
		case 9:
			//Test removed in CR1489
			break;
		case 10:
			//Test removed in CR1489
			break;
		case 11:
			Graphics_Wserv_Gce_11L(win);
			break;
		case 12:
			//Test removed in CR1489
			break;
		case 13:
			Graphics_Wserv_Gce_13L(win);
			break;
		case 14:
			//Test removed in CR1489
			break;
		case 15:
			Graphics_Wserv_Gce_15L(win);
			break;
		case 16:
			//Test removed in CR1489
			break;
		case 17:
			Graphics_Wserv_Gce_17L(win);
			break;
		case 18:
			//Test removed in CR1489
			break;
		case 19:
			Graphics_Wserv_Gce_19L(win);
			break;
		case 20:
			//Test removed in CR1489
			break;
		case 21:
			Graphics_Wserv_Gce_21L(win);
			break;
		case 22:
			Graphics_Wserv_Gce_22L(win);
			break;
		case 23:
			//Test removed in CR1489
			break;
		case 24:
			//Test removed in CR1489
			break;
		case 25:
			//Test removed in CR1489
			break;
		case 26:
			//Test removed in CR1489
			break;
		case 27:
			//Test removed in CR1489
			break;
		case 28:
			//Test removed in CR1489
			break;
		case 29:
			//Test removed in CR1489
			break;
		case 30:
			//Test removed in CR1489
			break;
		case 31:
			Graphics_Wserv_Gce_31L(blankWin);
			break;
		case 32:
			Graphics_Wserv_Gce_32L(win);
			break;
		case 33:
			Graphics_Wserv_Gce_33L(win);
			break;
		case 34:
			Graphics_Wserv_Gce_34L(win);
			break;
		case 35:
			Graphics_Wserv_Gce_35L(win);
			break;
		case 36:
			Graphics_Wserv_Gce_36L(win);
			break;
		case 37:
			//Test removed in CR1489
			break;
		default:
			User::Panic(KT_BasicStepPanicTxt, KErrNotFound);  //Unexpected value!
		}

	__UHEAP_MARKEND;
	return TestStepResult();
	}

//Test setting a surface as the background of a window
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_1L(RWindow& aWin)
	{
	CreateSurfaceL(iSurfaceId);
	CreateRWindowL(aWin);
	TEST(KErrNone==aWin.SetBackgroundSurface(iSurfaceId));
	ForceWindowToRedraw(aWin);
	DestroySurface();
	aWin.Close();
	}
//Test setting a surface as the background of a RBlankWindow
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_2L(RBlankWindow& aBlankWin)
	{
	CreateSurfaceL(iSurfaceId);
	CreateRBlankWindowL(aBlankWin);
	TEST(KErrNone==aBlankWin.SetBackgroundSurface(iSurfaceId));
	ForceWindowToRedraw(aBlankWin);
	DestroySurface();
	aBlankWin.Close();
	}

//Negative test for setting a surface as the background of a RBackedUpWindow
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_3L()
	{
	CreateSurfaceL(iSurfaceId);
	RBackedUpWindow backedUpWindow( iWsSession );
	CleanupClosePushL( backedUpWindow );
	User::LeaveIfError( backedUpWindow.Construct( iWinGroup, EColor16MA, ENullWsHandle) );
	backedUpWindow.SetSizeErr( iScreen->SizeInPixels() );
	backedUpWindow.SetVisible( ETrue );
	backedUpWindow.Activate();
	iWsSession.Flush();
	User::After(delay);
	// Expect a panic here - TClientPanic::EWservPanicDrawable
	backedUpWindow.SetBackgroundSurface( iSurfaceId );
	DestroySurface();
	CleanupStack::PopAndDestroy( &backedUpWindow );
	}

//Test for moving a window with its background set
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_4L(RWindow& aWin)
	{
	// Create window and assign a surface to it
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	TSize scrSize(iScreen->SizeInPixels());
	// move the window around
	aWin.SetPosition( TPoint( (scrSize.iWidth)-KTempWindowWidth, 0) );
	iWsSession.Flush();
	User::After(delay);
	aWin.SetPosition( TPoint(aWin.Position().iX, (scrSize.iHeight)-KTempWindowHeight ));
	iWsSession.Flush();
	User::After(delay);
	aWin.SetPosition( TPoint(0, (scrSize.iHeight)-KTempWindowHeight) );
	iWsSession.Flush();
	User::After(delay);
	aWin.SetPosition(TPoint(0,0));
	iWsSession.Flush();
	User::After(delay);
	DestroySurface();
	aWin.Close();
		}

//Test moving a window with its background set partially and wholly offscreen
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_5L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	TSize screenSize(iScreen->SizeInPixels());
	// partially off screen
	TUint xCord=(screenSize.iWidth-KTempWindowWidth)+(KTempWindowWidth/2);
	TUint yCord=(screenSize.iHeight-KTempWindowHeight)+(KTempWindowHeight/2);
	aWin.SetPosition(TPoint(xCord,yCord));
	iWsSession.Flush();
	User::After(delay);
	// wholly off screen
	aWin.SetPosition( TPoint(screenSize.iWidth, screenSize.iHeight) );
	iWsSession.Flush();
	User::After(delay);
	DestroySurface();
	aWin.Close();
	}

//Test resizing a window with its background set to a surface
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_6L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	TSize sz(aWin.Size());
	aWin.SetSize( sz+TPoint(20,0) );
	iWsSession.Flush();
	User::After( delay );
	aWin.SetSize( sz+TPoint(0,20) );
	iWsSession.Flush();
	User::After( delay );
	aWin.SetSize( sz+TPoint(20,20) );
	iWsSession.Flush();
	User::After( delay );
	aWin.SetSize( TSize(KSurfaceWidth, KSurfaceHeight ) );
	iWsSession.Flush();
	User::After( delay );
  	DestroySurface();
	aWin.Close();
	}


// Test Opaque drawing in front of a background surface
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_11L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	// Opaque drawing on bkgd surface
	DrawShape( aWin, 0x0000FF00 );
	DestroySurface();
	aWin.Close();
	}

//Test Semi-transparent drawing in front of a background surface
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_13L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	// semi Transp on bkgd surface
	DrawShape( aWin, 0x8000FF00 );
	DestroySurface();
	aWin.Close();
	}

//Test that an opaque window can be put in front of an Rwindow with background surface
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_15L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	CleanupClosePushL(aWin);
	// draw 2nd window partially over the first one
	RWindow win2;
	CreateRWindowL(win2, TPoint(10,10), KRgbGreen );
	DestroySurface();
	CleanupStack::PopAndDestroy(&aWin);
	win2.Close();
	aWin.Close();
	}

//Test that a semi-transparent window can be put in front of a Rwindow with surface
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_17L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	CleanupClosePushL(aWin);
	// draw semi Transp win partially over first win with surface
	RWindow win2;
	CreateRWindowL(win2, TPoint(10,10));
	INFO_PRINTF1(_L("Destroy Surface"));
	DestroySurface();
	CleanupStack::PopAndDestroy(&aWin);
	win2.Close();
	aWin.Close();
	}

//Test updating a background surface
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_19L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	CleanupClosePushL(aWin);
	// create a 2nd surface
	TSurfaceId surfaceId2;
	CreateSurfaceL(surfaceId2, 0x0000FF00);
	ASSERT_EQUALS( KErrNone, aWin.SetBackgroundSurface(surfaceId2) );
	ForceWindowToRedraw(aWin);
	TInt ret = iSurfaceManager.CloseSurface(surfaceId2);
	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Surface manager failed to close surface"));
		}
	// destroys iSurfaceId plus closes iSurfaceUpdateSession,iChunk,iSurfaceManager
	DestroySurface();
	CleanupStack::PopAndDestroy(&aWin);
	}

//Test shape of a window is applied to window a surface
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_21L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	TRect rc[2] = { TRect(0,0,100,200),TRect(100,120,200,200) };
	RRegion region(2, rc);
	aWin.SetShape( region );
	User::After(delay);	// For DEBUG
	DestroySurface();
	aWin.Close();
	}

//Test that a valid key color is returned when a surface
//has been set as the background of a window
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_22L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	TRgb colValue(aWin.KeyColor());
	if ( aWin.DisplayMode()==EColor16MA || aWin.DisplayMode()==EColor16MAP )
		{
		TEST(aWin.KeyColor().Internal()==0x00000000);
		}
	else
		{
		TEST(aWin.KeyColor().Alpha()==0xFF);
		}
	aWin.Close();
	}

//Test that the use of SetColor() i.e. with no parameters removes
//any surface assigned to the window
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_31L(RBlankWindow& aBlankWin)
	{
	CreateSurfaceL(iSurfaceId);
	CreateRBlankWindowL(aBlankWin);
	ASSERT_EQUALS(KErrNone, aBlankWin.SetBackgroundSurface(iSurfaceId));
	ForceWindowToRedraw(aBlankWin);
	aBlankWin.SetColor();
	ForceWindowToRedraw(aBlankWin);
	DestroySurface();
	aBlankWin.Close();
	}

//Test that the shaped window maintains the surface when moved
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_32L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	TRect rc[2] = { TRect(0,0,100,200),TRect(100,120,200,200) };
	RRegion region(2, rc);
	aWin.SetShape( region );
	User::After(delay);
	aWin.SetPosition( TPoint( aWin.Position().iX+10, aWin.Position().iY+10 ) );
	iWsSession.Flush();
	User::After(delay);
	DestroySurface();
	aWin.Close();
	}

//Test that window and surface scale to the new size when a shaped
//window with surface is resized
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_33L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	TRect rc[2] = { TRect(0,0,100,200),TRect(100,120,200,200) };
	RRegion region(2, rc);
	aWin.SetShape( region );
	User::After(delay);
	aWin.SetSize( TSize(200,150) );
	iWsSession.Flush();
	User::After(delay);
	DestroySurface();
	aWin.Close();
	}

//Test that the background surface blends with the semi transparent window
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_34L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	CleanupClosePushL(aWin);
	ForceWindowToRedraw(aWin);
	RWindow win2;
	CreateRWindowL(win2,TPoint(10,0), 0x80A9B9C9);
	// draw semi Transp on window bkgd
	DrawShape( win2, 0x8000FF00 );
	DestroySurface();
	CleanupStack::PopAndDestroy(&aWin);
	win2.Close();
	aWin.Close();
	}

// Test that the corner style is applied to the window with surface
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_35L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	aWin.SetCornerType( EWindowCorner5 );
	User::After(delay);
	DestroySurface();
	aWin.Close();
	}

/***
Test that shaped windows with background surfaces arranged such that
the back window overlaps the front's bounding box, but not its shaped
region (e.g. the front is L-shaped and the back overlaps the area within
the 'L'). In this situation, the back window's surface should be visible
in the overlap area
*/
void CT_WServIntegStepBasic::Graphics_Wserv_Gce_36L(RWindow& aWin)
	{
	ASSERT_EQUALS( EPass, SetSurfaceL(aWin,iSurfaceId));
	CleanupClosePushL(aWin);
	TRect rc1[2] = { TRect(110,20,150,80),TRect(150,60,200,100) };
	RRegion region1(2, rc1);
	aWin.SetShape( region1 );
	User::After(delay);

	// create a 2nd surface
	TSurfaceId surfaceId2;
	CreateSurfaceL(surfaceId2,0x00ABCDEF);
	RWindow win2;
	CreateRWindowL(win2);
	ASSERT_EQUALS( KErrNone, win2.SetBackgroundSurface(surfaceId2));
	User::After(delay);
	TRect rc2[2] = { TRect(0,0,100,200),TRect(100,120,200,200) };
	RRegion region2(2, rc2);
	win2.SetShape( region2 );
	User::After(delay);

	TInt ret = iSurfaceManager.CloseSurface(surfaceId2);
	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Surface manager failed to close surface"));
		}
	// destroys iSurfaceId plus closes iSurfaceUpdateSession,iChunk,iSurfaceManager
	DestroySurface();
	CleanupStack::PopAndDestroy(&aWin);
	win2.Close();
	aWin.Close();
	}


/**
Creates a RWindow

@param aWin	The window object
@param aPos	The Position of the window
@param aBkgdColor The background color of the window
@param aSize The size of the window
*/
void CT_WServIntegStepBasic::CreateRWindowL(RWindow& aWin, const TPoint& aPos, const TRgb& aBkgdColor, const TSize& aWinSize)
	{
	aWin = RWindow( iWsSession );
	CleanupClosePushL( aWin );
	User::LeaveIfError( aWin.Construct( iWinGroup, ENullWsHandle ) );
	CleanupStack::Pop(&aWin);
	aWin.SetExtent(aPos, aWinSize);
	aWin.SetBackgroundColor( aBkgdColor );
	aWin.Activate();
	aWin.BeginRedraw();
	aWin.EndRedraw();
	aWin.SetVisible( ETrue );
	iWsSession.Flush();
	User::After(delay);
	}

/**
Creates a RBlankWindow

@param aBlankWin The window object
*/
void CT_WServIntegStepBasic::CreateRBlankWindowL(RBlankWindow& aBlankWin)
	{
	aBlankWin = RBlankWindow(iWsSession);
	CleanupClosePushL(aBlankWin);
	User::LeaveIfError(aBlankWin.Construct(iWinGroup, ENullWsHandle) );
	TSize sz( KTempWindowWidth, KTempWindowHeight );
	aBlankWin.SetSize(sz);
	aBlankWin.SetColor(TRgb(0x800000FF));	// 0xAABBGGRR
	aBlankWin.Activate();
	CleanupStack::Pop(&aBlankWin);
	iWsSession.Flush();
	User::After(delay);
	}

void CT_WServIntegStepBasic::ForceWindowToRedraw(RWindowBase& aWin)
	{
	aWin.SetVisible( EFalse );
	iWsSession.Flush();
	aWin.SetVisible( ETrue );
	iWsSession.Flush();
	User::After(delay);
	}

void CT_WServIntegStepBasic::CreateSurfaceManager()
	{
	INFO_PRINTF1(_L("Loading the device driver"));
	TInt ret = iSurfaceManager.Open();
	if(ret==KErrNone)
		{
		INFO_PRINTF1(_L("Creating surface manager OK"));
		}
	}

/**
Set up code for creating a surface and fill it with a color

@param aSurfaceId The surface object, to be initialized.
@param aColorPattern The color to fill the surface with.
*/
void CT_WServIntegStepBasic::CreateSurfaceL(TSurfaceId& aSurfaceId, TUint aColorPattern)
	{
	INFO_PRINTF1(_L("Creating a surface manager"));
	CreateSurfaceManager();

	INFO_PRINTF1(_L("Setting up surface attributes"));
	RSurfaceManager::TSurfaceCreationAttributesBuf attribs;
	RSurfaceManager::TSurfaceCreationAttributes& surfaceCreationAtribs=attribs();

	surfaceCreationAtribs.iSize.iWidth = KSurfaceWidth;
	surfaceCreationAtribs.iSize.iHeight = KSurfaceHeight;
	surfaceCreationAtribs.iBuffers = 1;
	surfaceCreationAtribs.iPixelFormat = KSurfaceFormat;
	surfaceCreationAtribs.iStride = KBytesPerPixel*KSurfaceWidth;
	surfaceCreationAtribs.iOffsetToFirstBuffer = 0;
	surfaceCreationAtribs.iAlignment = 4;
	surfaceCreationAtribs.iContiguous = EFalse;
	surfaceCreationAtribs.iMappable = ETrue;
	INFO_PRINTF1(_L("Call to CreateSurfaceL()"));
	TInt err = iSurfaceManager.CreateSurface(attribs, aSurfaceId);
	if (err == KErrNone)
		{
		//we have a surface, so map it in
		INFO_PRINTF1(_L("Surface created ok, mapping to it"));
		TInt err = iSurfaceManager.MapSurface(aSurfaceId, iChunk);
		User::LeaveIfError(err);
		TUint32* surfacePtr = reinterpret_cast<TUint32*>(iChunk.Base());
		TUint32* linePtr = surfacePtr;

		// Fill first line
		for (TInt xx = 0; xx < KSurfaceWidth; xx++)
			{
			surfacePtr[xx] = aColorPattern;
			}

		// Now copy that to the other lines
		TInt stride = KBytesPerPixel * KSurfaceWidth;
		for (TInt yy = 1; yy < KSurfaceHeight; yy++)
			{
			linePtr += KSurfaceWidth;
			Mem::Move(linePtr, surfacePtr, stride);
			}
		}

	INFO_PRINTF1(_L("Create Surface update session"));
	CreateSurfaceUpdateSessionL();
	err = iSurfaceUpdateSession.SubmitUpdate(KAllScreens, aSurfaceId, 0, NULL);
	if (err!=KErrNone)
		{
	   INFO_PRINTF1(_L("Fail in submitting update"));
		}
	}

void CT_WServIntegStepBasic::CreateSurfaceUpdateSessionL()
	{
	TInt ret = iSurfaceUpdateSession.Connect();

	if (ret==KErrAlreadyExists)
		{
		INFO_PRINTF1(_L("Device driver already loaded"));
		}
	else if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Connected to surface update server"));
		}
	else
		{
		INFO_PRINTF1(_L("Fatal error connecting to surface update server"));
		User::LeaveIfError(ret);
		}
	}

void CT_WServIntegStepBasic::DestroySurface()
	{
	INFO_PRINTF1(_L("Destroy Surface update session"));
	iSurfaceUpdateSession.Close();

	//close the chunk
	INFO_PRINTF1(_L("Closing chunk"));
	iChunk.Close();

	INFO_PRINTF1(_L("Closing surface"));
	TInt ret = iSurfaceManager.CloseSurface(iSurfaceId);
	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Surface manager failed to close"));
		}

	INFO_PRINTF1(_L("Destroy Surface Manager"));
	iSurfaceManager.Close();
	}

/**
Common set up code for assigning a surface to the window. Calls other utility
functions for creating a window and a surface

@param aWin	The window object
@param aSurfaceId	The surface object, to be initialized.
*/
TVerdict CT_WServIntegStepBasic::SetSurfaceL(RWindow& aWin, TSurfaceId& aSurfaceId)
	{
	TVerdict verdict=EPass;
	CreateRWindowL(aWin);
	CleanupClosePushL(aWin);
	CreateSurfaceL( aSurfaceId );
	CleanupStack::Pop(&aWin);
	if ( aWin.SetBackgroundSurface(aSurfaceId)!=KErrNone)
		{
		verdict=EFail;
		}
	iWsSession.Flush();
	ForceWindowToRedraw(aWin);
	return verdict;
	}

/**
Draw an Ellipse shape

@param aWin	The window object, connected to a session
@param aColor The (transparent/opaque)color which the shape is filled with
*/
void CT_WServIntegStepBasic::DrawShape(RWindow& aWin, const TRgb& aColor)
	{
   	iGc->Activate( aWin );
   	iGc->SetBrushColor( aColor );
   	iGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
   	TRect rect(TPoint(0,0), TPoint(aWin.Size().iWidth, aWin.Size().iHeight));
   	iGc->DrawEllipse(rect);
   	iWsSession.Flush();
   	User::After(delay);
   	iGc->Deactivate();
	}

