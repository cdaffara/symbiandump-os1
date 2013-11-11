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

#ifndef __PANIC_H__
#define __PANIC_H__

enum TStdPluginPanic
	{
	EPluginPanicTemp = 0,
	EPluginPanicNoDisplayModeFound = 1,
	EPluginPanicCreatedOffScreenBitmapInWrongDimensions = 2,
	EPluginPanicDisplayRenderStageMustBeFinal = 3,
	EPluginPanicFlickerBufferRenderStageMustNotBeFinal = 4,
	EPluginPanicIniFileMissing = 5,	// can't find an MWsIniFile instance
	EPluginPanicPreviousUiSurfaceUnregisterable = 6,
	EPluginPanicDuplicateUiElement = 7,	// occurs when trying to add a UI element when another UI element is already part of the scene
	EPluginPanicNonMatchingRemovalOfUiElement = 8, // occurs when removing a UI element, when it's not the current UI element
	EPluginPanicUiSurfaceIsNull = 9,	// occurs when the UI surface is NULL
	EPluginPanicScreenDeviceMissing = 10, // can't get MWsScreenDevice from the next render stage
	EPluginPanicCompositionContextMissing = 11, // can't get MWsCompositionContext from the next render stage
	EPluginPanicPopGcSettings = 12, // problem when popping gc settings, previously saved
	EPluginPanicDrawCommandsInvalidState = 13,	//The draw commands state doesn't match with the action that is occuring
	EPluginPanicInvalidCursorType = 14, // invalid cursor
	EPluginPanicRegionError = 15, //Region is not NULL and TRegion::CheckError() has returned ETrue, or validation of region state has failed
	EPluginPanicBadBeginEndSequence = 16, // End() has been called without corresponding Begin(), or Begin() has been called twice.
	EPluginPanicRedrawNotCompleted = 17, // A redraw has been requested before the previous redraw has completed.  
	EPluginPanicNoContext = 18, // Graphics context from the next rendering stage is not available
	EPluginPanicSizeInTwipsFailed = 19, // HAL has returned a value of zero for display twips
	EPluginPanicMultipleUpdateRequests = 20, // Client has requested an update with updates outstanding
	EPluginPanicObjectConstructionFailed = 21, // Allocation and construction of an object has failed 
	EPluginPanicNoDrawableSourceProvider = 22, // The next render stange doesn't support MWsDrawableSourceProvider
	EPluginPanicDrawableSourceInitializationFailed = 23, // Failure when calling MWsDrawableSourceProvider::OpenDrawable() for an offscreen image
	EPluginPanicInvalidDrawableSource = 24, // Invalid drawable source pointer passed to MWsDrawableSourceProvider
	EPluginPanicDisplayControlMissing = 25, // End() has been called without corresponding Begin(), or Begin() has been called twice.
	EPluginPanicScreenCaptureRenderStageMustNotBeFinal = 26, // A redraw has been requested before the previous redraw has completed.
	EPluginPanicScreenCaptureInvalidCommand=27,  // No Such Command
	EPluginPanicDisplayMappingMissing = 28, // can't get MWsDisplayMapping from the next render stage
	EPluginPanicNewUiSurfaceUnsettable = 30,	//can't set a new ui surface
	EPluginPanicHalSetDisplayState = 31, //error returned when trying to set DisplayState
	EPluginPanicSceneMissing = 32, // can't get MWsScene from the next render stage
	EPluginPanicCompositionSequenceError = 33, // an attempt to compose was made which was out of sequence with Begin()/End() 

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

#endif //__PANIC_H__
