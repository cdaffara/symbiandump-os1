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

#ifndef __SCENECHECKER_H__
#define __SCENECHECKER_H__

#include <graphics/wsscene.h>
#include <graphics/wsgraphicdrawer.h>

class CSceneChecker :  public CBase, 
									public MWsScene, 
									public MWsWindowTreeObserver
	{
public:
	static CSceneChecker* NewL(MWsGraphicDrawerEnvironment& aEnv, MWsScreen& aScreen);
	~CSceneChecker();

	void SetScene(MWsScene* aScene);
	void OnEnd();
	
public: // from MWsScene
	TAny* ResolveObjectInterface(TUint aTypeId);
	TInt SetSceneRotation(TSceneRotation aRotation);
	TSceneRotation SceneRotation() const;
	MWsElement* CreateSceneElementL();
	void DestroySceneElement(MWsElement* aLayer);
	void ComposePendingScene(TSurfaceId& aOffScreenTarget, TRequestStatus* aCompleted);
	TInt InsertSceneElement(MWsElement* aInsertElement, MWsElement* aSubordinateElement);
	TInt RemoveSceneElement(MWsElement* aRemoveElement);
	TInt RegisterSurface(const TSurfaceId& aSurface);
	TInt UnregisterSurface(const TSurfaceId& aSurface);

public: // from MWsWindowTreeObserver
	void NodeCreated(const MWsWindowTreeNode& aWindowTreeNode, MWsWindowTreeNode const* aParent);
	void NodeReleased(const MWsWindowTreeNode& aWindowTreeNode);
	void NodeActivated(const MWsWindowTreeNode& aWindowTreeNode);
	void NodeExtentChanged(const MWsWindowTreeNode& aWindowTreeNode, const TRect& aRect);
	void SiblingOrderChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aNewPosition);
	void MovedToWindowGroup(const MWsWindowTreeNode& aWindowTreeNode, const MWsWindowTreeNode& aNewWindowGroup);
	void FlagChanged(const MWsWindowTreeNode& aWindowTreeNode, MWsWindowTreeObserver::TFlags aAttribute, TBool aNewValue);
	void AttributeChanged(const MWsWindowTreeNode& aWindowTreeNode, MWsWindowTreeObserver::TAttributes aAttribute);
	void TransparentRegionChanged(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aNewTransparentRegion, const TRegion* aNewOpaqueRegion);
	void ElementAdded(const MWsWindowTreeNode& aWindowTreeNode, const MWsElement& aLayer);
	void FadeCountChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aFadeCount);
	void FadeAllChildren(const MWsWindowTreeNode& aWindowTreeNode, TBool aFaded);
	void WindowGroupChained(const MWsWindowTreeNode& aParent, const MWsWindowTreeNode& aChild);
	void WindowGroupChainBrokenAfter(const MWsWindowTreeNode& aWindowGroup);
	
private:
	CSceneChecker(MWsScreen& aScreen);
private:
	MWsScreen& iScreen;
	MWsScene* iScene;
	MWsElement* iBottomElement;
	TInt iElementSetCount;
	TBool iElementSetChanged;
	RPointerArray<MWsElement> iElementsWithoutAssociatedWindow;
	};

#endif //__SCENECHECKER_H__
