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

#ifndef __STDPANIC_H__
#define __STDPANIC_H__

enum TStdPluginPanic
{
	EStdPanicTemp = 0,
	EStdPanicNoDisplayModeFound = 1,
	EStdPanicCreatedOffScreenBitmapInWrongDimensions = 2,
	EStdPanicStdRenderStageMustBeFinal = 3,
	EStdPanicFlickerBufferRenderStageMustNotBeFinal = 4,
	EStdPanicIniFileMissing = 5,	// can't find an MWsIniFile instance
	EStdPanicPreviousUiSurfaceUnregisterable = 6,
	EStdPanicDuplicateUiLayer = 7,	// occurs when trying to add a UI layer when another UI layer is already part of the scene
	EStdPanicNonMatchingRemovalOfUiLayer = 8, // occurs when removing a UI layer, when it's not the current UI layer
	EStdPanicUiSurfaceIsNull = 9,	// occurs when the UI surface is NULL
	EStdPanicScreenDeviceMissing = 10, // can't get MWsScreenDevice from the next render stage
	EStdPanicCompositionContextMissing = 11, // can't get MWsCompositionContext from the next render stage
	EStdPanicPopGcSettings = 12, // problem when popping gc settings, previously saved
	EStdPanicDrawCommandsInvalidState = 13,	//The draw commands state doesn't match with the action that is occuring
	EStdPanicInvalidCursorType = 14, // invalid cursor
	EStdPanicRegionError = 15, //Region is not NULL and TRegion::CheckError() has returned ETrue, or validation of region state has failed
	EStdPanicBadBeginEndSequence = 16, // End() has been called without corresponding Begin(), or Begin() has been called twice.
	EStdPanicRedrawNotCompleted = 17, // A redraw has been requested before the previous redraw has completed.  
	EStdPanicNoContext = 18, // Graphics context from the next rendering stage is not available
};

void Panic(TStdPluginPanic aPanic);

#ifdef _DEBUG
#define STD_ASSERT_DEBUG(c, p) \
	do { \
		if(!(c)) \
			{ \
			Panic(p); \
			} \
		} while (EFalse)
#else
#define STD_ASSERT_DEBUG(c, p)
#endif //_DEBUG

#define STD_ASSERT_ALWAYS(c, p) \
	do { \
		if(!(c)) \
			{ \
			Panic(p); \
			} \
		} while (EFalse)

#endif //__STDPANIC_H__
