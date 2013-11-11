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

#ifndef __FBRENDERSTAGE_H__
#define __FBRENDERSTAGE_H__

#include <graphics/wsrenderstage.h>
#include <graphics/wsscene.h>
#include <graphics/wsdisplaycontrol.h>
#include <graphics/wsdisplaymapping.h>
#include "rendertarget.h"

class CRenderTarget;
class MWsScreenDevice;
class MWsDrawableSourceProvider;
class CDisplayPolicy;
class CFbRenderStage;

// An animation throttle to ensure that WSERV is not always busy animating.
class CAnimationThrottle : public CTimer
	{
public:
	static CAnimationThrottle* NewL (CFbRenderStage* aRenderStage);
	~CAnimationThrottle ();
private:
	CAnimationThrottle (CFbRenderStage* aRenderStage);
	void ConstructL ();
	void RunL();
private:
	CFbRenderStage* iRenderStage;
	};

/**
Active object to receive downstream pipeline signals (if the upstream uses signalling).
*/ 
class CRenderStageController : public CActive
	{
public:
	static CRenderStageController* NewL(CFbRenderStage* aRenderStage);
	
	~CRenderStageController();
	void SetActive();
	TBool IsActive();

private:
	CRenderStageController(CFbRenderStage* aRenderStage);
	void ConstructL();
	void DoCancel();
	void RunL();

private:
	CFbRenderStage* iRenderStage;
	CAnimationThrottle* iAnimationThrottle;
	};

/**
This is the implementation of CWsRenderStage which is created by a CFbRenderStageFactory
*/
class CFbRenderStage :
	public CWsRenderStage,
	public MWsDisplayControl,
	public MWsDisplayMapping,
	public MWsScene
	{
public:
	static CFbRenderStage* NewL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage);
	~CFbRenderStage();

public: //from CWsRenderStage
	TAny* ResolveObjectInterface(TUint aTypeId);
	void Begin(const TRegion* aRegion);
	void End(TRequestStatus* aFinished);	

public: //from MWsScene
    TInt SetSceneRotation(const TSceneRotation aSceneRotation);
    MWsScene::TSceneRotation SceneRotation() const;
    MWsElement* CreateSceneElementL();
    void DestroySceneElement(MWsElement* aElement);
    TInt InsertSceneElement(MWsElement* aInsertElement, MWsElement* aSubordinateElement);
    TInt RemoveSceneElement(MWsElement* aRemoveElement);
    void ComposePendingScene(TSurfaceId& aOffScreenTarget, TRequestStatus* aCompleted);
    TInt RegisterSurface(const TSurfaceId& aSurface);
    TInt UnregisterSurface(const TSurfaceId& aSurface);
    
public:
	void DoEnd();

public: // from MWsDisplayControl/MDisplayControlBase
	void NotifyOnDisplayChange(TRequestStatus& aStatus);
	void NotifyOnDisplayChangeCancel();
	void NotifyOnConfigChange(TRequestStatus& aStatus);
	void NotifyOnConfigChangeCancel();
	TInt NumberOfResolutions() const;
	TInt GetResolutions(RArray<TResolution>& aResolutions) const;
	void GetConfiguration(TDisplayConfiguration& aConfig) const;
	TInt SetConfiguration(const TDisplayConfiguration& aConfig);
	TInt PreferredDisplayVersion() const;
	
public: //from MWsDisplayMapping/MDisplayMapping
	virtual TInt MapCoordinates(TCoordinateSpace aSourceSpace, const TRect& aSource, TCoordinateSpace aTargetSpace, TRect& aTarget) const;
	void SetSizeModeExtent(TRect& aExtent, TBitFlags32 aContext);	
	
private:
	CFbRenderStage();
	void ConstructL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage);
	
private:
	CRenderStageController* iController;
	TRequestStatus* iUpstreamRequestStatus;
	TBool iRequestPending;
	MWsScreenDevice* iNextScreenDevice;
	MWsScene* iNextScene;
	RRegion* iUpdateRegion;
	CRenderTarget* iRenderTarget;
	MWsDisplayControl*				iDisplayControl;
	CDisplayPolicy*					iDisplayPolicy;
	MWsDisplayMapping*				iDisplayMapping;

	MWsDrawableSourceProvider* iDrawableSourceProvider;
	/** Drawable sources for copying iRenderTarget to the next render stage */
	TAny* iDrawableSources[CRenderTarget::EAspectRatioCount];
	};

#endif //__FBRENDERSTAGE_H__
