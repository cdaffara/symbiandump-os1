/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "T_ActiveRConsoleRead.h"

const TInt KDefaultTimeout		=10000000;
const TUint8 KWhite				=0xFF;
const TUint8 KBlack				=0x00;

/*@{*/
_LIT(KFldTimeout,				"timeout");
_LIT(KFldErrorMargin,			"errorMargin");
_LIT(KFldTests,					"tests");


_LIT(KFldEventType,				"eventType%d");
_LIT(KFldEventOccurance,		"eventOccurance%d");
_LIT(KFldDataVerify,			"dataVerify%d");
_LIT(KFldDataDraw,				"dataDraw%d");
_LIT(KFldExitKeyCode,			"exitKeyCode");
_LIT(KFldExitRectangle,			"exitRectangle");


_LIT(KFldKeyCode,				"keyCode%d");
_LIT(KFldPoint,					"point%d");
_LIT(KFldRect,					"rect%d");
_LIT(KFldColourWhite,			"white");
_LIT(KFldColourBlack,			"black");


//format fields
_LIT(KLogUnexpectedEvent,		"Unexpected event. Expected %d Actual %d Index %d");
_LIT(KLogUnexpectedKeyCode,		"Unexpected key code %d");
_LIT(KLogUnexpectedPoint,		"Unexpected point (%d,%d)");
_LIT(KLogEvent,					"Event %d");
_LIT(KLogKey,					"Key %d");

/*@}*/
//This represents a NULL attribute
_LIT(KEnumNull,					"");

// TRawEvent
_LIT(KNone,						"ENone");
_LIT(KPointerMove,				"EPointerMove");
_LIT(KPointerSwitchOn,			"EPointerSwitchOn");
_LIT(KKeyDown,					"EKeyDown");
_LIT(KKeyUp,					"EKeyUp");
_LIT(KRedraw,					"ERedraw");
_LIT(KSwitchOn,					"ESwitchOn");
_LIT(KActive,					"EActive");
_LIT(KInactive,					"EInactive");
_LIT(KUpdateModifiers,			"EUpdateModifiers");
_LIT(KButton1Down,				"EButton1Down");
_LIT(KButton1Up,				"EButton1Up");
_LIT(KButton2Down,				"EButton2Down");
_LIT(KButton2Up,				"EButton2Up");
_LIT(KButton3Down,				"EButton3Down");
_LIT(KButton3Up,				"EButton3Up");
_LIT(KSwitchOff,				"ESwitchOff");
_LIT(KKeyRepeat,				"EKeyRepeat");
_LIT(KCaseOpen,					"ECaseOpen");
_LIT(KCaseClose,				"ECaseClose");
_LIT(KRestartSystem,			"ERestartSystem");
const CDataWrapperBase::TEnumEntryTable	CT_ActiveRConsoleRead::iEnumRawEventTable [] =
{
/**	Enum as a descriptor Enum integar value
 *	In this case these enums represent TRawEvents
 *
 *
 */
//	Enum as a descriptor			Enum
	KNone,							TRawEvent::ENone,
	KPointerMove,					TRawEvent::EPointerMove,
	KPointerSwitchOn,				TRawEvent::EPointerSwitchOn,
	KKeyDown,						TRawEvent::EKeyDown,
	KKeyUp,							TRawEvent::EKeyUp,
	KRedraw,						TRawEvent::ERedraw,
	KSwitchOn,						TRawEvent::ESwitchOn,
	KActive,						TRawEvent::EActive,
	KInactive,						TRawEvent::EInactive,
	KUpdateModifiers,				TRawEvent::EUpdateModifiers,
	KButton1Down,					TRawEvent::EButton1Down,
	KButton1Up,						TRawEvent::EButton1Up,
	KButton2Down,					TRawEvent::EButton2Down,
	KButton2Up,						TRawEvent::EButton2Up,
	KButton3Down,					TRawEvent::EButton3Down,
	KButton3Up,						TRawEvent::EButton3Up,
	KSwitchOff,						TRawEvent::ESwitchOff,
	KKeyRepeat,						TRawEvent::EKeyRepeat,
	KCaseOpen,						TRawEvent::ECaseOpen,
	KCaseClose,						TRawEvent::ECaseClose,
	KRestartSystem,					TRawEvent::ERestartSystem,
	KEnumNull,						-1,
	};

//	TKeyCode
_LIT(KKeyNull,						"EKeyNull");
_LIT(KKeyBell,						"EKeyBell");
_LIT(KKeyBackspace,					"EKeyBackspace");
_LIT(KKeyTab,						"EKeyTab");
_LIT(KKeyLineFeed,					"EKeyLineFeed");
_LIT(KKeyVerticalTab,				"EKeyVerticalTab");
_LIT(KKeyFormFeed,					"EKeyFormFeed");
_LIT(KKeyEnter,						"EKeyEnter");
_LIT(KKeyEscape,					"EKeyEscape");
_LIT(KKeySpace,						"EKeySpace");
_LIT(KKeyDelete,					"EKeyDelete");
_LIT(KKeyPrintScreen,				"EKeyPrintScreen");
_LIT(KKeyPause,						"EKeyPause");
_LIT(KKeyHome,						"EKeyHome");
_LIT(KKeyEnd,						"EKeyEnd");
_LIT(KKeyPageUp,					"EKeyPageUp");
_LIT(KKeyPageDown,					"EKeyPageDown");
_LIT(KKeyInsert,					"EKeyInsert");
_LIT(KKeyLeftArrow,					"EKeyLeftArrow");
_LIT(KKeyRightArrow,				"EKeyRightArrow");
_LIT(KKeyUpArrow,					"EKeyUpArrow");
_LIT(KKeyDownArrow,					"EKeyDownArrow");
_LIT(KKeyLeftShift,					"EKeyLeftShift");
_LIT(KKeyRightShift,				"EKeyRightShift");
_LIT(KKeyLeftAlt,					"EKeyLeftAlt");
_LIT(KKeyRightAlt,					"EKeyRightAlt");
_LIT(KKeyLeftCtrl,					"EKeyLeftCtrl");
_LIT(KKeyRightCtrl,					"EKeyRightCtrl");
_LIT(KKeyLeftFunc,					"EKeyLeftFunc");
_LIT(KKeyRightFunc,					"EKeyRightFunc");
_LIT(KKeyCapsLock,					"EKeyCapsLock");
_LIT(KKeyNumLock,					"EKeyNumLock");
_LIT(KKeyScrollLock,				"EKeyScrollLock");
_LIT(KKeyF1,						"EKeyF1");
_LIT(KKeyF2,						"EKeyF2");
_LIT(KKeyF3,						"EKeyF3");
_LIT(KKeyF4,						"EKeyF4");
_LIT(KKeyF5,						"EKeyF5");
_LIT(KKeyF6,						"EKeyF6");
_LIT(KKeyF7,						"EKeyF7");
_LIT(KKeyF8,						"EKeyF8");
_LIT(KKeyF9,						"EKeyF9");
_LIT(KKeyF10,						"EKeyF10");
_LIT(KKeyF11,						"EKeyF11");
_LIT(KKeyF12,						"EKeyF12");
_LIT(KKeyF13,						"EKeyF13");
_LIT(KKeyF14,						"EKeyF14");
_LIT(KKeyF15,						"EKeyF15");
_LIT(KKeyF16,						"EKeyF16");
_LIT(KKeyF17,						"EKeyF17");
_LIT(KKeyF18,						"EKeyF18");
_LIT(KKeyF19,						"EKeyF19");
_LIT(KKeyF20,						"EKeyF20");
_LIT(KKeyF21,						"EKeyF21");
_LIT(KKeyF22,						"EKeyF22");
_LIT(KKeyF23,						"EKeyF23");
_LIT(KKeyF24,						"EKeyF24");
_LIT(KKeyOff,						"EKeyOff");
_LIT(KKeyIncContrast,				"EKeyIncContrast");
_LIT(KKeyDecContrast,				"EKeyDecContrast");
_LIT(KKeyBacklightOn,				"EKeyBacklightOn");
_LIT(KKeyBacklightOff,				"EKeyBacklightOff");
_LIT(KKeyBacklightToggle,			"EKeyBacklightToggle");
_LIT(KKeySliderDown,				"EKeySliderDown");
_LIT(KKeySliderUp,					"EKeySliderUp");
_LIT(KKeyMenu,						"EKeyMenu");
_LIT(KKeyDictaphonePlay,			"EKeyDictaphonePlay");
_LIT(KKeyDictaphoneStop,			"EKeyDictaphoneStop");
_LIT(KKeyDictaphoneRecord,			"EKeyDictaphoneRecord");
_LIT(KKeyHelp,						"EKeyHelp");
_LIT(KKeyDial,						"EKeyDial");
_LIT(KKeyScreenDimension0,			"EKeyScreenDimension0");
_LIT(KKeyScreenDimension1,			"EKeyScreenDimension1");
_LIT(KKeyScreenDimension2,			"EKeyScreenDimension2");
_LIT(KKeyScreenDimension3,			"EKeyScreenDimension3");
_LIT(KKeyIncVolume,					"EKeyIncVolume");
_LIT(KKeyDecVolume,					"EKeyDecVolume");
_LIT(KKeyDevice0,					"EKeyDevice0");
_LIT(KKeyDevice1,					"EKeyDevice1");
_LIT(KKeyDevice2,					"EKeyDevice2");
_LIT(KKeyDevice3,					"EKeyDevice3");
_LIT(KKeyDevice4,					"EKeyDevice4");
_LIT(KKeyDevice5,					"EKeyDevice5");
_LIT(KKeyDevice6,					"EKeyDevice6");
_LIT(KKeyDevice7,					"EKeyDevice7");
_LIT(KKeyDevice8,					"EKeyDevice8");
_LIT(KKeyDevice9,					"EKeyDevice9");
_LIT(KKeyDeviceA,					"EKeyDeviceA");
_LIT(KKeyDeviceB,					"EKeyDeviceB");
_LIT(KKeyDeviceC,					"EKeyDeviceC");
_LIT(KKeyDeviceD,					"EKeyDeviceD");
_LIT(KKeyDeviceE,					"EKeyDeviceE");
_LIT(KKeyDeviceF,					"EKeyDeviceF");
_LIT(KKeyApplication0,				"EKeyApplication0");
_LIT(KKeyApplication1,				"EKeyApplication1");
_LIT(KKeyApplication2,				"EKeyApplication2");
_LIT(KKeyApplication3,				"EKeyApplication3");
_LIT(KKeyApplication4,				"EKeyApplication4");
_LIT(KKeyApplication5,				"EKeyApplication5");
_LIT(KKeyApplication6,				"EKeyApplication6");
_LIT(KKeyApplication7,				"EKeyApplication7");
_LIT(KKeyApplication8,				"EKeyApplication8");
_LIT(KKeyApplication9,				"EKeyApplication9");
_LIT(KKeyApplicationA,				"EKeyApplicationA");
_LIT(KKeyApplicationB,				"EKeyApplicationB");
_LIT(KKeyApplicationC,				"EKeyApplicationC");
_LIT(KKeyApplicationD,				"EKeyApplicationD");
_LIT(KKeyApplicationE,				"EKeyApplicationE");
_LIT(KKeyApplicationF,				"EKeyApplicationF");
_LIT(KKeyYes,						"EKeyYes");
_LIT(KKeyNo,						"EKeyNo");
_LIT(KKeyIncBrightness,				"EKeyIncBrightness");
_LIT(KKeyDecBrightness,				"EKeyDecBrightness");
_LIT(KKeyKeyboardExtend,			"EKeyKeyboardExtend");
_LIT(KKeyDevice10,					"EKeyDevice10");
_LIT(KKeyDevice11,					"EKeyDevice11");
_LIT(KKeyDevice12,					"EKeyDevice12");
_LIT(KKeyDevice13,					"EKeyDevice13");
_LIT(KKeyDevice14,					"EKeyDevice14");
_LIT(KKeyDevice15,					"EKeyDevice15");
_LIT(KKeyDevice16,					"EKeyDevice16");
_LIT(KKeyDevice17,					"EKeyDevice17");
_LIT(KKeyDevice18,					"EKeyDevice18");
_LIT(KKeyDevice19,					"EKeyDevice19");
_LIT(KKeyDevice1A,					"EKeyDevice1A");
_LIT(KKeyDevice1B,					"EKeyDevice1B");
_LIT(KKeyDevice1C,					"EKeyDevice1C");
_LIT(KKeyDevice1D,					"EKeyDevice1D");
_LIT(KKeyDevice1E,					"EKeyDevice1E");
_LIT(KKeyDevice1F,					"EKeyDevice1F");
_LIT(KKeyApplication10,				"EKeyApplication10");
_LIT(KKeyApplication11,				"EKeyApplication11");
_LIT(KKeyApplication12,				"EKeyApplication12");
_LIT(KKeyApplication13,				"EKeyApplication13");
_LIT(KKeyApplication14,				"EKeyApplication14");
_LIT(KKeyApplication15,				"EKeyApplication15");
_LIT(KKeyApplication16,				"EKeyApplication16");
_LIT(KKeyApplication17,				"EKeyApplication17");
_LIT(KKeyApplication18,				"EKeyApplication18");
_LIT(KKeyApplication19,				"EKeyApplication19");
_LIT(KKeyApplication1A,				"EKeyApplication1A");
_LIT(KKeyApplication1B,				"EKeyApplication1B");
_LIT(KKeyApplication1C,				"EKeyApplication1C");
_LIT(KKeyApplication1D,				"EKeyApplication1D");
_LIT(KKeyApplication1E,				"EKeyApplication1E");
_LIT(KKeyApplication1F,				"EKeyApplication1F");
_LIT(KKeyDevice20,					"EKeyDevice20");
_LIT(KKeyDevice21,					"EKeyDevice21");
_LIT(KKeyDevice22,					"EKeyDevice22");
_LIT(KKeyDevice23,					"EKeyDevice23");
_LIT(KKeyDevice24,					"EKeyDevice24");
_LIT(KKeyDevice25,					"EKeyDevice25");
_LIT(KKeyDevice26,					"EKeyDevice26");
_LIT(KKeyDevice27,					"EKeyDevice27");
_LIT(KKeyApplication20,				"EKeyApplication20");
_LIT(KKeyApplication21,				"EKeyApplication21");
_LIT(KKeyApplication22,				"EKeyApplication22");
_LIT(KKeyApplication23,				"EKeyApplication23");
_LIT(KKeyApplication24,				"EKeyApplication24");
_LIT(KKeyApplication25,				"EKeyApplication25");
_LIT(KKeyApplication26,				"EKeyApplication26");
_LIT(KKeyApplication27,				"EKeyApplication27");
const CDataWrapperBase::TEnumEntryTable	CT_ActiveRConsoleRead::iEnumKeyCodeTable [] =
	{
//	Enum as a descriptor					Enum
	KKeyNull,						EKeyNull,
	KKeyBell,						EKeyBell,
	KKeyBackspace,					EKeyBackspace,
	KKeyTab,						EKeyTab,
	KKeyLineFeed,					EKeyLineFeed,
	KKeyVerticalTab,				EKeyVerticalTab,
	KKeyFormFeed,					EKeyFormFeed,
	KKeyEnter,						EKeyEnter,
	KKeyEscape,						EKeyEscape,
	KKeySpace,						EKeySpace,
	KKeyDelete,						EKeyDelete,
	KKeyPrintScreen,				EKeyPrintScreen,
	KKeyPause,						EKeyPause,
	KKeyHome,						EKeyHome,
	KKeyEnd,						EKeyEnd,
	KKeyPageUp,						EKeyPageUp,
	KKeyPageDown,					EKeyPageDown,
	KKeyInsert,						EKeyInsert,
	KKeyLeftArrow,					EKeyLeftArrow,
	KKeyRightArrow,					EKeyRightArrow,
	KKeyUpArrow,					EKeyUpArrow,
	KKeyDownArrow,					EKeyDownArrow,
	KKeyLeftShift,					EKeyLeftShift,
	KKeyRightShift,					EKeyRightShift,
	KKeyLeftAlt,					EKeyLeftAlt,
	KKeyRightAlt,					EKeyRightAlt,
	KKeyLeftCtrl,					EKeyLeftCtrl,
	KKeyRightCtrl,					EKeyRightCtrl,
	KKeyLeftFunc,					EKeyLeftFunc,
	KKeyRightFunc,					EKeyRightFunc,
	KKeyCapsLock,					EKeyCapsLock,
	KKeyNumLock,					EKeyNumLock,
	KKeyScrollLock,					EKeyScrollLock,
	KKeyF1,							EKeyF1,
	KKeyF2,							EKeyF2,
	KKeyF3,							EKeyF3,
	KKeyF4,							EKeyF4,
	KKeyF5,							EKeyF5,
	KKeyF6,							EKeyF6,
	KKeyF7,							EKeyF7,
	KKeyF8,							EKeyF8,
	KKeyF9,							EKeyF9,
	KKeyF10,						EKeyF10,
	KKeyF11,						EKeyF11,
	KKeyF12,						EKeyF12,
	KKeyF13,						EKeyF13,
	KKeyF14,						EKeyF14,
	KKeyF15,						EKeyF15,
	KKeyF16,						EKeyF16,
	KKeyF17,						EKeyF17,
	KKeyF18,						EKeyF18,
	KKeyF19,						EKeyF19,
	KKeyF20,						EKeyF20,
	KKeyF21,						EKeyF21,
	KKeyF22,						EKeyF22,
	KKeyF23,						EKeyF23,
	KKeyF24,						EKeyF24,
	KKeyOff,						EKeyOff,
	KKeyIncContrast,				EKeyIncContrast,
	KKeyDecContrast,				EKeyDecContrast,
	KKeyBacklightOn,				EKeyBacklightOn,
	KKeyBacklightOff,				EKeyBacklightOff,
	KKeyBacklightToggle,			EKeyBacklightToggle,
	KKeySliderDown,					EKeySliderDown,
	KKeySliderUp,					EKeySliderUp,
	KKeyMenu,						EKeyMenu,
	KKeyDictaphonePlay,				EKeyDictaphonePlay,
	KKeyDictaphoneStop,				EKeyDictaphoneStop,
	KKeyDictaphoneRecord,			EKeyDictaphoneRecord,
	KKeyHelp,						EKeyHelp,
	KKeyDial,						EKeyDial,
	KKeyScreenDimension0,			EKeyScreenDimension0,
	KKeyScreenDimension1,			EKeyScreenDimension1,
	KKeyScreenDimension2,			EKeyScreenDimension2,
	KKeyScreenDimension3,			EKeyScreenDimension3,
	KKeyIncVolume,					EKeyIncVolume,
	KKeyDecVolume,					EKeyDecVolume,
	KKeyDevice0,					EKeyDevice0,
	KKeyDevice1,					EKeyDevice1,
	KKeyDevice2,					EKeyDevice2,
	KKeyDevice3,					EKeyDevice3,
	KKeyDevice4,					EKeyDevice4,
	KKeyDevice5,					EKeyDevice5,
	KKeyDevice6,					EKeyDevice6,
	KKeyDevice7,					EKeyDevice7,
	KKeyDevice8,					EKeyDevice8,
	KKeyDevice9,					EKeyDevice9,
	KKeyDeviceA,					EKeyDeviceA,
	KKeyDeviceB,					EKeyDeviceB,
	KKeyDeviceC,					EKeyDeviceC,
	KKeyDeviceD,					EKeyDeviceD,
	KKeyDeviceE,					EKeyDeviceE,
	KKeyDeviceF,					EKeyDeviceF,
	KKeyApplication0,				EKeyApplication0,
	KKeyApplication1,				EKeyApplication1,
	KKeyApplication2,				EKeyApplication2,
	KKeyApplication3,				EKeyApplication3,
	KKeyApplication4,				EKeyApplication4,
	KKeyApplication5,				EKeyApplication5,
	KKeyApplication6,				EKeyApplication6,
	KKeyApplication7,				EKeyApplication7,
	KKeyApplication8,				EKeyApplication8,
	KKeyApplication9,				EKeyApplication9,
	KKeyApplicationA,				EKeyApplicationA,
	KKeyApplicationB,				EKeyApplicationB,
	KKeyApplicationC,				EKeyApplicationC,
	KKeyApplicationD,				EKeyApplicationD,
	KKeyApplicationE,				EKeyApplicationE,
	KKeyApplicationF,				EKeyApplicationF,
	KKeyYes,						EKeyYes,
	KKeyNo,							EKeyNo,
	KKeyIncBrightness,				EKeyIncBrightness,
	KKeyDecBrightness,				EKeyDecBrightness,
	KKeyKeyboardExtend,				EKeyKeyboardExtend,
	KKeyDevice10,					EKeyDevice10,
	KKeyDevice11,					EKeyDevice11,
	KKeyDevice12,					EKeyDevice12,
	KKeyDevice13,					EKeyDevice13,
	KKeyDevice14,					EKeyDevice14,
	KKeyDevice15,					EKeyDevice15,
	KKeyDevice16,					EKeyDevice16,
	KKeyDevice17,					EKeyDevice17,
	KKeyDevice18,					EKeyDevice18,
	KKeyDevice19,					EKeyDevice19,
	KKeyDevice1A,					EKeyDevice1A,
	KKeyDevice1B,					EKeyDevice1B,
	KKeyDevice1C,					EKeyDevice1C,
	KKeyDevice1D,					EKeyDevice1D,
	KKeyDevice1E,					EKeyDevice1E,
	KKeyDevice1F,					EKeyDevice1F,
	KKeyApplication10,				EKeyApplication10,
	KKeyApplication11,				EKeyApplication11,
	KKeyApplication12,				EKeyApplication12,
	KKeyApplication13,				EKeyApplication13,
	KKeyApplication14,				EKeyApplication14,
	KKeyApplication15,				EKeyApplication15,
	KKeyApplication16,				EKeyApplication16,
	KKeyApplication17,				EKeyApplication17,
	KKeyApplication18,				EKeyApplication18,
	KKeyApplication19,				EKeyApplication19,
	KKeyApplication1A,				EKeyApplication1A,
	KKeyApplication1B,				EKeyApplication1B,
	KKeyApplication1C,				EKeyApplication1C,
	KKeyApplication1D,				EKeyApplication1D,
	KKeyApplication1E,				EKeyApplication1E,
	KKeyApplication1F,				EKeyApplication1F,
	KKeyDevice20,					EKeyDevice20,
	KKeyDevice21,					EKeyDevice21,
	KKeyDevice22,					EKeyDevice22,
	KKeyDevice23,					EKeyDevice23,
	KKeyDevice24,					EKeyDevice24,
	KKeyDevice25,					EKeyDevice25,
	KKeyDevice26,					EKeyDevice26,
	KKeyDevice27,					EKeyDevice27,
	KKeyApplication20,				EKeyApplication20,
	KKeyApplication21,				EKeyApplication21,
	KKeyApplication22,				EKeyApplication22,
	KKeyApplication23,				EKeyApplication23,
	KKeyApplication24,				EKeyApplication24,
	KKeyApplication25,				EKeyApplication25,
	KKeyApplication26,				EKeyApplication26,
	KKeyApplication27,				EKeyApplication27,
	KEnumNull,						-1,
	};

_LIT(KEEventOccuranceOnce,			"EEventOccuranceOnce");
_LIT(KEEventOccuranceZeroOrMore,	"EEventOccuranceZeroOrMore");
_LIT(KEEventOccuranceOneOrMore,		"EEventOccuranceOneOrMore");
const CDataWrapperBase::TEnumEntryTable	CT_ActiveRConsoleRead::iEnumEventOccuranceTable[] =
	{
//	Enum as a descriptor					Enum
	KEEventOccuranceOnce,			CT_ActiveRConsoleRead::EEventOccuranceOnce,
	KEEventOccuranceZeroOrMore,		CT_ActiveRConsoleRead::EEventOccuranceZeroOrMore,
	KEEventOccuranceOneOrMore,		CT_ActiveRConsoleRead::EEventOccuranceOneOrMore,
	KEnumNull,						-1,
	};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_ActiveRConsoleRead* CT_ActiveRConsoleRead::NewL(CDataWrapperBase& aDataWrapperBase)
	{
	CT_ActiveRConsoleRead* self=new (ELeave) CT_ActiveRConsoleRead(aDataWrapperBase);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CT_ActiveRConsoleRead::CT_ActiveRConsoleRead(CDataWrapperBase& aDataWrapperBase)
:	CActiveCallbackBase(aDataWrapperBase,EPriorityStandard)
,	iColourValueBlack(0)
,	iColourValueWhite(0)
,	iEventIndex(0)
,	iEventCount(0)
,	iTimeOut(0)
,	iErrorMargin(0)
,	iHasExitKeyCode(EFalse)
,	iHasExitRectangle(EFalse)
,	iExitKeyCode(0)
,	iTestIndex(0)
,	iNumberOfTests(0)
,	iHasExpectedKeyCode(EFalse)
,	iHasExpectedRectangle(EFalse)
,	iExpectedKeyCode(0)
	{
	}


CT_ActiveRConsoleRead::~CT_ActiveRConsoleRead()
/**
 * Public destructor
 */
	{
	DestroyData();
	}

void CT_ActiveRConsoleRead::DestroyData()
	{
	iEvent.Reset();
	iEvent.Close();
	}

TBool	CT_ActiveRConsoleRead::KickStartL(const TDesC& aSection, const TInt aAsyncErrorIndex, RConsole& aConsole)
/**
 * Kick Start the object and set up intials
 *	@param		aSection			The section in the ini containing data for the command
 * 	@param		aAsyncErrorIndex	Command index for async calls to return errors to
 *	@param		aConsole			The RConsole object
 */
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	iSection.Set(aSection);

	iColourValueBlack	=KBlack;
	iColourValueWhite	=KWhite;
	iDataWrapperBase.GetUint8FromConfig(iSection, KFldColourBlack(), iColourValueBlack);
	iDataWrapperBase.GetUint8FromConfig(iSection, KFldColourWhite(), iColourValueWhite);

	iTimeOut=KDefaultTimeout;
	iDataWrapperBase.GetIntFromConfig(iSection, KFldTimeout(), iTimeOut);

	iErrorMargin=0;
	iDataWrapperBase.GetIntFromConfig(iSection, KFldErrorMargin(), iErrorMargin);

	iHasExitKeyCode=iDataWrapperBase.GetHexFromConfig(iSection, KFldExitKeyCode(), iExitKeyCode);

	iHasExitRectangle=iDataWrapperBase.GetRectFromConfig(iSection, KFldExitRectangle(), iExitRectangle);
	if ( iHasExitRectangle )
		{
		//	Draw rectangle
		TInt	height =Abs(iExitRectangle.iBr.iY-iExitRectangle.iTl.iY);
		TInt	width	=Abs(iExitRectangle.iBr.iX-iExitRectangle.iTl.iX);
		CDrawUtils::DrawSquareUtility(iExitRectangle.iTl, height, width, iColourValueWhite);
		}

	iEvent.Reset();

	TEventConfig	config;
	TInt			eventIndex=0;
	TBool			dataOk=ETrue;
	TBool			moreData=ETrue;
	while ( moreData )
		{
		tempStore.Format(KFldEventType, ++eventIndex);
		moreData=iDataWrapperBase.GetEnumFromConfig(iSection, tempStore, iEnumRawEventTable, config.iEventType);
		if ( moreData )
			{
			tempStore.Format(KFldEventOccurance, eventIndex);
			TInt	eventOccurance=EEventOccuranceOnce;
			iDataWrapperBase.GetEnumFromConfig(iSection, tempStore, iEnumEventOccuranceTable, eventOccurance);
			config.iEventOccurance=(TEventOccurance)eventOccurance;
			
			tempStore.Format(KFldDataVerify, eventIndex);
			config.iDataVerify=EFalse;
			iDataWrapperBase.GetBoolFromConfig(iSection, tempStore, config.iDataVerify);

			tempStore.Format(KFldDataDraw, eventIndex);
			config.iDataDraw=EFalse;
			iDataWrapperBase.GetBoolFromConfig(iSection, tempStore, config.iDataDraw);

			iEvent.AppendL(config);
			dataOk=ETrue;
			}
		}

	//	If -1 then we have an umlimited number of test(s) that completes with an exit event
	//	which can be an exit key code or a pen event in the exit rectangle
	iNumberOfTests=-1;
	iDataWrapperBase.GetIntFromConfig(iSection, KFldTests(), iNumberOfTests);

	if ( dataOk )
		{
		iTestIndex=0;
		dataOk=KickNext(aAsyncErrorIndex, aConsole);
		}

	return dataOk;
	}

TBool	CT_ActiveRConsoleRead::KickNext(const TInt aAsyncErrorIndex, RConsole& aConsole)
	{
	TInt	testing=(iTestIndex<iNumberOfTests) || (iNumberOfTests==-1);

	if ( testing )
		{
		TBuf<KMaxTestExecuteCommandLength>	tempStore;
		iEventIndex=0;
		iEventCount=0;
		iHasExpectedKeyCode=EFalse;
		iHasExpectedRectangle=EFalse;

		++iTestIndex;

		tempStore.Format(KFldKeyCode, iTestIndex);
		iHasExpectedKeyCode=iDataWrapperBase.GetHexFromConfig(iSection, tempStore, iExpectedKeyCode);

		tempStore.Format(KFldPoint, iTestIndex);
		TPoint	point;
		if ( iDataWrapperBase.GetPointFromConfig(iSection, tempStore, point) )
			{
			iHasExpectedRectangle=ETrue;
			iExpectedRectangle.SetRect(point.iX-iErrorMargin, point.iY-iErrorMargin, point.iX+iErrorMargin, point.iY+iErrorMargin);
			}

		tempStore.Format(KFldRect, iTestIndex);
		TRect	rect;
		if ( iDataWrapperBase.GetRectFromConfig(iSection, tempStore, rect) )
			{
			iHasExpectedRectangle=ETrue;
			iExpectedRectangle=rect;
			}

		if ( iHasExpectedRectangle )
			{
			//	Draw boundary
			TInt height =Abs(iExpectedRectangle.iBr.iY-iExpectedRectangle.iTl.iY);
			TInt width	=Abs(iExpectedRectangle.iBr.iX-iExpectedRectangle.iTl.iX);
			CDrawUtils::DrawSquareOutLineUtility(iExpectedRectangle.iTl,height,width,iColourValueWhite);
			}

		Kick(aAsyncErrorIndex, aConsole);
		}

	return testing;
	}

void CT_ActiveRConsoleRead::Kick(const TInt aAsyncErrorIndex, RConsole& aConsole)
/**
 * Kick the object
 */
	{
	iDataWrapperBase.INFO_PRINTF1(_L("issuing kick"));
	aConsole.Read(iKey, iStatus);//wait for user input
	CActiveCallbackBase::Activate(aAsyncErrorIndex, iTimeOut);
	}

TBool CT_ActiveRConsoleRead::VerifyDataAndKick(const TInt aAsyncErrorIndex, RConsole& aConsole)
	{
	TInt	eventType=iKey.Type();
	iDataWrapperBase.INFO_PRINTF2(KLogEvent, eventType);

	//	Do we ignore the event
	TBool			moreToDo=!ExitEvent();
	TBool			eventOk=EFalse;

	TEventConfig	config=iEvent[iEventIndex];
	while ( iEventIndex<iEvent.Count() && !eventOk && moreToDo )
		{
		config=iEvent[iEventIndex];
		if ( eventType==config.iEventType )
			{
			++iEventCount;
			eventOk=ETrue;
			iDataWrapperBase.INFO_PRINTF3(_L("Events type match Expected=%d, Actual=%d"), config.iEventType,eventType);
			}
		else
			{
			switch(config.iEventOccurance)
				{
			case EEventOccuranceOnce:
				iDataWrapperBase.ERR_PRINTF4(KLogUnexpectedEvent, config.iEventType, eventType, iEventIndex);
				iDataWrapperBase.SetBlockResult(EFail);
				moreToDo=EFalse;
				break;
			case EEventOccuranceZeroOrMore:
				//	If the event may not occur then go to the next one
				++iEventIndex;
				iEventCount=0;
				break;
			case EEventOccuranceOneOrMore:
				if(iEventCount==0)
					{
					iDataWrapperBase.ERR_PRINTF4(KLogUnexpectedEvent, config.iEventType, eventType, iEventIndex);
					iDataWrapperBase.SetBlockResult(EFail);
					moreToDo=EFalse;
					}
				else
					{
					++iEventIndex;
					iEventCount=0;
					}
				break;
				}
			}
		}
	
	if ( eventOk )
		{
		if ( config.iDataDraw )
			{
			DrawData();
			}

		if ( config.iDataVerify )
			{
			if ( iHasExpectedRectangle )
				{
				TPoint	point=iKey.MousePos();
				if ( !iExpectedRectangle.Contains(point) )
					{
					iDataWrapperBase.ERR_PRINTF3(KLogUnexpectedPoint, point.iX, point.iY);
					iDataWrapperBase.SetBlockResult(EFail);
					}
				}

			if ( iHasExpectedKeyCode )
				{
				TKeyCode	keyCode=iKey.Code();
				if ( keyCode!=iExpectedKeyCode )
					{
					iDataWrapperBase.ERR_PRINTF2(KLogUnexpectedKeyCode, keyCode);
					iDataWrapperBase.SetBlockResult(EFail);
					}
				else
					{
					iDataWrapperBase.INFO_PRINTF3(_L("keycodes match as expected:%d, actual:%d"), iExpectedKeyCode,keyCode);
					}
				}
			}

		if ( config.iEventOccurance==EEventOccuranceOnce )
			{
			++iEventIndex;
			iEventCount=0;
			}
		if ( iEventIndex>=iEvent.Count() )
			{
			moreToDo=KickNext(aAsyncErrorIndex, aConsole);
			}
		else
			{
			//	Event can occur multiple times
			Kick(aAsyncErrorIndex, aConsole);
			}
		}

	return	moreToDo;
	}

TConsoleKey CT_ActiveRConsoleRead::GetConsoleKey()
	{
	return iKey;
	}

TBool CT_ActiveRConsoleRead::ExitEvent()
/**
 * Exit the testcase if certain scenarios are hit
 *
 *
 */
	{
	TBool	exitEvent=EFalse;

	iDataWrapperBase.INFO_PRINTF2(_L("checking if we are an exit event:%d"),iKey.Type() );

	switch (iKey.Type() )
		{
	case TRawEvent::ENone:
		break;
	case TRawEvent::EPointerMove:
	case TRawEvent::EButton1Down:
		break;
	case TRawEvent::EButton1Up:
		if ( iHasExitRectangle )
			{
			exitEvent=iExitRectangle.Contains(iKey.MousePos());
			}
		break;
	case TRawEvent::EButton2Down:
		break;
	case TRawEvent::EButton2Up:
		break;
	case TRawEvent::EButton3Down:
		break;
	case TRawEvent::EButton3Up:
		break;
	case TRawEvent::EKeyDown:
		break;
	case TRawEvent::EKeyUp:
		if ( iHasExitKeyCode )
			{
			iDataWrapperBase.INFO_PRINTF2(KLogKey, TInt (iKey.Code()));
			if ( !iKey.Code()==TKeyCode(iExitKeyCode) )
				{
				iDataWrapperBase.ERR_PRINTF3(_L("Test failed due to user feedback :actual key %d, expected key %d"), TInt(iKey.Code()), iExitKeyCode);
				iDataWrapperBase.SetBlockResult(EFail);
				}
			exitEvent=ETrue;
			}
		break;
	case TRawEvent::EKeyRepeat:
		break;
	default:
		break;
		}

	return exitEvent;
	}

void CT_ActiveRConsoleRead::DrawData()
	{
	switch (iKey.Type() )
		{
	case TRawEvent::ENone:
		break;
	case TRawEvent::EPointerMove:
	case TRawEvent::EButton1Down:
	case TRawEvent::EButton1Up:
	case TRawEvent::EButton2Down:
	case TRawEvent::EButton2Up:
	case TRawEvent::EButton3Down:
	case TRawEvent::EButton3Up:
		//	Draw the point
		CDrawUtils::ColourFillUtility(iColourValueBlack, iKey.MousePos());
		break;
	case TRawEvent::EKeyDown:
		break;
	case TRawEvent::EKeyUp:
		break;
	case TRawEvent::EKeyRepeat:
		break;
	default:
		break;
		}
	}
