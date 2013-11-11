// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// wins\inc\gui.h
// 
//

#ifndef __EX_GUI_H__
#define __EX_GUI_H__

#define _CRTIMP			// we want to use the win32 static runtime library

#include "platform.h"
#include <kernel/win32/property.h>
#include "nk_priv.h"
#include "vwins.h"
#include <e32keys.h>
#include <stdlib.h>
#include <videodriver.h>
#include <dispchannel.h>

GLREF_C const char* skipws(const char* aPtr);
GLREF_C const char* skiptok(const char* aPtr);

// keyboard mapping

const TUint KKeyExtendedBit=0x100;
const TUint KKeyNormalBits=0xFF;
const TInt KMaxExtendedKey=0x5d;
const TInt KExtendedKeyBase=0x1c;
const TInt KStandardKeyMapSize = 89;
const TInt KExtendedKeyMapSize = KMaxExtendedKey + 1 - KExtendedKeyBase;
const TUint KMaxHotKeyCombinationLength=10;
const TInt KDatFileVersion= 1000;

enum TEmulCommand
	{
	ENoCommand,
	EKey,
	ENextConfig,
	ESelectConfig,
	};

class VirtualKey
	{
public:
	VirtualKey(const TInt aCommandData, TEmulCommand aCommand);
	virtual TBool Contains(TInt aX, TInt aY) const = 0;
	virtual void Draw(HDC aHdc,COLORREF aColor) const =0;
	inline TInt Value() const {return iData;};
	inline TEmulCommand Command() const {return iCommand;};
private:
	TEmulCommand iCommand;
	TInt iData;
	};

typedef TStdScanCode KeyArray[KMaxHotKeyCombinationLength];
class KeyCombination
	{
public:
	KeyCombination(const TInt aCommandData, TEmulCommand aCommand);
	TBool AddKey(TStdScanCode);
	TBool CheckCombinationPressed();

	TEmulCommand iCommand;
	TInt iData;
private:
	KeyArray iCombination;
	};

class VKRect : public VirtualKey
	{
public:
	VKRect(const TInt aCommandData, TEmulCommand aCommand, TInt aX, TInt aY, TInt aWidth, TInt aHeight);
	// From VirtualKey
	TBool Contains(TInt aX, TInt aY) const;
	virtual void Draw(HDC aHdc,COLORREF aColor) const;
private:
	TInt iLeft,iTop,iRight,iBottom;
	VKRect();
	};



class Alias
	{
public:
	Alias();
//
	TInt Add(const TDesC8& aAlias, const TDesC8& aName);
	const TDesC8* operator[](const TDesC8& aAlias);
private:
	struct SEntry
		{
		TDesC8* iAlias;
		TDesC8* iName;
		};
	static TBool Compare(const SEntry& aLhs, const SEntry& aRhs);
private:
	RArray<SEntry> iMap;
	};

// GUI settings

const TInt KPowerTimerId=1;

const TInt KRomMajorVersionNumber=1;
const TInt KRomMinorVersionNumber=0;
const TInt KRomBuildVersionNumber=1;
//
const TInt KScreenOffsetX=90;
const TInt KScreenOffsetY=51;
const TInt KScreenWidth=640;
const TInt KScreenHeight=240;
const TInt KWinPosX=10;
const TInt KWinPosY=10;

const TInt KCompositionBuffers=2;	// Default to double-buffering
const TInt KRefreshRateHz=30;	// Default to 30fps

const TInt KMaxNameSize=80;	//size of window title string

//
// window style
const DWORD KInvisibleControlWinStyle=WS_CAPTION|WS_CLIPCHILDREN|WS_MINIMIZEBOX|WS_OVERLAPPED|WS_SYSMENU;
const DWORD KControlWinStyle=KInvisibleControlWinStyle|WS_VISIBLE;

const DWORD KInvisibleWinStyle=WS_CAPTION|WS_CLIPCHILDREN|WS_OVERLAPPED|WS_HSCROLL|WS_VSCROLL|WS_SIZEBOX|
	WS_MAXIMIZEBOX|WS_MINIMIZEBOX|WS_SYSMENU;
const DWORD KWinStyle=KInvisibleWinStyle|WS_VISIBLE;
//
// Scaling factors to use if PhysicalScreenWidth or Height are not specified
const TReal KDefaultPixelsToTwipsX=11.90625;
const TReal KDefaultPixelsToTwipsY=11.9083333334;
//
const TInt KHeapSize=0x2000;
const TInt KWindowServerHeapSize=0x40000;
const TInt KKeyDown=0x80;
//
const TInt KLedTop=45;
const TInt KLedLeft=36;
const TInt KLedSize=14;
const TInt KLedGap=5;
const TBool KLedVertical=ETrue;

GLREF_C TInt MultiProperty(TInt (*aHandler)(TAny* aObj, const char*), TAny* aPtr, const char* aProperty);

const TInt KSecureLedColor = 0xFFFF;

class DWinsKeyboard : public DBase
	{
public:
	DWinsKeyboard();
	TInt Init(TInt aId);
//
	TInt ScanCodeToRemappedKey(TInt aScanCode);
	static TInt ScanCodeToStandardKey(TInt aScanCode);
//
	TInt GetEPOCKeyCode(const TDesC8& aStr);
	TInt GetScanCode(const TDesC8& aStr);
private:
	static TInt ScanCodeToKey(TInt aScanCode, const TUint8* aStandardMap, const TUint8* aExtendedMap);
	TInt DefineAlias(const char* aValue);
	static TInt DoDefineAlias(TAny* aObj, const char* aValue);
	TInt MapKey(const char* aValue);
	static TInt DoMapKey(TAny* aObj, const char* aValue);
private:
	TUint8 iStandardKeyMap[KStandardKeyMapSize];
	TUint8 iExtendedKeyMap[KExtendedKeyMapSize];
	TUint8 iAltStandardKeyMap[KStandardKeyMapSize];
	TUint8 iAltExtendedKeyMap[KExtendedKeyMapSize];
	TUint8 iCtrlStandardKeyMap[KStandardKeyMapSize];
	TUint8 iCtrlExtendedKeyMap[KExtendedKeyMapSize];
	Alias iAliasedKeys;
	};

struct TWindowState
	{
public:
	WINDOWPLACEMENT iWinPlace;
	TInt iXoffset;
	TInt iYoffset;
	TEmulatorFlip iFlipstate;
	};

class DScreenProperties;
class TViewport 
	{
public:
	TViewport();
	TViewport(DScreenProperties* aScreenProps);
	~TViewport();
		
	void UpdateScrollBarH(HWND aHwnd);
	void UpdateScrollBarV(HWND aHwnd);
	
	TInt GetMaxWindowWidth() const;
	TInt GetMaxWindowHeight() const;
	TInt GetMaxWidth() const;
	TInt GetMaxHeight() const;
	
	TInt GetViewportOffsetX() const;
	TInt GetViewportOffsetY() const;
		
	void SetViewportWidth(TInt aWidth);
	void SetViewportHeight(TInt aHeight);
	TInt GetViewportWidth() const;
	TInt GetViewportHeight() const;
	
	void ScrollToY(TInt aPosition,HWND aHwnd);
	void ScrollToX(TInt aPosition, HWND aHwnd);
	
	void UpdateChildPos(HWND aHwnd);
		
private:
	
	void SetViewportOffsetX(TInt aOffset);
	void SetViewportOffsetY(TInt aOffset);
	
	/**
	The DScreenProperties object which owns this viewport
	*/
	DScreenProperties* iScreenProps;
	
	/**
	Width in pixels of the client area of the window through which the emulator is being viewed
	*/
	TInt iViewportWidth;
	/**
	Height in pixels of the client area of the window through which the emulator is being viewed
	*/
	TInt iViewportHeight;
	/**
	X Offset of viewport edge from emulator image ie. How far viewport is translated
	*/
	TInt iViewportOffsetX;
	/**
	Y Offset of viewport edge from emulator image ie. How far viewport is translated
	*/
	TInt iViewportOffsetY;
	
	};

class DScreenProperties
	{
friend class TViewport;
public:
	DScreenProperties();
	~DScreenProperties();
	TInt SetupProperties(TInt aConf, TInt aScreen);
	TWindowState GetWindowState();
public:
	TInt iScreenWidth;
	TInt iScreenHeight;
	TInt iMaxScreenWidth;
	TInt iMaxScreenHeight;
    TInt iXYInputWidth;
    TInt iXYInputHeight;
    TInt iXYOrientation;
	TInt iPhysicalScreenWidth;
	TInt iPhysicalScreenHeight;
	TInt iMaxPhysicalScreenWidth;
	TInt iMaxPhysicalScreenHeight;
	TInt iScreenOffsetX; 
	TInt iScreenOffsetY;
	TUint iColorDepth; 			///<In emulator this is a bitfield of the available modes. All other aspects of each mode are identical.
	TInt iDisplayContrast;
	TEmulatorFlip iScreenRotation;
	CHAR iFasciaFileName[MAX_PATH];
	HWND iWindowHandle;
	WINDOWPLACEMENT iWinPlace; ///<Used to store the window's WINDOWPLACEMENT struct.
	RECT iWinRect;
	TBool iScreenOff;
	TInt  iCurrentMode;		   ///<Indicates the current mode when multiple modes are supported
	TInt  iMaxModes;
	TInt  iModeDepths[KEmulMaxNumModes];
	TInt iCompositionBuffers;
	TInt iRefreshRateHz;
	/**
	The viewport owned by the screen
	*/
	TViewport iViewport;

	};
	

class DDisplayChannel;

class DWinsUi : public DWinsUiBase
	{
public:
	DWinsUi();
	TInt Create(TInt aId);
//
	TInt GetVirtualKey(TEmulCommand& aCommand, TInt aX, TInt aY);
	void SetVirtualKey(const TBool aProcessing, const TInt aCommandData, const TEmulCommand aCommand);
	TBool WasVirtualKey(TInt& aCommandData, TEmulCommand& aCommand);
	void TranslateMouseCoords(const TEmulatorFlip aFlipState, const TInt aX, const TInt aY, const TInt aRegionWidth, const TInt aRegionHeight, TInt& aNewX, TInt& aNewY);

	//
	TBool OnScreen(TInt aScreen, TInt aX, TInt aY) const;
	TBool OnDigitizer(TInt aX, TInt aY) const;
	TBool MultiTouchEnabled() const;
	TBool GCEEnabled() const;
	TInt MultiTouchPressureStep() const;
	TInt MultiTouchProximityStep() const;
//private:
	void ScreenInfo(TScreenInfoV01& aInfo);
	TBool VideoInfo(TInt aDeviceNumber, TVideoInfoV01& aInfo);
	TBool VideoInfo(TInt aDeviceNumber,TInt aModeNumber, TVideoInfoV01& aInfo);
	TBool VideoInfoForDisplayDriver(TInt aDeviceNumber,TInt aModeNumber, TVideoInfoV01& aInfo, TBool aRealWidthAndHeight = EFalse);
//
	TInt SetupProperties(TInt aId);
	TInt DefineVirtualKey(const char* aValue);
	static TInt DoDefineVirtualKey(TAny* aObj, const char* aValue);
	TInt DefineEmulatorControl(const char* aValue);
	static TInt DoDefineEmulatorControl(TAny* aObj, const char* aValue);

	static TInt DoDefineEmulatorControlHotKey(TAny* aPtr, const char* aValue);
	TInt DefineEmulatorControlHotKey(const char* aValue);
//	implementing DWinsUiBase
	TUint ColorDepth(TInt aScreenNumber);
	TInt SetFlip(TEmulatorFlip aFlip, TInt aScreenNumber);
	void Info(TVariantInfoV01& aInfo);
	HWND HWnd();
	void SwitchDisplay(TBool aSecure);
	TInt NumberOfScreens();
	virtual TInt SetDisplayChannel(TInt aScreenNumber, DDisplayChannel* aDisplay);
	TInt SetDisplayChannelImpl(TInt aScreenNumber, DDisplayChannel* aDisplay);
public:
	DWinsKeyboard iKeyboard;
//
	TInt iLedSize;
	TInt iLedGap;
	TInt iLedOffsetX;
	TInt iLedOffsetY;
	TBool iLedVertical;
	TBuf8<0x10> iPointerType;
	TXYInputType iXYInputType;
	CHAR iSysIniFileName[MAX_PATH];
	WINDOWPLACEMENT iWinPlace;
    RPointerArray<VirtualKey> iVirtualKeys;
	RPointerArray<KeyCombination> iControlHotKeys;
	TInt iDigitizerWidth;
	TInt iDigitizerHeight;
	TInt iDigitizerOffsetX;	//offset from the epoc screen 0!
	TInt iDigitizerOffsetY;//offset from the epoc screen 0!
	TBool iDigitizerEnabled;
	CHAR iWindowTitle[KMaxNameSize+1];
	TBool iDisplayVersionInfo;
	TInt aId;			//configuration number;
	TInt iInitialFlipMsg;
	TBool iProcessingVirtualKey;
	TUint iFakedVirtualKey;
	TEmulCommand iVirtualKeyCommand;
	RPointerArray<DScreenProperties> iScreens;
private:
	TBool iMultiTouchEnabled;
	TBool iGCEEnabled;
	TInt iMultiTouchProximityStep;
	TInt iMultiTouchPressureStep;
	};

struct TBufferAddressA
	{
	DChunk* iChunk;
	TAny* iAddress;
	};
	
class TScreenBuffer
	{
public:	
	RPointerArray<TAny> iFrameBuffers;
	RPointerArray<TBufferAddressA> iMemChunks;
	TInt 	iDisplayBufferOffset;    //offset to pixel data in current mode for display buffer. Mode of display buffer doesn't change we presume!
	};


// Chunk cleanup object - used to clean up the process/addr table when the process goes away.
class TChunkCleanup : public TDfc
    {
public:
    TChunkCleanup(DProcess* aProcess, TInt aScreenNumber);
    void Cancel();
    inline void SetIndex(TUint aIndex) { iIndex = aIndex; }
private:
    static void ChunkDestroyed(TChunkCleanup* aSelf);
private:
    DProcess* iProcess;
    TInt      iScreenNumber;
    TInt	  iIndex;
    };


// Entry to hold the process/disp.memory address, as we need to find the address of the display
// memory when requested. A new entry would be added when a process first requests the address of display
// memory. When the process dies, we remove the process entry (by way of a DFC for a chunk that is 
// created on behalf of that process).
class TProcessAddrEntry 
    {
public:
	TProcessAddrEntry(DProcess *aProcess, TUint8* aAddress);
public:
	DProcess* iProcess;
	TUint8* iAddress;
	};



class TBufferSet
	{
public:
	TAny* iDisplayBuffer;		//current display buffer
	BITMAPV4HEADER iInfo;		// This can be cast to a BITMAPINFOHEADER
	TInt           iDisplayDriverCount;
	TScreenBuffer  iScreenBuffer;
	TScreenBuffer  iDsaBuffer;
	RArray<TProcessAddrEntry> iProcAddrTable;
	TInt iDisplayState;
	TInt iStateChangeCount;
	RDisplayChannel::TBufferFormat iBufferFormat;
	DDisplayChannel* iDisplayChannel;
	};

class DMasterIni : public DBase
	{
public:
	TInt SetupProperties();
	TInt Create();
	static TInt DoHalFunction(TAny* aPtr, TInt aFunction, TAny* a1, TAny* a2);
	TInt HalFunction(TInt aDeviceNumber, TInt aFunction, TAny* a1, TAny* a2);
	static TInt DoXYHalFunction(TAny* aThis, TInt aFunction, TAny* a1, TAny* a2);
	TInt XYHalFunction(TInt aFunction, TAny* a1, TAny* a2);
	static TInt DoMouseHalFunction(TAny* aThis, TInt aFunction, TAny* a1, TAny* a2);
	TInt MouseHalFunction(TInt aFunction, TAny* a1, TAny* a2);
	static TInt DoKbdHalFunction(TAny* aThis, TInt aFunction, TAny* a1, TAny* a2);

	void InitBitmapHeader(DScreenProperties& aScreenProperties, LPBITMAPV4HEADER aInfo);
	void InitBufferFormat(DScreenProperties& aScreenProperties, RDisplayChannel::TBufferFormat& aBufferFormat);
	TInt AllocateFrameBuffers(TInt aScreenNumber, TInt aCount, TInt aSize);
	void ReleaseFrameBuffers(TInt aScreenNumber);

	
	TInt DisplayMemoryAddress(TInt aScreenNumber, TInt& aAddress);
	TInt DisplayMemoryHandle(TInt aScreenNumber, TInt& aHandle);
	
	void ProcessDeletedDFC();

	void SetDisplaySize(TInt aDisplayNumber, TInt aWidth, TInt aHeight);
	void SetBufferFormat(TInt aDisplayNumber, TUint aAgregatSize, RDisplayChannel::TPixelFormat aPixelFormat);
	void SetImageSize(TInt aScreenNumber);

public:
	//not set or used: TInt iConfigurationCount;
	RPointerArray<DWinsUi> iSystemInis;	//Indexed by config
	RArray<TBufferSet>     iBufferSet;
	// Fast mutex used to control access to the process/address table in the 
	// TBufferSet. 
	NFastMutex iLock;
	TUint iMaxSizeInBytes;
	static const RDisplayChannel::TPixelFormat iSupportedPixelFormatTable[];
	static const TInt iSupportedPixelFormatTableSize;
private:
	TInt NumberOfResolutions(TInt aDeviceNumber, TAny* a1, TAny* a2);
	TInt SpecificScreenInfo(TInt aDeviceNumber, TAny* a1, TAny* a2);
	TInt CurrentScreenInfo(TInt aDeviceNumber, TAny* a1, TAny* a2);
};


GLREF_C TInt CompareI(const TDesC8& aLhs, const TDesC8& aRhs);

// user-defined messages ...
// flip the emulator window
#define WM_FLIP_MESSAGE 0x7FFF
// emulator window power state
#define WM_EMUL_POWER_ON 0x7FFE

//redraw client window from the internal buffer
#define WM_REDRAW_WINDOW 	(WM_USER + 1)

// Set the display frame buffer for this window to lParam
#define WMU_SET_DISPLAY_BUFFER	(WM_USER + 2)

// Set the display size for this window to (wParam,lParam)
// Rotation (flip) is ignored. Current display state is used.
// Configurations are searched to find a match, message is ignored if none found.
#define WMU_SET_DISPLAY_SIZE	(WM_USER + 3)

//modifies the header of the bitmap that is getting painted, updating the size
#define WMU_SET_BUFFER_FORMAT	(WM_USER + 4)

enum TGuiPanic
	{
	EGuiGetModuleHandle=21,
	EGuiRegisterWindow=22,
	EGuiKernelWindowCreate=23,
	EGuiRegisterChildWindow=24,
	EGuiKernelChildWindowCreate=25,
	EGuiCreateBitmap=26,
	EGuiChildWinProc=27,
	EGuiEnterCS=28,
	EGuiLeaveCS=29,
	EGuiStandbyWhilstOff=30,
	EGuiResetPowerEvent=31,
	EGuiSetPowerEvent=32,
	EGuiWaitPowerOnFailed=33,
	EGuiNoDisplayChannel=34,
	EGuiVideoInfoUnavailable=35,
	EGuiInvalidMultiTouch=36
	};

GLREF_C void Fault(TGuiPanic aPanic);

#endif
