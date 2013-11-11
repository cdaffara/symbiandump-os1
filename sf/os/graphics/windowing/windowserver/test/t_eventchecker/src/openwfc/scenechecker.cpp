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

#include "scenechecker.h"
#include <graphics/wselement.h>
#include "panics.h"

CSceneChecker* CSceneChecker::NewL(MWsGraphicDrawerEnvironment& /*aEnv*/, MWsScreen& aScreen)
	{
	CSceneChecker* self = new(ELeave) CSceneChecker(aScreen);
	return self;
	}

CSceneChecker::CSceneChecker(MWsScreen& aScreen)
	: iScreen(aScreen)
	{
	}

CSceneChecker::~CSceneChecker()
	{
	iScene = NULL;
	iBottomElement = NULL;
	iElementsWithoutAssociatedWindow.Reset();
	}

void CSceneChecker::SetScene(MWsScene* aScene)
	{
	iScene = aScene;
	}

void CSceneChecker::OnEnd()
	{
	if(iElementSetChanged)
		{
		if(!iBottomElement)
			{
			CHK_ASSERT_ALWAYS(iElementSetCount == 0, EEventCheckerPanicElementCountMismatch);
			return;
			}

		//walk upwards
		MWsElement* layer = iBottomElement;
		CHK_ASSERT_ALWAYS(!layer->ElementBelow(), EEventCheckerPanicElementLinksMismatch);
		TInt index = 0;
		while(layer->ElementAbove())
			{
			MWsElement* previousElement = layer;
			layer = layer->ElementAbove();
			index++;
			CHK_ASSERT_ALWAYS(layer->ElementBelow() == previousElement, EEventCheckerPanicElementLinksMismatch);
			}
		CHK_ASSERT_ALWAYS(iElementSetCount == index + 1, EEventCheckerPanicElementCountMismatch);
		//walk downwards
		CHK_ASSERT_ALWAYS(!layer->ElementAbove(), EEventCheckerPanicElementLinksMismatch);
		while(layer->ElementBelow())
			{
			MWsElement* previousElement = layer;
			layer = layer->ElementBelow();
			index--;
			CHK_ASSERT_ALWAYS(layer->ElementAbove() == previousElement, EEventCheckerPanicElementLinksMismatch);
			}
		CHK_ASSERT_ALWAYS(layer == iBottomElement, EEventCheckerPanicElementLinksMismatch);
		CHK_ASSERT_ALWAYS(index == 0, EEventCheckerPanicElementLinksMismatch);
		}

	CHK_ASSERT_ALWAYS(iElementsWithoutAssociatedWindow.Count() == 0, EEventCheckerPanicElementsWithoutAssociatedWindow);
	}
	
TAny* CSceneChecker::ResolveObjectInterface(TUint aTypeId)
	{
	switch(aTypeId)
		{
		case MWsWindowTreeObserver::EWsObjectInterfaceId:
			return static_cast<MWsWindowTreeObserver*>(this);
		}	
	return iScene->ResolveObjectInterface(aTypeId);
	}

TInt CSceneChecker::SetSceneRotation(TSceneRotation aRotation)
	{
	return iScene->SetSceneRotation(aRotation);
	}

MWsScene::TSceneRotation CSceneChecker::SceneRotation() const
	{
	return iScene->SceneRotation();
	}

MWsElement* CSceneChecker::CreateSceneElementL()
	{
	return iScene->CreateSceneElementL();
	}

void CSceneChecker::DestroySceneElement(MWsElement* aElement)
	{
	if(iBottomElement == aElement || aElement->ElementAbove() || aElement->ElementBelow()) //part of the layer set
		{
		iElementSetCount--;
		iElementSetChanged = ETrue;
		if(aElement == iBottomElement)
			iBottomElement = iBottomElement->ElementAbove();
		}
	iScene->DestroySceneElement(aElement);
	}

void CSceneChecker::ComposePendingScene(TSurfaceId& aOffScreenTarget, TRequestStatus* aCompleted)
	{
	iScene->ComposePendingScene(aOffScreenTarget, aCompleted);
	}

TInt CSceneChecker::InsertSceneElement(MWsElement* aInsertElement, MWsElement* aSubordinateElement)
	{
	TInt err = KErrNone;

	iElementSetCount++;
	iElementSetChanged = ETrue;
	
    if(!aSubordinateElement)
        {
        iBottomElement = aInsertElement;
        }
    else if(aInsertElement == iBottomElement)
        {
        iBottomElement = iBottomElement->ElementAbove();
        }
    
    TUint32 flags;
    aInsertElement->GetRenderStageFlags(flags);
	if(!(flags & MWsElement::EElementIsIndirectlyRenderedUserInterface)) //the UI element won't be associated with any window
		{
		err = iElementsWithoutAssociatedWindow.Append(aInsertElement);
		}
	
	if(!err)
		err = iScene->InsertSceneElement(aInsertElement, aSubordinateElement);

	//cleanup on error
	if(err)
		{
		const TInt index = iElementsWithoutAssociatedWindow.Find(aInsertElement);
		if(index >= 0)
			iElementsWithoutAssociatedWindow.Remove(index);
		}

	return err;
	}

TInt CSceneChecker::RemoveSceneElement(MWsElement* aRemoveElement)
	{
	iElementSetCount--;
	iElementSetChanged = ETrue;
	if(aRemoveElement == iBottomElement)
		iBottomElement = iBottomElement->ElementAbove();
	return iScene->RemoveSceneElement(aRemoveElement);
	}

TInt CSceneChecker::RegisterSurface(const TSurfaceId& aSurface)
	{
	return iScene->RegisterSurface(aSurface);
	}

TInt CSceneChecker::UnregisterSurface(const TSurfaceId& aSurface)
	{
	return iScene->UnregisterSurface(aSurface);
	}

void CSceneChecker::NodeCreated(const MWsWindowTreeNode& /*aWindowTreeNode*/, MWsWindowTreeNode const* /*aParent*/)
	{
	}

void CSceneChecker::NodeReleased(const MWsWindowTreeNode& /*aWindowTreeNode*/)
	{
	}

void CSceneChecker::NodeActivated(const MWsWindowTreeNode& /*aWindowTreeNode*/)
	{
	}

void CSceneChecker::NodeExtentChanged(const MWsWindowTreeNode& /*aWindowTreeNode*/, const TRect& /*aRect*/)
	{
	}

void CSceneChecker::SiblingOrderChanged(const MWsWindowTreeNode& /*aWindowTreeNode*/, TInt /*aNewPosition*/)
	{
	}

void CSceneChecker::MovedToWindowGroup(const MWsWindowTreeNode& /*aWindowTreeNode*/, const MWsWindowTreeNode& /*aNewWindowGroup*/)
	{
	}

void CSceneChecker::FlagChanged(const MWsWindowTreeNode& /*aWindowTreeNode*/, MWsWindowTreeObserver::TFlags /*aAttribute*/, TBool /*aNewValue*/)
	{
	}

void CSceneChecker::AttributeChanged(const MWsWindowTreeNode& /*aWindowTreeNode*/, MWsWindowTreeObserver::TAttributes /*aAttribute*/)
	{
	}

void CSceneChecker::TransparentRegionChanged(const MWsWindowTreeNode& /*aWindowTreeNode*/, const TRegion& /*aNewTransparentRegion*/, const TRegion* /*aNewOpaqueRegion*/)
	{
	}

void CSceneChecker::ElementAdded(const MWsWindowTreeNode& aWindowTreeNode, const MWsElement& aElement)
	{
	CHK_ASSERT_ALWAYS(aWindowTreeNode.Window(), EEventCheckerPanicAcquireElementWithoutWindow);
	MWsScreen* eventScreen = aWindowTreeNode.Window()->WsScreen();
	if(eventScreen == &iScreen)
		{
		const TInt index = iElementsWithoutAssociatedWindow.Find(&aElement);
		CHK_ASSERT_ALWAYS(index >= 0, EEventCheckerPanicAcquireElementForUnknownElement);
		iElementsWithoutAssociatedWindow.Remove(index);
		}
	}

void CSceneChecker::FadeCountChanged(const MWsWindowTreeNode& /*aWindowTreeNode*/, TInt /*aFadeCount*/)
	{
	}

void CSceneChecker::FadeAllChildren(const MWsWindowTreeNode& /*aWindowTreeNode*/, TBool /*aFaded*/)
	{
	}

void CSceneChecker::WindowGroupChained(const MWsWindowTreeNode& /*aParent*/, const MWsWindowTreeNode& /*aChild*/)
	{
	}

void CSceneChecker::WindowGroupChainBrokenAfter(const MWsWindowTreeNode& /*aWindowGroup*/)
	{
	}

	
