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

#include "eventcheckerrenderstage.h"
#include "graphicscontextchecker.h"
#include "scenechecker.h"
#include "panics.h"

#define GET_INTERFACE(aType, aRealInterface) \
	aType* aRealInterface; \
	do { \
		aRealInterface = static_cast<aType*>(Next()->ResolveObjectInterface(aTypeId)); \
		CHK_ASSERT_ALWAYS(aRealInterface, EEventCheckerPanicNextRenderStageHasNo##aType); \
	} while (EFalse)

#define GET_AND_CHECK_BEGINEND_INTERFACE(aType, aRealInterface, aPreviousInterface) \
	CHK_ASSERT_ALWAYS(iWithinBeginAndEnd, EEventCheckerPanic##aType##RequestedOutsideBeginEnd); \
	GET_INTERFACE(aType, aRealInterface); \
	do { \
		CHK_ASSERT_ALWAYS(!aPreviousInterface || /* first resolve after Begin() */ \
			(aPreviousInterface == aRealInterface), /* re-resolve of same instance */ \
			EEventCheckerPanicNextRenderStageHasChanged##aType##WithinBeginEnd); \
	} while (EFalse)


CEventCheckerRenderStage::CEventCheckerRenderStage(MWsGraphicDrawerEnvironment& aEnv, MWsScreen& aScreen)
	: iEnv(aEnv), iScreen(aScreen), iWithinBeginAndEnd(EFalse)
	{
	}
	
CEventCheckerRenderStage* CEventCheckerRenderStage::NewL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, MWsScreenRedraw* /*aScreenRedraw*/, CWsRenderStage* aNextStage)
	{
	CEventCheckerRenderStage* stage = new(ELeave) CEventCheckerRenderStage(*aEnv, *aScreen);
	CleanupStack::PushL(stage);
	stage->ConstructL(aNextStage);
	CleanupStack::Pop(stage);
	return stage;
	}
	
void CEventCheckerRenderStage::ConstructL(CWsRenderStage* aNextStage)
	{
	BaseConstructL();
	iGraphicsContextChecker = CGraphicsContextChecker::NewL(iEnv);
	iSceneChecker = CSceneChecker::NewL(iEnv, iScreen);
	SetNext(aNextStage);
	}

CEventCheckerRenderStage::~CEventCheckerRenderStage()
	{
	delete iGraphicsContextChecker;
	delete iSceneChecker;
	}

TAny* CEventCheckerRenderStage::ResolveObjectInterface(TUint aTypeId)
	{
	switch(aTypeId)
		{
	case MWsGraphicsContext::EWsObjectInterfaceId:
		{
		GET_AND_CHECK_BEGINEND_INTERFACE(MWsGraphicsContext, realGc, iGraphicsContextChecker->GraphicsContext());
		iGraphicsContextChecker->SetGraphicsContext(realGc);
		return static_cast<MWsGraphicsContext*>(iGraphicsContextChecker);
		}
	case MWsFader::EWsObjectInterfaceId:
		{
		GET_AND_CHECK_BEGINEND_INTERFACE(MWsFader, realFader, iGraphicsContextChecker->Fader());
		iGraphicsContextChecker->SetFader(realFader);
		return static_cast<MWsFader*>(iGraphicsContextChecker);
		}
	case MWsTextCursor::EWsObjectInterfaceId:
		{
		GET_INTERFACE(MWsTextCursor, realTextCursor);
		iGraphicsContextChecker->SetTextCursor(realTextCursor);
		return static_cast<MWsTextCursor*>(iGraphicsContextChecker);
		}
	case MWsDrawAnnotationObserver::EWsObjectInterfaceId:
		{
		return static_cast<MWsDrawAnnotationObserver*>(iGraphicsContextChecker);
		}
	case MWsScene::EWsObjectInterfaceId:
		{
		GET_INTERFACE(MWsScene, realScene);
		iSceneChecker->SetScene(realScene);
		return static_cast<MWsScene*>(iSceneChecker);
		}
	case MWsWindowTreeObserver::EWsObjectInterfaceId:
		{
		return static_cast<MWsWindowTreeObserver*>(iSceneChecker);
		}
	default:
		return CWsRenderStage::ResolveObjectInterface(aTypeId);
		}
	}

void CEventCheckerRenderStage::Begin(const TRegion* aRegion)
	{
	CHK_ASSERT_ALWAYS(!iWithinBeginAndEnd, EEventCheckerPanicBeginWithoutPrecedingEnd);
	iWithinBeginAndEnd = ETrue;
	if (Next())
		{
		Next()->Begin(aRegion);
		}
	}
	
void CEventCheckerRenderStage::End(TRequestStatus* aCompositorReady)
	{
	CHK_ASSERT_ALWAYS(iWithinBeginAndEnd, EEventCheckerPanicEndWithoutPrecedingBegin);
	iWithinBeginAndEnd = EFalse;
	if (Next())
		{
		Next()->End(aCompositorReady);
		}

	iSceneChecker->OnEnd();
	//the following interfaces need to be requested for each Begin()/End() pair
	iGraphicsContextChecker->SetGraphicsContext(NULL);
	iGraphicsContextChecker->SetFader(NULL);
	}

