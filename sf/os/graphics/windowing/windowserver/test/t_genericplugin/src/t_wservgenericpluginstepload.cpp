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
 @internalComponent
*/

#include "t_wservgenericpluginstepload.h"
#include <tefunit.h>
#include <e32const.h>	//KNullUidValue
#include <gdi.h>
#include <stdlib.h>

const TPoint KWinPos(0,0);
const TSize KWinSize(100,100);
const TRect KWinRect(KWinPos,KWinSize);
const TRect KEllipseRect(TPoint(10,10), TSize(50,50));

_LIT(KDefaultPluginBmp, "Z:\\wstest\\genericplugin\\defaultplugin.mbm");
_LIT(KTestPluginBmp, "Z:\\wstest\\genericplugin\\testplugin.mbm");
_LIT(KTWServGenericPluginStepTestId,"testid");

/**
Constructor of CT_WServGenericpluginStepLoad
*/
CT_WServGenericpluginStepLoad::CT_WServGenericpluginStepLoad()
	{
	SetTestStepName(KT_WServGenericpluginStepLoad);
	}

/**
Destructor of CT_WServGenericpluginStepLoad
*/
CT_WServGenericpluginStepLoad::~CT_WServGenericpluginStepLoad()
	{
	delete iGc;
	delete iScreen;
	delete iScreen1;
	delete iDefaultPluginBmp;
	delete iTestPluginBmp;
	iWinGroup.Close();
	iWinGroup1.Close();
	iWsSession.Flush();
	iWsSession.Close();
	}

/**
Checks if message is logged in the log file.
@param	aMsg The message to be checked.
@return ETrue if the message is found in the log file, otherwise EFalse.
*/
TInt CT_WServGenericpluginStepLoad::CheckLogL(TDesC8& aMsg)
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	RFile logFile;
	CleanupClosePushL(logFile);

	TInt ret = logFile.Open(fs, KLogFileName, EFileShareAny|EFileRead);
	if (ret == KErrNone)
		{
		TInt fileSize;
    	User::LeaveIfError(logFile.Size(fileSize));

		HBufC8* buf = HBufC8::NewLC(fileSize);
		TPtr8 ptr(buf->Des());
		ret = logFile.Read(ptr);
		if (ret == KErrNone)
			{
			ret = ptr.Find(aMsg);
			if (ret != KErrNotFound)
				{
				ret = EMsgFound;
				}
			else
				{
				ret = EMsgNotFound;
				}		
			}
		CleanupStack::PopAndDestroy(buf);	
		}
	else if (ret == KErrNotFound)
		{
		ret = EFileNotExist;
		}
	CleanupStack::PopAndDestroy(&logFile);
	CleanupStack::PopAndDestroy(&fs);
	return ret;
	}

/**
Overrides test step preamble.
*/
enum TVerdict CT_WServGenericpluginStepLoad::doTestStepPreambleL()
	{
	TVerdict ret = CTestStep::doTestStepPreambleL();
	if ( !GetIntFromConfig( ConfigSection(), KTWServGenericPluginStepTestId, iTestId ))
		{
		User::Leave(KErrNotFound);
		}
	else
		{
		iDisplayMode = EColor64K;
		User::LeaveIfError(iWsSession.Connect());

		iScreen = new (ELeave) CWsScreenDevice(iWsSession);
		User::LeaveIfError(iScreen->Construct());
		iWinGroup = RWindowGroup(iWsSession);
		User::LeaveIfError(iWinGroup.Construct(KNullWsHandle, iScreen));
		iWinGroup.AutoForeground(ETrue);
		iGc = new (ELeave) CWindowGc(iScreen);
		User::LeaveIfError(iGc->Construct());

		iScreen1 = new (ELeave) CWsScreenDevice(iWsSession);
		User::LeaveIfError(iScreen1->Construct(1));
		iWinGroup1 = RWindowGroup(iWsSession);
		User::LeaveIfError(iWinGroup1.Construct(KNullWsHandle, iScreen1) );
		iWinGroup1.AutoForeground(ETrue);
	
		iDefaultPluginBmp = new(ELeave) CFbsBitmap;
		User::LeaveIfError(iDefaultPluginBmp->Load(KDefaultPluginBmp));
		iTestPluginBmp = new(ELeave) CFbsBitmap;
		User::LeaveIfError(iTestPluginBmp->Load(KTestPluginBmp));
		iWsSession.Flush();
		}
	return ret;
	}

/**
Overrides test step prostamble.
*/
enum TVerdict CT_WServGenericpluginStepLoad::doTestStepPostambleL()
	{
	return TestStepResult();
	}

/**
Starts test step
@return TVerdict pass / fail
*/
enum TVerdict CT_WServGenericpluginStepLoad::doTestStepL()
	{
	__UHEAP_MARK;
	RWindow win;
	CleanupClosePushL(win);
	CreateRWindowL(iWinGroup, win, KWinPos, KRgbBlue, KWinSize);
	DrawShape(win);
	
	RWindow win1;
	CleanupClosePushL(win1);
	CreateRWindowL(iWinGroup1, win1, KWinPos, KRgbBlue, KWinSize);
	DrawShape(win1);
	
	switch( iTestId )
		{
		case 1:
			INFO_PRINTF1(_L("Testing Control of CWsPlugin loading from WSINI.INI..."));
			GraphicsWservGenericpluginLoad1L();
			break;
		case 2:	
			INFO_PRINTF1(_L("Testing that plugins can be specified on a per-screen basis through WSINI.INI file..."));
			GraphicsWservGenericpluginLoad2L();
			break;
		case 3:	
			INFO_PRINTF1(_L("Testing Integer and string attributes in WSINI.INI file can be read from CWsPlugin..."));
			GraphicsWservGenericpluginLoad3L();
			break;
		case 4:
			INFO_PRINTF1(_L("Testing CWsPlugin can gain information about closing windows using MWsWindow interface..."));
			GraphicsWservGenericpluginLoad4L();
			break;
		case 5:	
			INFO_PRINTF1(_L("Testing CWsPlugin can obtain instance of another CWPlugin..."));
			GraphicsWservGenericpluginLoad5L();
			break;
		default:
			break;	
		}
	CleanupStack::PopAndDestroy(2,&win);

	__UHEAP_MARKEND;
	return TestStepResult();
	}

/**
Creates a RWindow
@param aWinGroup The window group object
@param aWin	The window object
@param aPos	The Position of the window
@param aBkgdColor The background color of the window
@param aSize The size of the window
@param aHandle The handle of the window
*/
void CT_WServGenericpluginStepLoad::CreateRWindowL(const RWindowGroup& aWinGroup, 
												   RWindow& aWin, 
												   const TPoint& aPos, 
												   const TRgb& aBkgdColor, 
												   const TSize& aWinSize,
												   const TUint32 aHandle)
	{
	aWin = RWindow( iWsSession );
	CleanupClosePushL( aWin );
	User::LeaveIfError( aWin.Construct( aWinGroup, aHandle ) );
	CleanupStack::Pop(&aWin);
	aWin.SetRequiredDisplayMode(iDisplayMode);
	aWin.SetExtent(aPos, aWinSize);
	aWin.SetBackgroundColor( aBkgdColor );
	aWin.Activate();
	aWin.SetVisible( ETrue );
	}

/**
Draw an ellipse to the window and fade the window.
@param aWin The window object
*/
void CT_WServGenericpluginStepLoad::DrawShape(RWindow& aWin)
	{
	TUint8 white = 255;
	TUint8 black = 128;
	
	iGc->Activate(aWin);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(KRgbWhite);
	iGc->DrawEllipse(KEllipseRect);
	iWsSession.Flush();	
	User::After(KDelay);
	
	aWin.SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly,black,white);
	iWsSession.Flush();	
	User::After(KDelay);

	iGc->Deactivate();
	}

/**
Compare the window with the bitmap.
@param aScreen The screen device object
@param aBitmap The bitmap object for comparison
@return ETrue if the window and the bitmap is identified. Otherwise return EFalse.
*/
TBool CT_WServGenericpluginStepLoad::CompareDisplayL(CWsScreenDevice* aScreen, CFbsBitmap* aBitmap)
	{
	// Capture window display to bitmap
	CFbsBitmap* screenBitmap = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(screenBitmap);
	User::LeaveIfError(screenBitmap->Create(KWinRect.Size(), iDisplayMode));
	User::LeaveIfError(aScreen->CopyScreenToBitmap(screenBitmap, KWinRect));	
	
	//Compare the window bitmap with the bitmap pass in for comparison
	TBool ret = ETrue;
	
	const TReal KErrorLimit = 0.05;
	
	TInt mismatchedPixels = 0;
	TRgb testWinPix = TRgb(0,0,0,0);
	TRgb checkWinPix = TRgb(0,0,0,0);
	for (TInt x = 0; x < KWinRect.Width(); x++)
		{
		for (TInt y = 0; y < KWinRect.Height(); y++)
			{
			screenBitmap->GetPixel(testWinPix, TPoint(x,y));
			aBitmap->GetPixel(checkWinPix, TPoint(x,y));
			
			//check if there are differeces between test Window colors and check Window colors
			if(((TReal)abs(testWinPix.Red() - checkWinPix.Red())/255) > KErrorLimit || 
					((TReal)abs(testWinPix.Blue() - checkWinPix.Blue())/255) > KErrorLimit || 
					((TReal)abs(testWinPix.Green() - checkWinPix.Green())/255) > KErrorLimit || 
					((TReal)abs(testWinPix.Alpha() - checkWinPix.Alpha())/255) > KErrorLimit)
				{
				mismatchedPixels++;  // -- Useful for debugging
				ret = EFalse;
				break;
				}
			}
		}
	
	/* INFO_PRINTF2(_L("Number of different pixels: %i"), mismatchedPixels); */ // -- Useful for debugging
	

	CleanupStack::PopAndDestroy(screenBitmap);	
	
	
	
	return ret;
	}

/**
Calculate the portion of window displayed on the screen (absRect).
@param aAbsRect The size of the window displayed on the screen
@param aPos The origin of the window
@param aWinSize The size of the window
*/
void CT_WServGenericpluginStepLoad::CalcAbsRect(TSize& aAbsRect, const TPoint& aPos, const TSize& aWinSize)
	{
	if((aPos.iX + aWinSize.iWidth) > iScreen->SizeInPixels().iWidth)
		{
		aAbsRect.iWidth = iScreen->SizeInPixels().iWidth - aPos.iX;
		}	
	else if(aPos.iX < 0)
		{
		aAbsRect.iWidth = aPos.iX + aWinSize.iWidth;
		}
	else
		{
		aAbsRect.iWidth = aWinSize.iWidth;
		}
	if((aPos.iY + aWinSize.iHeight) > iScreen->SizeInPixels().iHeight)
		{
		aAbsRect.iHeight = iScreen->SizeInPixels().iHeight - aPos.iY;
		}
	else if(aPos.iY < 0)
		{
		aAbsRect.iHeight = aPos.iY + aWinSize.iHeight;
		}
	else
		{
		aAbsRect.iHeight = aWinSize.iHeight;
		}
	}

/**
@SYMTestCaseID			graphics-wserv-1841-0001
@SYMPREQ				1841
@SYMTestCaseDesc		Control of CWsPlugin loading from WSINI.INI			
@SYMTestActions			tests alternative fader specified by an ID and alternative 
						render stage specified by TYPE in wsini.ini file are loaded
@SYMTestStatus			Implemented
@SYMTestPriority		2
@SYMTestExpectedResults	Alternative plugins are loaded as specified.
						Window created in screen 0 is faded with custom fader and 
						two lines are drawn on the left top window by custom render stage.
@SYMTestType			CT
*/	
void CT_WServGenericpluginStepLoad::GraphicsWservGenericpluginLoad1L()
	{
	//Compare the window with the bitmap which uses test plugin. They are identical.
	TEST(CompareDisplayL(iScreen, iTestPluginBmp));
	//Check the log file. Test fader and test render stage are created.
	TBuf8<255> msg(_L8("TestMWsIniFile is created."));
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("TestRenderStageFactory is created.");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("TestRenderStage is created.");
	TEST(CheckLogL(msg) == EMsgFound);
	}

/**
@SYMTestCaseID			graphics-wserv-1841-0002
@SYMPREQ				1841
@SYMTestCaseDesc		Test that plugins can be specified on a per-screen basis through WSINI.INI file
@SYMTestActions			Create modified WSINI.INI file which specifies different fader and render stage
						plugins to be used on screens 0 and 1.
@SYMTestStatus			Implemented
@SYMTestPriority		2
@SYMTestExpectedResults	Both sets of plugins are loaded, and the correct plugin is used on each screen.
						Windows created in different screen are faded with different color.
						Windows in screen 0 have two lines drawn on left top window by custom render stage 
						while windows in screen 1 use standard render stage.
@SYMTestType			CT
*/
void CT_WServGenericpluginStepLoad::GraphicsWservGenericpluginLoad2L()
	{
	//Test drawing in screen 0. testfader and testRenderStage are used.
	//Compare the window with the bitmap which uses default plugin.
	TEST(!CompareDisplayL(iScreen, iDefaultPluginBmp));
	//Compare the window with the bitmap which uses test plugins.
	TEST(CompareDisplayL(iScreen, iTestPluginBmp));

	//Test drawing in screen 1. testfader_data and testrenderstage_invalid are used.
	//TestRenderStage_Invalid is not created because NULL is returned from CreateStageL.
	
	//Check the log file. 
	TBuf8<255> msg(_L8("TestMWsIniFile is created."));
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("TestRenderStageFactory is created.");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("TestRenderStage is created.");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("TestRenderStage_invalid is created.");
	TEST(CheckLogL(msg) == EMsgNotFound);
	}

/**
@SYMTestCaseID			graphics-wserv-1841-0004
@SYMPREQ				1841
@SYMTestCaseDesc		Integer and string attributes in WSINI.INI file can be read from CWsPlugin. 			
@SYMTestActions			Create modified WSINI.INI file which specifies a test fader to be loaded to screen 0.
						Specifies integer and string variables for default, custom, and screen sections
@SYMTestStatus			Implemented
@SYMTestPriority		2
@SYMTestExpectedResults	Alternative plug-in is loaded as specified and attributes can be accessed from plug-in.
@SYMTestType			CT
*/	
void CT_WServGenericpluginStepLoad::GraphicsWservGenericpluginLoad3L()
	{
	//Check the log file.
	TBuf8<255> msg;
	msg = _L8("Screen Section has correct integer data");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Screen Section has correct string data");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Custom Section has correct integer data");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Custom Section has correct string data");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Default Section has correct integer data");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Default Section has correct string data");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Default Section does not have missing attribute");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Screen Section does not have missing attribute");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Custom Section does not have missing attribute");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Default Section - Integer data retrieved with method for string attribute");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Default Section - Could not access string attribute with method for integer attribute");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Screen Section - Integer data retrieved with method for string attribute");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Screen Section - Could not access string attribute with method for integer attribute");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Custom Section - Integer data retrieved with method for string attribute");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Custom Section - Could not access string attribute with method for integer attribute");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Could not access variables because screen does not exist");
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Could not access variables because section does not exist");
	TEST(CheckLogL(msg) == EMsgFound);
	}

/**
@SYMTestCaseID			graphics-wserv-1841-0005
@SYMPREQ				1841
@SYMTestCaseDesc		CWsPlugin can gain information about closing windows using MWsWindow interface.
@SYMTestActions			Create modified WSINI.INI file which specifies a test renderer to be loaded.
						Register CWsPLugin as eventhandler receiving EWindowClosing events.
@SYMTestStatus			Implemented
@SYMTestPriority		2
@SYMTestExpectedResults	Plugin can access information about closing windows through MWsWindow interface.
						Windows with different size and position are created and closed in screen 0.
@SYMTestType			CT
*/	
void CT_WServGenericpluginStepLoad::GraphicsWservGenericpluginLoad4L()
	{
	const TUint32 KWin1WsHandle = 0xFFFFFFFC;
	const TUint32 KWin2WsHandle = 0xFFFFFFFD;
	const TUint32 KWin3WsHandle = 0xFFFFFFFE;
	TPoint w1Point(50,240);
	TPoint w2Point(580,100);
	TPoint w3Point(25,100);
	TSize w1Size(10,10);
	TSize w2Size(150,70);
	TSize w3Size(50,140);
		
	//Draw windows with different origins and sizes	
	RWindow win;	
	CleanupClosePushL(win);
	CreateRWindowL(iWinGroup, win, w1Point, KRgbBlue, w1Size, KWin1WsHandle);
	DrawShape(win);
	CleanupStack::PopAndDestroy(&win);
	
	CleanupClosePushL(win);
	CreateRWindowL(iWinGroup, win, w2Point, KRgbBlue, w2Size, KWin2WsHandle);
	DrawShape(win);
	CleanupStack::PopAndDestroy(&win);
	
	CleanupClosePushL(win);
	CreateRWindowL(iWinGroup, win, w3Point, KRgbBlue, w3Size, KWin3WsHandle);
	DrawShape(win);
	CleanupStack::PopAndDestroy(&win);
	
	CleanupClosePushL(win); //create dummy 4th window to ensure window3 closing event has time to log info
	CreateRWindowL(iWinGroup, win, w2Point, KRgbBlue, w2Size, KWin2WsHandle);
	DrawShape(win);
	CleanupStack::PopAndDestroy(&win);
	
	//Calculate AbsRect values
	TSize absRect1;
	TSize absRect2;
	TSize absRect3;
	CalcAbsRect(absRect1, w1Point, w1Size);	
	CalcAbsRect(absRect2, w2Point, w2Size);	
	CalcAbsRect(absRect3, w3Point, w3Size);
	
	TBuf8<255> msg;
	//check log for window1 info
	msg.Format(_L8("Closing Window Handle %d - Origin: %d, %d"), KWin1WsHandle, w1Point.iX, w1Point.iY);
	TEST(CheckLogL(msg) == EMsgFound);
	msg.Format(_L8("Closing Window Handle %d - AbsRec: Height %d, Width %d"), KWin1WsHandle, absRect1.iHeight, absRect1.iWidth);
	TEST(CheckLogL(msg) == EMsgFound);
	msg.Format(_L8("Closing Window Handle %d - Size: Height %d, Width %d"), KWin1WsHandle, w1Size.iHeight, w1Size.iWidth);
	TEST(CheckLogL(msg) == EMsgFound);
	//check log for window2 info
	msg.Format(_L8("Closing Window Handle %d - Origin: %d, %d"), KWin2WsHandle, w2Point.iX, w2Point.iY);
	TEST(CheckLogL(msg) == EMsgFound);
	msg.Format(_L8("Closing Window Handle %d - AbsRec: Height %d, Width %d"), KWin2WsHandle, absRect2.iHeight, absRect2.iWidth);
	TEST(CheckLogL(msg) == EMsgFound);
	msg.Format(_L8("Closing Window Handle %d - Size: Height %d, Width %d"), KWin2WsHandle, w2Size.iHeight, w2Size.iWidth);
	TEST(CheckLogL(msg) == EMsgFound);
	//check log for window3 info
	msg.Format(_L8("Closing Window Handle %d - Origin: %d, %d"), KWin3WsHandle, w3Point.iX, w3Point.iY);
	TEST(CheckLogL(msg) == EMsgFound);
	msg.Format(_L8("Closing Window Handle %d - AbsRec: Height %d, Width %d"), KWin3WsHandle, absRect3.iHeight, absRect3.iWidth);
	TEST(CheckLogL(msg) == EMsgFound);
	msg.Format(_L8("Closing Window Handle %d - Size: Height %d, Width %d"), KWin3WsHandle, w3Size.iHeight, w3Size.iWidth);
	TEST(CheckLogL(msg) == EMsgFound);
	}

/**
@SYMTestCaseID			graphics-wserv-1841-0006
@SYMPREQ				1841
@SYMTestCaseDesc		CWsPlugin can obtain instance of another CWPlugin.
@SYMTestActions			Create modified WSINI.INI file which specifies a test renderer, test fader, and service plug-in to be loaded.
						Service plugin offers elementary service to other plug-ins.
@SYMTestStatus			Implemented
@SYMTestPriority		2
@SYMTestExpectedResults	Test fader can access information set in service plugin by test renderer.
@SYMTestType			CT
*/
void CT_WServGenericpluginStepLoad::GraphicsWservGenericpluginLoad5L()
	{
	//Check the log file.
	TBuf8<255> msg;
	msg = _L8("Fading parameters have been set.");	
	TEST(CheckLogL(msg) == EMsgFound);
	msg = _L8("Returned fade color.");	
	TEST(CheckLogL(msg) == EMsgFound);
	}

