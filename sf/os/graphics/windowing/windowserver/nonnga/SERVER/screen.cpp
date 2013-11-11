// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "server.h"
#include "wstop.h"
#include "rootwin.h"
#include "walkwindowtree.h"
#include "offscreenbitmap.h"
#include "EVENT.H"
#include "windowgroup.h"
#include "gc.h"
#include "inifile.h"
#include "pointer.h"

#include "debugbar.h"
#include "ScreenRedraw.h"
#include "wspluginmanager.h"

GLREF_D CDebugLogBase *wsDebugLog;
GLREF_D TDisplayMode ParseDisplayMode(const TDesC& aModeName);

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
	delete iMap;
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
	
//
// CScreen
//
CScreen::CScreen(): iDirects(_FOFF(CWsDirectScreenAccess,iLink)), iMaxContrast(-1), iMaxBrightness(-1)
	{
	}

CScreen::~CScreen()
	{
	delete iDebugBar;
	delete iOffScreenBitmap;
	delete iScreenGdi;
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
	delete iScreenDevice;
	delete iFallbackMap;
	delete iSpriteManager;
	delete iRedraw;
	}

void CScreen::ConstructL(const TRect& aDigitiserArea, TInt aScreenNumber )
	{
	iScreenNumber = aScreenNumber ;

	if (wsDebugLog)
		{
		_LIT(KWSERVInitScreen,"Initialising for Screen %d");
		wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant, KWSERVInitScreen, aScreenNumber);
		}

	CreateScreenDeviceL();
	iScreenDevice->SetAutoUpdate(EFalse);
	iPhysicalScreenSize=iScreenDevice->SizeInPixels();
	LoadScreenSizesL(iPhysicalScreenSize);
	// mode 0 might not be available, get the first/lowest valid mode
	SetInitialScreenSizeMode();
	
	iFallbackMap = CFallbackMap::NewL(this);
//
	iScreenGdi=CFbsBitGc::NewL();
	iScreenGdi->Activate(iScreenDevice);
	LoadScreenSizeProperties();
	SetDigitiserAreas(iScreenDevice->SizeInPixels(),aDigitiserArea);
//
	iScreenGdi->SetOrientation(Orientation());
	const TSizeMode& sizeMode=ScreenSizeModeData(ScreenSizeMode());
	iScreenDevice->SetScalingFactor(sizeMode.iOrigin,sizeMode.iScreenScale.iWidth,sizeMode.iScreenScale.iHeight,1,1);
	iScreenDevice->ChangeScreenDevice(NULL);    //This is necessary to initialise the screen
//
	// Off Screen Bitmaps
	iOffScreenBitmap=CWsOffScreenBitmap::NewL(this);
	TSize osbSize(iOffScreenBitmap->Bitmap()->SizeInPixels());
	TSize osbTwips(sizeMode.iScreenTwipsSize);
	if (osbSize!=sizeMode.iScreenSize)
		{
		// The specified screen twips size is for the specified screen pixel size, however the OSB
		// is potentially larger as it needs to hold the maximum possible screen size, so we need
		// to scale the twips size up correspondingly.
		osbTwips.iWidth=sizeMode.iScreenTwipsSize.iWidth*osbSize.iWidth/sizeMode.iScreenSize.iWidth;
		osbTwips.iHeight=sizeMode.iScreenTwipsSize.iHeight*osbSize.iHeight/sizeMode.iScreenSize.iHeight;
		}
	iOffScreenBitmap->Bitmap()->SetSizeInTwips(osbTwips);		

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

	SetShadowVector(TPoint(EDefaultShadowX,EDefaultShadowY));

	iRedraw = CScreenRedraw::NewL(*this);

	iRootWindow = new (ELeave) CWsRootWindow(NULL, this);
	iRootWindow->ConstructL();

	TInt refreshRate = 1000000;
	_LIT(KDebugBar, "DEBUGBAR");
	if (WsIniFile->FindVar(refreshRate, KDebugBar))
		{
		if (refreshRate < 100000)
			refreshRate = 50000;
		const TPoint origin = CurrentScreenModeOrigin();
		iDebugBar = CDebugBar::NewL(this, TRect(origin.iX,origin.iY,origin.iX+CurrentScreenSize().iWidth,origin.iY+16), refreshRate);
		}
	// Default fading parameters
	iBlackMap=128;
	iWhiteMap=255;
	
	iSpriteManager = CWsSpriteManager::NewL();
	//Look for fading plugins here.
	_LIT(KDefaultFaderPluginName, "wsfader");
  	_LIT(KFaderPluginIni,"FADER");
	CWsPluginManager* mgr = CWsTop::WindowServer()->PluginManager();
	if (mgr)
		{
	  	TPtrC faderName;
  		if (WsIniFile->FindVar(iScreenNumber, KFaderPluginIni, faderName))
      		iFader = mgr->FindNamedImplementation<MWsFader>(faderName);
    	else	
		  	iFader = mgr->FindNamedImplementation<MWsFader>(KDefaultFaderPluginName);
		}
	
	_LIT(KBltOffScreenBitmap,"BLTOFFSCREENBITMAP");
	if (WsIniFile->FindVar(iScreenNumber, KBltOffScreenBitmap))
		{
		iFlags|=EBltOffScreenBitmap;
		}	
	}

void CScreen::CreateScreenDeviceL()
	{
	_LIT(KScreenMode,"SCREENMODE");
	_LIT(KWindowMode,"WINDOWMODE");
	TPtrC screenModeName;
	TDisplayMode screenMode = ENone;
	if (WsIniFile->FindVar(iScreenNumber, KScreenMode, screenModeName))
		{
		screenMode = ParseDisplayMode(screenModeName);
		}
	else if (WsIniFile->FindVar(iScreenNumber, KWindowMode, screenModeName))
		{
		screenMode = ParseDisplayMode(screenModeName);
		}

	if (screenMode != ENone)
		{
		if(DoCreateScreenDevice(screenMode))
			return;
		}

	// No screen mode was specified, or we failed creating the specified screen
	// mode. Default to as high display mode as possible.

	// Check if the screen device supports 16M colors (it can only support one of
	// 16M, 16MU, 16MA, 16MAP, or none of them)
	screenMode = CFbsDevice::DisplayMode16M();
	if(screenMode != ENone)
		{
		if(DoCreateScreenDevice(screenMode))
			return;
		}
	
	// Try creating the screen device with all available display modes, going from best to worst
	__ASSERT_COMPILE(EColorLast == 14); // if any display mode is added to TDisplayMode we must update the list below
	// (the list below contains all enums in TDisplayMode except ENone, ERgb, EColorLast)
	if(DoCreateScreenDevice(EColor16MAP))
		return;
	if(DoCreateScreenDevice(EColor16MA))
		return;
	if(DoCreateScreenDevice(EColor16MU))
		return;
	if(DoCreateScreenDevice(EColor16M))
		return;
	if(DoCreateScreenDevice(EColor64K))
		return;
	if(DoCreateScreenDevice(EColor4K))
		return;
	if(DoCreateScreenDevice(EColor256))
		return;
	if(DoCreateScreenDevice(EColor16))
		return;
	if(DoCreateScreenDevice(EGray256))
		return;
	if(DoCreateScreenDevice(EGray16))
		return;
	if(DoCreateScreenDevice(EGray4))
		return;
	if(DoCreateScreenDevice(EGray2))
		return;
		
	User::Leave(KErrNotSupported);
	}

TBool CScreen::DoCreateScreenDevice(TDisplayMode aScreenMode)
	{
	TRAPD(err, iScreenDevice = CFbsScreenDevice::NewL(iScreenNumber, aScreenMode));
	return (err == KErrNone);
	}

void CScreen::AbortDSAs(RDirectScreenAccess::TTerminationReasons aReason,TSglQue<CWsDirectScreenAccess>& aDirects)
	{
	if (aDirects.IsEmpty())
		return;
	TInt nofDSAs = 0;
	TSglQueIter<CWsDirectScreenAccess> iter(aDirects);
	CWsDirectScreenAccess* direct;
	while (( direct=iter++)!=NULL )
		{
		nofDSAs++;
		direct->SignalAbort( aReason );
		}

	TRequestStatus timerStatus;
	RTimer& timer=CWsTop::Timer();
	timer.Cancel();
	
	TRequestStatus** cancelReqList = (TRequestStatus**) User::AllocZ( sizeof( TRequestStatus* ) * (nofDSAs + 1)  );  
	if ( NULL != cancelReqList  )
		{		
			TInt dsaNo = 1; 
		
			timer.After( timerStatus, KDSAAbortingImmediateRespAwaitFrameMicrosec );		

			iter.SetToFirst();
			while (( direct=iter++)!=NULL )
				{
					cancelReqList[ dsaNo ] =  &direct->AbortStatus();
					dsaNo++;
				}
			cancelReqList[ 0 ] = &timerStatus;
			
			User::WaitForNRequest( cancelReqList, nofDSAs + 1 );
			
			iter.SetToFirst();
			while (( direct=iter++)!=NULL )
				{
				if ( direct->AbortStatus() != KRequestPending )
					direct->CancelAbortObject(); // responded
				else
					direct->Abort();
				}

			if (timerStatus == KRequestPending)
				{
				timer.Cancel();
				User::WaitForRequest( timerStatus );
				}
			
			User::Free( cancelReqList );
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

#if defined(__WINS__)
void CScreen::UpdateOffScreenBitmap()
	{
	if (iOffScreenBitmap)
		iOffScreenBitmap->Update();
	}
#endif
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
		if (oldFocus && (focusedScreen||CWsTop::MultiFocusPolicy()) && oldFocus!=aClosingWindow)
			{
			oldFocus->LostFocus();
			}
		if (newFocusedScreen)
			{
			CWsTop::SetCurrentFocusScreen(newFocusedScreen);
			newFocus->ReceivedFocus();
			}
		else if (iCurrentFocus && (focusedScreen||CWsTop::MultiFocusPolicy()))
			{
			iCurrentFocus->ReceivedFocus();
			}
		WsPointer::UpdatePointerCursor();
		TWindowServerEvent::SendFocusChangedEvents();
		}
	TWindowServerEvent::SendGroupListChangedEvents();
	}

void CScreen::SetShadowVector(const TPoint &aShadowShift)
	{
	iShadowShift=aShadowShift;
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
TInt CScreen::ColorModesFlag()
	{
	return MODE_TO_FLAG(DisplayMode());
	}

void CScreen::Update()
	{
#if defined(__WINS__) && defined(_DEBUG)
	if (iOffScreenBitmap)
		iOffScreenBitmap->Update();
#endif

	if (iRedirectGc)
		TWindowServerEvent::NotifyDrawer(TWservCrEvent(TWservCrEvent::EScreenUpdated, iScreenNumber));
	else
		iScreenDevice->Update();
	}

void CScreen::UpdateGcs()
	{
	iScreenGdi->Activate(iScreenDevice);
	}

void CScreen::ChangeDisplayModeForAllOffScreenBitmap(TBool aSwapWidthAndHeight/*=EFalse*/)
	{
	TInt err;
	if (iOffScreenBitmap)
		{
		err=iOffScreenBitmap->DisplayModeChanged(aSwapWidthAndHeight);
		if(err!=KErrNone)
			{
			delete iOffScreenBitmap;
			iOffScreenBitmap=NULL;
			}
		}
	}

CFbsScreenDevice *CScreen::ScreenDevice()
	{
	return(iScreenDevice);
	}

void CScreen::UpdateOrientation()
	{
	CFbsBitGc::TGraphicsOrientation orientation=Orientation();
	iScreenGdi->SetOrientation(orientation);
	TWservCrEvent crEvent(TWservCrEvent::EScreenOrientationChanged,iScreenNumber,&orientation);
	TWindowServerEvent::NotifyDrawer(crEvent);
	}

void CScreen::SetPointerCursorArea(TInt aMode,const TRect& aRect)
	{
	(*iModes)[aMode]->iPointerCursorArea=aRect;
	WsPointer::SetPointerCursorPos(WsPointer::PointerCursorPos());
	}

CFbsBitGc::TGraphicsOrientation CScreen::Orientation()
	{
  	WS_ASSERT_DEBUG(IsValidScreenSizeMode(iScreenSizeMode),EWsPanicInvalidScreenSizeMode);
	return (*iModes)[iScreenSizeMode]->iRotation;
	}

TRect CScreen::DrawableArea() const
	{
	TRect drawRect;
	iScreenDevice->GetDrawRect(drawRect);
	return drawRect;
	}

TClientPanic CScreen::SetModeRotation(TInt aMode,CFbsBitGc::TGraphicsOrientation aRotation)
	{
	if (!IsValidScreenSizeMode(aMode))
		return EWservPanicScreenModeNumber;
	TSizeMode& mode=*(*iModes)[aMode];
	if (!(ROTATION_TO_FLAG(aRotation)&mode.iAlternativeRotations))
		return EWservPanicRotation;
	TInt oldRotation=mode.iRotation;
	mode.iRotation=aRotation;
	CWsWindowGroup::NewOrientation(aMode,aRotation, iRootWindow);
	if (aMode==ScreenSizeMode())
		{
		UpdateOrientation();
		TBool shouldSwapWidthAndHeight=ShouldSwapWidthAndHeightOffScBitmap(oldRotation);
		ChangeDisplayModeForAllOffScreenBitmap(shouldSwapWidthAndHeight);
		iRootWindow->OrientationChanged();
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

inline TBool CScreen::ShouldSwapWidthAndHeightOffScBitmap(TInt aOldRotation)
	{
	TInt rot=Abs((*iModes)[iScreenSizeMode]->iRotation-aOldRotation);
	return (rot==1||rot==3);
	}

void CScreen::doSetScreenMode(TInt aMode)
	{
  	WS_ASSERT_DEBUG(IsValidScreenSizeMode(aMode),EWsPanicInvalidScreenSizeMode);

	TWindowServerEvent::NotifyDrawer(TWservCrEvent(TWservCrEvent::EScreenSizeModeAboutToChange, aMode));

	TInt oldRotation=(*iModes)[iScreenSizeMode]->iRotation;
	iScreenSizeMode=aMode;
	TBool shouldSwapWidthAndHeight=ShouldSwapWidthAndHeightOffScBitmap(oldRotation);
	CWsWindowGroup::SetScreenDeviceValidStates(ETrue,shouldSwapWidthAndHeight,this);
	if (shouldSwapWidthAndHeight)
		{
		SetPhysicalScreenSize();
		}
	TWindowServerEvent::SendScreenDeviceChangedEvents(this);
	ResetFocus(NULL);
	}

void CScreen::UpdateOffScreenBitmapGc(const TBool aSwapWidthAndHeight)
	{
	if (iOffScreenBitmap)
		{
		iOffScreenBitmap->UpdateGc(aSwapWidthAndHeight);
		}
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

	UpdateOrientation();
	TBool shouldSwapWidthAndHeight=ShouldSwapWidthAndHeightOffScBitmap(currentRotation);
	ChangeDisplayModeForAllOffScreenBitmap(shouldSwapWidthAndHeight);
	if (shouldSwapWidthAndHeight)
		{
		SetPhysicalScreenSize();
		}
	iRootWindow->OrientationChanged();
	}

TPoint CScreen::PhysicalToLogical(TPoint aPhysicalPt)
	{
	const TSizeMode& mode=ScreenSizeModeData();
	TPoint logicalPt;
	logicalPt=aPhysicalPt-mode.iOrigin;
	if (mode.iScreenScale.iWidth!=1)
		logicalPt.iX=(logicalPt.iX>=0 ? logicalPt.iX/mode.iScreenScale.iWidth : (logicalPt.iX-(mode.iScreenScale.iWidth-1))/mode.iScreenScale.iWidth);
	if (mode.iScreenScale.iHeight!=1)
		logicalPt.iY=(logicalPt.iY>=0 ? logicalPt.iY/mode.iScreenScale.iHeight : (logicalPt.iY-(mode.iScreenScale.iHeight-1))/mode.iScreenScale.iHeight);
	return logicalPt;
	}

void CScreen::LoadScreenSizesL(TSize aScreenSize)
	{
	_LIT(KWSERVNumScrSizeMode, "NUMSCREENMODES");
	TBool allowScrGap=WsIniFile->FindVar(iScreenNumber, KWSERVNumScrSizeMode, iNumScreenSizeModes);
	iModes=new(ELeave) RPointerArray<TSizeMode>(1);
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
		if (screenSize.iWidth==0 && screenSize.iHeight==0)
			screenSize=aScreenSize;
		TSizeMode* newSizeMode=new(ELeave) TSizeMode(screenSize);
		CleanupStack::PushL(newSizeMode);
		iModes->AppendL(newSizeMode);
		CleanupStack::Pop(newSizeMode);
		++iNumSupportedScreenSizeModes;
		}
	// If sparse index is enabled and no screen size mode defined, all iModes entries will be NULL
	// Otherwise iModes will be empty
	if (iModes->Count()==0 || iNumSupportedScreenSizeModes==0)
		{
		TSizeMode* defaultSizeMode=new(ELeave) TSizeMode(aScreenSize);
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

void CScreen::LoadScreenSizeProperties()
	{
	TBool orientations[4];
	TUint allowableRotations=0;
	TInt ii;
	iScreenGdi->OrientationsAvailable(orientations);
	for (ii=0;ii<4;++ii)
		{
		if (orientations[ii])
			allowableRotations|=ROTATION_TO_FLAG(ii);
		}
	TBuf<32> xScale;
	TBuf<32> yScale;
	_LIT(KWSERVScreenXScale,"SCR_XSCALE%d");
	_LIT(KWSERVScreenYScale,"SCR_YSCALE%d");
	for(TInt sizeLoop=0;sizeLoop<iModes->Count();sizeLoop++)
		{
		TSizeMode* modePtr=(*iModes)[sizeLoop];
		if (!modePtr)
			continue;
		TSizeMode& mode=*modePtr;
		TBuf<32> varLeft;
		TBuf<32> varTop;
		TBuf<32> varRotation;
		TBuf<32> varNameWidth;
		TBuf<32> varNameHeight;
		TBuf<32> varDisplayMode;
		_LIT(KWSERVScreenLeftPattern,"SCR_LEFT%d");
		varLeft.Format(KWSERVScreenLeftPattern,sizeLoop+1);
		_LIT(KWSERVScreenTopPattern,"SCR_TOP%d");
		varTop.Format(KWSERVScreenTopPattern,sizeLoop+1);
		_LIT(KWSERVScreenRotationPattern,"SCR_ROTATION%d");
		varRotation.Format(KWSERVScreenRotationPattern,sizeLoop+1);
		_LIT(KWSERVScreenTwipWidthPattern,"SCR_TWIP_WIDTH%d");
		varNameWidth.Format(KWSERVScreenTwipWidthPattern,sizeLoop+1);
		_LIT(KWSERVScreenTwipHeightPattern,"SCR_TWIP_HEIGHT%d");
		varNameHeight.Format(KWSERVScreenTwipHeightPattern,sizeLoop+1);
		_LIT(KWSERVScreenDisplayModePattern,"SCR_WINDOWMODE%d");
		varDisplayMode.Format(KWSERVScreenDisplayModePattern,sizeLoop+1);
		xScale.Format(KWSERVScreenXScale,sizeLoop+1);
		yScale.Format(KWSERVScreenYScale,sizeLoop+1);
		if (!WsIniFile->FindVar(iScreenNumber,xScale,mode.iScreenScale.iWidth))
			{
			mode.iScreenScale.iWidth=1;
			}
		if (!WsIniFile->FindVar(iScreenNumber,yScale,mode.iScreenScale.iHeight))
			{
			mode.iScreenScale.iHeight=1;
			}
		if (!WsIniFile->FindVar( iScreenNumber, varLeft,mode.iOrigin.iX))
			{
			mode.iOrigin.iX=0;
			}
		if (!WsIniFile->FindVar( iScreenNumber, varTop,mode.iOrigin.iY))
			{
			mode.iOrigin.iY=0;
			}
		TPtrC displayModeName(NULL,0);
		mode.iDefaultDisplayMode = iScreenDevice->DisplayMode();
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
		WS_ASSERT_ALWAYS((ROTATION_TO_FLAG(rotation)&allowableRotations)>0, EWsPanicFailedToInitialise);
		mode.iRotation=(CFbsBitGc::TGraphicsOrientation&)rotation;
		mode.iAlternativeRotations=allRotations & allowableRotations;
		
		TSize twipsSize;
		TSize pixels(mode.iScreenSize);
		
		switch(mode.iRotation)
			{
			// CFbsBitGc::TGraphicsOrientation
			case CFbsBitGc::EGraphicsOrientationRotated90:
			case CFbsBitGc::EGraphicsOrientationRotated270:
				{
				//swap the axes in order to use the correct twips per pixel ratio, as CFbsScreenDevice
				//does not take into consideration rotation, when converting pixels to twips
				if (!WsIniFile->FindVar( iScreenNumber, varNameWidth,twipsSize.iWidth))
					twipsSize.iWidth=iScreenDevice->VerticalPixelsToTwips(pixels.iWidth);
				if (!WsIniFile->FindVar( iScreenNumber, varNameHeight,twipsSize.iHeight))
					twipsSize.iHeight=iScreenDevice->HorizontalPixelsToTwips(pixels.iHeight);
				break;
				}
			case CFbsBitGc::EGraphicsOrientationNormal:
			case CFbsBitGc::EGraphicsOrientationRotated180:
				{
				if (!WsIniFile->FindVar( iScreenNumber, varNameWidth,twipsSize.iWidth))
					twipsSize.iWidth=iScreenDevice->HorizontalPixelsToTwips(pixels.iWidth);
				if (!WsIniFile->FindVar( iScreenNumber, varNameHeight,twipsSize.iHeight))
					twipsSize.iHeight=iScreenDevice->VerticalPixelsToTwips(pixels.iHeight);
				break;
				}
			default:
				WS_PANIC_ALWAYS(EWsPanicFailedToInitialise);
				break;			
			}
		mode.iScreenTwipsSize=twipsSize;
		}
//
	TInt intForFindVar=0;
	_LIT(KWSERVIniFileVarSizeMode,"SIZE_MODE");
	WsIniFile->FindVar( iScreenNumber, KWSERVIniFileVarSizeMode,intForFindVar);
	iSizeEnforcementMode=(TScreenModeEnforcement)intForFindVar;
	}

void CScreen::SetDigitiserAreas(const TSize& aScreenSize,const TRect& aDigitiserArea)
	{
	for(TInt sizeLoop=0;sizeLoop<iModes->Count();sizeLoop++)
		{
		TSizeMode* modePtr=(*iModes)[sizeLoop];
		if (!modePtr)
			continue;
		TSizeMode& mode=*modePtr;
		switch (mode.iRotation)
			{
			case CFbsBitGc::EGraphicsOrientationNormal:
				mode.iPointerCursorArea=aDigitiserArea;
				continue;
			case CFbsBitGc::EGraphicsOrientationRotated90:
				mode.iPointerCursorArea.SetRect(aDigitiserArea.iTl.iY,aScreenSize.iWidth-aDigitiserArea.iBr.iX,
																aDigitiserArea.iBr.iY,aScreenSize.iWidth-aDigitiserArea.iTl.iX);
				break;
			case CFbsBitGc::EGraphicsOrientationRotated180:
				mode.iPointerCursorArea.SetRect(-(aDigitiserArea.iBr-aScreenSize),-(aDigitiserArea.iTl-aScreenSize));
				break;
			case CFbsBitGc::EGraphicsOrientationRotated270:
				mode.iPointerCursorArea.SetRect(aScreenSize.iHeight-aDigitiserArea.iBr.iY,aDigitiserArea.iTl.iX,
																aScreenSize.iHeight-aDigitiserArea.iTl.iY,aDigitiserArea.iBr.iX);
				break;
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
		aSar.iTwipsSize.iWidth=iScreenDevice->HorizontalPixelsToTwips(aSar.iPixelSize.iWidth);
		aSar.iTwipsSize.iHeight=iScreenDevice->VerticalPixelsToTwips(aSar.iPixelSize.iHeight);
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
		CWsWindowGroup::SetScreenDeviceValidStates(EFalse,EFalse,this);
		ResetFocus(NULL);
		}
	return ETrue;
	}

CWsOffScreenBitmap* CScreen::OffScreenBitmap()
	{
	return iOffScreenBitmap;
	}

CFbsDevice * CScreen::DrawDevice()
	{
	if (iOffScreenBitmap)
		return iOffScreenBitmap->BitmapDevice();
	else
		return ScreenDevice();
	}

void CScreen::FreeOffScreenBitmap()
	{
	// for Flicker Free test
	/** Andy - this either needs to talk to render stages or simply be removed.
	Deleting the OSB when CRPs already know it exists is one thing - we dont do it while
	testing ones that can't cope - deleting it when render stages use it is something else.
	Fortunately we never actually use it.
	if (iOffScreenBitmap)
		{
		delete iOffScreenBitmap;
		iOffScreenBitmap = NULL;
		}
	*/
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

TInt CScreen::GetScreenSizeModeListL()
	{
	RArray<TInt> list(iNumScreenSizeModes);
	CleanupClosePushL(list);
	TInt index;
	for (index=0; index<iModes->Count(); ++index)
		{
		TSizeMode* modePtr=(*iModes)[index];
		if (modePtr)
			list.AppendL(index);
		}
	TInt count=list.Count();
	CWsClient::ReplyBuf(&list[0], count*sizeof(TInt));
	CleanupStack::PopAndDestroy(&list);
	return count;
	}

void CScreen::SetInitialScreenSizeMode()
	{
	// get first/lowest valid screen size mode, if mode 0 not available
	TInt index;
	for (index=0; index<iModes->Count(); ++index)
		{
		TSizeMode* modePtr=(*iModes)[index];
		if (modePtr)
			{
			iScreenSizeMode=index;
			break;
			}
		}
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

CFbsDevice* CScreen::GetFbsDevice()
	{
	if (iRedirectGc)
		{
		WS_ASSERT_DEBUG(iRedirectGc->Device(), EWsPanicNullDeviceHandle);
		return static_cast<CFbsDevice*>(iRedirectGc->Device());
		}
	else
		{
		return iScreenDevice;
		}
	}

void CScreen::AddRedrawRegion(const TRegion& aRegion, TBool aSchedule, TRedrawDepth aDepth)
	{
	iRedraw->AddRedrawRegion(aRegion, aSchedule, aDepth);
	}

void CScreen::DoRedrawNow()
	{
	iRedraw->DoRedrawNow();
	}

// implementing MWsScreen

const TTime& CScreen::Now() const
	{
	return iRedraw->Now();
	}

void CScreen::ScheduleAnimation(const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop)
	{
	iRedraw->ScheduleAnimation(aRect,aFromNow,aFreq,aStop);
	}

void CScreen::OnAnimation()
	{
	iRedraw->OnAnimation();
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
		case MWsScreenConfig::EWsObjectInterfaceId:
			interface = static_cast<MWsScreenConfig*>(this);
			break;
		case MWsFrontBuffer::EWsObjectInterfaceId:
			interface = static_cast<MWsFrontBuffer*>(this);
			break;	
		case MWsWindow::EWsObjectInterfaceId:
			interface = static_cast<MWsWindow*>(RootWindow());
			break;
		}
	
	if (!interface && iOffScreenBitmap)
		interface = iOffScreenBitmap->ResolveObjectInterface(aTypeId);

	if (!interface)
		interface = iRedraw->ResolveObjectInterface(aTypeId);

	return interface;
	}

/**
 Implementing MWsScreenConfig
*/
TDisplayMode CScreen::DisplayMode() const
	{
	return iScreenDevice->DisplayMode();
	}

TSize CScreen::SizeInPixels() const
	{
	return iScreenDevice->SizeInPixels();
	}

TSize CScreen::ScreenModeSizeInPixels() const
	{
	return (*iModes)[iScreenSizeMode]->iScreenSize;
	}

TInt CScreen::Stride() const
	{
	return iScreenDevice->Stride();
	}

CFbsBitGc::TGraphicsOrientation CScreen::Orientation() const
	{
	return iScreenDevice->Orientation();
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

/**
 Implementing MWsFrontBuffer
*/
const TAny* CScreen::GetBits()
	{
	return iScreenDevice->Bits();
	}

CFbsBitGc* CScreen::GetBitGc()
	{
	return iScreenGdi;
	}

CFbsBitGc* CScreen::GetBitGcCurrent()
	{
	if (iRedirectGc)
		return iRedirectGc;
	else
		return iScreenGdi;
	}
	
/**
 Redirect screen drawing to specified gc. Passing NULL will stop redirection.
*/
TInt CScreen::SetBitGc(CFbsBitGc* aBitGc)
	{
	if (aBitGc && (aBitGc==iScreenGdi || aBitGc==iRedirectGc))
		return KErrAlreadyExists;

	if (aBitGc && !aBitGc->Device())
		return KErrArgument;

	// screen shall not be redirected when there is at least one DSA client is actively drawing
	if (aBitGc && iDsaDrawState==EDsaDrawStateDrawing)
		return KErrInUse;

	iRedirectGc = aBitGc;
	
	// Redraw window CWindowGC objects have CFbsBitGcs active on the screen device and need reactivating:
	TWalkWindowTreeReactivateGcs wwt;
	RootWindow()->WalkWindowTree(wwt, EWalkChildren);
	
	// Andy - should we do something with the CPlaybackGc here?  Can they be active at this point?
	// if so, do they care, or should it already be handled for them?

	return KErrNone;
	}

TInt CScreen::SetBitGc(CFbsBitGc* aBitGc, TBool aInvalidateScreen)
	{
	const TInt err = SetBitGc(aBitGc);

	if (err==KErrNone && aInvalidateScreen)
		iRootWindow->InvalidateWholeScreen();

	return err;
	}

void CScreen::DiscardAllSchedules()
	{
	iRedraw->DiscardAllSchedules();
	}

void CScreen::ScheduleRegionUpdate(const TRegion* aDefinitelyDirty)
	{
	iRedraw->ScheduleRegionUpdate(aDefinitelyDirty);
	}
void	CScreen::DSARegionSyncStart( CWsDirectScreenAccess& aDSA )
	{
	iRedraw->BanThisRegionUpdate( aDSA.RegionUnderSync() );
	}

void	CScreen::DSARegionSyncOver( CWsDirectScreenAccess& aDSA  )
	{
	iRedraw->LiftRegionUpdateBan( aDSA.RegionUnderSync()  );
	}

TBool CScreen::IsUpdatePending()
	{
	return iRedraw->IsUpdatePending();
	}

TBool 	CScreen::IsDSAClientWindow( const CWsClientWindow* aWin ) const
	{
	TBool res = EFalse; 
	if ( ! iDirects.IsEmpty() )
		{
		TSglQueIter<CWsDirectScreenAccess> iter( (TSglQueBase&)iDirects );
		iter.SetToFirst();
		CWsDirectScreenAccess* dsa;
		while ( (dsa = iter++) != NULL && !res )
			{
			res =  (dsa->ClientWindow() == aWin ) && ( dsa->IsVisible() || dsa->IsSyncTimeoutPending() );
			}
		}
	return res;	
	}

void CScreen::AcceptFadeRequest( CWsWindow* aWin, TBool aIsFaded, TBool aIsBehind, TBool aIncludeChildren )
	{
	iRedraw->AcceptFadeRequest( aWin, aIsFaded, aIsBehind, aIncludeChildren );
	}
