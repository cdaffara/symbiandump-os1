// Copyright (c) 1995-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// wins\specific\gui.cpp
// 
//
#define WINVER 0x0500

#include "gui.h"
#include <emulator.h>
#include <assp.h>
#include <kernel/kern_priv.h>
#include <kernel/kpower.h>
#include "variant.h"
#include "resource.h"
#include "winsgui.h"
#include "display_chan.h"
#include "pixelformats.h"
#include "multitouch.h"

#include "monitors.h"

//Define these so that emulator generates varying values for gce stride and offset.
//By default in emulator, stride is exactly right for display resolution and offset is zero
//Setting these will identify code which incorrectly calculates these factors instead of requesting them
//Note that multiples of 4 bytes are preferred for various reasons.
//[3/5/07 The Secure presentation burffer ignores stride extra because it uses a windows bitmap header to render.]
// #define TEST_GCE_VARIABLE_STRIDE_EXTRA	16		 	//This constant is added to each mode's scanline length in bytes. It may cause a break if enabled because the iDisplayBufferOffset is not being set
// #define TEST_GCE_VARIABLE_START_EXTRA	16			//A multiple of this is added to each mode's start address in bytes
// #define ASSYMETRIC_SQUARE_STRIDE						//If this is defined and the width==height the the stride will not be the same!

enum  
	{
		KMaskModeNum=0x0FFFFFFF,
		KMaskModeFlag8=0x80000000,
		KMaskModeFlag4=0x40000000,
		KMaskModeFlag2=0x20000000,
		KMaskModeFlag1=0x10000000,
		
		KModeFlagFlipped=KMaskModeFlag8,
		
	};
enum  
	{
		KMaskScreenNum=0x0FFF,
		KMaskScreenFlag8=0x8000,
		KMaskScreenFlag4=0x4000,
		KMaskScreenFlag2=0x2000,
		KMaskScreenFlag1=0x1000,
		
		KScreenFlagSecure=KMaskScreenFlag8,
		
	};
const TInt KMaxDisplayColors=16777216;
const TInt KMaxDisplayContrast=1;

static TEmulatorFlip* CurrentFlipState=NULL;
static TInt CurrentConfiguration = 0;
static TInt SavedFlipMessage = 0;

DWinsUi *systemIni=NULL;
DMasterIni* masterIni;

DMultiTouch* TheMultiTouch;
static HWND TheControlWin;
static HWND* TheChildWin=NULL;
static HWND* TheWin=NULL;
static HWND hwndStatus; // To display the X,Y,Z information of each mouse
static TInt VirtualKeyPressed = EStdKeyNull;
static HBITMAP* TheScreenBitmap=NULL;
static TUint LedMask;
static TBool WsSwitchOnScreen;

const char * DefaultWindowTitle = "Symbian OS Emulator";

#ifdef __VC32__

#ifdef _DEBUG
const char * VersionText = " - wins udeb";
#else
const char * VersionText = " - wins urel";
#endif

#else
#ifdef __CW32__

#ifdef _DEBUG
const char * VersionText = " - winscw udeb";
#else
const char * VersionText = " - winscw urel";
#endif

#else
//not winscw or wins!
#ifdef _DEBUG
const char * VersionText = " - unknown udeb";
#else
const char * VersionText = " - unknown urel");
#endif

#endif
#endif

void Inactive();
void Active();
void DrawLeds();
void UpdateModifiers(); 
TInt DisplayHalFunction(TAny*, TInt aFunction, TAny* a1, TAny* a2);
LOCAL_C TBool PaintWindowFromBuffer(HWND hWnd);

GLDEF_C const char* skipws(const char* aPtr)
	{
	while (isspace(*aPtr))
		++aPtr;
	return aPtr;
	}

GLDEF_C const char* skiptok(const char* aPtr)
	{
	while (isalnum(*aPtr))
		++aPtr;
	return aPtr;
	}

GLDEF_C TInt CompareI(const TDesC8& aLhs, const TDesC8& aRhs)
//
// Case insensitive comparison of descriptors
// (TDesC::CompareF not available to kernel side code)
//
	{
	TInt ll = aLhs.Length();
	TInt rl = aRhs.Length();
	TInt len = Min(ll, rl);
	TInt k = _strnicmp((const char*)aLhs.Ptr(), (const char*)aRhs.Ptr(), len);
	return k != 0 ? k : ll - rl;
	}

GLDEF_C TInt MultiProperty(TInt (*aHandler)(TAny* aObj, const char*), TAny* aPtr, const char* aProperty)
	{
	const char* value = Property::GetString(aProperty);
	if (!value)
		return KErrNone;
	for (;;)
		{
		TInt r = aHandler(aPtr, value);
		if (r != KErrNone)
			return r;
		const char* ev = strchr(value, ';');
		if (!ev)
			break;
		value = ev + 1;
		}
	return KErrNone;
	}

class DWinsGuiPowerHandler : public DPowerHandler
	{
public: // from DPowerHandler
	void PowerDown(TPowerState);
	void PowerUp();
public:
	static DWinsGuiPowerHandler* New();
	void ScreenOn();
	void ScreenOff();
	void ScreenOn(TInt aScreen);
	void ScreenOff(TInt aScreen);
public:
	DWinsGuiPowerHandler();
	TBool ProcessEvent(const TRawEvent* aEvent);
	TBool ProcessEventDfc(const TRawEvent* aEvent);
	TBool	iStandby;
	};

static DWinsGuiPowerHandler* WinsGuiPowerHandler;

_LIT(KWinsGuiName, "WinsGui");

DWinsGuiPowerHandler* DWinsGuiPowerHandler::New()
	{
	DWinsGuiPowerHandler* self = new DWinsGuiPowerHandler();
	if (!self)
		return NULL;
	self->Add();

	return self;
	}

DWinsGuiPowerHandler::DWinsGuiPowerHandler() : DPowerHandler(KWinsGuiName)
	{
	}

void DWinsGuiPowerHandler::ScreenOff()
	{
	for(TInt ix=0;ix<systemIni->iScreens.Count();ix++)
		ScreenOff(ix);
	}

void DWinsGuiPowerHandler::ScreenOn()
	{
	for(TInt ix=0;ix<systemIni->iScreens.Count();ix++)
		ScreenOn(ix);
	}

void DWinsGuiPowerHandler::ScreenOff(TInt aScreen)
	{
	PostMessageA(TheWin[aScreen], WM_EMUL_POWER_ON, FALSE, NULL);
	systemIni->iScreens[aScreen]->iScreenOff = ETrue;
	}

void DWinsGuiPowerHandler::ScreenOn(TInt aScreen)
	{
	PostMessageA(TheWin[aScreen], WM_EMUL_POWER_ON, TRUE, NULL);
	systemIni->iScreens[aScreen]->iScreenOff = EFalse;
	}

void DWinsGuiPowerHandler::PowerDown(TPowerState aState)
	{
	if (aState == EPwStandby)
		iStandby = ETrue;
	ScreenOff();
	PowerDownDone();
	}


void DWinsGuiPowerHandler::PowerUp()
	{
	iStandby = EFalse;
	ScreenOn();
	PowerUpDone();
	}

// called in the interrupt context
TBool DWinsGuiPowerHandler::ProcessEvent(const TRawEvent* aEvent)
	{
	if (!iStandby)
		// Pass through 
		return EFalse;

	if ((aEvent->Type() == TRawEvent::EKeyDown))
			{
			Wins::Self() -> AssertWakeupSignal();
			}
	
	// Ignore
	return ETrue;
	}

// called in DFC
TBool DWinsGuiPowerHandler::ProcessEventDfc(const TRawEvent* aEvent)
	{
	if (aEvent->Type() == TRawEvent::EKeyDown)
		{
		Wins::Self() -> WakeupEvent();
		if (aEvent->ScanCode() == EStdKeyF5)
			{
			// Simulate a media change interrupt (media removed)
			Wins::MediaChangeCallBack();
			*Wins::MediaDoorOpenPtr()=ETrue;
			// Ignore
			return ETrue;
			}
		if (aEvent->ScanCode() == EStdKeyF8)
			{
			TRawEvent v;
			v.Set(TRawEvent::ECaseClose);
			Kern::AddEvent(v);
			// Ignore
			return ETrue;
			}
		if (aEvent->ScanCode() == EStdKeyF8)
			{
			TRawEvent v;
			v.Set(TRawEvent::ECaseClose);
			Kern::AddEvent(v);
			// Ignore
			return ETrue;
			}
		if (aEvent->ScanCode() == EStdKeyOff)
			{
			// Pass through
			return EFalse;
			}
		if (aEvent->ScanCode() == EStdKeyF10)
			{
			TRawEvent v;
			v.Set(TRawEvent::ESwitchOff);
			Kern::AddEvent(v);
			// Ignore
			return ETrue;
			}
		if (aEvent->ScanCode() == EStdKeyF11)
			{
			TRawEvent v;
			v.Set(TRawEvent::ECaseOpen);
			Kern::AddEvent(v);
			// Ignore
			return ETrue;
			}
		}
	else if (aEvent->Type() == TRawEvent::EKeyUp)
		{
		if (aEvent->ScanCode() == EStdKeyF10)
			// Ignore
			return ETrue;

		if (aEvent->ScanCode() == EStdKeyF5)
			{
			// Simulate a media change interrupt (media Present)
			*Wins::MediaDoorOpenPtr()=EFalse;
			return ETrue;
			}
		}

	// Path through
	return EFalse;
	}

class EventQ
	{
	enum {ESize = 16};
public:
	EventQ();
	void Add(const TRawEvent& aEvent);
private:
	static void Dfc(TAny* aPtr);
	void Empty();
private:
	TDfc iDfc;
	TRawEvent* iTail;
	TRawEvent iQ[ESize];
	};

EventQ::EventQ()
	:iDfc(&EventQ::Dfc, this, Kern::DfcQue0(), 6), iTail(iQ)
	{}


void EventQ::Add(const TRawEvent& aEvent)
	{
	StartOfInterrupt();	
	if (WinsGuiPowerHandler->ProcessEvent(&aEvent)) 
		{
		EndOfInterrupt();
		return;
		}

	TRawEvent* pE = iTail;
	if (pE != &iQ[ESize])
		{
		*pE = aEvent;
		iTail = pE + 1;
		if (pE == iQ)
			iDfc.Add();
		}
	EndOfInterrupt();
	}

void EventQ::Dfc(TAny* aPtr)
	{
	static_cast<EventQ*>(aPtr)->Empty();
	}

void EventQ::Empty()
//
// Called in the DFC
//
	{
	TInt irq;
	TRawEvent* pE = iQ;
	for (;;)
		{
		if (!WinsGuiPowerHandler->ProcessEventDfc(pE)) 
			Kern::AddEvent(*pE);
		++pE;
		irq = NKern::DisableAllInterrupts();
		if (pE == iTail)
			break;
		NKern::RestoreInterrupts(irq);
		}
	iTail = iQ;
	NKern::RestoreInterrupts(irq);
	}

LOCAL_D EventQ TheEventQ;

// Virtual keys


VirtualKey::VirtualKey(const TInt aCommandData, const TEmulCommand aCommand) : iCommand(aCommand), iData(aCommandData)
	{
	}

TBool VKRect::Contains(TInt aX, TInt aY) const
	{
	return (aX >= iLeft && aX < iRight && aY >= iTop && aY < iBottom);
	}

VKRect::VKRect(const TInt aCommandData, const TEmulCommand aCommand, TInt aX, TInt aY, TInt aWidth, TInt aHeight) :
	VirtualKey(aCommandData, aCommand)
	{
	iLeft = aX;
	iTop = aY;
	iRight = aX + aWidth;
	iBottom = aY + aHeight;
	}



void VKRect::Draw(HDC aHdc,COLORREF aColor) const
	{
	HPEN pen;
	pen=CreatePen(PS_SOLID, 2, aColor);
	SelectObject(aHdc, pen);
	POINT point;

	MoveToEx(aHdc, (int)iLeft, (int)iTop, &point);
	LineTo(aHdc, (int)iLeft, (int)iBottom);
	LineTo(aHdc, (int)iRight, (int)iBottom);
	LineTo(aHdc, (int)iRight, (int)iTop);
	LineTo(aHdc, (int)iLeft, (int)iTop);
	}


KeyCombination::KeyCombination(const TInt aCommandData, TEmulCommand aCommand):
	iData(aCommandData),
	iCommand(aCommand)
{
	for (TInt i=0;i<KMaxHotKeyCombinationLength;i++) 
		{
		iCombination[i]=EStdKeyNull;
		}
}

TBool KeyCombination::CheckCombinationPressed()
{
	for (TInt j=0;(j<KMaxHotKeyCombinationLength && iCombination[j]!=0);j++)
		{
		if (GetAsyncKeyState(MapVirtualKey(iCombination[j],1))>=0)//if at least one key is not pressed, we return false
			return EFalse;				
		}
	return ETrue;	
}

TBool KeyCombination::AddKey(TStdScanCode aKey)
{
	TInt i;
	for (i=0;i<KMaxHotKeyCombinationLength;i++) 
		{
		if (iCombination[i]==EStdKeyNull) 
			break;
		}
	if (KMaxHotKeyCombinationLength==i)
		return EFalse;
	else		
		iCombination[i]=aKey;	

	return ETrue;
}


DScreenProperties::DScreenProperties()
	{
	memset(this,0,sizeof(DScreenProperties));
	iColorDepth=KDefaultColorDepth;
	
	iViewport = TViewport(this);
	}


LOCAL_C TInt MaskGceOnly(TInt aModeBits)
	{	   //All HAL modes are now reported. The GCE may refuse to register the surfaces.
	return aModeBits&KEmulModes;	//previous useful settings: //(KEmulPixPerLong2|KEmulPixPerLong1);	//|KEmulPixPerLong4;
	}
	
LOCAL_C TInt BitsForSingleMode(TInt aModeColor)
	{	//only 1 bit should be set in aModeColor
	switch (aModeColor)
		{
		case KEmulGray2:	return 1;	
		case KEmulGray4:	return 2;	
		case KEmulGray16:	return 4;
		case KEmulGray256:	return 8;
		case KEmulColor16:	return 4;
		case KEmulColor256:	return 8;
		case KEmulColor4K:	return 12;
		case KEmulColor64K:	return 16;
		case KEmulColor16M:	return 24;
		default:	return 32;
		}
	
	}

DScreenProperties::~DScreenProperties() 
	{
	}

TWindowState DScreenProperties::GetWindowState()
	{
	TWindowState state;
	state.iWinPlace = iWinPlace;
	state.iFlipstate = iScreenRotation;
	state.iXoffset = iViewport.GetViewportOffsetX();
	state.iYoffset = iViewport.GetViewportOffsetY();
	return state;
	}
	
TInt DScreenProperties::SetupProperties(TInt aConf, TInt aScreen)
	{
	char property[50];

	// Calculate maximum dimensions
	TInt configurations = Property::GetInt("ConfigCount", 0);
	if (configurations == 0)
		return KErrGeneral;

	TInt count, screenWidth, screenHeight, physicalScreenWidth, physicalScreenHeight;
	for (count = 0; count < configurations; ++count)
		{
		wsprintfA(property, "Configuration[%d][%d]ScreenWidth", count, aScreen);
		screenWidth = Property::GetInt(property, KScreenWidth);
		screenWidth = (screenWidth + 3) & ~3;
		if (screenWidth > iMaxScreenWidth)
			iMaxScreenWidth = screenWidth;
		wsprintfA(property, "Configuration[%d][%d]ScreenHeight", count, aScreen);
		screenHeight = Property::GetInt(property, KScreenHeight);
		screenHeight = (screenHeight + 3) & ~3;
		if (screenHeight > iMaxScreenHeight)
			iMaxScreenHeight = screenHeight;
//
		wsprintfA(property, "Configuration[%d][%d]PhysicalScreenWidth", count, aScreen);
		physicalScreenWidth = Property::GetInt(property);
		if (physicalScreenWidth > iMaxPhysicalScreenWidth)
			iMaxPhysicalScreenWidth = physicalScreenWidth;
		wsprintfA(property, "Configuration[%d][%d]PhysicalScreenHeight", count, aScreen);
		physicalScreenHeight = Property::GetInt(property);
		if (physicalScreenHeight > iMaxPhysicalScreenHeight)
			iMaxPhysicalScreenHeight = physicalScreenHeight;
		}

	// Read figures for current configuration
	TInt givenWidth, givenHeight;
	wsprintfA(property, "Configuration[%d][%d]ScreenWidth",aConf,aScreen);
	givenWidth = Property::GetInt(property, KScreenWidth);
	iScreenWidth = (givenWidth + 3) & ~3;
	wsprintfA(property, "Configuration[%d][%d]ScreenHeight",aConf,aScreen);
	givenHeight = Property::GetInt(property, KScreenHeight);
	iScreenHeight = (givenHeight + 3) & ~3;
	// Width of screen should be multiple number of 4 pixels.
	if (givenWidth & 3 || givenHeight & 3)
		{
		Kern::Printf("Width and Height of Screen should be multiple number of 4 pixels.\n"
				"\tWidth of screen[%d] set to: %d\n\tHeight of screen[%d] set to: %d", 
				aScreen, iScreenWidth, aScreen, iScreenHeight);
		}

//
	wsprintfA(property, "Configuration[%d][%d]PhysicalScreenWidth",aConf,aScreen);
	iPhysicalScreenWidth = Property::GetInt(property);
	wsprintfA(property, "Configuration[%d][%d]PhysicalScreenHeight",aConf,aScreen);
	iPhysicalScreenHeight = Property::GetInt(property);
//
	wsprintfA(property, "Configuration[%d][%d]ScreenOffsetX",aConf,aScreen);
	iScreenOffsetX = Property::GetInt(property, KScreenOffsetX);
	wsprintfA(property, "Configuration[%d][%d]ScreenOffsetY",aConf,aScreen);
	iScreenOffsetY = Property::GetInt(property, KScreenOffsetY);
	
	wsprintfA(property, "Configuration[%d][%d]CompositionBuffers",aConf,aScreen);
	iCompositionBuffers = Property::GetInt(property, KCompositionBuffers);

	wsprintfA(property, "Configuration[%d][%d]RefreshRateHz",aConf,aScreen);
	iRefreshRateHz = Property::GetInt(property, KRefreshRateHz);


	wsprintfA(property, "Configuration[%d][%d]ColorDepth",aConf,aScreen);
	const char* colors = Property::GetString(property);
	if (colors)
		{
		TUint colorDepth=0;
		const char* end = colors;
		for (;;)
			{
			const char* beg = skipws(end);
			if(*beg==';')
				break;
			if (!*beg)
				break;
			end	= skiptok(beg);
			if (_strnicmp("Gray2",beg,end-beg) == 0)
				{
				colorDepth|=KEmulGray2|KEmulIsBitMask;
				}
			else if (_strnicmp("Gray4",beg,end-beg) == 0)
				{
				colorDepth|=KEmulGray4|KEmulIsBitMask;
				}
			else if (_strnicmp("Gray16",beg,end-beg) == 0)
				{
				colorDepth|=KEmulGray16|KEmulIsBitMask;
				}
			else if (_strnicmp("Gray256",beg,end-beg) == 0)
				{
				colorDepth|=KEmulGray256|KEmulIsBitMask;
				}
			else if (_strnicmp("Color16",beg,end-beg) == 0)
				{
				colorDepth|=KEmulColor16|KEmulIsBitMask;
				}
			else if (_strnicmp("Color256",beg,end-beg) == 0)
				{
				colorDepth|=KEmulColor256|KEmulIsBitMask;
				}
			else if (_strnicmp("Color4K",beg,end-beg) == 0)
				{
				colorDepth|=KEmulColor4K|KEmulIsBitMask;
				}
			else if (_strnicmp("Color64K",beg,end-beg) == 0)
				{
				colorDepth|=KEmulColor64K|KEmulIsBitMask;
				}
			else if (_strnicmp("Color16M",beg,end-beg) == 0)
				{
				colorDepth|=KEmulColor16M|KEmulIsBitMask;
				}
			else
				return KErrArgument;
			}
		iColorDepth = colorDepth;
		
		}
	//multiple mode support is currently only for GCE. 
	//I fill this array in before knowing if GCE will be instanced.
	if (iColorDepth&KEmulIsBitMask)
		{
		//iModeDepths is only used by GCE
		TInt colorDepth=MaskGceOnly(iColorDepth);
		TInt setMode=0;
		for (TInt i=1;i!=KEmulIsBitMask;i+=i)
			if (colorDepth&i)
				iModeDepths[setMode++]=BitsForSingleMode(i);
		iMaxModes= setMode;	
		iModeDepths[setMode++]=0;	//a bit width of 0 is illegal	
		}
	else
		{
		iModeDepths[0]=iColorDepth;
		iMaxModes=1;
		iModeDepths[1]=0;	//a bit width of 0 is illegal
		}

	wsprintfA(property, "Configuration[%d][%d]FasciaBitmap",aConf,aScreen);
	const char* fascia = Property::GetString(property);
	if (fascia)
		{
		TInt len = strlen(fascia);
		//the path may have quotes at the start and end
		//need to work out if this is an absolute or relative path
		if (fascia[0] == '\"')
			{
			++fascia;
			--len;
			if (--len > 0 && fascia[len-1] == '\"')
				--len;
			}
		char* p = iFasciaFileName;
		if (fascia[0] != '\\' && (len < 3 || fascia[1] != ':'))
			{
			//relative path
			strcpy(p, Property::GetString("EmulatorDataPath"));
			p += strlen(p);
			}
		memcpy(p, fascia, len);
		p[len] = '\0';
		}
	else
		{
		// default to machine name
		strcpy(iFasciaFileName, Property::GetString("EmulatorDataPath"));
		strcat(iFasciaFileName, Property::GetString("MachineName"));
		strcat(iFasciaFileName, ".bmp");
		}
	return KErrNone;
	}

TViewport::TViewport() 
	:iScreenProps(NULL),iViewportWidth(0), iViewportHeight(0), iViewportOffsetX(0), iViewportOffsetY(0)
	{
	}

TViewport::TViewport(DScreenProperties* aScreenProps)
	:iScreenProps(aScreenProps),iViewportWidth(0), iViewportHeight(0), iViewportOffsetX(0), iViewportOffsetY(0)
	{	
	}	
TViewport::~TViewport()
	{
	}
	

/**
Changes the logical position of the viewport within the input area
of the emulator screen. The method may adjust the position so that
the viewport stays within the input area.
@param aPosition The new Y position of the top left hand corner of the viewport.
@param aHwnd The window associated with the viewport
*/
void TViewport::ScrollToY(TInt aPosition, HWND aHwnd)
	{

	SCROLLINFO scrollinfo;
	scrollinfo.cbSize=sizeof(scrollinfo);

	//save for later
	scrollinfo.fMask=SIF_POS;
	GetScrollInfo(aHwnd, SB_VERT, &scrollinfo);
	TInt oldY=scrollinfo.nPos;

	if(aPosition<0)
	{
		scrollinfo.nPos = 0;
	}
	else if( (aPosition+GetViewportHeight())>GetMaxHeight())
	{
		scrollinfo.nPos = max(0,GetMaxHeight() - GetViewportHeight() );
	}
	else
	{
		scrollinfo.nPos=aPosition;
	}

	SetViewportOffsetY(scrollinfo.nPos);
	scrollinfo.fMask=SIF_POS;
	SetScrollInfo(aHwnd,SB_VERT, &scrollinfo, TRUE );
	ScrollWindowEx(aHwnd, 0, oldY-scrollinfo.nPos, 0, 0, NULL, NULL, SW_INVALIDATE);
	
	UpdateChildPos(aHwnd);
	}
	
/**
As for ScrollToY but for the X direction
*/
void TViewport::ScrollToX(TInt aPosition, HWND aHwnd)
	{
	SCROLLINFO scrollinfo;
	scrollinfo.cbSize=sizeof(scrollinfo);
	
	//save for later
	scrollinfo.fMask=SIF_POS;
	GetScrollInfo(aHwnd, SB_HORZ, &scrollinfo);
	TInt oldX=scrollinfo.nPos;

	if(aPosition<0)
	{
		scrollinfo.nPos = 0;
	}
	else if( (aPosition+GetViewportWidth())>GetMaxWidth())
	{
		scrollinfo.nPos = max(0,GetMaxWidth() - GetViewportWidth() );
	}
	else
	{
		scrollinfo.nPos=aPosition;
	}

	SetViewportOffsetX(scrollinfo.nPos);
	scrollinfo.fMask=SIF_POS;
	SetScrollInfo(aHwnd,SB_HORZ, &scrollinfo, TRUE );
	ScrollWindowEx(aHwnd, oldX-scrollinfo.nPos, 0, 0, 0, NULL, NULL, SW_INVALIDATE);

	UpdateChildPos(aHwnd);
	}
	
//Forward declaration
LOCAL_C TInt ScreenFromHWND(HWND aHwnd,HWND* pWin);

/**
Move the child window to it's correct position.

@param aHwnd The HWND of the parent window
*/
void TViewport::UpdateChildPos(HWND aHwnd)
	{
	TInt screenNumber = ::ScreenFromHWND(aHwnd,TheWin);
	HWND childWin = TheChildWin[screenNumber];

	switch (iScreenProps->iScreenRotation)
		{
		case EEmulatorFlipRestore:
			MoveWindow(
				childWin,
				iScreenProps->iScreenOffsetX - GetViewportOffsetX(),
				iScreenProps->iScreenOffsetY - GetViewportOffsetY(),
				iScreenProps->iScreenWidth,
				iScreenProps->iScreenHeight,
				TRUE
				);
			break;
		case EEmulatorFlipInvert:
			MoveWindow(
				childWin,
				iScreenProps->iXYInputWidth-(iScreenProps->iScreenOffsetX+iScreenProps->iScreenWidth) - GetViewportOffsetX(),
				iScreenProps->iXYInputHeight-(iScreenProps->iScreenOffsetY+iScreenProps->iScreenHeight) - GetViewportOffsetY(),
				iScreenProps->iScreenWidth,
				iScreenProps->iScreenHeight,
				TRUE
				);
			break;
		case EEmulatorFlipLeft:
			MoveWindow(
				childWin,
				iScreenProps->iScreenOffsetY - GetViewportOffsetX(),
				iScreenProps->iXYInputWidth-(iScreenProps->iScreenOffsetX+iScreenProps->iScreenWidth)- GetViewportOffsetY(),
				iScreenProps->iScreenHeight,
				iScreenProps->iScreenWidth,
				TRUE
				);
			break;
		case EEmulatorFlipRight:
			MoveWindow(
				childWin,
				iScreenProps->iXYInputHeight-(iScreenProps->iScreenOffsetY+iScreenProps->iScreenHeight) - GetViewportOffsetX(),
				iScreenProps->iScreenOffsetX - GetViewportOffsetY(),
				iScreenProps->iScreenHeight,
				iScreenProps->iScreenWidth,
				TRUE
				);
			break;
		}

	}
	
/**
Update the range of the horizontal scrollbar,
to take account of the current viewport width.

@param aHwnd The window to be updated
*/
void TViewport::UpdateScrollBarH(HWND aHwnd)
	{

	SCROLLINFO scrollinfoHor;
	scrollinfoHor.cbSize=sizeof(scrollinfoHor);
	scrollinfoHor.fMask=SIF_RANGE|SIF_PAGE;
	scrollinfoHor.nMin=0;
	scrollinfoHor.nMax= GetMaxWidth()-1;
	
	
	TInt newPage = GetViewportWidth() ;
	TBool redraw=EFalse; //redraw window if a resize has caused a scrollbar to disappear and reveal image.
	if ( newPage>= scrollinfoHor.nMax -GetSystemMetrics(SM_CXVSCROLL)
		&& newPage < scrollinfoHor.nMax+1)
		{
		redraw=ETrue;
		newPage=GetMaxWidth();

		}
	scrollinfoHor.nPage= newPage;

	SetScrollInfo(aHwnd,SB_HORZ, &scrollinfoHor, TRUE );
	if(redraw)
		{
		ScrollToX(0, aHwnd); //in case egde of fascia was against edge of vertical scrollbar
		InvalidateRect(aHwnd, NULL, TRUE);
		}
	}
	
/**
Update the range of the vertical scrollbar,
to take account of the current viewport width.

@param aHwnd The window to be updated
*/	
void TViewport::UpdateScrollBarV(HWND aHwnd)
	{
	SCROLLINFO scrollinfoVer;
	scrollinfoVer.cbSize=sizeof(scrollinfoVer);
	scrollinfoVer.fMask=SIF_RANGE|SIF_PAGE;
	scrollinfoVer.nMin=0;
	scrollinfoVer.nMax= GetMaxHeight()-1;
	
	TInt newPage = GetViewportHeight() ;
	TBool redraw=EFalse; //redraw window if a resize has caused a scrollbar to disappear and reveal image.
	if ( newPage>= scrollinfoVer.nMax -GetSystemMetrics(SM_CYHSCROLL)
		&& newPage < scrollinfoVer.nMax+1)
		{
		redraw=ETrue;
		newPage=GetMaxHeight();
		}
	scrollinfoVer.nPage= newPage;

	SetScrollInfo(aHwnd,SB_VERT, &scrollinfoVer, TRUE );
	if(redraw)
		{
		ScrollToY(0, aHwnd); //in case egde of fascia was against edge of vertical scrollbar
		InvalidateRect(aHwnd, NULL, TRUE);
		}
	}

/**
Returns the max width for the viewport window (non-client area) so that it
may be bounded. Takes account of scrollbar.

@return Max width
*/	
TInt TViewport::GetMaxWindowWidth() const
	{
	
	RECT rect = {0,0,0,0};
	
	switch(iScreenProps->iScreenRotation)
		{
		case EEmulatorFlipRestore:
		case EEmulatorFlipInvert:
			{
			rect.right=iScreenProps->iXYInputWidth;
			rect.bottom=iScreenProps->iXYInputHeight;
			break;
			}
		case EEmulatorFlipLeft:
		case EEmulatorFlipRight:
			{
			rect.right=iScreenProps->iXYInputHeight;
			rect.bottom=iScreenProps->iXYInputWidth;
			break;
			}
		}
	AdjustWindowRect(//take account of window decorations
		&rect,
		KWinStyle,
		FALSE
		);
	
	
	return (rect.right-rect.left);
	}
	
/**
Returns the max height for the viewport window (non-client area) so that it
may be bounded. Takes account of scrollbar.

@return Max height
*/
TInt TViewport::GetMaxWindowHeight() const
	{
	
	RECT rect ={0,0,0,0};

	switch(iScreenProps->iScreenRotation)
		{
		case EEmulatorFlipRestore:
		case EEmulatorFlipInvert:
			{
			rect.right=iScreenProps->iXYInputWidth;
			rect.bottom=iScreenProps->iXYInputHeight;
			break;
			}
		case EEmulatorFlipLeft:
		case EEmulatorFlipRight:
			{
			rect.right=iScreenProps->iXYInputHeight;
			rect.bottom=iScreenProps->iXYInputWidth;
			break;
			}
		}
	AdjustWindowRect(//take account of window decorations
		&rect,
		KWinStyle,
		FALSE
		);
	return (rect.bottom-rect.top);
	}

/**
Returns the maximum width for the viewport (client area only).
Allowing for the orientation of the emulator.

@return Max width
*/
TInt TViewport::GetMaxWidth() const
	{
	TInt width=0;
	switch(iScreenProps->iScreenRotation)
		{
		case EEmulatorFlipRestore:
		case EEmulatorFlipInvert:
			{
			width = iScreenProps->iXYInputWidth;
			break;
			}
		case EEmulatorFlipLeft:
		case EEmulatorFlipRight:
			{
			width = iScreenProps->iXYInputHeight;
			break;
			}
		}
	
	return width;
	}

/**
Returns the maximum height for the viewport (client area only).
Allowing for the orientation of the emulator.

@return Max height
*/
TInt TViewport::GetMaxHeight() const
	{
	TInt height=0;
	switch(iScreenProps->iScreenRotation)
		{
		case EEmulatorFlipRestore:
		case EEmulatorFlipInvert:
			{
			height = iScreenProps->iXYInputHeight;
			break;
			}
		case EEmulatorFlipLeft:
		case EEmulatorFlipRight:
			{
			height =  iScreenProps->iXYInputWidth;
			break;
			}
		}
	
	return height;
	
	}

/**
Sets the X offset of the viewport from the edge of the input area
@param aOffset The X offset
*/
void TViewport::SetViewportOffsetX(TInt aOffset)
	{
	iViewportOffsetX = aOffset;
	}

/**
Sets the Y offset of the viewport from the edge of the input area
@param aOffset The Y offset
*/
void TViewport::SetViewportOffsetY(TInt aOffset)
	{
	iViewportOffsetY = aOffset;
	}

TInt TViewport::GetViewportOffsetX() const
	{
	return iViewportOffsetX;	
	}
TInt TViewport::GetViewportOffsetY() const
	{
	return iViewportOffsetY;
	}
	
/**
Sets the viewport width, this is equal to the width
of the window's client area
@param aWidth The width
*/
void TViewport::SetViewportWidth(TInt aWidth)
	{
	iViewportWidth=aWidth;
	}

/**
Sets the viewport height, this is equal to the height
of the window's client area
@param aHeight The height
*/
void TViewport::SetViewportHeight(TInt aHeight)
	{
	iViewportHeight=aHeight;
	}

TInt TViewport::GetViewportWidth() const
	{
	return iViewportWidth;
	}
TInt TViewport::GetViewportHeight() const
	{
	return iViewportHeight;
	}

// the UI class

DWinsUi::DWinsUi()
	:iVirtualKeys(10),
	iControlHotKeys(10)
	{}

/// Returns the current mode's depth. Remember current mode is never set!
TUint DWinsUi::ColorDepth(TInt aScreenNumber)
	{
	TVideoInfoV01 info;
	VideoInfo(aScreenNumber, info);
	return info.iBitsPerPixel;
	}

TInt DWinsUi::SetFlip(TEmulatorFlip aFlip, TInt aScreenNumber)
	{
	if(TUint(aScreenNumber)>=TUint(systemIni->iScreens.Count()))
		return KErrArgument;
	int r1 = PostMessageA(TheChildWin[aScreenNumber],WM_FLIP_MESSAGE,(TUint)aFlip,NULL);
	return r1 ? KErrNone : KErrGeneral;
	}

void DWinsUi::Info(TVariantInfoV01& aInfo)
	{
	aInfo.iLedCapabilities=0x3;
	}

HWND DWinsUi::HWnd()
	{
	return TheControlWin;
	}

TInt DWinsUi::SetupProperties(TInt aId)
	
//
// load UI settings from the emulator properties
//
	{
	//setup the screens
	TInt screens = Property::GetInt("[screens]", 1);
 
 	for (TInt x = 0; x < screens; ++x)
 		{
 		DScreenProperties * pScr = new DScreenProperties();
 		if (!pScr)
 			return KErrNoMemory;
 		
 		TInt ret = pScr->SetupProperties(aId,x);
 		if (KErrNone == ret)
 			ret = iScreens.Append(pScr);
 
 		if (KErrNone != ret)
 			{
 			delete pScr;
 			return ret;
 			}
 		}
//
	char property[50];
	wsprintfA(property, "Configuration[%d]LedSize",aId);
	iLedSize = Property::GetInt(property, KLedSize);
	wsprintfA(property, "Configuration[%d]LedArrangeVertically",aId);
	iLedVertical = Property::GetBool(property, KLedVertical);
	wsprintfA(property, "Configuration[%d]LedArrangeHorizontally",aId);
	if (Property::GetBool(property))
		iLedVertical = EFalse;
	wsprintfA(property, "Configuration[%d]LedOffsetX",aId);
	iLedOffsetX = Property::GetInt(property, KLedLeft);
	wsprintfA(property, "Configuration[%d]LedOffsetY",aId);
	iLedOffsetY = Property::GetInt(property, KLedTop);
	wsprintfA(property, "Configuration[%d]LedGap",aId);
	iLedGap = Property::GetInt(property, KLedGap);
//
	wsprintfA(property, "Configuration[%d]PointerType",aId);
	const char* pointer = Property::GetString(property, "Pen");
	if (_stricmp(pointer, "None") == 0)
		{
		iPointerType=_S8("NONE");
		iXYInputType=EXYInputNone;
		}
	else if (_stricmp(pointer,"Pen") == 0)
		{
		iPointerType=_S8("PEN");
		iXYInputType=EXYInputPointer;
		}
	else if (_stricmp(pointer,"Mouse") == 0)
		{
		iPointerType=_S8("MOUSE");
		iXYInputType=EXYInputMouse;
		}
	else if (_stricmp(pointer,"Delta-Mouse") == 0)
		{
		iPointerType=_S8("MOUSE");
		iXYInputType=EXYInputDeltaMouse;
		}
	else
		return KErrArgument;
//
	wsprintfA(property, "Configuration[%d]DigitizerOffsetX",aId);
	iDigitizerOffsetX = Property::GetInt(property, -iScreens[0]->iScreenOffsetX);
	wsprintfA(property, "Configuration[%d]DigitizerOffsetY",aId);
	iDigitizerOffsetY = Property::GetInt(property, -iScreens[0]->iScreenOffsetY);
	wsprintfA(property, "Configuration[%d]DigitizerWidth",aId);
	iDigitizerWidth = Property::GetInt(property,-1);
	wsprintfA(property, "Configuration[%d]DigitizerHeight",aId);
	iDigitizerHeight = Property::GetInt(property,-1);
	wsprintfA(property, "Configuration[%d]DisableDigitizer",aId);
	iDigitizerEnabled = !Property::GetBool(property);
//	To enable the multitouch 
	wsprintfA(property, "EnableMultiTouch");
	iMultiTouchEnabled = Property::GetBool(property,EFalse);
	wsprintfA(property, "SYMBIAN_BASE_USE_GCE");
	iGCEEnabled = Property::GetBool(property,EFalse);
	wsprintfA(property, "MultiTouchProximityStep");
	iMultiTouchProximityStep = Property::GetInt(property,-1);
	wsprintfA(property, "MultiTouchPressureStep");
	iMultiTouchPressureStep = Property::GetInt(property,-1);
//
	strcpy(iSysIniFileName, Property::GetString("EmulatorDataPath"));
	strcat(iSysIniFileName, "emulator\\");
	if (!Emulator::CreateAllDirectories(iSysIniFileName))
		return Emulator::LastError();
	strcat(iSysIniFileName, Property::GetString("MachineName"));
	strcat(iSysIniFileName, ".sys.ini");
//
	TInt r = iKeyboard.Init(aId);
	if (r != KErrNone)
		return r;

	wsprintfA(property, "Configuration[%d]VirtualKey",aId);
	r = MultiProperty(&DWinsUi::DoDefineVirtualKey, this, property);
	if (r != KErrNone)
		return r;
//

	wsprintfA(property, "Configuration[%d]NoVersionInfo",aId);
	iDisplayVersionInfo = !Property::GetBool(property);
	
	wsprintfA(property, "Configuration[%d]WindowTitle",aId);
	const char * p = Property::GetString(property);
	if (p && (strlen(p) <= KMaxNameSize))
		strcpy(iWindowTitle, p);
	else
		strcpy(iWindowTitle, DefaultWindowTitle);

	if (iDisplayVersionInfo)
		{
		TInt wtLen = strlen(iWindowTitle);
		TInt vtLen = strlen(VersionText);
		if ((wtLen + vtLen) > KMaxNameSize)
			iWindowTitle[KMaxNameSize-vtLen] = '\0';
		strcat(iWindowTitle, VersionText);
		}

	wsprintfA(property, "Configuration[%d]OnActivation",aId);
	pointer = Property::GetString(property);	
	//example	OnActivation 270 EKeyScreenDimension1
	//params are rotation(int) and key(string)
	if (pointer)
		{
		char * next;
	
		//skip any white space
		const char* beg = skipws(pointer);
		
		//get the number
		long rotation = strtol(beg, &next, 0);
		if (next == beg)
			return KErrArgument;

		switch (rotation)
			{
			case 0:
				iScreens[0]->iScreenRotation = EEmulatorFlipRestore;
				break;
			case 90:
				iScreens[0]->iScreenRotation = EEmulatorFlipRight;
				break;
			case 180:
				iScreens[0]->iScreenRotation = EEmulatorFlipInvert;
				break;
			case 270:
				iScreens[0]->iScreenRotation = EEmulatorFlipLeft;
				break;
			default:
				r = KErrArgument;
			}
		if (r != KErrNone)
			return r;
		
		beg = skipws(next);
		
		//beg should now point to the keycode
		TInt key = iKeyboard.GetEPOCKeyCode(TPtrC8((const TUint8*)beg, strlen(beg)));
		if (key == KErrNotFound)
			return key;
		iInitialFlipMsg = key;
		}

	//EmulatorControl messages are a bit like virtual keys
	wsprintfA(property, "Configuration[%d]EmulatorControl",aId);
	r = MultiProperty(&DWinsUi::DoDefineEmulatorControl, this, property);
	if (r != KErrNone)
		return r;

	wsprintfA(property, "Configuration[%d]EmulatorControlHotKey",aId);
	r = MultiProperty(&DWinsUi::DoDefineEmulatorControlHotKey, this, property);
	if (r != KErrNone)
		return r;
	
	return KErrNone;
	}

TInt DWinsUi::NumberOfScreens()
	{
	return iScreens.Count();
	}

/**
Return the highest bit depth from an emulator mode mask.
@param aModeMask	A bitwise combination of KEmul... display mode mask values.
@return A color depth in bits per pixel.
*/
LOCAL_C TInt MaximumBitDepthFromMask(TInt aModeMask)
	{
	// Choose the highest bits per pixel based on the display mode mask.
	if (aModeMask & KEmulColor16M)
		{
		return 24;
		}
	if (aModeMask & KEmulColor64K)
		{
		return 16;
		}
	if (aModeMask & KEmulColor4K)
		{
		return 12;
		}

	// Lower bit depths are not supported, so use the default
	return 24;
	}


/**
Return the TDisplayRotation corresponding to the given TEmulatorFlip.
@param aFlip	A screen rotation as a TEmulatorFlip.
@return The screen rotation as a TDisplayRotation.
*/
LOCAL_C RDisplayChannel::TDisplayRotation FlipToDisplayRotation(TEmulatorFlip aFlip)
	{
	switch (aFlip)
		{
		case EEmulatorFlipLeft:
			return RDisplayChannel::ERotation90CW;
		case EEmulatorFlipInvert:
			return RDisplayChannel::ERotation180;
		case EEmulatorFlipRight:
			return RDisplayChannel::ERotation270CW;
		}
	return RDisplayChannel::ERotationNormal;
	}


TInt DWinsUi::SetDisplayChannel(TInt aScreenNumber, DDisplayChannel* aDisplay)
	{
	return systemIni->SetDisplayChannelImpl(aScreenNumber,aDisplay);
	}


TInt DWinsUi::SetDisplayChannelImpl(TInt aScreenNumber, DDisplayChannel* aDisplay)
	{
	if (TUint(aScreenNumber) >= TUint(NumberOfScreens()))
		{
		// Screen number is either negative or too big.
		return KErrArgument;
		}

	TInt r = KErrNone;
	HWND hWnd = TheChildWin[aScreenNumber];
	TBufferSet& buffer = masterIni->iBufferSet[aScreenNumber];
	
	if (aDisplay)
		{
		// Display driver connecting
		DScreenProperties* screen = iScreens[aScreenNumber];
		RDisplayChannel::TDisplayInfo info;

		TInt pixelBytes = 2;
		info.iBitsPerPixel = MaximumBitDepthFromMask(screen->iColorDepth);

		switch (info.iBitsPerPixel)
			{
			case 12:	// XRGB4444
				info.iPixelFormat = EUidPixelFormatXRGB_4444;
				break;
			case 16:	// RGB565
				info.iPixelFormat = EUidPixelFormatRGB_565;
				break;
			default:
				// Force anything else to packed RGB888
				pixelBytes = 4;
				info.iBitsPerPixel = 24;
				info.iPixelFormat = EUidPixelFormatXRGB_8888;
				break;
			}

		TInt width = screen->iMaxScreenWidth;
		TInt height = screen->iMaxScreenHeight;

		info.iRefreshRateHz = screen->iRefreshRateHz;
		info.iAvailableRotations = RDisplayChannel::ERotationNormal | RDisplayChannel::ERotation90CW |
									RDisplayChannel::ERotation180 | RDisplayChannel::ERotation270CW;
		info.iNormal.iWidth = width;
		info.iNormal.iHeight = height;
		// Windows requires rounding up to 4-bytes words
		info.iNormal.iOffsetBetweenLines = _ALIGN_UP(width * pixelBytes, 4);
		info.iFlipped.iWidth = height;
		info.iFlipped.iHeight = width;
		// Windows requires rounding up to 4-bytes words
		info.iFlipped.iOffsetBetweenLines = _ALIGN_UP(height * pixelBytes, 4);
	
		TInt maxSize=0;	 
		//ensure legacy buffer is large enough for all supported modes.
		//It would be a very strange setup for the max size to not be the max bpp,
		//but we don't know which mode is max bpp anyway!
		TVideoInfoV01 videoInfo;
		for (TInt mode=0,maxMode=screen->iMaxModes;mode<maxMode;mode++)
			{
			if (systemIni->VideoInfoForDisplayDriver(aScreenNumber,mode, videoInfo))	//can't actually fail currently
				{
				TInt dwSize=videoInfo.iOffsetToFirstPixel+videoInfo.iOffsetBetweenLines*videoInfo.iSizeInPixels.iHeight;
				if (dwSize>maxSize)
					maxSize=dwSize;
				}
			else
				{
				Fault(EGuiVideoInfoUnavailable);
				}
			//rotated mode may use more RAM?? Height may be >Width or may not be a multiple of stride quantum
			if (systemIni->VideoInfoForDisplayDriver(aScreenNumber,mode|KModeFlagFlipped, videoInfo))	//can't actually fail currently
				{
				TInt dwSize=videoInfo.iOffsetToFirstPixel+videoInfo.iOffsetBetweenLines*videoInfo.iSizeInPixels.iWidth;
				if (dwSize>maxSize)
					{
					maxSize=dwSize;
					}
				}
			else
				{
				Fault(EGuiVideoInfoUnavailable);
				}
			}

		masterIni->iMaxSizeInBytes = maxSize;
		if (__e32_atomic_add_ord32(&buffer.iDisplayDriverCount, 1) == 0)
			{
			// First driver to connect, allocate frame buffers.
			// +1 frame buffer is ui legacy buffer at [0], so does need to take account of stride and offset
			r = masterIni->AllocateFrameBuffers(aScreenNumber, screen->iCompositionBuffers + 1, maxSize);
			}

		if (r == KErrNone)
			{
			buffer.iScreenBuffer.iDisplayBufferOffset = 0;
			masterIni->iBufferSet[aScreenNumber].iDisplayChannel = aDisplay;
			masterIni->InitBitmapHeader(*screen, &buffer.iInfo);
			masterIni->InitBufferFormat(*screen, buffer.iBufferFormat);
			if(systemIni->VideoInfoForDisplayDriver(aScreenNumber,screen->iCurrentMode, videoInfo, ETrue))
				{
					r = aDisplay->Initialize(info,
											 FlipToDisplayRotation(screen->iScreenRotation),
											 hWnd, buffer.iScreenBuffer.iFrameBuffers,
											 buffer.iScreenBuffer.iMemChunks,
											 buffer.iDsaBuffer,
						                     videoInfo.iSizeInPixels,videoInfo.iSizeInTwips,
						                     masterIni->iSupportedPixelFormatTable,
						                     masterIni->iSupportedPixelFormatTableSize,
						                     buffer.iBufferFormat);
				}
			else
				{
				Fault(EGuiVideoInfoUnavailable);
				}
			}

		if (r != KErrNone && __e32_atomic_tas_ord32(&buffer.iDisplayDriverCount, 1, -1, 0) == 1)
			{
			// Release any that were allocated
			masterIni->ReleaseFrameBuffers(aScreenNumber);
			}
		}
	else
		{
		// Display driver disconnected
		if (__e32_atomic_tas_ord32(&buffer.iDisplayDriverCount, 1, -1, 0) == 1)
			{
			// All drivers disconnected, deallocate memory.
			masterIni->ReleaseFrameBuffers(aScreenNumber);
			}
		}

	return r;
	}


void DWinsUi::SetVirtualKey(const TBool aProcessing, const TInt aCommandData, const TEmulCommand aCommand)
	{
	iProcessingVirtualKey = aProcessing;
	iFakedVirtualKey = aCommandData;
	iVirtualKeyCommand = aCommand;
	}

TBool DWinsUi::WasVirtualKey(TInt& aCommandData, TEmulCommand& aCommand)
	{
	if (iProcessingVirtualKey)
		{

		aCommandData = iFakedVirtualKey;
		aCommand = iVirtualKeyCommand;
		}
	return iProcessingVirtualKey;
	}


TInt DWinsUi::DoDefineEmulatorControl(TAny* aPtr, const char* aValue)
	{
	return static_cast<DWinsUi*>(aPtr)->DefineEmulatorControl(aValue);
	}


TInt DWinsUi::DefineEmulatorControl(const char* aValue)
	{

	//example EmulatorControl SelectConfig 2 rect 223,640 29,22
	//example EmulatorControl NextConfig rect 223,640 29,22
	const char* beg = skipws(aValue);
	const char* end = skiptok(beg);
	TInt err = KErrNone;
	
	TEmulCommand command = ENoCommand;
	TInt data = 0;
	if (_strnicmp(beg, "SelectConfig", end-beg) == 0)
		{
		//get the int param which is the config to switch to
		beg = end;
		char * e;
		data = strtol(beg, &e,0);
		if (beg == e)
			err = KErrArgument;
		end = e;
		command = ESelectConfig;
		}
	else if(_strnicmp(beg, "NextConfig", end-beg) == 0)

		{
		command = ENextConfig;
		}
	else
		err = KErrArgument;

	if (err != KErrNone)
		return err;
	
	//get the shape
	beg = skipws(end);
	end = skiptok(beg);
	if (end - beg != 4 || _strnicmp(beg, "rect", 4) != 0)
		return KErrArgument;
		
	// get the parameters
	beg = skipws(end);
	char* end2;
	TInt x = strtol(beg, &end2, 10);
	if (beg == end2 || *end2++ != ',')
		return KErrArgument;
	beg = end2;
	TInt y = strtol(beg, &end2, 10);
	if (beg == end2)
		return KErrArgument;
	beg = skipws(end2);
	TInt w = strtol(beg, &end2, 10);
	if (beg == end2 || *end2++ != ',')
		return KErrArgument;
	beg = end2;
	TInt h = strtol(beg, &end2, 10);
	if (beg == end2)
		return KErrArgument;
	
	VKRect* pRect = new VKRect(data, command, x, y, w, h);
	if (!pRect)
		return KErrNoMemory;
	return iVirtualKeys.Append(pRect);

	}


TInt DWinsUi::DoDefineVirtualKey(TAny* aPtr, const char* aValue)
	{
	return static_cast<DWinsUi*>(aPtr)->DefineVirtualKey(aValue);
	}

TInt DWinsUi::DefineVirtualKey(const char* aValue)
	{
	// Get the key to map
	const char* beg = skipws(aValue);
	const char* end = skiptok(beg);
	TInt key = iKeyboard.GetEPOCKeyCode(TPtrC8((const TUint8*)beg, end-beg));
	if (key == KErrNotFound)
		return key;

	//get the shape
	beg = skipws(end);
	end = skiptok(beg);
	if (end - beg != 4 || _strnicmp(beg, "rect", 4) != 0)
		return KErrArgument;
		
	// get the parameters
	beg = skipws(end);
	char* end2;
	TInt x = strtol(beg, &end2, 10);
	if (beg == end2 || *end2++ != ',')
		return KErrArgument;
	beg = end2;
	TInt y = strtol(beg, &end2, 10);
	if (beg == end2)
		return KErrArgument;
	beg = skipws(end2);
	TInt w = strtol(beg, &end2, 10);
	if (beg == end2 || *end2++ != ',')
		return KErrArgument;
	beg = end2;
	TInt h = strtol(beg, &end2, 10);
	if (beg == end2)
		return KErrArgument;
	
	VKRect* pRect = new VKRect(key, EKey, x, y, w, h);
	if (!pRect)
		return KErrNoMemory;
	return iVirtualKeys.Append(pRect);
	}


LOCAL_C TInt readBitmapInfo(PBITMAPINFOHEADER aHeader, const char* aFileName)
	{
	PBITMAPFILEHEADER pbmfh=NULL;
	PBITMAPINFOHEADER pbmih=NULL;
	TInt bfOffBits;

	HANDLE fh=CreateFileA(aFileName,GENERIC_READ,NULL,NULL,OPEN_EXISTING,NULL,NULL);
	if (!fh || fh==INVALID_HANDLE_VALUE)
		return KErrNotFound;

	TInt r=KErrNone;

	// read in the bitmap file header.  save the offset to bits.
	pbmfh = (PBITMAPFILEHEADER)LocalAlloc(LPTR, sizeof(BITMAPFILEHEADER));
	if (pbmfh==NULL)
		{
		r=KErrNotFound;
        goto exit;
		}
	DWORD bytesRead;
	ReadFile(fh, (LPVOID)pbmfh, sizeof(BITMAPFILEHEADER), &bytesRead, NULL);
	bfOffBits=pbmfh->bfOffBits;

	// read in the bitmap info header and the color table right after it.
	pbmih = (PBITMAPINFOHEADER)LocalAlloc(LPTR, bfOffBits- sizeof(BITMAPFILEHEADER));
	if (pbmih==NULL)
		{
		r=KErrNotFound;
        goto exit;
		}
	ReadFile(fh, (LPVOID)pbmih, bfOffBits-sizeof(BITMAPFILEHEADER),&bytesRead,NULL);
	*aHeader=*pbmih;
exit:
	LocalFree(LocalHandle ((LPSTR)pbmih));
	LocalFree(LocalHandle ((LPSTR)pbmfh));
	CloseHandle(fh);
	return r;
	}

HBITMAP readBitmap(HDC aHdc, const char* aFileName)
//
// reads a BMP file from disk and returns a HBITMAP
//
	{
	HBITMAP hbm=NULL;
	PBITMAPFILEHEADER pbmfh=NULL;
	PBITMAPINFOHEADER pbmih=NULL;
	TUint8 *pBits=NULL;
	TInt bfOffBits;
	TInt nbytes;

	HANDLE fh=CreateFileA(aFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	if (!fh || fh==INVALID_HANDLE_VALUE)
		return NULL;

	nbytes=GetFileSize((HANDLE)fh, NULL);
	// read in the bitmap file header.  save the offset to bits.
	pbmfh = (PBITMAPFILEHEADER)LocalAlloc(LPTR, sizeof(BITMAPFILEHEADER));
	if (pbmfh==NULL)
        goto exit;
	DWORD bytesRead;
	ReadFile(fh, (LPVOID)pbmfh, sizeof(BITMAPFILEHEADER),&bytesRead,NULL);
	bfOffBits=pbmfh->bfOffBits;

	// read in the bitmap info header and the color table right after it.
	pbmih = (PBITMAPINFOHEADER)LocalAlloc(LPTR, bfOffBits- sizeof(BITMAPFILEHEADER));
	if (pbmih==NULL)
        goto exit;
	ReadFile(fh, (LPVOID)pbmih, bfOffBits-sizeof(BITMAPFILEHEADER),&bytesRead,NULL);

	// finally read in the bit data.
	pBits = (PBYTE)LocalAlloc (LPTR, (nbytes - bfOffBits));
	if (pBits==NULL)
        goto exit;
	ReadFile(fh, (LPVOID)pBits, nbytes-bfOffBits,&bytesRead,NULL);
		
	hbm=CreateDIBitmap(aHdc, pbmih, CBM_INIT, pBits,(PBITMAPINFO) pbmih, DIB_RGB_COLORS);
exit:
	LocalFree(LocalHandle ((LPSTR)pBits));
	LocalFree(LocalHandle ((LPSTR)pbmih));
	LocalFree(LocalHandle ((LPSTR)pbmfh));
	CloseHandle(fh);
	return hbm;
	}

void LoadFasciaBitmap(TInt aScreen)
	{
	HDC hdc=GetDC(TheWin[aScreen]);
	RECT windowRect = {0};
	windowRect.right=systemIni->iScreens[aScreen]->iXYInputWidth;
	windowRect.bottom=systemIni->iScreens[aScreen]->iXYInputHeight;
	HBITMAP screenBitmap=readBitmap(hdc, systemIni->iScreens[aScreen]->iFasciaFileName);
	if (screenBitmap==NULL)
		{
		screenBitmap=CreateCompatibleBitmap(hdc, windowRect.right-windowRect.left, windowRect.bottom-windowRect.top);
		HDC hdcMem=CreateCompatibleDC(hdc);
		SelectObject(hdcMem, screenBitmap);
		PatBlt(hdcMem, 0, 0, windowRect.right-windowRect.left, windowRect.bottom-windowRect.top, BLACKNESS);
		DeleteDC(hdcMem);
		}
	__ASSERT_ALWAYS(screenBitmap!=NULL,Fault(EGuiCreateBitmap));
	TheScreenBitmap[aScreen]=screenBitmap;

	DrawLeds();

	ReleaseDC(TheWin[aScreen], hdc);
	}
TBool DWinsUi::MultiTouchEnabled() const
	{
	return iMultiTouchEnabled;
	}

TBool DWinsUi::GCEEnabled() const
	{
	return iGCEEnabled;
	}

TInt DWinsUi::MultiTouchProximityStep() const
	{
	return iMultiTouchProximityStep;
	}

TInt DWinsUi::MultiTouchPressureStep() const
	{
	return iMultiTouchPressureStep;
	}

TBool DWinsUi::OnDigitizer(TInt aX, TInt aY) const
	{
	if (!iDigitizerEnabled)
		return EFalse;
	switch(CurrentFlipState[0])
		{
		case EEmulatorFlipRestore:
			{
			aX -= iDigitizerOffsetX;
			aY -= iDigitizerOffsetY;
			break;
			}
		case EEmulatorFlipInvert:
			{
			aX -= systemIni->iScreens[0]->iScreenWidth - iDigitizerOffsetX - iDigitizerWidth;
			aY -= systemIni->iScreens[0]->iScreenHeight - iDigitizerOffsetY - iDigitizerHeight;
			break;
			}
		case EEmulatorFlipRight:
			{
			TInt oldY = aY;
			aY = aX - (systemIni->iScreens[0]->iScreenHeight - iDigitizerOffsetY - iDigitizerHeight);
			aX = oldY - iDigitizerOffsetX;
			break;
			}
		case EEmulatorFlipLeft:
			{
			TInt oldY = aY;
			aY = aX - iDigitizerOffsetY;
			aX = oldY - (systemIni->iScreens[0]->iScreenWidth - iDigitizerOffsetX - iDigitizerWidth);
			break;
			}
		}
	return (TUint(aX) < TUint(iDigitizerWidth) && TUint(aY) < TUint(iDigitizerHeight));
	}

LOCAL_C void addMouseEvent(TRawEvent::TType aType,TInt aXpos,TInt aYpos)
//
// Add a mouse event.
//
	{
	if (systemIni->OnDigitizer(aXpos, aYpos))
		{
		TRawEvent v;
		v.Set(aType,aXpos,aYpos);
		TheEventQ.Add(v);
		}
	}

LOCAL_C void addMouseEvent(TRawEvent::TType aType,TInt aXpos,TInt aYpos,TInt aZpos, TInt aPointerId=0)
//
// Add a multitouch mouse event.
//
	{
	if (systemIni->OnDigitizer(aXpos, aYpos))
		{
		TRawEvent v;
		v.Set(aType,aXpos,aYpos, aZpos);
		v.SetPointerNumber(static_cast<const TUint8>(aPointerId));
		TheEventQ.Add(v);
		}
	}
LOCAL_C void addKeyEvent(TRawEvent::TType aType,TInt aKey)
	{
	TRawEvent v;
	v.Set(aType, aKey);
	TheEventQ.Add(v);
	}


LOCAL_C void SwitchConfiguration(TInt aData, TBool aSendFlipKey = ETrue)
	{
	if (aData < 0 || aData >= masterIni->iSystemInis.Count())
		return;

	CurrentConfiguration = aData;
	systemIni = masterIni->iSystemInis[aData];
	
	//get the correct fascia bitmaps
	TInt screens=systemIni->iScreens.Count();
	TInt i;
	TUint disabledWinType=ENormalResolution;
	for(i=0;i<screens;i++)
		{
		DeleteObject(TheScreenBitmap[i]);
		LoadFasciaBitmap(i);
		if (masterIni->iBufferSet[i].iDisplayState!=ENormalResolution)
			{
			disabledWinType=masterIni->iBufferSet[i].iDisplayState;
			}
		}
	
	//update the window title
	if (disabledWinType!=ENormalResolution && disabledWinType < 4)	//hardwired 4 because the code below is hardwired
		{	//string may be multi-part indexed by disable type, or it may not
		CHAR* firstsemi=strchr(systemIni->iWindowTitle,';');
		CHAR* secondsemi=NULL;
		if (firstsemi)
			{
			secondsemi=strchr(firstsemi+1,';');
			}
		if (firstsemi&&secondsemi)
			{
			*firstsemi='\0';
			*secondsemi='\0';
			char* ptr[4]={0,systemIni->iWindowTitle,firstsemi+1,secondsemi+1};
			SetWindowTextA(TheControlWin, ptr[disabledWinType]);
			*firstsemi=';';
			*secondsemi=';';
			}
		else
			{
			SetWindowTextA(TheControlWin, systemIni->iWindowTitle);
			}
		
		}
	else
		{
		SetWindowTextA(TheControlWin, systemIni->iWindowTitle);
		}
	//resize and repaint the current window anyway.
	//the text window server doesn't respond to orientation messages
	for(i=0;i<screens;i++)
		{
		InvalidateRect(TheWin[i], NULL, false);
		SendMessage(TheWin[i], WM_FLIP_MESSAGE, systemIni->iScreens[i]->iScreenRotation,0);
		}

	//pass on the orientation key to the windows server
	if (aSendFlipKey)
		{
		if (!WinsGuiPowerHandler->iStandby)
			{
			addKeyEvent(TRawEvent::EKeyDown, systemIni->iInitialFlipMsg);
			addKeyEvent(TRawEvent::EKeyUp, systemIni->iInitialFlipMsg);
			}
		else
			{
			//remember the flip message so we can send it to the window server when we come out of standby
			SavedFlipMessage = systemIni->iInitialFlipMsg;
			}
		}
	}
/**
Sets the specified screen to the given width and height, if available.

The configurations are searched to find a match, taking the display state into
account. If no configuration is available, the request is ignored.

@param aScreenNumber	the screen index
@param aWidth 			the desired width
@param aHeight			the desired height
**/
void DMasterIni::SetDisplaySize(TInt aDisplayNumber, TInt aWidth, TInt aHeight)
	{
	TInt displayCount = iBufferSet.Count();

	if (aDisplayNumber < 0 || aDisplayNumber >= displayCount)
		{
		// Invalid screen number, discard.
		return;
		}

	if (iBufferSet[aDisplayNumber].iDisplayState != ENormalResolution)
		{
		// No (non-zero) resolutions available, discard.
		return;
		}

	TInt count = iSystemInis.Count();
	TInt index;
	for (index = 0; index < count; index++)
		{
		DWinsUi* newIni = masterIni->iSystemInis[index];
		DScreenProperties* newProps = newIni->iScreens[aDisplayNumber];

		if (newProps->iScreenWidth == aWidth && newProps->iScreenHeight == aHeight)
			{
			// Found a potential match. Check other screens match their current size.
			if (newIni == systemIni)
				{
				// Current configuration, already in use. Nothing to do.
				break;
				}
			
			TInt display;
			for (display = 0; display < displayCount; display++)
				{
				if (display == aDisplayNumber)
					{
					// No need to check the display we are changing
					continue;
					}

				DScreenProperties* currentPropsN = systemIni->iScreens[display];
				DScreenProperties* newPropsN = newIni->iScreens[display];
				
				if (newPropsN->iScreenWidth != currentPropsN->iScreenWidth ||
						newPropsN->iScreenHeight != currentPropsN->iScreenHeight)
					{
					// Resolution mismatch, try next configuration.
					break;
					}
				}
			
			if (display == displayCount)
				{
				// Match found, switch to this configuration and stop. Force
				// rotation to the same as the current rotation.
				newProps->iScreenRotation = systemIni->iScreens[aDisplayNumber]->iScreenRotation;
				SwitchConfiguration(index);
				break;
				}
			}
		}
	}


void DMasterIni::SetBufferFormat(TInt aDisplayNumber, TUint aAggregateSize, RDisplayChannel::TPixelFormat aPixelFormat)
	{
	TInt displayCount = iBufferSet.Count();

	if (aDisplayNumber < 0 || aDisplayNumber >= displayCount)
		{
		// Invalid screen number, discard.
		return;
		}
	
	LPBITMAPV4HEADER info = &iBufferSet[aDisplayNumber].iInfo;
	
	// update the bitmap header taking in consideration the new pixel format
	switch (aPixelFormat)
		{
		case EUidPixelFormatXRGB_4444:
		case EUidPixelFormatARGB_4444:
			info->bV4BitCount=16;
			info->bV4V4Compression = BI_BITFIELDS;
			info->bV4RedMask   = 0x0F00;
			info->bV4GreenMask = 0x00F0;
			info->bV4BlueMask  = 0x000F;
			break;
		case EUidPixelFormatRGB_565:
			info->bV4BitCount=16;
			info->bV4V4Compression = BI_BITFIELDS;
			info->bV4RedMask   = 0xF800;
			info->bV4GreenMask = 0x07E0;
			info->bV4BlueMask  = 0x001F;
			break;
		case EUidPixelFormatXRGB_8888:	// Really 32bpp, but top 8 unused
		case EUidPixelFormatARGB_8888:
		case EUidPixelFormatARGB_8888_PRE:
			info->bV4BitCount=32;
			info->bV4V4Compression = BI_RGB;
			// Mask is implicit for BI_RGB compression
			break;
		default:
			// We got an error, it seems. Let's ignore the message
			return;
		}
	iBufferSet[aDisplayNumber].iBufferFormat.iPixelFormat = aPixelFormat;
	
	// taking advantage of limiting the width and size to KMaxTInt16
	TInt width = aAggregateSize & 0x0000ffff;
	TInt height = (aAggregateSize >> 16) & 0x0000ffff;

	// let's deal with the new size just received
	iBufferSet[aDisplayNumber].iBufferFormat.iSize.iWidth = width;
	iBufferSet[aDisplayNumber].iBufferFormat.iSize.iHeight = height;
	
	// update the bitmap header, taking in consideration the rotation
	switch (CurrentFlipState[aDisplayNumber])
		{
		case EEmulatorFlipRestore:
		case EEmulatorFlipInvert:
			info->bV4Width = width;
			info->bV4Height = -height;
			break;
		case EEmulatorFlipLeft:
		case EEmulatorFlipRight:
			info->bV4Width = height;
			info->bV4Height = -width;
			break;
		}
	// finally, update the image size
	SetImageSize(aDisplayNumber);
	}

void DMasterIni::SetImageSize(TInt aScreenNumber)
	{
	TInt displayCount = iBufferSet.Count();

	if (aScreenNumber >= 0 && aScreenNumber < displayCount)
		{
		LPBITMAPV4HEADER info = &iBufferSet[aScreenNumber].iInfo;
		TInt bpp = _ALIGN_UP(info->bV4BitCount, 16); //12 & 16 --> 16 ; 24 & 32 --> 32
		TInt widthInBpp = info->bV4Width * bpp;
		//rounding to 32 bits (4 octets) and converting, then, bits to octets;
		TInt scanLineInBytes = _ALIGN_UP(widthInBpp, 32) >> 3;
		// info->bV4Height is negative or zero
		info->bV4SizeImage = -info->bV4Height * scanLineInBytes;
		}
	}

LOCAL_C void NextConfiguration()
	{
	TInt config = CurrentConfiguration;
	if (++config == masterIni->iSystemInis.Count())
		config = 0;
	SwitchConfiguration(config);
	}



LOCAL_C TBool ProcessedByEmulatorKey(TInt aScanCode, HWND hWnd,TUint message,TUint wParam,TUint lParam)
	{

	TBool rVal = EFalse;
	rVal = ETrue;
	for (TInt i=0;i<systemIni->iControlHotKeys.Count();i++)//check key combinations
		{
		if (systemIni->iControlHotKeys[i]->CheckCombinationPressed()) 
			{				
			switch (systemIni->iControlHotKeys[i]->iCommand)
				{
				
				case ENextConfig:
					NextConfiguration();
					break;
		
				case ESelectConfig:
					SwitchConfiguration(systemIni->iControlHotKeys[i]->iData);
					break;
							
				}
			return ETrue;
			}
		}
	switch (aScanCode)
	{
	
	case EStdKeyF4:
		{
		// Simulate a change of media card
		TInt irq = NKern::DisableAllInterrupts();
		if (*Wins::MediaDoorOpenPtr())
			{
			*Wins::CurrentPBusDevicePtr() += 1;
			if (*Wins::CurrentPBusDevicePtr() == 2)
				{
				*Wins::CurrentPBusDevicePtr() = -1;
				}
			}
		NKern::RestoreInterrupts(irq);
		
		// pass on to the windows system so that if
		// Alt-F4 is pressed the window will close
		if (hWnd)
			DefWindowProcA(hWnd,message,wParam,lParam);
		break;
		}

	default:
		rVal = EFalse;
		break;
	}
	return rVal;
	}

LOCAL_C void MultiChildWndPointer(TUint aMessage,TInt aXpos,TInt aYpos, TInt aZ, TInt aPointerId)
//
// Handle a multi-touch pointer event in the Symbian OS screen window 
//
	{
	TRawEvent::TType eventType=TRawEvent::ENone;
	CHAR buf[50];
	
	if (aZ <= TheMultiTouch->iZMaxRange) //negative
		{
		eventType = TRawEvent::EPointer3DOutOfRange;
		wsprintf((LPTSTR)buf, (LPCTSTR)TEXT("Out Of Range"));
		SendMessage(hwndStatus, SB_SETTEXT, aPointerId , (LPARAM)(buf));					
		}
	else 
		{
		wsprintf((LPTSTR)buf, (LPCTSTR)TEXT("%d: %d,%d,%d"), aPointerId, aXpos,aYpos,aZ);
		SendMessage(hwndStatus, SB_SETTEXT, aPointerId , (LPARAM)(buf));					
		switch (aMessage)
	    	{
			case WM_MOUSEMOVE:
				{
				eventType=TRawEvent::EPointerMove;
				break;
				}
			case WM_LBUTTONDOWN:
				{
				SetCapture(TheChildWin[0]);
				eventType = TRawEvent::EButton1Down;
				}
				break;
			case WM_LBUTTONUP:
				{
				ReleaseCapture();
				eventType = TRawEvent::EButton1Up;
				break;
				}
			case WM_RBUTTONDOWN:
				{
				eventType = TRawEvent::EButton3Down;
				break;
				}
			case WM_RBUTTONUP:
				{
				eventType = TRawEvent::EButton3Up;
				break;
				}
			case WM_MOUSEWHEEL:
				{
				eventType = TRawEvent::EPointerMove;
				break;
				}
			default:
				return;
			}
		}

	if (!WinsGuiPowerHandler->iStandby)
		{
		addMouseEvent(eventType, aXpos, aYpos, aZ, aPointerId);
		}
	}

LOCAL_C void ChildWndPointer(TUint message,TInt aXpos,TInt aYpos)
//
// Handle a pointer event in the Symbian OS screen window
//
	{
	// Enable the multitouch if the cursor is inside the main client window
	if (DMultiTouch::iMultiTouchCreated)
		{
		RECT client;
		WINDOWINFO info;
		GetWindowInfo(TheChildWin[0], &info);
		POINT pt = {aXpos+(TInt)info.rcClient.left, aYpos+(TInt)info.rcClient.top};
		if (GetWindowRect(TheChildWin[0], &client) &&
				(PtInRect(&client,pt)!=NULL) && !DMultiTouch::iMultiTouchTempEnabled)	// within the window
			{
			if (systemIni->MultiTouchEnabled() && systemIni->GCEEnabled())
				{
				if(TheMultiTouch->Register())	// Register successfully
					{
					DMultiTouch::iMultiTouchTempEnabled = TRUE;
					//Show the status bars at the bottom of the emulator
					SetWindowPos(hwndStatus,0,0,0,0,0,SWP_SHOWWINDOW);
					SetFocus(TheWin[0]);
					SetCursor(LoadCursorA(NULL,MAKEINTRESOURCEA(32512)));
					}
				}
			}
		}
	TRawEvent::TType eventType=TRawEvent::ENone;
	switch (message)
    	{
	case WM_MOUSEMOVE:
		eventType=TRawEvent::EPointerMove;
		break;
	case WM_LBUTTONDOWN:
		{
		SetCapture(TheChildWin[0]);
		eventType=TRawEvent::EButton1Down;
		}
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		eventType=TRawEvent::EButton1Up;
		break;
	case WM_RBUTTONDOWN:
		eventType=TRawEvent::EButton3Down;
		break;
	case WM_RBUTTONUP:
		eventType=TRawEvent::EButton3Up;
		break;
	case WM_MBUTTONDOWN:
		eventType=TRawEvent::EButton2Down;
		break;
	case WM_MBUTTONUP:
		eventType=TRawEvent::EButton2Up;
		break;
		}
	if (!WinsGuiPowerHandler->iStandby)
		{
		addMouseEvent(eventType, aXpos, aYpos);
		}
	}

LOCAL_C void FrameWndPointer(TUint message,TInt aXpos,TInt aYpos, TInt aScreenNumber, TInt aPointerId = 0)
//
// Handle a frame wnd pointer event.
//
	{
	static bool	processingScreenOn=FALSE;
	TEmulCommand command = ENoCommand;
	TInt commandData = 0;
	TBool mouseEvent = ETrue;
	
	TRawEvent::TType eventType=TRawEvent::ENone;
	
	TViewport& viewport = systemIni->iScreens[aScreenNumber]->iViewport;
	aXpos += viewport.GetViewportOffsetX(); // make mouse-coords relative to fascia edge even if window is scrolled
	aYpos += viewport.GetViewportOffsetY();
	
	switch (message)
    	{
	case WM_MOUSEMOVE:
		{
		TInt newX, newY;
		systemIni->TranslateMouseCoords(CurrentFlipState[0], aXpos, aYpos, systemIni->iScreens[0]->iXYInputWidth, systemIni->iScreens[0]->iXYInputHeight, newX, newY);

		if (aPointerId == 0)
			{ // only system pointer changes shape
		if (systemIni->GetVirtualKey(command, newX, newY) >= 0)
			{
			HMODULE hmodule = GetModuleHandleA("winsgui.dll");
			SetCursor(LoadCursorA((HINSTANCE)hmodule,MAKEINTRESOURCEA(OVERKEY)));		//hand cursor
			}
		else
			SetCursor(LoadCursorA(NULL,MAKEINTRESOURCEA(32512))); //ICD_ARROW
				}

		eventType=TRawEvent::EPointerMove;
		
		}
		break;
	case WM_LBUTTONDOWN:
		{
		SetCapture(TheWin[0]);
		//check the configuration
		TInt newX, newY;
		
		//if the emulator screen is rotated, rotate/flip the current mouse cursor position
		//so it can be checked to see if it is in a key region.
		systemIni->TranslateMouseCoords(CurrentFlipState[0], aXpos, aYpos, systemIni->iScreens[0]->iXYInputWidth, systemIni->iScreens[0]->iXYInputHeight, newX, newY);
		commandData = systemIni->GetVirtualKey(command, newX, newY);
		
		if (commandData >= 0)
			{
			eventType=TRawEvent::EKeyDown;
			mouseEvent = EFalse;
			systemIni->SetVirtualKey(ETrue, commandData, command);
			}
		else
			eventType=TRawEvent::EButton1Down;
		}
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		if (processingScreenOn)
			{
			// ignore button up - button down was switching things on
			processingScreenOn=FALSE;
			return;
			}
		if (systemIni->WasVirtualKey(commandData, command))
			{
			eventType=TRawEvent::EKeyUp;
			mouseEvent = EFalse;
			systemIni->SetVirtualKey(EFalse, EStdKeyNull, ENoCommand);
			}
		else
			eventType=TRawEvent::EButton1Up;
		break;
	case WM_RBUTTONDOWN:
		eventType=TRawEvent::EButton3Down;
		break;
	case WM_RBUTTONUP:
		eventType=TRawEvent::EButton3Up;
		break;
	case WM_MBUTTONDOWN:
		eventType=TRawEvent::EButton2Down;
		break;
	case WM_MBUTTONUP:
		eventType=TRawEvent::EButton2Up;
		break;
		}
	if (mouseEvent)
		{
		
		if (!WinsGuiPowerHandler->iStandby)
			{
			/*
			mouse events are relative to the child window position
			and are clipped to the digitzer region in addMouseEvent
			so all the mouse clicks are passed on here after being translated 
			to the child window coordinate system (under the current rotation)
			*/
			TInt newX, newY;
			switch (CurrentFlipState[0])
				{
				case EEmulatorFlipRestore:
				default:
					newX = aXpos - systemIni->iScreens[0]->iScreenOffsetX;
					newY = aYpos - systemIni->iScreens[0]->iScreenOffsetY;
					break;
				case EEmulatorFlipInvert:
					newX = aXpos - (systemIni->iScreens[0]->iXYInputWidth - systemIni->iScreens[0]->iScreenWidth - systemIni->iScreens[0]->iScreenOffsetX);
					newY = aYpos - (systemIni->iScreens[0]->iXYInputHeight - systemIni->iScreens[0]->iScreenHeight - systemIni->iScreens[0]->iScreenOffsetY);
					break;
				case EEmulatorFlipLeft:
					newX = aXpos - systemIni->iScreens[0]->iScreenOffsetY;
					newY = aYpos - (systemIni->iScreens[0]->iXYInputWidth - systemIni->iScreens[0]->iScreenWidth - systemIni->iScreens[0]->iScreenOffsetX);
					break;	
				case EEmulatorFlipRight:
					newX = aXpos - (systemIni->iScreens[0]->iXYInputHeight - systemIni->iScreens[0]->iScreenHeight - systemIni->iScreens[0]->iScreenOffsetY);
					newY = aYpos - systemIni->iScreens[0]->iScreenOffsetX;
					break;
				}
			addMouseEvent(eventType, newX, newY);
			}
		}
	else if ((((message == WM_LBUTTONDOWN && command == EKey) && !ProcessedByEmulatorKey((TUint8)commandData,0,0,0,0)))
			|| (message == WM_LBUTTONUP))
		{
			switch (command)
			{
			case EKey:
				if (!WinsGuiPowerHandler->iStandby)
					addKeyEvent(eventType, (TUint8)commandData);
				break;

			case ENextConfig:
				NextConfiguration();
				break;
			
			case ESelectConfig:
				SwitchConfiguration(commandData);
				break;
			}
		}
	}

LOCAL_C TInt ScreenFromHWND(HWND aHwnd,HWND* pWin)
	{
	TInt screens=systemIni->iScreens.Count();
	TInt r=-1;
	for(TInt i=0;i<screens;i++)
		{
		if(pWin[i]==aHwnd)
			{
			r=i;
			break;
			}
		}
	return r;
	}
void MultiTouchWndPointer(TUint message,TInt aXpos,TInt aYpos, TInt aZ, TInt aPointerId)
	{
	WINDOWINFO info;
	info.cbSize = sizeof(WINDOWINFO);
	if (GetWindowInfo(TheWin[0], &info))
		{
		POINT pt = {aXpos,aYpos};
		if (PtInRect(&info.rcWindow,pt))
			{
			RECT client;
			if (GetWindowRect(TheChildWin[0], &client) && PtInRect(&client,pt))	// within the window
				{
				MultiChildWndPointer(message, aXpos-client.left, aYpos-client.top, aZ, aPointerId);
				}
			else  
				{		
				//	Disable the multitouch if the cursor is outside the application window
				if (DMultiTouch::iMultiTouchTempEnabled)	// within the window
					{
					DMultiTouch::iMultiTouchTempEnabled = FALSE;
					if(TheMultiTouch->UnRegister())
						{
						SetWindowPos(hwndStatus,0,0,0,0,0,SWP_HIDEWINDOW);
						}
					}
				FrameWndPointer(message, aXpos-info.rcClient.left, aYpos-info.rcClient.top, 0, aPointerId);	
				}
			}
		}
	}

LOCAL_C DScreenProperties* ScreenPropsFromHWND(HWND aHwnd, HWND* pWin)
	{
	TInt screenNumber =  ScreenFromHWND(aHwnd, pWin);
	
	if(screenNumber >=0)
		{
		return systemIni->iScreens[screenNumber];
		}
	return NULL;

	}


TInt APIENTRY childWinProc(HWND hWnd,TUint message,TUint wParam,TUint lParam)
//
// The child window function.
//
	{
	TInt screenNumber = 0;
	TRawEvent v;
    switch (message)
    	{
	case WM_FLIP_MESSAGE: // pass the flip message onto the parent window
		{
		screenNumber =ScreenFromHWND(hWnd,TheChildWin);
		if(TUint(screenNumber) < TUint(systemIni->iScreens.Count()))
			PostMessageA(TheWin[screenNumber],WM_FLIP_MESSAGE,wParam,NULL);
		break;
		}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		{
		if (DMultiTouch::iMultiTouchTempEnabled)
			{
			DMultiTouch::iMultiTouchTempEnabled = FALSE;
			}
		screenNumber=ScreenFromHWND(hWnd,TheChildWin);
		if(screenNumber==0)
			{
			ChildWndPointer(message,(TInt16)(lParam&0xFFFF),(TInt16)((lParam>>16)&0xFFFF));
			}
		break;
		}
    case WM_PAINT:
		if (!PaintWindowFromBuffer(hWnd))
			{
			// Original behaviour
			ValidateRect(hWnd,NULL);
			v.Set(TRawEvent::ERedraw);
			TheEventQ.Add(v);
			}
        break;
    case WM_ACTIVATE:
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYUP:
		Fault(EGuiChildWinProc);
		break;
    case WM_DESTROY:
		break;

	case WM_CHAR:
	case WM_SYSCHAR:
	case WM_DEADCHAR:
	case WM_SYSDEADCHAR:
        break;

	case WMU_SET_DISPLAY_BUFFER:
		screenNumber = ScreenFromHWND(hWnd, TheChildWin);
		if (TUint(screenNumber) < TUint(systemIni->iScreens.Count()))
			{
			masterIni->iBufferSet[screenNumber].iDisplayBuffer = (LPVOID)lParam;
			}
		break;
	case WMU_SET_DISPLAY_SIZE:
		screenNumber = ScreenFromHWND(hWnd, TheChildWin);
		masterIni->SetDisplaySize(screenNumber, wParam, lParam);
		break;
		
	case WMU_SET_BUFFER_FORMAT:
		screenNumber = ScreenFromHWND(hWnd, TheChildWin);
		masterIni->SetBufferFormat(screenNumber, wParam, (RDisplayChannel::TPixelFormat) lParam);
		break;
		
 	default:
        return(DefWindowProcA(hWnd,message,wParam,lParam));
	    }
    return(FALSE);
	}


LOCAL_C TBool PaintWindowFromBuffer(HWND hWnd)
	{
	TInt screenNumber = ScreenFromHWND(hWnd,TheChildWin);
	if (TUint(screenNumber) >= TUint(masterIni->iBufferSet.Count()))
		{
		return EFalse;
		}

	LPVOID displayBuffer = masterIni->iBufferSet[screenNumber].iDisplayBuffer;
	if (!displayBuffer)
		{
		return EFalse;
		}

	TInt   frameOffset = masterIni->iBufferSet[screenNumber].iScreenBuffer.iDisplayBufferOffset;
	displayBuffer=LPVOID(frameOffset+(char*)displayBuffer);

	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);

	// Paint directly from the buffer to the window
	LPBITMAPINFO info = (LPBITMAPINFO)&masterIni->iBufferSet[screenNumber].iInfo;
	WORD width = (WORD)info->bmiHeader.biWidth;
	WORD height = (WORD)-info->bmiHeader.biHeight;	// stored -ve in info
	SetDIBitsToDevice(ps.hdc,
						0, 0, 	// Dst X, Y
						width, height,	// Src W, H
						0, 0,	// Src X, Y
						0,		// Src offset to first line
						height,	// Src lines available
						displayBuffer,	// Src pointer to pixels
						info,			// Src info
						DIB_RGB_COLORS);

	EndPaint(hWnd, &ps);

	return TRUE;
	}


LOCAL_C void CalcTextPos(TInt aScreen, TInt& aX, TInt& aY)
	{
	switch (CurrentFlipState[aScreen])
		{
	case EEmulatorFlipInvert:
		aX = systemIni->iScreens[aScreen]->iXYInputWidth-(systemIni->iScreens[aScreen]->iScreenOffsetX+systemIni->iScreens[aScreen]->iScreenWidth);
		aY = systemIni->iScreens[aScreen]->iXYInputHeight-(systemIni->iScreens[aScreen]->iScreenOffsetY+systemIni->iScreens[aScreen]->iScreenHeight);
		break;
	case EEmulatorFlipLeft:
		aX = systemIni->iScreens[aScreen]->iScreenOffsetY;
		aY = systemIni->iScreens[aScreen]->iXYInputWidth-(systemIni->iScreens[aScreen]->iScreenOffsetX+systemIni->iScreens[aScreen]->iScreenWidth);
		break;
	case EEmulatorFlipRight:
		aX = systemIni->iScreens[aScreen]->iXYInputHeight-(systemIni->iScreens[aScreen]->iScreenOffsetY+systemIni->iScreens[aScreen]->iScreenHeight);
		aY = systemIni->iScreens[aScreen]->iScreenOffsetX;
		break;
	case EEmulatorFlipRestore:
	default:
		aX = systemIni->iScreens[aScreen]->iScreenOffsetX;
		aY = systemIni->iScreens[aScreen]->iScreenOffsetY;
	break;
		}
	//subtract viewport offset here
	aX -= systemIni->iScreens[aScreen]->iViewport.GetViewportOffsetX();
	aY -= systemIni->iScreens[aScreen]->iViewport.GetViewportOffsetY();
	}

TInt APIENTRY ctrlwinProc(HWND hWnd,TUint message,TUint wParam,TUint lParam)
//
// The control window function
//
	{
	switch(message)
	{
		case WM_SYSCOMMAND:
			{
				switch(wParam)
				{
					case 1:
						{
						NextConfiguration();
						return 0;
						}
					case SC_MINIMIZE:
					case SC_RESTORE:
						{
						if (wParam == SC_RESTORE) 
							Active();
						for(TInt ix=0;ix<systemIni->iScreens.Count();ix++)
							{
							SendMessage(TheWin[ix],message,wParam,lParam);
							}
						if (wParam == SC_MINIMIZE) 
							Inactive();
						}			
				}
				return(DefWindowProcA(hWnd,message,wParam,lParam));
			}
		case WM_CLOSE: // tell all emulator screen windows to close
			{
			for(TInt i=0;i<systemIni->iScreens.Count();i++)
				{
				DestroyWindow(TheWin[i]);
				}
			DestroyWindow(hWnd);
			break;
			}
		case WM_DESTROY:
			{
			// save the main window position information
			HANDLE hSysIni;
			hSysIni = CreateFileA(systemIni->iSysIniFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
			DScreenProperties* screenProps;
			if (hSysIni != INVALID_HANDLE_VALUE)
				{
				DWORD numWritten;
				//write an identifier into file so that program can avoid loading old version
				WriteFile(hSysIni, &KDatFileVersion, sizeof(TInt), &numWritten, 0);
				
				//record current configuration at start of file.
				WriteFile(hSysIni, &CurrentConfiguration, sizeof(TInt), &numWritten, 0);
				
				//Write out the state for each window.
				for(TInt i=0;i<systemIni->iScreens.Count();i++)
					{
					screenProps= systemIni->iScreens[i];
					
					TWindowState winState= screenProps->GetWindowState();
					WriteFile(hSysIni, &winState, sizeof(TWindowState), &numWritten, 0);
					
					
					}
				}
			CloseHandle(hSysIni);

			PostQuitMessage(KErrNone);
			break;
			}
		case WM_INPUT:
			{
			if (!DMultiTouch::iMultiTouchTempEnabled)
				{
				for(TInt ix=0;ix<systemIni->iScreens.Count();ix++)
					{
					DMultiTouch::iMultiTouchTempEnabled = TRUE;

					SendMessage(TheWin[ix],message,wParam,lParam);
					}
				}
			else if (systemIni->MultiTouchEnabled() && DMultiTouch::iMultiTouchSupported && systemIni->GCEEnabled())
				{
			   	TheMultiTouch->OnWmInput(hWnd, message, wParam, lParam,TheChildWin[0]);	
				}
			else
				{
				Fault(EGuiInvalidMultiTouch);
				}
		   	break;
			}	
		default:
			return(DefWindowProcA(hWnd,message,wParam,lParam));
	}
	return(FALSE);
	}

TInt APIENTRY winProc(HWND hWnd,TUint message,TUint wParam,TUint lParam)
//
// The border window function.
//
	{

	TRawEvent v;
	
	switch (message)
    	{
		case WM_GETMINMAXINFO:
			{
			DScreenProperties* screenProps = ScreenPropsFromHWND(hWnd, TheWin);
			if(screenProps == NULL)
				{
				return DefWindowProcA(hWnd, message, wParam, lParam);
				}
						
			MINMAXINFO* minMaxInfo = reinterpret_cast<MINMAXINFO*>(lParam);
			minMaxInfo->ptMaxTrackSize.x = screenProps->iViewport.GetMaxWindowWidth();
			minMaxInfo->ptMaxTrackSize.y = screenProps->iViewport.GetMaxWindowHeight();

			minMaxInfo->ptMaxSize.x = minMaxInfo->ptMaxTrackSize.x;
			minMaxInfo->ptMaxSize.y = minMaxInfo->ptMaxTrackSize.y;
			DefWindowProcA(hWnd, message, wParam, lParam);
			
			break;
			}
	   
    	    		
 	    	
    	case WM_SIZE:
    		{
    		DScreenProperties* screenProps = ScreenPropsFromHWND(hWnd, TheWin);
			if(screenProps == NULL)
				{
				return DefWindowProcA(hWnd, message, wParam, lParam);
				}
    		TViewport& viewport = screenProps->iViewport;
    		//update size of viewport
    		viewport.SetViewportWidth(LOWORD(lParam));
    		viewport.SetViewportHeight(HIWORD(lParam));
    		
    		
			//If resize goes beyond boundary of emulator then scroll to compensate
			TInt ox = viewport.GetViewportOffsetX();
    		TInt xs = ox + LOWORD(lParam) - viewport.GetMaxWidth();
    		if (xs>0) 
    			{    			
    			viewport.ScrollToX(ox-xs, hWnd);
    			}
    		
    		TInt oy = viewport.GetViewportOffsetY();
    		TInt ys = oy + HIWORD(lParam) - viewport.GetMaxHeight();
    		if (ys>0) 
    			{    			
    			viewport.ScrollToY(oy-ys, hWnd);
    			}
    	
    		//Adjust ranges of scroll bars
    	   	viewport.UpdateScrollBarH(hWnd);
    		viewport.UpdateScrollBarV(hWnd);
    		    		
    		
    		
    		break;    		
    		}
    	case WM_HSCROLL:
    		{
    		DScreenProperties* screenProps = ScreenPropsFromHWND(hWnd, TheWin);
			if(screenProps == NULL)
				{
				return DefWindowProcA(hWnd, message, wParam, lParam);
				}
    		TViewport& viewport = screenProps->iViewport;
    		
    		switch (LOWORD(wParam)) 
    			{
    			case SB_THUMBTRACK:
    				{
    				viewport.ScrollToX(HIWORD(wParam),hWnd);
    				break;
    				}
				case SB_PAGELEFT:
					{
					viewport.ScrollToX(viewport.GetViewportOffsetX() - viewport.GetViewportWidth(), hWnd );
					break;
					}
				case SB_PAGERIGHT:
					{
					viewport.ScrollToX(viewport.GetViewportOffsetX() + viewport.GetViewportWidth() , hWnd);
					break;
					}
				case SB_LINEUP:
					{
					viewport.ScrollToX(viewport.GetViewportOffsetX() - 1, hWnd);
					break;
					}
				case SB_LINEDOWN:
					{
					viewport.ScrollToX(viewport.GetViewportOffsetX() + 1, hWnd);
					break;
					}
    			
    			}
    		   		
   
    		break;
    		}
    	
    	case WM_VSCROLL:
    		{
    		DScreenProperties* screenProps = ScreenPropsFromHWND(hWnd, TheWin);
			if(screenProps == NULL)
				{
				return DefWindowProcA(hWnd, message, wParam, lParam);
				}
    		TViewport& viewport = screenProps->iViewport;
    		
			
			switch (LOWORD(wParam)) 
    			{
    			case SB_THUMBTRACK:
    				{
    				viewport.ScrollToY(HIWORD(wParam), hWnd);
    				break;
    				}
				case SB_PAGELEFT:
					{
					viewport.ScrollToY(viewport.GetViewportOffsetY() - viewport.GetViewportHeight() , hWnd);
					break;
					}
				case SB_PAGERIGHT:
					{
					viewport.ScrollToY(viewport.GetViewportOffsetY() + viewport.GetViewportHeight(), hWnd );
					break;
					}
				case SB_LINEUP:
					{
					viewport.ScrollToY(viewport.GetViewportOffsetY() - 1, hWnd);
					break;
					}
				case SB_LINEDOWN:
					{
					viewport.ScrollToY(viewport.GetViewportOffsetY() + 1, hWnd);
					break;
					}
    			
    			}
    		
    		break;
    		
    		}
    	
    	 		
		case WM_FLIP_MESSAGE:
			{
			DScreenProperties* screenProps = ScreenPropsFromHWND(hWnd, TheWin);
			if(screenProps == NULL)
				{
				return DefWindowProcA(hWnd, message, wParam, lParam);

				}
			
			TViewport& viewport = screenProps->iViewport;
			RECT windowRect={0,0,0,0};
			GetClientRect(hWnd, &windowRect);
			
			TInt screenNumber=ScreenFromHWND(hWnd,TheWin);
			if(TUint(screenNumber) >= TUint(systemIni->iScreens.Count()))
				break;
			PBITMAPV4HEADER info = &masterIni->iBufferSet[screenNumber].iInfo;
			CurrentFlipState[screenNumber]=(TEmulatorFlip)wParam;
			TBufferSet* bufferSet = &masterIni->iBufferSet[screenNumber];
			switch (CurrentFlipState[screenNumber])
				{
				case EEmulatorFlipRestore:
				case EEmulatorFlipInvert:
					windowRect.right=systemIni->iScreens[screenNumber]->iXYInputWidth;
					windowRect.bottom=systemIni->iScreens[screenNumber]->iXYInputHeight;
					info->bV4Width = bufferSet->iBufferFormat.iSize.iWidth;
					info->bV4Height = -bufferSet->iBufferFormat.iSize.iHeight;
					break;
				case EEmulatorFlipLeft:
				case EEmulatorFlipRight:
					windowRect.right=systemIni->iScreens[screenNumber]->iXYInputHeight;
					windowRect.bottom=systemIni->iScreens[screenNumber]->iXYInputWidth;
					info->bV4Width = bufferSet->iBufferFormat.iSize.iHeight;
					info->bV4Height = -bufferSet->iBufferFormat.iSize.iWidth;
					break;
				}
			AdjustWindowRect(&windowRect,KWinStyle,FALSE);
    		
    		
			viewport.ScrollToX(0, hWnd);
			viewport.ScrollToY(0, hWnd);

			
			screenProps->iScreenRotation = (TEmulatorFlip)wParam; 
			
						
			RECT currentWindowRect;
			GetWindowRect(hWnd,&currentWindowRect);
			InvalidateRect(hWnd,NULL,FALSE);
			MoveWindow(
				TheWin[screenNumber],
				max(currentWindowRect.left,0), // so the window doesn't disappear off the screen
				max(currentWindowRect.top,0),
				windowRect.right-windowRect.left,
				windowRect.bottom-windowRect.top,
				TRUE
				);
			// move the child window
			screenProps->iViewport.UpdateChildPos(hWnd);
			
			viewport.UpdateScrollBarH(hWnd);
    		viewport.UpdateScrollBarV(hWnd);

			InvalidateRect(hWnd,NULL,TRUE);
			UpdateWindow(hWnd);
			
			break;
			}
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if (!(HIWORD(lParam)&KF_REPEAT))
				{
				
				
				TUint scanCode=DWinsKeyboard::ScanCodeToStandardKey(HIWORD(lParam));
				TUint newScanCode = systemIni->iKeyboard.ScanCodeToRemappedKey(HIWORD(lParam));
				MSG msg={hWnd,message,wParam,lParam,GetMessageTime(),GetMessagePos()};
				TranslateMessage(&msg);
				TUint charCode=0;
				// look in the message queue to get character associated with keypress
				// so long as the control, shift and alt keys aren't depressed
				if (!(HIBYTE(GetKeyState(VK_CONTROL)) && HIBYTE(GetKeyState(VK_MENU)) && HIBYTE(GetKeyState(VK_SHIFT))))
					if (PeekMessageA(&msg,hWnd,WM_CHAR,WM_CHAR,PM_NOREMOVE) &&
						scanCode == newScanCode) //no remapping
						charCode=msg.wParam;
				// Pass the character as the HIWORD of the Epoc scan code
				
				scanCode = newScanCode;
				v.Set(TRawEvent::EKeyDown,(charCode<<16)|scanCode);
				if (!ProcessedByEmulatorKey(scanCode,hWnd,message,wParam,lParam))
   					TheEventQ.Add(v);
				
				}
			break;
		case WM_TIMER:
			break;
		case WM_EMUL_POWER_ON:
			{
			TInt screenNumber=ScreenFromHWND(hWnd,TheWin);
			if(TUint(screenNumber) >= TUint(systemIni->iScreens.Count()))
				break;
//			HWND win = systemIni->iSecureDisplay ? TheSecureChildWin : TheChildWin;
			HWND win = TheChildWin[screenNumber];
			if (wParam==TRUE)
				{
				ShowWindow(win, SW_HIDE);
				ShowWindow(win, SW_SHOWNORMAL);
				if (SavedFlipMessage)
					{
					addKeyEvent(TRawEvent::EKeyDown, SavedFlipMessage);
					addKeyEvent(TRawEvent::EKeyUp, SavedFlipMessage);
					SavedFlipMessage = 0;
					}
				}
			else
				{
				ShowWindow(win, SW_SHOWNORMAL);
				ShowWindow(win, SW_HIDE);
				}
			}
			break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
			{
			//get the key code, this will pick up if it has been remapped or not.
			TUint scanCode = systemIni->iKeyboard.ScanCodeToRemappedKey(HIWORD(lParam));
	   /*
		* We could do this to support generation of special characters using Alt+KeyPadNum
		* combinations, but we would need to find a way to suppress the generation of
		* home/end scancodes etc., so leave it for the moment.
					MSG msg={hWnd,message,wParam,lParam,GetMessageTime(),GetMessagePos()};
					TranslateMessage(&msg);
					TUint charCode=0;
					// look in the message queue to get character associated with keypress
					if (PeekMessageU()(&msg,hWnd,WM_CHAR,WM_CHAR,PM_NOREMOVE))
						charCode=msg.wParam;
					// Pass the character as the HIWORD of the Epoc scan code
					v.Set(TRawEvent::EKeyUp,(charCode<<16)|scanCode);
		*/
					v.Set(TRawEvent::EKeyUp,scanCode);
	    			TheEventQ.Add(v);
			break;
			}
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
				{
				//only handle mouse clicks on screen 0
				TInt xpos=((TInt16)(lParam&0xffff));
				TInt ypos = (TInt16)((lParam>>16)&0xFFFF);
				if (DMultiTouch::iMultiTouchTempEnabled)
					{
					MultiChildWndPointer(message,xpos,ypos,0,0);
					DMultiTouch::iMultiTouchTempEnabled = FALSE;				
					}
				else
					{
					TInt screenNumber=ScreenFromHWND(hWnd,TheWin);
					if(screenNumber!=0)
						break;
					FrameWndPointer(message,xpos,ypos,screenNumber);
					}
				break;
				}
		case WM_PAINT:
			{
			DScreenProperties* screenProps = ScreenPropsFromHWND(hWnd, TheWin);
			if(screenProps == NULL)
				{
				return DefWindowProcA(hWnd, message, wParam, lParam);
				}
    		TViewport& viewport = screenProps->iViewport;
    		TInt screenNumber=ScreenFromHWND(hWnd,TheWin);

			PAINTSTRUCT p;

			BeginPaint(hWnd,&p);
	   		HDC hdcBits;
			BITMAP bm;
    		hdcBits=CreateCompatibleDC(p.hdc);
			GetObjectA(TheScreenBitmap[screenNumber],sizeof(BITMAP),&bm);
    		SelectObject(hdcBits,TheScreenBitmap[screenNumber]);
		
			RECT windowRect;
			GetClientRect(TheWin[screenNumber],&windowRect);
			
			viewport.SetViewportHeight(windowRect.bottom);
			viewport.SetViewportWidth(windowRect.right);
			
			
			switch (CurrentFlipState[screenNumber])
				{
				case EEmulatorFlipRestore:
					{
					BitBlt(p.hdc,0,0,windowRect.right,windowRect.bottom,hdcBits,
						viewport.GetViewportOffsetX(),viewport.GetViewportOffsetY(),SRCCOPY);
					break;
					}
				case EEmulatorFlipInvert:
					{
					TInt sourceX = screenProps->iXYInputWidth - viewport.GetViewportWidth() - viewport.GetViewportOffsetX();
					if(sourceX<0)
						sourceX=0;
					TInt sourceY = screenProps->iXYInputHeight - viewport.GetViewportHeight() - viewport.GetViewportOffsetY();
					if(sourceY<0)
						sourceY=0;
					TInt sourceWidth = viewport.GetMaxWidth()-sourceX - viewport.GetViewportOffsetX();
					TInt sourceHeight = viewport.GetMaxHeight()-sourceY - viewport.GetViewportOffsetY();
					
					//when inverted it is necessary to translate the image by 1 pixel up and to the left,
					//to avoid a glitch when scrolling using ScrollWindowEx()
					POINT arrayPoints[3]={
						{sourceWidth-1,sourceHeight-1},
						{-1,sourceHeight-1},
						{sourceWidth-1,-1}
						};
					PlgBlt(p.hdc,arrayPoints,hdcBits,sourceX,sourceY,sourceWidth,sourceHeight,NULL,NULL,NULL);
					break;
					}
				case EEmulatorFlipLeft:
					{
					TInt offsetX = screenProps->iXYInputWidth- viewport.GetViewportHeight()  - viewport.GetViewportOffsetY(); 	
					TInt offsetY = viewport.GetViewportOffsetX(); 	
		
					POINT arrayPoints[3]={{0,windowRect.bottom},{0,0},{windowRect.right,windowRect.bottom}};
					PlgBlt(p.hdc,arrayPoints,hdcBits,offsetX,offsetY,viewport.GetViewportHeight(),viewport.GetViewportWidth(),NULL,NULL,NULL);
					break;
					}
				case EEmulatorFlipRight:
					{
					TInt offsetX = viewport.GetViewportOffsetY(); 
					TInt offsetY = screenProps->iXYInputHeight - viewport.GetViewportWidth() - viewport.GetViewportOffsetX(); 	
									
					POINT arrayPoints[3]={{windowRect.right,0},{windowRect.right,windowRect.bottom},{0,0}};
					PlgBlt(p.hdc,arrayPoints,hdcBits,offsetX,offsetY,viewport.GetViewportHeight(),viewport.GetViewportWidth(),NULL,NULL,NULL);
					break;
					}
				}

			
			DeleteDC(hdcBits);
			if (WinsGuiPowerHandler->iStandby)
				{
				TInt x,y;
				CalcTextPos(screenNumber, x, y);
				TextOutA(p.hdc, x, y, "Standby", 7);
				}
			else if (systemIni->iScreens[screenNumber]->iScreenOff)
				{
				TInt x,y;
				CalcTextPos(screenNumber, x, y);
				TextOutA(p.hdc, x, y, "Screen Off", 10);
				}
 			EndPaint(hWnd,&p);
			break;
			}
		case WM_ACTIVATE:
			//Added so that change in modifier keys can be detected without sending
			//EActive/EInActive to wserv as it results in switching the timers
   			if((wParam & 0xffff)!= WA_INACTIVE)
   				UpdateModifiers();
			break;
		case WM_CHAR:
		case WM_SYSCHAR:
		case WM_DEADCHAR:
		case WM_SYSDEADCHAR:
			break;
		case WM_CLOSE: //pass on message to control window, it will then destroy all e,ulator windows
			SendMessage(TheControlWin,WM_CLOSE, NULL, NULL);
			break;
		case WM_DESTROY:
			{
			DScreenProperties* screenProps = ScreenPropsFromHWND(hWnd, TheWin);
			if(screenProps == NULL)
				{
				return DefWindowProcA(hWnd, message, wParam, lParam);
				}
    					
			// save window's position information
			screenProps->iWinPlace.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(hWnd, &screenProps->iWinPlace);
			
			break;
			}
		case WM_INPUT:
			{
			if (systemIni->MultiTouchEnabled() && DMultiTouch::iMultiTouchSupported && systemIni->GCEEnabled())
				{
				TInt screenNumber=ScreenFromHWND(hWnd,TheWin);
				if(screenNumber==0)
					{
					TheMultiTouch->OnWmInput(hWnd, message, wParam, lParam,TheChildWin[screenNumber]);	
					}
				}
			else
				{
				Fault(EGuiInvalidMultiTouch);
				}
		   	break;
			}
		default:
	        return(DefWindowProcA(hWnd,message,wParam,lParam));
	    }
    return(FALSE);
	
	}

void SetStatusBarFont(HWND& aStatusBar)
	{
	int statwidths[] = {100,200,300,400,500,600,700,800};
	SendMessage(aStatusBar, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);
	HFONT hOrigFont = (HFONT) SendMessage(aStatusBar, WM_GETFONT, 0, 0);
	SetProp(aStatusBar, TEXT("PROP_ORIGINAL_FONT"), (HANDLE) hOrigFont);
	LOGFONT lf;
	GetObject(hOrigFont, sizeof(lf), &lf);
	lf.lfHeight = (long)(lf.lfHeight / 1.5);
	lf.lfWeight = FW_NORMAL;
	HFONT hFont = CreateFontIndirect(&lf);
	SetProp(aStatusBar, TEXT("PROP_ITALIC_FONT"), (HANDLE) hFont);
	hFont = (HFONT) GetProp(hwndStatus, TEXT("PROP_ITALIC_FONT"));
	SendMessage(aStatusBar, WM_SETFONT, (WPARAM) hFont, FALSE);
	}

DWORD WINAPI KernelWindowThread(LPVOID aArg)
//
// The kernel window thread.
//
	{
	HMODULE hmodule = GetModuleHandleA("winsgui.dll");
	__ASSERT_ALWAYS(hmodule!=NULL,Fault(EGuiGetModuleHandle));

	WNDCLASSA w;
	memclr(&w, sizeof(WNDCLASSA));
   	w.style=CS_OWNDC|CS_VREDRAW|CS_HREDRAW;
   	w.lpfnWndProc=(WNDPROC)ctrlwinProc;
   	w.hInstance=(HINSTANCE)aArg;
   	w.hIcon=LoadIconA((HINSTANCE)hmodule,MAKEINTRESOURCEA(EPOC_ICON));
   	w.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
   	w.lpszClassName="E32KernelControlWindow";
	
	ATOM a=RegisterClassA(&w);
	__ASSERT_ALWAYS(a!=0,Fault(EGuiRegisterWindow));

	RECT ctlrwindowRect={0,0,270,0};
	AdjustWindowRect(&ctlrwindowRect,KControlWinStyle,FALSE);
	TInt ctrlwindowWidth=ctlrwindowRect.right-ctlrwindowRect.left;
	TInt ctrlwindowHeight=ctlrwindowRect.bottom-ctlrwindowRect.top;

    TheControlWin=CreateWindowA(
		"E32KernelControlWindow",
		systemIni->iWindowTitle,
		KInvisibleControlWinStyle,
		KWinPosX,
		KWinPosY,
		ctrlwindowWidth,
		ctrlwindowHeight,
		(HWND)NULL,
		NULL,
		(HINSTANCE)aArg,
		(LPSTR)NULL
		);
	__ASSERT_ALWAYS(TheControlWin!=NULL,Fault(EGuiKernelWindowCreate));

	memclr(&w, sizeof(WNDCLASSA));
   	w.style=CS_OWNDC;
   	w.lpfnWndProc=(WNDPROC)winProc;
   	w.hInstance=(HINSTANCE)aArg;
   	w.hIcon=LoadIconA((HINSTANCE)hmodule,MAKEINTRESOURCEA(EPOC_ICON));
   	w.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
   	w.lpszClassName="E32KernelWindow";

	a=RegisterClassA(&w);
	__ASSERT_ALWAYS(a!=0,Fault(EGuiRegisterWindow));

	memclr(&w, sizeof(WNDCLASSA));
	w.style=CS_OWNDC;
	w.lpfnWndProc=(WNDPROC)childWinProc;
	w.hInstance=(HINSTANCE)aArg;
	w.hCursor=LoadCursorA(NULL,MAKEINTRESOURCEA(32512)); //ICD_ARROW
	w.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	w.lpszMenuName=NULL;
	w.lpszClassName="E32KernelChildWindow";
	a=RegisterClassA(&w);
	__ASSERT_ALWAYS(a!=0,Fault(EGuiRegisterChildWindow));

	if (masterIni && masterIni->iSystemInis.Count() > 1)	
		{
		//add Configuration Items to the system menu if there's > 1 config
		HMENU hMenu = GetSystemMenu(TheControlWin, FALSE);
		InsertMenu(hMenu,5, MF_BYPOSITION|MF_SEPARATOR,0,NULL);
		InsertMenuA(hMenu,6, MF_BYPOSITION|MF_STRING, 1, "Next Config...");
		}
	
	ShowWindow(TheControlWin,SW_SHOWDEFAULT);
	UpdateWindow(TheControlWin);

	//Create frame windows and child windows
	for(TInt screen=0;screen<systemIni->iScreens.Count();screen++)
		{
	
		RECT windowRect={0,0,systemIni->iScreens[screen]->iXYInputWidth,systemIni->iScreens[screen]->iXYInputHeight};
		AdjustWindowRect(&windowRect,KWinStyle,FALSE);
		TInt windowWidth=windowRect.right-windowRect.left;
		TInt windowHeight=windowRect.bottom-windowRect.top;
    
		CHAR title[20];
		wsprintfA(title, "Screen %d", screen);

		TheWin[screen]=CreateWindowA(
			"E32KernelWindow",
			title,
			KInvisibleWinStyle,
			KWinPosX,
			KWinPosY,
			windowWidth,
			windowHeight,
			(HWND)NULL,
			NULL,
			(HINSTANCE)aArg,
			(LPSTR)NULL
			);
		__ASSERT_ALWAYS(TheWin[screen]!=NULL,Fault(EGuiKernelWindowCreate));
		
		LoadFasciaBitmap(screen);

		TheChildWin[screen]=CreateWindowA(
			"E32KernelChildWindow",
			"",
			WS_CHILDWINDOW|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
			systemIni->iScreens[screen]->iScreenOffsetX,
			systemIni->iScreens[screen]->iScreenOffsetY,
			systemIni->iScreens[screen]->iScreenWidth,
			systemIni->iScreens[screen]->iScreenHeight,
			TheWin[screen],
			NULL,
			(HINSTANCE)aArg,
			(LPSTR)NULL
			);
		__ASSERT_ALWAYS(TheChildWin[screen]!=NULL,Fault(EGuiKernelChildWindowCreate));
		
		// Create status bars
		if (systemIni->MultiTouchEnabled() && systemIni->GCEEnabled())
			{
			HMODULE hmodComCtl = LoadLibrary(TEXT("comctl32.dll"));
			typedef int (WINAPI* FNINITCC)();
			FNINITCC pfnInitCommonControls = GetProcAddress(hmodComCtl, "InitCommonControls");
			pfnInitCommonControls();
			hwndStatus = CreateWindowExA(0, STATUSCLASSNAMEA, NULL,
								WS_CHILD | WS_VISIBLE | CCS_BOTTOM ,
									0,0,0,0,
									TheWin[0], NULL, GetModuleHandle(NULL), NULL);                 
			SetStatusBarFont(hwndStatus);
			SetWindowPos(hwndStatus,NULL, 0,0,0,0,SWP_HIDEWINDOW);
			}
		}
	
	//Restore window data from ini file if it exists. 
	HANDLE hSysIni = CreateFileA(systemIni->iSysIniFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	TBool success=(hSysIni != INVALID_HANDLE_VALUE) ? ETrue : EFalse;
	
	DWORD numRead;
	TInt fileVersion=0;
	if(success) 
		{
		ReadFile(hSysIni, &fileVersion, sizeof(TInt), &numRead, 0);
		}
	
	//Check we are using a dat file created by this version of the program.
	if(success && (fileVersion==KDatFileVersion) )
		{
				
		TInt savedConfiguration=0; //set this to default configuration
				
		if(ReadFile(hSysIni, &savedConfiguration, sizeof(TInt), &numRead, 0) && (numRead>0) )
			{
			//Don't restore the saved configuration, see INC114502.
			//This could be reenabled in future as an optional operation
			//dependent on an entry in the epoc.ini file.

			//SwitchConfiguration(savedConfiguration);
			}
	
		//restore each window to saved state
		for(TInt screen=0;screen<systemIni->iScreens.Count();screen++)
			{
		
			//	If the .ini file was opened, get the saved settings for the windows position the last time
			//	this emulator was run, if any, and move the window accordingly.
			
			TWindowState savedState;
			
			TBool stateLoaded = ReadFile(hSysIni, &savedState, sizeof(TWindowState), &numRead, 0) && (numRead>0);
			 			
			if (stateLoaded)
				{				
				//only allow window to be restored to 
				//maximized or normal mode,
				//this prevents it being restored in minimized mode
				//or others.
				if(savedState.iWinPlace.showCmd != SW_MAXIMIZE)
						savedState.iWinPlace.showCmd= SW_NORMAL;
				
				//if starting in same configuration and/or rotation as last time emulator was run
				//it makes sense to restore scroll offset, window position,
				//and dimensions, if not, only restore position and window (ie. maximized/normal) state.
				if(savedConfiguration == CurrentConfiguration &&
					savedState.iFlipstate == CurrentFlipState[screen])
					{
					//Restore window placement
					SetWindowPlacement(TheWin[screen], &savedState.iWinPlace);
					
					TViewport& viewport = systemIni->iScreens[screen]->iViewport;
								
					viewport.ScrollToX(savedState.iXoffset, TheWin[screen]);
					viewport.ScrollToY(savedState.iYoffset, TheWin[screen]);
					}
				else
					{
					
					RECT oldRect;
					GetWindowRect(TheWin[screen], &oldRect);
					//save default window dimensions
					TInt width=oldRect.right-oldRect.left;
					TInt height=oldRect.bottom - oldRect.top;

					//restore position and window state from file
					SetWindowPlacement(TheWin[screen], &savedState.iWinPlace);
					
					RECT currentRect;
					GetWindowRect(TheWin[screen], &currentRect);
					//keep default size.
					MoveWindow(TheWin[screen],currentRect.left, currentRect.top, width, height, TRUE);
					
					}


				// Check that enough of the recorded window position is visible on the screen

				TBool enoughVisible = false;

				// vague values for ensuring we have enough of the window title bar to grab
				// if the window is partly off screen 
				const TInt KTitleBarGrabX=80;
				const TInt KTitleBarGrabY=50;

				//inspect dimensions of the window to be restored.
				RECT savedRect;
				GetWindowRect(TheWin[screen], &savedRect);

				SystemMonitors monitors;

				if (monitors.Count() == 1)		/* Original algorithm */
					{
					RECT rcIntersect, rcScreen;

					SetRect(&rcScreen, KTitleBarGrabX, savedRect.bottom-savedRect.top,
						GetSystemMetrics(SM_CXSCREEN)-KTitleBarGrabX, GetSystemMetrics(SM_CYSCREEN)-KTitleBarGrabY);

					enoughVisible = IntersectRect(&rcIntersect, &savedRect, &rcScreen);
					}
				else							/* > 1 monitor; do it differently */
					{
					RECT cornerBox1, cornerBox2;

					// The top-left corner box
					SetRect(&cornerBox1, savedRect.left, savedRect.top,
						savedRect.left + KTitleBarGrabX, savedRect.top + KTitleBarGrabY);

					// The top-right corner box
					SetRect(&cornerBox2, savedRect.right - KTitleBarGrabX, savedRect.top,
						savedRect.right, savedRect.top + KTitleBarGrabY);

					// Require one of these rectangles to be all on one monitor
					enoughVisible = monitors.RectAllOnOne(cornerBox1) || monitors.RectAllOnOne(cornerBox2);
					}

				if (!enoughVisible)
					{
					SetWindowPos(TheWin[screen], HWND_TOP, 0,0,0,0, SWP_NOSIZE);
					}

				}
			else //if there was no stored info for this screen
				{
				ShowWindow(TheWin[screen],SW_MAXIMIZE);
				}
			}
		}
	else
		{
		//use default configuration and make windows visible
		SwitchConfiguration(CurrentConfiguration);
		for(TInt screen=0;screen<systemIni->iScreens.Count();screen++)
			{
			ShowWindow(TheWin[screen],SW_MAXIMIZE);
			UpdateWindow(TheWin[screen]);
			}
		}

	//close file if it was opened	
	if(success) 
		{
		CloseHandle(hSysIni);
		}
		
			
	if (systemIni->iInitialFlipMsg != 0)
		{
		addKeyEvent(TRawEvent::EKeyDown,systemIni->iInitialFlipMsg);
		addKeyEvent(TRawEvent::EKeyUp,systemIni->iInitialFlipMsg);
		}

	SetFocus(TheWin[0]);

	MSG m;
	while (GetMessageA(&m,NULL,0,0))
    	{
        DispatchMessageA(&m);
	    }
	
	ExitProcess(m.wParam);
	return 0;
	}

SystemMonitors::SystemMonitors(void)
	{
	TInt n;

	iCount = 1;
	iHaveMultiMonFunctions = false;

	if ((n = GetSystemMetrics(SM_CMONITORS)) <= 1)
		{
		return;
		}

	HMODULE huser32 = GetModuleHandleA("user32.dll");

	// Get pointers to the APIs we want
	if (huser32 == NULL ||
		(ipMonitorFromRect =
		(HMONITOR (WINAPI *)(LPCRECT lprcScreenCoords, UINT uFlags))
		GetProcAddress(huser32, "MonitorFromRect")) == NULL ||
		(ipGetMonitorInfo =
		(BOOL (WINAPI *)(HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo))
		GetProcAddress(huser32, "GetMonitorInfoA")) == NULL)
		{
		return;
		}

	iCount = n;
	iHaveMultiMonFunctions = true;
	}

TBool SystemMonitors::RectAllOnOne(RECT& rect)
	{
	HMONITOR monitor = MonitorFromRect(rect);
	if (monitor == NULL)
		{
		return false;
		}

	MONITORINFO monInfo;
	monInfo.cbSize = sizeof(MONITORINFO);

	if (! GetMonitorInfo(monitor, &monInfo))
		{
		return false;
		}

	RECT overlap;

	if (IntersectRect(&overlap, &rect, &monInfo.rcWork) &&
		EqualRect(&overlap, &rect))
		{
		return true;
		}

	return false;
	}

HMONITOR SystemMonitors::MonitorFromRect(const RECT& rect, UINT flags)
	{
	if (! iHaveMultiMonFunctions)
		{
		return NULL;
		}

	return (*ipMonitorFromRect)(&rect, flags);
	}

TBool SystemMonitors::GetMonitorInfo(HMONITOR monitor, LPMONITORINFO pMonInfo)
	{
	if (! iHaveMultiMonFunctions)
		{
		return false;
		}

	return (*ipGetMonitorInfo)(monitor, pMonInfo);
	}

void DrawLeds()
	{
	HDC winDC = GetDC(TheWin[0]);
	HDC hdcBits;
	hdcBits=CreateCompatibleDC(winDC);
	SelectObject(hdcBits,TheScreenBitmap[0]);
	HPEN pen=CreatePen(PS_SOLID,0,RGB(0,0,0));
	SelectObject(hdcBits,pen);
	HBRUSH brush;
	LOGBRUSH redbrush={BS_SOLID, RGB(0xff,0,0)};
	LOGBRUSH greenbrush={BS_SOLID, RGB(0,0xff,0)};
	LOGBRUSH blackbrush={BS_SOLID, RGB(0,0,0)};
	// red
	if (LedMask & KLedMaskRed1)
		brush=CreateBrushIndirect(&redbrush);
	else
		brush=CreateBrushIndirect(&blackbrush);
	SelectObject(hdcBits,brush);
	DWinsUi *ini=systemIni;
	Ellipse(hdcBits, ini->iLedOffsetX, ini->iLedOffsetY, ini->iLedOffsetX+ini->iLedSize, ini->iLedOffsetY+ini->iLedSize);
	DeleteObject(brush);
	// green
	if (LedMask & KLedMaskGreen1)
		brush=CreateBrushIndirect(&greenbrush);
	else
		brush=CreateBrushIndirect(&blackbrush);
	SelectObject(hdcBits,brush);
	if (ini->iLedVertical)
		Ellipse(hdcBits, ini->iLedOffsetX, ini->iLedOffsetY+ini->iLedSize+ini->iLedGap,
		ini->iLedOffsetX+ini->iLedSize, ini->iLedOffsetY+ini->iLedSize+ini->iLedGap+ini->iLedSize);
	else
		Ellipse(hdcBits, ini->iLedOffsetX+ini->iLedSize+ini->iLedGap, ini->iLedOffsetY,
		ini->iLedOffsetX+ini->iLedSize+ini->iLedGap+ini->iLedSize, ini->iLedOffsetY+ini->iLedSize);
	DeleteObject(brush);
	DeleteObject(pen);
	DeleteDC(hdcBits);
	ReleaseDC(TheWin[0], winDC);
	if (ini->iLedVertical)
		{
		RECT r={ini->iLedOffsetX,
			ini->iLedOffsetY,
			ini->iLedOffsetX+ini->iLedSize,
			ini->iLedOffsetY+ini->iLedSize+ini->iLedGap+ini->iLedSize};
		InvalidateRect(TheWin[0], &r, FALSE);
		}
	else
		{
		RECT r={ini->iLedOffsetX,
			ini->iLedOffsetY,
			ini->iLedOffsetX+ini->iLedSize+ini->iLedGap+ini->iLedSize,
			ini->iLedOffsetY+ini->iLedSize};
		InvalidateRect(TheWin[0], &r, FALSE);
		}
	}

void DWinsUi::ScreenInfo(TScreenInfoV01& aInfo)
//
// Return screen 0 info to the window server.
//
	{
	aInfo.iWindowHandleValid=ETrue;
	aInfo.iWindowHandle=TheChildWin[0];
	aInfo.iScreenAddressValid=EFalse;
	aInfo.iScreenAddress=NULL;
	aInfo.iScreenSize.iWidth = iScreens[0]->iMaxScreenWidth;
	aInfo.iScreenSize.iHeight = iScreens[0]->iMaxScreenHeight;
	}
	
	
TBool DWinsUi::VideoInfo(TInt aScreenNumber, TVideoInfoV01& aInfo)
	{
	return VideoInfo(aScreenNumber,iScreens[aScreenNumber&KMaskScreenNum]->iCurrentMode,aInfo);
	}
	
/// Could fail if flip mode is not supported
TBool DWinsUi::VideoInfo(TInt aScreenNumber,TInt aModeNumber, TVideoInfoV01& aInfo)
	{
	aScreenNumber &= KMaskScreenNum;
	if (aScreenNumber>=iScreens.Count())
		return EFalse;
	if (masterIni->iBufferSet.Count() && masterIni->iBufferSet[aScreenNumber].iDisplayDriverCount > 0)
		{
			return VideoInfoForDisplayDriver(aScreenNumber,aModeNumber,aInfo);
		}
	else
		{
		if ((aModeNumber&KMaskModeNum)>=1)
			return EFalse;	//non-gce emulator doesn't support changing the mode number.
		DScreenProperties* screenProperties=iScreens[aScreenNumber];
		aInfo.iSizeInPixels.iWidth = screenProperties->iMaxScreenWidth;
		aInfo.iSizeInPixels.iHeight = screenProperties->iMaxScreenHeight;
		aInfo.iSizeInTwips.iWidth = screenProperties->iMaxPhysicalScreenWidth ? screenProperties->iMaxPhysicalScreenWidth : TInt(screenProperties->iScreenWidth*KDefaultPixelsToTwipsX);
		aInfo.iSizeInTwips.iHeight = screenProperties->iMaxPhysicalScreenHeight ? screenProperties->iMaxPhysicalScreenHeight : TInt(screenProperties->iScreenHeight*KDefaultPixelsToTwipsY);
		aInfo.iIsMono = EFalse;
		aInfo.iIsPalettized = EFalse;
		aInfo.iDisplayMode=screenProperties->iCurrentMode;
		aInfo.iIsPixelOrderRGB = ETrue;
		aInfo.iIsPixelOrderLandscape=ETrue;

		aInfo.iVideoAddress =  (TInt)TheChildWin[aScreenNumber];
		aInfo.iBitsPerPixel = screenProperties->iColorDepth;
		aInfo.iOffsetToFirstPixel=0;
		aInfo.iOffsetBetweenLines=0;
		}
	return ETrue;	
	}

/** Could fail if flip mode is not supported.
 Note that this method is inteneded to be called directly to parameterise the setting up of the display driver,
 so it must survive absense of the display driver installation!
**/
 
TBool DWinsUi::VideoInfoForDisplayDriver(TInt aScreenNumber,TInt aModeNumber, TVideoInfoV01& aInfo,  TBool aRealWidthAndHeight)
	{
	aScreenNumber &= KMaskScreenNum;
	DScreenProperties* screenProperties = iScreens[aScreenNumber];
	if ((aModeNumber&KMaskModeNum) >= screenProperties->iMaxModes)
		{
		return EFalse;
		}
		
	aInfo.iSizeInPixels.iWidth = aRealWidthAndHeight ? screenProperties->iScreenWidth : screenProperties->iMaxScreenWidth;
	aInfo.iSizeInPixels.iHeight = aRealWidthAndHeight ? screenProperties->iScreenHeight : screenProperties->iMaxScreenHeight;
	
	if (aRealWidthAndHeight==EFalse)
		{
		aInfo.iSizeInTwips.iWidth = screenProperties->iMaxPhysicalScreenWidth ?	screenProperties->iMaxPhysicalScreenWidth : TInt(screenProperties->iScreenWidth*KDefaultPixelsToTwipsX);
		aInfo.iSizeInTwips.iHeight = screenProperties->iMaxPhysicalScreenHeight ? screenProperties->iMaxPhysicalScreenHeight : TInt(screenProperties->iScreenHeight*KDefaultPixelsToTwipsY);
		}
	else
		{
		aInfo.iSizeInTwips.iWidth = screenProperties->iPhysicalScreenWidth ? screenProperties->iPhysicalScreenWidth : TInt(screenProperties->iScreenWidth*KDefaultPixelsToTwipsX);
		aInfo.iSizeInTwips.iHeight = screenProperties->iPhysicalScreenHeight ? screenProperties->iPhysicalScreenHeight : TInt(screenProperties->iScreenHeight*KDefaultPixelsToTwipsY);
		}
	
	aInfo.iIsMono = EFalse;
	aInfo.iIsPalettized = EFalse;
	aInfo.iDisplayMode=screenProperties->iCurrentMode;
	aInfo.iIsPixelOrderRGB = ETrue;
	aInfo.iIsPixelOrderLandscape=ETrue;
	
	// Set memory to iVideoAddress to NULL to trigger the HAL code into querying the video address
	// separately
	aInfo.iVideoAddress = NULL;

	TInt bpp=screenProperties->iModeDepths[aModeNumber&KMaskModeNum];												
	aInfo.iBitsPerPixel=bpp;
	if (bpp>8)
		{
		bpp=(bpp+15)&-16;	//12 & 16 --> 16 ; 24 & 32 --> 32
		}
	
	aInfo.iOffsetToFirstPixel=0;
#ifdef TEST_GCE_VARIABLE_START_EXTRA
	aInfo.iOffsetToFirstPixel+= TEST_GCE_VARIABLE_START_EXTRA*(1+aModeNumber&KMaskScreenModeNum);
	if ((aModeNumber& KModeFlagFlipped)
		{
#ifndef ASSYMETRIC_SQUARE_STRIDE		
		if (aInfo.iSizeInPixels.iWidth!=aInfo.iSizeInPixels.iHeight)
#endif
			aInfo.iOffsetToFirstPixel+= TEST_GCE_VARIABLE_START_EXTRA*KEmulMaxNumModes;	
		}
#endif
	if (aModeNumber& KModeFlagFlipped)
		{
		// we calculate the number of bytes per scanline that MUST be a multiple of 32 bits word (alignment)
		// screenProperties->iMaxScreenHeight * bpp represnts the number of bits per scanline
		// +31 is the ceiling
		// we shift right (>>3) because there are 8 bits/byte
		// we mask with ~3 because we are intrested in the octet value
		aInfo.iOffsetBetweenLines=((screenProperties->iMaxScreenHeight * bpp + 31) >> 3) & ~3;
		}
	else
		{
		// please see the comment above
		aInfo.iOffsetBetweenLines=((screenProperties->iMaxScreenWidth * bpp + 31) >> 3) & ~3;
		}
#ifdef  TEST_GCE_VARIABLE_STRIDE_EXTRA
	aInfo.iOffsetBetweenLines+=TEST_GCE_VARIABLE_STRIDE_EXTRA;
#endif	

	return ETrue;	
	}

TInt DMasterIni::DoHalFunction(TAny* aPtr, TInt aFunction, TAny* a1, TAny* a2)
	{
	return masterIni->HalFunction((TInt)aPtr,aFunction,a1,a2);
	}


TInt DMasterIni::HalFunction(TInt aDeviceNumber, TInt aFunction, TAny* a1, TAny* a2)
	{
	if (TUint(aDeviceNumber) >= TUint(systemIni->iScreens.Count()))
		return KErrArgument;
	
	TInt mode;
	TInt maxMode=1;
	TInt r=KErrNone;
	switch(aFunction)
		{
		case EDisplayHalScreenInfo:
			{
			TPckgBuf<TScreenInfoV01> vPckg;
			systemIni->ScreenInfo(vPckg());
			Kern::InfoCopy(*(TDes8*)a1,vPckg);
			break;
			}
		case EDisplayHalWsRegisterSwitchOnScreenHandling:
			WsSwitchOnScreen=(TBool)a1;
			break;
		case EDisplayHalSetState:
			{
			if(!Kern::CurrentThreadHasCapability(ECapabilityPowerMgmt,__PLATSEC_DIAGNOSTIC_STRING("Checked by Hal function EDisplayHalSetState")))
				return KErrPermissionDenied;
			if ((TBool)a1)
				WinsGuiPowerHandler->ScreenOn(aDeviceNumber);
			else
				WinsGuiPowerHandler->ScreenOff(aDeviceNumber);
			}
			break;

		case EDisplayHalState:
			*(TInt*)a1=!systemIni->iScreens[aDeviceNumber]->iScreenOff;
			break;
		case EDisplayHalWsSwitchOnScreen:
			WinsGuiPowerHandler->ScreenOn();
			break;
		case EDisplayHalMaxDisplayContrast:
			kumemput32(a1,&KMaxDisplayContrast,sizeof(KMaxDisplayContrast));
			break;
		case EDisplayHalDisplayContrast:
			kumemput32(a1,&systemIni->iScreens[aDeviceNumber]->iDisplayContrast,sizeof(systemIni->iScreens[aDeviceNumber]->iDisplayContrast));
			break;
		case EDisplayHalSetDisplayContrast:
			if(!Kern::CurrentThreadHasCapability(ECapabilityWriteDeviceData,__PLATSEC_DIAGNOSTIC_STRING("Checked by Hal function EDisplayHalSetDisplayContrast")))
				return KErrPermissionDenied;
			if (TUint(a1) <= TUint(KMaxDisplayContrast))
				systemIni->iScreens[aDeviceNumber]->iDisplayContrast = TInt(a1);
			else
				r = KErrArgument;
			break;
		case EDisplayHalBacklightOn:
			{
			TBool c = EFalse;
			kumemput32(a1,&c,sizeof(TBool));
			}
			break;
		
		case EDisplayHalCurrentModeInfo:
			{
			//a1 has ptr to buffer for results
			TPckgBuf<TVideoInfoV01> vPckg;
			if (systemIni->VideoInfo(aDeviceNumber, vPckg()))
				Kern::InfoCopy(*(TDes8*)a1,vPckg);
			else
				r=KErrNotSupported;
			}
			break;

		case EDisplayHalSpecifiedModeInfo:
			{
			kumemget32(&mode, a1, sizeof(mode));
			TPckgBuf<TVideoInfoV01> vPckg;
			if (!systemIni->VideoInfo(aDeviceNumber, mode, vPckg()))
				return KErrArgument;
			Kern::InfoCopy(*(TDes8*)a2, vPckg);
			}
			break;

		case EDisplayHalSetMode:
// 			if(!Kern::CurrentThreadHasCapability(ECapabilityMultimediaDD,__PLATSEC_DIAGNOSTIC_STRING("Checked by Hal function EDisplayHalSetMode")))
// 				return KErrPermissionDenied;

			//Note that at present the HAL mode does not apparently get set when the CFbsScreenDevice requires a different mode.
			//At least in the emulator and default h4 implementation...
			
			mode=KMaskModeNum&(TInt) a1;
			maxMode=1;
			//can't avoid this behaviour change test against gce loaded 
			if (masterIni->iBufferSet.Count() &&  masterIni->iBufferSet[aDeviceNumber].iDisplayDriverCount > 0)
				maxMode=systemIni->iScreens[aDeviceNumber]->iMaxModes;
			if (mode >=maxMode || mode<0)
				{
				r = KErrArgument;
				break;
				}
			//Harmless/Pointless in vanilla wins mode.
			systemIni->iScreens[aDeviceNumber]->iCurrentMode=mode;
			
			break;
		
		case EDisplayHalMode:
			{
			//This is always 0 in non-gce emulator
			kumemput32(a1,&systemIni->iScreens[aDeviceNumber]->iCurrentMode,sizeof(systemIni->iScreens[aDeviceNumber]->iCurrentMode));
			}
			break;

		case EDisplayHalModeCount:
			{
			//Need to actually count them here!
			//GCE will ignore modes<=8
			TInt encodedMode=1;
			if (masterIni->iBufferSet.Count() &&  masterIni->iBufferSet[aDeviceNumber].iDisplayDriverCount > 0)
				encodedMode=systemIni->iScreens[aDeviceNumber]->iMaxModes;
			kumemput32(a1,&encodedMode,sizeof(encodedMode));
			}
			break;

		case EDisplayHalColors:
			{
			TInt deepestMode=0;
			if (masterIni->iBufferSet.Count()==0 ||  masterIni->iBufferSet[aDeviceNumber].iDisplayDriverCount <= 0)
				{
				deepestMode = KMaxDisplayColors;  	//I could try and work it out, but this is what used to happen!
				}
			else
				{
				TInt maxBpp=0;
				for (TInt i=0,maxI=systemIni->iScreens[aDeviceNumber]->iMaxModes;i<maxI;i++)	
					if (systemIni->iScreens[aDeviceNumber]->iModeDepths[i]>maxBpp)
						maxBpp=systemIni->iScreens[aDeviceNumber]->iModeDepths[i];
				deepestMode= 1<<maxBpp;
				}

			kumemput32(a1,&deepestMode,sizeof(deepestMode));
			}
			break;
		case EDisplayHalGetDisplayMemoryHandle:
			{
			TInt val = 0;
			TInt passedIn = 0;
			kumemget32(&passedIn, a1, sizeof(TInt));
			if (passedIn != -1)	//not from a getall
				{
				NKern::ThreadEnterCS();
				if (!(masterIni->iBufferSet.Count() == 0 || masterIni->iBufferSet[aDeviceNumber].iDisplayDriverCount <= 0 ))
					{
						r = masterIni->DisplayMemoryHandle(aDeviceNumber, val); 
					}
				else
					{
					r = KErrArgument;
					}
				NKern::ThreadLeaveCS();
				}
			kumemput32(a1, &val, sizeof(TInt));

			}
			break;

		case EDisplayHalGetDisplayMemoryAddress:
			{
			TInt val = 0;
			TInt passedIn = 0;
			kumemget32(&passedIn, a1, sizeof(TInt));
			if (passedIn != -1)	//not from a getall
				{
				if (!(masterIni->iBufferSet.Count() == 0 || masterIni->iBufferSet[aDeviceNumber].iDisplayDriverCount <= 0 ))
					{
					r = masterIni->DisplayMemoryAddress(aDeviceNumber, val);
					}
				else
					{
					r = KErrArgument;
					}
				}
			kumemput32(a1, &val, sizeof(TInt));
			}
			break;

		case EDisplayHalNumberOfResolutions:
			{
			r = NumberOfResolutions(aDeviceNumber, a1, a2);
			}
			break;
		case EDisplayHalSpecificScreenInfo:
			{
			r = SpecificScreenInfo(aDeviceNumber, a1, a2);
			}
			break;
		case EDisplayHalCurrentScreenInfo:
			{
			r = CurrentScreenInfo(aDeviceNumber, a1, a2);			
			}
			break;
		case EDisplayHalSetDisplayState:
			{
			//increase the spinner at both beginning and end of resetting the display state
			NKern::LockedInc(iBufferSet[aDeviceNumber].iStateChangeCount);
			kumemget32(&iBufferSet[aDeviceNumber].iDisplayState, a1, sizeof(TInt));

			switch(iBufferSet[aDeviceNumber].iDisplayState)
				{
				case ENoResolution:
				case EDisconnect:
				case ESingleResolution:
					// the fascia effect of 0x0 resolution
					SetDisplaySize(aDeviceNumber, 0, 0);
					break;
				}

			NKern::LockedInc(iBufferSet[aDeviceNumber].iStateChangeCount);
			}
			break;
		case EDisplayHalGetStateSpinner:
			{
			kumemput32(a1,&iBufferSet[aDeviceNumber].iStateChangeCount,
					sizeof(iBufferSet[aDeviceNumber].iStateChangeCount));
			}
			break;
		default:
			r=KErrNotSupported;
			break;
		}
	return r;
	}

TInt DMasterIni::NumberOfResolutions(TInt aDeviceNumber, TAny* a1, TAny* a2)
	{
	TInt numberOfConfigs;
	switch(iBufferSet[aDeviceNumber].iDisplayState)
		{
		case ENoResolution:
			{
			numberOfConfigs = 0;
			}
			break;
		case EDisconnect:
			{
			return KErrDisconnected;
			}
			break;
		case ESingleResolution:
			{
			numberOfConfigs = 1;
			}
			break;
		case ENormalResolution:
		default:
			{
			numberOfConfigs = iSystemInis.Count();
			if (numberOfConfigs > 1)
				{
				TVideoInfoV01 info1;
				TVideoInfoV01 info2;
				iSystemInis[0]->VideoInfoForDisplayDriver(aDeviceNumber, 0, info1, ETrue);
				iSystemInis[1]->VideoInfoForDisplayDriver(aDeviceNumber, 0, info2, ETrue);
				if (info1.iSizeInPixels.iWidth == info2.iSizeInPixels.iWidth &&
						info1.iSizeInPixels.iHeight == info2.iSizeInPixels.iHeight)
					{
					numberOfConfigs = 1;	//It looks like all resolutions for this device are the same
					}
				}
			}
		}
	kumemput32(a1,&numberOfConfigs,sizeof(numberOfConfigs));
	if(a2)
		{
		kumemput32(a2,&(iBufferSet[aDeviceNumber].iStateChangeCount),sizeof(iBufferSet[aDeviceNumber].iStateChangeCount));
		}
	return KErrNone;
	}

TInt DMasterIni::SpecificScreenInfo(TInt aDeviceNumber, TAny* a1, TAny* a2)
	{
	TInt config;
	switch(iBufferSet[aDeviceNumber].iDisplayState)
		{
		case ENoResolution: //Do Nothing
			break;
		case EDisconnect:
			{
			return KErrDisconnected;
			}
			break;
		case ESingleResolution: //fill (0,0) as the only element in resolution array
			{
			if(*(TInt*)a1 == 0)
				{
				TVideoInfoV01 info;
				info.iSizeInPixels.iHeight = 0;
				info.iSizeInPixels.iWidth = 0;
				info.iSizeInTwips.iHeight = 0;
				info.iSizeInTwips.iWidth = 0;
				TPtr8 infoPtr((TUint8*)&info, sizeof(info), sizeof(info));
				Kern::InfoCopy(*(TDes8*)a2, infoPtr);
				}
			}
			break;
		case ENormalResolution:
		default:
			{
			kumemget32(&config, a1, sizeof(config));
			TPckgBuf<TVideoInfoV01> vPckg;
			iSystemInis[config]->VideoInfoForDisplayDriver(aDeviceNumber, 0, vPckg(), ETrue);
			Kern::InfoCopy(*(TDes8*)a2,vPckg);
			}
		}
	return KErrNone;
	}

TInt DMasterIni::CurrentScreenInfo(TInt aDeviceNumber, TAny* a1, TAny* /*a2*/)
	{
	switch(iBufferSet[aDeviceNumber].iDisplayState)
		{
		case ENoResolution: //Do Nothing
			break;
		case EDisconnect:
			{
			return KErrDisconnected;
			}
			break;
		case ESingleResolution: //fill (0,0)
			{
			TVideoInfoV01 info;
			info.iSizeInPixels.iHeight = 0;
			info.iSizeInPixels.iWidth = 0;
			info.iSizeInTwips.iHeight = 0;
			info.iSizeInTwips.iWidth = 0;
			TPtr8 infoPtr((TUint8*)&info, sizeof(info), sizeof(info));
			Kern::InfoCopy(*(TDes8*)a1, infoPtr);
			}
			break;
		case ENormalResolution:
		default:
			{
			TPckgBuf<TVideoInfoV01> vPckg;
			systemIni->VideoInfoForDisplayDriver(aDeviceNumber, 0, vPckg(), ETrue);
			Kern::InfoCopy(*(TDes8*)a1,vPckg);
			}
		}
	return KErrNone;
	}

TInt DMasterIni::DoXYHalFunction(TAny* aThis, TInt aFunction, TAny* a1, TAny* a2)
	{
	return static_cast<DMasterIni*>(aThis)->XYHalFunction(aFunction,a1,a2);
	}

TInt DMasterIni::XYHalFunction(TInt aFunction, TAny* a1, TAny* a2)
	{
	TInt r=KErrNone;
	switch(aFunction)
		{
		case EDigitiserHalXYInfo:
			{
			if(systemIni->iXYInputType==EXYInputPointer)
				{
				TPckgBuf<TDigitiserInfoV01> vPckg;
				TDigitiserInfoV01& xyinfo=vPckg();
				xyinfo.iDigitiserSize.iWidth=max(systemIni->iScreens[0]->iXYInputWidth,systemIni->iScreens[0]->iMaxScreenWidth+systemIni->iScreens[0]->iScreenOffsetX);		
				xyinfo.iDigitiserSize.iHeight=max(systemIni->iScreens[0]->iXYInputHeight,systemIni->iScreens[0]->iMaxScreenHeight+systemIni->iScreens[0]->iScreenOffsetY);
				xyinfo.iOffsetToDisplay.iX=systemIni->iScreens[0]->iScreenOffsetX;
				xyinfo.iOffsetToDisplay.iY=systemIni->iScreens[0]->iScreenOffsetY;
				Kern::InfoCopy(*(TDes8*)a1,vPckg);
				}
			else
				r=KErrNotSupported;
			}
			break;
			
		// a2 = TBool aSet (ETrue for setting, EFalse for retrieval) 
		// a1 = TDigitizerOrientation (set)
		// a1 = &TDigitizerOrientation (get)
		// Assume screen [0] here as this HAL function is only registered for
		// screen 0 on the emulator platform. Function only called if display==0
		case EDigitiserOrientation:	
			if ((TBool)a2)
				{
				// Set the orientation attribute
				// In case user thread, check it has WDD capability
				if(!Kern::CurrentThreadHasCapability(ECapabilityWriteDeviceData,__PLATSEC_DIAGNOSTIC_STRING("Checked by Hal function EDigitiserOrientation")))
					return KErrPermissionDenied;
				systemIni->iScreens[0]->iXYOrientation = (TInt)a1;
				}
			else
				{
				// Get the orientation attribute, safe copy it into user memory
				kumemput32(a1, &(systemIni->iScreens[0]->iXYOrientation), sizeof(TInt));	
				}
			break; 
			
		default:
			r=KErrNotSupported;
			break;
		}
	return r;
	}

TInt DMasterIni::DoMouseHalFunction(TAny* aThis, TInt aFunction, TAny* a1, TAny* a2)
	{
	return static_cast<DMasterIni*>(aThis)->MouseHalFunction(aFunction,a1,a2);
	}

TInt DMasterIni::MouseHalFunction(TInt aFunction, TAny* a1, TAny* /*a2*/)
	{
	TInt r=KErrNone;
	switch(aFunction)
		{
		case EMouseHalMouseInfo:
			{
			if(systemIni->iXYInputType==EXYInputMouse || systemIni->iXYInputType==EXYInputDeltaMouse)
				{
				TPckgBuf<TMouseInfoV01> vPckg;
				TMouseInfoV01& xyinfo=vPckg();
				xyinfo.iMouseButtons=2;
				xyinfo.iMouseAreaSize.iWidth=max(systemIni->iScreens[0]->iXYInputWidth,systemIni->iScreens[0]->iMaxScreenWidth+systemIni->iScreens[0]->iScreenOffsetX);		
				xyinfo.iMouseAreaSize.iHeight=max(systemIni->iScreens[0]->iXYInputHeight,systemIni->iScreens[0]->iMaxScreenHeight+systemIni->iScreens[0]->iScreenOffsetY);
				xyinfo.iOffsetToDisplay.iX=systemIni->iScreens[0]->iScreenOffsetX;
				xyinfo.iOffsetToDisplay.iY=systemIni->iScreens[0]->iScreenOffsetY;
				Kern::InfoCopy(*(TDes8*)a1,vPckg);
				}
			else
				r=KErrNotSupported;
			}
			break;
		default:
			r=KErrNotSupported;
			break;
		}
	return r;
	}

TInt DMasterIni::DoKbdHalFunction(TAny* /*aThis*/, TInt aFunction, TAny* /*a1*/, TAny* /*a2*/)
	{
	// don't actually do anything, just enough to report a Keyboard is present
	TInt r=KErrNone;
	if(aFunction!=EKeyboardHalKeyboardInfo)
		r=KErrNotSupported;
	return r;
	}

void Inactive()
//
// Window has been minimised.
//
    {

	TRawEvent v;
	v.Set(TRawEvent::EInactive);
    TheEventQ.Add(v);
    }

void UpdateModifiers()
//Updates the modifier key states and sends an event to wserv about the 
//change in state
	{
	TRawEvent v;
	TInt modifiers=0;
    if(GetKeyState(VK_SCROLL)&1)
        modifiers|=EModifierScrollLock;
    if(GetKeyState(VK_NUMLOCK)&1)
        modifiers|=EModifierNumLock;
    if(GetKeyState(VK_CAPITAL)&1)
        modifiers|=EModifierCapsLock;
    v.Set(TRawEvent::EUpdateModifiers,modifiers);
    TheEventQ.Add(v);
	}
	
void Active()
//
// Window has been restored.
// Update the toggling modifiers, reset any others
//
    {
	TRawEvent v;
    UpdateModifiers();
    v.Set(TRawEvent::EActive);
    TheEventQ.Add(v);
    }


void ClearScreen()
    {

    }


TInt DWinsUi::GetVirtualKey(TEmulCommand& aCommand, TInt aX, TInt aY)
	{
	//if the point is in the list of shapes, set the key and return true
	for (TInt keyCount = iVirtualKeys.Count(); --keyCount >= 0; )
		{
		const VirtualKey& vk = *iVirtualKeys[keyCount];
		if (vk.Contains(aX, aY))
			{
			aCommand = vk.Command();
			return vk.Value();
			}
		}
	return -1;
	}

void DWinsUi::TranslateMouseCoords(const TEmulatorFlip aFlipState, const TInt aX, const TInt aY, const TInt aRegionWidth, const TInt aRegionHeight, TInt& aNewX, TInt& aNewY)
	{
	switch (aFlipState)
		{
		case EEmulatorFlipRestore:
			aNewX = aX;
			aNewY = aY;
			break;
		
		case EEmulatorFlipInvert:
			aNewX = aRegionWidth - aX;
			aNewY = aRegionHeight - aY;
			break;
		
		case EEmulatorFlipLeft:
			aNewX = aRegionWidth - aY;
			aNewY = aX;
			break;

		case EEmulatorFlipRight:
			aNewX = aY;
			aNewY = aRegionHeight - aX;
			break;
		}
	}


TBool DWinsUi::OnScreen(TInt aScreen, TInt ax, TInt ay) const
//
// Checks if a point within the Emulator window is on the screen
//
	{
	return (TUint(ax) < TUint(systemIni->iScreens[aScreen]->iScreenWidth) && TUint(ay) < TUint(systemIni->iScreens[aScreen]->iScreenHeight));
	}

TInt DWinsUi::Create(TInt aId)
	{
	TInt r = SetupProperties(aId);
	if (r != KErrNone)
		return r;
	TInt screen;
	DScreenProperties* currentScreen = NULL;
	for(screen=0;screen<iScreens.Count();screen++)
		{
		BITMAPINFOHEADER bitmapinfo;
		currentScreen = iScreens[screen];
		if (readBitmapInfo(&bitmapinfo, currentScreen->iFasciaFileName) == KErrNone)
			{
			currentScreen->iXYInputWidth=bitmapinfo.biWidth;
			currentScreen->iXYInputHeight=bitmapinfo.biHeight;
			}
		currentScreen->iXYInputWidth=max(currentScreen->iXYInputWidth,currentScreen->iScreenWidth+currentScreen->iScreenOffsetX);
		currentScreen->iXYInputHeight=max(currentScreen->iXYInputHeight,currentScreen->iScreenHeight+currentScreen->iScreenOffsetY);
		}

	currentScreen = iScreens[0];
	//note digitizer offsets are relative to EPOC screen 0
	if (-1 == iDigitizerWidth)
		iDigitizerWidth = currentScreen->iXYInputWidth - 
			(currentScreen->iScreenOffsetX + iDigitizerOffsetX);
	else
		currentScreen->iXYInputWidth=max(currentScreen->iXYInputWidth,iDigitizerWidth);

	if (-1 == iDigitizerHeight)
		iDigitizerHeight = currentScreen->iXYInputHeight - 
			(currentScreen->iScreenOffsetY + iDigitizerOffsetY);
	else
		currentScreen->iXYInputHeight=max(currentScreen->iXYInputHeight,iDigitizerHeight);

	return r;
	}

const RDisplayChannel::TPixelFormat DMasterIni::iSupportedPixelFormatTable[] =
	{
		EUidPixelFormatXRGB_8888,
		EUidPixelFormatARGB_8888,
		EUidPixelFormatARGB_8888_PRE,
		EUidPixelFormatXRGB_4444,
		EUidPixelFormatARGB_4444,
		EUidPixelFormatRGB_565 
	};

const TInt DMasterIni::iSupportedPixelFormatTableSize = static_cast<TInt>(sizeof(iSupportedPixelFormatTable)/
		                                                                  sizeof(iSupportedPixelFormatTable[0]));

void DMasterIni::InitBufferFormat(DScreenProperties& aScreenProperties, RDisplayChannel::TBufferFormat& aBufferFormat)
	{
	TUint bitsPerPixel = MaximumBitDepthFromMask(aScreenProperties.iColorDepth);
	
	aBufferFormat.iSize.iWidth = aScreenProperties.iMaxScreenWidth;
	aBufferFormat.iSize.iHeight = aScreenProperties.iMaxScreenHeight;
	switch (bitsPerPixel)
		{
	case 12:	// XRGB4444
		aBufferFormat.iPixelFormat = EUidPixelFormatXRGB_4444;
		break;
	case 16:	// RGB565
		aBufferFormat.iPixelFormat = EUidPixelFormatRGB_565;
			break;
	case 24:	// Really 32bpp, but top 8 unused
	case 32:	// While 32bpp, top 8 will not be used
	default:
		aBufferFormat.iPixelFormat = EUidPixelFormatXRGB_8888;
			break;
		}
	}

TInt DMasterIni::Create()
	{
	TInt configurations = Property::GetInt("ConfigCount", 0);
	if (configurations == 0)
		return KErrGeneral;

	// the pixel formats table is, at present, configuration independent
	TInt count;
	TInt r = KErrNone;
	for (count = 0; count < configurations && r == KErrNone; ++count)
		{
		DWinsUi* dwi = new DWinsUi;
		if (dwi)
			r = dwi->Create(count);

		if (r == KErrNone)
			iSystemInis.Append(dwi);
		}
	if (r != KErrNone)
		return r;

	systemIni = masterIni->iSystemInis[0];

	WinsGuiPowerHandler = DWinsGuiPowerHandler::New();
	if (!WinsGuiPowerHandler)
		return KErrNoMemory;

	TheWin=new HWND[systemIni->iScreens.Count()];
	TheChildWin=new HWND[systemIni->iScreens.Count()];
	TheScreenBitmap=new HBITMAP[systemIni->iScreens.Count()];
	CurrentFlipState=new TEmulatorFlip[systemIni->iScreens.Count()];

	if(!TheWin || !TheChildWin || !TheScreenBitmap || !CurrentFlipState)
		return KErrNoMemory;
	memset(CurrentFlipState,EEmulatorFlipRestore,systemIni->iScreens.Count());

	TBufferSet buffer;
	buffer.iDisplayDriverCount = 0;
	buffer.iDisplayState = ENormalResolution;
	buffer.iDisplayBuffer = 0;
	buffer.iDisplayChannel = NULL;


	TInt i;
	for(i=0;i<systemIni->iScreens.Count();i++)
		{
		DScreenProperties *pScr = systemIni->iScreens[i];

		masterIni->InitBitmapHeader(*pScr, &buffer.iInfo);
		masterIni->InitBufferFormat(*pScr, buffer.iBufferFormat);

		r = masterIni->iBufferSet.Append(buffer);
		if (r != KErrNone)
			return r;
		}

	if (CreateWin32Thread(EThreadEvent, &KernelWindowThread, NULL, ETrue) == NULL)
		return KErrGeneral;

	for(i=0;i<systemIni->iScreens.Count();i++)
		{
		r = Kern::AddHalEntry(EHalGroupDisplay,&DoHalFunction,(TAny*)i,i);
		if (r != KErrNone)
			return r;
		}

	// should really come from Keyboard driver, but not doing it now...
	r = Kern::AddHalEntry(EHalGroupKeyboard,&DoKbdHalFunction,this);
	if (r != KErrNone)
		return r;

	if(systemIni->iXYInputType==EXYInputPointer)
		{
		r = Kern::AddHalEntry(EHalGroupDigitiser,&DoXYHalFunction,this);
		if (r != KErrNone)
			return r;
		}
	else if(systemIni->iXYInputType==EXYInputMouse || systemIni->iXYInputType==EXYInputDeltaMouse)
		{
		r = Kern::AddHalEntry(EHalGroupMouse,&DoMouseHalFunction,this);
		if (r != KErrNone)
			return r;
		}

	return r;
	}

void DMasterIni::InitBitmapHeader(DScreenProperties& aScreenProperties, LPBITMAPV4HEADER aInfo)
	{
	TInt width = aScreenProperties.iMaxScreenWidth; 
	TInt height = aScreenProperties.iMaxScreenHeight;
	TUint bitsPerPixel = MaximumBitDepthFromMask(aScreenProperties.iColorDepth);

	memset(aInfo, 0, sizeof(BITMAPV4HEADER));

	switch (bitsPerPixel)
		{
	case 12:	// XRGB4444
			aInfo->bV4BitCount = 16;
			aInfo->bV4V4Compression = BI_BITFIELDS;
			aInfo->bV4RedMask   = 0x0F00;
			aInfo->bV4GreenMask = 0x00F0;
			aInfo->bV4BlueMask  = 0x000F;
			break;
	case 16:	// RGB565
			aInfo->bV4BitCount = 16;
			aInfo->bV4V4Compression = BI_BITFIELDS;
			aInfo->bV4RedMask   = 0xF800;
			aInfo->bV4GreenMask = 0x07E0;
			aInfo->bV4BlueMask  = 0x001F;
			break;
	case 24:	// Really 32bpp, but top 8 unused
	case 32:	// While 32bpp, top 8 will not be used
	default:
			aInfo->bV4BitCount = 32;
			aInfo->bV4V4Compression = BI_RGB;
			// Mask is implicit for BI_RGB compression
			break;
		}
	
	aInfo->bV4Size = sizeof(BITMAPV4HEADER);
	aInfo->bV4Width = width;
	aInfo->bV4Height = -height;	// Bitmap runs top to bottom
	aInfo->bV4Planes = 1;
	
	TInt bpp = _ALIGN_UP(aInfo->bV4BitCount, 16); //12 & 16 --> 16 ; 24 & 32 --> 32
	TInt widthInPixel = aInfo->bV4Width * bpp;
	//rounding to 32 bits (4 octets) and converting, then, bits to octets;
	TInt scanLineInBytes = _ALIGN_UP(widthInPixel, 32) >> 3; 
	aInfo->bV4SizeImage = -aInfo->bV4Height * scanLineInBytes;

	// Set color space as uncalibrated. All other members are then ignored.
#if defined(LCS_DEVICE_RGB)
	aInfo->bV4CSType = LCS_DEVICE_RGB;
#elif defined(LCS_sRGB)
	aInfo->bV4CSType = LCS_sRGB;
#endif
	}

// Helper function that allocates a single frame buffer. 
static TInt AllocateOneFrameBuffer(TInt aSize, TScreenBuffer &aScreenBuffer)
	{
    // Open shared chunk to the composition framebuffer
	DChunk* chunk = 0;
	// round to page size
	if (aSize <= 0)
		return KErrArgument;
	TUint round = Kern::RoundToPageSize(aSize);
	TLinAddr chunkKernelAddr = 0;
	TUint32 physicalAddress = 0;
	TUint32 chunkMapAttr = 0;

	// create shared chunk
	NKern::ThreadEnterCS();

	TChunkCreateInfo info;
	info.iType = TChunkCreateInfo::ESharedKernelMultiple;
	info.iMaxSize = round;
	info.iMapAttr = 0;
	info.iOwnsMemory = ETrue;
	info.iDestroyedDfc = 0;

	TInt r = Kern::ChunkCreate(info, chunk, chunkKernelAddr, chunkMapAttr);
	if (r == KErrNone)
		{
		// map our chunk to specific 
		r = Kern::ChunkCommitContiguous(chunk, 0, aSize, physicalAddress);
		if (r != KErrNone)
			{
			Kern::ChunkClose(chunk);
			}
		}

	if (r == KErrNone)
		{
		TBufferAddressA* bufferAddress = new TBufferAddressA;
		if (!bufferAddress)
			{
			r = KErrNoMemory;
			}
		else 
			{
			bufferAddress->iAddress = (TAny*)chunkKernelAddr;
			bufferAddress->iChunk = chunk;

			if ((r = aScreenBuffer.iFrameBuffers.Append(bufferAddress->iAddress)) == KErrNone)
				{
				r = aScreenBuffer.iMemChunks.Append(bufferAddress);
				}
			}
		}
	if (r != KErrNone)
		{
		Kern::ChunkClose(chunk);
		}
	NKern::ThreadLeaveCS();
	return KErrNone;
	}

TInt DMasterIni::AllocateFrameBuffers(TInt aScreenNumber, TInt aCount, TInt aSize)
	{
	while (aCount--)
		{
		TInt r = AllocateOneFrameBuffer(aSize, masterIni->iBufferSet[aScreenNumber].iScreenBuffer);
		if (r != KErrNone)
			{
			return r;
			}
		}
	return KErrNone;
	}

void DMasterIni::ReleaseFrameBuffers(TInt aScreenNumber)
	{
	RPointerArray<TAny>& frameBuffers = masterIni->iBufferSet[aScreenNumber].iScreenBuffer.iFrameBuffers;
	RPointerArray<TBufferAddressA>& memChunks = masterIni->iBufferSet[aScreenNumber].iScreenBuffer.iMemChunks;
	RPointerArray<TBufferAddressA>& dsaChunks = masterIni->iBufferSet[aScreenNumber].iDsaBuffer.iMemChunks;

   	NKern::ThreadEnterCS();
	TInt index;
	TInt count = memChunks.Count();
	for (index = 0; index < count; index++)
		{
		Kern::ChunkClose(memChunks[index]->iChunk);
		}
	count = dsaChunks.Count();
	for (index = 0; index < count; index++)
		{
		Kern::ChunkClose(dsaChunks[index]->iChunk);
		}
	NKern::ThreadLeaveCS();
	
	frameBuffers.Reset();
	memChunks.Reset();
	dsaChunks.Reset();
	}


TProcessAddrEntry::TProcessAddrEntry(DProcess *aProcess, TUint8* aAddress): 
	iProcess(aProcess), iAddress(aAddress)
	{
	}


/**
Contruct a Shared Chunk cleanup object which will be used to clean up 
after the process/address table entry. 
*/
TChunkCleanup::TChunkCleanup(DProcess* aProcess, TInt aScreenNumber)
    : TDfc((TDfcFn)TChunkCleanup::ChunkDestroyed,this,Kern::SvMsgQue(),0)
    , iProcess(aProcess)
    , iScreenNumber(aScreenNumber)
    , iIndex(-1)
    {}

/**
Cancel the action of the cleanup object.
*/
void TChunkCleanup::Cancel()
    {
    // Clear iProcess which means that when the DFC gets queued on chunk destruction
    // our ChunkDestroyed method will do nothing other than cleanup itself.
    iProcess = NULL;
    }

/**
Callback function called when the DFC runs, i.e. when a chunk is destroyed.
*/
void TChunkCleanup::ChunkDestroyed(TChunkCleanup* aSelf)
    {
    DProcess* process = aSelf->iProcess;
    TInt screenNumber = aSelf->iScreenNumber;
    TUint index = aSelf->iIndex;
    // If we haven't been Cancelled...
    if(process && index != -1)
        {
    	if (masterIni->iBufferSet[screenNumber].iProcAddrTable[index].iProcess == process)
    		{
    		masterIni->iBufferSet[screenNumber].iProcAddrTable[index].iProcess = 0;
    		}
    	else
    		{
    		__KTRACE_OPT(KEXTENSION,Kern::Printf("Oops! Someone has messed up our process index!"));
    		}
        }

    // We've finished so now delete ourself
    delete aSelf;
    }


TInt DMasterIni::DisplayMemoryHandle(TInt aScreenNumber, TInt& aHandle)
	{
	if (iBufferSet[aScreenNumber].iDsaBuffer.iMemChunks.Count() == 0)
		{	
		int r;
		r = AllocateOneFrameBuffer(iMaxSizeInBytes, iBufferSet[aScreenNumber].iDsaBuffer);
		if (KErrNone != r)
			{
			return r;
			}
		__ASSERT_DEBUG(iBufferSet[aScreenNumber].iDisplayChannel, Fault(EGuiNoDisplayChannel));
		iBufferSet[aScreenNumber].iDisplayChannel->SetLegacyBuffer(iBufferSet[aScreenNumber].iDsaBuffer.iFrameBuffers[0]);
		}
	
	aHandle = Kern::MakeHandleAndOpen(&Kern::CurrentThread(), 
			iBufferSet[aScreenNumber].iDsaBuffer.iMemChunks[0]->iChunk);
	
	if (aHandle < 0)
		{
		return aHandle;
		}
	
	return KErrNone;
	}



// Find the address of the display memory. 
TInt DMasterIni::DisplayMemoryAddress(TInt aScreenNumber, TInt& aAddress)
	{
	TBufferSet &bufferSet = iBufferSet[aScreenNumber]; 
	DProcess *process = &Kern::CurrentProcess();
	TInt firstFree = -1;
	NKern::FMWait(&iLock);
	TUint count = bufferSet.iProcAddrTable.Count();
	for(TUint i = 0; i < count; ++i)
		{
		DProcess *curProcess = bufferSet.iProcAddrTable[i].iProcess;
		if (curProcess == process)
			{
			aAddress = reinterpret_cast<TInt>(bufferSet.iProcAddrTable[i].iAddress);
			NKern::FMSignal(&iLock);
			return KErrNone;
			}
		if (curProcess == 0 && firstFree == -1)
			{
			firstFree = i;
			}
		}
	NKern::FMSignal(&iLock);
	// If we get here, we couldn't find the process in the iProcAddrTable. 
	// Create a new Process Address entry. 
	// Step 1
	// Create a dummy chunk so that we can detect when the process dies, 
	// give a handle to the user [but don't actually let the process KNOW what the handle is
	// so the user process can't do anything silly with the chunk]. Close our side of the
	// chunk to make sure there is only one reference to it. 
	DChunk* chunk = 0;
	// find page size for one page. 
	TUint round = Kern::RoundToPageSize(1);
	TLinAddr chunkKernelAddr = 0;
	TUint32 chunkMapAttr = 0;
	
	// create shared chunk
	NKern::ThreadEnterCS();
	// Cleanup object, used to issue a DFC when the chunk is closed (and 
	// as the handle of the chunk is not given to the process, it can only
	// be closed when the process terminates!)
	TChunkCleanup *cleanup = new TChunkCleanup(process, aScreenNumber);
	if (!cleanup)
		{
		NKern::ThreadLeaveCS();
		return KErrNoMemory;
		}

	TChunkCreateInfo info;
	info.iType = TChunkCreateInfo::ESharedKernelMultiple;
	info.iMaxSize = round;
	info.iMapAttr = 0;
	info.iOwnsMemory = ETrue;
	info.iDestroyedDfc = cleanup;

	TInt r = Kern::ChunkCreate(info, chunk, chunkKernelAddr, chunkMapAttr);
	
	if (r != KErrNone)
		{
		delete cleanup;
		NKern::ThreadLeaveCS();
		return r;
		}
	
	
	// Create a new handle for the user thread. 
    r = Kern::MakeHandleAndOpen(&Kern::CurrentThread(), chunk);
	Kern::ChunkClose(chunk);
	if (r <= 0)
		{
		NKern::ThreadLeaveCS();
		return r;
		}

	// Step 2
	// Create a second handle for the chunk to the DSA buffer.
	// First part: Make sure there is a DisplayMemoryHandle;
	TInt handle = 0;
	r = DisplayMemoryHandle(aScreenNumber, handle); 
	if (r != KErrNone)
		{
		Kern::ChunkClose(chunk);
		NKern::ThreadLeaveCS();
		return r;
		}
	
	DChunk *dsaChunk = bufferSet.iDsaBuffer.iMemChunks[0]->iChunk;
	
	// Step 3
	// Get the base addrss and insert into table. 
	TUint8* baseAddress = Kern::ChunkUserBase(dsaChunk, &Kern::CurrentThread());
	NKern::FMWait(&iLock);
	// Optimistically, the place we found earlier in the table is still free.
	if (firstFree != -1 && bufferSet.iProcAddrTable[firstFree].iProcess != 0)
		{
		// If not, we go find another one.
		firstFree = -1;
		TUint count = bufferSet.iProcAddrTable.Count();
		for(TUint i = 0; i < count; ++i)
			{
			if (bufferSet.iProcAddrTable[i].iProcess == 0)
				{
				firstFree = i;
				break;
				}
			}
		}
	// Check if there is a free entry - if so, re-use it. 
	if (firstFree != -1)
		{
		bufferSet.iProcAddrTable[firstFree].iProcess = process;
		bufferSet.iProcAddrTable[firstFree].iAddress = baseAddress;
		cleanup->SetIndex(firstFree);
		NKern::FMSignal(&iLock);
		}
	else
		{
		// No free entry. Append it to the list. 
		NKern::FMSignal(&iLock);
		TProcessAddrEntry entry(process, baseAddress);
		r = bufferSet.iProcAddrTable.Append(entry);
		if (r != KErrNone)
			{
			Kern::ChunkClose(chunk);
			NKern::ThreadLeaveCS();
			return r;
			}
		// We added it at the end - so we start from the back and check for the 
		// process, as some other process COULD have added one after us, so we 
		// can't just use the count for index!
		TUint index;
		for(index = bufferSet.iProcAddrTable.Count()-1; index; --index)
			{
			if (bufferSet.iProcAddrTable[index].iProcess == process 
					&& bufferSet.iProcAddrTable[index].iAddress != baseAddress)
				{
				break;
				}
			}
		cleanup->SetIndex(index);
		}
	aAddress = reinterpret_cast<TInt>(baseAddress);

	NKern::ThreadLeaveCS();
	return KErrNone;
	}

EXPORT_C TInt WinsGui::CurrentConfiguration()
	{
	return ::CurrentConfiguration;
	}

GLDEF_C void Fault(TGuiPanic aPanic)
	{
	Kern::Fault("WINS-UI",aPanic);
	}

DECLARE_STANDARD_EXTENSION()
	{
	__KTRACE_OPT(KEXTENSION,Kern::Printf("Starting Emulator GUI"));

	// if NoGui property == true do nothing
	if (Property::GetBool("NoGui",EFalse))
		return KErrNone;

	// create keyboard driver
	TInt r=KErrNoMemory;
	masterIni = new DMasterIni;
	if (masterIni)
		{
		r = masterIni->Create();
		if (r!= KErrNone)
			{
			return r;
			}
		}
	
	DMultiTouch::iMultiTouchSupported = DMultiTouch::Init();

	// Create multitouch when necessary
	if (systemIni->MultiTouchEnabled() && systemIni->GCEEnabled() && DMultiTouch::iMultiTouchSupported)
		{
		TheMultiTouch = new DMultiTouch(systemIni->MultiTouchProximityStep(),systemIni->MultiTouchPressureStep());
		if(!TheMultiTouch)
			{
			r = KErrNoMemory;
			__KTRACE_OPT(KEXTENSION,Kern::Printf("Returns %d",r));
			return r;
			}
		DMultiTouch::iMultiTouchCreated = TRUE;
		}

	__KTRACE_OPT(KEXTENSION,Kern::Printf("Returns %d",r));
	return r;
	}

TInt DWinsUi::DoDefineEmulatorControlHotKey(TAny* aPtr, const char* aValue)
	{
	return static_cast<DWinsUi*>(aPtr)->DefineEmulatorControlHotKey(aValue);
	}


TInt DWinsUi::DefineEmulatorControlHotKey(const char* aValue)
	{
	const char* beg = skipws(aValue);
	const char* end = skiptok(beg);
	TInt err = KErrNone;
	
	TEmulCommand command = ENoCommand;
	TInt data = 0;
	if (_strnicmp(beg, "SelectConfig", end-beg) == 0)
		{
		//get the int param which is the config to switch to
		beg = end;
		char * e;
		data = strtol(beg, &e,0);
		if (beg == e)
			err = KErrArgument;
		end = e;
		command = ESelectConfig;
		}
	else if(_strnicmp(beg, "NextConfig", end-beg) == 0)
		{
		command = ENextConfig;
		}
	else
		{
		err = KErrArgument;
		}
	if (err != KErrNone)
		return err;
	
	// get the keys	
	KeyCombination* pCombination = new KeyCombination(data, command);
	if (!pCombination)
		return KErrNoMemory;	
	
	beg = skipws(end);
	const char* end2;	
	for (TInt i=0;i<KMaxHotKeyCombinationLength;i++)
		{
		TInt key=KErrNotFound;		
		end2 = skiptok(beg);
		TPtrC8 name((const TUint8*)beg, end2-beg);
		if ((key=iKeyboard.GetScanCode(name))!=KErrNotFound)
			pCombination->AddKey((TStdScanCode)key);
		
		if (beg == end2 || *end2++ != ',') 
			break;		
		beg = end2;
		}
	return iControlHotKeys.Append(pCombination);
	}
