// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <graphics/surface.h>
#include <hal_data.h>
#include <graphics/wsscene.h>
#include <graphics/wsdisplaypolicy.h>

#include "W32STD.H"
#include "w32cmd.h"
#include "wstypes.h"
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>
#include "panics.h"

#include "wsdisplaychangeao.h"


class CWsDirectScreenAccess;

class CWsWindowGroup;
class CWsWindow;
class CWsRootWindow;
class CWsClientWindow;
class CDebugBar;
class CScreenRedraw;
class CWsSpriteManager;
class CWindowElementSet;
class CActiveComposer;
class CRegisteredSurfaceMap;
class MWsScene;
class MWsElement;
class MWsScreenDevice;
class MWsDisplayMapping;
class MWsDisplayControl;
class MWsTextCursor;
class CGraphicsDeviceMap;
class TDisplayConfiguration;
#if defined(__WINS__) && defined(_DEBUG)
class CDebugOsbWin;
#endif

enum TAnimType
	{
	EWindowAnim			= 0x01,
	ESpriteAnim			= 0x02,
	ETextCursor			= 0x04,
	EWindowSprite   	= 0x08,
	EFloatingSprite 	= 0x10,
	EFloatingSpriteAnim = 0x20,
	ECrpAnim			= 0x40,
	};

_LIT(KWSERVIniFileVarChangeTracking, "CHANGETRACKING");

class CScreen : public CBase, public MWsScreen, public MWsScreenConfigList, public MWsScreenConfig, public MWsWindowTree
	{
public:
	enum {EGetScanLineBufLen=0x100};	// Buffer of for returning result of GetScanLine
	enum TDisplayScreen
		{
		EBlankScreenOnRotation = 0x1,
 		EAutoClear = 0x2,
 		EHasDynamicSizeModes = 0x04,
		EChangeTracking = 0x8,
		};
	
	/**
	The fallback map is an array of 32 bit integers sufficiently large to store a single bit for any
	possible orientation and size of the screen, with the pixel rows padded to the end of the int.
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
		TInt Resize(const TSize& aSize);

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
	void ConstructL( const TRect& aDigitiserArea, TInt aScreenNumber);
	void AbortAllDirectDrawing(RDirectScreenAccess::TTerminationReasons aReason);
	void AddDirect(CWsDirectScreenAccess& aDirect);
	void RemoveDirect(CWsDirectScreenAccess& aDirect);
	void AbortDSAs(RDirectScreenAccess::TTerminationReasons aReason,TSglQue<CWsDirectScreenAccess>& aDirects);
	void ReleaseDsaScreenDevice();
	void AcquireDsaScreenDeviceL();
	void CreateDsaScreenDeviceIfSupportedL(TDisplayMode aScreenMode);
	TBool DoCreateDsaScreenDevice(TDisplayMode aScreenMode);
#if defined(_DEBUG)
	TBool IsDirectOnQueue(const CWsDirectScreenAccess* aDirect);
#endif

	inline CWsWindowGroup* FocusWindowGroup();
	void KillForegroundSession();
	void ResetFocus(CWsWindowGroup *aClosingWindow);
	inline TBool BlankScreenOnRotation();
	inline TBool AutoClear();
	void RemoveFromDefaultOwningList(CWsWindowGroup *aDestroyedGroup);
	void SetDefaultOwningWindow(CWsWindowGroup *aGroup);
	inline CWsWindowGroup *DefaultOwningWindowGroup();
	const MWsScreenDevice& ScreenDevice() const;
	const CGraphicsDeviceMap& DeviceMap() const;	
	inline const TSurfaceId& DsaSurface() const;
	inline CWsRootWindow* RootWindow() const;
	inline TInt ScreenNumber() const;
	void UpdateDsa();
	inline TDisplayMode DefaultDisplayMode(const TInt aMode) const;
	TDisplayMode FirstDefaultDisplayMode() const;
	void MaxNumColors(TInt& aColors,TInt& aGrays);
	TInt ColorModesFlag();
	inline void GetFadingParams(TUint8& aBlackMap,TUint8& aWhiteMap) const;
	inline void SetFadingParams(TUint8 aBlackMap,TUint8 aWhiteMap);
	void GetScanLine(const TWsSdCmdGetScanLine *aGetScanLine);

	TBool SetScreenModeEnforcement(TInt aMode);
	inline TInt ScreenSizeMode() const;
	void SetPointerCursorArea(TInt aMode,const TRect& aRect);
	inline TRect GetPointerCursorArea(TInt aMode) const;
	CFbsBitGc::TGraphicsOrientation Orientation() const;
	void CycleDisplaySize();
	TBool UpdateOrientation(MWsScene::TSceneRotation* aOldRotation = NULL);
	void doSetScreenMode(TInt aMode,TBool aInsideStartup=EFalse);
	inline TSize CurrentScreenSize() const;
	TRect DrawableArea() const;
	void CycleOrientation();
	TClientPanic SetModeRotation(TInt aMode,CFbsBitGc::TGraphicsOrientation aRotation);
	void LoadScreenSizesL(TSize aScreenSize);
	void LoadScreenSizeProperties(TDisplayMode aDefaultDisplayMode);
	void SetDigitiserAreas(const TSize& aUiSize);
	inline TInt NumScreenSizeModes() const;
	inline const TSizeMode& ScreenSizeModeData() const;
	inline const TSizeMode& ScreenSizeModeData(TInt aMode) const;
	inline TScreenModeEnforcement SizeEnforcementMode() const;
	void GetScreenSizeAndRotation(TPixelsTwipsAndRotation &aSar, TInt aScreenMode);
	void GetScreenSizeAndRotation(TPixelsAndRotation &aSar, TInt aScreenMode);
	void SetCurrentScreenModeAttributes(const TSizeMode &aModeData);
	TPoint PhysicalToLogical(TPoint aPhysicalPt);
	void IncContrast();
	void DecContrast();
	void IncBrightness();
	void DecBrightness();
	inline TBool IsValidScreenSizeMode(TInt aMode) const;
	TInt GetScreenSizeModeListL();
	// called by CWsGc when it is implementing MWsGc
	const TTime& Now() const;
	void ScheduleAnimation(TAnimType aType, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop, CWsWindow* aWindow);
	TBool IsScheduled(TAnimType aType, const TRect& aRect, CWsWindow* aWindow) const;
	// redraw scheduling code
	void AddRedrawRegion(const TRegion& aRegion, TBool aSchedule = ETrue, TRedrawDepth aDepth = ERedrawAll);
	void ScheduleRender(const TTimeIntervalMicroSeconds& aFromNow);
	void DoRedrawNow();
	void RedrawNowIfPending();
	void ScheduleRegionUpdate(const TRegion* aDefinitelyDirty);
// implementing MWsScreen
	void OnAnimation(TRequestStatus* aFinished);
	void Redraw();
	TBool RedrawInvalid(const TArray<TGraphicDrawerId>& aInvalid);
	inline TBool BackLightFlag();
// implementing interface extension
	TAny* ResolveObjectInterface(TUint aTypeId);
	
// implementing MWsScreenConfig... this might be better as RS interface, but half methods are in here!
	TSize ScreenModeSizeInPixels() const;
	TInt Stride() const;
	TInt SizeMode() const;
	TSize ScalingFactor() const;
	TPoint Origin() const;
	TPoint ScaledOrigin() const;
	
// implementing MWsScreenConfigList
	TInt	GetScreenSizeModeList(RArray<TInt>&aList) const;
	TDisplayMode DisplayModeL(TInt aIndex) const;
	TSize ScreenModeSizeInPixelsL(TInt aIndex) const;
	/** Get screen size in twips for current screen size mode as defined in wsini */
	TSize ScreenModeSizeInTwipsL(TInt aIndex) const;
	CFbsBitGc::TGraphicsOrientation OrientationL(TInt aIndex) const;
	TInt AvailableOrientationsL(TInt aIndex) const;
	TSize ScalingFactorL(TInt aIndex) const;
	TPoint OriginL(TInt aIndex) const;
	TPoint ScaledOriginL(TInt aIndex) const;
	TInt ModeFlagsL(TInt aIndex) const;

//implementing MWsWindowTree
	void SendTree() const;
	
// other public methods

	TDisplayMode DisplayMode() const;
	TSize SizeInPixels() const;
	TSize SizeInTwips() const;
	TSize DSASizeInPixels() const;
	inline CWindowElementSet& WindowElements() const;
 	inline MWsElement& UiElement() const;
	void ElementAdded();
	void ElementRemoved();
	//check if the current display mode has alpha
	inline TBool HasAlpha() const;
	//list of session and surface id
	CRegisteredSurfaceMap* SurfaceMap();

	void DiscardAllSchedules();
	inline CDebugBar * DebugBar();
	inline CFallbackMap * FallbackMap();
	inline CWsSpriteManager* SpriteManager() const;
	TBool IsQuickFadeScheduled( CWsWindow* aWin ) const;
	void RemoveFromQuickFadeList( CWsWindow* aWin );
	void AcceptFadeRequest( CWsWindow* aWin, TBool	aFadeOn );
	TBool HasVisibleDirectOnQueue();
	TBool IsDSAClientWindow( const CWsClientWindow* aWin ) const;
	MWsTextCursor* RenderStageTextCursor() const;
	void ClearDsaSurface(const TRect& area, const TRgb& color);
	void PositionUiElements(const TRect& aExtent,	const TRect& aViewport);
	TInt SetConfiguration(const TDisplayConfiguration& aConfig);
	TInt UpdateConfiguration(const TDisplayConfiguration& aConfig);
	void UpdateDynamicScreenModes();
	void RecalculateModeTwips(const TDisplayConfiguration* aConfig = NULL);
	inline void IncreaseDisplaySpinner();
	inline void IncreaseConfigSpinner();
	inline TInt DisplaySpinner();
	inline TInt ConfigSpinner();
	TInt AddNotificationClient(CWsClient *aClient);
	void RemoveNotificationClient(CWsClient *aClient);
	TInt FindNotificationClient (CWsClient *aClient);
	TInt GetNotificationClients(RPointerArray<CWsClient>& aClientsArray);
	inline MWsDisplayPolicy* DisplayPolicy();
	inline MWsDisplayMapping* DisplayMapping();

	//CR1577
	inline TBool ChangeTracking() const;
	inline MWsWindowTreeObserver* WindowTreeObserver() const;
	inline MWsDrawAnnotationObserver* DrawAnnotationObserver() const;
	inline MWsWindowVisibilityNotifier* WindowVisibilityNotifier() const;
	void ScheduleWindow(CWsWindow* aWindow);
	void RemoveFromScheduledList(CWsWindow* aWindow);
	void RemoveFromTimedDrawList(CWsWindow* aWindow);
	void SetupVisibleRegionTracking(CWsWindow& aWindow, TBool aRegister) const;
	
	TBool IsAnimating() const;
	
private:
	class TInternalSizeMode: public TSizeMode
		{
	public:
		inline TInternalSizeMode(){}
		inline TInternalSizeMode(TSize& aSize)
			:	TSizeMode(aSize), iFlags(0)
			{	}
		TInt iFlags;
		};
	enum TSyncConsts
		{
		KDSAAbortingImmediateRespAwaitFrameMicrosec = 80000		// 0.08 sec
		};
	void SetInitialScreenSizeModeAndRotation();
	CWsWindowGroup* FindNewFocus(CWsRootWindow* aRootWindow);
	void UpdateCompositionMode();
	void InitializeSceneL();
	void InitializeUiElementsL();
	TInt InitializeDsaSurface();
	void LoadScreenRotationProperties(TInternalSizeMode& aMode, const TInt aModeIndex);
	void LoadScreenTwipsProperties(TInternalSizeMode& aMode, const TInt aModeIndex);
	void CreateFallbackMapL();
	MWsElement* CreateUiElementL(const TRect& aExtent);
	const TInternalSizeMode* ModePtrL(TInt aIndex) const;
	void ApplyRemainingWsiniSettingsL();
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

	MWsScreenDevice* iScreenDevice;
	CFbsScreenDevice* iDsaDevice;
	CFbsBitGc* iDsaGc;
	CGraphicsDeviceMap* iDeviceMap;
// Size and rotation members
	RPointerArray<TInternalSizeMode> *iModes;
	TInt iScreenSizeMode;
	TScreenModeEnforcement iSizeEnforcementMode;
	TInt iScreenNumber;
	TInt iMaxContrast;
	TInt iMaxBrightness;
	TInt iNumScreenSizeModes;
	TInt iNumSupportedScreenSizeModes;
	TInt iNumberDrawingDsa;
	TBool iBackLightFlag;
	
	TDsaDrawState iDsaDrawState;
	
	MWsDisplayMapping* iDisplayMapping;
	MWsDisplayControl* iDisplayControl;
	MWsDisplayPolicy* iDisplayPolicy;
	MWsScene* iScene;
	CWindowElementSet* iWindowElementSet;
	MWsElement* iUiElement;
	TSurfaceId iDsaSurface;
	CRegisteredSurfaceMap* iSurfaceMap;

	CDebugBar* iDebugBar;
	CFallbackMap* iFallbackMap;
	TUint8 iBlackMap;
	TUint8 iWhiteMap;
	CWsSpriteManager* iSpriteManager;
	//The next three interfaces should be provided by renderstages if the screen is to be used in CHANGETRACKING mode
	MWsWindowTreeObserver* iWindowTreeObserver;
	MWsDrawAnnotationObserver* iDrawAnnotationObserver;
	MWsWindowVisibilityNotifier* iWindowVisibilityNotifier;

#if defined(__WINS__) && defined(_DEBUG)
	CDebugOsbWin* iDebugWin;
#endif
	CWsDisplayChangeNotifier* iDisplayChangeNotifier;
	CWsConfigChangeNotifier* iConfigChangeNotifier;
	TInt iDisplayChangeSpinner;
	TInt iConfigChangeSpinner;
	RPointerArray<CWsClient> iWsClientList; 
	TRect iDigitiserArea;
	};



//
// inlines			//
//

//
// CScreen
//

inline CWsWindowGroup* CScreen::FocusWindowGroup()
	{return(iCurrentFocus);}
inline TBool CScreen::BlankScreenOnRotation()
	{return(iFlags&EBlankScreenOnRotation);}
inline TBool CScreen::AutoClear()
	{return(iFlags&EAutoClear);}
inline CWsWindowGroup* CScreen::DefaultOwningWindowGroup()
	{return(iDefaultOwningWindow);}
inline const TSurfaceId& CScreen::DsaSurface() const
	{return iDsaSurface;}

inline CWsRootWindow* CScreen::RootWindow() const
	{return iRootWindow;}
inline TInt CScreen::ScreenNumber() const
	{return iScreenNumber;}
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
inline TDisplayMode CScreen::DefaultDisplayMode(const TInt aMode) const
	{return (*iModes)[aMode]->iDefaultDisplayMode;}
inline TScreenModeEnforcement CScreen::SizeEnforcementMode() const
	{return iSizeEnforcementMode;}
inline TBool CScreen::IsValidScreenSizeMode(TInt aMode) const
	{return (aMode>=0 && aMode<iModes->Count() && (*iModes)[aMode]!=NULL);}
inline TBool CScreen::BackLightFlag()
	{return iBackLightFlag;}

inline TBool CScreen::HasAlpha() const
	{return (0x3 & (0x1 << (EColor16MAP - DisplayMode())));} //currently it is just EColor16MA Or EColor16MAP

inline CWindowElementSet& CScreen::WindowElements() const
	{
	return *iWindowElementSet;
	}

inline MWsElement& CScreen::UiElement() const
    {
    return *iUiElement;
    }

inline CDebugBar * CScreen::DebugBar()
	{return iDebugBar;}
inline CScreen::CFallbackMap * CScreen::FallbackMap()
	{return iFallbackMap;}
	
inline void CScreen::GetFadingParams(TUint8& aBlackMap,TUint8& aWhiteMap) const
    {aBlackMap=iBlackMap;aWhiteMap=iWhiteMap;}
inline void CScreen::SetFadingParams(TUint8 aBlackMap,TUint8 aWhiteMap)
	{iBlackMap=aBlackMap;iWhiteMap=aWhiteMap;}
inline CWsSpriteManager* CScreen::SpriteManager() const
	{return iSpriteManager;}
inline MWsWindowTreeObserver* CScreen::WindowTreeObserver() const
	{return iWindowTreeObserver;}
inline MWsDrawAnnotationObserver* CScreen::DrawAnnotationObserver() const
	{return iDrawAnnotationObserver;}
inline MWsWindowVisibilityNotifier* CScreen::WindowVisibilityNotifier() const
	{return iWindowVisibilityNotifier;}
inline TBool CScreen::ChangeTracking() const
	{return iFlags&EChangeTracking;}
inline void CScreen::IncreaseDisplaySpinner()
	{iDisplayChangeSpinner++;}
inline void CScreen::IncreaseConfigSpinner()
	{iConfigChangeSpinner++;}
inline TInt CScreen::DisplaySpinner()
	{return iDisplayChangeSpinner;}
inline TInt CScreen::ConfigSpinner()
	{return iConfigChangeSpinner;}
inline MWsDisplayPolicy* CScreen::DisplayPolicy()
	{return iDisplayPolicy;}
inline MWsDisplayMapping* CScreen::DisplayMapping()
	{return iDisplayMapping;}
#endif

