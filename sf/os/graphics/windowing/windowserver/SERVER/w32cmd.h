// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Command numbers and structures for client to window server communications.
// 
//

#ifndef __W32CMD_H__
#define __W32CMD_H__

#include <e32std.h>
#include <w32std.h>
#include <gdi.h>
#if defined(SYMBIAN_GRAPHICS_GCE)
#include <graphics/surfaceconfiguration.h>
#include <graphics/sgresource.h>
#endif

#include <graphics/displayconfiguration.h>
#include <graphics/displaymapping.h>

const TUint KWservMajorVersionNumber=1;
const TUint KWservMinorVersionNumber=0;
const TUint KWservBuildVersionNumber=151;

const TUint KWservLoggingDllUidValue8=268435925;
const TUid KWservLoggingDllUid8={KWservLoggingDllUidValue8};
const TUint KWservLoggingDllUidValue16=268450595;
const TUid KWservLoggingDllUid16={KWservLoggingDllUidValue16};
//const TUint KUidWservLoggingCommon=268439591;
const TUint KWservLoggingDllUidValue=KWservLoggingDllUidValue16;
const TUid KWservLoggingDllUid={KWservLoggingDllUidValue};
const TInt KDummyScreenNumber = -1;
class CClickMaker;
class CAnimDll;
class CDebugLogBase;
class TSurfaceId;

/** Creates an interface object for Shell

@publishedPartner
@released
*/
typedef TInt (*ShellEntryPoint)(TAny *);

/** Creates the key or pointer click plug-in provider interface.

When a key is pressed, a key auto repeats or when the screen is tapped with a pointer, 
the window server informs the plug-in DLL, which can then make the sound.

@publishedAll
@released
*/
typedef CClickMaker *(*CreateCClickHandler)();

/** Creates the animation dll factory interface object.

Animation DLL objects are created by the CreateCAnimDllL() function,
which is called by the window server at the request of the client.

@publishedAll
@released
*/
typedef CAnimDll* (*CreateCAnimDll)();


class WservEncoding
	{
public:
	static inline TUint Encode8BitValues(TUint8 aFirst,TUint8 aSecond);
	static inline TUint Encode8BitValues(TUint8 aFirst,TUint8 aSecond,TUint8 aThird);
	static inline TUint8 ExtractFirst8BitValue(TUint aUint);
	static inline TUint8 ExtractSecond8BitValue(TUint aUint);
	static inline TUint8 ExtractThird8BitValue(TUint aUint);
	};

struct SKeyRepeatSettings
	{
	SKeyRepeatSettings();
	TTimeIntervalMicroSeconds32 iInitialTime;
	TTimeIntervalMicroSeconds32 iTime;
	};
struct SDoubleClickSettings
	{
	SDoubleClickSettings();
	TTimeIntervalMicroSeconds32 iInterval;
	TInt iDistance;
	};
//struct SMaxNumColors
struct SDefModeMaxNumColors
	{
	SDefModeMaxNumColors();
	TInt iColors;
	TInt iGrays;
	TDisplayMode iDisplayMode;
	};
struct SEventMessageReady
	{
	TInt iWindowGroupIdentifier;
	TUid iMessageUid;
	TInt iMessageParametersSize;
	};
class TCmdSpriteMember
	{
public:
	inline TCmdSpriteMember();
	TCmdSpriteMember(const TSpriteMember &aSpriteMember);
public:
	TInt iBitmap;
	TInt iMaskBitmap;
	TBool iInvertMask;
	CGraphicsContext::TDrawMode iDrawMode;
	TPoint iOffset;			// Offset from sprites central position
	TTimeIntervalMicroSeconds32 iInterval;
	};

enum {EClientBufferSize=640};		// default buffer size
enum {EClientBufferMaxSize=0x4000};	// 16K is max buffer size

enum {EWsOpcodeHandle=0x8000};	// Indicates a new handle is included in the message header

enum {EMaxPolygonPoints=50}; // Max size polygon that can be drawn without allocing
enum {EMaxPolylinePoints=50}; // Max size polygon that can be drawn without allocing

enum {EClickLoaded=0x01, EClickLoadable=0x02};

////////////////////////
// Opcodes
////////////////////////

// Client opcodes

enum TWsClientOpcodes
	{
	EWsClOpDisconnect, //Keep this as the first enum value - used by test code
	EWsClOpClearHotKeys,
	EWsClOpSetHotKey,
	EWsClOpRestoreDefaultHotKey,
	EWsClOpComputeMode,
	EWsClOpEventReady,
	EWsClOpEventReadyCancel,
	EWsClOpGetEvent,
	EWsClOpPurgePointerEvents,
	EWsClOpRedrawReady,
	EWsClOpRedrawReadyCancel,
	EWsClOpGetRedraw,
	EWsClOpPriorityKeyReady,
	EWsClOpPriorityKeyReadyCancel,
	EWsClOpGetPriorityKey,
	EWsClOpSetShadowVector,
	EWsClOpShadowVector,
	EWsClOpSetKeyboardRepeatRate,
	EWsClOpGetKeyboardRepeatRate,
	EWsClOpSetDoubleClick,
	EWsClOpGetDoubleClickSettings,
	EWsClOpCreateWindowGroup,
	EWsClOpCreateWindow,
	EWsClOpCreateGc,
	EWsClOpCreateAnimDll,
	EWsClOpCreateScreenDevice,
	EWsClOpCreateSprite,
	EWsClOpCreatePointerCursor,
	EWsClOpStartSetCustomTextCursor,
	EWsClOpCompleteSetCustomTextCursor,
	EWsClOpCreateBitmap,
	EWsClOpNumWindowGroups,
	EWsClOpNumWindowGroupsAllPriorities,
	EWsClOpWindowGroupList,
	EWsClOpWindowGroupListAllPriorities,
	EWsClOpSetWindowGroupOrdinalPosition,
	EWsClOpGetWindowGroupHandle,
	EWsClOpGetWindowGroupOrdinalPriority,
	EWsClOpGetWindowGroupClientThreadId,
	EWsClOpGetWindowGroupNameFromIdentifier,
	EWsClOpGetFocusWindowGroup,
	EWsClOpGetDefaultOwningWindow,
	EWsClOpSendEventToWindowGroup,
	EWsClOpFindWindowGroupIdentifier,
	EWsClOpFindWindowGroupIdentifierThread,
	EWsClOpSetBackgroundColor,
	EWsClOpGetBackgroundColor,
	EWsClOpSetSystemPointerCursor,
	EWsClOpClearSystemPointerCursor,
	EWsClOpClaimSystemPointerCursorList,
	EWsClOpFreeSystemPointerCursorList,
	EWsClOpGetModifierState,
	EWsClOpSetModifierState,
	EWsClOpHeapCount,
	EWsClOpResourceCount,
	EWsClOpTestInvariant,
	EWsClOpHeapSetFail,
	EWsClOpRawEvent,
	EWsClOpSystemInfo,
	EWsClOpLogMessage,
	EWsClOpPasswordEntered,
	EWsClOpSendMessageToWindowGroup,
	EWsClOpSendOffEventsToShell,
	EWsClOpGetDefModeMaxNumColors,
	EWsClOpGetColorModeList,
	EWsClOpKeyEvent,
	EWsClOpSendEventToAllWindowGroup,
	EWsClOpSendEventToAllWindowGroupPriority,
	EWsClOpSetPointerCursorArea,
	EWsClOpPointerCursorArea,
	EWsClOpSetPointerCursorMode,
	EWsClOpPointerCursorMode,
	EWsClOpSetDefaultSystemPointerCursor,
	EWsClOpClearDefaultSystemPointerCursor,
	EWsClOpSetPointerCursorPosition,
	EWsClOpPointerCursorPosition,
	EWsClOpSetDefaultFadingParams,
	EWsClOpSendMessageToAllWindowGroups,
	EWsClOpSendMessageToAllWindowGroupsPriority,
	EWsClOpFetchMessage,
	EWsClOpPrepareForSwitchOff,
	EWsClOpCreateDirectScreenAccess,
	EWsClOpSetFaded,
	EWsClOpLogCommand,
	EWsClOpCreateClick,
	EWsClOpSendEventToOneWindowGroupPerClient,
#if defined(__WINS__)
	EWsClOpRemoveKeyCode,
	EWsClOpSimulateXyInput,
#endif
	EWsClOpNoFlickerFree,
	EWsClOpSetFocusScreen,
	EWsClOpGetFocusScreen,
	EWsClOpWindowGroupListAndChain,
	EWsClOpWindowGroupListAndChainAllPriorities,
	EWsClOpSetClientCursorMode,
	EWsClOpClearAllRedrawStores,
	EWsClOpCreateGraphic,
	EWsClOpGraphicMessageReady,
	EWsClOpGetGraphicMessage,
	EWsClOpGraphicMessageCancel,
	EWsClOpNumWindowGroupsOnScreen,
	EWsClOpGetNumberOfScreens, 
	EWsClOpGraphicAbortMessage,
	EWsClOpGraphicFetchHeaderMessage,
	EWsClOpDebugInfo,
	EWsClOpDebugInfoReplyBuf,
	EWsClOpRegisterSurface,
	EWsClOpUnregisterSurface,
	EWsClOpSetCloseProximityThresholds,
	EWsClOpSetHighPressureThresholds,
	EWsClOpGetEnterCloseProximityThreshold,
	EWsClOpGetExitCloseProximityThreshold,
	EWsClOpGetEnterHighPressureThreshold,
	EWsClOpGetExitHighPressureThreshold,
	EWsClOpCreateDrawableSource,
	EWsClOpCreateDirectScreenAccessRegionTrackingOnly,
	EWsClOpSendEffectCommand,
	EWsClOpRegisterTFXEffectBuf,
	EWsClOpRegisterTFXEffectIPC,
	EWsClOpUnregisterTFXEffect,
	EWsClOpUnregisterAllTFXEffect,
	EWsClOpOverrideEffectBuf,
	EWsClOpOverrideEffectIPC,
	EWsClOpIndicateAppOrientation,
	EWsClOpLastEnumValue //Keep this at the end - used by test code
	};
	
// Graphic Drawer opcodes

enum TWsGraphicDrawerOpcodes
	{
	EWsGdOpFree = 0x0000,
	EWsGdOpSendMsg,
	EWsGdOpGetGraphicId,
	EWsGdOpShareGlobally,
	EWsGdOpUnShareGlobally,
	EWsGdOpShare,
	EWsGdOpUnShare,
	EWsGdOpSendSynchronMsg,
	};
	
// types of Graphic Drawer messages to be passed from the Graphic Drawer to the client
	
enum TWsGraphicMessageType
	{
	EWsGraphMessageTypeUser = 0x00
	// max 0x03 as packed into lower two bits of client handle
	};

// Window opcodes

enum TWsWindowOpcodes
	{
	EWsWinOpFree=0x0000,
	EWsWinOpSetExtent,
	EWsWinOpSetExtentErr,
	EWsWinOpOrdinalPosition,
	EWsWinOpFullOrdinalPosition,
	EWsWinOpSetOrdinalPosition,
	EWsWinOpSetOrdinalPositionPri,
	EWsWinOpSetOrdinalPriorityAdjust,
	EWsWinOpSetPos,
	EWsWinOpSetSize,
	EWsWinOpSetSizeErr,
	EWsWinOpPosition,
	EWsWinOpAbsPosition,
	EWsWinOpSize,
	EWsWinOpActivate,
	EWsWinOpInvalidate,
	EWsWinOpInvalidateFull,
	EWsWinOpBeginRedraw,
	EWsWinOpBeginRedrawFull,
	EWsWinOpEndRedraw,
	EWsWinOpTestInvariant,
	EWsWinOpParent,
	EWsWinOpPrevSibling,
	EWsWinOpNextSibling,
	EWsWinOpChild,
	EWsWinOpInquireOffset,
	EWsWinOpPointerFilter,
	EWsWinOpSetPointerCapture,
	EWsWinOpSetPointerGrab,
	EWsWinOpClaimPointerGrab,
	EWsWinOpSetBackgroundColor,
	EWsWinOpSetOrdinalPositionErr,
	EWsWinOpSetVisible,
	EWsWinOpScroll,
	EWsWinOpScrollClip,
	EWsWinOpScrollRect,
	EWsWinOpScrollClipRect,
	EWsWinOpReceiveFocus,
	EWsWinOpAutoForeground,
	EWsWinOpSetNoBackgroundColor,
	EWsWinOpCaptureKey,
	EWsWinOpCancelCaptureKey,
	EWsWinOpCaptureKeyUpsAndDowns,
	EWsWinOpCancelCaptureKeyUpsAndDowns,
	EWsWinOpAddPriorityKey,
	EWsWinOpRemovePriorityKey,
	EWsWinOpSetTextCursor,
	EWsWinOpSetTextCursorClipped,
	EWsWinOpCancelTextCursor,
	EWsWinOpSetShadowHeight,
	EWsWinOpShadowDisabled,
	EWsWinOpGetInvalidRegion,
	EWsWinOpGetInvalidRegionCount,
	EWsWinOpSetColor,
	EWsWinOpSetCornerType,
	EWsWinOpSetShape,
	EWsWinOpBitmapHandle,
	EWsWinOpUpdateScreen,
	EWsWinOpUpdateScreenRegion,
	EWsWinOpUpdateBackupBitmap,
	EWsWinOpMaintainBackup,
	EWsWinOpName,
	EWsWinOpSetName,
	EWsWinOpSetOwningWindowGroup,
	EWsWinOpDefaultOwningWindow,
	EWsWinOpRequiredDisplayMode,
	EWsWinOpEnableOnEvents,
	EWsWinOpDisableOnEvents,
	EWsWinOpEnableGroupChangeEvents,
	EWsWinOpDisableGroupChangeEvents,
	EWsWinOpSetPointerCursor,
	EWsWinOpSetCustomPointerCursor,
	EWsWinOpRequestPointerRepeatEvent,
	EWsWinOpCancelPointerRepeatEventRequest,
	EWsWinOpAllocPointerMoveBuffer,
	EWsWinOpFreePointerMoveBuffer,
	EWsWinOpEnablePointerMoveBuffer,
	EWsWinOpDisablePointerMoveBuffer,
	EWsWinOpRetrievePointerMoveBuffer,
	EWsWinOpDiscardPointerMoveBuffer,		//Tested to here %%%
	EWsWinOpEnableModifierChangedEvents,
	EWsWinOpDisableModifierChangedEvents,
	EWsWinOpEnableErrorMessages,
	EWsWinOpDisableErrorMessages,
	EWsWinOpAddKeyRect,
	EWsWinOpRemoveAllKeyRects,
	EWsWinOpPasswordWindow,
	EWsWinOpEnableBackup,
	EWsWinOpIdentifier,
	EWsWinOpDisableKeyClick,
	EWsWinOpSetFade=EWsWinOpDisableKeyClick+3,	//Two messages removed
	EWsWinOpSetNonFading,
	EWsWinOpFadeBehind,
	EWsWinOpEnableScreenChangeEvents,
	EWsWinOpDisableScreenChangeEvents,
	EWsWinOpSendPointerEvent,
	EWsWinOpSendAdvancedPointerEvent,
	EWsWinOpGetDisplayMode,
	EWsWinOpGetIsFaded,
	EWsWinOpGetIsNonFading,
	EWsWinOpOrdinalPriority,
	EWsWinOpClearPointerCursor,
	EWsWinOpMoveToGroup,
	EWsWinOpEnableFocusChangeEvents,
	EWsWinOpDisableFocusChangeEvents,
	EWsWinOpEnableGroupListChangeEvents,
	EWsWinOpDisableGroupListChangeEvents,
	EWsWinOpCaptureLongKey,
	EWsWinOpCancelCaptureLongKey,
	EWsWinOpStoreDrawCommands,
	EWsWinOpHandleTransparencyUpdate,
	EWsWinOpSetTransparencyFactor,
	EWsWinOpSetTransparencyBitmap,
	EWsWinOpAllowChildWindowGroup,
	EWsWinOpSetTransparencyBitmapCWs, 
	EWsWinOpEnableVisibilityChangeEvents,
	EWsWinOpDisableVisibilityChangeEvents,
	EWsWinOpSetTransparencyAlphaChannel,
	EWsWinOpWindowGroupId,
	EWsWinOpSetPointerCapturePriority,
	EWsWinOpGetPointerCapturePriority,
	EWsWinOpSetTransparentRegion,
	EWsWinOpSetTransparencyPolicy,
	EWsWinOpIsRedrawStoreEnabled,
	EWsWinOpEnableOSB,
	EWsWinOpDisableOSB,
	EWsWinOpClearChildGroup,
	EWsWinOpSetChildGroup,
	EWsWinOpClientHandle,
	EWsWinOpSetBackgroundSurface,
	EWsWinOpKeyColor=EWsWinOpSetBackgroundSurface+2, 	//One message removed
	EWsWinOpSetBackgroundSurfaceConfig=EWsWinOpKeyColor+5,	//Four messages removed
	EWsWinOpRemoveBackgroundSurface=EWsWinOpSetBackgroundSurfaceConfig+2,	//One message removed
	EWsWinOpGetBackgroundSurfaceConfig=EWsWinOpRemoveBackgroundSurface+2,	//One message removed
	EWsWinOpClearRedrawStore=EWsWinOpGetBackgroundSurfaceConfig+2,	//One message removed
	EWsWinOpScreenNumber,
	EWsWinOpEnableAdvancedPointers,
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS	
	EWsWinOpSetSurfaceTransparency,
#endif
	EWsWinOpSetPurpose,
	EWsWinOpSendEffectCommand,
	EWsWinOpOverrideEffectBuf,
	EWsWinOpOverrideEffectIPC,
	EWsWinOpTestLowPriorityRedraw=0x2000,  //Specific opcode for testing redraw queue priorities
	};

// GC opcodes

enum TWsGcOpcodes
	{
	EWsGcOpFree=0x0000,
	EWsGcOpActivate,
	EWsGcOpDeactivate,
	EWsGcOpSetClippingRegion,
	EWsGcOpSetClippingRect,
	EWsGcOpCancelClippingRegion,
	EWsGcOpCancelClippingRect,
	EWsGcOpSetWordJustification,
	EWsGcOpSetCharJustification,
	EWsGcOpSetPenColor,
	EWsGcOpSetPenStyle,
	EWsGcOpSetPenSize,
	EWsGcOpSetBrushColor,
	EWsGcOpSetBrushStyle,
	EWsGcOpSetBrushOrigin,
	EWsGcOpUseBrushPattern,
	EWsGcOpDiscardBrushPattern,
	EWsGcOpDrawArc,
	EWsGcOpDrawLine,
	EWsGcOpPlot,
	EWsGcOpDrawTo,
	EWsGcOpDrawBy,
	EWsGcOpDrawPolyLine,
	EWsGcOpDrawPolyLineContinued,   //Quater Way

	EWsGcOpDrawPie,
	EWsGcOpDrawRoundRect,
	EWsGcOpDrawPolygon,
	EWsGcOpStartSegmentedDrawPolygon,
	EWsGcOpSegmentedDrawPolygonData,
	EWsGcOpDrawSegmentedPolygon,
	EWsGcOpDrawBitmap,
	EWsGcOpDrawBitmap2,
	EWsGcOpDrawBitmap3,
	EWsGcOpDrawBitmapMasked,
	EWsGcOpWsDrawBitmapMasked,
	EWsGcOpDrawText,
	EWsGcOpDrawTextPtr,
	EWsGcOpDrawTextInContextPtr,
	EWsGcOpDrawTextInContext,
	EWsGcOpDrawTextVertical,
	EWsGcOpDrawTextInContextVertical,
	EWsGcOpDrawTextInContextVerticalPtr,
	EWsGcOpDrawTextVerticalPtr,
	EWsGcOpDrawBoxTextOptimised1,
	EWsGcOpDrawBoxTextOptimised2,
	EWsGcOpDrawBoxTextInContextOptimised1,
	EWsGcOpDrawBoxTextInContextOptimised2,
	EWsGcOpDrawBoxText,
	EWsGcOpDrawBoxTextInContext,
	EWsGcOpDrawBoxTextPtr,
	EWsGcOpDrawBoxTextInContextPtr,
	EWsGcOpDrawBoxTextVertical,
	EWsGcOpDrawBoxTextInContextVertical,
	EWsGcOpDrawBoxTextVerticalPtr,
	EWsGcOpDrawBoxTextInContextVerticalPtr,
	EWsGcOpMoveBy,
	EWsGcOpMoveTo,
	EWsGcOpSetOrigin,
	EWsGcOpCopyRect,
	EWsGcOpDrawRect,
	EWsGcOpDrawEllipse,		//Half Way

	EWsGcOpGdiBlt2,
	EWsGcOpGdiBlt3,
	EWsGcOpGdiBltMasked,
	EWsGcOpGdiWsBlt2,
	EWsGcOpGdiWsBlt3,
	EWsGcOpGdiWsBltMasked,
	EWsGcOpSize,
	EWsGcOpUseFont,
	//Two unused codes deleted
	EWsGcOpDiscardFont=EWsGcOpUseFont+3,
	EWsGcOpSetUnderlineStyle,
	EWsGcOpSetStrikethroughStyle,
	EWsGcOpSetDrawMode,
	EWsGcOpTestInvariant,
	EWsGcOpClearRect,
	EWsGcOpClear,
	EWsGcOpReset,
	EWsGcOpSetDitherOrigin,
	EWsGcOpMapColors,
	EWsGcOpDrawWsGraphic, //PREQ1246 Bravo
	EWsGcOpDrawWsGraphicPtr,
//
// Local opcodes used internally access GC drawing code
//
	EWsGcOpDrawPolyLineLocal,
	EWsGcOpDrawPolyLineLocalBufLen,
	EWsGcOpDrawPolygonLocal,
	EWsGcOpDrawPolygonLocalBufLen,
	EWsGcOpDrawTextLocal,
	EWsGcOpDrawTextInContextLocal,
	EWsGcOpDrawBoxTextLocal,
	EWsGcOpDrawBoxTextInContextLocal,
	EWsGcOpDrawBitmapLocal,
	EWsGcOpDrawBitmap2Local,
	EWsGcOpDrawBitmap3Local,
	EWsGcOpDrawBitmapMaskedLocal,
	EWsGcOpGdiBlt2Local,
	EWsGcOpGdiBlt3Local,
	EWsGcOpGdiBltMaskedLocal,

//
// Local opcodes used when reading long strings
//
	EWsGcOpDrawTextPtr1,
	EWsGcOpDrawTextInContextPtr1,
	EWsGcOpDrawTextVerticalPtr1,
	EWsGcOpDrawTextInContextVerticalPtr1,
	EWsGcOpDrawBoxTextPtr1,
	EWsGcOpDrawBoxTextInContextPtr1,
	EWsGcOpDrawBoxTextVerticalPtr1,
	EWsGcOpDrawBoxTextInContextVerticalPtr1,
			
//
// New functions added leaving a space just in case it may be useful
//
	EWsGcOpSetFaded=200,
	EWsGcOpSetFadeParams,
	EWsGcOpGdiAlphaBlendBitmaps,
	EWsGcOpGdiWsAlphaBlendBitmaps,
	EWsGcOpSetOpaque,
	EWsGcOpMapColorsLocal,	
	EWsGcOpAlphaBlendBitmapsLocal,
	EWsGcOpSetShadowColor,
	EWsGcOpSetDrawTextInContext,
	EWsGcOpDrawResourceToPos,
	EWsGcOpDrawResourceToRect,
	EWsGcOpDrawResourceFromRectToRect,
	EWsGcOpDrawResourceWithData,
//
// Codes for internal use only
//
	EWsStoreAllGcAttributes=1000,
	EWsStoreClippingRegion,
	EWsGcOpFlagDrawOp=0x8000,
	};

enum TWsAnimDllOpcode
	{
	EWsAnimDllOpCreateInstance=0x0000,
	EWsAnimDllOpCommand,
	EWsAnimDllOpCommandReply,
	EWsAnimDllOpDestroyInstance,
	EWsAnimDllOpFree,
	EWsAnimDllOpCreateInstanceSprite,
	};

enum TWsScreenDeviceOpcodes
	{
	EWsSdSetableBitFlag=0x80000000,
	EWsSdOpGetScanLine=0x0000,
	EWsSdOpPixel,
	EWsSdOpTwipsSize,
	EWsSdOpPixelSize,
	EWsSdOpFree,
	EWsSdOpHorizontalTwipsToPixels,
	EWsSdOpVerticalTwipsToPixels,
	EWsSdOpHorizontalPixelsToTwips,
	EWsSdOpVerticalPixelsToTwips,
	EWsSdOpDisplayMode,
	EWsSdOpRectCompare,
	EWsSdOpPointerRect,
	EWsSdOpCopyScreenToBitmap,
	EWsSdOpCopyScreenToBitmap2,
	EWsSdOpSetScreenSizeAndRotation,
	EWsSdOpSetScreenSizeAndRotation2,
	EWsSdOpGetDefaultScreenSizeAndRotation,
	EWsSdOpGetDefaultScreenSizeAndRotation2,
	EWsSdOpGetNumScreenModes,
	EWsSdOpSetScreenMode,
	EWsSdOpGetScreenModeSizeAndRotation,
	EWsSdOpGetScreenModeSizeAndRotation2,
	EWsSdOpSetScreenModeEnforcement,
	EWsSdOpScreenModeEnforcement,
	EWsSdOpSetModeRotation,
	EWsSdOpGetRotationList,
	EWsSdOpPaletteAttributes,
	EWsSdOpSetPalette,
	EWsSdOpGetPalette,
	EWsSdOpGetScreenMode,
	EWsSdOpGetDefaultScreenModeOrigin,
	EWsSdOpGetScreenModeOrigin,
	EWsSdOpGetScreenModeScale,
	EWsSdOpGetCurrentScreenModeScale,
	EWsSdOpSetAppScreenMode,
	EWsSdOpGetScreenModeScaledOrigin,
	EWsSdOpGetCurrentScreenModeScaledOrigin,
	EWsSdOpSetCurrentScreenModeAttributes,
	EWsSdOpGetCurrentScreenModeAttributes,
	EWsSdOpGetScreenNumber,
	EWsSdOpGetScreenSizeModeList,
	EWsSdOpGetScreenModeDisplayMode,
	EWsClOpSetBackLight,
	EWsSdOpExtensionsSupported,
	EWsSdOpXDcGetNumberOfResolutions,
	EWsSdOpXDcGetResolutionsList,
	EWsSdOpXDcGetConfiguration,
	EWsSdOpXDcSetConfiguration,
	EWsSdOpXDcGetPreferredDisplayVersion,
	EWsSdOpXDcNotifyOnDisplayChange,
	EWsSdOpXDcNotifyOnDisplayChangeCancel,
	EWsSdOpXDcDisplayChangeEventEnabled,
	EWsSdOpXDmMapExtent,
	EWsSdOpXTestScreenCapture,
	EWsSdOpIsScreenModeDynamic,
	EWsSdOpXTestScreenCaptureSize,
	};

/**	Bitfield of interface extensions explicitly supported.
 * 	The client interface is welcome to group functionality in  different ways, if desired.
 * 	Operations classified as inside extensions can be called and fail safely on an individual basis.
 * 	Note that to distinguish versions, new bits may need to be used.
 *  The top bit is reserved so that error value returns can be distinguished. 
 **/    
enum TWsInterfaceExtensions
	{
	TWsSdXDisplayControl	=	0x00000001,
	TWsSdXDisplayMapping	=	0x00000002,
	TWsSdXDebugComposition	=	0x00000004,
	
	
	TWsSdXReservedErrorFlag	=	0x80000000
	};

enum TWsSpriteOpcodes
	{
	EWsSpriteOpFree=0x0000,
	EWsSpriteOpSetPosition,
	EWsSpriteOpUpdateMember,
	EWsSpriteOpUpdateMember2,
	EWsSpriteOpActivate,
	EWsSpriteOpAppendMember,
	};

enum TWsBitmapOpcodes
	{
	EWsBitmapOpFree=0x0000,
	};

enum TWsDirectOpcodes
	{
	EWsDirectOpFree=0x0000,
	EWsDirectOpRequest,
	EWsDirectOpInitFailed,
	EWsDirectOpGetRegion,
	EWsDirectOpCancel,
	EWsDirectOpGetSendQueue,
	EWsDirectOpGetRecQueue,
	};

enum TWsClickOpcodes
	{
	EWsClickOpFree=0x0000,
	EWsClickOpIsLoaded,
	EWsClickOpUnLoad,
	EWsClickOpLoad,
	EWsClickOpCommandReply,
	EWsClickOpSetKeyClick,
	EWsClickOpSetPenClick,
	EWsClickOpKeyClickEnabled,
	EWsClickOpPenClickEnabled,
	};
	
enum TWsGraphicBitFlags
	{
	EWsGraphicIdUid			= 0x01,
	EWsGraphicIdTransient	= 0x02,
	EWsGraphicReplace		= 0x04
	};

enum TWsDrawableSourceOpcodes
	{
	EWsDrawableSourceOpFree=0x0000,
	};

////////////////////////
// Command structures
////////////////////////

/** Structure used to pass commands between the client and the server.

@internalComponent
@released
@see RWsBuffer::DoWrite()
@see CWsClient::DoCommandBufL()
*/
struct TWsCmdHeaderBase
	{
	TUint16 iOpcode;
	TInt16 iCmdLength;
	};

/** Structure used to pass commands between the client and the server.

@internalComponent
@released
@see RWsBuffer::DoWrite()
@see CWsClient::DoCommandBufL()
*/
struct TWsCmdHeader
	{
	TWsCmdHeaderBase iBase;
	TInt32 iDestHandle;
	};

//
// Command structures
//

// Client command structures

enum TWinTypes
	{
	EWinRedraw,
	EWinBackedUp,
	EWinBlank,
	};

struct TWsClCmdCreateWindow
	{
	TInt parent;
	TUint32 clientHandle;
	TWinTypes type;
	TDisplayMode displayMode;	// Only used for backed up windows
	};
struct TWsClCmdCreateWindowGroup
	{
	TUint32 clientHandle;
	TBool focus;
	TInt parentId;
	TInt32 screenDeviceHandle;
	};
struct TWsClCmdLoadAnimDll
	{
	TInt length;
	};
struct TWsClCmdCreatePointerCursor
	{
	TUint flags;
	};
struct TWsClCmdCustomTextCursorData
	{
	TInt identifier;
	TUint flags;
	RWsSession::TCustomTextCursorAlignment alignment;
	};
struct TWsClCmdCreateSprite
	{
	inline TWsClCmdCreateSprite(TInt aWindow, const TPoint &aPos, TUint aFlags);
	TInt window;
	TPoint pos;
	TUint flags;
	};
struct TWsClCmdCreateBitmap
	{
	TInt handle;
	};
struct TWsClCmdSetHotKey
	{
	TUint type;
	TUint modifiers;
	TUint modifierMask;
	TUint16 keycode;
	TUint16 filler;
	};
struct TWsClCmdWindowGroupList
	{
	TInt priority;
	TInt count;
	TInt screenNumber;
	};
struct TWsClCmdSetWindowGroupOrdinalPosition
	{
	inline TWsClCmdSetWindowGroupOrdinalPosition(TInt aIdentifier, TInt aPosition);
	TInt identifier;
	TInt position;
	};
struct TWsClCmdSetKeyboardRepeatRate
	{
	inline TWsClCmdSetKeyboardRepeatRate(const TTimeIntervalMicroSeconds32 &aInitial,const TTimeIntervalMicroSeconds32 &aTime);
	TTimeIntervalMicroSeconds32 initial;
	TTimeIntervalMicroSeconds32 time;
	};
struct TWsClCmdSetDoubleClick
	{
	inline TWsClCmdSetDoubleClick(const TTimeIntervalMicroSeconds32 &aInterval,TInt aDistance);
	TTimeIntervalMicroSeconds32 interval;
	TInt distance;
	};
struct TWsClCmdSetSystemPointerCursor
	{
	TInt handle;
	TInt number;
	};
struct TWsClCmdHeapSetFail
	{
	RHeap::TAllocFail type;
	TInt value;
	TInt burst;
	};
struct TWsClCmdSendEventToWindowGroup
	{
	inline TWsClCmdSendEventToWindowGroup(TInt aParameter, const TWsEvent &aEvent);
	TInt parameter;
	TWsEvent event;
	};
struct TWsClCmdFindWindowGroupIdentifier
	{
	inline TWsClCmdFindWindowGroupIdentifier(TInt aPreviousIdentifier,TInt aOffset, TInt aLength);
	TInt identifier;
	TInt offset;
	TInt length;
	};
struct TWsClCmdSendMessageToWindowGroup
	{
	inline TWsClCmdSendMessageToWindowGroup(TInt aIdentifier, TUid aUid, TInt aDataLength, const TDesC8 *aPtr);
	TInt identifierOrPriority; // identifier in case of EWsClOpSendMessageToWindowGroup, not used in case of EWsClOpSendMessageToAllWindowGroup, priority in case of EWsClOpSendMessageToAllWindowGroupPriority
	TUid uid;
	TInt dataLength;
	const TDesC8 *ptr;
	};
struct TWsClCmdFetchMessage
	{
	inline TWsClCmdFetchMessage(TInt aWindowGroupIdentifier);
	TInt windowGroupIdentifier;
	};
struct TWsClCmdFindWindowGroupIdentifierThread
	{
	inline TWsClCmdFindWindowGroupIdentifierThread(TInt aPreviousIdentifier,TThreadId aThreadId);
	TInt identifier;
	TThreadId threadId;
	};
struct TWsClCmdGetWindowGroupNameFromIdentifier
	{
	inline TWsClCmdGetWindowGroupNameFromIdentifier(TInt aIdentifier,TInt aMaxLength);
	TInt identifier;
	TInt maxLength;
	};
struct TWsClCmdOffEventsToShell
	{
	inline TWsClCmdOffEventsToShell(TBool aOn,TUint32 aWindow);
	TBool on;
	TUint32 window;
	};
struct TWsClCmdSetModifierState
	{
	TEventModifier modifier;
	TModifierState state;
	};
struct TWsClCmdSetPointerCursorArea
	{
	inline TWsClCmdSetPointerCursorArea(TInt aMode,TRect aArea);
	TInt mode;
	TRect area;
	};
struct TWsClCmdSetSystemFaded
	{
	enum {EFaded=0x01,EUseDefaultMap=0x02};
	inline TWsClCmdSetSystemFaded(TBool aFaded,TBool aUseDefaults=ETrue,TUint8 aBlackMap=0,TUint8 aWhiteMap=0);
	inline TBool Faded() const;
	inline TBool UseDefaultMap() const;
	inline void GetFadingParams(TUint8& aBlackMap,TUint8& aWhiteMap) const;
	TUint params;
	};
struct TWsClCmdCreateGraphic
	{
	TUint iFlags; // TWsGraphicBitFlags
	TInt iClientHandle;
	TInt iId;
	TUid iType;
	TInt iDataLen;
	TBool iRemoteReadData;
	};
struct TWsClCmdGdSendMessage
	{
	TInt iDataLen;
	TBool iRemoteReadData;
	};
struct TWsClCmdGdGetId
	{
	TInt iId;
	TBool iIsUid;
	};
struct TWsClCmdDebugInfo
	{
	inline TWsClCmdDebugInfo(TInt aFunction, TInt aParam);
	TInt iFunction;
	TInt iParam;
	};
struct TWsClCmdCreateDrawableSource;
struct TWsClCmdSurfaceRegister;
#if defined(SYMBIAN_GRAPHICS_GCE)
struct TWsClCmdCreateDrawableSource
	{
	inline TWsClCmdCreateDrawableSource(const TSgDrawableId& aDrawableId, TInt aScreenNumber);
	TSgDrawableId iDrawableId;
	TInt iScreenNumber;
	};
struct TWsClCmdSurfaceRegister
	{
	inline TWsClCmdSurfaceRegister(TInt aScreenNumber, const TSurfaceId& aSurface);
	TInt screenNumber;
	TSurfaceId surfaceId;
	};
#endif
struct TWsClCmdNumWindowGroups
	{
	TInt screenNumber;
	TInt priority;	
	};
struct TWsClCmdCreateScreenDevice
	{
	TInt screenNumber;	
	TUint clientScreenDevicePointer;
	};
struct TWsClCmdZThresholdPair
	{
	inline TWsClCmdZThresholdPair(const TInt aEnterThreshold, const TInt aExitThreshold);
	TInt enterThreshold;
	TInt exitThreshold;
	};
struct TWsClCmdSendEffectCommand
	{
	inline TWsClCmdSendEffectCommand(TInt aTfxCmd,TInt aTfxCmdDataLength, TInt aWinHandle);
	TInt tfxCmd;
	TInt tfxCmdDataLength;
	TInt windowHandle;
	};
struct TWsClCmdRegisterEffect
    {
    inline TWsClCmdRegisterEffect(TInt aAction, TInt aPurpose, TInt aDirStrSize, TInt aPhase1StrSize, TInt aPhase2StrSize, TUint aAppUid, TBitFlags aFlags);
    TInt tfxAction;
    TInt tfxPurpose;
    TInt tfxDirStrSize;
    TInt tfxPhase1StrSize;
    TInt tfxPhase2StrSize;
    TUint tfxAppUid;
    TBitFlags tfxFlags;
    };
struct TWsClCmdUnRegisterEffect
    {
    inline TWsClCmdUnRegisterEffect(TInt aAction, TInt aPurpose, TUint aAppUid);
    TInt tfxAction;
    TInt tfxPurpose;
    TUint tfxAppUid;
    };
struct TWsClCmdOverrideEffect
    {
    inline TWsClCmdOverrideEffect(TInt aAction, TInt aPurpose, TInt aDirStrSize, TInt aPhase1StrSize, TInt aPhase2StrSize, TBitFlags aFlags);
    TInt tfxAction;
    TInt tfxPurpose;
    TInt tfxDirStrSize;
    TInt tfxPhase1StrSize;
    TInt tfxPhase2StrSize;
    TBitFlags tfxFlags;
    };
typedef TRequestStatus *RqStatPtr;
union TWsClCmdUnion
	{
	const TAny* any;
	const TAny** pAny;
	const TInt* Int;
	const TUint* UInt;
	const TBool* Bool;
	const TUid* Uid;
	const TPointerCursorMode* Mode;
    const TInt *XyInput;
	const TPoint* Point;
	const TRgb* rgb;
	const RqStatPtr* RequestStatus;
	const TRawEvent* RawEvent;
	const TKeyEvent* KeyEvent;
	const RWsSession::TComputeMode* ComputeMode;
	const RWsSession::TLoggingCommand* LogCommand;
	const TWsClCmdCreateWindow* CreateWindow;
	const TWsClCmdCreateWindowGroup* CreateWindowGroup;
	const TWsClCmdLoadAnimDll* LoadAnimDll;
	const TWsClCmdCreatePointerCursor* CreatePointerCursor;
	const TWsClCmdCustomTextCursorData* CustomTextCursorData;
	const TWsClCmdCreateSprite* CreateSprite;
	const TWsClCmdCreateBitmap* CreateBitmap;
	const TWsClCmdSetHotKey* SetHotKey;
	const TWsClCmdWindowGroupList* WindowGroupList;
	const TWsClCmdSetWindowGroupOrdinalPosition* SetWindowGroupOrdinalPosition;
	const TWsClCmdSetKeyboardRepeatRate* SetKeyboardRepeatRate;
	const TWsClCmdHeapSetFail* HeapSetFail;
	const TWsClCmdSetDoubleClick* SetDoubleClick;
	const TWsClCmdSetSystemPointerCursor* SetSystemPointerCursor;
	const TWsClCmdSendEventToWindowGroup* SendEventToWindowGroup;
	const TWsClCmdSendMessageToWindowGroup* SendMessageToWindowGroup;
	const TWsClCmdFetchMessage* FetchMessage;
	const TWsClCmdFindWindowGroupIdentifier* FindWindowGroupIdentifier;
	const TWsClCmdFindWindowGroupIdentifierThread* FindWindowGroupIdentifierThread;
	const TWsClCmdGetWindowGroupNameFromIdentifier* GetWindowGroupNameFromIdentifier;
	const TWsClCmdOffEventsToShell* OffEventsToShell;
	const TWsClCmdSetModifierState* SetModifierState;
	const TWsClCmdSetPointerCursorArea* SetPointerCursorArea;
	const TWsClCmdSetSystemFaded* SetSystemFaded;
	const TWsClCmdCreateGraphic* CreateGraphic;
	const TWsClCmdGdSendMessage* GraphicSendMessage;
	const TWsClCmdGdGetId* GetGraphicId;
	const TWsClCmdDebugInfo* DebugInfo;
	const TWsClCmdSurfaceRegister* SurfaceRegister;
	const TWsClCmdNumWindowGroups* NumWinGroups;
	const TWsClCmdCreateScreenDevice* CreateScreenDevice;
	const TWsClCmdZThresholdPair* ZThresholdPair;
	const TWsClCmdCreateDrawableSource* CreateDrawableSource;
	const TWsClCmdSendEffectCommand* SendEffectCommand;
	const TWsClCmdRegisterEffect* RegisterEffect;
	const TWsClCmdUnRegisterEffect* UnRegisterEffect; 
	const TWsClCmdOverrideEffect* OverrideEffect;
	const TRenderOrientation* Orientation;
	};

// Window command structures

struct TWsWinCmdSetExtent
	{
	inline TWsWinCmdSetExtent(const TPoint &aPos,const TSize &aSize);
	TPoint pos;
	TSize size;
	};
struct TWsWinCmdOrdinalPos
	{
	TInt pos;
	TInt ordinalPriority;
	};
struct TWsWinCmdScroll
	{
	inline TWsWinCmdScroll(const TRect &aClip,const TPoint &aOffset,const TRect &aRect);
	TRect clip;
	TPoint offset;
	TRect rect;
	};
struct TWsWinCmdCaptureKey
	{
	TUint modifiers;
	TUint modifierMask;
	TUint key;
	TInt priority;
	};
struct TWsWinCmdCaptureLongKey
	{
	inline TWsWinCmdCaptureLongKey();
	inline TWsWinCmdCaptureLongKey(TUint aModifiers,TUint aModifierMask,TUint aInputKey,TUint aOutputKey
																	,TTimeIntervalMicroSeconds32 aDelay,TInt aPriority,TUint aFlags);
	TUint modifiers;
	TUint modifierMask;
	TUint inputKey;
	TUint outputKey;
	TTimeIntervalMicroSeconds32 delay;
	TInt priority;
	TUint flags;
	};
struct TWsWinCmdPriorityKey
	{
	TUint modifiers;
	TUint modifierMask;
	TUint keycode;
	};
struct TWsWinCmdSetTextCursor
	{
	inline TWsWinCmdSetTextCursor(TUint32 aWindow, const TPoint &aPos, const TTextCursor &aCursor, const TRect &aRect);
	inline TWsWinCmdSetTextCursor(TUint32 aWindow, const TPoint &aPos, const TTextCursor &aCursor);
	TUint32 window;
	TPoint pos;
	TTextCursor cursor;
	TRect rect;
	};
struct TWsWinCmdPointerFilter
	{
	TUint32 mask;
	TUint32 flags;
	};
struct TWsWinCmdSetCornerType
	{
	TCornerType type;
	TInt flags;
	};
struct TWsWinCmdSetShape
	{
	TPtrC8 *ptr;
	TInt count;
	};
struct TWsWinCmdUpdateScreen
	{
	TPtrC8 *ptr;
	TInt count;
	};
struct TWsWinCmdGrabControl
	{
	enum TWsWinCmdGrabControlFlags
		{
		ENone=0,
		ESendUpEvent=0x2,
		ESendReply=0x4
		};
	inline TWsWinCmdGrabControl(const TUint aGrabControlFlags);
	inline TWsWinCmdGrabControl(const TUint8 aPointerNumber, const TUint aGrabControlFlags);
	inline TBool CheckFlags(TUint aFlags) const;
	inline TBool HasPointerNumber() const;
	
	// integer to ensure passed in unsigned TUint8 can't get confused with
	// signed negative values that are used internally.
	// The signed int member will be "promoted" to unsigned for the assignement in the constructor.
	TInt pointerNumber;
	TUint flags;	
	};

struct TWsWinCmdCancelPointerRepeatEventRequest
	{
	enum TCancelRepeatFlags
		{
		ECancelRepeatFlagsNone=0,
		ECancelRepeatFlagsSendReply=0x2
		};	
	
	inline TWsWinCmdCancelPointerRepeatEventRequest();
	inline TWsWinCmdCancelPointerRepeatEventRequest(const TUint8 aPointerNumber);	
	inline TWsWinCmdCancelPointerRepeatEventRequest(const TUint8 aPointerNumber, const TUint aFlags);
	inline TBool HasPointerNumber() const;
	TInt pointerNumber;
	TUint flags;
	};

struct TWsWinCmdRequestPointerRepeatEvent
	{
	enum TRepeatFlags
		{
		ERepeatFlagsNone=0,
		ERepeatFlagsSendReply=0x2
		};
	
	inline TWsWinCmdRequestPointerRepeatEvent(const TTimeIntervalMicroSeconds32 &aTime,const TRect &aRect);
	inline TWsWinCmdRequestPointerRepeatEvent(const TTimeIntervalMicroSeconds32 &aTime,const TRect &aRect,const TUint8 aPointerNumber, const TUint aRepeatFlags);
	inline TBool HasPointerNumber() const;
	TTimeIntervalMicroSeconds32 time;
	TRect rect;
	TInt pointerNumber;
	TUint repeatFlags;
	};
struct TWsWinCmdAllocPointerMoveBuffer
	{
	TInt maxNumPoints;
	TInt flags;
	};
struct TWsWinCmdSetName
	{
	TInt length;
	const TDesC *ptr;
	};
struct TWsWinCmdAddKeyRect
	{
	inline TWsWinCmdAddKeyRect(const TRect &aRect,TInt aScanCodea, TBool aActivatedByPointerSwitchOn);
	TRect rect;
	TInt scanCode;
	TBool activatedByPointerSwitchOn;
	};
struct TWsWinOpSetBackgroundSurfaceConfig;
#if defined(SYMBIAN_GRAPHICS_GCE)
struct TWsWinOpSetBackgroundSurfaceConfig
	{
	inline TWsWinOpSetBackgroundSurfaceConfig(const TSurfaceConfiguration& aConfiguration, TBool aTriggerRedraw);
	TSurfaceConfiguration surfaceConfig;
	TBool triggerRedraw;
	};
#endif
struct TWsWinCmdEnableModifierChangedEvents
	{
	inline TWsWinCmdEnableModifierChangedEvents(TUint aModifierMask, TEventControl aCircumstances);
	TInt modifierMask;
	TEventControl circumstances;
	};
struct TWsWinCmdSetFaded
	{
	enum {EFaded=0x01,EIncludeChildren=0x02,EUseDefaultMap=0x04};
	inline TWsWinCmdSetFaded(TBool aFaded, TBool aIncludeChildren
														,TBool aUseDefaults=ETrue,TUint8 aBlackMap=0,TUint8 aWhiteMap=0);
	inline TBool Faded() const;
	inline TBool IncludeChildren() const;
	inline TBool UseDefaultMap() const;
	inline void GetFadingParams(TUint8& aBlackMap,TUint8& aWhiteMap) const;
	TUint params;
	};

union TWsWinCmdUnion
	{
	const TAny *any;
	const TAny **pAny;
	const TRect *rect;
	const TPoint *pos;
	const TSize *size;
	const TInt *Int;
	const TUint *UInt;
	const TBool *Bool;
	const TEventControl *EventControl;
	const TRgb *rgb;
	const TDisplayMode *DisplayMode;
	const TPasswordMode *PasswordMode;
	const TRawEvent *RawEvent;
	const TSurfaceConfiguration *SurfaceConfiguration;
	const TWsWinCmdSetExtent *SetEx;
	const TWsWinCmdOrdinalPos *OrdinalPos;
	const TWsWinCmdScroll *ScrollRect;
	const TWsWinCmdCaptureKey *CaptureKey;
	const TWsWinCmdCaptureLongKey *CaptureLongKey;
	const TWsWinCmdPriorityKey *PriorityKey;
	const TWsWinCmdSetTextCursor *SetTextCursor;
	const TWsWinCmdPointerFilter *PointerFilter;
	const TWsWinCmdSetCornerType *SetCornerType;
	const TWsWinCmdSetShape *SetShape;
	const TWsWinCmdUpdateScreen *UpdateScreen;
	const TWsWinCmdRequestPointerRepeatEvent *RequestPointerRepeatEvent;
	const TWsWinCmdAllocPointerMoveBuffer *AllocPointerMoveBuffer;
	const TWsWinCmdSetName *SetName;
	const TWsWinCmdAddKeyRect *AddKeyRect;
	const TWsWinOpSetBackgroundSurfaceConfig *SurfaceConfigurationAndTrigger;
	const TWsWinCmdEnableModifierChangedEvents *EnableModifierChangedEvents;
	const TWsWinCmdSetFaded *SetFaded;
	const TSurfaceId *Surface;
	const TWsWinCmdCancelPointerRepeatEventRequest *CancelPointerRepeatEventRequest;
	const TWsWinCmdGrabControl *GrabControl;
	const TWsClCmdSendEffectCommand *SendEffectCommand;
	const TWsClCmdOverrideEffect* OverrideEffect;
	};

// GC command structures

struct TWsGcLargeStruct
	{
	TInt int1;
	TInt int2;
	TInt int3;
	TInt int4;
	TInt int5;
	TInt int6;
	TInt int7;
	TInt int8;
	TInt int9;
	TBool tbool;
	CGraphicsContext::TTextAlign align;
	TAny* ptr;
	};
struct TWsGcCmdSetClippingRegion
	{
	TPtrC8 *ptr;
	TInt count;
	};
struct TWsGcCmdSetJustification
	{
	TInt excessWidth;
	TInt numGaps;
	};
struct TWsGcCmdDrawText
	{
	inline TWsGcCmdDrawText(const TPoint &aPos, TInt aLength);
	TPoint pos;
	TInt length;
	};
struct TWsGcCmdDrawTextPtr
	{
	TPoint pos;
	TDesC *text;
	};


struct TWsGcCmdDrawTextVertical
	{
	inline TWsGcCmdDrawTextVertical(const TPoint &aPos, TInt aLength, TBool aUp);
	TPoint pos;
	TInt length;
	TBool up;
	};

struct TWsGcCmdDrawTextVerticalPtr
	{
	TPoint pos;
	TBool up;
	TDesC *text;
	inline TWsGcCmdDrawTextVerticalPtr(const TDesC *aDesc, const TPoint& aPos, const TBool aUp);
	};

struct TWsGcCmdDrawTextLocal
	{
	inline TWsGcCmdDrawTextLocal(const TPoint &aPos,const TDesC *aDesc);
	TPoint pos;
	const TDesC *desc;
	};

struct TWsGcCmdBoxTextOptimised1
	{
	inline TWsGcCmdBoxTextOptimised1(const TRect &aRect,TInt aBaselineOffset,TInt alength);
	TRect box;
	TInt baselineOffset;
	TInt length;
	};
struct TWsGcCmdBoxTextOptimised2
	{
	inline TWsGcCmdBoxTextOptimised2(const TRect &aRect,TInt aBaselineOffset,CGraphicsContext::TTextAlign aHoriz,TInt aLeftMrg,TInt aLength);
	TRect box;
	TInt baselineOffset;
	CGraphicsContext::TTextAlign horiz;
	TInt leftMrg;
	TInt length;
	};

struct TWsGcCmdBoxText
	{
	inline TWsGcCmdBoxText(const TRect &aRect,TInt aBaselineOffset,CGraphicsContext::TTextAlign aHoriz,TInt aLeftMrg,TInt aLength,TInt aWidth);
	TRect box;
	TInt baselineOffset;
	CGraphicsContext::TTextAlign horiz;
	TInt leftMrg;
	TInt length;
	TInt width;
	};

struct TWsGcCmdBoxTextPtr
	{
	TRect box;
	TInt baselineOffset;
	CGraphicsContext::TTextAlign horiz;
	TInt leftMrg;
	TInt width;
	TDesC *text;
	};

struct TWsGcCmdBoxTextVertical
	{
	inline TWsGcCmdBoxTextVertical(const TRect& aBox);
	TRect box;
	TInt baselineOffset;
	TBool up;
	CGraphicsContext::TTextAlign vert;
	TInt margin;
	TInt length;
	TInt width;
	};

struct TWsGcCmdBoxTextVerticalPtr
	{
	TRect box;
	TInt baselineOffset;
	TBool up;
	CGraphicsContext::TTextAlign vert;
	TInt margin;
	TInt width;
	TDesC *text;
	};

struct TWsGcCmdBoxTextLocal
	{
	inline TWsGcCmdBoxTextLocal(const TRect &aBox);
	TRect box;
	TInt baselineOffset;
	CGraphicsContext::TTextAlign horiz;
	TInt leftMrg;
	const TDesC *desc;
	};

struct TWsGcCmdDrawLine
	{
	inline TWsGcCmdDrawLine(const TPoint &aPnt1,const TPoint &aPnt2);
	TPoint pnt1;
	TPoint pnt2;
	};
struct TWsGcCmdDrawPolyLine
	{
	TInt numPoints;
	TInt more;
	TPoint last;
	};
struct TWsGcCmdDrawArcOrPie
	{
	inline TWsGcCmdDrawArcOrPie(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd);
	TRect rect;
	TPoint start;
	TPoint end;
	};
struct TWsGcCmdDrawRoundRect
	{
	inline TWsGcCmdDrawRoundRect(const TRect &aRect,const TSize &aEllipse);
	TRect rect;
	TSize ellipse;
	};
struct TWsGcCmdDrawPolygon
	{
	TInt numPoints;
	CGraphicsContext::TFillRule fillRule;
	};
struct TWsGcCmdStartSegmentedDrawPolygon
	{
	TInt totalNumPoints;
	};
struct TWsGcCmdSegmentedDrawPolygonData
	{
	const TDesC8 *points;
	TInt index;
	TInt numPoints;
	};
struct TWsGcCmdDrawSegmentedPolygon
	{
	CGraphicsContext::TFillRule fillRule;
	};
struct TWsGcCmdDrawPolygonLocal
	{
	const CArrayFix<TPoint> *pointList;
	CGraphicsContext::TFillRule fillRule;
	};
struct TWsGcCmdDrawPolygonLocalBufLen
	{
	inline TWsGcCmdDrawPolygonLocalBufLen(const TPoint *aPointList,TInt aNumPoints, CGraphicsContext::TFillRule aFillRule);
	const TPoint *points;
	TInt length;
	CGraphicsContext::TFillRule fillRule;
	};
struct TWsGcCmdDrawPolyLineLocalBufLen
	{
	inline TWsGcCmdDrawPolyLineLocalBufLen(const TPoint *aPointList,TInt aNumPoints);
	const TPoint *points;
	TInt length;
	};
struct TWsGcCmdDrawBitmap
	{
	inline TWsGcCmdDrawBitmap(const TPoint &aPoint,TInt aHandle);
	TInt handle;
	TPoint pos;
	};
struct TWsGcCmdDrawBitmap2
	{
	inline TWsGcCmdDrawBitmap2(const TRect &aRect,TInt aHandle);
	TInt handle;
	TRect rect;
	};
struct TWsGcCmdDrawBitmap3
	{
	inline TWsGcCmdDrawBitmap3(const TRect &aRect,TInt aHandle,const TRect &aSrcRect);
	TInt handle;
	TRect rect;
	TRect srcRect;
	};
struct TWsGcCmdDrawBitmapMasked
	{
	inline TWsGcCmdDrawBitmapMasked(const TRect &aDestRect,TInt aHandle,const TRect& aSrcRect,TInt aMaskHandle,TBool aInvertMask);
	TInt iHandle;
	TInt iMaskHandle;
	TRect iRect;
	TRect iSrcRect;
	TBool iInvertMask;
	};
struct TWsGcCmdDrawBitmapLocal
	{
	inline TWsGcCmdDrawBitmapLocal(const TPoint &aPos,const CFbsBitmap *aBitmap);
	const CFbsBitmap *bitmap;
	TPoint pos;
	};
struct TWsGcCmdDrawBitmap2Local
	{
	inline TWsGcCmdDrawBitmap2Local(const TRect &aRect,const CFbsBitmap *aBitmap);
	const CFbsBitmap *bitmap;
	TRect rect;
	};
struct TWsGcCmdDrawBitmap3Local
	{
	inline TWsGcCmdDrawBitmap3Local(const TRect &aRect,const CFbsBitmap *aBitmap, const TRect &aSrcRect);
	const CFbsBitmap *bitmap;
	TRect rect;
	TRect srcRect;
	};
struct TWsGcCmdDrawBitmapMaskedLocal
	{
	inline TWsGcCmdDrawBitmapMaskedLocal(const TRect &aDestRect,const CFbsBitmap* aBitmap,const TRect& aSrcRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask);
	const CFbsBitmap* iBitmap;
	const CFbsBitmap* iMaskBitmap;
	TRect iRect;
	TRect iSrcRect;
	TBool iInvertMask;
	};
struct TWsGcCmdCopyRect
	{
	inline TWsGcCmdCopyRect(const TPoint &anOffset,const TRect &aRect);
	TRect rect;		// Must be first
	TPoint pos;
	};
struct TWsGcCmdGdiBlt2
	{
	inline TWsGcCmdGdiBlt2(const TPoint &aPoint, TInt aHandle);
	TInt handle;
	TPoint pos;
	};
struct TWsGcCmdGdiBlt3
	{
	inline TWsGcCmdGdiBlt3(const TPoint &aPoint, TInt aHandle, const TRect &aSrcRect);
	TInt handle;
	TPoint pos;
	TRect rect;
	};
struct TWsGcCmdGdiBlt2Local
	{
	inline TWsGcCmdGdiBlt2Local(const TPoint &aPoint,const CFbsBitmap *aBitmap);
	const CFbsBitmap *bitmap;
	TPoint pos;
	};
struct TWsGcCmdGdiBlt3Local
	{
	inline TWsGcCmdGdiBlt3Local(const TPoint &aDestination,const CFbsBitmap *aBitmap,const TRect &aSource);
	const CFbsBitmap *bitmap;
	TPoint pos;
	TRect rect;
	};
struct TWsGcCmdGdiBltMaskedLocal
	{
	inline TWsGcCmdGdiBltMaskedLocal(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask);
	const CFbsBitmap *bitmap;
	const CFbsBitmap *maskBitmap;
	TBool invertMask;
	TPoint pos;
	TRect rect;
	};
struct TWsGcCmdUsePatternBrush
    {
    TRgb *pdata;
    TInt width;
    TInt height;
    TBool color;
	};
struct TWsGcCmdBltMasked
	{
	inline TWsGcCmdBltMasked();
	inline TWsGcCmdBltMasked(const TPoint& aPoint,TInt aBitmap,const TRect& aSourceRect,TInt aMaskBitmap,TBool aInvertMask);
	TPoint destination;
	TInt handle;
	TRect source;
	TInt maskHandle;
	TBool invertMask;
	};
struct TWsGcCmdAlphaBlendBitmaps
	{
	inline TWsGcCmdAlphaBlendBitmaps();
	inline TWsGcCmdAlphaBlendBitmaps(const TPoint& aPoint, TInt aBitmap,const TRect& aSourceRect,TInt aAlpha,const TPoint& aAlphaPoint);
	TPoint point;
	TInt bitmapHandle;
	TRect source;
	TInt alphaHandle;
	TPoint alphaPoint;
	};

struct TWsGcCmdAlphaBlendBitmapsLocal
	{
	inline TWsGcCmdAlphaBlendBitmapsLocal(const TPoint& aPoint, const CFbsBitmap* aBitmap, const TRect& aSourceRect, const CFbsBitmap* aAlpha, const TPoint& aAlphaPoint);
	TPoint point;
	const CFbsBitmap *iBitmap;
	TRect source;
	const CFbsBitmap *iAlpha;
	TPoint alphaPoint;
	};
	
struct TWsGcCmdMapColors
	{
	inline TWsGcCmdMapColors(const TRect &aRect, TInt aNumPairs, TBool aMapForwards);
	TRect rect;
	TInt numPairs;
	TBool mapForwards;
	};
	
struct TWsGcCmdMapColorsLocal
	{
	inline TWsGcCmdMapColorsLocal(const TRect &aRect, const TRgb* aColors, TInt aNumPairs, TBool aMapForwards);
	TRect rect;
	const TRgb *colors;
	TInt numPairs;
	TBool mapForwards;
	};
	
struct TWsGcCmdDrawWsGraphic
	{
	inline TWsGcCmdDrawWsGraphic(const TWsGraphicId& aId,const TRect& aRect);
	TUint iFlags; // TWsGraphicBitFlags
	TInt iId;
	TRect iRect;
	TInt iDataLen;
	};
struct TWsGcCmdDrawTextInContext
	{
	inline TWsGcCmdDrawTextInContext(const TPoint &aPos, TInt aLength, TInt aStart, TInt aEnd);
	TPoint pos;
	TInt length;
	TInt start;
	TInt end;
	};
struct TWsGcCmdDrawTextInContextPtr
	{
	TPoint pos;
	TDesC *text;
	TInt start;
	TInt end;
	};
struct TWsGcCmdDrawTextInContextVertical
	{
	inline TWsGcCmdDrawTextInContextVertical(const TPoint &aPos, TInt aLength, TBool aUp, TInt aStart, TInt aEnd);
	TPoint pos;
	TInt length;
	TBool up;
	TInt start;
	TInt end;
	};
struct TWsGcCmdDrawTextInContextVerticalPtr
	{
	TPoint pos;
	TBool up;
	TDesC *text;
	TInt start;
	TInt end;
	inline TWsGcCmdDrawTextInContextVerticalPtr(const TDesC *aDesc, const TPoint& aPos, const TBool aUp, const TInt aStart, const TInt aEnd);
	};
struct TWsGcCmdDrawTextInContextLocal
	{
	inline TWsGcCmdDrawTextInContextLocal(const TPoint &aPos,const TDesC *aDesc,const TInt aStart,const TInt aEnd);
	TPoint pos;
	const TDesC *desc;
	TInt start;
	TInt end;
	};
struct TWsGcCmdBoxTextInContextOptimised1
	{
	inline TWsGcCmdBoxTextInContextOptimised1(const TRect &aRect,TInt aBaselineOffset,TInt alength, TInt aStart, TInt aEnd);
	TRect box;
	TInt baselineOffset;
	TInt length;
	TInt start;
	TInt end;
	};
struct TWsGcCmdBoxTextInContextOptimised2
	{
	inline TWsGcCmdBoxTextInContextOptimised2(const TRect &aRect,TInt aBaselineOffset,CGraphicsContext::TTextAlign aHoriz,TInt aLeftMrg,TInt aLength, TInt aStart, TInt aEnd);
	TRect box;
	TInt baselineOffset;
	CGraphicsContext::TTextAlign horiz;
	TInt leftMrg;
	TInt length;
	TInt start;
	TInt end;
	};
struct TWsGcCmdBoxTextInContext
	{
	inline TWsGcCmdBoxTextInContext(const TRect &aRect,TInt aBaselineOffset,CGraphicsContext::TTextAlign aHoriz,TInt aLeftMrg,TInt aLength,TInt aWidth, TInt aStart, TInt aEnd);
	TRect box;
	TInt baselineOffset;
	CGraphicsContext::TTextAlign horiz;
	TInt leftMrg;
	TInt length;
	TInt width;
	TInt start;
	TInt end;
	};
struct TWsGcCmdBoxTextInContextPtr
	{
	TRect box;
	TInt baselineOffset;
	CGraphicsContext::TTextAlign horiz;
	TInt leftMrg;
	TInt width;
	TDesC *text;
	TInt start;
	TInt end;
	};
struct TWsGcCmdBoxTextInContextVertical
	{
	inline TWsGcCmdBoxTextInContextVertical(const TRect& aBox);
	TRect box;
	TInt baselineOffset;
	TBool up;
	CGraphicsContext::TTextAlign vert;
	TInt margin;
	TInt length;
	TInt width;
	TInt start;
	TInt end;
	};
struct TWsGcCmdBoxTextInContextVerticalPtr
	{
	TRect box;
	TInt baselineOffset;
	TBool up;
	CGraphicsContext::TTextAlign vert;
	TInt margin;
	TInt width;
	TInt start;
	TInt end;
	TDesC *text;
	};
struct TWsGcCmdBoxTextInContextLocal
	{
	inline TWsGcCmdBoxTextInContextLocal(const TRect &aBox,TInt aStart,TInt aEnd);
	TRect box;
	TInt baselineOffset;
	CGraphicsContext::TTextAlign horiz;
	TInt leftMrg;
	TInt start;
	TInt end;
	const TDesC *desc;
	};
struct TWsGcCmdDrawResourceToPos
	{
	inline TWsGcCmdDrawResourceToPos(TInt aWsHandle, const TPoint &aPos, CWindowGc::TGraphicsRotation aRotation);
	TInt wsHandle;
	TPoint pos;
	CWindowGc::TGraphicsRotation rotation;
	};
struct TWsGcCmdDrawResourceToRect
	{
	inline TWsGcCmdDrawResourceToRect(TInt aWsHandle, const TRect &aRect, CWindowGc::TGraphicsRotation aRotation);
	TInt wsHandle;
	TRect rect;
	CWindowGc::TGraphicsRotation rotation;
	};
struct TWsGcCmdDrawResourceFromRectToRect
	{
	inline TWsGcCmdDrawResourceFromRectToRect(TInt aWsHandle, const TRect &aRectDest, const TRect &aRectSrc, CWindowGc::TGraphicsRotation aRotation);
	TInt wsHandle;
	TRect rectDest;
	TRect rectSrc;
	CWindowGc::TGraphicsRotation rotation;
	};
struct TWsGcCmdDrawResourceWithData
	{
	inline TWsGcCmdDrawResourceWithData(TInt aWsHandle, const TRect &aRect, const TDesC8* aDesc);
	TInt wsHandle;
	TRect rect;
	const TDesC8* desc;
	};

union TWsGcCmdUnion
	{
	const TAny *any;
	const TAny **pAny;
	const TRect *Rect;
	const TUint *UInt;
	const TInt *Int;
	const TInt *handle;
	const TBool *Bool;
	const TRgb *rgb;
	const TPoint *Point;
	const TSize *Size;
	const TFontUnderline *SetUnderlineStyle;
	const TFontStrikethrough *SetStrikethroughStyle;
	const TWsGcCmdDrawText *DrawText;
	const TWsGcCmdDrawTextInContext *DrawTextInContext;
	const TWsGcCmdDrawTextVertical *DrawTextVertical;
	const TWsGcCmdDrawTextInContextVertical *DrawTextInContextVertical;
	const TWsGcCmdDrawLine *DrawLine;
	const TWsGcCmdDrawPolyLine *PolyLine;
	const TWsGcCmdStartSegmentedDrawPolygon *StartSegmentedDrawPolygon;
	const TWsGcCmdSegmentedDrawPolygonData *SegmentedDrawPolygonData;
	const TWsGcCmdDrawSegmentedPolygon *DrawSegmentedPolygon;
	const TWsGcCmdDrawPolygonLocal *DrawPolygonLocal;
	const TWsGcCmdDrawPolygonLocalBufLen *DrawPolygonLocalBufLen;
	const TWsGcCmdDrawPolyLineLocalBufLen *DrawPolyLineLocalBufLen;
	const TWsGcCmdDrawArcOrPie *ArcOrPie;
	const TWsGcCmdDrawRoundRect *RoundRect;
	const TWsGcCmdDrawPolygon *Polygon;
	const TWsGcCmdDrawBitmap *Bitmap;
	const TWsGcCmdDrawBitmap2 *Bitmap2;
	const TWsGcCmdDrawBitmap3 *Bitmap3;
	const TWsGcCmdDrawBitmapMasked* iBitmapMasked;
	const TWsGcCmdCopyRect *CopyRect;
	const TWsGcCmdGdiBlt2 *GdiBlt2;
	const TWsGcCmdGdiBlt3 *GdiBlt3;
	const TWsGcCmdUsePatternBrush *UsePatternBrush;
	const TWsGcCmdBltMasked *GdiBltMasked;
	const TWsGcCmdSetClippingRegion *SetClippingRegion;
	const TWsGcCmdBoxText *BoxText;
	const TWsGcCmdBoxTextInContext *BoxTextInContext;
	const TWsGcCmdBoxTextOptimised1 *BoxTextO1;
	const TWsGcCmdBoxTextOptimised2 *BoxTextO2;
	const TWsGcCmdBoxTextInContextOptimised1 *BoxTextInContextO1;
	const TWsGcCmdBoxTextInContextOptimised2 *BoxTextInContextO2;
	const TWsGcCmdDrawTextLocal *DrawTextLocal;
	const TWsGcCmdDrawTextInContextLocal *DrawTextInContextLocal;
	const TWsGcCmdBoxTextVertical *DrawBoxTextVertical;
	const TWsGcCmdBoxTextInContextVertical *DrawBoxTextInContextVertical;
	const TWsGcCmdBoxTextLocal *BoxTextLocal;
	const TWsGcCmdBoxTextInContextLocal *BoxTextInContextLocal;
	const TWsGcCmdGdiBlt2Local *GdiBlt2Local;
	const TWsGcCmdGdiBlt3Local *GdiBlt3Local;
	const TWsGcCmdGdiBltMaskedLocal *GdiBltMaskedLocal;
	const TWsGcCmdDrawBitmapLocal *BitmapLocal;
	const TWsGcCmdDrawBitmap2Local *Bitmap2Local;
	const TWsGcCmdDrawBitmap3Local *Bitmap3Local;
	const TWsGcCmdDrawBitmapMaskedLocal* iBitmapMaskedLocal;
	const CArrayFix<TPoint> *PointList;
	const TWsGcCmdSetJustification *SetJustification;
	const TWsGcCmdMapColors *MapColors;
	TWsGcCmdDrawTextPtr *DrawTextPtr;
	TWsGcCmdDrawTextVerticalPtr *DrawTextVerticalPtr;
	TWsGcCmdDrawTextInContextVerticalPtr *DrawTextInContextVerticalPtr;
	TWsGcCmdBoxTextPtr *DrawBoxTextPtr;
	TWsGcCmdBoxTextInContextPtr *DrawBoxTextInContextPtr;
	TWsGcCmdBoxTextVerticalPtr *DrawBoxTextVerticalPtr;
	TWsGcCmdBoxTextInContextVerticalPtr *DrawBoxTextInContextVerticalPtr;
	TWsGcCmdDrawTextInContextPtr *DrawTextInContextPtr;
	const TWsGcLargeStruct *LargeStruct;
	const TWsGcCmdAlphaBlendBitmaps *AlphaBlendBitmaps;
	const TWsGcCmdAlphaBlendBitmapsLocal *AlphaBlendBitmapsLocal;
	const TWsGcCmdMapColorsLocal *MapColorsLocal;
	const TWsGcCmdDrawWsGraphic* WsGraphic;
	const TWsGcCmdDrawResourceToPos* DrawWsResourceToPos;
	const TWsGcCmdDrawResourceToRect* DrawWsResourceToRect;
	const TWsGcCmdDrawResourceFromRectToRect* DrawWsResourceFromRectToRect;
	const TWsGcCmdDrawResourceWithData* DrawWsResourceWithData;
	};

struct TWsSdCmdSetScreenRotation
	{
	inline TWsSdCmdSetScreenRotation(TInt aMode, CFbsBitGc::TGraphicsOrientation aRotation);
	TInt mode;
	CFbsBitGc::TGraphicsOrientation rotation;
	};
struct TWsSdCmdGetScanLine
	{
	inline TWsSdCmdGetScanLine(const TPoint &aPos, TInt aLen, TDisplayMode aDispMode);
	TPoint pos;
	TInt len;
	TDisplayMode dispMode;
	};
struct TWsSdCmdRectCompare
	{
	inline TWsSdCmdRectCompare(const TRect &aRect1,const TRect &aRect2,TUint aFlags);
	TRect rect1;
	TRect rect2;
	TUint flags;
	};
struct TWsSdCmdCopyScreenToBitmap
	{
	inline TWsSdCmdCopyScreenToBitmap(TInt aHandle);
	TInt handle;
	};
struct TWsSdCmdCopyScreenToBitmap2
	{
	inline TWsSdCmdCopyScreenToBitmap2(const TRect &aRect, TInt aHandle);
	TRect rect;
	TInt handle;
	};
struct TWsSdCmdSetPalette
	{
	inline TWsSdCmdSetPalette(TPtr8& aPtr);
	TPtr8* ptr;
	TInt length;
	};
struct TWsSdCmdMapCoordinates
	{
	inline TWsSdCmdMapCoordinates(const TRect &aSource, TCoordinateSpace aSourceSpace, TCoordinateSpace aTargetSpace);
	TRect sourceRect;
	TCoordinateSpace sourceSpace;
	TCoordinateSpace targetSpace;
	};
union TWsSdCmdUnion
	{
	const TAny *any;
	const TAny **pAny;
	const TUint *UInt;
	const TInt *Int;
	const TPoint *Point;
	const TPixelsTwipsAndRotation *PixelsTwipsAndRotation;
	const TPixelsAndRotation *PixelsAndRotation;
	const TWsSdCmdSetScreenRotation *SetScreenRotation;
	const TWsSdCmdGetScanLine *GetScanLine;
	const TWsSdCmdRectCompare *RectCompare;
	const TWsSdCmdCopyScreenToBitmap *CopyScreenToBitmap;
	const TWsSdCmdCopyScreenToBitmap2 *CopyScreenToBitmap2;
	const TWsSdCmdSetPalette *SetPalette;
	const TSizeMode *ScreenSizeMode;
	const TDisplayConfiguration *DisplayConfiguration;
	const TWsSdCmdMapCoordinates *MapCoordinates;
	};

union TWsAnimDllCmdUnion 
	{
	const TAny *any;
	const TUint *UInt;
	};

struct TWsSpriteCmdUpdateMember
	{
	inline TWsSpriteCmdUpdateMember(TInt aIndex,const TSpriteMember &aData);
	TInt index;
	TCmdSpriteMember data;
	};
union TWsSpriteCmdUnion
	{
	const TAny *any;
	const TPoint *Point;
	const TCmdSpriteMember *SpriteMember;
	const TWsSpriteCmdUpdateMember *UpdateMember;
	};

struct TWsDirectCmdRequestReply
	{
	inline TWsDirectCmdRequestReply();
	inline TWsDirectCmdRequestReply(TThreadId aId,TRequestStatus* aRequest);
	TThreadId id;
	TRequestStatus* request;
	};
union TWsDirectCmdUnion
	{
	const TAny *any;
	const TInt *Int;
	};

union TWsClickCmdUnion
	{
	const TAny *any;
	const TInt *Int;
	const TBool *Bool;
	};

// Client side Panics and asserts

enum TW32Assert	// used for w32 code errors
	{
	EW32AssertOddLengthData,
	EW32AssertNotImplemented,
	EW32AssertIllegalOpcode,
	EW32AssertDirectMisuse,
	EW32AssertBufferLogic,
	EW32AssertUnexpectedOutOfRangePointerNumber, // Used for an out-of-range pointer number error within the old API's which doesn't take a pointer number.
	EW32AssertWindowSizeCacheFailure,
	};

enum TW32Panic // used for application errors
	{
	EW32PanicReConnect,		//An attempt was made to reconnect to the window server using RWsSession::Connect() without having closed the existing one first
	EW32PanicWindowAlreadyClosed,		//Not used
	EW32PanicGcAlreadyClosed,		//Not used
	EW32PanicPaletteAlreadyClosed,		//Not used
	EW32PanicMapColorsTooManyPairs,		//Raised by CWindowGc::MapColors() when its aNumPairs argument has a value greater than 16.
	EW32PanicDataExceedsBufferLength,		//A single string is too long to fit into the buffer.
	EW32PanicNullPalette,		//CWsScreenDevice::SetCustomPalette() was called with a NULL pointer.
	EW32PanicSilentFail,		//The caller would not know that the function was not successful
	EW32PanicDirectMisuse,		//A direct screen access function is being called out of turn
	EW32PanicInvalidRegion,        //An attempt to pass an invalid region to window server
	EW32PanicUsingReservedIpcSlot,		//Raised when the Client is trying to use a wrong IpcSlot. The first slot if raised by: RAnim::CommandReply or RAnimDll::Construct. Either the first or the second if raised by RAnim::AsyncCommandReply
	EW32PanicNullArray,        //A Function has been called with a NULL array pointer when it should not be
	EW32PanicGraphicAlreadyConstructed,        //Not used
	EW32PanicGraphicOrphaned,		//The CWsGraphic::CManager's RPointerArray<CWsGraphic> has still pointers to CWsGraphic left inside when it should be empty
	EW32PanicGraphicInternal,		//Raised when one of the CWsGraphic::CManager's member has an inconsistent value (i.e. NULL when it should not be and viceversa)
	EW32PanicGraphicBadBuffer,		//Raised if RWsGraphicMsgBuf has been passed an out of bounds index or an incorret type message
	EW32PanicGraphicNullData,		//Raised when trying to send an empty TWsClCmdGdSendMessage
	EW32PanicFunctionNotSupported,	//Operation not supported in the current OS distribution
	EW32PanicInvalidParameter,		// Raised when an invalid parameter is passed to a function 
	EW32PanicGraphicDoubleConstruction,		//Raised when Construct() is called on an already constructed client-side object
	EW32PanicBadClientInterface,	//Debug: Trying to use an interface that is not initialised 
	EW32PanicSizeNotExpected,		//Debug: Returned data does not match expected sizes or granularity
	EW32PanicStringTooLong,			//A string is longer than it is allowed to be
	};

enum WservShutdown
	{EWservShutdownCheck=0x13572468};	// Parameter to EWservMessShutdown to stop accidental shutdowns

enum WservMessages
	{
	EWservMessCommandBuffer,
	EWservMessShutdown,
	EWservMessInit,
	EWservMessFinish,	
	EWservMessSyncMsgBuf,
	EWservMessAsynchronousService=0x010000,
	EWservMessAnimDllAsyncCommand=0x100000,
	};

enum WH_HANDLES
	{
	WS_HANDLE_ROOT_WINDOW,
	WS_HANDLE_CLIENT,
	WS_HANDLE_WINDOW,
	WS_HANDLE_GROUP_WINDOW,
	WS_HANDLE_GC,
	WS_HANDLE_CAPTURE_KEY,
	WS_HANDLE_ANIM_DLL,
	WS_HANDLE_SCREEN_DEVICE,
	WS_HANDLE_SPRITE,
	WS_HANDLE_POINTER_CURSOR,
	WS_HANDLE_BITMAP,
	WS_HANDLE_DIRECT,
	WS_HANDLE_CLICK,
	WS_HANDLE_TEXT_CURSOR,
	WS_HANDLE_GRAPHIC_DRAWER,
	WS_HANDLE_DRAWABLE_SOURCE,
	WS_HANDLE_CAPTURE_KEY_UPDOWNS,
	WS_HANDLE_CAPTURE_LONG_KEY,
	};

enum TClientPanic
	{
	EWservNoPanic=0,						// Dummy Value used to indicate no panic
	EWservPanicOpcode=1,					// Out of range opcode
	EWservPanicBuffer=2,					// Invalid command buffer
	EWservPanicHandle=3,					// Invalid handle
	EWservPanicNullHandle=4,				// Null handle given as a handle value
	EWservPanicDrawable=5,					// Invalid drawable handle
	EWservPanicWindow=6,					// Invalid window handle
	EWservPanicBitmap=7,					// Invalid bitmap handle
	EWservPanicReadOutstanding=8,			// Event read already outstanding
	EWservPanicGcNotActive=9,				// Tried use a non-active GC
	EWservPanicGcActive=10,					// Tried to activate an already active GC
	EWservPanicWindowActive=11,				// Window already active
	EWservPanicRedrawActive=12,				// Already inside a begin/end redraw pair
	EWservPanicFont=13,						// Invalid font handle
	EWservPanicNoFont=14,					// Printing with no active font
	EWservPanicInvalidTextCursor=15,		// Tried to set invalid text cursor type
	EWservPanicReadOnlyDrawable=16,			// Attempted to write to a read only drawable
	EWservPanicInvalidRgb=17,				// Invalid RGB passed to window server
	EWservPanicPatternBrush=18,				// Invalid pattern brush
	EWservPanicNoBrush=19,					// Trying to paint without a brush
	EWservPanicUninitialisedBitmap=20,		// Trying to use an uninitialised bitmap
	EWservPanicDestroy=21,					// Trying to do a client destroy function on an illegal handle
	EWservPanicAnimDll=22,					// Panic from Animator DLL
	EWservPanicAnim=23,						// Invalid Anim object handle
	EWservPanicAnimLeave=24,				// Leave from a non-leaving anim function
	EWservPanicKeyOfDeath=25,				// Killed by the key of death!!!!
	EWservPanicTmpFont=26,					// Tried to reuse tmp font
	EWservPanicNoTmpFont=27,				// Tried to free tmp font when not allocated
	EWservPanicBadPolyData=28,				// Inconsistent polygon/polyline data
	EWservPanicNegativeShadowHeight=29,		// Tried to set a negative shadow height
	EWservPanicRedrawToNonRedrawWindow=30,	// Tried to redraw a non-redrawable window
	EWservPanicRedrawType=31,				// Init message contained invalid redraw type
	EWservPanicInvalidRegionCount=32,		// InvalidRegion was passed a zero count
	EWservPanicNoReplyBuffer=33,			// No reply buffer was passed to a function that requires one
	EWservPanicBackedUpWindowGcActive=34,	// Attempting to reactive a backed up window's GC
	EWservPanicCornerParams=35,				// Invalid corner type or flags
	EWservPanicBitmapAccess=36,				// Bitmap access not enabled on a backed up window
	EWservPanicDescriptor=37,				// Bad descriptor passed
	EWservPanicWindowDestroyed=38,			// Accessing a sprite after it's window has been destroyed
	EWservPanicUnsignalledEventData=39,		// A request for event data without a signal for that data being made
	EWservPanicMaskSize=40,					// The mask for a sprite is smaller then the bitmap
	EWservPanicSprite=41,					// bad sprite handle
	EWservPanicNotSystemPointerCursorListOwner=42,	 // Trying to set/clear a system pointer cursor when not owning the list
	EWservPanicNoPointerBuffer=43,			// Enabling the pointer buffer when there isn't one allocated
	EWservPanicBufferPtr=44,				// Invalid Buf,Len in the window server command buffer
	EWservPanicHotKey=45,					// Invalid hot key type
	EWservPanicPassword=46,					// Invalid call to PasswordEntered()
	EWservPanicSetComputeMode=47,			// Invalid value in call to SetComputeMode()
	EWservPanicBackupDisplayMode=48,		// Trying to set the display mode of a backed up window
	EWservPanicFetchMessage=49,				// Trying to fetch a message when one has not been signalled
	EWservPanicReInitialise=50,				// Client sent a second Init() message
	EWservPanicNoWindowSpecifed=51,			// Trying to get off messages without specifing a window to send them to
	EWservPanicParentDeleted=52,			// Setting size, pos or extent of a window whose parent or ancestor has been deleted
	EWservPanicResetingDefaultOwner=53,		// Calling the function RWindowGroup::DefaultOwningWindow() twice on the same group window
	EWservPanicScreenModeNumber=54,			// Calling an API function with an illegal screen size mode index
	EWservPanicScreenEnforcementMode=55,	// Illegal screen mode enforcement mode
	EWservPanicEventType=56,				// Must be a pointer event
	EWservPanicRotation=57,					// Not valid rotation
	EWservPanicNotTopClient=58,				// Can only call this on a window whoes parent is a group window
	EWservPanicDirectMisuse=59,				// Trying to do things to a Direct Screen Access object in the wrong order
	EWservPanicDuplicateHandle=60,			// Client Handle already in use
	EWservPanicNoCustomTextCursor=61,		// The custom text cursor has not been set
	EWservPanicCustomTextCursorAlign=62,	// Illegal custom text cursor allignment used
	EWservPanicNoSpriteMember=63,			// Use of a sprite that does not have any member set.
	EWservPanicTransparencyObjNotCreated=64,// Trying to set a transparency operation to a window that has not been enabled to be transparent.
	EWservPanicScreenNumber=65,				// Calling an multiple screen API function with an illegal screen number
	EWservPanicPermissionDenied=66,
	EWservPanicFunctionLeave=67,			// A leave occured while processing command in the middle of the buffer
	EWservPanicGroupWinScreenDeviceDeleted=68,	//Trying to use a group window with a deleted Screen Device
	EWservPanicDrawCommandsInvalidState=69,	//Redraw storing cannot be disabled for transparent window
	EWservPanicWsGraphic=70,				// Bad internal state in CWsGraphic code
	EWservPanicUninitialisedClient=71,	 	// Tried to use an uninitialised CWsClient
	EWservPanicNullMessageFromClient=72,		// Client IPC message is NULL
	EWservPanicTransparencyMisuse=73,		// Incompatible use of window transparency and background surface
	EWservPanic_Removed1=74,	// Panic removed in CR1489, can be reused
	EWservPanicInvalidSurface=75,			// An incorrect surface type has been used
	EWservPanicInvalidSurfaceConfiguration=76,	// Use of a surface configuration without valid members
	EWservPanic_Removed2=77,	// Panic removed in CR1489, can be reused
	EwservPanicIllegalDisplayMode=78,           // Illegal display mode is used
	EWservPanicWindowBeginRedrawNotCalled=79,   // A draw operation was performed on the CWindowGc outside a RWindow::BeginRedraw() / EndRedraw() pair. Enable AutoFlush to debug.
	EWservPanicInvalidParameter=80,             // General invalid parameter code for invariant checking
	EWservPanicDrawableSource=81,               // Invalid drawable source handle
	EWservPanicWrongScreen=82,                  // Child apps can only be constructed on the same screen as their parent. See RWindowGroup::ConstructChildApp
	EWservPanicScreenCaptureInvalidRequest=83,  // With Screen Capture disabled, an unexpected invalid request has been received
	EWservPanicInvalidDisplayConfiguration=84,  // Use of a display configuration without valid members
	EWservPanicUnableToEnableAdvPointer=85,     // Use when advanced pointers are enabled after an RWindow is activated
	};

#if defined(__WINS__)
	// Under WINS character code is passed in as HIWORD of the scan code,
	// and will need to be removed in some situations
	#define __REMOVE_WINS_CHARCODE &0x0000FFFF
	#define __WINS_CHARCODE(c)	((c) & 0xFFFF0000)
#else
	#define __REMOVE_WINS_CHARCODE
#endif

class CDebugLogBase: public CBase
	{
public:
	enum {EDummyConnectionId=0};		//This value is used to mean WSERV itself
	enum
		{
		ELogEverything=1,	// Gives the complete WSERV client server interaction along with client logging.
		ELogIntermediate=5,	// Gives the important WSERV client server interaction along with client logging.
		ELogImportant=9,	// Gives logging of panics and client logging.
		};
public:
	virtual void CommandBuf(TInt aApp)=0;
	virtual void Command(TInt aClass, TInt aOpcode, const TAny *aCmdData, TInt aHandle)=0;
	virtual void NewClient(TUint aConnectionHandle)=0;
	virtual void Reply(TInt aData)=0;
	virtual void ReplyBuf(const TDesC8 &aDes)=0;
	virtual void ReplyBuf(const TDesC16 &aDes)=0;
	virtual void SignalEvent(TInt aApp)=0;
	virtual void Panic(TInt aApp, TInt aReason)=0;
	virtual void MiscMessage(TInt aPriority,const TDesC &aFmt,TInt aParam=0)=0;
	virtual void HeapDump()=0;
	virtual void SetLoggingLevel(TInt aLevel)=0;
	virtual void IniFileSettingRead(TInt aScreen, const TDesC& aVarName, TBool aFound, const TDesC& aResult)=0;
	};

inline TUint WservEncoding::Encode8BitValues(TUint8 aFirst,TUint8 aSecond)
	{return aSecond<<8|aFirst;}
inline TUint WservEncoding::Encode8BitValues(TUint8 aFirst,TUint8 aSecond,TUint8 aThird)
	{return aThird<<16|Encode8BitValues(aFirst,aSecond);}
inline TUint8 WservEncoding::ExtractFirst8BitValue(TUint aUint)
	{return STATIC_CAST(TUint8,aUint&KMaxTUint8);}
inline TUint8 WservEncoding::ExtractSecond8BitValue(TUint aUint)
	{return STATIC_CAST(TUint8,aUint>>8&KMaxTUint8);}
inline TUint8 WservEncoding::ExtractThird8BitValue(TUint aUint)
	{return STATIC_CAST(TUint8,aUint>>16);}
//
inline TWsClCmdCreateSprite::TWsClCmdCreateSprite(TInt aWindow, const TPoint &aPos, TUint aFlags) :
	window(aWindow),pos(aPos),flags(aFlags)
	{}
inline TWsClCmdSetWindowGroupOrdinalPosition::TWsClCmdSetWindowGroupOrdinalPosition(TInt aIdentifier, TInt aPosition) :
	identifier(aIdentifier),position(aPosition)
	{}
inline TWsClCmdSetKeyboardRepeatRate::TWsClCmdSetKeyboardRepeatRate(const TTimeIntervalMicroSeconds32 &aInitial,const TTimeIntervalMicroSeconds32 &aTime) :
	initial(aInitial), time(aTime)
	{}
inline TWsClCmdSetDoubleClick::TWsClCmdSetDoubleClick(const TTimeIntervalMicroSeconds32 &aInterval,TInt aDistance) :
	interval(aInterval), distance(aDistance)
	{}
inline TWsClCmdSendEventToWindowGroup::TWsClCmdSendEventToWindowGroup(TInt aParameter, const TWsEvent &aEvent) :
	parameter(aParameter), event(aEvent)
	{}
inline TWsClCmdFindWindowGroupIdentifier::TWsClCmdFindWindowGroupIdentifier(TInt aPreviousIdentifier,TInt aOffset,TInt aLength) :
	identifier(aPreviousIdentifier),offset(aOffset),length(aLength)
	{}
inline TWsClCmdSendMessageToWindowGroup::TWsClCmdSendMessageToWindowGroup(TInt aIdentifier, TUid aUid, TInt aDataLength, const TDesC8 *aPtr) :
	identifierOrPriority(aIdentifier), uid(aUid), dataLength(aDataLength), ptr(aPtr)
	{}
inline TWsClCmdFetchMessage::TWsClCmdFetchMessage(TInt aWindowGroupIdentifier) :
	windowGroupIdentifier(aWindowGroupIdentifier)
	{}
inline TWsClCmdFindWindowGroupIdentifierThread::TWsClCmdFindWindowGroupIdentifierThread(TInt aPreviousIdentifier,TThreadId aThreadId) :
	identifier(aPreviousIdentifier),threadId(aThreadId)
	{}
inline TWsClCmdGetWindowGroupNameFromIdentifier::TWsClCmdGetWindowGroupNameFromIdentifier(TInt aIdentifier,TInt aMaxLength) :
	identifier(aIdentifier),maxLength(aMaxLength)
	{}
inline TWsClCmdOffEventsToShell::TWsClCmdOffEventsToShell(TBool aOn,TUint32 aWindow) :
	on(aOn),window(aWindow)
	{}
inline TWsClCmdSetPointerCursorArea::TWsClCmdSetPointerCursorArea(TInt aMode,TRect aArea) :
	mode(aMode),area(aArea)
	{}
inline TWsClCmdSetSystemFaded::TWsClCmdSetSystemFaded(TBool aFaded, TBool aUseDefaults/*=ETrue*/, TUint8 aBlackMap/*=0*/, TUint8 aWhiteMap/*=0*/)
	{params=WservEncoding::Encode8BitValues(aBlackMap,aWhiteMap
			,STATIC_CAST(TUint8,(aFaded?EFaded:0)|(aUseDefaults?EUseDefaultMap:0)));}
inline TBool TWsClCmdSetSystemFaded::Faded() const
	{return WservEncoding::ExtractThird8BitValue(params)&EFaded;}
inline TBool TWsClCmdSetSystemFaded::UseDefaultMap() const
	{return WservEncoding::ExtractThird8BitValue(params)&EUseDefaultMap;}
inline void TWsClCmdSetSystemFaded::GetFadingParams(TUint8& aBlackMap,TUint8& aWhiteMap) const
	{aBlackMap=WservEncoding::ExtractFirst8BitValue(params);aWhiteMap=WservEncoding::ExtractSecond8BitValue(params);}
inline TWsClCmdDebugInfo::TWsClCmdDebugInfo(TInt aFunction, TInt aParam) : iFunction(aFunction), iParam(aParam)
	{}
#if defined(SYMBIAN_GRAPHICS_GCE)
inline TWsClCmdCreateDrawableSource::TWsClCmdCreateDrawableSource(const TSgDrawableId& aDrawableId, TInt aScreenNumber)
	: iDrawableId(aDrawableId), iScreenNumber(aScreenNumber)
	{}
inline TWsClCmdSurfaceRegister::TWsClCmdSurfaceRegister(TInt aScreenNumber, const TSurfaceId& aSurface):
	screenNumber(aScreenNumber), surfaceId(aSurface)
	{}
#endif
inline TWsWinCmdSetExtent::TWsWinCmdSetExtent(const TPoint &aPos,const TSize &aSize) :
	pos(aPos),size(aSize)
	{}
inline TWsWinCmdCaptureLongKey::TWsWinCmdCaptureLongKey()
	{}
inline TWsWinCmdCaptureLongKey::TWsWinCmdCaptureLongKey(TUint aModifiers,TUint aModifierMask,TUint aInputKey,TUint aOutputKey
																	,TTimeIntervalMicroSeconds32 aDelay,TInt aPriority,TUint aFlags) :
	modifiers(aModifiers),modifierMask(aModifierMask),inputKey(aInputKey),outputKey(aOutputKey)
																						,delay(aDelay),priority(aPriority),flags(aFlags)
	{}
inline TWsWinCmdScroll::TWsWinCmdScroll(const TRect &aClip,const TPoint &aOffset,const TRect &aRect) :
	clip(aClip), offset(aOffset), rect(aRect)
	{}
inline TWsWinCmdSetTextCursor::TWsWinCmdSetTextCursor(TUint32 aWindow, const TPoint &aPos, const TTextCursor &aCursor, const TRect &aRect) :
	window(aWindow), pos(aPos), cursor(aCursor), rect(aRect)
	{}
inline TWsWinCmdSetTextCursor::TWsWinCmdSetTextCursor(TUint32 aWindow, const TPoint &aPos, const TTextCursor &aCursor) :
	window(aWindow), pos(aPos), cursor(aCursor)
	{}
inline TWsWinCmdGrabControl::TWsWinCmdGrabControl(const TUint aGrabControlFlags)
: pointerNumber(KErrNotFound), flags(aGrabControlFlags)
	{}
inline TWsWinCmdGrabControl::TWsWinCmdGrabControl(const TUint8 aPointerNumber, const TUint aGrabControlFlags)
: pointerNumber(aPointerNumber), flags(aGrabControlFlags)
	{}
inline TBool TWsWinCmdGrabControl::CheckFlags(TUint aFlags) const
	{return (flags&aFlags)==aFlags;}
inline TBool TWsWinCmdGrabControl::HasPointerNumber() const
	{return KErrNotFound!=pointerNumber;}

inline TWsWinCmdCancelPointerRepeatEventRequest::TWsWinCmdCancelPointerRepeatEventRequest()
:	pointerNumber(KErrNotFound), flags(TWsWinCmdCancelPointerRepeatEventRequest::ECancelRepeatFlagsNone)
	{}
inline TWsWinCmdCancelPointerRepeatEventRequest::TWsWinCmdCancelPointerRepeatEventRequest(const TUint8 aPointerNumber)
:	pointerNumber(aPointerNumber), flags(TWsWinCmdCancelPointerRepeatEventRequest::ECancelRepeatFlagsNone)
	{}
inline TWsWinCmdCancelPointerRepeatEventRequest::TWsWinCmdCancelPointerRepeatEventRequest(const TUint8 aPointerNumber, const TUint aFlags)
:	pointerNumber(aPointerNumber), flags(aFlags)
	{}
inline TBool TWsWinCmdCancelPointerRepeatEventRequest::HasPointerNumber() const
	{return KErrNotFound!=pointerNumber;}
inline TWsWinCmdRequestPointerRepeatEvent::TWsWinCmdRequestPointerRepeatEvent(const TTimeIntervalMicroSeconds32 &aTime,const TRect &aRect)
:	time(aTime),rect(aRect),pointerNumber(KErrNotFound),repeatFlags(ERepeatFlagsNone)
	{}
inline TWsWinCmdRequestPointerRepeatEvent::TWsWinCmdRequestPointerRepeatEvent(const TTimeIntervalMicroSeconds32 &aTime,const TRect &aRect,const TUint8 aPointerNumber, const TUint aRepeatFlags)
:	time(aTime),rect(aRect),pointerNumber(aPointerNumber),repeatFlags(aRepeatFlags)
	{}
inline TBool TWsWinCmdRequestPointerRepeatEvent::HasPointerNumber() const
	{return KErrNotFound!=pointerNumber;}
inline TWsClCmdZThresholdPair::TWsClCmdZThresholdPair(const TInt aEnterThreshold, const TInt aExitThreshold)
:	enterThreshold(aEnterThreshold),exitThreshold(aExitThreshold)
	{}
inline TWsClCmdSendEffectCommand::TWsClCmdSendEffectCommand(TInt aTfxCmd,TInt aTfxCmdDataLength, TInt aWinHandle) :
    tfxCmd(aTfxCmd), tfxCmdDataLength(aTfxCmdDataLength), windowHandle(aWinHandle)
	{}
inline TWsClCmdRegisterEffect::TWsClCmdRegisterEffect(TInt aAction, TInt aPurpose, TInt aDirStrSize, TInt aPhase1StrSize, TInt aPhase2StrSize, TUint aAppUid, TBitFlags aFlags) :
	tfxAction(aAction), tfxPurpose(aPurpose), tfxDirStrSize(aDirStrSize), tfxPhase1StrSize(aPhase1StrSize), tfxPhase2StrSize(aPhase2StrSize), tfxAppUid(aAppUid), tfxFlags(aFlags) 
	{}
inline TWsClCmdUnRegisterEffect::TWsClCmdUnRegisterEffect(TInt aAction, TInt aPurpose, TUint aAppUid) :
	tfxAction(aAction), tfxPurpose(aPurpose), tfxAppUid(aAppUid)
	{}
inline TWsClCmdOverrideEffect::TWsClCmdOverrideEffect(TInt aAction, TInt aPurpose, TInt aDirStrSize, TInt aPhase1StrSize, TInt aPhase2StrSize, TBitFlags aFlags) :
	tfxAction(aAction), tfxPurpose(aPurpose), tfxDirStrSize(aDirStrSize), tfxPhase1StrSize(aPhase1StrSize), tfxPhase2StrSize(aPhase2StrSize), tfxFlags(aFlags)
	{}
inline TWsWinCmdAddKeyRect::TWsWinCmdAddKeyRect(const TRect &aRect,TInt aScanCode, TBool aActivatedByPointerSwitchOn) :
	rect(aRect), scanCode(aScanCode), activatedByPointerSwitchOn(aActivatedByPointerSwitchOn)
	{}
#if defined(SYMBIAN_GRAPHICS_GCE)
inline TWsWinOpSetBackgroundSurfaceConfig::TWsWinOpSetBackgroundSurfaceConfig(const TSurfaceConfiguration& aConfiguration, TBool aTriggerRedraw) :
	surfaceConfig(aConfiguration), triggerRedraw(aTriggerRedraw)
	{}
#endif
inline TWsWinCmdEnableModifierChangedEvents::TWsWinCmdEnableModifierChangedEvents(TUint aModifierMask, TEventControl aCircumstances) :
	modifierMask(aModifierMask), circumstances(aCircumstances)
	{}
inline TWsWinCmdSetFaded::TWsWinCmdSetFaded(TBool aFaded, TBool aIncludeChildren, TBool aUseDefaults/*=ETrue*/, TUint8 aBlackMap/*=0*/, TUint8 aWhiteMap/*=0*/)
	{params=WservEncoding::Encode8BitValues(aBlackMap,aWhiteMap
			,STATIC_CAST(TUint8,(aFaded?EFaded:0)|(aIncludeChildren?EIncludeChildren:0)|(aUseDefaults?EUseDefaultMap:0)));}
inline TBool TWsWinCmdSetFaded::Faded() const
	{return WservEncoding::ExtractThird8BitValue(params)&EFaded;}
inline TBool TWsWinCmdSetFaded::IncludeChildren() const
	{return WservEncoding::ExtractThird8BitValue(params)&EIncludeChildren;}
inline TBool TWsWinCmdSetFaded::UseDefaultMap() const
	{return WservEncoding::ExtractThird8BitValue(params)&EUseDefaultMap;}
inline void TWsWinCmdSetFaded::GetFadingParams(TUint8& aBlackMap,TUint8& aWhiteMap) const
	{aBlackMap=WservEncoding::ExtractFirst8BitValue(params);aWhiteMap=WservEncoding::ExtractSecond8BitValue(params);}
//
inline TWsGcCmdBoxTextVertical::TWsGcCmdBoxTextVertical(const TRect& aBox) : box(aBox)
	{}
inline TWsGcCmdBoxTextLocal::TWsGcCmdBoxTextLocal(const TRect &aBox) : box(aBox)
	{}
inline TWsGcCmdDrawTextLocal::TWsGcCmdDrawTextLocal(const TPoint &aPos,const TDesC *aDesc) :
	pos(aPos), desc(aDesc)
	{}
inline TWsGcCmdDrawLine::TWsGcCmdDrawLine(const TPoint &aPnt1,const TPoint &aPnt2) : pnt1(aPnt1),pnt2(aPnt2)
	{}
inline TWsGcCmdDrawArcOrPie::TWsGcCmdDrawArcOrPie(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd) :
	rect(aRect),start(aStart),end(aEnd)
	{}
inline TWsGcCmdDrawText::TWsGcCmdDrawText(const TPoint &aPos, TInt aLength) : 
	pos(aPos),length(aLength)
	{}
inline TWsGcCmdDrawTextVertical::TWsGcCmdDrawTextVertical(const TPoint &aPos, TInt aLength, TBool aUp) : 
	pos(aPos),length(aLength),up(aUp)
	{}
inline TWsGcCmdBoxTextOptimised1::TWsGcCmdBoxTextOptimised1(const TRect &aRect, TInt aBaselineOffset,TInt aLength) :
	box(aRect),baselineOffset(aBaselineOffset),length(aLength)
	{}
inline TWsGcCmdBoxTextOptimised2::TWsGcCmdBoxTextOptimised2(const TRect &aRect,TInt aBaselineOffset,CGraphicsContext::TTextAlign aHoriz,TInt aLeftMrg,TInt aLength) :
	box(aRect),baselineOffset(aBaselineOffset),horiz(aHoriz),leftMrg(aLeftMrg),length(aLength)
	{}
inline TWsGcCmdBoxText::TWsGcCmdBoxText(const TRect &aRect,TInt aBaselineOffset,CGraphicsContext::TTextAlign aHoriz,TInt aLeftMrg,TInt aLength,TInt aWidth) :
	box(aRect),baselineOffset(aBaselineOffset),horiz(aHoriz),leftMrg(aLeftMrg),length(aLength),width(aWidth)
	{}
inline TWsGcCmdDrawRoundRect::TWsGcCmdDrawRoundRect(const TRect &aRect,const TSize &aEllipse) : rect(aRect), ellipse(aEllipse)
	{}
inline TWsGcCmdDrawPolygonLocalBufLen::TWsGcCmdDrawPolygonLocalBufLen(const TPoint *aPointList,TInt aNumPoints, CGraphicsContext::TFillRule aFillRule) :
	points(aPointList),length(aNumPoints),fillRule(aFillRule)
	{}
inline TWsGcCmdDrawPolyLineLocalBufLen::TWsGcCmdDrawPolyLineLocalBufLen(const TPoint *aPointList,TInt aNumPoints) :
	points(aPointList),length(aNumPoints)
	{}
inline TWsGcCmdDrawBitmap::TWsGcCmdDrawBitmap(const TPoint &aPoint,TInt aHandle) : handle(aHandle), pos(aPoint)
	{}
inline TWsGcCmdDrawBitmap2::TWsGcCmdDrawBitmap2(const TRect &aRect,TInt aHandle) : handle(aHandle), rect(aRect)
	{}
inline TWsGcCmdDrawBitmap3::TWsGcCmdDrawBitmap3(const TRect &aRect,TInt aHandle,const TRect &aSrcRect) :
	 handle(aHandle), rect(aRect), srcRect(aSrcRect)
	{}
inline TWsGcCmdDrawBitmapMasked::TWsGcCmdDrawBitmapMasked(const TRect &aDestRect,TInt aHandle,const TRect& aSrcRect,TInt aMaskHandle,TBool aInvertMask) :
	 iHandle(aHandle),iMaskHandle(aMaskHandle),iRect(aDestRect),iSrcRect(aSrcRect),iInvertMask(aInvertMask)
	 {}
inline TWsGcCmdDrawBitmapLocal::TWsGcCmdDrawBitmapLocal(const TPoint &aPos,const CFbsBitmap *aBitmap) :
	bitmap(aBitmap), pos(aPos)
	{}
inline TWsGcCmdDrawBitmap2Local::TWsGcCmdDrawBitmap2Local(const TRect &aRect,const CFbsBitmap *aBitmap) :
	bitmap(aBitmap), rect(aRect)
	{}
inline TWsGcCmdDrawBitmap3Local::TWsGcCmdDrawBitmap3Local(const TRect &aRect,const CFbsBitmap *aBitmap,const TRect &aSrcRect) :
	bitmap(aBitmap), rect(aRect), srcRect(aSrcRect)
	{}
inline TWsGcCmdDrawBitmapMaskedLocal::TWsGcCmdDrawBitmapMaskedLocal(const TRect &aDestRect,const CFbsBitmap* aBitmap,const TRect& aSrcRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask) :
	iBitmap(aBitmap),iMaskBitmap(aMaskBitmap),iRect(aDestRect),iSrcRect(aSrcRect),iInvertMask(aInvertMask)
	{}
inline TWsGcCmdCopyRect::TWsGcCmdCopyRect(const TPoint &anOffset,const TRect &aRect) :
	rect(aRect),pos(anOffset)
	{}
inline TWsGcCmdGdiBlt2::TWsGcCmdGdiBlt2(const TPoint &aPoint, TInt aHandle) :
	handle(aHandle), pos(aPoint)
	{}
inline TWsGcCmdGdiBlt3::TWsGcCmdGdiBlt3(const TPoint &aPoint, TInt aHandle, const TRect &aSrcRect) :
	handle(aHandle),pos(aPoint),rect(aSrcRect)
	{}
inline TWsGcCmdGdiBlt2Local::TWsGcCmdGdiBlt2Local(const TPoint &aPoint,const CFbsBitmap *aBitmap) :
	bitmap(aBitmap), pos(aPoint)
	{}
inline TWsGcCmdGdiBlt3Local::TWsGcCmdGdiBlt3Local(const TPoint &aDestination,const CFbsBitmap *aBitmap,const TRect &aSource) :
	bitmap(aBitmap), pos(aDestination), rect(aSource)
	{}
inline TWsGcCmdGdiBltMaskedLocal::TWsGcCmdGdiBltMaskedLocal(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect &aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask) :
	bitmap(aBitmap), maskBitmap(aMaskBitmap), invertMask(aInvertMask), pos(aPoint), rect(aSourceRect)
	{}
	inline TWsGcCmdBltMasked::TWsGcCmdBltMasked()
	{}
inline TWsGcCmdBltMasked::TWsGcCmdBltMasked(const TPoint& aPoint,TInt aBitmap,const TRect& aSourceRect,TInt aMaskBitmap,TBool aInvertMask) :
	destination(aPoint), handle(aBitmap), source(aSourceRect), maskHandle(aMaskBitmap), invertMask(aInvertMask)
	{}
inline TWsGcCmdMapColors::TWsGcCmdMapColors(const TRect &aRect, TInt aNumPairs, TBool aMapForwards) :
	rect(aRect), numPairs(aNumPairs), mapForwards(aMapForwards)
	{}
inline TWsGcCmdMapColorsLocal::TWsGcCmdMapColorsLocal(const TRect &aRect, const TRgb *aColors, TInt aNumPairs, TBool aMapForwards) :
	rect(aRect), colors(aColors), numPairs(aNumPairs), mapForwards(aMapForwards)
	{}	
inline TWsGcCmdAlphaBlendBitmaps::TWsGcCmdAlphaBlendBitmaps()
	{}
inline TWsGcCmdAlphaBlendBitmaps::TWsGcCmdAlphaBlendBitmaps(const TPoint& aPoint, TInt aBitmap,const TRect& aSourceRect,TInt aAlpha,const TPoint& aAlphaPoint) :
	point(aPoint), bitmapHandle(aBitmap), source(aSourceRect), alphaHandle(aAlpha), alphaPoint(aAlphaPoint)
	{}
inline TWsGcCmdAlphaBlendBitmapsLocal::TWsGcCmdAlphaBlendBitmapsLocal(const TPoint& aPoint, const CFbsBitmap *aBitmap, const TRect& aSourceRect, const CFbsBitmap *aAlpha, const TPoint& aAlphaPoint) :
	point(aPoint), iBitmap(aBitmap), source(aSourceRect), iAlpha(aAlpha), alphaPoint(aAlphaPoint)
	{}

//
inline TWsSdCmdSetScreenRotation::TWsSdCmdSetScreenRotation(TInt aMode, CFbsBitGc::TGraphicsOrientation aRotation) :
	mode(aMode), rotation(aRotation)
	{}
inline TWsSdCmdGetScanLine::TWsSdCmdGetScanLine(const TPoint &aPos, TInt aLen, TDisplayMode aDispMode) :
	pos(aPos), len(aLen), dispMode(aDispMode)
	{}
inline TWsSdCmdRectCompare::TWsSdCmdRectCompare(const TRect &aRect1,const TRect &aRect2,TUint aFlags) :
	rect1(aRect1), rect2(aRect2), flags(aFlags)
	{}
inline TWsSdCmdCopyScreenToBitmap::TWsSdCmdCopyScreenToBitmap(TInt aHandle) :
	handle(aHandle)
	{}
inline TWsSdCmdCopyScreenToBitmap2::TWsSdCmdCopyScreenToBitmap2(const TRect &aRect, TInt aHandle) :
	rect(aRect), handle(aHandle)
	{}
inline TWsSdCmdSetPalette::TWsSdCmdSetPalette(TPtr8& aPtr) :
	ptr(&aPtr), length(aPtr.Length())
	{}
inline TWsSdCmdMapCoordinates::TWsSdCmdMapCoordinates(const TRect &aSource, TCoordinateSpace aSourceSpace, TCoordinateSpace aTargetSpace):
	sourceRect(aSource), sourceSpace(aSourceSpace), targetSpace(aTargetSpace)
	{}
//
inline TWsSpriteCmdUpdateMember::TWsSpriteCmdUpdateMember(TInt aIndex,const TSpriteMember &aData) :
	index(aIndex), data(aData)
	{}
//
inline TWsDirectCmdRequestReply::TWsDirectCmdRequestReply()
	{}
inline TWsDirectCmdRequestReply::TWsDirectCmdRequestReply(TThreadId aId,TRequestStatus* aRequest) :
	id(aId), request(aRequest)
	{}
//
inline SKeyRepeatSettings::SKeyRepeatSettings()
	{}
inline SDoubleClickSettings::SDoubleClickSettings()
	{}
inline SDefModeMaxNumColors::SDefModeMaxNumColors()
	{}
//
inline TCmdSpriteMember::TCmdSpriteMember()
	{}
	
inline TWsGcCmdDrawTextVerticalPtr::TWsGcCmdDrawTextVerticalPtr(const TDesC *aDesc, const TPoint& aPos, const TBool aUp) : pos(aPos), up(aUp), text(const_cast<TDesC*>(aDesc))
	{}
	
inline TWsGcCmdDrawWsGraphic::TWsGcCmdDrawWsGraphic(const TWsGraphicId& aId,const TRect& aRect):
	iRect(aRect), iDataLen(0)
	{
	if(aId.IsUid())
		{
		iId = aId.Uid().iUid;
		iFlags = EWsGraphicIdUid;
		}
	else
		{
		iId = aId.Id();
		iFlags = EWsGraphicIdTransient;
		}
	}
inline TWsGcCmdDrawTextInContext::TWsGcCmdDrawTextInContext(const TPoint &aPos, TInt aLength, TInt aStart, TInt aEnd) : 
	pos(aPos),length(aLength),start(aStart),end(aEnd)
	{}
inline TWsGcCmdDrawTextInContextVertical::TWsGcCmdDrawTextInContextVertical(const TPoint &aPos, TInt aLength, TBool aUp,TInt aStart,TInt aEnd) : 
	pos(aPos),length(aLength),up(aUp),start(aStart),end(aEnd)
	{}
inline TWsGcCmdDrawTextInContextVerticalPtr::TWsGcCmdDrawTextInContextVerticalPtr(const TDesC *aDesc, const TPoint& aPos, const TBool aUp,TInt aStart,TInt aEnd) : pos(aPos), up(aUp), text(const_cast<TDesC*>(aDesc)),start(aStart),end(aEnd)
	{}
inline TWsGcCmdBoxTextInContextLocal::TWsGcCmdBoxTextInContextLocal(const TRect &aBox,const TInt aStart,const TInt aEnd) : box(aBox),start(aStart),end(aEnd)
	{}
inline TWsGcCmdBoxTextInContextOptimised1::TWsGcCmdBoxTextInContextOptimised1(const TRect &aRect, TInt aBaselineOffset,TInt aLength,TInt aStart,TInt aEnd) :
	box(aRect),baselineOffset(aBaselineOffset),length(aLength),start(aStart),end(aEnd)
	{}
inline TWsGcCmdBoxTextInContextOptimised2::TWsGcCmdBoxTextInContextOptimised2(const TRect &aRect,TInt aBaselineOffset,CGraphicsContext::TTextAlign aHoriz,TInt aLeftMrg,TInt aLength,TInt aStart,TInt aEnd) :
	box(aRect),baselineOffset(aBaselineOffset),horiz(aHoriz),leftMrg(aLeftMrg),length(aLength),start(aStart),end(aEnd)
	{}
inline TWsGcCmdBoxTextInContext::TWsGcCmdBoxTextInContext(const TRect &aRect,TInt aBaselineOffset,CGraphicsContext::TTextAlign aHoriz,TInt aLeftMrg,TInt aLength,TInt aWidth,TInt aStart,TInt aEnd) :
	box(aRect),baselineOffset(aBaselineOffset),horiz(aHoriz),leftMrg(aLeftMrg),length(aLength),width(aWidth),start(aStart),end(aEnd)
	{}
inline TWsGcCmdBoxTextInContextVertical::TWsGcCmdBoxTextInContextVertical(const TRect& aBox) : box(aBox)
	{}
inline TWsGcCmdDrawTextInContextLocal::TWsGcCmdDrawTextInContextLocal(const TPoint &aPos,const TDesC *aDesc,TInt aStart,TInt aEnd) :
	pos(aPos), desc(aDesc),start(aStart),end(aEnd)
	{}
inline TWsGcCmdDrawResourceToPos::TWsGcCmdDrawResourceToPos(TInt aWsHandle, const TPoint &aPos, CWindowGc::TGraphicsRotation aRotation) : 
	wsHandle(aWsHandle), pos(aPos), rotation(aRotation) 
	{}
inline TWsGcCmdDrawResourceToRect::TWsGcCmdDrawResourceToRect(TInt aWsHandle, const TRect &aRect, CWindowGc::TGraphicsRotation aRotation) : 
	wsHandle(aWsHandle), rect(aRect), rotation(aRotation)
	{}
inline TWsGcCmdDrawResourceFromRectToRect::TWsGcCmdDrawResourceFromRectToRect(TInt aWsHandle, const TRect &aRectDest, const TRect &aRectSrc, CWindowGc::TGraphicsRotation aRotation) :
	wsHandle(aWsHandle), rectDest(aRectDest), rectSrc(aRectSrc), rotation(aRotation)
	{}
inline TWsGcCmdDrawResourceWithData::TWsGcCmdDrawResourceWithData(TInt aWsHandle, const TRect &aRect, const TDesC8* aDesc) :
	wsHandle(aWsHandle), rect(aRect), desc(aDesc)
	{}
#endif
