// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Window server panics
// 
//

#ifndef __PANICS_H__
#define __PANICS_H__

#include <e32def.h>
#include <e32cmn.h>

enum TWservPanic
	{
	EWsPanicTemp = 0, 				// Used for panics that aren't intended to be left in the code
	EWsPanicInvalidInvalidArea = 1,
	EWsPanicArea = 2,
	EWsPanicRegion = 3,
	EWsPanicQueueUpdateCount = 4,
	EWsPanicUnknownCaptureKey = 5,
	EWsPanicInvalidCursorType = 6,
	EWsPanicInvalidRegion = 7,
	EWsPanicInvalidArea = 8,
	EWsPanicTextCursorFocus = 9,
	
	EWsPanicWindowType = 10,
	EWsPanicRootCommand = 11,
	EWsPanicWindowCheck = 12,
	EWsPanicFailedToInitialise = 13,
	EWsPanicBadActiveGcList = 14,
	//EWsPanicShadowLockingError = 15,
	EWsPanicBackupRegion = 16,
	EWsPanicKeyRepeat = 17 ,
	EWsPanicErrorInRegion = 18,
	EWsPanicRedrawQueueError = 19,
	
	EWsPanicRegionErrorCheck = 20, // NOT USED
	EWsPanicPointerRepeatTimerStatus = 21,
	EWsPanicCheckEventQueue = 22,
	EWsPanicEventQueueCopy = 23,
	EWsPanicFontCacheCount = 24,
	//EWsPanicNegativeShadow = 25,
	EWsPanicPointerClaimGrab = 26,
	EWsPanicPointerCursorAccessCount = 27,
	EWsPanicSetOrdinalPos = 28,
	EWsPanicGcStructSizeError = 29,
	
	EWsPanicPointerTimer = 30,
	EWsPanicNullDeviceHandle = 31,			//A function returned a NULL device handle unexpectantly
	EWsPanicColorModeInconsitency = 32,		//Logic error while analysing the nature of Color Modes
	EWsPanicCurrentlyFaded = 33,			//Should not do while window is fadded
	EWsPanicEventType = 34,					//Must be a pointer event
	EWsPanicObjectIndexError = 35,			//Object being removed has wrong type
	EWsPanicAnimHasNoWindow = 36,			//This AnimDLL function needs the Anim to have a window
	EWsPanicSpriteAlreadyActive = 37,		//An attempt to activate an active sprite
	EWsPanicAlreadyHaveBackup = 38,			//Getting backup behind when already got it
	EWsPanicSpriteHiddenCount = 39,			//The Sprite's hidden count is inconsistent with the action being performed on it
	
	EWsPanicRawEventsTypeChanged = 40,		//Changes to the Raw Event Type List will probably break this code
	EWsPanicEventHandlerInconsistency = 41,	//The number of things claiming to be event handlers doesn't make sence
	EWsPanicBackupStateError = 42,			//Something has gone wrong with the state of the Backup classes
	EWsPanicRegionNull = 43,				//A region shouldn't be NULL or set to the nullRegion at this point
	//EWsPanicShadowError = 44,				//The shadows have not be updated correctly
	EWsPanicDirectScreenAccess = 45,		//Error in the state machine of or logic error in Direct Screen Access code
	EWsPanicClickPluginNotLoaded = 46,		//Trying to call function in Click PlugIn when there isn't one loaded
	EWsPanicChangeClickPlugin = 47,			//Trying to change keyclick plugin when it's fixed
	EWsPanicInvalidScreenSizeMode = 48,		//The current screen size mode is one that does not exist
	EWsPanicDrawCommandsBufferCorrupt = 49,	//The draw commands buffer contains some inconsistent information
	
	EWsPanicDrawCommandsInvalidState = 50,	//The draw commands state doesn't match with the action that is occuring
	EWsPanicDrawCommandsNullSession = 51,	//Session NULL when it shouldn't be
	EWsPanicBitmapOpcodeInvalid = 52,		//At least one of the bitmap opcodes is invalid
	EWsPanicNoDisplayModeFound = 53,			//No display mode has been found when creating the off-screen bitmap
	EWsCreatedOffScreenBitmapInWrongDimensions = 54,	//There is not enough memory to swap the width and the height as the bitmap was not created with the right dimensions
	EWsFailedToAllocateForOffScreenBitmapDevice = 55,	//Couldn't allocate memory for the device after deleting it
	EWsPanicTransparentInvalidState = 56,				//Something required for transparency is not available
	EWsPanicBadDescriptor = 57,							//The descriptor has been badly constructed
	EWsPanicAlphaBlendBadParams = 58,		//Bad parameters were passed in our call to the Alpha Blending function
	EWsPanicInvalidMessageHandle = 59,		//The current screen size mode is one that does not exist	
	
	EWsPanicWindowNull = 60,				//Window handle NULL when it shouldn't be
	EWsPanicPanicFlagError = 61,			//The panic flag setting is inconsistent with other information
	EWsPanicGCDrawingToWrongDevice = 62,	//The device a GC is drawing to has been changed unexpectantly.
	EWsPanicScreenInformationError = 63,	//Conflict between number of screens defined in INI file and HAL
	EWsPanicGroupWindowChainError = 64,		//An error has occured in the group window chain algorithms
	EWsPanicNoScreen = 65,					//There is no screen when there should be one
	EWsPanicWrongNumberBitmapDevices = 66,	//The number of OSB bitmap devices does not match the number of screen modes
	EWsPanicHiddenInvalidError = 67,		//The Hidden Invalid contains area outside of the FullRect of the window
	EWsPanicWsGraphic = 68,					//An issue in internal state in the CWsGraphicDrawer mechanisms, thrown in debug builds only
	EWsPanicOffScreenBitmapMissing = 69,	//Found an unexpected NULL pointer to an OffScreenBitmap
	
	EWsPanicGCDrawRegionError = 70,//NOT USED//A logic error in the code to decide which region to be drawing too
	EWsPanicFailedToSetThread = 71, 		//Setting wserv thread to system permananent is failed 
	EWsPanicAnimationNotScheduled = 72,		//Animation ran when not scheduled
	EWsPanicAnimationAlreadyAnimating = 73,	//Animation already marked as animating
	EWsPanicSpriteBitmapSizeChange = 74,	//A sprite member bitmap has been made larger, since the sprite was activated, without calling UpdateMember
	EWsPanicMsgQueueError = 75, 			//Error occurs if inactive message queue doesn't have expected number of messages
	EWsPanicRedrawRegionArrayEmpty = 76,	//Trying to retrieve the current redraw region when the redraw region array is empty
	EWsPanicIllegalInvalidArea = 77,		//Invalid area includes region outside visible area of the window.
//	EWsPanicTransparencyManagerNotFreed = 78, // incorrect session shutdown
//	EWsPanicFailedToSetProcess = 79,	   	//For future extention
	EWsPanicDebugRegionMemoryLeak = 80,		//Panic code for RRegionWserv memory leak
	EWsPanicDefaultHotKeyNotFound = 81,		//Default hot key was not found
//	EWsPanicInvalidRedrawStoreDiscardParameter = 82,	 // Invalid parameter specified for redraw store discard detection algorithm.
//	EWsPanicOverflowRedrawStoreDiscardParameter = 83, // Parameter specified for redraw store discard detection algorithm will cause overflow.
	EwsPanicDisplayModeNotSupported = 84,	//Display mode is not supported by screen
	EWsPanicCommandBufferStillBeingProcessed = 85, //Previous command buffer still being processed when another one arrives
	EWsPanicNullClientMessage = 86,			//Processing a client message that is NULL
	EWsPanicNoWindowElement = 87,			//Element not found for window
	EWsPanicInvalidNotificationFlag = 88,	//Invalid Notification event
	EWsPanicNotificationTableIsNull = 89,	//Notification table is unexpectedly NULL
	EWsPanicInvalidOperation = 90,			//An internal use of an operation was invalid
	EWsPanicCompositionNotSupported = 91,			//An attempt to use compositor on a screen which doesn't support it XXX UNUSED??
	EWsPanicCompositionErrorIgnored = 92,			//In release the composition error would have been ignored. Occurs in debug builds only. XXX UNUSED??
	EWsPanicExpectedSurfaceChange = 93,		//Surfaces returned by different screen devices unexpectedly match
	EWsPanicSurfaceMapError = 94,			//There was a problem when unregistering a surface from the registered surface map.
	EWsPanicScheduledRedraw = 95,			//Something went horribly wrong in the redraw scheduling code
	EWsPanicRedrawSegmentsInvalidState = 96, //The redraw segments are in a state they should never be able to reach
	EWsPanicMemoryManager = 97,				//The memory manager encountered an inconsistant environment somehow
	EWsPanicAccessList = 98,				//The access list is invalid
	EWsPanicScreenFallback = 99,			//The screens low memory fallback system has failed
	
	EWsPanicActiveScheduler = 100,			//The active scheduler encountered an inconsistant environment somehow
	EWsPanicDrawFailure = 101,				//A draw function which can't leave left
	EWsPanicMemoryLock = 102,				//The memory lock/unlock for memory management failed
	
	EWsPanicRecycleElement = 103,			//No element to recycle
	EWsPanicUnexpectedOpacity = 104,		//Unexpected opacity being set on a window
	EWsPanicMaxPointersOutOfRange = 105,	//HALData::EPointerMaxPointers is out of range
	EWsPanicMaxPointersInconsistent = 106,  //HALData::EPointerMaxPointers is inconsistent with TXYInputType reported by HAL
	EWsPanicPointer3DInconsistent = 107,	//HALData::EPointer3D is inconsistent with TXYInputType reported by HAL
	EWsPanicProxThresholdsInconsist = 108,	//HALData::EPointer3DEnterCloseProximityThreshold is settable, but EPointer3DExitCloseProximityThreshold is not
	EWsPanicPressThresholdsInconsist = 109,	//HALData::EPointer3DEnterHighPressureThreshold is settable, EPointer3DExitHighPressureThreshold is not
	EWsPanicScreenDeviceMissing = 110,		//Can't find an instance of MWsScreenDevice
	EWsPanicSceneMissing = 111,				//Can't find an instance of MWsScene
	EWsPanicNoRenderStagePipeline = 112,	//Failed to create/connect the render stage pipeline
	EWsPanicInvalidChromaKeyColor = 113,	//Chroma key color has to be fully opaque
	EWsPanicTextCursorInterfaceMissing = 114,	//The interface in the Render Stage to support Text Cursors is missing
	EWsPanicSceneErrorIgnored = 115,		//In release the scene error would have been ignored. Occurs in debug builds only.
	EWsPanicNoDisplayControl = 116,	//Display control expected
	EWsPanicVisibleRegionTracking = 117,
	EWsPanicNoChangetracking = 118,			//CHANGETRACKING mode is expected to be set
	EWsPanicNoSprite = 119,
	EWsPanicAnim = 120,
	EWsPanicFloatingSprite = 121,
	EWsPanicRenderstageAPIs = 122,
	EWsPanicBitmapArrayNotEmpty = 123,      //Bitmap array is not empty, when it is expected to be empty
	EWsPanicCounterValue = 124,             //Counter value is not the expected one
	EWsPanicUnexpectedBitmapHandleInArray = 125,     //Bitmap handle in array with no matching bitmap object in bitmap ref array
	EWsPanicBitmapNotFound = 126,           //Failed to find a bitmap in the bitmap ref array
	EWsPanicInvalidRotation = 127,			//Invalid rotation used
	EWsPanicInvalidPointerOffset = 128,      //The pointer offset value in wsini.ini is invalid.
	EWsPanicArrayInsertFailed = 129,	//Array insert failed.           
	EWsPanicAccessBeyondCommandBuf = 130,	//Access beyond wserv command buffer
	EWsPanicKeyEventRouterBadResult = 131,		// Invalid result code from key event routing plug-in
	EWsPanicKeyEventRouterBadWindowGroup = 132,	// Invalid destination window group from key event routing plug-in
	EWsPanicKeyEventRouterLeave = 133,			// Invalid leave from Key event routing plug-in
	};

void Panic(TWservPanic aPanic);
void PanicWithInfo(TWservPanic aPanic, const TDesC& aFileName, const TDesC& aPanicName, TInt aLine);
void PanicWithCondAndInfo(TWservPanic aPanic, const TDesC& aCondition, const TDesC& aFileName, const TDesC& aPanicName, TInt aLine);

// helper macros
#define WS_ASSERT_FILENAME(f)	_LIT(KPanicFileName, f);
#define WS_ASSERT_PANICNAME(p)	_LIT(KPanicEnum ## p, # p);
#define WS_ASSERT_CONDITION(c)	_LIT(KPanicCondition, c);


/* Use these macros for WServ server Asserts & Panics with diagnostic logging:
   WS_ASSERT_ALWAYS, WS_PANIC_ALWAYS, WS_ASSERT_DEBUG and WS_PANIC_DEBUG
   Note that the "do { ... } while (0)" construction forces the invocation to have a ';'
   and makes the macro expansion safe in nested "if ... else ..." clauses that forget to use the
   { braces } in the Coding Standard.
 */
#define WS_ASSERT_ALWAYS(c, p) \
		do  { \
			if (!(c)) \
				{ \
				WS_ASSERT_FILENAME(__FILE__); \
				WS_ASSERT_PANICNAME(p); \
				WS_ASSERT_CONDITION(#c); \
				PanicWithCondAndInfo(p, KPanicCondition, KPanicFileName, KPanicEnum ## p, __LINE__); \
				} \
			} while (0)

#define WS_PANIC_ALWAYS(p) \
		do  { \
			WS_ASSERT_FILENAME(__FILE__); \
			WS_ASSERT_PANICNAME(p); \
			PanicWithInfo(p, KPanicFileName, KPanicEnum ## p, __LINE__); \
			} while (0)


#if defined(_DEBUG)
#define WS_ASSERT_DEBUG(c, p) \
		do  { \
			if (!(c)) \
				{ \
				WS_ASSERT_FILENAME(__FILE__); \
				WS_ASSERT_PANICNAME(p); \
				WS_ASSERT_CONDITION(#c); \
				PanicWithCondAndInfo(p, KPanicCondition, KPanicFileName, KPanicEnum ## p, __LINE__); \
				} \
			} while (0)

#define WS_PANIC_DEBUG(p) \
		do  { \
			WS_ASSERT_FILENAME(__FILE__); \
			WS_ASSERT_PANICNAME(p); \
			PanicWithInfo(p, KPanicFileName, KPanicEnum ## p, __LINE__); \
			} while (0)
#else
#define WS_ASSERT_DEBUG(c, p)
#define WS_PANIC_DEBUG(p)
#endif


#endif
