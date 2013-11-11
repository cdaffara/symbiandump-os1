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

#include <w32std.h>
#include "wsdynamicresbase.h"
#include "teflogextensions.h"
#include "globalsettings.h"
#include <bitdraw.h>
#include <bitdrawinterfaceid.h>
#include <bitdrawsurface.h>
#include <graphics/surfaceconfiguration.h>
#include "regionextend.h"


#if defined(__X86GCC__)
extern "C" TInt atexit(void (*function)(void))
	{
	return KErrNone;
	}
#endif

CWsDynamicResBase::CWsDynamicResBase():
	iUtility(this)
	{
	}

CWsDynamicResBase::~CWsDynamicResBase()
	{
	}

/**
Common set up code for all tests.

Creates the session and window group for further windows, plus a simple white
background to obscure any unwanted stuff behind the test. Sets up the surface
update session and surface manager, to work with surfaces. Creates a screen
device for use in the tests.
*/
void CWsDynamicResBase::SetupL()
	{
	SetupL(EFalse);
	}
void CWsDynamicResBase::SetupL(TBool aUseOtherScreenForInfo)
	{
	CWsDynamicResWinBase::SetupL(aUseOtherScreenForInfo);
	
	
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

/**
Common tear down code for all tests.

Windows, group and session created are closed. Screen device is destroyed.
Surfaces, manager and update session are closed.
*/
void CWsDynamicResBase::TearDownL()
	{
	CWsDynamicResWinBase::TearDownL();
	delete iUtility();
	PostTestCleanupInstance().iCleanedUpOnExit=ETrue;
	//Pause(1000);
	}
/**
 * Note that this is not the ideal mechanism.
 * A derived class may thinks its TearDown is safe to do from delete, but in the class it is derived from it may not be safe
 **/
void CWsDynamicResBase::TearDownFromDeleteL()	
	{
	CWsDynamicResBase::TearDownL();	
	}

//Allocating an instance of surface utility here means all test code instances will share the same instance of the utility class.
// Owns the singleton
/*static*/ const CWsDynamicResBase::TPostTestCleanup& CWsDynamicResBase::PostTestCleanupInstance()
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

void	CWsDynamicResBase::TPostTestCleanup::CreateSharedUtilityL()const
	{
	if (iSharedUtility==NULL)
		iSharedUtility=CSurfaceUtility::NewL();
	}

/**
Pause for the given number of milliseconds.

@param aMilliseconds	Time to wait in milliseconds.
*/
void CWsDynamicResBase::Pause(TInt aMilliseconds)
	{
	User::After(TTimeIntervalMicroSeconds32(aMilliseconds * 1000));
	}

void CWsDynamicResBase::LargerTestWindow(TInt aPercentOfBack)
	{
	TRect newPos=iTestPos;
	TSize backSize=iTestPos.Size();
	newPos.Grow((aPercentOfBack-100)*backSize.iWidth/200,(aPercentOfBack-100)*backSize.iHeight/200);
	iCenteredFrontWinRect=newPos;
	if (iTestFront.WsHandle())
		{
		iTestFront.SetExtent(newPos.iTl,newPos.Size());
		}
	
	}


void CWsDynamicResBase::MakeExtraChildWindowL(const RWindowBase& aFromParent,TRect aChildRect,TRgb aChildColor)
	{
	ASSERT(aFromParent.WsHandle());
	if (!iTestSecondChild.WsHandle())
		{
		iTestSecondChild=RWindow(iSession);
		ASSERT_EQUALS_X(iTestSecondChild.Construct(aFromParent, ++iWindowHandle), KErrNone);
		iTestSecondChild.SetRequiredDisplayMode(iDisplayMode);
		}
	iTestSecondChild.SetBackgroundColor(aChildColor);
	iTestSecondChild.SetExtent(aChildRect.iTl,aChildRect.Size());
	iTestSecondChild.Activate();
	iTestSecondChild.BeginRedraw();
	iGc->Activate(iCompare);
	iGc->SetBrushColor(iBlue);
	iGc->Clear();
	iGc->Deactivate();
	iTestSecondChild.EndRedraw();
	iTestSecondChild.SetVisible(ETrue);
	}

CWsDynamicResBase::LoopingGcPtr  CWsDynamicResBase::LoopBeginActivateWithWipe(const TRegion& aRegion,RWindow& aWin,TRgb aColor)
	{
	InvalidateRegion(aRegion,aWin);
	return LoopingGcPtr(aRegion,aWin,aColor,GcForWindow(aWin));
	}

//This gets called each itteration of the while loop, and is used to step the rectangle count!
void CWsDynamicResBase::LoopingGcPtr::operator ++()
	{
	iPass++;
	if (iPass<iRegion.Count())
		{	//Invalidate all the rectangles in the region
		iWin.BeginRedraw(iRegion[iPass]);
		CWsDynamicResWinBase::ActivateWithWipe(iGc,iWin,iColor);
//		return true;
		}
	else
		{
		iGc=NULL;
//		return false;
		}
	}

CWindowGc*	CWsDynamicResBase::BeginActivateWithWipe(TBool aInvalidate,RWindow& aWin,TRgb aColor)
	{
	return CWsDynamicResWinBase::BeginActivateWithWipe(aInvalidate,aWin,aColor);
	}

CWindowGc*	CWsDynamicResBase::BeginActivateWithWipe(TBool aInvalidate,TRect aRect,RWindow& aWin,TRgb aColor)
	{
	return CWsDynamicResWinBase::BeginActivateWithWipe(aInvalidate,aRect,aWin,aColor);
	}

CWindowGc*	CWsDynamicResBase::BeginActivateWithWipe(const TRegion& aRegion,RWindow& aWin,TRgb aColor)
	{
	return CWsDynamicResWinBase::BeginActivateWithWipe(aRegion,aWin,aColor);
	}

/**	
 * 	Generates a valid surfaceID for the current display mode.
 * 	This is then used in negative testing.
 * 
 * 
 **/
void CWsDynamicResBase::UISurfaceL(TSurfaceId& aSurfaceId) const
	{
	CFbsDrawDevice* screenDevice=NULL;
	TDisplayMode displayMode=iScreenDevice->DisplayMode();
	screenDevice = CFbsDrawDevice::NewScreenDeviceL(TGlobalSettings::Instance().iScreen, displayMode);
	CleanupStack::PushL(screenDevice);
	TAny* p=NULL;
	User::LeaveIfError(screenDevice->GetInterface(KSurfaceInterfaceID, p));
	MSurfaceId* uiSurface = static_cast<MSurfaceId*>(p);
	uiSurface->GetSurface(aSurfaceId);
	CleanupStack::PopAndDestroy(screenDevice);
	}

/**
Common set up code for creating a surface based window.

Given window and surface objects and a color: creates the window and surface,
sets the window to a default size (the same as the surface), sets the background
to the bitwise inverse of the given color, sets the surface as the background,
fills the surface with the color and completes a redraw to prevent an event.

@param aWindow	The window object, connected to a session.
@param aSurface	The surface object, to be initialized.
@param aColor	The color to fill the surface with.
*/
void CWsDynamicResBase::CommonSurfaceWindowSetupL(RWindow& aWindow, TSurfaceId& aSurface, const TRgb& aColor)
	{
	TInt err = KErrNone;

	TRAP(err, aSurface = iUtility->CreateSurfaceL(TSize(KSurfaceWidth, KSurfaceHeight), 
			KSurfaceFormat, KSurfaceWidth * KBytesPerPixel));
	PRINT_ON_ERROR2_L(err, _L("Failed to create surface: %d"), err);
	ASSERT_EQUALS_X(aWindow.Construct(iGroup, ++iWindowHandle), KErrNone);

	aWindow.SetRequiredDisplayMode(iDisplayMode);
	aWindow.SetExtent(TPoint(0, 0), TSize(KSurfaceWidth, KSurfaceHeight));
	aWindow.SetBackgroundColor(TRgb(aColor.Value() ^ 0xFFFFFF));

	ASSERT_EQUALS_X(aWindow.SetBackgroundSurface(aSurface), KErrNone);

	TRAP(err, iUtility->FillSurfaceL(aSurface, aColor));
	PRINT_ON_ERROR2_L(err, _L("Failed to fill surface: %d"), err);
	DrawUIContent(aWindow);
	}

/**
Common set up code for resizing tests.

Similar to the common surface window code, but filling the surface with a grid
instead of a solid color. The grid lines are always black. Also, the background
color is always blue.
*/
void CWsDynamicResBase::ResizeTestCommonSetupL(RWindow& aWindow, const TRgb& aColor)
	{
	// Session and group created in SetUpL()

	TSurfaceId surface;
	TInt err = KErrNone;

	TRAP(err, surface = iUtility->CreateSurfaceL(TSize(KSurfaceWidth, KSurfaceHeight), 
			KSurfaceFormat, KSurfaceWidth * KBytesPerPixel));
	PRINT_ON_ERROR2_L(err, _L("Failed to create surface: %d"), err);

	ASSERT_EQUALS_X(aWindow.Construct(iGroup, ++iWindowHandle), KErrNone);

	aWindow.SetRequiredDisplayMode(iDisplayMode);
	aWindow.SetExtent(TPoint(0, 0), TSize(KSurfaceWidth, KSurfaceHeight));
	aWindow.SetBackgroundColor(iBlue);
	ASSERT_EQUALS_X(aWindow.SetBackgroundSurface(surface), KErrNone);

	TRAP(err, iUtility->GridFillSurfaceL(surface, aColor, TRgb(0)));
	PRINT_ON_ERROR2_L(err, _L("Failed to grid fill surface: %d"), err);
	DrawUIContent(aWindow);
	}

CFbsBitmap* CWsDynamicResBase::RotateBitmapLC(const CFbsBitmap* aSrcBitmap)
	{
	CFbsBitmap* rv=new CFbsBitmap;
	CleanupStack::PushL(rv);
	TSize srcSize=aSrcBitmap->SizeInPixels();
	rv->Create(TSize(srcSize.iHeight,srcSize.iWidth),EColor16MA);
	TRgb* linestore=new TRgb[srcSize.iHeight];
	TPtr8 buff((unsigned char*)linestore,srcSize.iHeight*sizeof(TRgb),srcSize.iHeight*sizeof(TRgb));
	for (TInt col=0;col<aSrcBitmap->SizeInPixels().iWidth;col++)
		{
		for (TInt row=0,brow=srcSize.iHeight-1;row<srcSize.iHeight;row++,brow--)
			{
			aSrcBitmap->GetPixel(linestore[row],TPoint(col,brow));
			}
		rv->SetScanLine(buff,col);
		}
	delete[] linestore;
	return rv;
	}


//
//
//
// Pattern checking. Is a given pattern still present?
// To make life interesting, the pattern is stored backwards!
// The pattern is fibonnacci sequence masked to byte:
//	1	2	3	5	8	13	21	34	55	89	144	233	121
//	98	219	61	24	85	109	194	47	241	32	17	49	66
//	115	181	40	221	5	226	231	201	176	121	41	162
//
//
void Pattern::Fill(void* aTrg,TInt aOffset,TInt aLength)
	{
	unsigned char* ptr=(unsigned char*)aTrg;
	TInt a=0;
	TInt b=1;
	while (--aLength)
		{
			TInt c=a+b;
			*(ptr+aOffset+aLength)=c&0xff;
			a=b;
			b=c;
		}
	}
TBool Pattern::Check(void* aTrg,TInt aOffset,TInt aLength)
	{
	unsigned char* ptr=(unsigned char*)aTrg;
	TInt a=0;
	TInt b=1;
	while (--aLength)
		{
			TInt c=a+b;
			if (*(ptr+aOffset+aLength)!=c&0xff)
				return EFalse;
			a=b;
			b=c;
		}
	return ETrue;
	}
TBool Pattern::CheckVal(void* aTrg,TInt aOffset,TInt aLength,char val)
	{
	unsigned char* ptr=(unsigned char*)aTrg;
	while (--aLength)
		{
			if (*(ptr+aOffset+aLength)!=val&0xff)
				return EFalse;
		}
	return ETrue;
	}

//I have removed these only because they use TRegionExtend
//
//TInt	CWsDynamicResBase::RegionDiffForUiLayer(TInt aUiLayer)
//	{
//	EWsDebugGetUILayerConfig,		//return: TSurfaceConfig		//Index UI layer via EWsDebugArgLayerMask
//	EWsDebugGetUILayerBase,			//return: TRect[]
//	
//	RRegion 	layerRegion;
//	TInt layerRegionLen=iSession.DebugInfo(EWsDebugGetUILayerBase,iSession.ObjInd(0,aUiLayer),layerRegion);
//	if (layerRegionLen==KErrCancel)
//		return TRegionExtend::EExact;
//	ASSERT_TRUE(layerRegionLen>=0);
//	TBuf8<sizeof(TSurfaceConfiguration)> configBuf(sizeof(TSurfaceConfiguration));
//	const TSurfaceConfiguration* config;
//	TInt configLen=iSession.DebugInfo(EWsDebugGetUILayerConfig,iSession.ObjInd(0,aUiLayer),configBuf,config);
//	ASSERT_TRUE(configLen>=0);
//	TRect layerExtent;
//	config->GetExtent(layerExtent);
//	TInt retVal=TRegionExtend::Cast(layerRegion).TestDifference(layerExtent.Size());
//	layerRegion.Close();
//	return retVal;
//	}
//CWsDynamicResBase::FastPathMode	CWsDynamicResBase::DeduceUiFastPathMode()
//	{
//	TInt blendedRegionState=RegionDiffForUiLayer(0);
//	TInt opaqueRegionState=RegionDiffForUiLayer(1);
//	if (blendedRegionState&TRegionExtend::ENoIntersect)
//		{
//		if (opaqueRegionState&TRegionExtend::ENoIntersect)
//			{
//			return EFpExternalOpaque;			//fullscreen fast-path external surface
//			}
//		else
//			if (opaqueRegionState&TRegionExtend::EAdd)
//				{
//				return (FastPathMode)(EFpUiOpaque|EFpUiRegions);	//windowed max-opt no blending
//				}
//			else
//				{
//				return EFpUiOpaque;		//full-screen fastpath
//				}
//		}
//	else
//		{
//		if (opaqueRegionState&TRegionExtend::ENoIntersect)
//			{
//			if (blendedRegionState&TRegionExtend::EAdd)
//				{
//				return (FastPathMode)(EFpUiBlended|EFpUiRegions);	//windowed max-opt no opaque
//				}
//			else
//				{
//				return (EFpUiBlended);	//full-screen blended
//				}
//			}
//		else
//			{
//			if ((blendedRegionState|opaqueRegionState)&TRegionExtend::EAdd)
//				{
//				return (FastPathMode)(EFpUiComplex|EFpUiRegions);	//moxed blending, opaque and external max optimisation
//				}
//			else
//				{
//				return EFpUiComplex;	//Error! blend and opaque both enabled and full-screen!
//				}
//			}
//		
//		}
//	}
