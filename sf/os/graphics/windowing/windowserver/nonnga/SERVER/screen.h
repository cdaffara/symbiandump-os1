// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the screen class (was in scrdev.h)
// 
//

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <e32std.h>
#include <e32base.h>
#include <w32std.h>	
#include "w32cmd.h"
#include "wstypes.h"

#include <Graphics/WSGRAPHICDRAWERINTERFACE.H>

class CWsDirectScreenAccess;

class CWsOffScreenBitmap;
class CWsWindowGroup;
class CWsWindow;
class CWsRootWindow;
class CWsClientWindow;
class CDebugBar;
class CScreenRedraw;
class CWsSpriteManager;

class CScreen : public CBase, public MWsScreen, public MWsScreenConfig, public MWsFrontBuffer
	{
	enum {EDefaultShadowX=2};
	enum {EDefaultShadowY=2};
public:
	enum {EGetScanLineBufLen=0x100};	// Buffer of for returning result of GetScanLine
	enum {EMaxNumModes=EColorLast-2};		// Number of available devices in the GDI enum TDisplayMode
	enum TDisplayScreen
		{
		EBlankScreenOnRotation = 0x1,
		EAutoClear = 0x2,
		EBltOffScreenBitmap = 0x04,			// Refrain from clearing window to default background color
		};
	/**
	The fallback map is an array of 32 bit integers sufficiently large to store a single bit for any
	possible orientation and size of the screen, with the pixel rows padded to the end of the int.
	Andy - This is only used by the screen redraw, so it should be in the screen redraw, not the screen.
	Also, it doesnt need its own region, since the root window has the correct region already.
	*/
	class CFallbackMap : public CBase
		{
	public:
		static CFallbackMap * NewL(CScreen* aScreen);
		~CFallbackMap();
		
		void Prepare();
		TBool FillRegion(const TRegion& aRegion);
		TBool FillRect(const TRect& aRect);
		TInt Count() const;
		const TRect * Rect() const;
		const RRegion * Region() const;

	private:	
		CFallbackMap(CScreen* aScreen);
		void ConstructL();
		
	public:
		CScreen* iScreen;
		RRegionBuf<1> iRegion;
		TInt iCount;
		TInt iMapSize; // size in padded TInts
		TInt * iMap;
		};
public:
	CScreen();
	~CScreen();
	void ConstructL( const TRect& aDigitiserArea, TInt aScreenNumber );
	void AbortAllDirectDrawing(RDirectScreenAccess::TTerminationReasons aReason);
	void AddDirect(CWsDirectScreenAccess& aDirect);
	void RemoveDirect(CWsDirectScreenAccess& aDirect);
	void AbortDSAs(RDirectScreenAccess::TTerminationReasons aReason,TSglQue<CWsDirectScreenAccess>& aDirects);
#if defined(_DEBUG)
	TBool IsDirectOnQueue(const CWsDirectScreenAccess* aDirect);
#if defined(__WINS__)
	void UpdateOffScreenBitmap();
#endif
#endif

	inline CWsWindowGroup* FocusWindowGroup();
	void KillForegroundSession();
	void ResetFocus(CWsWindowGroup *aClosingWindow);
	void SetShadowVector(const TPoint &aShadowShift);
	inline TPoint ShadowVector();
	inline TBool BlankScreenOnRotation();
	inline TBool AutoClear();
	void RemoveFromDefaultOwningList(CWsWindowGroup *aDestroyedGroup);
	void SetDefaultOwningWindow(CWsWindowGroup *aGroup);
	inline CWsWindowGroup *DefaultOwningWindowGroup();

	void CheckDisplayMode(TBool aUpdateNow);
	CFbsScreenDevice *ScreenDevice();
	inline CWsRootWindow* RootWindow() const;
	inline TInt ScreenNumber() const;
	void Update();
	void Update(const TRegion &aRegion);
	inline TDisplayMode CurrentDisplayMode();
	inline TDisplayMode DefaultDisplayMode();
	inline TDisplayMode DefaultDisplayMode(const TInt aMode) const;
	TDisplayMode FirstDefaultDisplayMode() const;
	//
	inline CFbsBitGc *ScreenGdi();
	void MaxNumColors(TInt& aColors,TInt& aGrays);
	TInt ColorModesFlag();
	inline void GetFadingParams(TUint8& aBlackMap,TUint8& aWhiteMap) const;
	inline void SetFadingParams(TUint8 aBlackMap,TUint8 aWhiteMap);
	void GetScanLine(const TWsSdCmdGetScanLine *aGetScanLine);

	TBool SetScreenModeEnforcement(TInt aMode);
	inline TInt ScreenSizeMode() const;
	void SetPointerCursorArea(TInt aMode,const TRect& aRect);
	inline TRect GetPointerCursorArea(TInt aMode) const;
	CFbsBitGc::TGraphicsOrientation Orientation();
	void CycleDisplaySize();
	void UpdateOrientation();
	void doSetScreenMode(TInt aMode);
	inline TSize CurrentScreenSize() const;
	TRect DrawableArea() const;
	void CycleOrientation();
	TClientPanic SetModeRotation(TInt aMode,CFbsBitGc::TGraphicsOrientation aRotation);
	void LoadScreenSizesL(TSize aScreenSize);
	void LoadScreenSizeProperties();
	void LoadScreenScale();
	void SetDigitiserAreas(const TSize& aScreenSize,const TRect& aDigitiserArea);
	inline TInt NumScreenSizeModes() const;
	inline const TSizeMode& ScreenSizeModeData() const;
	inline const TSizeMode& ScreenSizeModeData(TInt aMode) const;
	inline TPoint ScreenModeOrigin(TInt aMode);
	inline TPoint CurrentScreenModeOrigin();
	inline TSize CurrentScreenModeScale();
	inline TPoint CurrentScreenModeScaledOrigin();
	inline TPoint ScreenModeScaledOrigin(TInt aMode);
	inline TScreenModeEnforcement SizeEnforcementMode() const;
	void GetScreenSizeAndRotation(TPixelsTwipsAndRotation &aSar, TInt aScreenMode);
	void GetScreenSizeAndRotation(TPixelsAndRotation &aSar, TInt aScreenMode);
	inline TSize GetScreenScale(TInt aMode) const;
	inline void SetCurrentScreenModeAttributes(const TSizeMode &aModeData);
	inline TSize GetPhysicalScreenSize();
	inline void SetPhysicalScreenSize();
	inline TBool ShouldSwapWidthAndHeightOffScBitmap(TInt aOldRotation);
	inline TInt Modes();
	CFbsDevice * DrawDevice();
	CWsOffScreenBitmap* OffScreenBitmap();
	void FreeOffScreenBitmap();
	void UpdateGcs();
	inline void SetScalingFactor();
	void UpdateOffScreenBitmapGc(const TBool aSwapWidthAndHeight);
	TPoint PhysicalToLogical(TPoint aPhysicalPt);
	void IncContrast();
	void DecContrast();
	void IncBrightness();
	void DecBrightness();
	inline TBool IsValidScreenSizeMode(TInt aMode);
	TInt GetScreenSizeModeListL();
	void SetDisplayModeFromName(const TDesC& aModeName, TDisplayMode& aMode);
	CFbsDevice* GetFbsDevice();
	// called by CWsGc when it is implementing MWsGc
	const TTime& Now() const;
	void ScheduleAnimation(const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop);
	// redraw scheduling code
	void AddRedrawRegion(const TRegion& aRegion, TBool aSchedule = ETrue, TRedrawDepth aDepth = ERedrawAll);
	void DoRedrawNow();
	void ScheduleRegionUpdate(const TRegion* aDefinitelyDirty);
	void EndClientBuffer();
	// implementing MWsScreen
	void OnAnimation();
	void Redraw();
	TBool RedrawInvalid(const TArray<TGraphicDrawerId>& aInvalid);
	inline TBool BackLightFlag();
	// implementing interface extension
	TAny* ResolveObjectInterface(TUint aTypeId);
	// implementing MWsScreenConfig interface	
	TDisplayMode DisplayMode() const;
	TSize SizeInPixels() const;
	TSize ScreenModeSizeInPixels() const;
	TInt Stride() const;
	CFbsBitGc::TGraphicsOrientation Orientation() const;
	TInt SizeMode() const;
	TSize ScalingFactor() const;
	TPoint Origin() const;
	TPoint ScaledOrigin() const;
	// implementing MWsFrontBuffer
	const TAny* GetBits();
	CFbsBitGc* GetBitGc();
	CFbsBitGc* GetBitGcCurrent();
	TInt SetBitGc(CFbsBitGc* aBitGc);
	TInt SetBitGc(CFbsBitGc* aBitGc, TBool aInvalidateScreen);
	inline TBool IsOffScreenBitmap();
	inline TBool MaskingEnabled();
	inline TBool BltOffScreen();
	void RecalcHeights();
	void DiscardAllSchedules();
	inline CDebugBar * DebugBar();
	inline CFallbackMap * FallbackMap();
	inline CWsSpriteManager* SpriteManager();
	inline MWsFader* Fader();
	TBool IsUpdatePending();
	void DSARegionSyncStart( CWsDirectScreenAccess& aDSA );
	void DSARegionSyncOver( CWsDirectScreenAccess& aDSA );
	TBool HasVisibleDirectOnQueue();
	TBool IsDSAClientWindow( const CWsClientWindow* aWin ) const;
	void AcceptFadeRequest( CWsWindow* aWin, TBool	aIsFaded, TBool aIsBehind, TBool aIncludeChildren = EFalse );

private:
	enum TSyncConsts
		{
		KDSAAbortingImmediateRespAwaitFrameMicrosec = 80000		// 0.08 sec
		};
private:
	void ChangeDisplayModeForAllOffScreenBitmap(TBool aSwapWidthAndHeight=EFalse);
	void SetInitialScreenSizeMode();
	CWsWindowGroup* FindNewFocus(CWsRootWindow* aRootWindow);
	void CreateFallbackMapL();
	void CreateScreenDeviceL();
	TBool DoCreateScreenDevice(TDisplayMode aScreenMode);
private:
	enum
		{
		eTimerCreated=0x1,
		};
	enum TDsaDrawState
		{
		EDsaDrawStateIdle,
		EDsaDrawStateDrawing
		};
private:
	TSglQue<CWsDirectScreenAccess> iDirects;
	RTimer iTimer;
	TRequestStatus iTimerStatus;
	TUint iFlags;

	CScreenRedraw* iRedraw;

	CWsRootWindow* iRootWindow;
	CWsWindowGroup *iCurrentFocus;
	CWsWindowGroup *iDefaultOwningWindow;
	CWsOffScreenBitmap* iOffScreenBitmap;
	TPoint iShadowShift;

	CFbsScreenDevice *iScreenDevice;
	CFbsBitGc *iScreenGdi;

// Size and roation members
	RPointerArray<TSizeMode> *iModes;
	TInt iScreenSizeMode;
	TScreenModeEnforcement iSizeEnforcementMode;
	TSize iPhysicalScreenSize;
	TInt iScreenNumber;
	TInt iMaxContrast;
	TInt iMaxBrightness;
	TInt iNumScreenSizeModes;
	TInt iNumSupportedScreenSizeModes;
	TBool iBackLightFlag;
	
	// used by MWsFrontBuffer to redirect screen output
	CFbsBitGc* iRedirectGc;

	TDsaDrawState iDsaDrawState;
	CDebugBar* iDebugBar;
	CFallbackMap* iFallbackMap;
	TUint8 iBlackMap;
	TUint8 iWhiteMap;
	CWsSpriteManager* iSpriteManager;
	MWsFader* iFader;
	};



//
// inlines			//
//

//
// CScreen
//
inline void CScreen::Update(const TRegion &aRegion)
	{iScreenDevice->Update(aRegion);}
inline TPoint CScreen::ShadowVector()
	{return(iShadowShift);}
inline CWsWindowGroup* CScreen::FocusWindowGroup()
	{return(iCurrentFocus);}
inline TBool CScreen::BlankScreenOnRotation()
	{return(iFlags&EBlankScreenOnRotation);}
inline TBool CScreen::AutoClear()
	{return(iFlags&EAutoClear);}
inline CWsWindowGroup* CScreen::DefaultOwningWindowGroup()
	{return(iDefaultOwningWindow);}

inline CWsRootWindow* CScreen::RootWindow() const
	{return iRootWindow;}
inline TInt CScreen::ScreenNumber() const
	{return iScreenNumber;}

inline CFbsBitGc *CScreen::ScreenGdi()
	{return(iScreenGdi);}
inline TDisplayMode CScreen::CurrentDisplayMode()
	{return iScreenDevice->DisplayMode();}
inline TDisplayMode CScreen::DefaultDisplayMode()
	{return iScreenDevice->DisplayMode();}
//
inline TInt CScreen::ScreenSizeMode() const
	{return(iScreenSizeMode);}
inline TRect CScreen::GetPointerCursorArea(TInt aMode) const
	{return (*iModes)[aMode]->iPointerCursorArea;}
inline TSize CScreen::CurrentScreenSize() const
	{return (*iModes)[iScreenSizeMode]->iScreenSize;}
inline TInt CScreen::NumScreenSizeModes() const
	{return iNumScreenSizeModes;}
inline const TSizeMode& CScreen::ScreenSizeModeData() const
	{return *(*iModes)[iScreenSizeMode];}
inline const TSizeMode& CScreen::ScreenSizeModeData(TInt aMode) const
	{return *(*iModes)[aMode];}
inline TPoint CScreen::ScreenModeOrigin(TInt aMode)
	{return (*iModes)[aMode]->iOrigin;}
inline TPoint CScreen::CurrentScreenModeOrigin()
	{return (*iModes)[iScreenSizeMode]->iOrigin;}
inline TSize CScreen::CurrentScreenModeScale()
	{return (*iModes)[iScreenSizeMode]->iScreenScale;}
inline TPoint CScreen::CurrentScreenModeScaledOrigin()
	{return (*iModes)[iScreenSizeMode]->ScaledOrigin();}
inline TPoint CScreen::ScreenModeScaledOrigin(TInt aMode)
	{return (*iModes)[aMode]->ScaledOrigin();}
inline TSize CScreen::GetScreenScale(TInt aMode) const
	{return (*iModes)[aMode]->iScreenScale;}
inline TDisplayMode CScreen::DefaultDisplayMode(const TInt aMode) const
	{return (*iModes)[aMode]->iDefaultDisplayMode;}
inline TSize CScreen::GetPhysicalScreenSize()
	{return iPhysicalScreenSize;}
inline void CScreen::SetScalingFactor()
	{iScreenDevice->SetScalingFactor((*iModes)[iScreenSizeMode]->iOrigin,(*iModes)[iScreenSizeMode]->iScreenScale.iWidth,(*iModes)[iScreenSizeMode]->iScreenScale.iHeight,1,1);}
inline void CScreen::SetCurrentScreenModeAttributes(const TSizeMode &aModeData)
	{*(*iModes)[iScreenSizeMode]=aModeData;}
inline void CScreen::SetPhysicalScreenSize()
	{TInt temp=iPhysicalScreenSize.iWidth;iPhysicalScreenSize.iWidth=iPhysicalScreenSize.iHeight;iPhysicalScreenSize.iHeight=temp;}
inline TScreenModeEnforcement CScreen::SizeEnforcementMode() const
	{return iSizeEnforcementMode;}
inline TBool CScreen::IsValidScreenSizeMode(TInt aMode)
	{return (aMode>=0 && aMode<iModes->Count() && (*iModes)[aMode]!=NULL);}
inline TBool CScreen::BackLightFlag()
	{return iBackLightFlag;}
inline TBool CScreen::IsOffScreenBitmap()
	{return iOffScreenBitmap!=NULL;}

inline CDebugBar * CScreen::DebugBar()
	{return iDebugBar;}
inline CScreen::CFallbackMap * CScreen::FallbackMap()
	{return iFallbackMap;}
	
inline void CScreen::GetFadingParams(TUint8& aBlackMap,TUint8& aWhiteMap) const
    {aBlackMap=iBlackMap;aWhiteMap=iWhiteMap;}
inline void CScreen::SetFadingParams(TUint8 aBlackMap,TUint8 aWhiteMap)
	{iBlackMap=aBlackMap;iWhiteMap=aWhiteMap;}
inline CWsSpriteManager* CScreen::SpriteManager()
	{return iSpriteManager;}
inline MWsFader* CScreen::Fader()
	{return iFader;}
inline TBool CScreen::BltOffScreen()
	{return iFlags&EBltOffScreenBitmap;}
#endif
