// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __DISPLAYRENDERSTAGE_H__
#define __DISPLAYRENDERSTAGE_H__

#include <graphics/surfaceupdateclient.h> 
#include <graphics/wsrenderstage.h>
#include <graphics/wsscreendevice.h>
#include <graphics/wsdisplaycontrol.h>
#include <graphics/wsdisplaymapping.h>
#include <graphics/wsdisplaypolicy.h>
#include <graphics/wsscene.h>

class CDirectGdiDriver;
class CDirectGdiGcWrapper;
class CRenderTarget;
class RSgImageCollection;
class CDisplayPolicy;
class CRsDisplayChangeNotifier;
class CRsConfigChangeNotifier;
class COpenWfcWrapper;

#if defined(__WINS__) && defined(_DEBUG)
class CDebugBarDrawer;
#endif

/**
This is the implementation of CWsRenderStage which is created by a CDisplayRenderStageFactory
*/
class CDisplayRenderStage : 
		public CWsRenderStage,
		public MWsScreenDevice,
		public MWsDisplayControl,
		public MWsDisplayMapping,
		public MWsDisplayPolicy,
		public MWsScene
	{
friend class COpenWfcWrapper;

public:
	static CDisplayRenderStage* NewL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, TInt aScreenNumber);
	~CDisplayRenderStage();

public: //from CWsRenderStage
	TAny* ResolveObjectInterface(TUint aTypeId);
	void Begin(const TRegion* aRegion);
	void End(TRequestStatus* aFinished);	

public: //from MWsScreenDevice
	TInt ScreenNumber() const;
	TDisplayMode DisplayMode() const;
	TSize SizeInPixels() const;
	TSize SizeInTwips() const;
	TRect PointerRect() const;
	void GetPixel(TRgb& aColor, const TPoint& aPixel) const;
	void GetScanLine(TDes8& aScanLine, const TPoint& aStartPixel, TInt aPixelLength, TDisplayMode aDispMode) const;
	TBool RectCompare(const TRect& aRect1, const TRect& aRect2) const;
	void CopyScreenToBitmapL(CFbsBitmap* aBitmap, const TRect &aRect) const;
	void ClearDisplay(TRgb aBackColor);
	
public: //from MWsScene
    TInt SetSceneRotation(const MWsScene::TSceneRotation aSceneRotation);
    MWsScene::TSceneRotation SceneRotation() const;
    MWsElement* CreateSceneElementL();
    void DestroySceneElement(MWsElement* aElement);
    TInt InsertSceneElement(MWsElement* aInsertElement, MWsElement* aSubordinateElement);
    TInt RemoveSceneElement(MWsElement* aRemoveElement);
    void ComposePendingScene(TSurfaceId& aOffScreenTarget, TRequestStatus* aCompleted);
    TInt RegisterSurface(const TSurfaceId& aSurface);
    TInt UnregisterSurface(const TSurfaceId& aSurface);
	
public: // from MWsDisplayControl/MDisplayControlBase
	void NotifyOnDisplayChange(TRequestStatus& aStatus);
	void NotifyOnDisplayChangeCancel();
	void NotifyOnConfigChange(TRequestStatus& aStatus);
	void NotifyOnConfigChangeCancel();
	TInt NumberOfResolutions() const;
	TInt GetResolutions(RArray<TResolution>& aResolutions) const;
	void GetConfiguration(TDisplayConfiguration& aConfig) const;
	TInt SetConfigurationAndBuffer(const TDisplayConfiguration& aConfig,const TSize* aUiSize);
	TInt SetConfiguration(const TDisplayConfiguration& aConfig);
	TInt PreferredDisplayVersion() const;
	
public: //from MWsDisplayMapping/MDisplayMapping
	virtual TInt MapCoordinates(TCoordinateSpace aSourceSpace, const TRect& aSource, TCoordinateSpace aTargetSpace, TRect& aTarget) const;
	void SetSizeModeExtent(TRect& aExtent, TBitFlags32 aContext);
	
public ://from MWsDisplayPolicy
	virtual void NewAppModesAvailable(); 
	TInt GetSizeModeConfiguration(TInt aScreenSizeMode, TDisplayConfiguration& aConfig, TRect& aSizeModePosition) const;
	TInt SuitableAppMode(TDisplayStatus aSituation);
	void SetLastAppMode(TInt aMode);
	
private:
	CDisplayRenderStage(TInt aScreenNumber);
	void ConstructL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, TInt aScreenNumber);
	void CreateRenderTargetL(TInt aScreenNumber, MWsIniFile* aIniFile);
	
private:
	TInt iScreenNumber;
	TTimeStamp iTimeStamp;
	RSurfaceUpdateSession iSurfaceUpdateSession;
	const TRegion* iUpdateRegion;
	CDirectGdiDriver* iDGdiDriver;
	CRenderTarget* iRenderTarget;
	COpenWfcWrapper* iOpenWfcWrapper;
	MWsDisplayControl*	iOpenWfcWrapperDisplayControl;
		
	CDisplayPolicy*	iDisplayPolicy;
	TBool iDisplayPolicyEnabled;
	
	//required for display and config change notification
	CRsDisplayChangeNotifier* iDispNotifier;
	CRsConfigChangeNotifier* iConfigNotifier;
	
#if defined(__WINS__) && defined(_DEBUG)
	CDebugBarDrawer* iDebugBarDrawer;
#endif
	};

#endif //__DISPLAYRENDERSTAGE_H__
