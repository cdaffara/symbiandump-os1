// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "screen.h"

#include <hal.h>
#include <graphics/wsscreendevice.h>
#include <graphics/wsscene.h>
#include <graphics/wselement.h>
#include "server.h"
#include "wstop.h"
#include "rootwin.h"
#include "walkwindowtree.h"
#include "EVENT.H"
#include "windowgroup.h"
#include "inifile.h"
#include "pointer.h"
#include "windowelementset.h"
#include "registeredsurfacemap.h"
#include "debugbar.h"
#include "ScreenRedraw.h"
#include "wspluginmanager.h"
#include "devicemap.h"
#include <graphics/wsdisplaymapping.h>
#if defined(__WINS__) && defined(_DEBUG)
#include "../../debuglog/osbwin.h"
#endif
#include <graphics/wsdisplaycontrol.h>

GLREF_D CDebugLogBase *wsDebugLog;

LOCAL_C inline MWsScene::TSceneRotation GcToScreen(CFbsBitGc::TGraphicsOrientation aGcOrientation)
	{
	MWsScene::TSceneRotation screenRotation = MWsScene::ESceneAntiClockwise0;

	switch (aGcOrientation)
		{
		case CFbsBitGc::EGraphicsOrientationRotated90:
			screenRotation = MWsScene::ESceneAntiClockwise90;
			break;
		case CFbsBitGc::EGraphicsOrientationRotated180:
			screenRotation = MWsScene::ESceneAntiClockwise180;
			break;
		case CFbsBitGc::EGraphicsOrientationRotated270:
			screenRotation = MWsScene::ESceneAntiClockwise270;
			break;
		}

	return screenRotation;
	}

LOCAL_C inline TDeviceOrientation GcToDevice(CFbsBitGc::TGraphicsOrientation aGcOrientation)
	{
	// Each device orientation is defined to be the value where just the bit
	// corresponding to the GDI orientation value is set.
	return (TDeviceOrientation)(1 << aGcOrientation);
	}

LOCAL_D TBool FindNextValue(TLex& aLex, TInt& aValue) // assumes the list cannot contain *negative* integers
	{
	while (!aLex.Eos())
		{
		const TUint character=aLex.Peek();
		if (Rng(TUint('0'), character, TUint('9')) || (character=='-'))
			{
			break;
			}
		aLex.Inc();
		}

	return (aLex.Val(aValue)==KErrNone);
	}

CScreen::CFallbackMap * CScreen::CFallbackMap::NewL(CScreen* aScreen)
	{
	CFallbackMap * self = new (ELeave) CFallbackMap(aScreen);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CScreen::CFallbackMap::CFallbackMap(CScreen* aScreen) :
	iScreen(aScreen),
	iRegion(TRect(TPoint(0,0), TSize(1,1))),
	iCount(0)
	{
	}
	
CScreen::CFallbackMap::~CFallbackMap()
	{
	delete []iMap;
	}
	
void CScreen::CFallbackMap::ConstructL()
	{
	iMapSize = 0;
	
	for (TInt num = 0; num < iScreen->NumScreenSizeModes(); ++num)
		{
		if (iScreen->IsValidScreenSizeMode(num))
			{
			const TSizeMode & mode = iScreen->ScreenSizeModeData(num);
			TInt width = mode.iScreenSize.iWidth / 32;
			if (mode.iScreenSize.iWidth & (32 - 1))
				++width;
			TInt size = width * mode.iScreenSize.iHeight;
			if (size > iMapSize)
				iMapSize = size;
			}
		}
		
	iMap = new (ELeave) TInt [iMapSize];
	}

void CScreen::CFallbackMap::Prepare()
	{
	const TSizeMode & mode = iScreen->ScreenSizeModeData();
	Mem::FillZ(iMap, iMapSize * sizeof(TInt));
	iCount = mode.iScreenSize.iHeight * mode.iScreenSize.iWidth;
	WS_ASSERT_DEBUG(iRegion.Count() == 1, EWsPanicScreenFallback);
	iRegion.Clear();
	iRegion.AddRect(TRect(mode.iOrigin, mode.iScreenSize));
	}

TBool CScreen::CFallbackMap::FillRegion(const TRegion& aRegion)
	{
	WS_ASSERT_DEBUG(!aRegion.CheckError(), EWsPanicScreenFallback);
	if (aRegion.Count() > 20 || aRegion.CheckError())
		return ETrue;
	TBool hit = EFalse;
	if (iCount > 0)
		{
		const TRect * rect = aRegion.RectangleList();
		for (TInt num = 0; num < aRegion.Count(); ++num)
			{
			hit = FillRect(*rect) || hit;
			if (iCount < 1)
				break;
			++rect;
			}
		}
	return hit;
	}
	
// x >> 5 is equivalent to x / 32
// 0x1F is the rounding error when dividing by 32
// 0x20 is 32.
// The compiler might do all the optimizations for us - not checked.
TBool CScreen::CFallbackMap::FillRect(const TRect& aRect)
	{
	TBool hit = EFalse;
	const TSizeMode & mode = iScreen->ScreenSizeModeData();
	TRect scrrect(mode.iOrigin, mode.iScreenSize);
	TRect rect = aRect;
	rect.Intersection(scrrect);
	TInt rowWidthInInts = mode.iScreenSize.iWidth;
	if (rowWidthInInts & 0x1F)
		rowWidthInInts += 0x20;
	rowWidthInInts >>= 5;

	TInt colStartInInts = rect.iTl.iX >> 5;
	TInt firstOffsetInBits = rect.iTl.iX & 0x1F;

	for(TInt row = rect.iTl.iY; row < rect.iBr.iY; ++row)
		{
		TInt * map = iMap + row * rowWidthInInts + colStartInInts;
		TInt offsetShift = 31 - firstOffsetInBits;
		for (TInt col = rect.iTl.iX; col < rect.iBr.iX; ++col)
			{
			WS_ASSERT_DEBUG(map - iMap < iMapSize, EWsPanicScreenFallback);
			if (!(*map & 1 << offsetShift))
				{
				--iCount;
				hit = ETrue;
				if (iCount < 1)
					break;
				(*map) |= (1 << offsetShift);
				}
			--offsetShift;
			if (offsetShift < 0)
				{
				offsetShift = 31;
				++map;
				}
			}
		}
	return hit;
	}

TInt CScreen::CFallbackMap::Count() const
	{
	return iCount;
	}

const TRect * CScreen::CFallbackMap::Rect() const
	{
	return iRegion.RectangleList();
	}
	
const RRegion * CScreen::CFallbackMap::Region() const
	{
	return &iRegion;
	}

TInt CScreen::CFallbackMap::Resize(const TSize& aSize)
	{
	TInt err = KErrNone;
	TInt width = (aSize.iWidth+31) >> 5;	// divide by 32
	TInt invertedWidth = (aSize.iHeight+31) >> 5;	// divide by 32
		
	TInt maxSize = Max(width * aSize.iHeight,invertedWidth * aSize.iWidth);
	if (maxSize > iMapSize)
		{
		TInt* newMap=NULL;
		newMap = new  TInt [maxSize];
		if (newMap)
			{
			delete []iMap;
			iMap = newMap;
			iMapSize = maxSize;
			}
		else
			{
			err = KErrNoMemory;
			}
		}
	return err;
	}
	
//
// CScreen
//
CScreen::CScreen(): iDirects(_FOFF(CWsDirectScreenAccess,iLink)), iMaxContrast(-1), iMaxBrightness(-1)
, iDisplayChangeSpinner(0), iConfigChangeSpinner(0)
	{
	}

CScreen::~CScreen()
	{
	delete iDebugBar;
	TInt ii;
	if(iModes)
		{
		for(ii=iNumScreenSizeModes-1;ii>=0;--ii)
			{
			delete (*iModes)[ii];
			}
		iModes->Close();
		delete iModes;
		}
	delete iRootWindow;
	iScreenDevice = NULL;
	delete iDsaDevice;
	delete iDeviceMap;
	delete iFallbackMap;
	delete iSpriteManager;
	delete iWindowElementSet;
	if (!iDsaSurface.IsNull())
	    {
	    TInt err = iScene->UnregisterSurface(iDsaSurface);
		WS_ASSERT_DEBUG(KErrNone == err, EWsPanicDirectScreenAccess);
	    }

	delete iSurfaceMap;
	delete iRedraw;
#if defined(__WINS__) && defined(_DEBUG)
	delete iDebugWin;
#endif
	
	if(iDisplayChangeNotifier)
		delete iDisplayChangeNotifier;
	if(iConfigChangeNotifier)
		delete iConfigChangeNotifier;
	iWsClientList.Close();
	}

void CScreen::ConstructL(const TRect& aDigitiserArea, TInt aScreenNumber)
	{
	iScreenNumber = aScreenNumber ;

	if (wsDebugLog)
		{
		_LIT(KWSERVInitScreen,"Initialising for Screen %d");
		wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant, KWSERVInitScreen, aScreenNumber);
		}

	// create screen redraw with render stages
	iRedraw = CScreenRedraw::NewL(*this);
	iScreenDevice = iRedraw->ObjectInterface<MWsScreenDevice>();
	WS_ASSERT_ALWAYS(iScreenDevice, EWsPanicScreenDeviceMissing);
	iScene = iRedraw->ObjectInterface<MWsScene>();
	WS_ASSERT_ALWAYS(iScene, EWsPanicSceneMissing);
	
	iDeviceMap = CGraphicsDeviceMap::NewL(*iScreenDevice);

	iDisplayControl = MWsScreen::ObjectInterface<MWsDisplayControl>();
	iDisplayMapping = MWsScreen::ObjectInterface<MWsDisplayMapping>();
	iDisplayPolicy = MWsScreen::ObjectInterface<MWsDisplayPolicy>();


	// initialize screen size mode data
	LoadScreenSizesL(iScreenDevice->SizeInPixels());
	iFallbackMap = CFallbackMap::NewL(this);

	LoadScreenSizeProperties(iScreenDevice->DisplayMode());

	if (iDisplayPolicy)
		{
		iDisplayPolicy->NewAppModesAvailable();
		}
	
	iDigitiserArea = aDigitiserArea;
	SetInitialScreenSizeModeAndRotation(); //get the first/lowest valid mode.  Also calls SetDigitiserAreas
	
	ApplyRemainingWsiniSettingsL();
	
	iRootWindow = new (ELeave) CWsRootWindow(NULL, this);
	iRootWindow->ConstructL();
	
	// Default fading parameters
	iBlackMap=128;
	iWhiteMap=255;
	
	iSpriteManager = CWsSpriteManager::NewL();

	InitializeSceneL();
	InitializeUiElementsL();
	iSurfaceMap = new (ELeave) CRegisteredSurfaceMap(*iScene);
	
	//if the interface for notification is available. start notification AO.
	if (iDisplayControl)
		{
		iDisplayChangeNotifier = CWsDisplayChangeNotifier::NewL(iDisplayControl, this);
		iDisplayChangeNotifier->IssueNotificationRequest();
		iConfigChangeNotifier = CWsConfigChangeNotifier::NewL(iDisplayControl, this);
		iConfigChangeNotifier->IssueNotificationRequest();
		}
	doSetScreenMode(iScreenSizeMode,ETrue);
	}

void CScreen::ApplyRemainingWsiniSettingsL()
	{
#if defined(__WINS__) && defined(_DEBUG)
	_LIT(KDebugOsb,"DEBUGOSB");
	if(WsIniFile->FindVar(iScreenNumber, KDebugOsb))
		{
		_LIT(KDebugWinTitleFormat, "Screen %d, DSA surface");
		TBuf<32> title;
		title.Format(KDebugWinTitleFormat, iScreenNumber);
		iDebugWin = CDebugOsbWin::NewL(title, iScreenDevice->SizeInPixels());
		}
#endif
	
   	TInt autoClear = 1;
   	_LIT(KWSERVIniFileVarAutoClear,"AUTOCLEAR");
   	WsIniFile->FindVar(iScreenNumber,KWSERVIniFileVarAutoClear,autoClear);
   	if (autoClear != 0)
   		{
   		iFlags|=EAutoClear;
   		}
		
	_LIT(KBackLight,"BACKLIGHTCONTROL");
	iBackLightFlag=WsIniFile->FindVar( iScreenNumber, KBackLight);

	_LIT(KWSERVIniFileVarBlankScreen, "BLANKSCREENONROTATION");
	if (WsIniFile->FindVar(iScreenNumber, KWSERVIniFileVarBlankScreen))
		{
		iFlags|=EBlankScreenOnRotation;
		}

	//Cache pointers to renderstage APIs required in CHANGETRACKING mode
	iWindowTreeObserver = iRedraw->ObjectInterface<MWsWindowTreeObserver>(); 
	iDrawAnnotationObserver = iRedraw->ObjectInterface<MWsDrawAnnotationObserver>();
	iWindowVisibilityNotifier = iRedraw->ObjectInterface<MWsWindowVisibilityNotifier>();
	if(iWindowVisibilityNotifier)
		{
		iWindowVisibilityNotifier->RegisterWindowVisibilityObserver(iRedraw);
		}
	if (WsIniFile->FindVar(iScreenNumber, KWSERVIniFileVarChangeTracking))
		{
		iFlags|=EChangeTracking;
		}

	//coverity[const]
	TInt refreshRate = 1000000;
	_LIT(KDebugBar, "DEBUGBAR");
	if (WsIniFile->FindVar(KDebugBar, refreshRate))
		{
		if (refreshRate < 100000)
			// coverity [dead_error_line]
			refreshRate = 50000;
		iDebugBar = CDebugBar::NewL(this, refreshRate);
		}
	}

void CScreen::AcquireDsaScreenDeviceL()
	{
	//creates WSERV's DSA buffer handle
	//registers the DSA surface into the scene accordingly to the value of aRegisterSurface
	if(!iDsaDevice)
		{
		TDisplayMode screenMode = ENone;
		screenMode = iScreenDevice->DisplayMode();
		if(screenMode != ENone)
			{
			CreateDsaScreenDeviceIfSupportedL(screenMode);
			// initialize DSA
			iDsaDevice->SetAutoUpdate(EFalse);
			iDsaDevice->SetDeviceOrientation(GcToDevice(ScreenSizeModeData().iRotation));
			iDsaDevice->ChangeScreenDevice(NULL);    //This is necessary to initialise the screen
			// register DSA Surface
			TInt err = InitializeDsaSurface();
			// create a graphics context to clear the DSA surface
			if (!err)
				err = iDsaDevice->CreateContext(iDsaGc);
			if (!err)
				iDsaGc->Activate(iDsaDevice);
			if (err != KErrNone)
				{
				//Creation of the DSA surface failed
				//Cleanup the DSA Surface ID
				iDsaSurface = TSurfaceId::CreateNullId();
				//and the iDsaDevice
				delete iDsaDevice;
				iDsaDevice = NULL;
				User::Leave(err);
				}
			}
		else
			{
			User::Leave(KErrNotSupported);
			}
		}
	iNumberDrawingDsa++;
	}

void CScreen::CreateDsaScreenDeviceIfSupportedL(TDisplayMode aScreenMode)
	{
	if(DoCreateDsaScreenDevice(aScreenMode))
		return;
	// Try creating the screen device with all available display modes, going from best to worst
	__ASSERT_COMPILE(EColorLast == 14); // if any display mode is added to TDisplayMode we must update the list below
	// (the list below contains all enums in TDisplayMode except ENone, ERgb, EColorLast)
	if(DoCreateDsaScreenDevice(EColor16MAP))
		return;
	if(DoCreateDsaScreenDevice(EColor16MA))
		return;
	if(DoCreateDsaScreenDevice(EColor16MU))
		return;
	if(DoCreateDsaScreenDevice(EColor16M))
		return;
	if(DoCreateDsaScreenDevice(EColor64K))
		return;
	if(DoCreateDsaScreenDevice(EColor4K))
		return;
	if(DoCreateDsaScreenDevice(EColor256))
		return;
	if(DoCreateDsaScreenDevice(EColor16))
		return;
	if(DoCreateDsaScreenDevice(EGray256))
		return;
	if(DoCreateDsaScreenDevice(EGray16))
		return;
	if(DoCreateDsaScreenDevice(EGray4))
		return;
	if(DoCreateDsaScreenDevice(EGray2))
		return;
	User::Leave(KErrNotSupported);
	}

TBool CScreen::DoCreateDsaScreenDevice(TDisplayMode aScreenMode)
	{
	TRAPD(err, iDsaDevice = CFbsScreenDevice::NewL(iScreenNumber, aScreenMode));
	if(err == KErrNone)
		{
		TUint supportedDsaRotationModes = iDsaDevice->DeviceOrientationsAvailable();
		MWsScene::TSceneRotation currenTSceneRotation = iScene->SceneRotation();
		TBool doesDsaSupportThisMode = EFalse;
		switch(currenTSceneRotation)
			{
			case MWsScene::ESceneAntiClockwise0:
				if(supportedDsaRotationModes & EDeviceOrientationNormal)
					{
					doesDsaSupportThisMode = ETrue;
					}
				break;
			case MWsScene::ESceneAntiClockwise90:
				if(supportedDsaRotationModes & EDeviceOrientation90CW)
					{
					doesDsaSupportThisMode = ETrue;
					}
				break;				
			case MWsScene::ESceneAntiClockwise180:
				if(supportedDsaRotationModes & EDeviceOrientation180)
					{
					doesDsaSupportThisMode = ETrue;
					}
				break;				
			case MWsScene::ESceneAntiClockwise270:
				if(supportedDsaRotationModes & EDeviceOrientation270CW)
					{
					doesDsaSupportThisMode = ETrue;
					}
				break;				
			default:
				RDebug::Print(_L("** CScreen::DoCreateDsaScreenDevice Panic, non existing rotation mode"));
				WS_PANIC_ALWAYS(EWsPanicInvalidOperation);
				break;
			}
		if(!doesDsaSupportThisMode)
			{
			delete iDsaDevice;
			iDsaDevice = NULL;
			RDebug::Print(_L("** Current Rotation Mode not supported by the DSA device"));
			err = KErrNotSupported;
			}
		}
	return (err == KErrNone);
	}

void CScreen::AbortDSAs(RDirectScreenAccess::TTerminationReasons aReason, TSglQue<CWsDirectScreenAccess>& aDirects)
	{
	if (aDirects.IsEmpty())
		return;

	TInt nofDSAs = 0;
	CWsDirectScreenAccess* direct= NULL;
	TSglQueIter<CWsDirectScreenAccess> iter(aDirects);
	while ((direct=iter++)!=NULL)
		{
		nofDSAs++;
		direct->SignalAbort(aReason);
		}

	TRequestStatus timerStatus;
	RTimer& timer=CWsTop::Timer();
	timer.Cancel();

	TRequestStatus** cancelReqList = (TRequestStatus**) User::AllocZ(sizeof(TRequestStatus*) * (nofDSAs + 1));
	if (NULL != cancelReqList)
		{
		TInt dsaNo = 1;
		timer.After(timerStatus, KDSAAbortingImmediateRespAwaitFrameMicrosec);
		iter.SetToFirst();
		while ((direct=iter++)!=NULL)
			{
			WS_ASSERT_DEBUG((dsaNo<=(nofDSAs)),EWsPanicDirectScreenAccess);
			cancelReqList[ dsaNo ] = &direct->AbortStatus();
			dsaNo++;
			}
		cancelReqList[ 0 ] = &timerStatus;

		//wait for response or timeout
		User::WaitForNRequest(cancelReqList, nofDSAs + 1);

		iter.SetToFirst();
		while ((direct=iter++)!=NULL)
			{
			if (direct->AbortStatus() != KRequestPending)
				direct->CancelAbortObject(); // responded
			else
				direct->Abort();
			}

		if (timerStatus == KRequestPending)
			{
			timer.Cancel();
			User::WaitForRequest(timerStatus);
			}

		User::Free(cancelReqList);
		}
	else
		{
		iter.SetToFirst();
		while ((direct=iter++) != NULL)
			{
			TRequestStatus timerStatus;
			RTimer& timer=CWsTop::Timer();
			timer.Cancel();
			timer.After(timerStatus, KDSAAbortingImmediateRespAwaitFrameMicrosec);
			
			//wait for response or timeout
			User::WaitForRequest(direct->AbortStatus(), timerStatus);
			
			if (direct->AbortStatus() != KRequestPending)
				direct->CancelAbortObject(); //responded
			else
				direct->Abort(); //timed out

			if (timerStatus == KRequestPending)
				{
				timer.Cancel();
				User::WaitForRequest(timerStatus);
				}
			}
		}
	}

void CScreen::AbortAllDirectDrawing(RDirectScreenAccess::TTerminationReasons aReason)
	{
	AbortDSAs(aReason,iDirects);
	}

void CScreen::AddDirect(CWsDirectScreenAccess& aDirect)
	{
	TBool emptyBefore = iDirects.IsEmpty();
	iDirects.AddLast(aDirect);
	TBool emptyAfter = iDirects.IsEmpty();
	if (emptyBefore && ! emptyAfter)
		{
		TWsEvent wsevent;
		wsevent.SetType(EEventDirectScreenAccessBegin);
		*(wsevent.Int()) = iScreenNumber;
		TWindowServerEvent::PublishNotification(wsevent);
		}

	if (iDsaDrawState==EDsaDrawStateIdle && aDirect.IsVisible())
		{
		iDsaDrawState = EDsaDrawStateDrawing;
		TWindowServerEvent::NotifyDrawer(TWservCrEvent(TWservCrEvent::EDsaDrawingBegin, iScreenNumber));
		}
	}

void CScreen::RemoveDirect(CWsDirectScreenAccess& aDirect)
	{
	TBool emptyBefore = iDirects.IsEmpty();
	iDirects.Remove(aDirect);
	TBool emptyAfter = iDirects.IsEmpty();
	if (emptyAfter && ! emptyBefore)
		{
		TWsEvent wsevent;
		wsevent.SetType(EEventDirectScreenAccessEnd);
		*(wsevent.Int()) = iScreenNumber;
		TWindowServerEvent::PublishNotification(wsevent);
		}

	if (iDsaDrawState==EDsaDrawStateDrawing && aDirect.IsVisible() && !HasVisibleDirectOnQueue())
		{
		iDsaDrawState = EDsaDrawStateIdle;
		TWindowServerEvent::NotifyDrawer(TWservCrEvent(TWservCrEvent::EDsaDrawingEnd, iScreenNumber));
		}
	}

TBool CScreen::HasVisibleDirectOnQueue()
	{
	if (iDirects.IsEmpty())
		return EFalse;

	TSglQueIter<CWsDirectScreenAccess> iter(iDirects);
	CWsDirectScreenAccess* dsa;
	while ((dsa=iter++)!=NULL)
		{
		if (dsa->IsVisible())
			return ETrue;
		}

	return EFalse;
	}

#if defined(_DEBUG)
TBool CScreen::IsDirectOnQueue(const CWsDirectScreenAccess* aDirect)
	{
	TSglQueIter<CWsDirectScreenAccess> iter(iDirects);
	CWsDirectScreenAccess* direct;
	while ((direct=iter++)!=NULL)
		{
		if (direct==aDirect)
			return ETrue;
		}
	return EFalse;
	}
#endif

void CScreen::KillForegroundSession()
	{
	if (iCurrentFocus)
		{
		_LIT(KWSERVKillWinGp,"Killing Session owning Window Group with Id=%d");
		if (wsDebugLog)
			wsDebugLog->MiscMessage(CDebugLogBase::ELogEverything,KWSERVKillWinGp,iCurrentFocus->Identifier());
		iCurrentFocus->WsOwner()->SessionTerminate();
		}
	}

CWsWindowGroup* CScreen::FindNewFocus(CWsRootWindow* aRootWindow)
	{
	CWsWindowGroup* newFocus;
	for(newFocus=aRootWindow->Child();newFocus && newFocus->CanReceiveFocus()==EFalse;newFocus=newFocus->NextSibling()) {}

	return newFocus;
	}

void CScreen::ResetFocus(CWsWindowGroup* aClosingWindow)
	{
	CWsWindowGroup* oldFocus=iCurrentFocus;
	CWsWindowGroup* newFocus=NULL;
	CScreen* newFocusedScreen=NULL;
	iCurrentFocus=FindNewFocus(iRootWindow);
	TBool focusedScreen= EFalse;
	/*Focus policy is specified in the wsini.ini file using the keyword 'MULTIFOCUSPOLICY'.
	If the keyword is not specified, then the default policy is run.
	*/
	if(!CWsTop::MultiFocusPolicy())
		{
		focusedScreen=(this==CWsTop::CurrentFocusScreen()); //check if this screen is the focus screen
		if (!iCurrentFocus && focusedScreen)
			{
			/*If this screen is the focused screen but does not have a focusable window group, then search for the
			next screen that has a focusable window group and set that screen as the focused screen.
			*/
			CScreen* screen=NULL;
			TInt screenNo;
			for (screenNo=0; screenNo<CWsTop::NumberOfScreens() && !newFocus; ++screenNo)
				{
				if (screenNo!=iScreenNumber)
					{
					screen=CWsTop::Screen(screenNo);
					newFocus=FindNewFocus(screen->RootWindow());
					}
				}
			if (newFocus)
				newFocusedScreen=screen;
			}
		}
	/*Scenario A: multi-focus policy
			newFocusedScreen is NULL
			focusedScreen is EFalse
			CWsTop::MultiFocusPolicy() returns ETrue
			Check if the new focusable window group is not the same, send focus lost message to window group
			that has just lost focus and send focus gain message to window group that can receive focus.
	  Scenario B: single-focus policy (default)
			CWsTop::MultiFocusPolicy() returns EFalse
			Check if the new focusable window group is not the same or if there is a new focused screen, send focus lost
			message to window group that has just lost focus and send focus gain message to window group that can receive focus.
	*/
	if (iCurrentFocus!=oldFocus || newFocusedScreen)
		{
		if (oldFocus && (focusedScreen || CWsTop::MultiFocusPolicy()) && oldFocus!=aClosingWindow)
			{
			oldFocus->LostFocus();
			}
		if (newFocusedScreen)
			{
			CWsTop::SetCurrentFocusScreen(newFocusedScreen);
			newFocus->ReceivedFocus();
			}
		else if (iCurrentFocus && (focusedScreen || CWsTop::MultiFocusPolicy()))
			{
			iCurrentFocus->ReceivedFocus();
			}
		TWsPointer::UpdatePointerCursor();
		TWindowServerEvent::SendFocusChangedEvents();
		}
	TWindowServerEvent::SendGroupListChangedEvents();
	}

void CScreen::RemoveFromDefaultOwningList(CWsWindowGroup *aDestroyedGroup)
	{
	for (CWsWindowGroup **group=&iDefaultOwningWindow;*group;group=(*group)->NextDefaultOwningWindowPtr())
		{
		if (*group==aDestroyedGroup)
			{
			*group=*aDestroyedGroup->NextDefaultOwningWindowPtr();
			break;
			}
		}
	}

void CScreen::SetDefaultOwningWindow(CWsWindowGroup *aGroup)
	{
	RemoveFromDefaultOwningList(aGroup);
	aGroup->SetNextDefaultOwningWindow(iDefaultOwningWindow);
	iDefaultOwningWindow=aGroup;
	}

void CScreen::GetScanLine(const TWsSdCmdGetScanLine *aGetScanLine)
	{
	TRgb buf[EGetScanLineBufLen];
	TPtr8 des((TUint8 *)&buf[0],EGetScanLineBufLen*sizeof(TRgb));
	TPoint pos(aGetScanLine->pos);
	TInt read=0;
	TInt len=(des.MaxLength()*EGetScanLineBufLen)/CFbsBitmap::ScanLineLength(EGetScanLineBufLen,aGetScanLine->dispMode);
	if (aGetScanLine->len < 0 || (CFbsBitmap::ScanLineLength(aGetScanLine->len, aGetScanLine->dispMode) >
									CWsClient::CurrentClient()->ClientMessage().GetDesMaxLength(1)))
		{
		CWsClient::PanicCurrentClient(EWservPanicInvalidParameter);
		}
	FOREVER
		{
		if ((aGetScanLine->len-read)<len)
			len=aGetScanLine->len-read;
		iScreenDevice->GetScanLine(des,pos,len,aGetScanLine->dispMode);
		CWsClient::ReplyBuf(des);
		read+=len;
		if (read==aGetScanLine->len)
			break;
		pos.iX+=len;
		}
	}

void CScreen::MaxNumColors(TInt& aColors,TInt& aGrays)
	{
	aGrays=0;
	aColors=TDisplayModeUtils::NumDisplayModeColors(DisplayMode());
	if (!TDisplayModeUtils::IsDisplayModeColor(DisplayMode()))
		{
		aGrays=aColors;
		aColors=0;
		}
	}

#define MODE_TO_FLAG(x) 1<<(x-1)
#define ROTATION_TO_FLAG(x) 1<<x
const TUint allRotationsMask = ROTATION_TO_FLAG(CFbsBitGc::EGraphicsOrientationNormal)
						| ROTATION_TO_FLAG(CFbsBitGc::EGraphicsOrientationRotated90)
						| ROTATION_TO_FLAG(CFbsBitGc::EGraphicsOrientationRotated180)
						| ROTATION_TO_FLAG(CFbsBitGc::EGraphicsOrientationRotated270);
const TUint KRotation0_180Mask = ROTATION_TO_FLAG(CFbsBitGc::EGraphicsOrientationNormal)
						| ROTATION_TO_FLAG(CFbsBitGc::EGraphicsOrientationRotated180);
const TUint KRotation90_270Mask = ROTATION_TO_FLAG(CFbsBitGc::EGraphicsOrientationRotated90)
						| ROTATION_TO_FLAG(CFbsBitGc::EGraphicsOrientationRotated270);
TInt CScreen::ColorModesFlag()
	{
	return MODE_TO_FLAG(DisplayMode());
	}

void CScreen::UpdateDsa()
	{
	if(iDsaDevice)
		{
		#if defined(__WINS__) && defined(_DEBUG)
			if (iDebugWin)
				iDebugWin->Refresh(iDsaDevice->SizeInPixels(), iDsaDevice->DisplayMode(), iDsaDevice->Bits());
		#endif
		
		iDsaDevice->Update();
		}
	}

const CGraphicsDeviceMap& CScreen::DeviceMap() const
	{
	return *iDeviceMap;
	}

const MWsScreenDevice& CScreen::ScreenDevice() const
	{
	return *iScreenDevice; 
	}

void CScreen::SetPointerCursorArea(TInt aMode,const TRect& aRect)
	{
	(*iModes)[aMode]->iPointerCursorArea=aRect;
	(*iModes)[aMode]->iFlags |= EClientDefinedDigitiserArea;
	TWsPointer::SetPointerCursorPos(TWsPointer::PointerCursorPos());
	}

CFbsBitGc::TGraphicsOrientation CScreen::Orientation() const
	{
  	WS_ASSERT_DEBUG(IsValidScreenSizeMode(iScreenSizeMode),EWsPanicInvalidScreenSizeMode);
	return (*iModes)[iScreenSizeMode]->iRotation;
	}

TRect CScreen::DrawableArea() const
	{
	TRect drawRect=iScreenDevice->SizeInPixels();
	if (iDisplayMapping)
		{
		iDisplayMapping->MapCoordinates(EFullScreenSpace,drawRect,EApplicationSpace,drawRect);                   
		}
	return drawRect;
	}

TClientPanic CScreen::SetModeRotation(TInt aMode,CFbsBitGc::TGraphicsOrientation aRotation)
	{
	if (!IsValidScreenSizeMode(aMode))
		return EWservPanicScreenModeNumber;
	TSizeMode& mode=*(*iModes)[aMode];
	if (!(ROTATION_TO_FLAG(aRotation)&mode.iAlternativeRotations))
		return EWservPanicRotation;
	CFbsBitGc::TGraphicsOrientation oldRotation=mode.iRotation;
	mode.iRotation=aRotation;
	CWsWindowGroup::NewOrientation(aMode,aRotation, iRootWindow);
	if (aMode==ScreenSizeMode())
		{
		if(iDisplayPolicy && iDisplayControl && (mode.iAlternativeRotations == allRotationsMask))
			{
			//square mode supports all 4 rotations. We'd better do a complete Setconfiguration
			//all parameters are recalculated for 90 degree rotation
			//The most important one is the offset, since we need to re-center the appmode and it's a policy behaviour
			TDisplayConfiguration config;
			iDisplayControl->GetConfiguration(config);
			//update rotation
			config.SetRotation((TDisplayConfiguration::TRotation)aRotation);
			SetConfiguration(config);
			}
		else if (!UpdateOrientation())
			{
			// Roll back earlier change
			mode.iRotation=oldRotation;
			CWsWindowGroup::NewOrientation(aMode, oldRotation, iRootWindow);
			}
		}
	return EWservNoPanic;
	}

void CScreen::CycleDisplaySize()
	{
	TInt newMode = iScreenSizeMode;
	TSizeMode* sizeMode = NULL;
	do
		{
		newMode = (newMode+1)%iModes->Count();
		sizeMode = (*iModes)[newMode];
		}
	while (sizeMode==NULL);
	doSetScreenMode(newMode);
	}

void CScreen::doSetScreenMode(TInt aMode,TBool aInsideStartup)
	{
  	WS_ASSERT_DEBUG(IsValidScreenSizeMode(aMode),EWsPanicInvalidScreenSizeMode);
 	TWindowServerEvent::NotifyDrawer(TWservCrEvent(TWservCrEvent::EScreenSizeModeAboutToChange, aMode));
	
	if (iDisplayControl && iDisplayPolicy)
		{
		TDisplayConfiguration config;
		TRect sizeModePosition;
		TInt error;
		TSizeMode& mode=*(*iModes)[aMode];
		config.SetRotation((TDisplayConfiguration::TRotation)mode.iRotation);
		//This will return suitable config when display is connected
		//and UI size equal to smallest appmode when disconnected
		error = iDisplayPolicy->GetSizeModeConfiguration(aMode,config,sizeModePosition);
		//set appmode in policy
		if (iDisplayMapping)
			{
			iDisplayMapping->SetSizeModeExtent(sizeModePosition,MWsDisplayMapping::KOffsetAll);
			}
		MWsScene::TSceneRotation oldRotation;
		TSize newUiSize;
		config.GetResolution(newUiSize);
		if (error == KErrNone)
			{
			oldRotation = iScene->SceneRotation();
			//The config info will always be set in policy. If display is connected, policy will have
			//correct composition, Ui and app res. otherwise Ui and appmode will be both set to smallest
			//app mode, composition will be set to zero
			if (iFlags&EHasDynamicSizeModes)
				{
				error = iFallbackMap->Resize(newUiSize);
				}
			if (error == KErrNone)
				{
				error = iDisplayControl->SetConfiguration(config);
				}
			}
		if (error == KErrNone)
			{
			UpdateDynamicScreenModes();
			AbortAllDirectDrawing(RDirectScreenAccess::ETerminateScreenMode);

			if(iDsaDevice && iDsaDevice->GraphicsAccelerator())
				{
				iDsaDevice->ChangeScreenDevice(iDsaDevice); // orientation has changed, therefore we need to re-initialise the screen device's graphics accelerator
				}

			iScreenSizeMode=aMode;
			//This could fail (to set orientation on the context, or to register the rotated DSA).
			//Previously, the update rotation was deferred, leaving the size mode out of step with the actual rotation
			//It also returns false if the orientation is "intentionally" not modified.
			(void) UpdateOrientation(&oldRotation);	
			
			//SetDigitiserAreas needs revisiting if/when we support dynamic resolutions
            //on a screen with touch input.
            //SetDigitiserAreas(newUiSize);
			
			CWsWindowGroup::SetScreenDeviceValidStates(this);
			
			if (!aInsideStartup)
				{
				iWindowElementSet->ResubmitAllElementExtents();
				//TODO jonas: we'd like to not have to clear at all... make the actual change to compositor etc lazily!
				if(BlankScreenOnRotation())
					{
					iRootWindow->ClearDisplay();
					}
	
				CWsTop::ClearAllRedrawStores();
				DiscardAllSchedules();
				iRootWindow->InvalidateWholeScreen();
				}
			}
		}
	else
		{
		if (iDisplayMapping)
			{
			TRect sizeModePosition;
			TRAPD(err,sizeModePosition=TRect(OriginL(aMode),ScreenModeSizeInPixelsL(aMode)));
			if (err==KErrNone)
				{
				iDisplayMapping->SetSizeModeExtent(sizeModePosition,MWsDisplayMapping::KOffsetAll);
				}
			}
		if (!aInsideStartup && (*iModes)[aMode]->iOrigin != (*iModes)[iScreenSizeMode]->iOrigin)
			{
			iWindowElementSet->ResubmitAllElementExtents();
			if ((*iModes)[aMode]->iRotation == (*iModes)[iScreenSizeMode]->iRotation)
				{
				//TODO jonas: we'd like to not have to clear at all... make the actual change to compositor etc lazily!
				if(BlankScreenOnRotation())
					{
					iRootWindow->ClearDisplay();
					}
	
				CWsTop::ClearAllRedrawStores();
				DiscardAllSchedules();
				iRootWindow->InvalidateWholeScreen();
				}
			}
		iScreenSizeMode=aMode;
		//This could fail (to set orientation on the context, or to register the rotated DSA).
		//Previously, the update rotation was deferred, leaving the size mode out of step with the actual rotation
		//It also returns false if the orientation is not modified.
		(void)UpdateOrientation();
		CWsWindowGroup::SetScreenDeviceValidStates(this);
		}
	TWindowServerEvent::SendScreenDeviceChangedEvents(this);
	ResetFocus(NULL);
	}

void CScreen::CycleOrientation()
	{
  	WS_ASSERT_DEBUG(IsValidScreenSizeMode(iScreenSizeMode),EWsPanicInvalidScreenSizeMode);
	TSizeMode& currentSizeMode=*(*iModes)[iScreenSizeMode];
	TUint rotations=currentSizeMode.iAlternativeRotations;
	TInt currentRotation=currentSizeMode.iRotation;
	TInt rotation=currentRotation+1;
	while (rotation!=currentRotation)
		{
		if (rotation>CFbsBitGc::EGraphicsOrientationRotated270)
			rotation=CFbsBitGc::EGraphicsOrientationNormal;
		if (ROTATION_TO_FLAG(rotation)&rotations)
			break;
		++rotation;
		}
	if (rotation==currentRotation)
		{
		if (rotation>CFbsBitGc::EGraphicsOrientationRotated90)
			rotation-=2;
		else
			rotation+=2;
		}
	currentSizeMode.iRotation=REINTERPRET_CAST(CFbsBitGc::TGraphicsOrientation&,rotation);
	CWsWindowGroup::NewOrientation(iScreenSizeMode,currentSizeMode.iRotation, iRootWindow);

	if (!UpdateOrientation())
		{
		// Roll back earlier changes
		currentSizeMode.iRotation=REINTERPRET_CAST(CFbsBitGc::TGraphicsOrientation&,currentRotation);
		CWsWindowGroup::NewOrientation(iScreenSizeMode, currentSizeMode.iRotation, iRootWindow);
		}
	}

/**
 * This method is called either when switching screen size mode, or when the 
 * orientation of the currently active screen size mode is changed.
 */
TBool CScreen::UpdateOrientation(MWsScene::TSceneRotation* aOldRotation)
	{
	CFbsBitGc::TGraphicsOrientation gcOrientation = Orientation();

	MWsScene::TSceneRotation oldRotation = aOldRotation? (*aOldRotation):(iScene->SceneRotation());
	MWsScene::TSceneRotation newRotation = GcToScreen(gcOrientation);
	TDeviceOrientation newDeviceOrientation = GcToDevice(gcOrientation);

	// We have to disable the text cursor here while we are still in the
	// same orientation or offset as we drew it.
	RWsTextCursor* cursor = CWsTop::CurrentTextCursor();
	if (cursor)
		{
		cursor->Disable();
		}
	
// Some of this method has to be done when changing mode even if not changing rotation
	TBool rotating=(oldRotation != newRotation);
	if (rotating)
		{
	
		// Set the new screen rotation and update the UI element extent
		if (iScene->SetSceneRotation(newRotation) != KErrNone)
			return EFalse;
		// Set the new orientation for the DSA device and update the DSA surface
		if(iDsaDevice)
			{
			iDsaDevice->SetDeviceOrientation(newDeviceOrientation);
	
			TSurfaceId newSurface;
			iDsaDevice->GetSurface(newSurface);
			TInt errRegister = iScene->RegisterSurface(newSurface);
			WS_ASSERT_DEBUG(KErrNone == errRegister, EWsPanicDirectScreenAccess);
			// This will remove all the DSA elements from the scene
            AbortAllDirectDrawing(RDirectScreenAccess::ETerminateRotation);
	
			TInt errUnregister = iScene->UnregisterSurface(iDsaSurface);
			WS_ASSERT_DEBUG(KErrNone == errUnregister, EWsPanicDirectScreenAccess);
			iDsaSurface = newSurface;
			}
		
		//updaterotation should not fail after this point (no cleanup)
			
        //update the last set config with the new rotation change so we don't incorrectly
        //change the layer extents
        if (iDisplayControl)
            {
            TDisplayConfiguration config;
            config.SetRotation(static_cast<TDisplayConfiguration::TRotation>(newRotation));           
            iConfigChangeNotifier->UpdateLastSetConfiguration(config);
            }		
		
		TWservCrEvent crEvent(TWservCrEvent::EDeviceOrientationChanged,iScreenNumber,&gcOrientation);
		TWindowServerEvent::NotifyDrawer(crEvent);
		
		if(iDsaDevice && iDsaDevice->GraphicsAccelerator())
			{
			iDsaDevice->ChangeScreenDevice(iDsaDevice); // orientation has changed, therefore we need to re-initialise the screen device's graphics accelerator
			}
		
		}
	
	iRootWindow->AdjustCoordsDueToRotation();
	if (rotating)
		{
		if(BlankScreenOnRotation())
			{
			iRootWindow->ClearDisplay();
			}
		
		CWsTop::ClearAllRedrawStores();	
		DiscardAllSchedules();
		iRootWindow->InvalidateWholeScreen();
		}
	return ETrue;
	}

TPoint CScreen::PhysicalToLogical(TPoint aPhysicalPt)
	{
	const TSizeMode& mode=ScreenSizeModeData();
	TPoint logicalPt;
	if(!iDisplayMapping)
		{
		//old behaviour
		logicalPt=aPhysicalPt-mode.iOrigin;
		if (mode.iScreenScale.iWidth!=1)
			logicalPt.iX=(logicalPt.iX>=0 ? logicalPt.iX/mode.iScreenScale.iWidth : (logicalPt.iX-(mode.iScreenScale.iWidth-1))/mode.iScreenScale.iWidth);
		if (mode.iScreenScale.iHeight!=1)
			logicalPt.iY=(logicalPt.iY>=0 ? logicalPt.iY/mode.iScreenScale.iHeight : (logicalPt.iY-(mode.iScreenScale.iHeight-1))/mode.iScreenScale.iHeight);
		}
	else
		{
		//rect with dummy size for coordinates mapping purpose
		TRect rectInComp(aPhysicalPt, TSize(1,1));
		TRect rectInApp;
		iDisplayMapping->MapCoordinates(ECompositionSpace, rectInComp, EApplicationSpace,rectInApp);
		logicalPt = rectInApp.iTl;
		}
	
	return logicalPt;
	}

void CScreen::LoadScreenSizesL(TSize aScreenSize)
	{
	_LIT(KWSERVNumScrSizeMode, "NUMSCREENMODES");
	TBool allowScrGap=WsIniFile->FindVar(iScreenNumber, KWSERVNumScrSizeMode, iNumScreenSizeModes);
	iModes=new(ELeave) RPointerArray<TInternalSizeMode>(1);
	WS_ASSERT_DEBUG(!allowScrGap || (allowScrGap && iNumScreenSizeModes>0), EWsPanicInvalidScreenSizeMode);
	TInt screenNum=0;
	FOREVER
		{
		++screenNum;
		TBuf<32> varNameWidth;
		TBuf<32> varNameHeight;
		_LIT(KWSERVScreenWidthPattern,"SCR_WIDTH%d");
		varNameWidth.Format(KWSERVScreenWidthPattern,screenNum);
		_LIT(KWSERVScreenHeightPattern,"SCR_HEIGHT%d");
		varNameHeight.Format(KWSERVScreenHeightPattern,screenNum);
		TSize screenSize;
		if (!WsIniFile->FindVar(iScreenNumber, varNameWidth, screenSize.iWidth) ||
			!WsIniFile->FindVar(iScreenNumber, varNameHeight, screenSize.iHeight))
			{
			if (allowScrGap && screenNum<=iNumScreenSizeModes)
				{
				iModes->AppendL(NULL);
				continue;
				}
			else
				break;
			}
		TInt flags=0;
		if (screenSize.iWidth==0 && screenSize.iHeight==0)
			{
			screenSize=aScreenSize;
			flags|=EHalDefault;
			}
		if (screenSize.iWidth==-1 && screenSize.iHeight==-1)
			{
			screenSize=aScreenSize;
			flags|=EDynamic;
			iFlags|=EHasDynamicSizeModes;
			}
		TInternalSizeMode* newSizeMode=new(ELeave) TInternalSizeMode(screenSize);
		newSizeMode->iFlags|=flags;
		CleanupStack::PushL(newSizeMode);
		iModes->AppendL(newSizeMode);
		CleanupStack::Pop(newSizeMode);
		++iNumSupportedScreenSizeModes;
		}
	// If sparse index is enabled and no screen size mode defined, all iModes entries will be NULL
	// Otherwise iModes will be empty
	if (iModes->Count()==0 || iNumSupportedScreenSizeModes==0)
		{
		TInternalSizeMode* defaultSizeMode=new(ELeave) TInternalSizeMode(aScreenSize);
		defaultSizeMode->iFlags|=EHalDefault;
		if (iModes->Count()>0)
			(*iModes)[0]=defaultSizeMode;
		else
			{
			CleanupStack::PushL(defaultSizeMode);
			iModes->AppendL(defaultSizeMode);
			CleanupStack::Pop(defaultSizeMode);
			}
		++iNumSupportedScreenSizeModes;
		}
	if (!allowScrGap)
		iNumScreenSizeModes=iNumSupportedScreenSizeModes;
	}

void CScreen::LoadScreenRotationProperties(TInternalSizeMode& aMode, const TInt aModeIndex)
	{
	TBuf<32> varRotation;
	_LIT(KWSERVScreenRotationPattern,"SCR_ROTATION%d");
	varRotation.Format(KWSERVScreenRotationPattern,aModeIndex+1);		
	TInt rotation=CFbsBitGc::EGraphicsOrientationNormal;
	TUint allRotations=0;
	TPtrC rotList(NULL,0);
	if (WsIniFile->FindVar( iScreenNumber, varRotation,rotList))
		{
		TLex lex(rotList);
		TBool foundOne=EFalse;
		TInt rot;

		while (!lex.Eos())
			{
			if (!FindNextValue(lex, rot))
				{
				break;
				}
			if (rot<0 || rot>360)
				{
				continue;
				}
			if (rot>4)
				{
				rot/=90;
				}
			if (!foundOne)
				{
				rotation=rot;
				foundOne=ETrue;
				}
			if (rot<=CFbsBitGc::EGraphicsOrientationRotated270)
				{
				allRotations|=ROTATION_TO_FLAG(rot);
				}
			}
		}
	if (allRotations==0)
		allRotations=ROTATION_TO_FLAG(rotation);
	const TInt KAllRotationsMask = 0xF;	//Used to keep the old behaviour
	WS_ASSERT_ALWAYS((ROTATION_TO_FLAG(rotation)&KAllRotationsMask)>0, EWsPanicFailedToInitialise);
	aMode.iRotation=reinterpret_cast<CFbsBitGc::TGraphicsOrientation&>(rotation);
	aMode.iAlternativeRotations=allRotations&KAllRotationsMask;
	}

void CScreen::LoadScreenTwipsProperties(TInternalSizeMode& aMode, const TInt aModeIndex)
	{
	TBuf<32> varNameWidth;
	_LIT(KWSERVScreenTwipWidthPattern,"SCR_TWIP_WIDTH%d");
	varNameWidth.Format(KWSERVScreenTwipWidthPattern,aModeIndex+1);
	TBuf<32> varNameHeight;
	_LIT(KWSERVScreenTwipHeightPattern,"SCR_TWIP_HEIGHT%d");
	varNameHeight.Format(KWSERVScreenTwipHeightPattern,aModeIndex+1);

	TSize twipsSize;
	TBool widthFound = WsIniFile->FindVar(iScreenNumber,varNameWidth,twipsSize.iWidth);
	TBool heightFound = WsIniFile->FindVar(iScreenNumber,varNameHeight,twipsSize.iHeight);	
	
	// if either of the width or height wsini reads has failed we need to generate default values
	switch(aMode.iRotation)
		{
		// CFbsBitGc::TGraphicsOrientation
		case CFbsBitGc::EGraphicsOrientationRotated90:	// deliberate drop-through
		case CFbsBitGc::EGraphicsOrientationRotated270:
			{
			// CFbsScreenDevice knows nothing about rotation, so we can't use it's PixelsTo Twips methods
			// So swap the axis here to use the correct twips per pixel ratio
			if (!widthFound)
				twipsSize.iWidth = DeviceMap().VerticalPixelsToTwips(aMode.iScreenSize.iWidth);
			if (!heightFound)
				twipsSize.iHeight = DeviceMap().HorizontalPixelsToTwips(aMode.iScreenSize.iHeight);	
			break;
			}
		case CFbsBitGc::EGraphicsOrientationNormal:		// deliberate drop-through
		case CFbsBitGc::EGraphicsOrientationRotated180:
			if (!widthFound)
				twipsSize.iWidth = DeviceMap().HorizontalPixelsToTwips(aMode.iScreenSize.iWidth);
			if (!heightFound)
				twipsSize.iHeight = DeviceMap().VerticalPixelsToTwips(aMode.iScreenSize.iHeight);
			break;
		default:
			RDebug::Print(_L("** CScreen::LoadScreenTwipsProperties Panic"));
			WS_PANIC_ALWAYS(EWsPanicFailedToInitialise);
			break;			
		}
	if (widthFound&&heightFound)
		{
		aMode.iFlags|=this->ETwipsSpecified;
		}
	aMode.iScreenTwipsSize=twipsSize;
	}


void CScreen::LoadScreenSizeProperties(TDisplayMode aDefaultDisplayMode)
	{
	for(TInt sizeLoop=0;sizeLoop<iModes->Count();sizeLoop++)
		{
		TInternalSizeMode* modePtr=(*iModes)[sizeLoop];
		if (!modePtr)
			continue;
		TInternalSizeMode& mode=*modePtr;
		TBuf<32> varDisplayMode;
		_LIT(KWSERVScreenDisplayModePattern,"SCR_WINDOWMODE%d");
		
		varDisplayMode.Format(KWSERVScreenDisplayModePattern,sizeLoop+1);
		mode.iScreenScale.iWidth=1;
		mode.iScreenScale.iHeight=1;

		TBuf<32> varLeft;
		TBuf<32> varTop;
		_LIT(KWSERVScreenLeftPattern,"SCR_LEFT%d");
		_LIT(KWSERVScreenTopPattern,"SCR_TOP%d");
		varLeft.Format(KWSERVScreenLeftPattern,sizeLoop+1);
		varTop.Format(KWSERVScreenTopPattern,sizeLoop+1);
		if (!WsIniFile->FindVar( iScreenNumber, varLeft,mode.iOrigin.iX))
			mode.iOrigin.iX=0;
		if (!WsIniFile->FindVar( iScreenNumber, varTop,mode.iOrigin.iY))
			mode.iOrigin.iY=0;
			
		TPtrC displayModeName(NULL,0);
		mode.iDefaultDisplayMode = aDefaultDisplayMode;
		// must know rotation before parsing twips
		LoadScreenRotationProperties(mode, sizeLoop);
		LoadScreenTwipsProperties(mode, sizeLoop);
		
		
		if(mode.iScreenSize.iWidth == mode.iScreenSize.iHeight && mode.iAlternativeRotations == allRotationsMask)
			{
			//square appmode with all four rotations allowed must have square twipsize
			if((mode.iFlags&ETwipsSpecified) && mode.iScreenTwipsSize.iWidth != mode.iScreenTwipsSize.iHeight)
				{
				RDebug::Print(_L("**Panic: Square appmode with all four rotations must have square twip size"));
				WS_PANIC_ALWAYS(EWsPanicFailedToInitialise);
				}
			//square appmode with all four rotations allowed must have square offset
			if(mode.iOrigin.iX != mode.iOrigin.iY)
				{
				RDebug::Print(_L("**Panic: Square appmode with all four rotations must have square offset"));
				WS_PANIC_ALWAYS(EWsPanicFailedToInitialise);
				}
			}
		else
			{
			//Everything else is treated as rectangle appmode. Square appmode not supporting all 4 rotations is considered
			//as rectangle appmode as well. Rectangle appmode suports 2 rotations at most (0 and 180, or 90 and 270)
			//first rotation of the appmode is taken to apply the corresponding rotation mask
			if(!((mode.iAlternativeRotations&KRotation0_180Mask) == mode.iAlternativeRotations
					|| (mode.iAlternativeRotations&KRotation90_270Mask) == mode.iAlternativeRotations))
				{
				RDebug::Print(_L("**Panic_DEBUG: non square appmode can only define (0,180) or (90,270) rotations"));
				WS_PANIC_DEBUG(EWsPanicFailedToInitialise);
				//in relase build, no panic, just correct the rotations set

				}
			//correct the rotations set
			mode.iAlternativeRotations &= ((ROTATION_TO_FLAG(mode.iRotation) & KRotation0_180Mask)? KRotation0_180Mask:
												KRotation90_270Mask);
			}
			
		}
//
	TInt intForFindVar=0;
	_LIT(KWSERVIniFileVarSizeMode,"SIZE_MODE");
	WsIniFile->FindVar( iScreenNumber, KWSERVIniFileVarSizeMode,intForFindVar);
	iSizeEnforcementMode=(TScreenModeEnforcement)intForFindVar;
	}

void CScreen::SetDigitiserAreas(const TSize& aUiSize)
	{  //aUiSize should be the unrotated current ui size
	//SetDigitiserAreas needs revisiting if/when we support dynamic resolutions on a screen 
    //with touch input. It is not known how digitiser coordinates will be represented if the 
    //physical display resolution changes. Currently digitisers are only supported on screen 0, 
    //and dynamic resolution only applies to higher screen numbers on real hardware.
	for(TInt sizeLoop=0;sizeLoop<iModes->Count();sizeLoop++)
		{
		TInternalSizeMode* modePtr=(*iModes)[sizeLoop];
		if (!modePtr)
			continue;
		TInternalSizeMode& mode=*modePtr;
		if(mode.iFlags & EClientDefinedDigitiserArea)
			{
			//if it's client set, keep it unchanged.
			continue;
			}
		switch (mode.iRotation)
			{
			case CFbsBitGc::EGraphicsOrientationNormal:
				mode.iPointerCursorArea=iDigitiserArea;
				continue;
			case CFbsBitGc::EGraphicsOrientationRotated90:
				mode.iPointerCursorArea.SetRect(iDigitiserArea.iTl.iY,aUiSize.iWidth-iDigitiserArea.iBr.iX,
																iDigitiserArea.iBr.iY,aUiSize.iWidth-iDigitiserArea.iTl.iX);
				break;
			case CFbsBitGc::EGraphicsOrientationRotated180:
				mode.iPointerCursorArea.SetRect(-(iDigitiserArea.iBr-aUiSize),-(iDigitiserArea.iTl-aUiSize));
				break;
			case CFbsBitGc::EGraphicsOrientationRotated270:
				mode.iPointerCursorArea.SetRect(aUiSize.iHeight-iDigitiserArea.iBr.iY,iDigitiserArea.iTl.iX,
				        aUiSize.iHeight-iDigitiserArea.iTl.iY,iDigitiserArea.iBr.iX);
				break;
			default:
			    WS_PANIC_ALWAYS(EWsPanicInvalidRotation);
			}
		}
	}

void CScreen::GetScreenSizeAndRotation(TPixelsTwipsAndRotation &aSar, TInt aScreenMode)
	{
	TSizeMode& mode=*(*iModes)[aScreenMode];
	aSar.iRotation=mode.iRotation;
	aSar.iPixelSize=mode.iScreenSize;
	aSar.iTwipsSize=mode.iScreenTwipsSize;
	if (aSar.iTwipsSize.iWidth==0)
		{
		aSar.iTwipsSize.iWidth  = iDeviceMap->HorizontalPixelsToTwips(aSar.iPixelSize.iWidth);
		aSar.iTwipsSize.iHeight = iDeviceMap->VerticalPixelsToTwips(aSar.iPixelSize.iHeight);
		}
	}

void CScreen::GetScreenSizeAndRotation(TPixelsAndRotation &aSar, TInt aScreenMode)
	{
	TSizeMode& mode=*(*iModes)[aScreenMode];
	aSar.iRotation=mode.iRotation;
	aSar.iPixelSize=mode.iScreenSize;
	}

TBool CScreen::SetScreenModeEnforcement(TInt aMode)
	{
	if (aMode<0 || aMode>ESizeEnforcementPixelsTwipsAndRotation)
		return EFalse;
	TScreenModeEnforcement newMode=(TScreenModeEnforcement)aMode;
	if (newMode!=iSizeEnforcementMode)
		{
		iSizeEnforcementMode=newMode;
		CWsWindowGroup::SetScreenDeviceValidStates(this);
		ResetFocus(NULL);
		}
	return ETrue;
	}

void CScreen::IncContrast()
	{
	TInt contrast;
	if (iMaxContrast<0)			//If failed to get it sofar get it again
		TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::EContrast, HAL::Get(iScreenNumber,HALData::EDisplayContrastMax,iMaxContrast));
	if (TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::EContrast, HAL::Get(iScreenNumber,HALData::EDisplayContrast,contrast)))
		return;
	if (contrast==iMaxContrast)
		contrast=-1;
	TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::EContrast, HAL::Set(iScreenNumber,HALData::EDisplayContrast,++contrast));
	}

void CScreen::DecContrast()
	{
	TInt contrast;
	if (TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::EContrast, HAL::Get(iScreenNumber,HALData::EDisplayContrast,contrast)))
		return;
	if (contrast==0)
		{
		if (iMaxContrast<0 && TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::EContrast,
															HAL::Get(iScreenNumber,HALData::EDisplayContrastMax,iMaxContrast)))
			return;
		contrast=iMaxContrast+1;
		}
	TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::EContrast, HAL::Set(iScreenNumber,HALData::EDisplayContrast,--contrast));
	}

void CScreen::IncBrightness()
	{
	TInt brightness;
	if (iMaxBrightness<0)			//If failed to get it sofar get it again
		TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::EBackLight, HAL::Get(iScreenNumber,HALData::EDisplayBrightnessMax,iMaxBrightness));
	if (TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::EBackLight, HAL::Get(iScreenNumber,HALData::EDisplayBrightness,brightness)))
		return;
	if (brightness==iMaxBrightness)
		brightness=-1;
	TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::EBackLight, HAL::Set(iScreenNumber,HALData::EDisplayBrightness,++brightness));
	}

void CScreen::DecBrightness()
	{
	TInt brightness;
	if (TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::EBackLight, HAL::Get(iScreenNumber,HALData::EDisplayBrightness,brightness)))
		return;
	if (brightness==0)
		{
		if (iMaxBrightness<0 && TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::EBackLight,
													HAL::Get(iScreenNumber,HALData::EDisplayBrightnessMax,iMaxBrightness)))
			return;
		brightness=iMaxBrightness+1;
		}
	TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::EBackLight, HAL::Set(iScreenNumber,HALData::EDisplayBrightness,--brightness));
	}
TInt CScreen::GetScreenSizeModeList(RArray<TInt>& aList) const
	{
	aList.Reset();
	TInt numModes=iNumScreenSizeModes;
	TInt err=aList.Reserve(numModes);
	if (err!=KErrNone)
		return err;
	TInt index;
	for (index=0; index<numModes; ++index)
		{
		TSizeMode* modePtr=(*iModes)[index];
		if (modePtr)
			aList.Append(index);	//Can't fail due to reserve
		}
	TInt count=aList.Count();
	return count;
	}

TInt CScreen::GetScreenSizeModeListL()
	{
	RArray<TInt> list;
	CleanupClosePushL(list);
	TInt count=GetScreenSizeModeList(list);
		User::LeaveIfError(count);
	CWsClient::ReplyBuf(&list[0], count*sizeof(TInt));
	CleanupStack::PopAndDestroy(&list);
	return count;
	}

void CScreen::SetInitialScreenSizeModeAndRotation()
	{	//Set first app mode that supports current supported rotations if available
	TInt index;
	TInt firstMode = -1;
	TInt bestMode = -1;
	// Since all screen rotation modes are supported.
	TBitFlags32 rotationFlags = CFbsBitGc::EGraphicsOrientationNormal|CFbsBitGc::EGraphicsOrientationRotated90|CFbsBitGc::EGraphicsOrientationRotated180|CFbsBitGc::EGraphicsOrientationRotated270;
	for (index=0; index<iModes->Count(); ++index)
		{
		TSizeMode* modePtr=(*iModes)[index];
		if (modePtr)
			{
			if (firstMode == -1)
				{
				firstMode = index;
				}
			if (rotationFlags.IsSet((TInt)modePtr->iRotation))
				{
				bestMode = index;
				break;
				}
			}
		}

	if (bestMode != -1)	//found a mode that supports current supported rotations
		{
		iScreenSizeMode = bestMode;
		}
	else
		{
		if (firstMode != -1)	//could only find a mode that doesnt support current supported rotations
			{
			iScreenSizeMode = firstMode;
			}
		else
			{
			return;	//couldn't find a mode at all
			}
		}
	if(iDisplayPolicy)
		{
		iDisplayPolicy->SetLastAppMode(iScreenSizeMode);
		}

    SetDigitiserAreas(iScreenDevice->SizeInPixels()); //unrotated size in pixels
    
    // Here we are mixing CFbsBitGc::TGraphicsOrientation with MWsScene::TSceneRotation
    // As they both have same values it is fine for now
    iScene->SetSceneRotation(GcToScreen(ScreenSizeModeData().iRotation)); //set rotation
	}


TDisplayMode CScreen::FirstDefaultDisplayMode() const
	{
	TInt mode=-1;
	while ((*iModes)[++mode]==NULL)
		{
		WS_ASSERT_DEBUG(mode<iModes->Count()-1,EWsPanicInvalidScreenSizeMode);
		}
	return((*iModes)[mode]->iDefaultDisplayMode);
	}

void CScreen::AddRedrawRegion(const TRegion& aRegion, TBool aSchedule, TRedrawDepth aDepth)
	{
	iRedraw->AddRedrawRegion(aRegion, aSchedule, aDepth);
	}

void CScreen::ScheduleRender(const TTimeIntervalMicroSeconds& aFromNow)
	{
	iRedraw->ScheduleRender(aFromNow);
	}

void CScreen::DoRedrawNow()
	{
	iRedraw->DoRedrawNow();
	}

// See CScreenRedraw::IsUpdatePending() for important notes on usage.
void CScreen::RedrawNowIfPending()
	{
	if(iRedraw->IsUpdatePending())
		DoRedrawNow();
	}

TBool CScreen::IsQuickFadeScheduled( CWsWindow* aWin ) const
	{
	return iRedraw->IsQuickFadeScheduled( aWin );
	}

void CScreen::RemoveFromQuickFadeList( CWsWindow* aWin )
	{
	iRedraw->RemoveFromQuickFadeList( aWin );
	}

void CScreen::AcceptFadeRequest( CWsWindow* aWin, TBool aIsFaded )
	{
	iRedraw->AcceptFadeRequest( aWin, aIsFaded );
	}

// implementing MWsScreen

const TTime& CScreen::Now() const
	{
	return iRedraw->Now();
	}

void CScreen::ScheduleAnimation(TAnimType aType, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop, CWsWindow* aWindow)
	{
	iRedraw->ScheduleAnimation(aType, aRect,aFromNow,aFreq,aStop, aWindow);
	}

TBool CScreen::IsScheduled(TAnimType aType, const TRect& aRect, CWsWindow* aWindow) const
	{
	return iRedraw->IsScheduled(aType, aRect, aWindow);
	}

void CScreen::OnAnimation(TRequestStatus* aFinished)
	{
	iRedraw->OnAnimation(aFinished);
	}
	
void CScreen::Redraw()
	{
	STACK_REGION bounds;
	bounds.AddRect(DrawableArea());
	AddRedrawRegion(bounds);
	bounds.Close();
	}

TBool CScreen::RedrawInvalid(const TArray<TGraphicDrawerId>& aInvalid)
	{
	TBool wasDirty = EFalse;
	STACK_REGION bounds;
	bounds.AddRect(DrawableArea());
	STACK_REGION dirty;
	TWalkWindowTreeCalcInvalidGraphics calc(&bounds,dirty,aInvalid);
	if(calc.CreateSubRegion())
		{
		calc.CalcInvalid(*this);
		if(dirty.CheckError() || dirty.Count())
			{
			Redraw();
			wasDirty = ETrue;
			}
		calc.DestroyRegions();
		}
	dirty.Close();
	bounds.Close();
	return wasDirty;
	}

/**
 Overidding MWsObjectProvider
*/
TAny* CScreen::ResolveObjectInterface(TUint aTypeId)
	{
	TAny* interface = NULL;

	switch (aTypeId)
		{
		case MWsWindow::EWsObjectInterfaceId:
			interface = static_cast<MWsWindow*>(RootWindow());
			break;
		case MWsScreenConfigList::EWsObjectInterfaceId:
			interface = static_cast<MWsScreenConfigList*>(this);
			break;
		case MWsScreenConfig::EWsObjectInterfaceId:
			interface = static_cast<MWsScreenConfig*>(this);
			break;
		case MWsWindowTree::EWsObjectInterfaceId:
			interface = static_cast<MWsWindowTree*>(this);
		}

	if (!interface)
		interface = iRedraw->ResolveObjectInterface(aTypeId);

	return interface;
	}

void CScreen::SendTree() const
	{
	if(!iWindowTreeObserver)
		return;
	
	TWalkWindowTreeSendState wtw(*iWindowTreeObserver);
	RootWindow()->WalkWindowTreeBackToFront(wtw, EVisitParentNodesFirst);

	//Standard text cursors
	RWsTextCursor* cursor = CWsTop::CurrentTextCursor();
	if(cursor)
		cursor->SendState(*iWindowTreeObserver);
	
	//Floating Sprites	
	SpriteManager()->SendState(*iWindowTreeObserver);
	
	//Window Group Chains
	for(CWsWindowGroup *group=RootWindow()->Child(); group!=NULL; group=group->NextSibling())
		{
		group->SendStateWindowGroupChain(*iWindowTreeObserver);
		}
	}

TDisplayMode CScreen::DisplayMode() const
	{
	return iScreenDevice->DisplayMode();
	}

TSize CScreen::SizeInPixels() const
	{
	return iScreenDevice->SizeInPixels();
	}

TSize CScreen::SizeInTwips() const
	{
	return iScreenDevice->SizeInTwips();
	}

void CScreen::DiscardAllSchedules()
	{
	iRedraw->DiscardAllSchedules();
	}

void CScreen::ScheduleRegionUpdate(const TRegion* aDefinitelyDirty)
	{
	iRedraw->ScheduleRegionUpdate(aDefinitelyDirty);
	}

TBool CScreen::IsDSAClientWindow( const CWsClientWindow* aWin ) const
	{
	TBool res = EFalse; 
	if ( ! iDirects.IsEmpty() )
		{
		TSglQueIter<CWsDirectScreenAccess> iter( (TSglQueBase&)iDirects );
		iter.SetToFirst();
		CWsDirectScreenAccess* dsa;
		while ( (dsa = iter++) != NULL && !res )
			{
			res =  (dsa->ClientWindow() == aWin) && (dsa->IsVisible());
			}
		}
	return res;	
	}

/**
Update the UI element composition method based on whether
there are any externals surfaces present and the current display mode. 
If the method changes, recomposition is triggered.
*/
void CScreen::UpdateCompositionMode()
	{
	// do nothing
	}

void CScreen::ElementAdded()
	{
	UpdateCompositionMode();
	}

void CScreen::ElementRemoved()
	{
	UpdateCompositionMode();
	}

CRegisteredSurfaceMap* CScreen::SurfaceMap()
	{
	return iSurfaceMap;
	}

void CScreen::InitializeSceneL()
	{
	// Ensure the surface is not valid to start with.
	iDsaSurface = TSurfaceId::CreateNullId();
	iWindowElementSet = CWindowElementSet::NewL(*iScene);
	}

MWsElement* CScreen::CreateUiElementL(const TRect& aExtent)
	{
	MWsElement* pElement = iScene->CreateSceneElementL();
	
	TUint32 flags = 0;
	pElement->GetRenderStageFlags(flags);
	flags |= MWsElement::EElementIsIndirectlyRenderedUserInterface;
	pElement->SetRenderStageFlags(flags);
	    
    iScene->InsertSceneElement(pElement, NULL);

	pElement->SetDestinationRectangle(aExtent);
	pElement->SetSourceRectangle(aExtent);	//initial guess... updated by PositionUiElements

	return pElement;
	}

void CScreen::InitializeUiElementsL()
	{
	const TRect screenRect(iScreenDevice->SizeInPixels());
	MWsElement* pElement = CreateUiElementL(screenRect);
	
	if(HasAlpha())
		{
		TUint32 flags = 0;
		pElement->GetTargetRendererFlags(flags);
		flags |= MWsElement::EElementTransparencySource;
		pElement->SetTargetRendererFlags(flags);
		}
	}

TInt CScreen::InitializeDsaSurface()
	{
	WS_ASSERT_DEBUG(iDsaSurface.IsNull(),EWsPanicInvalidOperation);
	iDsaDevice->GetSurface(iDsaSurface);
	// Currently Surface Manager does not recognise DSA surface IDs originating
	// from the Screen Driver.  This causes it to fail to register such
	// surfaces.  OpenWF should be amended to properly register DSA surfaces.
	iScene->RegisterSurface(iDsaSurface);

	return KErrNone;
	}

TSize CScreen::DSASizeInPixels() const
	{
	if(iDsaDevice)
		{
		return iDsaDevice->SizeInPixels();
		}
	else
		{
		return TSize(0,0);
		}
	}

MWsTextCursor* CScreen::RenderStageTextCursor() const
	{
	return iRedraw->RenderStageTextCursor();
	}

void CScreen::ClearDsaSurface(const TRect& area, const TRgb& color)
	{
	WS_ASSERT_DEBUG(iDsaGc, EWsPanicInvalidOperation);
	iDsaGc->SetBrushStyle(CFbsBitGc::ESolidBrush);
	iDsaGc->SetPenStyle(CFbsBitGc::ENullPen);
	iDsaGc->SetBrushColor(color);
	iDsaGc->DrawRect(area);
	iDsaDevice->Update();
	}

void CScreen::ReleaseDsaScreenDevice()
	{
	//This function checks if any of the DSA currently active on the screen is actually used to draw
	//If not it unregister the DSA surface and destroys the iDsaDevice.
	//This function should be called only by a drawing DSA so a surface should be in place.
	iNumberDrawingDsa--;
	if(iNumberDrawingDsa == 0)
		{
		WS_ASSERT_DEBUG(!iDsaSurface.IsNull(),EWsPanicInvalidOperation);
		delete iDsaGc;
		iDsaGc = NULL;
		// Currently Surface Manager does not recognise DSA surface IDs originating
		// from the Screen Driver.  This causes it to fail to unregister such
		// surfaces.  OpenWF should be amended to properly register DSA surfaces.
		iScene->UnregisterSurface(iDsaSurface);

		delete iDsaDevice;
		iDsaDevice = NULL;
		//the old surface Id is now meaningless
		iDsaSurface = TSurfaceId::CreateNullId();
		}
	}

TInt CScreen::SetConfiguration(const TDisplayConfiguration& aConfigInput)
	{
	TInt reply = KErrNone;
	if(iDisplayControl)
		{
		TDisplayConfiguration config(aConfigInput);
		TRect sizeModePosition;
		if (iDisplayPolicy)
			{	//validate config and update to a valid hardware config
			reply = iDisplayPolicy->GetSizeModeConfiguration(iScreenSizeMode,config,sizeModePosition);
			if (reply >= KErrNone)
				{//set appmode in policy
				if (iDisplayMapping)
					{
					iDisplayMapping->SetSizeModeExtent(sizeModePosition,MWsDisplayMapping::KOffsetAll);
					}
				}
			}
		else
			{	//exessive strategy: limit rotation agains curr app mode.
				//really we want the system to accept the rotation change regardless of the app mode.
			TDisplayConfiguration::TRotation newRot;
			if (aConfigInput.GetRotation(newRot))
				{	//This should cast between rotation enumertaions "properly"
				if (!(iModes[0][iScreenSizeMode]->iAlternativeRotations&(1<<newRot)))
					{
					reply=KErrArgument;
					}
				}
			}
		if (reply < KErrNone)
			{
			return reply;
			}
		MWsScene::TSceneRotation oldRotation;
		oldRotation = iScene->SceneRotation();
		TSize newUiSize;
		config.GetResolution(newUiSize);
		TDisplayConfiguration oldConfig;
		iDisplayControl->GetConfiguration(oldConfig);
		if(iFlags&EHasDynamicSizeModes)
			{
			reply = iFallbackMap->Resize(newUiSize);
			}
		if (reply >= KErrNone)
			{
			reply=iDisplayControl->SetConfiguration(config);
			}	
		if (reply==KErrNone)
			{
			TSize oldConfigRes;
			oldConfig.GetResolution(oldConfigRes);
            TDisplayConfiguration newConfig;
			if (oldConfigRes.iWidth == 0 || oldConfigRes.iHeight == 0)
				{
				iDisplayControl->GetConfiguration(newConfig);
				RecalculateModeTwips(&newConfig);	//needs res and twips information
				}
			UpdateDynamicScreenModes();
			
			//update the last set config in the config change notifier to 
			//prevent SetConfiguration() from being called again!
			newConfig.ClearAll();
			iDisplayControl->GetConfiguration(newConfig);
			iConfigChangeNotifier->UpdateLastSetConfiguration(newConfig); 			
			
			TWindowServerEvent::NotifyDrawer(TWservCrEvent(TWservCrEvent::EScreenSizeModeAboutToChange, iScreenSizeMode));
			// This will remove all the DSA elements from the scene
			AbortAllDirectDrawing(RDirectScreenAccess::ETerminateRotation);
			
			//SetDigitiserAreas needs revisiting if/when we support dynamic resolutions
			//on a screen with touch input.
			//SetDigitiserAreas(newUiSize);
			
			//failure here should only be because of DSA orientation change failure, which shouldn't happen, either.
			//Or there may be no change to do.
			(void)UpdateOrientation(&oldRotation);
			
			iWindowElementSet->ResubmitAllElementExtents();
			if(iDsaDevice && iDsaDevice->GraphicsAccelerator())
				{
				iDsaDevice->ChangeScreenDevice(iDsaDevice); // orientation has changed, therefore we need to re-initialise the screen device's graphics accelerator
				}
			
			iRootWindow->AdjustCoordsDueToRotation();

			//TODO jonas: we'd like to not have to clear at all... make the actual change to compositor etc lazily!
			if(BlankScreenOnRotation())
				{
				iRootWindow->ClearDisplay();
				}

			CWsTop::ClearAllRedrawStores();
			DiscardAllSchedules();
			iRootWindow->InvalidateWholeScreen();
			CWsWindowGroup::SetScreenDeviceValidStates(this);
			TWindowServerEvent::SendScreenDeviceChangedEvents(this);
			}
		else
			{
			return reply;
			}
		}
	else
		{
		reply = KErrNotSupported;
		}
	return reply;
	}

/**
 * Updates the screen device display properties. This is to ensure the screen device is 
 * consistent with any configuration changes not made using CScreen::SetConfiguration.
 * 
 * @param aConfigInput a fully populated display configuration
 **/
TInt CScreen::UpdateConfiguration(const TDisplayConfiguration& aConfigInput)
    {
    TInt reply = KErrNone;
    if(iDisplayControl)
        {
        TDisplayConfiguration config(aConfigInput);
        TRect sizeModePosition;
        if (iDisplayPolicy)
            {   //validate config and update to a valid hardware config
            reply = iDisplayPolicy->GetSizeModeConfiguration(iScreenSizeMode,config,sizeModePosition);
            if (reply >= KErrNone)
                {//set appmode in policy
                if (iDisplayMapping)
                    {
                    iDisplayMapping->SetSizeModeExtent(sizeModePosition,MWsDisplayMapping::KOffsetAll);
                    }
                }
            }
        else
            {   //exessive strategy: limit rotation agains curr app mode.
                //really we want the system to accept the rotation change regardless of the app mode.
            TDisplayConfiguration::TRotation newRot;
            if (aConfigInput.GetRotation(newRot))
                {   //This should cast between rotation enumertaions "properly"
                if (!(iModes[0][iScreenSizeMode]->iAlternativeRotations&(1<<newRot)))
                    {
                    reply=KErrArgument;
                    }
                }
            }

        MWsScene::TSceneRotation oldRotation;
        oldRotation = iScene->SceneRotation();
        TSize newUiSize;
        config.GetResolution(newUiSize);
        if(iFlags&EHasDynamicSizeModes)
            {
            reply = iFallbackMap->Resize(newUiSize);
            }

        RecalculateModeTwips(&config);   //needs res and twips information
        UpdateDynamicScreenModes();        
        
        TWindowServerEvent::NotifyDrawer(TWservCrEvent(TWservCrEvent::EScreenSizeModeAboutToChange, iScreenSizeMode));
        // This will remove all the DSA elements from the scene
        AbortAllDirectDrawing(RDirectScreenAccess::ETerminateRotation);
        
        //SetDigitiserAreas needs revisiting if/when we support dynamic resolutions
        //on a screen with touch input.
        //SetDigitiserAreas(newUiSize);
        
        //failure here should only be because of DSA orientation change failure, which shouldn't happen, either.
        //Or there may be no change to do.
        (void)UpdateOrientation(&oldRotation);
        
        iWindowElementSet->ResubmitAllElementExtents();
        if(iDsaDevice && iDsaDevice->GraphicsAccelerator())
            {
            iDsaDevice->ChangeScreenDevice(iDsaDevice); // orientation has changed, therefore we need to re-initialise the screen device's graphics accelerator
            }
        
        iRootWindow->AdjustCoordsDueToRotation();

        //TODO jonas: we'd like to not have to clear at all... make the actual change to compositor etc lazily!
        if(BlankScreenOnRotation())
            {
            iRootWindow->ClearDisplay();
            }

        CWsTop::ClearAllRedrawStores();
        DiscardAllSchedules();
        iRootWindow->InvalidateWholeScreen();
        CWsWindowGroup::SetScreenDeviceValidStates(this);
        TWindowServerEvent::SendScreenDeviceChangedEvents(this);
        }
    else
        {
        reply = KErrNotSupported;
        }
    return reply;  
    }

void CScreen::UpdateDynamicScreenModes()
	{
	WS_ASSERT_DEBUG(iDisplayControl,EWsPanicNoDisplayControl);
	TDisplayConfiguration newConfig;
	iDisplayControl->GetConfiguration(newConfig);
	TSize res;
	TSize twips;
	newConfig.GetResolution(res);
	newConfig.GetResolutionTwips(twips);
	for (TInt i=0; i<iModes->Count(); i++)
		{
		if ((*iModes)[i] && ((*iModes)[i]->iFlags & EDynamic))
			{
			(*iModes)[i]->iScreenSize = res;
			(*iModes)[i]->iScreenTwipsSize = twips;
			}
		}
	}

void CScreen::RecalculateModeTwips(const TDisplayConfiguration* aConfig)
	{
	TDisplayConfiguration config;
	iDisplayControl->GetConfiguration(config);
	TSize res;
	TSize twips;
	if (aConfig)	//called from SetConfiguration
		{
		aConfig->GetResolution(res);
		if (res.iWidth == 0 || res.iHeight == 0)
			{
			return;
			}
		aConfig->GetResolutionTwips(twips);
		}
	else	//called from DisplayChangeNotifier during attach
		{
		config.GetResolution(res);
		if ((res.iWidth == 0 || res.iHeight == 0) && !iDisplayPolicy)
			{
			return;
			}
		config.GetResolutionTwips(twips);
		}
	TInt err=KErrNone;
	TInt flags=0;
	for (TInt ii=0; ii<iModes->Count(); ii++)
		{	//for every mode
		TRAP(err, flags = ModePtrL(ii)->iFlags);
		if (err != KErrNone || flags&(ETwipsSpecified|EDynamic))
			{	//continue if mode doesnt exist,twips specified or dynamic mode specified
			continue;
			}

		if (iDisplayPolicy)
			{	//get ideal config for app mode from policy
			TRect modePosition;
			config.ClearAll();
			TInt err = iDisplayPolicy->GetSizeModeConfiguration(ii,config,modePosition);
			if (err != KErrNone)
				{	//nothing we can do, the twips will not be calculated correctly
				continue;
				}
			config.GetResolution(res);
			config.GetResolutionTwips(twips);
			}
		TSizeMode* modePtr=(*iModes)[ii];
		modePtr->iScreenTwipsSize.iWidth = (twips.iWidth * modePtr->iScreenSize.iWidth)/
				res.iWidth;
		modePtr->iScreenTwipsSize.iHeight = (twips.iHeight * modePtr->iScreenSize.iHeight)/
				res.iHeight;
		}
	
	}

TInt CScreen::AddNotificationClient(CWsClient *aClient)
	{
	TInt err = iWsClientList.InsertInAddressOrder(aClient);
	if(!(err == KErrNone || err == KErrAlreadyExists))
		{
		return err;
		}
	return KErrNone;
	}
void CScreen::RemoveNotificationClient(CWsClient *aClient)
	{
	TInt index = iWsClientList.FindInAddressOrder(aClient);
	if(index != KErrNotFound)
		{
		iWsClientList.Remove(index);
		}
	}
TInt CScreen::GetNotificationClients(RPointerArray<CWsClient>& aClientsArray)
	{
	TInt err = aClientsArray.Reserve(iWsClientList.Count());
	if(err != KErrNone)
		return err;
	
	for(TInt i = 0; i < iWsClientList.Count(); i++)
		{
		aClientsArray.Append(iWsClientList[i]);
		}
	return KErrNone;
	}

TInt CScreen::FindNotificationClient (CWsClient *aClient)
	{
	return iWsClientList.FindInAddressOrder(aClient);
	}

// implementing MWsScreenConfig... this might be better as RS interface
TSize CScreen::ScreenModeSizeInPixels() const
	{
	return (*iModes)[iScreenSizeMode]->iScreenSize;
	}
TInt CScreen::Stride() const
	{
	return 0;
	}
TInt CScreen::SizeMode() const
	{
	return iScreenSizeMode;

	}
TSize CScreen::ScalingFactor() const
	{
	return (*iModes)[iScreenSizeMode]->iScreenScale;
	}
TPoint CScreen::Origin() const
	{
	return (*iModes)[iScreenSizeMode]->iOrigin;
	}
TPoint CScreen::ScaledOrigin() const
	{
	return (*iModes)[iScreenSizeMode]->ScaledOrigin();
	}

const CScreen::TInternalSizeMode* CScreen::ModePtrL(TInt aIndex) const
	{
	if (aIndex>=iModes->Count() || aIndex<0)
		{
		User::Leave(KErrArgument);
		}
	if (iModes==NULL)
		{
		User::Leave(KErrNotReady);
		}
	TInternalSizeMode* modePtr=(*iModes)[aIndex];
	if (modePtr==NULL)
		{
		User::Leave(KErrArgument);
		}
	return modePtr;
	}

TDisplayMode CScreen::DisplayModeL(TInt aIndex) const
	{
	return ModePtrL(aIndex)->iDefaultDisplayMode;
	}
TSize CScreen::ScreenModeSizeInPixelsL(TInt aIndex) const
	{
	return ModePtrL(aIndex)->iScreenSize;
	}
TSize CScreen::ScreenModeSizeInTwipsL(TInt aIndex) const 
	{
	return ModePtrL(aIndex)->iScreenTwipsSize;
	}

CFbsBitGc::TGraphicsOrientation CScreen::OrientationL(TInt aIndex) const
	{
	return ModePtrL(aIndex)->iRotation;
	}
TInt CScreen::AvailableOrientationsL(TInt aIndex) const
	{
	return ModePtrL(aIndex)->iAlternativeRotations;
	}
TSize CScreen::ScalingFactorL(TInt aIndex) const
	{
	return ModePtrL(aIndex)->iScreenScale;
	}
TPoint CScreen::OriginL(TInt aIndex) const
	{
	return ModePtrL(aIndex)->iOrigin;
	}
TPoint CScreen::ScaledOriginL(TInt aIndex) const
	{
	return ModePtrL(aIndex)->ScaledOrigin();
	}
TInt CScreen::ModeFlagsL(TInt aIndex) const
	{
	return ModePtrL(aIndex)->iFlags;
	}
void CScreen::SetCurrentScreenModeAttributes(const TSizeMode &aModeData)
	{	
	TSizeMode* modeToOverwrite=(*iModes)[iScreenSizeMode];
	*modeToOverwrite=aModeData;
	}

void CScreen::ScheduleWindow(CWsWindow* aWindow)
	{
	iRedraw->ScheduleWindow(aWindow);
	}

void CScreen::RemoveFromScheduledList(CWsWindow* aWindow)
	{
	iRedraw->RemoveFromScheduledList(aWindow);
	}

void CScreen::RemoveFromTimedDrawList(CWsWindow* aWindow)
	{
	iRedraw->RemoveFromTimedDrawList(aWindow);
	}

void CScreen::SetupVisibleRegionTracking(CWsWindow& aWindow, TBool aRegister) const
	{
	if(ChangeTracking() && iWindowVisibilityNotifier)
		{
		if(aRegister)
			{
			iWindowVisibilityNotifier->RegisterWindow(aWindow);
			}
		else
			{
			iWindowVisibilityNotifier->UnregisterWindow(aWindow);
			}
		}
	}

TBool CScreen::IsAnimating() const
    {
    return iRedraw->IsAnimating();
    }
