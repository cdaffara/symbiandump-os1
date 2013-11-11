// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header defs for ANIMDLL
// 
//

#ifndef __AUTODLL_H__
#define __AUTODLL_H__


#if !defined(__W32ADLL_H__)
#include <w32adll.h>
#endif

#ifdef SYMBIAN_GRAPHICS_GCE
_LIT(KAnimDLLName,"autodl_nga.dll");
#else
_LIT(KAnimDLLName,"autodl_nonnga.dll");
#endif

const TInt KIpcSlot=1;

struct CTAParams
	{
	TPoint pos;
	TInt interval;
	TInt bit1;
	TInt bit2;
	};

struct TAnimRawEvent
	{
	//inline TAnimRawEvent(TRawEvent& aRawEvent,TBool aEat) :iRawEvent(aRawEvent), iEat(aEat) {}
	TRawEvent iRawEvent;
	TBool iEat;
	};

enum AnimDllPanicOpcodes
	{
	EADllPanicCallPanic,
	EADllPanicSetInterval,
	EADllPanicSetNextInterval,
	EADllPanicBadFont,
	EADllPanicBadBitmap,
	EADllPanicLeave,
	EADllPanicDestroy,
	EADllPanicOpcode,
	EADllPanicDoubleActivateGC,
	EADllPanicDrawRectWithNoGc,
	EADllPanicDrawTextWithNoFont,
	EADllPanicLeaveInAnimate,
	EADllPanicLeaveInRedraw,
	EADllPanicSetVisWithGcActive,
	EADllReadRemoteDescriptor,
	EADllSyncTests,
	EADllTimeChangedTest,
	EADllTestWindowSize,
	EADllTestSetVisible,
	EADllPanicSetVisWithGcActiveReply,
	EADllBlankOpCode,
	EADllPanicLeaveInActiveCallback,
	EADllPanicSetClippingRectWithNoGc,
	EADllPanicSetClippingRegionWithNoGc,
	EADllPanicCancelClippingRegionWithNoGc,
	EADllPanicCancelClippingRectWithNoGc,
	EADllPanicSetDrawModeWithNoGc,
	EADllPanicUseFontWithNoGc,
	EADllPanicDiscardFontWithNoGc,
	EADllPanicSetUnderlineStyleWithNoGc,
	EADllPanicSetStrikeThoughStyleWithNoGc,
	EADllPanicSetWordJustificationWithNoGc,
	EADllPanicSetCharJustificationWithNoGc,
	EADllPanicSetPenColorWithNoGc,
	EADllPanicSetPenStyleWithNoGc,
	EADllPanicSetPenSizeWithNoGc,
	EADllPanicSetBrushColorWithNoGc,
	EADllPanicSetBrushStyleWithNoGc,
	EADllPanicSetBrushOriginWithNoGc,
	EADllPanicUseBrushPatterWithNoGc,
	EADllPanicDiscardBrushPatternWithNoGc,
	EADllPanicSetFadedWithNoGc,
	EADllPanicSetFadingParametersWithNoGc,
	EADllPanicDrawArcWithNoGc,
	EADllPanicDrawPieWithNoGc,
	EADllPanicDrawLineWithNoGc,
	EADllPanicDrawLineToWithNoGc,
	EADllPanicDrawLineByWithNoGc,
	EADllPanicDrawEllipseWithNoGc,
	EADllPanicDrawRoundedRecWithNoGc,
	EADllPanicClearWithRectWithNoGc,
	EADllPanicClearWithNoGc,
	EADllPanicBitBltWithNoGc,
	EADllPanicBitBltWithRectWithNoGc,
	EADllPanicBitBltMaskedWithNoGc,
	EADllPanicDrawBitmapPointWithNoGc,
	EADllPanicDrawBitmapWithNoGc,
	EADllPanicDrawBitmapRectWithNoGc,
	EADllPanicDrawBitmapMaskedFbsBitmapWithNoGc,
	EADllPanicDrawBitmapMaskedWsBitmapWithNoGc,
	EADllPanicDrawPolyLinePointsWithNoGc,
	EADllPanicDrawPolyLineArrayWithNoGc,
	EADllPanicDrawPolygonPointsWithNoGc,
	EADllPanicDrawPolygonArrayWithNoGc,
	EADllPanicMoveToWithNoGc,
	EADllPanicMoveByWithNoGc,
	EADllPanicPlotWithNoGc,
	EADllPanicSetOriginWithNoGc,
	EADllPanicCopyRectWithNoGc,
	EADllPanicResetWithNoGc,
	EADllPanicAlphaBlendingBitmapsFbsBitmapWithNoGc,
	EADllPanicMapColorsWithNoGc,
	EADllPanicDrawTextWithRectWithNoFont,
	EADllPanicDrawTextVerticalWithNoFont,
	EADllPanicDrawTextVerticalWithRectWithNoFont,
	EADllPanicDevice,
	EADllPanicInvalidFocusScreenTooBig,
	EADllPanicInvalidFocusScreenNegative,
	};

enum AnimDllTestOpcodes
	{
	};

enum AnimDllOpcodes3
	{
	EADllSetRect,
	EADllStartAnimate,
	EADllCancelAnimate,
	EADllSetVisible,
	EADllSetShadowDrawMode,
	EADllShadowAnimTest,
	EADllDrawTestScreen,
	EADllParameterRectValueTest,
	EADllDoSetSync,
	EADllSetInterval,
	EADllSetMode
	};

enum AnimDllOpcodesScrBack
	{
	EADllDrawOtherRect,
	EADllIsHidden,
	};

enum AnimDllOpcodesSprite
	{
	EADllActivate,
	EADllDeactivate,
	EADllCheckMember,
	EADllSizeChanged,
	EADllSetPos,
	EADllDraw1,
	EADllDraw2,
	EADllDraw3,
	EADllIncDraw,
	};

enum AnimDllOpcodesFreeTimer
	{
    EStartAnimation,
    EStopAnimation,
    ESetFrameTime
	};

enum AnimDllOpcodesTimer
	{
	EADllNoTimer,
	EADllTimerStarted,
	EADllResetCount,
	EADllCurrentCount,
	EADllFailed,
	};

enum AnimDllWinFunctions
	{
	EADllScreen,
	EADllFocusScreen,
	EADllWindowGroups,
	EADllWindowGroupInfo,
	EADllWindowGroupName,
	EADllSetOrdinalPosition,
	EADllSetFocusScreen,
	};

enum AnimDllOpcodesNotificationTest
	{
	EADllQueryDSA,
	EADllQueryHeartbeatState,
	EADllIsAnimating,
	};

enum AnimDllTransTest
	{
	EADllDrawNow,
	EADllNextFrame,
	EADllStartAnimText,
	EADllEndAnimText,
	EADllSetSync,
	EADllSetFont,
	};

enum TAnimTypes
	{
	EAnimTypeTest1,
	EAnimTypeTest2,
	EAnimTypeTest3,
	EAnimTypeScrBackTest,
	EAnimTypeSprite,
	EAnimTypeFreeTimer,
	EAnimTypeEventTest,
	EAnimTypeEventPostingTest,
	EAnimTypeTimer,
	EAnimTypeWindowFunctions,
	EAnimTypeNotificationTest,
	EAnimTypeTrans,
	EAnimTypeEventHandler,
	EAnimTypeRemovableAnim,
	/** CWsAnimGc functional coverage */
	EAnimTypeCoverage,
	EAnimTypeMultiPointer,
	};

enum {ERemoteBufSize=0x10};
enum FlashState{EStateNone,EStateFlash,EStateSecond,EStateMinute};

enum AnimDllOpcodesEventTestAnimBase
	{
	EADllAddExpectedEvent,
	EADllNumberOfEventsReceived,
	EADllErrorCode,
	EADllReset,
	EADllEventAnimBase_EndBaseEnum,
	};

enum AnimDllOpcodesEventTestAnim
	{
	EADllNoEventsAndReset=EADllEventAnimBase_EndBaseEnum,
	EADllLoadDeviceDriver,
	EADllUnloadDeviceDriver,
	EADllSendEvent,
	EADllAfterEvent,
	};

enum AnimDllOpcodesRemovableAnim
	{
	EADllSetEventHandlerLifetime=EADllEventAnimBase_EndBaseEnum,
	EADllEventHandlerLifetime,
	EADllAddToEventHandlers,
	EADllRemoveFromEventHandlers,
	EADllTestPassed,
	};

enum AnimDllOpcodeMultiPointerAnim
	{
	EADllAddExpectedMultiPtrEvent,
	EADllMultiPtrEventError,
	EADllMultiPtrEventErrorDesc,
	EADllMultiPtrEventReset,
	};

struct TRemoteBufAddress
	{
	TAny *descPtr;
	};
struct TSyncTests
	{
	TRequestStatus *status;
	MAnimGeneralFunctions::TAnimSync syncMode;
	};
struct TShadowDrawTest
	{
	TRequestStatus *status;
	};
struct TTimeChangeTest
	{
	TRequestStatus *status;
	TRequestStatus *status2;
	};
struct TDrawTestScreen
	{
	TInt bitmap;
	TInt maskBitmap;
	TInt font;
	};
struct TSpriteMemberInfo
	{
	inline TSpriteMemberInfo() {}
	inline TSpriteMemberInfo(TPoint aSpritePos,TInt aMemberNo,const TSpriteMember& aMember);
	TPoint iSpritePos;
	TInt iMember;
	TInt iBitmapHandle;
	TInt iMaskBitmapHandle;
	TBool iInvertMask;
	CGraphicsContext::TDrawMode iDrawMode;
	TPoint iOffset;
	TTimeIntervalMicroSeconds32 iInterval;
	};
inline TSpriteMemberInfo::TSpriteMemberInfo(TPoint aSpritePos,TInt aMemberNo,const TSpriteMember& aMember)
	:iSpritePos(aSpritePos), iMember(aMemberNo), iBitmapHandle(aMember.iBitmap->Handle()),
	 iMaskBitmapHandle(aMember.iMaskBitmap ? aMember.iMaskBitmap->Handle():0), iInvertMask(aMember.iInvertMask),
	 iDrawMode(aMember.iDrawMode), iOffset(aMember.iOffset), iInterval(aMember.iInterval)
	{}
struct TWindowGroupInfoParms
	{
	TInt iScreen;
	TInt iOrdinalPosition;
	};
struct TSetOrdinalParms
	{
	TInt iIdentifier;
	TInt iOrdinalPosition;
	TInt iOrdinalPriority;
	};
struct TFrameData
	{
	TInt sync;
	TInt font;
	TInt bitmap;
	TRgb color;
	TInt draw;
	TInt text;
	TRect rect;
	};

class RTestAnim : public RAnim
	{
public:
	RTestAnim();
	RTestAnim(RAnimDll &aDll);
 	inline TInt Construct(const RWindowBase &aWin,TInt aType,const TDesC8 &aParams) {return(RAnim::Construct(aWin,aType,aParams));}
 	inline TInt Construct(const RWindowBase &aDevice,const TRect &aRect) {return(RAnim::Construct(aDevice, EAnimTypeScrBackTest,TPtrC8((TUint8 *)&aRect,sizeof(aRect))));}
 	inline TInt Construct(const RWsSprite &aSprite,TInt aType,const TDesC8 &aParams) {return(RAnim::Construct(aSprite,aType,aParams));}
 	inline TInt CommandReply(TInt aOpcode,const TDesC8& aArgs,const TIpcArgs& aIpcArgs)  {return(RAnim::CommandReply(aOpcode,aArgs,aIpcArgs));}
 	inline TInt CommandReply(TInt aOpcode,const TPtrC8 &aArgs) {return(RAnim::CommandReply(aOpcode,aArgs));}
 	inline TInt CommandReply(TInt aOpcode) {return(RAnim::CommandReply(aOpcode));}
 	inline void Command(TInt aOpcode,const TPtrC8 &aArgs) {RAnim::Command(aOpcode,aArgs);}
 	inline void Command(TInt aOpcode) {RAnim::Command(aOpcode);}
 	inline void ChangeRect(const TRect &aRect) {RAnim::Command(EADllDrawOtherRect,TPtrC8((TUint8 *)&aRect,sizeof(aRect)));}
	};

#endif
