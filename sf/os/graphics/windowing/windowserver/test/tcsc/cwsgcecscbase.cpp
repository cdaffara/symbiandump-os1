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
// @file
// 
//

#include <e32std.h>
#include <e32math.h>
#include <w32std.h>
#include <w32debug.h>
#include <e32base.h>
#include "teflogextensions.h"
#include "globalsettings.h"
#include "CWsGceCscBase.h"
#include <bitdraw.h>
#include <bitdrawinterfaceid.h>
#include <imageconversion.h>
#include <test/tefunit.h>


CWsGceCscBase::CWsGceCscBase():
iDoTearDown(EFalse),
iUtility(this, NULL)
{
}

CWsGceCscBase::~CWsGceCscBase()
{
if (iDoTearDown)
	TearDownFromDeleteL();	//This mechanism is not entirely clean to use.
}

void CWsGceCscBase::SetupL()
{
iDoTearDown=ETrue;
iRed.SetInternal(0xFFFF0000);
iGreen.SetInternal(0xFF00FF00);
iBlue.SetInternal(0xFF0000FF);
iCyan.SetInternal(0xFF00FFFF);
iMagenta.SetInternal(0xFFFF00FF);
iYellow.SetInternal(0xFFFFFF00);
iWhite.SetInternal(0xFFFFFFFF);

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

TRAP(err, iGc = new (ELeave) CWindowGc(iScreenDevice));
PRINT_ON_ERROR2_L(err, _L("Failed to create graphics context: %d"), err);
ASSERT_EQUALS_X(iGc->Construct(), KErrNone);

iGroup = RWindowGroup(iSession);
ASSERT_EQUALS_X(iGroup.Construct(++iWindowHandle,iScreenDevice), KErrNone);
iSession.Flush();

RWindow testTrans(iSession);
ASSERT_EQUALS_X(testTrans.Construct(iGroup, ++iWindowHandle), KErrNone);
iTransparencyEnabled=(testTrans.SetTransparencyFactor(iWhite)==KErrNone);
testTrans.Close();

//clean-up if previous test abended
	if (PostTestCleanupInstance().iSharedUtility)
	{
//Temp removed - may be causing ONB fails!
//	if (PostTestCleanupInstance().iSharedUtility->DestroyAll())
//		{
//		INFO_PRINTF1(_L("Destroyed some surfaces from previous test."));
//
//		}
	}
	if (!PostTestCleanupInstance().iCleanedUpOnExit)
	PostTestCleanupInstance().iCleanedUpOnExit=EFalse;
	
	if (!GCEIsSupported())
		{
		INFO_PRINTF1(_L("Some Setup skipped: GCE support is not loaded"));
		return;
		}

	TRAPD(err_FailedToCreateSurfaceUtility, iUtility = CSurfaceUtility::NewL( PostTestCleanupInstance().iSharedUtility));
	ASSERT_EQUALS(err_FailedToCreateSurfaceUtility,KErrNone);

}

void CWsGceCscBase::TearDownL()
{
iDoTearDown=EFalse;
delete iGc;
delete iScreenDevice;

iGroup.Close();
iSession.Flush();
iSession.Close();

delete iUtility();
PostTestCleanupInstance().iCleanedUpOnExit=ETrue;
Pause(1000);
}

void CWsGceCscBase::TearDownFromDeleteL()
{
CWsGceCscBase::TearDownL();
}

void CWsGceCscBase::Pause(TInt aMilliseconds)
{
User::After(TTimeIntervalMicroSeconds32(aMilliseconds * 1000));
}

/**	Test using an indipendent method that GCE version of WServ is running
 *  This method can only be called after the testcase is started
 * 
 * 	@return	true if WServ version is GCE technology, false if legacy technology
 **/
TBool CWsGceCscBase::GCEIsSupported() const
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
TBool CWsGceCscBase::GCEIsSupportedStatic()
	{
	CFbsDrawDevice* screenDevice=NULL;
	RWsSession session;
	if (session.Connect()!=KErrNone)
		{
		return EFalse;
		}
	CWsScreenDevice screen(session);
	if (screen.Construct(TGlobalSettings::Instance().iScreen)!=KErrNone)
		{
		return EFalse;
		}
	
	TDisplayMode displayMode=screen.DisplayMode();
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

//Allocating an instance of surface utility here means all test code instances will share the same instance of the utility class.
// Owns the singleton
CWsGceCscBase::TPostTestCleanup& CWsGceCscBase::PostTestCleanupInstance()
	{
	static 
	class RPostTestCleanup:public TPostTestCleanup
		{
		public:
		RPostTestCleanup()
			{
			iSharedUtility=NULL;
			iCleanedUpOnExit=ETrue;
			}
		~RPostTestCleanup()
			{
	// I want to cleanly release the surface utility, but at this point the threads have already been pulled down!
	//		if (iSharedUtility)
	//			delete iSharedUtility;
			iSharedUtility=NULL;	//avoid phoenix behaviour
			}
		}	staticInstance;
		return staticInstance;
	}

// This handles any non-member uses of the extended ASSERT_XXX macros
extern void TefUnitFailLeaveL();
/**
 * side-effect: log the state info just before I leave!
 * Note that this only logs intentional assertion failures.
 * Fails due to panics or throws won't log this info.
 **/
void CWsGceCscBase::TefUnitFailLeaveL()
	{
	for (TInt line=0;line<KMaxInfoLines;line++)
		if (iTestInfo[line].Length())
			Logger().LogExtra((TText8*)"Test state at fail - line",line, ESevrAll, iTestInfo[line]);
	TGlobalSettings::Instance().Report(Logger());
	
	User::Leave(KErrTEFUnitFail);
	}

TInt	CWsGceCscBase::DebugInfo(TWsDebugInfoFunc aFunction, 
		                                 TInt aParam, 
		                                 TDes8& aHostBuffer,
		                                 const void*&aReturnedObject,
		                                 TInt aObjectSize)const
	{
	TInt reqSize=iSession.DebugInfo(aFunction,aHostBuffer,aParam);
	aReturnedObject=NULL;
	if (reqSize<0)
		{
		if ((reqSize%aObjectSize)!=0)
			{
			return KErrCorrupt;
			}
		else
			{
			return reqSize;
			}
		}
	if (reqSize>aHostBuffer.MaxLength())
		{
		return reqSize/aObjectSize;
		}
	if (reqSize==0)
		{
		reqSize=aHostBuffer.MaxLength();
		}
	if ((reqSize%aObjectSize)!=0)
		{
		return KErrCorrupt;
		}
	aReturnedObject=(const void*)aHostBuffer.Ptr();
	reqSize/=aObjectSize;
	return reqSize;
	}

void CWsGceCscBase::SetCompositionMode(RWindow& aWindow, TDisplayMode aMode)
	{
		iDisplayMode = (TDisplayMode)aWindow.SetRequiredDisplayMode(aMode);
		aWindow.Invalidate();
		aWindow.BeginRedraw();
		aWindow.EndRedraw();
	}

TBool CWsGceCscBase::Compare(CFbsBitmap& aBitmap1, CFbsBitmap& aBitmap2)
	{
	if ((aBitmap1.SizeInPixels() == aBitmap2.SizeInPixels()) &&
		(aBitmap1.DisplayMode() == aBitmap2.DisplayMode()))
		{
		TSize size = aBitmap1.SizeInPixels();
		TInt width = size.iWidth;
		TInt height = size.iHeight;
		TRgb color1, color2;
		
		for (TInt i = 0; i < width; i++)
			{
			for (TInt j = 0; j < height; j++)
				{
				aBitmap1.GetPixel(color1, TPoint(i, j));
				aBitmap2.GetPixel(color2, TPoint(i, j));
				if (color1 != color2)
					{
					return EFalse;
					}
				}
			}
		
		return ETrue;
		}
	return EFalse;
	}

void CWsGceCscBase::ConstructOpaqueWindowL(RWindow& aWindow, 
		                                   const TRgb& aColor, 
		                                   TInt aPos,
		                                   const TPoint &aPoint, 
		                                   const TSize &aSize)
	{
	ASSERT_EQUALS_X(aWindow.Construct(iGroup, ++iWindowHandle), KErrNone);
	aWindow.SetRequiredDisplayMode(iDisplayMode);
	aWindow.SetExtent(aPoint, aSize);
	aWindow.SetBackgroundColor(aColor);
	aWindow.SetOrdinalPosition(aPos);	
	aWindow.Activate();
	aWindow.BeginRedraw();
	iGc->Activate(aWindow);
	iGc->Deactivate();
	aWindow.EndRedraw();
	}

void CWsGceCscBase::ConstructOpaqueWindowLC(RWindow& aWindow, 
		                                    const TRgb& aColor, 
		                                    TInt aPos,
		                                    const TPoint &aPoint, 
		                                    const TSize &aSize)
	{
	ASSERT_EQUALS_X(aWindow.Construct(iGroup, ++iWindowHandle), KErrNone);
	CleanupClosePushL(aWindow);
	aWindow.SetRequiredDisplayMode(iDisplayMode);
	aWindow.SetExtent(aPoint, aSize);
	aWindow.SetBackgroundColor(aColor);
	aWindow.SetOrdinalPosition(aPos);
	aWindow.SetVisible(ETrue);
	aWindow.Activate();
	aWindow.BeginRedraw();
	iGc->Activate(aWindow);
	iGc->Clear();
	iGc->Deactivate();
	aWindow.EndRedraw();
	}


void CWsGceCscBase::SetBackgroundColorWindow(RWindow& aWindow, const TRgb& aColor)
	{
	aWindow.Invalidate();
	aWindow.SetBackgroundColor(aColor);
	aWindow.BeginRedraw();
	iGc->Activate(aWindow);
	iGc->Deactivate();
	aWindow.EndRedraw();
	}

void CWsGceCscBase::ConstructOpaqueSurfacedWindowLC(RWindow& aWindow,
		                                            const TRgb& aColor, 
		                                            TInt aPos,
		                                            const TPoint &aPoint, 
		                                            const TSize &aSize,
		                                            TSurfaceId& aSurfaceID)
	{
	ASSERT_EQUALS_X(aWindow.Construct(iGroup, ++iWindowHandle), KErrNone);
	CleanupClosePushL(aWindow);
	aWindow.SetRequiredDisplayMode(iDisplayMode);
	aWindow.SetExtent(aPoint, aSize);
	aWindow.SetBackgroundColor(aColor);
	aWindow.SetOrdinalPosition(aPos);	
	ASSERT_EQUALS_X(aWindow.SetBackgroundSurface(aSurfaceID), KErrNone);
	aWindow.Activate();
	aWindow.BeginRedraw();
	iGc->Activate(aWindow);
	iGc->Deactivate();
	aWindow.EndRedraw();
	}

void CWsGceCscBase::ConstructOpaqueSurfacedWindowL(RWindow& aWindow,
		                                           const TRgb& aColor, 
		                                           TInt aPos,
		                                           const TPoint& aPoint, 
		                                           const TSize& aSize,
		                                           const TSurfaceId& aSurfaceID)
	{
	ASSERT_EQUALS_X(aWindow.Construct(iGroup, ++iWindowHandle), KErrNone);
	aWindow.SetRequiredDisplayMode(iDisplayMode);
	aWindow.SetExtent(aPoint, aSize);
	aWindow.SetBackgroundColor(aColor);
	aWindow.SetOrdinalPosition(aPos);	
	ASSERT_EQUALS_X(aWindow.SetBackgroundSurface(aSurfaceID), KErrNone);
	aWindow.Activate();
	aWindow.BeginRedraw();
	iGc->Activate(aWindow);
	iGc->Deactivate();
	aWindow.EndRedraw();
	}


