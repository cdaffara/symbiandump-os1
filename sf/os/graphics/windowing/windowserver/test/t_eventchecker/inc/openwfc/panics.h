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

#ifndef __PANICS_H__
#define __PANICS_H__

enum TEventCheckerPanic
	{
	EEventCheckerPanicEventCheckerMustNotBeFinalRenderStage,
	//CWsRenderStage
	EEventCheckerPanicBeginWithoutPrecedingEnd,
	EEventCheckerPanicEndWithoutPrecedingBegin,
	//MWsGraphicsContext
	EEventCheckerPanicMWsGraphicsContextRequestedOutsideBeginEnd,
	EEventCheckerPanicNextRenderStageHasNoMWsGraphicsContext,
	EEventCheckerPanicNextRenderStageHasChangedMWsGraphicsContextWithinBeginEnd,
	EEventCheckerPanicUsingMWsGraphicsContextAfterEnd,
	//MWsTextCursor
	EEventCheckerPanicMWsTextCursorRequestedOutsideBeginEnd,
	EEventCheckerPanicNextRenderStageHasNoMWsTextCursor,
	EEventCheckerPanicNextRenderStageHasChangedMWsTextCursorWithinBeginEnd,
	EEventCheckerPanicUsingMWsTextCursorAfterEnd,
	//MWsFader
	EEventCheckerPanicMWsFaderRequestedOutsideBeginEnd,
	EEventCheckerPanicNextRenderStageHasNoMWsFader,
	EEventCheckerPanicNextRenderStageHasChangedMWsFaderWithinBeginEnd,
	EEventCheckerPanicUsingMWsFaderAfterEnd,
	//MWsScene
	EEventCheckerPanicNextRenderStageHasNoMWsScene,
	EEventCheckerPanicElementCountMismatch,
	EEventCheckerPanicElementLinksMismatch,
	EEventCheckerPanicAcquireElementWithoutWindow,
	EEventCheckerPanicAcquireElementForUnknownElement,
	EEventCheckerPanicElementsWithoutAssociatedWindow,
	//MWsDrawAnnotationObserver
	EEventCheckerPanicUnbalancedDrawingTargetEvents,
	EEventCheckerPanicDrawingWithoutTarget,
	};

void Panic(TEventCheckerPanic aPanic);

#ifdef _DEBUG
#define CHK_ASSERT_DEBUG(c, p) \
	do { \
		if(!(c)) \
			{ \
			Panic(p); \
			} \
		} while (EFalse)
#else
#define CHK_ASSERT_DEBUG(c, p)
#endif //_DEBUG

#define CHK_ASSERT_ALWAYS(c, p) \
	do { \
		if(!(c)) \
			{ \
			Panic(p); \
			} \
		} while (EFalse)

#endif // __PANICS_H__
