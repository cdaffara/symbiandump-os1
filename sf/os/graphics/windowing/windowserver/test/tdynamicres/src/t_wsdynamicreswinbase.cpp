// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/


#include <e32std.h>
#include <e32math.h>
#include <w32std.h>
#include <w32debug.h>
#include "teflogextensions.h"
#include "t_wsdynamicreswinbase.h"
#include "globalsettings.h"
#include <bitdraw.h>
#include <bitdrawinterfaceid.h>
#include <u32hal.h>
#include <dispchannel.h>
#include <graphics/displaycontrol.h>

#if (!defined(K_DISPLAY_CH_MAJOR_VERSION_NUMBER) && !defined(K_DISPLAY_CH_MINOR_VERSION_NUMBER))
#define MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER
#endif

_LIT(KMonospaceTestFontTypefaceName,"Arial");
const TInt KMaxFontSize = 150;
TBool	CWsDynamicResWinBase::iTransparencyEnabled=EFalse;

CWsDynamicResWinBase::CWsDynamicResWinBase():
	iDoTearDown(EFalse),
	iSession(TGlobalSettings::Instance().iScreen)
{
}

void CWsDynamicResWinBase::SetupL()
	{
	SetupL(EFalse);
	}
CActiveScheduler	CWsDynamicResWinBase::iScheduler;

void CWsDynamicResWinBase::SetupL(TBool aUseOtherScreenForInfo)
	{	
	if (CActiveScheduler::Current()!=&iScheduler)
		{
		new (&iScheduler) CActiveScheduler;
		CActiveScheduler::Install(&iScheduler);
		}
	iDoTearDown=ETrue;
	iRed.SetInternal(0xFFFF0000);
	iGreen.SetInternal(0xFF00FF00);
	iBlue.SetInternal(0xFF0000FF);
	iCyan.SetInternal(0xFF00FFFF);
	iMagenta.SetInternal(0xFFFF00FF);
	iYellow.SetInternal(0xFFFFFF00);
	iWhite.SetInternal(0xFFFFFFFF);
	iLastGceHoleColor.SetInternal(0);
	TITLE_BACKGROUND=iCyan;
	COMPARE_BACKGROUND=iBlue;

	ASSERT_EQUALS_X(iSession.Connect(), KErrNone);

		{//Stolen from TAuto CloseAllPanicWindows()
		TInt idFocus = iSession.GetFocusWindowGroup();
		TWsEvent event;
		event.SetType(EEventKey); //EEventKeyDown
		TKeyEvent *keyEvent = event.Key();
		keyEvent->iCode = EKeyEscape;
		keyEvent->iScanCode = EStdKeyEscape;
		keyEvent->iModifiers = 0;
		TInt theLimit = 50;
		while(idFocus != NULL && (theLimit-- > 0))
			{
			iSession.SendEventToAllWindowGroups(event);
			TInt idNewFocus = iSession.GetFocusWindowGroup();
			if (idNewFocus!=idFocus)
				{
				INFO_PRINTF1(_L("A window was closed [probably a panic box from the previous test]."));
				}
			idFocus=idNewFocus;
			}
		}
	TInt err = KErrNone;

	TRAP(err, iScreenDevice = new (ELeave) CWsScreenDevice(iSession));
	PRINT_ON_ERROR2_L(err, _L("Failed to create screen device: %d"), err);
	ASSERT_EQUALS_X(iScreenDevice->Construct(TGlobalSettings::Instance().iScreen), KErrNone);
	iDisplayMode = iScreenDevice->DisplayMode();	// Get default display mode

	CheckAndConnectScreen();

	TRAP(err, iGc = new (ELeave) CWindowGc(iScreenDevice));
	PRINT_ON_ERROR2_L(err, _L("Failed to create graphics context: %d"), err);
	ASSERT_EQUALS_X(iGc->Construct(), KErrNone);
	
	iGroup = RWindowGroup(iSession);
	ASSERT_EQUALS_X(iGroup.Construct(++iWindowHandle,iScreenDevice), KErrNone);
	iGroup.SetOrdinalPositionErr(0, KPasswordWindowGroupPriority - 1); // Added code ---- Fastpath
	iSession.Flush();
	
	if (aUseOtherScreenForInfo)
		{
		if (iSession.NumberOfScreens()>1)
			{	//Create server objects for info windows to appear on alternate screen
			TInt alternateScreenNum=iSession.NumberOfScreens()-1;
			if (TGlobalSettings::Instance().iScreen==alternateScreenNum)
				{	//Alternate screen is last screen, or first screen if that is being tested. 
				alternateScreenNum=0;
				}
			
			TRAP(err, iInfoScreenDevice = new (ELeave) CWsScreenDevice(iSession));
			PRINT_ON_ERROR2_L(err, _L("Failed to create second screen device: %d"), err);
			ASSERT_EQUALS_X(iInfoScreenDevice->Construct(alternateScreenNum), KErrNone);
	
			TRAP(err, iInfoGc = new (ELeave) CWindowGc(iInfoScreenDevice));
			PRINT_ON_ERROR2_L(err, _L("Failed to create second graphics context: %d"), err);
			ASSERT_EQUALS_X(iInfoGc->Construct(), KErrNone);

			iInfoGroupInstance = RWindowGroup(iSession);
			ASSERT_EQUALS_X(iInfoGroupInstance.Construct(++iWindowHandle,iInfoScreenDevice), KErrNone);
			iInfoGroup=&iInfoGroupInstance;
			}
		else
			{	//If alternate screen is not available then no text or compare windows should be created!
			iInfoScreenDevice=iScreenDevice;	//it is "convenient" for the device to still be good.
			iInfoGc=NULL;
			iInfoGroup=NULL;
			}
		}
	else
		{	//
		iInfoScreenDevice=iScreenDevice;
		iInfoGc=iGc;
		iInfoGroup=&iGroup;
		}
	
	if (iInfoGroup && iInfoGc)
		{
		// Add a plain background window to obscure anything else that
		// happens to be behind the test. Setting this window's display mode is also
		// used to set the screen device display mode, and hence the composition
		// mode: alpha or chroma key.
		iBackground = RBlankWindow(iSession);
		ASSERT_EQUALS_X(iBackground.Construct(*iInfoGroup, ++iWindowHandle), KErrNone);
	
		iBackground.SetOrdinalPosition(100);	// Behind anything else in this group.
		iBackground.SetColor(TRgb(iWhite));
		//iBackground.SetExtent(TPoint(-1000,-1000),TSize(3000,3000));
		iBackground.Activate();
		iBackground.SetVisible(ETrue);
		}
	iSession.Flush();
	
	RWindow testTrans(iSession);
	ASSERT_EQUALS_X(testTrans.Construct(iGroup, ++iWindowHandle), KErrNone);
	iTransparencyEnabled=(testTrans.SetTransparencyFactor(iWhite)==KErrNone);
	if (iTransparencyEnabled)
		{
		TTestName testName;
		testName.Format(_L("Screen %i, depth %i: Found Trans Man"),
				TGlobalSettings::Instance().iScreen
			);
		UpdateTitleWindowL(testName,KMaxInfoLines-1);
		}
	else
		{
		TTestName testName;
		testName.Format(_L("Screen %i, depth %i: No Trans Man"),
				TGlobalSettings::Instance().iScreen
			);
		UpdateTitleWindowL(testName,KMaxInfoLines-1);
		}
	testTrans.Close();
	}


void CWsDynamicResWinBase::CheckAndConnectScreen()
	{
	if (TGlobalSettings::Instance().iDisconnected) //set from script file to match wsini keyword SIMULATE_STARTUP_DISCONNECTED
		{
		//Verify that the display really is disconnected
		ASSERT_TRUE_X(iScreenDevice != NULL);
		MDisplayControl* interface = static_cast<MDisplayControl*>(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
		ASSERT_TRUE_X(interface != NULL);
		RArray<MDisplayControl::TResolution> resolutions;
		const TInt err = interface->GetResolutions(resolutions);
		ASSERT_EQUALS_X(err, KErrDisconnected);
		resolutions.Close();
		}
	
#ifndef MODE_CHANGE_BASE_FUNCTIONALITY_NOT_PRESENT_IN_HEADER
	//make sure display is attached to screen (only if I/F is available at compile time...)
	TInt displayState = ENormalResolution;
	UserSvr::HalFunction(EHalGroupDisplay | (TGlobalSettings::Instance().iScreen<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	Pause(200);
#endif	
	
	if (TGlobalSettings::Instance().iDisconnected)
		{
		//Verify that the display now is connected
		MDisplayControl* interface = static_cast<MDisplayControl*>(iScreenDevice->GetInterface(MDisplayControl::ETypeId));
		RArray<MDisplayControl::TResolution> resolutions;
		const TInt err = interface->GetResolutions(resolutions);
		ASSERT_EQUALS_X(err, KErrNone);
		const_cast<TGlobalSettings&>(TGlobalSettings::Instance()).iDisconnected = EFalse;
		resolutions.Close();
		}
	}

/**
Common tear down code for all tests.

Windows, group and session created are closed. Screen device is destroyed.
Surfaces, manager and update session are closed.
*/
void CWsDynamicResWinBase::TearDownL()
	{
	iDoTearDown=EFalse;
	if (iInfoGc!=iGc)
		delete iInfoGc;
	delete iGc;
	if (iInfoScreenDevice!=iScreenDevice)
		delete iInfoScreenDevice;
	delete iScreenDevice;

	iGroup.Close();
	if (iInfoGroupInstance.WsHandle())
		iInfoGroupInstance.Close();
	iSession.Flush();
	iSession.Close();
	}
/**
 * Note that this is not the ideal mechanism.
 * A derived class may thinks its TearDown is safe to do from delete, but in the class it is derived from it may not be safe
 **/
void CWsDynamicResWinBase::TearDownFromDeleteL()	
	{
	CWsDynamicResWinBase::TearDownL();	//Explicitly call the non-derived implementation.
	}

CWsDynamicResWinBase::~CWsDynamicResWinBase()
{
	if (iDoTearDown)
		TearDownFromDeleteL();	//This mechanism is not entirely clean to use.
}
/**
Pause for the given number of milliseconds.

@param aMilliseconds	Time to wait in milliseconds.
*/
void CWsDynamicResWinBase::Pause(TInt aMilliseconds)
	{
	User::After(TTimeIntervalMicroSeconds32(aMilliseconds * 1000));
	}
// This handles any non-member uses of the extended ASSERT_XXX macros
void TefUnitFailLeaveL()
	{
	
	User::Leave(KErrTEFUnitFail);
	}
/**
 * side-effect: log the state info just before I leave!
 * Note that this only logs intentional assertion failures.
 * Fails due to panics or throws won't log this info.
 **/
void CWsDynamicResWinBase::TefUnitFailLeaveL()
	{
	for (TInt line=0;line<KMaxInfoLines;line++)
		if (iTestInfo[line].Length())
			Logger().LogExtra((TText8*)"Test state at fail - line",line, ESevrAll, iTestInfo[line]);
	
	User::Leave(KErrTEFUnitFail);
	}

/**	Creates the LHS info window, annd a middle window to display a representation of the expected result.
 *  Also sets up a rectangle representing the space on the right to be used for the test
 * 	@param	aTitle	The title to display on the info window
 *  @param	aDetail	Optional text to display on the first line under the title
**/
void CWsDynamicResWinBase::MakeTitleAndCompareWindowsL(TRefByValue<const TDesC16> aTitle,TRefByValue<const TDesC16> aDetail)
	{	
	iTestName=aTitle;
	iTestInfo[0]=aDetail;
	
	TRect screenSize(iInfoScreenDevice->SizeInPixels());
	TPoint oneThird(screenSize.iBr.iX/3,screenSize.iBr.iY/3);
	TRect winSize(0,0,oneThird.iX,oneThird.iY);
	
	if (oneThird.iX>oneThird.iY)
		{
		oneThird.iY=0;
		winSize.iBr.iY=screenSize.iBr.iY;
		}
	else
		{
		oneThird.iX=0;
		winSize.iBr.iX=screenSize.iBr.iX;
		}
	winSize.Shrink(5,5);

	if (iInfoGc)
		{
		iTitle=RWindow(iSession);
		ASSERT_EQUALS_X(iTitle.Construct(*iInfoGroup, ++iWindowHandle), KErrNone);
		iTitle.SetBackgroundColor(iCyan);
		iTitle.SetExtent(winSize.iTl,winSize.Size());
		iTitle.Activate();
		
		RepaintTitleWindowL();
		iTitle.SetVisible(ETrue);
	
		winSize.Move(oneThird);
		iCompare=RWindow(iSession);
		ASSERT_EQUALS_X(iCompare.Construct(*iInfoGroup, ++iWindowHandle), KErrNone);
		iCompare.SetBackgroundColor(COMPARE_BACKGROUND);
		iCompare.SetExtent(winSize.iTl,winSize.Size());
		iCompare.Activate();
		iCompare.BeginRedraw();
		ActivateWithWipe(iInfoGc,iCompare,COMPARE_BACKGROUND);
	
		TFontSpec fspec(KMonospaceTestFontTypefaceName,KMaxFontSize);
		CFont *font=NULL;
		ASSERT_EQUALS(iScreenDevice->GetNearestFontToDesignHeightInTwips(font,fspec),KErrNone);
		iInfoGc->UseFont(font);
		iInfoGc->DrawText(_L("Simulation"),winSize.Size(),winSize.Size().iHeight-5,iGc->ECenter);
	
		iInfoGc->Deactivate();
		iCompare.EndRedraw();
		iCompare.SetVisible(ETrue);
		if (iScreenDevice!=iInfoScreenDevice)
			{
			winSize.Move(-oneThird);
			}
		else
			{
			winSize.Move(oneThird);
			}
		}
	else
		{
		winSize=iScreenDevice->SizeInPixels();
		}
	
	iTestPos=winSize;
	iTestPointCentre=winSize.Center();
	iCenteredFrontWinRect=winSize;
	iCenteredFrontWinRect.Shrink(winSize.Size().iWidth/3,winSize.Size().iHeight/3);

	}
/**	Makes the compare window larger by covering the test window area as well.
 * 	Copes with vertically aligned screens, but will be naughty if called multiple times!!!
 *  @param aGoLarger	If set false, resets the size back.
 **/
void CWsDynamicResWinBase::LargerCompareWindow(TBool aGoLarger)
	{
	TPoint currPos=iCompare.AbsPosition();
	TSize currSize=iCompare.Size();
	if (currPos.iX<currPos.iY)
		{
		if (aGoLarger)
			currSize.iHeight<<=1;
		else
			currSize.iHeight>>=1;
		}
	else
		{
		if (aGoLarger)
			currSize.iWidth<<=1;
		else
			currSize.iWidth>>=1;
		}
	iCompare.SetSize(currSize);
	}
	

/**	Puts a line of text on the LHS window.
 *  Repaints the window. The line of text will also be shown in the log if the test fails.
 *	@param	aDetail	The text to display
 * 	@param	aIndex	The row number to display at
 **/
void CWsDynamicResWinBase::UpdateTitleWindowL(TRefByValue<const TDesC16> aDetail,TInt aIndex)
	{
	ASSERT(aIndex>=0 && aIndex<KMaxInfoLines);
	iTestInfo[aIndex]=aDetail;

	RepaintTitleWindowL();
	
	}
/**	Activate the GC onto the Window.
 *  In non-transparent mode it also performs a wipe background as the WServ system does not necessarily do this.
 * 	@param	aGc		The GC to associate
 * 	@param	aWin	The window to associate
 * 	@param	aColor	The color to use as the wipe. Default is transparent, which means no wipe.
 **/
TBool	CWsDynamicResWinBase::ActivateWithWipe(CWindowGc* aGc,RWindow& aWin,TRgb aColor)
	{
	aGc->Activate(aWin);
	aGc->SetBrushColor(aColor);
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	if (aColor!=TRgb(0,0) && !iTransparencyEnabled)	//presume that all redraw-stored windows will draw background
		{
		aGc->Clear();
		return ETrue;	//window was cleared
		}
	return EFalse;	
	}

CWindowGc*	CWsDynamicResWinBase::GcForWindow(RWindow& aWin)
	{
	if (aWin.WsHandle()==NULL)
		return NULL;	//can't activate uninitialised window.
	CWindowGc* gc=iGc;
	if (iGc!=iInfoGc)
		if (&aWin==&iCompare || &aWin==&iTitle)
			gc=iInfoGc;
		else if (iInfoGroup && aWin.WindowGroupId()==iInfoGroup->WindowGroupId())
			gc=iInfoGc;
	return gc;
	}
/** Activates an appropriate predefined GC on the specified window and wipes the background if necessary. 
 *  @param	aWin	The window to wipe
 * 	@param	aColor	The colour to wipe with (if necessary)
 * 	@return	the GC to use for drawing and deactivate at end. This may be NULL if the window is not "live"
 **/
CWindowGc*	CWsDynamicResWinBase::BeginActivateWithWipe(TBool aInvalidate,RWindow& aWin,TRgb aColor)
	{
	CWindowGc* gc=GcForWindow(aWin);
	iSession.Flush();
	if (gc==NULL)
		return gc;	//can't activate uninitialised window.
	if (aInvalidate)
		aWin.Invalidate();
	iSession.Flush();
	aWin.BeginRedraw();
	iSession.Flush();
	ActivateWithWipe(gc,aWin,aColor);
	return gc;	
	}

/** Activates an appropriate predefined GC on the specified window and wipes the background if necessary. 
 *  @param	aWin	The window to wipe
 * 	@param	aColor	The colour to wipe with (if necessary)
 * 	@return	the GC to use for drawing and deactivate at end. This may be NULL if the window is not "live"
 **/
CWindowGc*	CWsDynamicResWinBase::BeginActivateWithWipe(TBool aInvalidate,TRect aRect,RWindow& aWin,TRgb aColor)
	{
	if (aWin.WsHandle()==NULL)
		return NULL;	//can't activate uninitialised window.
	if (aInvalidate)
		aWin.Invalidate(aRect);
	aWin.BeginRedraw(aRect);
	CWindowGc* gc=iGc;
	if (iGc!=iInfoGc)
		if (&aWin==&iCompare || &aWin==&iTitle)
			gc=iInfoGc;
		else if (aWin.WindowGroupId()==iInfoGroup->WindowGroupId())
			gc=iInfoGc;
	ActivateWithWipe(gc,aWin,aColor);
	return gc;	
	}

TBool		CWsDynamicResWinBase::InvalidateRegion(const TRegion& aRegion,RWindow& aWin)
	{
	if (aWin.WsHandle()==NULL)
		return false;	//can't activate uninitialised window.
	for (TInt ii = 0; ii < aRegion.Count(); ii++)
		{
		aWin.Invalidate(aRegion[ii]);
		}
	return true;
	}

CWindowGc*	CWsDynamicResWinBase::BeginActivateWithWipe(const TRegion& aRegion,RWindow& aWin,TRgb aColor)
	{
	if (!InvalidateRegion(aRegion,aWin))
		return NULL;	//can't activate uninitialised window.

	aWin.BeginRedraw();
	CWindowGc* gc=iGc;
	if (iGc!=iInfoGc)
		if (&aWin==&iCompare || &aWin==&iTitle)
			gc=iInfoGc;
		else if (aWin.WindowGroupId()==iInfoGroup->WindowGroupId())
			gc=iInfoGc;
	ActivateWithWipe(gc,aWin,aColor);
	return gc;	
	}


/**	Paints the LHS window with rows of text.
 * 
 **/
void CWsDynamicResWinBase::RepaintTitleWindowL()
	{
	if (iTitle.WsHandle())
		{
		iTitle.Invalidate();
	
		iTitle.BeginRedraw();
		ActivateWithWipe(iInfoGc,iTitle,TITLE_BACKGROUND);
		iInfoGc->SetUnderlineStyle(EUnderlineOn);
		TSize winSize=iTitle.Size();
		TRect textRect(winSize);
		textRect.iBr.iY/=4;
		TFontSpec fspec(KMonospaceTestFontTypefaceName,KMaxFontSize);
		CFont *font=NULL;
		ASSERT_EQUALS(iInfoScreenDevice->GetNearestFontToDesignHeightInTwips(font,fspec),KErrNone);
		iInfoGc->UseFont(font);
		iInfoGc->DrawText(iTestName,textRect,textRect.iBr.iY/2,iGc->ECenter);
		iInfoGc->SetUnderlineStyle(EUnderlineOff);
		textRect.iTl.iY=textRect.iBr.iY;
		TInt rowHeight=winSize.iHeight*3/(4*(KMaxInfoLines+1));
		textRect.iBr.iY+=rowHeight;
		for (TInt index=0;index<KMaxInfoLines;index++)
			{
			if (iTestInfo[index].Length())
			    {
				iInfoGc->DrawText(iTestInfo[index],textRect,textRect.Size().iHeight*3/4,iInfoGc->ECenter);
			    }
			textRect.Move(0,rowHeight);
			}
		iInfoGc->DiscardFont();
		iInfoGc->Deactivate();
		iTitle.EndRedraw();
		iInfoScreenDevice->ReleaseFont(font);
	
		iSession.Flush();
		iSession.Finish();
		}
	}

/** Useful test culled from other GCE test classes.
 * 	
 * 
 * 
 **/
TBool CWsDynamicResWinBase::DisplayHasAlpha() const
	{
	return (iDisplayMode == EColor16MA || iDisplayMode == EColor16MAP);
	}
/**	Test using an indipendent method that GCE version of WServ is running
 *  This method can only be called after the testcase is started
 * 
 * 	@return	true if WServ version is GCE technology, false if legacy technology
 **/
TBool CWsDynamicResWinBase::GCEIsSupported() const
	{
	CFbsDrawDevice* screenDevice=NULL;
	TDisplayMode displayMode=iScreenDevice->DisplayMode();
	TRAPD(err, screenDevice = CFbsDrawDevice::NewScreenDeviceL(TGlobalSettings::Instance().iScreen, displayMode));
	TBool rv=EFalse;
	if(err == KErrNone)
		{
		TAny* p=NULL;
		rv=(screenDevice->GetInterface(KSurfaceInterfaceID, p)==KErrNone);
		delete screenDevice;
		}
	return rv;
	}
/**	Test using an indipendent method that GCE version of WServ is running
 *  This method can be called at any time, even by external code, but creates temporary window session objects
 * 
 * 	@return	true if WServ version is GCE technology, false if legacy technology
 **/
TBool CWsDynamicResWinBase::GCEIsSupportedStatic()
	{
	CFbsDrawDevice* screenDevice=NULL;
	RWsSession session;
	if (session.Connect()!=KErrNone)
		{
		return EFalse;
		}
	TDisplayMode displayMode=ENone;
		{CWsScreenDevice screen(session);
		if (screen.Construct(TGlobalSettings::Instance().iScreen)!=KErrNone)
			{
			return EFalse;
			}
			displayMode=screen.DisplayMode();
		}//screen destroyed
		TRAPD(err, screenDevice = CFbsDrawDevice::NewScreenDeviceL(TGlobalSettings::Instance().iScreen, displayMode));
		TBool rv=EFalse;
		if(err == KErrNone)
			{
			TAny* p=NULL;
			rv=(screenDevice->GetInterface(KSurfaceInterfaceID, p)==KErrNone);
			delete screenDevice;
			}
	return rv;
	}//session destroyed
/**
Use the full-screen background window to select a display mode that doesn't use
alpha (anything other than EColor16MA or EColor16MAP). Record the mode for use
in setting all other windows.
@return ETrue if an appropriate mode was selected, EFalse otherwise.
*/
TBool CWsDynamicResWinBase::SelectChromaCompositionMode()
	{
	// Request EColor64K, but as long as the actual mode doesn't use alpha, it
	// doesn't matter too much which one is used.
	if (iInfoGc==iGc)
		{
		iDisplayMode = (TDisplayMode)iBackground.SetRequiredDisplayMode(EColor64K);
		iSession.Flush();		// Force switching to the display mode.
		}
	return !DisplayHasAlpha();
	}
/**	Returns the colour used by WServ to paint holes in UI layer to reveal the GCE behind.
 * 	The window should have a surface attached.
 * 	If the method is called after the surface has been detached or the window was removed then 
 * 	the previous recorded hole color is returned.
 **/
TRgb  CWsDynamicResWinBase::GceHoleColor( RWindowBase& aWin)const
	{
	if (aWin.WsHandle()==NULL)
		{
		return iLastGceHoleColor;
		}
	TRgb retVal=aWin.KeyColor();
	if (retVal==TRgb(0,0))
		{
		return iLastGceHoleColor;
		}
	else
		{
		iLastGceHoleColor=retVal;
		return retVal;
		}
	}


/**
Use the full-screen background window to select a display mode that can use
alpha (either EColor16MA or EColor16MAP). Record the mode for use in setting all
other windows.
@return ETrue if an appropriate mode was selected, EFalse otherwise.
*/
TBool CWsDynamicResWinBase::SelectAlphaCompositionMode(TDisplayMode aMode)
	{
	// Request EColor16MA, but as long as the actual mode can use alpha, it
	// doesn't matter too much which one is used.
	if (iInfoGc==iGc)
		{
		iDisplayMode = (TDisplayMode)iBackground.SetRequiredDisplayMode(aMode);
		iSession.Flush();		// Force switching to the display mode.
		}
	return DisplayHasAlpha();
	}

/**
 * 	Interesting UI pattern used by other GCE tests.
 * 
 * 
 **/
void CWsDynamicResWinBase::DrawUIContent(RWindow& aWindow)
	{
	aWindow.BeginRedraw();
	CWindowGc* gc=(&aWindow==&iCompare)?iInfoGc:iGc;
	
	gc->Activate(aWindow);

	TBool hasAlpha = DisplayHasAlpha();

	// Draw a red-green graduated box in the central portion of the window,
	// with alpha if available.
	TPoint start;
	TPoint end;
	TInt halfW = KSurfaceWidth / 2;
	TInt quarterW = halfW / 2;
	TInt halfH = KSurfaceHeight / 2;
	TInt quarterH = halfH / 2;

	// Set constant ordinals for non-alpha case.
	start.iX = quarterW;
	end.iX = quarterW + halfW;

	for (TInt yy = 0; yy < halfH; yy++)
		{
		TInt yval = yy * 255 / (halfH - 1);
		start.iY = yy + quarterH;
		end.iY = start.iY;

		if (hasAlpha)
			{
			for (TInt xx = 0; xx < halfW; xx++)
				{
				TInt xval = xx * 255 / (halfW - 1);
				start.iX = xx + quarterW;
				end.iX = start.iX + 1;
				gc->SetPenColor(TRgb(yval, 255 - yval, 0, xval));
				gc->DrawLine(start, end);
				}
			}
		else
			{
			gc->SetPenColor(TRgb(yval, 255 - yval, 0));
			gc->DrawLine(start, end);
			}
		}

	gc->Deactivate();
	aWindow.EndRedraw();
	}

/**
 * 	Causes the given window to be redrawn.
 * 	Doesn't draw anything except the background wipe, when the transparency manager hasn't
 * 
 **/
void CWsDynamicResWinBase::DrawPlainUI(RWindow& aWindow,TBool aInvalidate,TRgb aWipeColor)
	{
	if (CWindowGc* gc=BeginActivateWithWipe(aInvalidate,aWindow,aWipeColor))
		{
		//actually does nothing!
		gc->Deactivate();
		aWindow.EndRedraw();
		}
	}
/**
 *  	Interesting UI pattern used by other GCE tests.
 * 
 * 
 **/
void CWsDynamicResWinBase::DrawCross(RWindow& aWindow, TRgb aColor, TInt aThickness)
	{
	aWindow.BeginRedraw();
	CWindowGc* gc=(&aWindow==&iCompare)?iInfoGc:iGc;
	gc->Activate(aWindow);

	// Draw a red diagonal cross in the window.
	gc->SetPenColor(aColor);
	gc->SetPenSize(TSize(aThickness, aThickness));
	gc->DrawLine(TPoint(0, 0), TPoint(KSurfaceWidth, KSurfaceHeight));
	gc->DrawLine(TPoint(KSurfaceWidth, 0), TPoint(0, KSurfaceHeight));

	gc->Deactivate();
	aWindow.EndRedraw();
	}

/**
 * 	Checks the RGB value
 * 
 * 
 **/
void CWsDynamicResWinBase::TestPixelL(TPoint aPt, TRgb aColor, TBool aMatch)
	{
	TRect screenArea(iScreenDevice->SizeInPixels());
	if (aPt.iX < screenArea.iTl.iX)
		{
		aPt.iX = screenArea.iTl.iX;
		}
	else if (aPt.iX >= screenArea.iBr.iX)
		{
		aPt.iX = screenArea.iBr.iX - 1;
		}
	if (aPt.iY < screenArea.iTl.iY)
		{
		aPt.iY = screenArea.iTl.iY;
		}
	else if (aPt.iY >= screenArea.iBr.iY)
		{
		aPt.iY = screenArea.iBr.iY - 1;
		}

	TRgb pixel;
	iScreenDevice->GetPixel(pixel, aPt);
	if (aMatch)
		{
		ASSERT_EQUALS_X(pixel.Internal(), aColor.Internal());
		}
	else
		{
		ASSERT_NOT_EQUALS_X(pixel.Internal(), aColor.Internal());
		}
	}

struct CountColour
	{
	TRgb	iColor;
	TInt	iCount;
	TBool operator  < (const CountColour& aRhs)const	
		{	return iColor.Value()<aRhs.iColor.Value();	}
	
	CountColour(TRgb	aColor,TInt	aCount=0): 
			iColor(aColor),iCount(aCount) {}
	
	CountColour(const CountColour& aRhs): 
			iColor(aRhs.iColor),iCount(aRhs.iCount) {}
	};

void LogColorL(TRgb aPixel,RArray<CountColour>& aColors)
	{
	//I am sure one of the find methods would do this, but life is too short!
	TInt existingIndex;
	for (existingIndex=0;existingIndex<aColors.Count();existingIndex++)
		if (aColors[existingIndex].iColor==aPixel)
			break;
	if (existingIndex==aColors.Count())
		aColors.AppendL(CountColour(aPixel,1));
	else
		aColors[existingIndex].iCount++;
	}

/**
 * Tests the outline of the given rectangle to verify that it has the expected inner and outer colors
 * There may be a limited number of other colors present.
 * If the given count of other colors is exceeded then false is returned
 * If the majority of inner and outer edge pixels are not the given colors then returns false
 * If the  colors are repeated on inside and outside then returns false
 * If there are corners then the insides of the corners are also check for outside color
 * Note that the right and bottom edge coordinates are outside the rectangle.
 * Note that I am not performing any safety clipping at present. The rectangle must be on-screen!
 * A negative value for corner size indicates that the window may or may not have corners that size.
 * Only leaves if fatal memory condition!
**/
TBool CWsDynamicResWinBase::TestRectL(TRect aRect,TRgb aInnerColor,TInt aOtherInnerColors,TRgb aOuterColor,TInt aOtherOuterColors,TInt aExpectedCornerSize)
	{
	if (aRect.iTl.iX<=0 || aRect.iTl.iY<=0)
		return ETrue;	//can't perform the test against the outside of the screen
	//quantise the expectation based on the current mode.
	TDisplayMode displayMode=iScreenDevice->DisplayMode();
	switch (displayMode)
		{
	case EColor4K:
		aInnerColor=aInnerColor.Color4K(aInnerColor.Color4K());
		aOuterColor=aOuterColor.Color4K(aOuterColor.Color4K());
		break;
	case EColor64K:
		aInnerColor=aInnerColor.Color64K(aInnerColor.Color64K());
		aOuterColor=aOuterColor.Color64K(aOuterColor.Color64K());
		break;
	case EColor16M:
	case EColor16MU:
	case EColor16MA:
	case EColor16MAP:
		break;
	default:
		ASSERT_EQUALS(EColor16MA,displayMode);		//Can't quantise color for this display mode!
		}

	RArray<CountColour> innerColors;
	innerColors.AppendL(aInnerColor);
	RArray<CountColour> outerColors;
	outerColors.AppendL(aOuterColor);
	TInt cornerSize=aExpectedCornerSize>=0?aExpectedCornerSize:-aExpectedCornerSize;
	//Check outside first!
	TRgb pixelVal;
	for(TPoint pixelPos(aRect.iTl.iX-1,aRect.iTl.iY-1);pixelPos.iX<aRect.iBr.iX;pixelPos.iX++)
		{
		iScreenDevice->GetPixel(pixelVal, pixelPos);
		LogColorL(pixelVal,outerColors);
		}
	for(TPoint pixelPos(aRect.iTl.iX,aRect.iBr.iY);pixelPos.iX<=aRect.iBr.iX;pixelPos.iX++)
		{
		iScreenDevice->GetPixel(pixelVal, pixelPos);
		LogColorL(pixelVal,outerColors);
		}
	for(TPoint pixelPos(aRect.iTl.iX-1,aRect.iTl.iY);pixelPos.iY<=aRect.iBr.iY;pixelPos.iY++)
		{
		iScreenDevice->GetPixel(pixelVal, pixelPos);
		LogColorL(pixelVal,outerColors);
		}
	for(TPoint pixelPos(aRect.iBr.iX,aRect.iTl.iY-1);pixelPos.iY<aRect.iBr.iY;pixelPos.iY++)
		{
		iScreenDevice->GetPixel(pixelVal, pixelPos);
		LogColorL(pixelVal,outerColors);
		}
	TInt cornerStart=1;
	if (cornerSize)
		{
		cornerStart=cornerSize;
		if (aExpectedCornerSize>0)
			{
			iScreenDevice->GetPixel(pixelVal, aRect.iTl);
			LogColorL(pixelVal,outerColors);
			iScreenDevice->GetPixel(pixelVal, TPoint(aRect.iTl.iX,aRect.iBr.iY-1));
			LogColorL(pixelVal,outerColors);
			iScreenDevice->GetPixel(pixelVal, TPoint(aRect.iBr.iX-1,aRect.iTl.iY));
			LogColorL(pixelVal,outerColors);
			iScreenDevice->GetPixel(pixelVal, TPoint(aRect.iBr.iX-1,aRect.iBr.iY-1));
			LogColorL(pixelVal,outerColors);
			}
		}
	
	//test inside edges (excluding 4 corner pixels - do them seperately)
	for(TPoint pixelPos(aRect.iTl.iX+cornerStart,aRect.iTl.iY);pixelPos.iX<aRect.iBr.iX-cornerStart;pixelPos.iX++)
		{
		iScreenDevice->GetPixel(pixelVal, pixelPos);
		LogColorL(pixelVal,innerColors);
		}
	for(TPoint pixelPos(aRect.iTl.iX+cornerStart,aRect.iBr.iY-1);pixelPos.iX<aRect.iBr.iX-cornerStart;pixelPos.iX++)
		{
		iScreenDevice->GetPixel(pixelVal, pixelPos);
		LogColorL(pixelVal,innerColors);
		}
	for(TPoint pixelPos(aRect.iTl.iX,aRect.iTl.iY+cornerStart);pixelPos.iY<aRect.iBr.iY-cornerStart;pixelPos.iY++)
		{
		iScreenDevice->GetPixel(pixelVal, pixelPos);
		LogColorL(pixelVal,innerColors);
		}
	for(TPoint pixelPos(aRect.iBr.iX-1,aRect.iTl.iY+cornerStart);pixelPos.iY<aRect.iBr.iY-cornerStart;pixelPos.iY++)
		{
		iScreenDevice->GetPixel(pixelVal, pixelPos);
		LogColorL(pixelVal,innerColors);
		}
	//the 4 corner cells - not checking the whole corner area...
	if (aExpectedCornerSize>=0)
		{
		iScreenDevice->GetPixel(pixelVal, TPoint(aRect.iTl.iX+cornerSize,aRect.iTl.iY+cornerSize));
		LogColorL(pixelVal,innerColors);
		iScreenDevice->GetPixel(pixelVal, TPoint(aRect.iTl.iX+cornerSize,aRect.iBr.iY-1-cornerSize));
		LogColorL(pixelVal,innerColors);
		iScreenDevice->GetPixel(pixelVal, TPoint(aRect.iBr.iX-1-cornerSize,aRect.iBr.iY-1-cornerSize));
		LogColorL(pixelVal,innerColors);
		iScreenDevice->GetPixel(pixelVal, TPoint(aRect.iBr.iX-1-cornerSize,aRect.iTl.iY+cornerSize));
		LogColorL(pixelVal,innerColors);
		}
	//OK... that has tested all the pixels, now check the result
	if (innerColors.Count()>aOtherInnerColors+1)
		return EFalse;
	if (outerColors.Count()>aOtherOuterColors+1)
		return EFalse;
	for (TInt index=1;index<innerColors.Count();index++)
		if (innerColors[0].iCount<innerColors[index].iCount)
			{
			return EFalse;
			}
	for (TInt index=1;index<outerColors.Count();index++)
		if (outerColors[0].iCount<outerColors[index].iCount)
			{
			return EFalse;
			}
	for (TInt indexIn=1;indexIn<innerColors.Count();indexIn++)
		for (TInt indexOut=0;indexOut<outerColors.Count();indexOut++)
			if (innerColors[indexIn].iColor.Value()==outerColors[indexOut].iColor.Value())
				{
				return EFalse;
				}
	return ETrue;
	}

TRect CWsDynamicResWinBase::PentCellRect(const TRect& aFullRect,char aStartLetter,char aEndLetter)
	{
	if (aEndLetter==0)
		aEndLetter=aStartLetter;
	aStartLetter&=0x1f;
	aEndLetter&=0x1f;
	TInt startx=(aStartLetter-1)%5;
	TInt starty=(aStartLetter-1)/5;
	TInt endx=(aEndLetter-1)%5;
	TInt endy=(aEndLetter-1)/5;
	if (starty>endy)
		{	//swap			//	s		e
		starty-=endy;		//	s-e
		endy+=starty;		//			s					
		starty=endy-starty;	//	e
		}
	if (startx>endx)
		{	//swap			//	s		e
		startx-=endx;		//	s-e
		endx+=startx;		//			s					
		startx=endx-startx;	//	e
		}
	TSize fullSize=aFullRect.Size();
	return TRect(	aFullRect.iTl.iX+fullSize.iWidth*startx/5,
					aFullRect.iTl.iY+fullSize.iHeight*starty/5,
					aFullRect.iTl.iX+fullSize.iWidth*(endx+1)/5,
					aFullRect.iTl.iY+fullSize.iHeight*(endy+1)/5
				);
	}
