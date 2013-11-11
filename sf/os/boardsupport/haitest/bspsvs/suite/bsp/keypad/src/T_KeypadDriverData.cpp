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

#include "T_KeypadDriverData.h"

_LIT(KEnumNull,						" ");
_LIT(KGetScanCodes,					"GetScanCodes");
_LIT(KGetModifiers,					"GetModifiers");
_LIT(KTestAllKeys,					"TestAllKeys");
_LIT(KTestOneModifierOneOtherKey,	"TestOneModifierOneOtherKey");
_LIT(KTestTwoKeys,					"TestTwoKeys");
_LIT(KTestMultipleKeys,				"TestMultipleKeys");
_LIT(KVerifyKeypadState,			"VerifyKeypadState");
_LIT(KPromptUser,					"PromptUser");
_LIT(KSet,							"set%d");
_LIT(KScanCodeString,				"scancode_string%d");
_LIT(KScanCode,						"scancode%d");
_LIT(KModifierString,				"modifier_string%d");
_LIT(KModifier,						"modifier%d");
_LIT(KTestCoverage,					"test_coverage");
_LIT(KFailTest,						"fail_test");
_LIT(KPassKeyStr,					"pass_key_str");
_LIT(KPassKey,						"pass_key");
_LIT(KExpectedState,				"expected_state");
_LIT(KTimeOut,						"timeout");
_LIT(KUserPrompt,					"user_prompt");
_LIT(KConsoleName,					"Console_Window");
_LIT(KMaximized,					"+M");
_LIT(KRawEventMode,					"+R");
_LIT(KRawEventModeOff,				"-R");

_LIT(KLogMissingParameter,			"Missing parameter '%S'");
/*@}*/

// Scancodes
_LIT(KStdKeyNumber0,				"'0'");
_LIT(KStdKeyNumber1,				"'1'");
_LIT(KStdKeyNumber2,				"'2'");
_LIT(KStdKeyNumber3,				"'3'");
_LIT(KStdKeyNumber4,				"'4'");
_LIT(KStdKeyNumber5,				"'5'");
_LIT(KStdKeyNumber6,				"'6'");
_LIT(KStdKeyNumber7,				"'7'");
_LIT(KStdKeyNumber8,				"'8'");
_LIT(KStdKeyNumber9,				"'9'");
_LIT(KStdKeyLetterA,				"'A'");
_LIT(KStdKeyLetterB,				"'B'");
_LIT(KStdKeyLetterC,				"'C'");
_LIT(KStdKeyLetterD,				"'D'");
_LIT(KStdKeyLetterE,				"'E'");
_LIT(KStdKeyLetterF,				"'F'");
_LIT(KStdKeyLetterG,				"'G'");
_LIT(KStdKeyLetterH,				"'H'");
_LIT(KStdKeyLetterI,				"'I'");
_LIT(KStdKeyLetterJ,				"'J'");
_LIT(KStdKeyLetterK,				"'K'");
_LIT(KStdKeyLetterL,				"'L'");
_LIT(KStdKeyLetterM,				"'M'");
_LIT(KStdKeyLetterN,				"'N'");
_LIT(KStdKeyLetterO,				"'O'");
_LIT(KStdKeyLetterP,				"'P'");
_LIT(KStdKeyLetterQ,				"'Q'");
_LIT(KStdKeyLetterR,				"'R'");
_LIT(KStdKeyLetterS,				"'S'");
_LIT(KStdKeyLetterT,				"'T'");
_LIT(KStdKeyLetterU,				"'U'");
_LIT(KStdKeyLetterV,				"'V'");
_LIT(KStdKeyLetterW,				"'W'");
_LIT(KStdKeyLetterX,				"'X'");
_LIT(KStdKeyLetterY,				"'Y'");
_LIT(KStdKeyLetterZ,				"'Z'");
_LIT(KStdKeyAsterisk,				"'*'");
_LIT(KStdKeyNull,					"EStdKeyNull");
_LIT(KStdKeyBackspace,				"EStdKeyBackspace");
_LIT(KStdKeyTab,					"EStdKeyTab");
_LIT(KStdKeyEnter,					"EStdKeyEnter");
_LIT(KStdKeyEscape,					"EStdKeyEscape");
_LIT(KStdKeySpace,					"EStdKeySpace");
_LIT(KStdKeyPrintScreen,			"EStdKeyPrintScreen");
_LIT(KStdKeyPause,					"EStdKeyPause");
_LIT(KStdKeyHome,					"EStdKeyHome");
_LIT(KStdKeyEnd,					"EStdKeyEnd");
_LIT(KStdKeyPageUp,					"EStdKeyPageUp");
_LIT(KStdKeyPageDown,				"EStdKeyPageDown");
_LIT(KStdKeyInsert,					"EStdKeyInsert");
_LIT(KStdKeyDelete,					"EStdKeyDelete");
_LIT(KStdKeyLeftArrow,				"EStdKeyLeftArrow");
_LIT(KStdKeyRightArrow,				"EStdKeyRightArrow");
_LIT(KStdKeyUpArrow,				"EStdKeyUpArrow");
_LIT(KStdKeyDownArrow,				"EStdKeyDownArrow");
_LIT(KStdKeyLeftShift,				"EStdKeyLeftShift");
_LIT(KStdKeyRightShift,				"EStdKeyRightShift");
_LIT(KStdKeyLeftAlt,				"EStdKeyLeftAlt");
_LIT(KStdKeyRightAlt,				"EStdKeyRightAlt");
_LIT(KStdKeyLeftCtrl,				"EStdKeyLeftCtrl");
_LIT(KStdKeyRightCtrl,				"EStdKeyRightCtrl");
_LIT(KStdKeyLeftFunc,				"EStdKeyLeftFunc");
_LIT(KStdKeyRightFunc,				"EStdKeyRightFunc");
_LIT(KStdKeyCapsLock,				"EStdKeyCapsLock");
_LIT(KStdKeyNumLock,				"EStdKeyNumLock");
_LIT(KStdKeyScrollLock,				"EStdKeyScrollLock");
_LIT(KStdKeyF1,						"EStdKeyF1");
_LIT(KStdKeyF2,						"EStdKeyF2");
_LIT(KStdKeyF3,						"EStdKeyF3");
_LIT(KStdKeyF4,						"EStdKeyF4");
_LIT(KStdKeyF5,						"EStdKeyF5");
_LIT(KStdKeyF6,						"EStdKeyF6");
_LIT(KStdKeyF7,						"EStdKeyF7");
_LIT(KStdKeyF8,						"EStdKeyF8");
_LIT(KStdKeyF9,						"EStdKeyF9");
_LIT(KStdKeyF10,					"EStdKeyF10");
_LIT(KStdKeyF11,					"EStdKeyF11");
_LIT(KStdKeyF12,					"EStdKeyF12");
_LIT(KStdKeyF13,					"EStdKeyF13");
_LIT(KStdKeyF14,					"EStdKeyF14");
_LIT(KStdKeyF15,					"EStdKeyF15");
_LIT(KStdKeyF16,					"EStdKeyF16");
_LIT(KStdKeyF17,					"EStdKeyF17");
_LIT(KStdKeyF18,					"EStdKeyF18");
_LIT(KStdKeyF19,					"EStdKeyF19");
_LIT(KStdKeyF20,					"EStdKeyF20");
_LIT(KStdKeyF21,					"EStdKeyF21");
_LIT(KStdKeyF22,					"EStdKeyF22");
_LIT(KStdKeyF23,					"EStdKeyF23");
_LIT(KStdKeyF24,					"EStdKeyF24");
_LIT(KStdKeyXXX,					"EStdKeyXXX");
_LIT(KStdKeyComma,					"EStdKeyComma");
_LIT(KStdKeyFullStop,				"EStdKeyFullStop");
_LIT(KStdKeyForwardSlash,			"EStdKeyForwardSlash");
_LIT(KStdKeyBackSlash,				"EStdKeyBackSlash");
_LIT(KStdKeySemiColon,				"EStdKeySemiColon");
_LIT(KStdKeySingleQuote,			"EStdKeySingleQuote");
_LIT(KStdKeyHash,					"EStdKeyHash");
_LIT(KStdKeySquareBracketLeft,		"EStdKeySquareBracketLeft");
_LIT(KStdKeySquareBracketRight,		"EStdKeySquareBracketRight");
_LIT(KStdKeyMinus,					"EStdKeyMinus");
_LIT(KStdKeyEquals,					"EStdKeyEquals");
_LIT(KStdKeyNkpForwardSlash,		"EStdKeyNkpForwardSlash");
_LIT(KStdKeyNkpAsterisk,			"EStdKeyNkpAsterisk");
_LIT(KStdKeyNkpMinus,				"EStdKeyNkpMinus");
_LIT(KStdKeyNkpPlus,				"EStdKeyNkpPlus");
_LIT(KStdKeyNkpEnter,				"EStdKeyNkpEnter");
_LIT(KStdKeyNkp1,					"EStdKeyNkp1");
_LIT(KStdKeyNkp2,					"EStdKeyNkp2");
_LIT(KStdKeyNkp3,					"EStdKeyNkp3");
_LIT(KStdKeyNkp4,					"EStdKeyNkp4");
_LIT(KStdKeyNkp5,					"EStdKeyNkp5");
_LIT(KStdKeyNkp6,					"EStdKeyNkp6");
_LIT(KStdKeyNkp7,					"EStdKeyNkp7");
_LIT(KStdKeyNkp8,					"EStdKeyNkp8");
_LIT(KStdKeyNkp9,					"EStdKeyNkp9");
_LIT(KStdKeyNkp0,					"EStdKeyNkp0");
_LIT(KStdKeyNkpFullStop,			"EStdKeyNkpFullStop");
_LIT(KStdKeyMenu,					"EStdKeyMenu");
_LIT(KStdKeyBacklightOn,			"EStdKeyBacklightOn");
_LIT(KStdKeyBacklightOff,			"EStdKeyBacklightOff");
_LIT(KStdKeyBacklightToggle,		"EStdKeyBacklightToggle");
_LIT(KStdKeyIncContrast,			"EStdKeyIncContrast");
_LIT(KStdKeyDecContrast,			"EStdKeyDecContrast");
_LIT(KStdKeySliderDown,				"EStdKeySliderDown");
_LIT(KStdKeySliderUp,				"EStdKeySliderUp");
_LIT(KStdKeyDictaphonePlay,			"EStdKeyDictaphonePlay");
_LIT(KStdKeyDictaphoneStop,			"EStdKeyDictaphoneStop");
_LIT(KStdKeyDictaphoneRecord,		"EStdKeyDictaphoneRecord");
_LIT(KStdKeyHelp,					"EStdKeyHelp");
_LIT(KStdKeyOff,					"EStdKeyOff");
_LIT(KStdKeyDial,					"EStdKeyDial");
_LIT(KStdKeyIncVolume,				"EStdKeyIncVolume");
_LIT(KStdKeyDecVolume,				"EStdKeyDecVolume");
_LIT(KStdKeyDevice0,				"EStdKeyDevice0");
_LIT(KStdKeyDevice1,				"EStdKeyDevice1");
_LIT(KStdKeyDevice2,				"EStdKeyDevice2");
_LIT(KStdKeyDevice3,				"EStdKeyDevice3");
_LIT(KStdKeyDevice4,				"EStdKeyDevice4");
_LIT(KStdKeyDevice5,				"EStdKeyDevice5");
_LIT(KStdKeyDevice6,				"EStdKeyDevice6");
_LIT(KStdKeyDevice7,				"EStdKeyDevice7");
_LIT(KStdKeyDevice8,				"EStdKeyDevice8");
_LIT(KStdKeyDevice9,				"EStdKeyDevice9");
_LIT(KStdKeyDeviceA,				"EStdKeyDeviceA");
_LIT(KStdKeyDeviceB,				"EStdKeyDeviceB");
_LIT(KStdKeyDeviceC,				"EStdKeyDeviceC");
_LIT(KStdKeyDeviceD,				"EStdKeyDeviceD");
_LIT(KStdKeyDeviceE,				"EStdKeyDeviceE");
_LIT(KStdKeyDeviceF,				"EStdKeyDeviceF");
_LIT(KStdKeyApplication0,			"EStdKeyApplication0");
_LIT(KStdKeyApplication1,			"EStdKeyApplication1");
_LIT(KStdKeyApplication2,			"EStdKeyApplication2");
_LIT(KStdKeyApplication3,			"EStdKeyApplication3");
_LIT(KStdKeyApplication4,			"EStdKeyApplication4");
_LIT(KStdKeyApplication5,			"EStdKeyApplication5");
_LIT(KStdKeyApplication6,			"EStdKeyApplication6");
_LIT(KStdKeyApplication7,			"EStdKeyApplication7");
_LIT(KStdKeyApplication8,			"EStdKeyApplication8");
_LIT(KStdKeyApplication9,			"EStdKeyApplication9");
_LIT(KStdKeyApplicationA,			"EStdKeyApplicationA");
_LIT(KStdKeyApplicationB,			"EStdKeyApplicationB");
_LIT(KStdKeyApplicationC,			"EStdKeyApplicationC");
_LIT(KStdKeyApplicationD,			"EStdKeyApplicationD");
_LIT(KStdKeyApplicationE,			"EStdKeyApplicationE");
_LIT(KStdKeyApplicationF,			"EStdKeyApplicationF");
_LIT(KStdKeyYes,					"EStdKeyYes");
_LIT(KStdKeyNo,						"EStdKeyNo");
_LIT(KStdKeyIncBrightness,			"EStdKeyIncBrightness");
_LIT(KStdKeyDecBrightness,			"EStdKeyDecBrightness");
_LIT(KStdKeyKeyboardExtend,			"EStdKeyKeyboardExtend");
_LIT(KStdKeyDevice10,				"EStdKeyDevice10");
_LIT(KStdKeyDevice11,				"EStdKeyDevice11");
_LIT(KStdKeyDevice12,				"EStdKeyDevice12");
_LIT(KStdKeyDevice13,				"EStdKeyDevice13");
_LIT(KStdKeyDevice14,				"EStdKeyDevice14");
_LIT(KStdKeyDevice15,				"EStdKeyDevice15");
_LIT(KStdKeyDevice16,				"EStdKeyDevice16");
_LIT(KStdKeyDevice17,				"EStdKeyDevice17");
_LIT(KStdKeyDevice18,				"EStdKeyDevice18");
_LIT(KStdKeyDevice19,				"EStdKeyDevice19");
_LIT(KStdKeyDevice1A,				"EStdKeyDevice1A");
_LIT(KStdKeyDevice1B,				"EStdKeyDevice1B");
_LIT(KStdKeyDevice1C,				"EStdKeyDevice1C");
_LIT(KStdKeyDevice1D,				"EStdKeyDevice1D");
_LIT(KStdKeyDevice1E,				"EStdKeyDevice1E");
_LIT(KStdKeyDevice1F,				"EStdKeyDevice1F");
_LIT(KStdKeyApplication10,			"EStdKeyApplication10");
_LIT(KStdKeyApplication11,			"EStdKeyApplication11");
_LIT(KStdKeyApplication12,			"EStdKeyApplication12");
_LIT(KStdKeyApplication13,			"EStdKeyApplication13");
_LIT(KStdKeyApplication14,			"EStdKeyApplication14");
_LIT(KStdKeyApplication15,			"EStdKeyApplication15");
_LIT(KStdKeyApplication16,			"EStdKeyApplication16");
_LIT(KStdKeyApplication17,			"EStdKeyApplication17");
_LIT(KStdKeyApplication18,			"EStdKeyApplication18");
_LIT(KStdKeyApplication19,			"EStdKeyApplication19");
_LIT(KStdKeyApplication1A,			"EStdKeyApplication1A");
_LIT(KStdKeyApplication1B,			"EStdKeyApplication1B");
_LIT(KStdKeyApplication1C,			"EStdKeyApplication1C");
_LIT(KStdKeyApplication1D,			"EStdKeyApplication1D");
_LIT(KStdKeyApplication1E,			"EStdKeyApplication1E");
_LIT(KStdKeyApplication1F,			"EStdKeyApplication1F");
_LIT(KStdKeyDevice20,				"EStdKeyDevice20");
_LIT(KStdKeyDevice21,				"EStdKeyDevice21");
_LIT(KStdKeyDevice22,				"EStdKeyDevice22");
_LIT(KStdKeyDevice23,				"EStdKeyDevice23");
_LIT(KStdKeyDevice24,				"EStdKeyDevice24");
_LIT(KStdKeyDevice25,				"EStdKeyDevice25");
_LIT(KStdKeyDevice26,				"EStdKeyDevice26");
_LIT(KStdKeyDevice27,				"EStdKeyDevice27");
_LIT(KStdKeyApplication20,			"EStdKeyApplication20");
_LIT(KStdKeyApplication21,			"EStdKeyApplication21");
_LIT(KStdKeyApplication22,			"EStdKeyApplication22");
_LIT(KStdKeyApplication23,			"EStdKeyApplication23");
_LIT(KStdKeyApplication24,			"EStdKeyApplication24");
_LIT(KStdKeyApplication25,			"EStdKeyApplication25");
_LIT(KStdKeyApplication26,			"EStdKeyApplication26");
_LIT(KStdKeyApplication27,			"EStdKeyApplication27");
const CDataWrapperBase::TEnumEntryTable	CT_KeypadDriverData::iEnumTableScanCodes[] =
	{
//	Scancode as a descriptor		Scancode
	KStdKeyNumber0,					(TStdScanCode) '0',
	KStdKeyNumber1,					(TStdScanCode) '1',
	KStdKeyNumber2,					(TStdScanCode) '2',
	KStdKeyNumber3,					(TStdScanCode) '3',
	KStdKeyNumber4,					(TStdScanCode) '4',
	KStdKeyNumber5,					(TStdScanCode) '5',
	KStdKeyNumber6,					(TStdScanCode) '6',
	KStdKeyNumber7,					(TStdScanCode) '7',
	KStdKeyNumber8,					(TStdScanCode) '8',
	KStdKeyNumber9,					(TStdScanCode) '9',
	KStdKeyLetterA,					(TStdScanCode) 'A',
	KStdKeyLetterB,					(TStdScanCode) 'B',
	KStdKeyLetterC,					(TStdScanCode) 'C',
	KStdKeyLetterD,					(TStdScanCode) 'D',
	KStdKeyLetterE,					(TStdScanCode) 'E',
	KStdKeyLetterF,					(TStdScanCode) 'F',
	KStdKeyLetterG,					(TStdScanCode) 'G',
	KStdKeyLetterH,					(TStdScanCode) 'H',
	KStdKeyLetterI,					(TStdScanCode) 'I',
	KStdKeyLetterJ,					(TStdScanCode) 'J',
	KStdKeyLetterK,					(TStdScanCode) 'K',
	KStdKeyLetterL,					(TStdScanCode) 'L',
	KStdKeyLetterM,					(TStdScanCode) 'M',
	KStdKeyLetterN,					(TStdScanCode) 'N',
	KStdKeyLetterO,					(TStdScanCode) 'O',
	KStdKeyLetterP,					(TStdScanCode) 'P',
	KStdKeyLetterQ,					(TStdScanCode) 'Q',
	KStdKeyLetterR,					(TStdScanCode) 'R',
	KStdKeyLetterS,					(TStdScanCode) 'S',
	KStdKeyLetterT,					(TStdScanCode) 'T',
	KStdKeyLetterU,					(TStdScanCode) 'U',
	KStdKeyLetterV,					(TStdScanCode) 'V',
	KStdKeyLetterW,					(TStdScanCode) 'W',
	KStdKeyLetterX,					(TStdScanCode) 'X',
	KStdKeyLetterY,					(TStdScanCode) 'Y',
	KStdKeyLetterZ,					(TStdScanCode) 'Z',
	KStdKeyAsterisk,				(TStdScanCode) '*',
	KStdKeyNull,					EStdKeyNull,
	KStdKeyBackspace,				EStdKeyBackspace,
	KStdKeyTab,						EStdKeyTab,
	KStdKeyEnter,					EStdKeyEnter,
	KStdKeyEscape,					EStdKeyEscape,
	KStdKeySpace,					EStdKeySpace,
	KStdKeyPrintScreen,				EStdKeyPrintScreen,
	KStdKeyPause,					EStdKeyPause,
	KStdKeyHome,					EStdKeyHome,
	KStdKeyEnd,						EStdKeyEnd,
	KStdKeyPageUp,					EStdKeyPageUp,
	KStdKeyPageDown,				EStdKeyPageDown,
	KStdKeyInsert,					EStdKeyInsert,
	KStdKeyDelete,					EStdKeyDelete,
	KStdKeyLeftArrow,				EStdKeyLeftArrow,
	KStdKeyRightArrow,				EStdKeyRightArrow,
	KStdKeyUpArrow,					EStdKeyUpArrow,
	KStdKeyDownArrow,				EStdKeyDownArrow,
	KStdKeyLeftShift,				EStdKeyLeftShift,
	KStdKeyRightShift,				EStdKeyRightShift,
	KStdKeyLeftAlt,					EStdKeyLeftAlt,
	KStdKeyRightAlt,				EStdKeyRightAlt,
	KStdKeyLeftCtrl,				EStdKeyLeftCtrl,
	KStdKeyRightCtrl,				EStdKeyRightCtrl,
	KStdKeyLeftFunc,				EStdKeyLeftFunc,
	KStdKeyRightFunc,				EStdKeyRightFunc,
	KStdKeyCapsLock,				EStdKeyCapsLock,
	KStdKeyNumLock,					EStdKeyNumLock,
	KStdKeyScrollLock,				EStdKeyScrollLock,
	KStdKeyF1,						EStdKeyF1,
	KStdKeyF2,						EStdKeyF2,
	KStdKeyF3,						EStdKeyF3,
	KStdKeyF4,						EStdKeyF4,
	KStdKeyF5,						EStdKeyF5,
	KStdKeyF6,						EStdKeyF6,
	KStdKeyF7,						EStdKeyF7,
	KStdKeyF8,						EStdKeyF8,
	KStdKeyF9,						EStdKeyF9,
	KStdKeyF10,						EStdKeyF10,
	KStdKeyF11,						EStdKeyF11,
	KStdKeyF12,						EStdKeyF12,
	KStdKeyF13,						EStdKeyF13,
	KStdKeyF14,						EStdKeyF14,
	KStdKeyF15,						EStdKeyF15,
	KStdKeyF16,						EStdKeyF16,
	KStdKeyF17,						EStdKeyF17,
	KStdKeyF18,						EStdKeyF18,
	KStdKeyF19,						EStdKeyF19,
	KStdKeyF20,						EStdKeyF20,
	KStdKeyF21,						EStdKeyF21,
	KStdKeyF22,						EStdKeyF22,
	KStdKeyF23,						EStdKeyF23,
	KStdKeyF24,						EStdKeyF24,
	KStdKeyXXX,						EStdKeyXXX,
	KStdKeyComma,					EStdKeyComma,
	KStdKeyFullStop,				EStdKeyFullStop,
	KStdKeyForwardSlash,			EStdKeyForwardSlash,
	KStdKeyBackSlash,				EStdKeyBackSlash,
	KStdKeySemiColon,				EStdKeySemiColon,
	KStdKeySingleQuote,				EStdKeySingleQuote,
	KStdKeyHash,					EStdKeyHash,
	KStdKeySquareBracketLeft,		EStdKeySquareBracketLeft,
	KStdKeySquareBracketRight,		EStdKeySquareBracketRight,
	KStdKeyMinus,					EStdKeyMinus,
	KStdKeyEquals,					EStdKeyEquals,
	KStdKeyNkpForwardSlash,			EStdKeyNkpForwardSlash,
	KStdKeyNkpAsterisk,				EStdKeyNkpAsterisk,
	KStdKeyNkpMinus,				EStdKeyNkpMinus,
	KStdKeyNkpPlus,					EStdKeyNkpPlus,
	KStdKeyNkpEnter,				EStdKeyNkpEnter,
	KStdKeyNkp1,					EStdKeyNkp1,
	KStdKeyNkp2,					EStdKeyNkp2,
	KStdKeyNkp3,					EStdKeyNkp3,
	KStdKeyNkp4,					EStdKeyNkp4,
	KStdKeyNkp5,					EStdKeyNkp5,
	KStdKeyNkp6,					EStdKeyNkp6,
	KStdKeyNkp7,					EStdKeyNkp7,
	KStdKeyNkp8,					EStdKeyNkp8,
	KStdKeyNkp9,					EStdKeyNkp9,
	KStdKeyNkp0,					EStdKeyNkp0,
	KStdKeyNkpFullStop,				EStdKeyNkpFullStop,
	KStdKeyMenu,					EStdKeyMenu,
	KStdKeyBacklightOn,				EStdKeyBacklightOn,
	KStdKeyBacklightOff,			EStdKeyBacklightOff,
	KStdKeyBacklightToggle,			EStdKeyBacklightToggle,
	KStdKeyIncContrast,				EStdKeyIncContrast,
	KStdKeyDecContrast,				EStdKeyDecContrast,
	KStdKeySliderDown,				EStdKeySliderDown,
	KStdKeySliderUp,				EStdKeySliderUp,
	KStdKeyDictaphonePlay,			EStdKeyDictaphonePlay,
	KStdKeyDictaphoneStop,			EStdKeyDictaphoneStop,
	KStdKeyDictaphoneRecord,		EStdKeyDictaphoneRecord,
	KStdKeyHelp,					EStdKeyHelp,
	KStdKeyOff,						EStdKeyOff,
	KStdKeyDial,					EStdKeyDial,
	KStdKeyIncVolume,				EStdKeyIncVolume,
	KStdKeyDecVolume,				EStdKeyDecVolume,
	KStdKeyDevice0,					EStdKeyDevice0,
	KStdKeyDevice1,					EStdKeyDevice1,
	KStdKeyDevice2,					EStdKeyDevice2,
	KStdKeyDevice3,					EStdKeyDevice3,
	KStdKeyDevice4,					EStdKeyDevice4,
	KStdKeyDevice5,					EStdKeyDevice5,
	KStdKeyDevice6,					EStdKeyDevice6,
	KStdKeyDevice7,					EStdKeyDevice7,
	KStdKeyDevice8,					EStdKeyDevice8,
	KStdKeyDevice9,					EStdKeyDevice9,
	KStdKeyDeviceA,					EStdKeyDeviceA,
	KStdKeyDeviceB,					EStdKeyDeviceB,
	KStdKeyDeviceC,					EStdKeyDeviceC,
	KStdKeyDeviceD,					EStdKeyDeviceD,
	KStdKeyDeviceE,					EStdKeyDeviceE,
	KStdKeyDeviceF,					EStdKeyDeviceF,
	KStdKeyApplication0,			EStdKeyApplication0,
	KStdKeyApplication1,			EStdKeyApplication1,
	KStdKeyApplication2,			EStdKeyApplication2,
	KStdKeyApplication3,			EStdKeyApplication3,
	KStdKeyApplication4,			EStdKeyApplication4,
	KStdKeyApplication5,			EStdKeyApplication5,
	KStdKeyApplication6,			EStdKeyApplication6,
	KStdKeyApplication7,			EStdKeyApplication7,
	KStdKeyApplication8,			EStdKeyApplication8,
	KStdKeyApplication9,			EStdKeyApplication9,
	KStdKeyApplicationA,			EStdKeyApplicationA,
	KStdKeyApplicationB,			EStdKeyApplicationB,
	KStdKeyApplicationC,			EStdKeyApplicationC,
	KStdKeyApplicationD,			EStdKeyApplicationD,
	KStdKeyApplicationE,			EStdKeyApplicationE,
	KStdKeyApplicationF,			EStdKeyApplicationF,
	KStdKeyYes,						EStdKeyYes,
	KStdKeyNo,						EStdKeyNo,
	KStdKeyIncBrightness,			EStdKeyIncBrightness,
	KStdKeyDecBrightness,			EStdKeyDecBrightness,
	KStdKeyKeyboardExtend,			EStdKeyKeyboardExtend,
	KStdKeyDevice10,				EStdKeyDevice10,
	KStdKeyDevice11,				EStdKeyDevice11,
	KStdKeyDevice12,				EStdKeyDevice12,
	KStdKeyDevice13,				EStdKeyDevice13,
	KStdKeyDevice14,				EStdKeyDevice14,
	KStdKeyDevice15,				EStdKeyDevice15,
	KStdKeyDevice16,				EStdKeyDevice16,
	KStdKeyDevice17,				EStdKeyDevice17,
	KStdKeyDevice18,				EStdKeyDevice18,
	KStdKeyDevice19,				EStdKeyDevice19,
	KStdKeyDevice1A,				EStdKeyDevice1A,
	KStdKeyDevice1B,				EStdKeyDevice1B,
	KStdKeyDevice1C,				EStdKeyDevice1C,
	KStdKeyDevice1D,				EStdKeyDevice1D,
	KStdKeyDevice1E,				EStdKeyDevice1E,
	KStdKeyDevice1F,				EStdKeyDevice1F,
	KStdKeyApplication10,			EStdKeyApplication10,
	KStdKeyApplication11,			EStdKeyApplication11,
	KStdKeyApplication12,			EStdKeyApplication12,
	KStdKeyApplication13,			EStdKeyApplication13,
	KStdKeyApplication14,			EStdKeyApplication14,
	KStdKeyApplication15,			EStdKeyApplication15,
	KStdKeyApplication16,			EStdKeyApplication16,
	KStdKeyApplication17,			EStdKeyApplication17,
	KStdKeyApplication18,			EStdKeyApplication18,
	KStdKeyApplication19,			EStdKeyApplication19,
	KStdKeyApplication1A,			EStdKeyApplication1A,
	KStdKeyApplication1B,			EStdKeyApplication1B,
	KStdKeyApplication1C,			EStdKeyApplication1C,
	KStdKeyApplication1D,			EStdKeyApplication1D,
	KStdKeyApplication1E,			EStdKeyApplication1E,
	KStdKeyApplication1F,			EStdKeyApplication1F,
	KStdKeyDevice20,				EStdKeyDevice20,
	KStdKeyDevice21,				EStdKeyDevice21,
	KStdKeyDevice22,				EStdKeyDevice22,
	KStdKeyDevice23,				EStdKeyDevice23,
	KStdKeyDevice24,				EStdKeyDevice24,
	KStdKeyDevice25,				EStdKeyDevice25,
	KStdKeyDevice26,				EStdKeyDevice26,
	KStdKeyDevice27,				EStdKeyDevice27,
	KStdKeyApplication20,			EStdKeyApplication20,
	KStdKeyApplication21,			EStdKeyApplication21,
	KStdKeyApplication22,			EStdKeyApplication22,
	KStdKeyApplication23,			EStdKeyApplication23,
	KStdKeyApplication24,			EStdKeyApplication24,
	KStdKeyApplication25,			EStdKeyApplication25,
	KStdKeyApplication26,			EStdKeyApplication26,
	KStdKeyApplication27,			EStdKeyApplication27,
	KEnumNull,						(TStdScanCode) -1
	};

_LIT(KEnumCoverageVeryLow,			"VeryLow");
_LIT(KEnumCoverageLow,				"Low");
_LIT(KEnumCoverageMedium,			"Medium");
_LIT(KEnumCoverageFull,				"Full");
const CDataWrapperBase::TEnumEntryTable	CT_KeypadDriverData::iEnumTableCoverage[] =
	{
	KEnumCoverageVeryLow,			8,
	KEnumCoverageLow,				4,
	KEnumCoverageMedium,			2,
	KEnumCoverageFull,				1,
	KEnumNull,						-1
	};

_LIT(KEnumKeyboardState,					"EKeyboardState");
_LIT(KEnumKeyboard,							"EKeyboard");
_LIT(KEnumKeyboardDeviceKeys,				"EKeyboardDeviceKeys");
_LIT(KEnumKeyboardAppKeys,					"EKeyboardAppKeys");
_LIT(KEnumKeyboardClick,					"EKeyboardClick");
_LIT(KEnumKeyboardClickVolumeMax,			"EKeyboardClickVolumeMax");
_LIT(KEnumKeyboardClickState,				"EKeyboardClickState");
_LIT(KEnumKeyboardClickVolume,				"EKeyboardClickVolume");
const CT_HALData::THalTableLookup	CT_KeypadDriverData::iTableLookup[] =
	{
//	Enum as a descriptor			Enum								Prepare input	Prepare				Validation of		Store data returned after a		Flag indicating
//																		value prior to	parameters			value returned		successful HAL::Get or			if validation
//																		HAL::Set call	prior to 			after a successful	HAL::Set call					is mandatory
//																						HAL::Get call		HAL::Get call
	KEnumKeyboardState,				HALData::EKeyboardState,			SetPrepareInt,	GetPrepareNothing,	GetValidationInt,	SetKeyboardState,				ETrue,
	KEnumKeyboard,					HALData::EKeyboard,					SetPrepareInt,	GetPrepareNothing,	GetValidationInt,	SetKeyboard,					ETrue,
	KEnumKeyboardDeviceKeys,		HALData::EKeyboardDeviceKeys,		SetPrepareInt,	GetPrepareNothing,	GetValidationInt,	SetKeyboardDeviceKeys,			ETrue,
	KEnumKeyboardAppKeys,			HALData::EKeyboardAppKeys,			SetPrepareInt,	GetPrepareNothing,	GetValidationInt,	SetKeyboardAppKeys,				ETrue,
	KEnumKeyboardClick,				HALData::EKeyboardClick,			SetPrepareInt,	GetPrepareNothing,	GetValidationInt,	SetKeyboardClick,				ETrue,
	KEnumKeyboardClickVolumeMax,	HALData::EKeyboardClickVolumeMax,	SetPrepareInt,	GetPrepareNothing,	GetValidationInt,	SetKeyboardClickVolumeMax,		ETrue,
	KEnumKeyboardClickState,		HALData::EKeyboardClickState,		SetPrepareInt,	GetPrepareNothing,	GetValidationInt,	SetKeyboardClickState,			ETrue,
	KEnumKeyboardClickVolume,		HALData::EKeyboardClickVolume,		SetPrepareInt,	GetPrepareNothing,	GetValidationInt,	SetKeyboardClickVolume,			ETrue,
	KEnumNull,						(HALData::TAttribute)0,				NULL,			NULL,				NULL,				NULL,							NULL
	};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_KeypadDriverData* CT_KeypadDriverData::NewL()
	{
	CT_KeypadDriverData*	ret=new (ELeave) CT_KeypadDriverData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


CT_KeypadDriverData::CT_KeypadDriverData()
:	CT_HALData(iTableLookup)
,	iKeyboardState(-1)
,	iKeyboard(-1)
,	iKeyboardDeviceKeys(-1)
,	iKeyboardAppKeys(-1)
,	iKeyboardClick(-1)
,	iKeyboardClickVolumeMax(-1)
,	iKeyboardClickState(-1)
,	iKeyboardClickVolume(-1)
,	iActiveKey(NULL)
,	iActiveCombination(NULL)
,	iActiveKeypadState(NULL)
,	iActivePrompt(NULL)
,	iExpectedState(0)
,	iKeyCount(0)
,	iCombinationCount(0)
,	iCombinationKeyCount(0)
,	iFailTest(EFalse)
,	iPassKey(0)
	{
	}

void CT_KeypadDriverData::ConstructL()
/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
	{
	iActiveKey 				= CActiveCallback::NewL(*this);
	iActiveCombination		= CActiveCallback::NewL(*this);
	iActiveKeypadState		= CActiveCallbackBase::NewL(*this);
	iActivePrompt			= CActiveCallback::NewL(*this);

	User::LeaveIfError(iConsole.Init(KConsoleName,TSize(KConsFullScreen,KConsFullScreen)));
	User::LeaveIfError(iConsole.Control(KMaximized));
	User::LeaveIfError(iConsole.Control(KRawEventMode));

	iEventStore.AppendL(TRawEvent::EKeyDown);
	iEventStore.AppendL(TRawEvent::EKeyUp);
	}

CT_KeypadDriverData::~CT_KeypadDriverData()
/**
 * Public destructor
 */
	{
	iConsole.Control(KRawEventModeOff);
	iConsole.Destroy();

	iPossibleScanCodeStrStore.Reset();
	iPossibleScanCodeStrStore.Close();
	iPossibleScanCodeStore.Reset();
	iPossibleScanCodeStore.Close();
	iPossibleModifierStrStore.Reset();
	iPossibleModifierStrStore.Close();
	iPossibleModifierStore.Reset();
	iPossibleModifierStore.Close();
	iAllPossibleKeysStrStore.Reset();
	iAllPossibleKeysStrStore.Close();
	iAllPossibleKeysStore.Reset();
	iAllPossibleKeysStore.Close();
	iCombinationStrStore.Reset();
	iCombinationStrStore.Close();
	iExpectedStore.Reset();
	iExpectedStore.Close();
	iActualStore.Reset();
	iActualStore.Close();
	iEventStore.Reset();
	iEventStore.Close();

	delete iActiveKey;
	delete iActiveCombination;
	delete iActiveKeypadState;
	delete iActivePrompt;
	}

TAny* CT_KeypadDriverData::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return NULL;
	}

//	An empty function to ensure that the code will work fine with HAL_Data
TBool CT_KeypadDriverData::GetPrepareNothing(CDataWrapperBase* /*aThis*/, const TDesC& /*aSection*/, TInt& /*aValueStart*/, TInt& /*aValueEnd*/)
	{
	return ETrue;
	}

//	Store the keypad state
void CT_KeypadDriverData::SetKeyboardState(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_KeypadDriverData*>(aThis)->iKeyboardState=aValue;
	}

//	Store the keypad type
void CT_KeypadDriverData::SetKeyboard(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_KeypadDriverData*>(aThis)->iKeyboard=aValue;
	}

//	Store the amount of keypad device keys
void CT_KeypadDriverData::SetKeyboardDeviceKeys(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_KeypadDriverData*>(aThis)->iKeyboardDeviceKeys=aValue;
	}

//	Store the amount of keypad app keys
void CT_KeypadDriverData::SetKeyboardAppKeys(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_KeypadDriverData*>(aThis)->iKeyboardAppKeys=aValue;
	}

//	Store the keypad click indicator
void CT_KeypadDriverData::SetKeyboardClick(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_KeypadDriverData*>(aThis)->iKeyboardClick=aValue;
	}

//	Store the keypad max click volume
void CT_KeypadDriverData::SetKeyboardClickVolumeMax(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_KeypadDriverData*>(aThis)->iKeyboardClickVolumeMax=aValue;
	}

//	Store the keypad click state
void CT_KeypadDriverData::SetKeyboardClickState(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_KeypadDriverData*>(aThis)->iKeyboardClickState=aValue;
	}

//	Store the keypad click volume
void CT_KeypadDriverData::SetKeyboardClickVolume(CDataWrapperBase* aThis, TInt aValue)
	{
	static_cast<CT_KeypadDriverData*>(aThis)->iKeyboardClickVolume=aValue;
	}

TBool CT_KeypadDriverData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
	{
	TBool	ret = ETrue;

	if ( aCommand == KGetScanCodes )
		{
		DoCmdGetScanCodesL(aSection);
		}
	else if ( aCommand == KGetModifiers )
		{
		DoCmdGetModifiersL(aSection);
		}
	else if ( aCommand == KTestAllKeys )
		{
		DoCmdTestAllKeysL(aAsyncErrorIndex);
		}
	else if ( aCommand == KTestOneModifierOneOtherKey )
		{
		DoCmdTestOneModifierOneOtherKeyL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KTestTwoKeys )
		{
		DoCmdTestTwoKeysL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KTestMultipleKeys )
		{
		DoCmdTestMultipleKeysL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KVerifyKeypadState )
		{
		DoCmdVerifyKeypadState(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KPromptUser )
		{
		DoCmdPromptUser(aSection, aAsyncErrorIndex);
		}
	else
		{
		ret = CT_HALData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	return ret;
	}


void CT_KeypadDriverData::DoCmdGetScanCodesL(const TDesC& aSection)
/**
 * Get the scancodes of the platform
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
	{
	iPossibleScanCodeStrStore.Reset();
	iPossibleScanCodeStore.Reset();
	iAllPossibleKeysStrStore.Reset();
	iAllPossibleKeysStore.Reset();

	TIniDataName	tempStore;
	TPtrC			scanCodeStr;
	TPtrC			scanCodeName;
	TInt		 	scanCode;

	TInt	index=0;
	TBool	moreData=ETrue;
	while ( moreData )
		{
		++index;
		tempStore.Format(KScanCodeString(), index);
		moreData=GetStringFromConfig(aSection, tempStore, scanCodeStr);
		if ( moreData )
			{
			tempStore.Format(KScanCode(), index);
			if (!GetEnumFromConfig(aSection, tempStore, iEnumTableScanCodes, scanCode))
				{
				ERR_PRINTF2(_L("Invalid scancode value: %d"), scanCode);
				SetBlockResult(EFail);
				}
			else
				{
				iPossibleScanCodeStrStore.AppendL(scanCodeStr);
				iPossibleScanCodeStore.AppendL(scanCode);

				iAllPossibleKeysStrStore.AppendL(scanCodeStr);
				iAllPossibleKeysStore.AppendL(scanCode);
				}
			}
		}
	}

void CT_KeypadDriverData::DoCmdGetModifiersL(const TDesC& aSection)
/**
 * Get the modifiers of the platform
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
	{
	iPossibleModifierStrStore.Reset();
	iPossibleModifierStore.Reset();

	TIniDataName	tempStore;
	TPtrC			modifierStr;
	TInt			modifier;

	TInt	index=0;
	TBool	moreData=ETrue;
	while ( moreData )
		{
		++index;
		tempStore.Format(KModifierString(), index);
		moreData=GetStringFromConfig(aSection, tempStore, modifierStr);
		if ( moreData )
			{
			tempStore.Format(KModifier(), index);
			if (!GetEnumFromConfig(aSection, tempStore, iEnumTableScanCodes, modifier))
				{
				ERR_PRINTF2(_L("Invalid modifier value: %d"), modifier);
				SetBlockResult(EFail);
				}
			else
				{
				iPossibleModifierStrStore.AppendL(modifierStr);
				iPossibleModifierStore.AppendL(modifier);

				iAllPossibleKeysStrStore.AppendL(modifierStr);
				iAllPossibleKeysStore.AppendL(modifier);
				}
			}
		}
	}

void CT_KeypadDriverData::DoCmdTestAllKeysL(const TInt aAsyncErrorIndex)
/**
 * Tests one key at the time by verifying the actual scancode against expected value
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
	{
	iExpectedStore.Reset();
	iActualStore.Reset();
	iConsole.ClearScreen();

	iKeyCount=0;
	for ( TInt key=0; key<iAllPossibleKeysStore.Count(); ++key )
		{
		for ( TInt event=0; event<iEventStore.Count(); ++event )
			{
			iExpectedStore.AppendL(iEventStore[event]);
			iExpectedStore.AppendL(iAllPossibleKeysStore[key]);
			}
		}

	//end of setup and start testcase
	iConsole.Write(_L("Keyboard layout test.\n"));
	iConsole.Write(_L("Press the key corresponding to the "));

	iConsole.Write(iAllPossibleKeysStrStore[iKeyCount]);

	iConsole.Read(iKey, iActiveKey->iStatus);
	iActiveKey->Activate( aAsyncErrorIndex );
	IncOutstanding(); 	// request an event
	}


void CT_KeypadDriverData::DoCmdTestOneModifierOneOtherKeyL(const TDesC& aSection, const TInt aAsyncErrorIndex)
/**
 * Tests one modifier + key combinations by verifying their scancodes against expected values
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
	{
	iCombinationStrStore.Reset();
	iExpectedStore.Reset();
	iActualStore.Reset();
	iConsole.ClearScreen();
	iCombinationCount  	 = 0;
	iCombinationKeyCount = 0;

	TInt	divider 	 = 1;
	TBool	dataOk=GetEnumFromConfig(aSection, KTestCoverage, iEnumTableCoverage, divider);
	if ( !dataOk )
		{
		ERR_PRINTF2(KLogMissingParameter, &KTestCoverage());
		SetBlockResult(EFail);
		}
	else
		{
		TInt	modifierCount 	= iPossibleModifierStore.Count();
		TInt	keyCount	  	= (TInt)(iPossibleScanCodeStore.Count() / divider);

		if (divider != 1)
			{
			++keyCount;
			}

		for ( TInt key=0; key<keyCount; ++key )
			{
			for ( TInt modifier=0; modifier<modifierCount; ++modifier )
				{
				for ( TInt event=0; event<iEventStore.Count(); ++event )
					{
					iExpectedStore.AppendL(iEventStore[event]);
					iExpectedStore.AppendL(iPossibleModifierStore[modifier]);
					iExpectedStore.AppendL(iEventStore[event]);
					iExpectedStore.AppendL(iPossibleScanCodeStore[key]);
					}

				TIniDataName	tempStrArray;
				tempStrArray.Append(iPossibleModifierStrStore[modifier]);
				tempStrArray.Append(_L(" + "));
				tempStrArray.Append(iPossibleScanCodeStrStore[key]);
				iCombinationStrStore.Append(tempStrArray);
				}
			}

		if ( iExpectedStore.Count()>0 )
			{//end of setup and start testcase
			iConsole.Write(_L("Keyboard layout test.\n"));
			iConsole.Write(_L("Press the following keys in the sequence as displayed:\n"));
			iConsole.Write(iCombinationStrStore[iCombinationCount]);
			iConsole.Write(_L("\n"));
			iConsole.Write(_L("and release them in the same sequence."));

			iConsole.Read(iKey, iActiveCombination->iStatus);
			iActiveCombination->Activate( aAsyncErrorIndex );
			IncOutstanding(); 	// request an event*/
			}
		else
			{
			INFO_PRINTF1(_L("No modifiers and/or keys available or too small test coverage set"));
			}
		}
	}

void CT_KeypadDriverData::DoCmdTestTwoKeysL(const TDesC& aSection, const TInt aAsyncErrorIndex)
/**
 * Tests key + key combinations by verifying their scancodes against expected values
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
 	{
	iCombinationStrStore.Reset();
	iExpectedStore.Reset();
	iActualStore.Reset();
	iConsole.ClearScreen();
	iCombinationCount		= 0;
	iCombinationKeyCount	= 0;

	TInt	divider			= 1;
	TBool	dataOk=GetEnumFromConfig(aSection, KTestCoverage, iEnumTableCoverage, divider);
	if ( !dataOk )
		{
		ERR_PRINTF2(KLogMissingParameter, &KTestCoverage());
		SetBlockResult(EFail);
		}
	else
		{
		TInt	keyCount=(TInt)(iPossibleScanCodeStore.Count() / divider);

		if (divider != 1)
			{
			++keyCount;
			}

		for ( TInt key1=0; key1<keyCount; ++key1 )
			{
			for ( TInt key2= 0; key2<keyCount; ++key2 )
				{
				if ( iPossibleScanCodeStore[key1]!=iPossibleScanCodeStore[key2] )
					{
					for ( TInt event=0; event<iEventStore.Count(); ++event )
						{
						iExpectedStore.AppendL(iEventStore[event]);
						iExpectedStore.AppendL(iPossibleScanCodeStore[key1]);
						iExpectedStore.AppendL(iEventStore[event]);
						iExpectedStore.AppendL(iPossibleScanCodeStore[key2]);
						}

					TIniDataName	tempStrArray;
					tempStrArray.Append(iPossibleScanCodeStrStore[key1]);
					tempStrArray.Append(_L(" + "));
					tempStrArray.Append(iPossibleScanCodeStrStore[key2]);
					iCombinationStrStore.Append(tempStrArray);
					}
				}
			}

		if ( iExpectedStore.Count()>0 )
			{//end of setup and start testcase
			iConsole.Write(_L("Keyboard layout test.\n"));
			iConsole.Write(_L("Press the following keys in the sequence as displayed:\n"));
			iConsole.Write(iCombinationStrStore[iCombinationCount]);
			iConsole.Write(_L("\n"));
			iConsole.Write(_L("and release them in the same sequence."));

			iConsole.Read(iKey, iActiveCombination->iStatus);
			iActiveCombination->Activate( aAsyncErrorIndex );
			IncOutstanding(); 	// request an event*/
			}
		else
			{
			INFO_PRINTF1(_L("No keys available or too small test coverage set"));
			}
		}
	}

void CT_KeypadDriverData::DoCmdTestMultipleKeysL(const TDesC& aSection, const TInt aAsyncErrorIndex)
/**
 * Tests multiple key combinations by verifying their scancodes against expected values
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
 	{
	iCombinationStrStore.Reset();
	iExpectedStore.Reset();
	iActualStore.Reset();
	iCombinationKeyCount = 0;
	iConsole.ClearScreen();

	RArray<TInt>	keyStore;
	RArray<TPtrC>	keyStrStore;

	keyStore.Reset();
	keyStrStore.Reset();

	TIniDataName	setStore;
	TIniDataName	strStore;
	TIniDataName	fullStore;
	TPtrC			scanCodeStr;
	TInt			scanCode;

	TInt	setIndex=0;
	TBool	moreSets=ETrue;
	while ( moreSets )
		{
		++setIndex;

		keyStore.Reset();
		keyStrStore.Reset();

		setStore.Format(KSet(), setIndex);
		setStore.Append(_L("_"));

		TInt	index	=0;
		TBool	moreData=ETrue;
		while ( moreData )
			{
			++index;

			fullStore 	= _L("");
			strStore.Format(KScanCodeString(), index);
			fullStore.Append(setStore);
			fullStore.Append(strStore);

			moreData=GetStringFromConfig(aSection, fullStore, scanCodeStr);
			if ( moreData )
				{
				fullStore = _L("");
				strStore.Format(KScanCode(), index);
				fullStore.Append(setStore);
				fullStore.Append(strStore);

				if (!GetEnumFromConfig(aSection, fullStore, iEnumTableScanCodes, scanCode))
					{
					ERR_PRINTF2(_L("Invalid scancode value: %d"), scanCode);
					SetBlockResult(EFail);
					}
				else
					{
					keyStrStore.AppendL(scanCodeStr);
					keyStore.AppendL(scanCode);
					}
				++index;
				}
			else
				{
				if (index == 1)
					{
					moreSets = EFalse;
					}
				}
			}

		if (moreSets)
			{
			TInt	keyCount=keyStore.Count();
			for ( TInt event=0; event<iEventStore.Count(); ++event )
				{
				for ( TInt key=0; key<keyCount; ++key )
					{
					iExpectedStore.AppendL(iEventStore[event]);
					iExpectedStore.AppendL(keyStore[key]);
					}
				}

			TIniDataName	tempStrArray;
			for ( TInt key=0; key<keyCount; ++key )
				{
				if (tempStrArray == _L(""))
					{
					tempStrArray.Append(keyStrStore[key]);
					}
				else
					{
					tempStrArray.Append(_L(" + "));
					tempStrArray.Append(keyStrStore[key]);
					}
				}

			iCombinationStrStore.Append(tempStrArray);
			}
		}

	keyStore.Reset();
	keyStore.Close();
	keyStrStore.Reset();
	keyStrStore.Close();

	if (iExpectedStore.Count() > 0)
		{//end of setup and start testcase
		iConsole.Write(_L("Keyboard layout test.\n"));
		iConsole.Write(_L("Press the following keys in the sequence as displayed:\n"));
		iConsole.Write(iCombinationStrStore[0]);
		iConsole.Write(_L("\n"));
		iConsole.Write(_L("and release them in the same sequence."));

		iConsole.Read(iKey, iActiveCombination->iStatus);
		iActiveCombination->Activate( aAsyncErrorIndex );
		IncOutstanding(); 	// request an event
		}
	else
		{
		INFO_PRINTF1(_L("No multiple key sets available"));
		}
	}


void CT_KeypadDriverData::DoCmdVerifyKeypadState(const TDesC& aSection, const TInt aAsyncErrorIndex)
/**
 * Function for verifying the keypad state manually if running manual tests
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
	{
	iExpectedState = 0;
	GetIntFromConfig(aSection, KExpectedState(), iExpectedState);

	TInt	timeOut=0;
	GetIntFromConfig(aSection, KTimeOut(), timeOut);

	TIniDataName	seconds;
	seconds.Format(_L("%d"), timeOut / 1000000);

	iConsole.ClearScreen();
	iConsole.Write(_L("Manual tests enabled...\n"));
	iConsole.Write(_L("Press any key within "));
	iConsole.Write(seconds);
	iConsole.Write(_L(" seconds to check the keypad state..."));

	iConsole.Read(iKey, iActiveKeypadState->iStatus);
	iActiveKeypadState->Activate( aAsyncErrorIndex, timeOut );
	IncOutstanding(); 	// request an event
	}


void CT_KeypadDriverData::DoCmdPromptUser(const TDesC& aSection, const TInt aAsyncErrorIndex)
/**
 * Function for verifying the volume manually if running manual tests
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
	{
	iFailTest = EFalse;
	GetBoolFromConfig(aSection, KFailTest(), iFailTest);

	if ( !GetStringFromConfig(aSection, KUserPrompt(), iUserPromptStr) )
		{
		ERR_PRINTF2(KLogMissingParameter, &KUserPrompt());
		SetBlockResult(EFail);
		}
	else
		{
		iConsole.ClearScreen();
		iConsole.Write(_L("Manual tests enabled...\n"));
		iConsole.Write(iUserPromptStr);

		if(iFailTest)
			{
			GetStringFromConfig(aSection, KPassKeyStr, iPassKeyStr);
			GetEnumFromConfig(aSection, KPassKey, iEnumTableScanCodes, iPassKey);

			iConsole.Write(_L("Press "));
			iConsole.Write(iPassKeyStr);
			iConsole.Write(_L(" to pass the test or any other key to fail->>>"));
			}
		else
			{
			iConsole.Write(_L("Press any key to continue->>>"));
			}

		iConsole.Read(iKey, iActivePrompt->iStatus);
		iActivePrompt->Activate( aAsyncErrorIndex );
		IncOutstanding(); 	// request an event
		}
	}


void CT_KeypadDriverData::DoCancel(CActive* aActive, TInt /*aIndex*/)
	{
	if( aActive == iActiveKeypadState )
		{
		INFO_PRINTF1(_L("Keypad state DoCancel called"));

		iConsole.ReadCancel();
		DecOutstanding();

		if(iExpectedState == 1)
			{
			ERR_PRINTF1(_L("No key press detected. Expected state is 1 (on)"));
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF2(_L("Key press result is corresponding to the expected state: %d"), iExpectedState);
			}
		}
	else
		{
		ERR_PRINTF1(_L("An unchecked active object cancelled"));
		SetBlockResult(EFail);
		}
	}

void CT_KeypadDriverData::RunL(CActive* aActive, TInt aIndex)
	{
	DecOutstanding();
	TBool	straySignal		= EFalse;
	TBool	furtherRequest	= EFalse;

	TBool	isCombination	= EFalse;
	TBool	isPrompt		= EFalse;

	if( aActive == iActiveKey )
		{
		INFO_PRINTF1(_L("RunL called"));

		isCombination			= EFalse;
		isPrompt				= EFalse;
		TInt	eventType		= iKey.Type();
		TInt	scanCode		= iKey.Code();

		if( eventType == TRawEvent::EKeyDown || eventType == TRawEvent::EKeyUp)
			{
			INFO_PRINTF2(_L("Raw Event: %d"), eventType);

			iActualStore.AppendL(eventType);
			iActualStore.AppendL(scanCode);

			INFO_PRINTF2(_L("Expected scancode string: %S"), &iAllPossibleKeysStrStore[iKeyCount]);
			INFO_PRINTF2(_L("Actual scancode: %d"), scanCode);

			if (eventType == TRawEvent::EKeyUp)
				{
				if(iKeyCount < iAllPossibleKeysStore.Count() - 1)
					{
					furtherRequest = ETrue;
					iKeyCount++;
					}
				else
					{
					furtherRequest = EFalse;
					}
				}
			else
				{
				furtherRequest = ETrue;
				}
			}
		else
			{
			ERR_PRINTF2(_L("Unexpected Raw Event: %d"), eventType);
			SetBlockResult(EFail);
			furtherRequest = ETrue;
			}
		}
	else if( aActive == iActiveCombination )
		{
		INFO_PRINTF1(_L("RunL called"));

		isCombination				= ETrue;
		isPrompt					= EFalse;
		TInt 	eventType			= iKey.Type();
		TInt	scanCode			= iKey.Code();

		if( eventType == TRawEvent::EKeyDown )
			{
			INFO_PRINTF2(_L("Raw Event: %d"), eventType);

			iActualStore.AppendL(eventType);
			iActualStore.AppendL(scanCode);

			INFO_PRINTF2(_L("Expected scancode string: %S"), &iCombinationStrStore[iCombinationCount]);
			INFO_PRINTF2(_L("Actual scancode: %d"), scanCode);

			iCombinationKeyCount++;

			furtherRequest = ETrue;
			}
		else if ( eventType == TRawEvent::EKeyUp )
			{
			INFO_PRINTF2(_L("Raw Event: %d"), eventType);

			iActualStore.AppendL(eventType);
			iActualStore.AppendL(scanCode);

			INFO_PRINTF2(_L("Actual scancode: %d"), scanCode);

			iCombinationKeyCount--;

			if(iCombinationCount < iCombinationStrStore.Count() - 1 && iCombinationKeyCount == 0)
				{
				furtherRequest = ETrue;
				iCombinationCount++;
				}
			else if (iCombinationKeyCount != 0)
				{
				furtherRequest = ETrue;
				}
			else
				{
				furtherRequest = EFalse;
				}
			}
		else
			{
			ERR_PRINTF2(_L("Unexpected Raw Event: %d"), eventType);
			SetBlockResult(EFail);
			furtherRequest = ETrue;
			}
		}
	else if( aActive == iActiveKeypadState )
		{
		INFO_PRINTF1(_L("RunL called"));

		TInt eventType = iKey.Type();
		TInt scanCode  = iKey.Code();

		if(eventType == TRawEvent::EKeyDown && scanCode != 0)
			{
			iActiveKeypadState->KillTimer();
			DecOutstanding();

			if(iExpectedState == 0)
				{
				ERR_PRINTF1(_L("Key press detected. Expected state is 0 (off)"));
				SetBlockResult(EFail);
				}
			else
				{
				INFO_PRINTF2(_L("Key press result is corresponding to the expected state: %d"), iExpectedState);
				}
			}
		}
	else if ( aActive == iActivePrompt )
		{
		INFO_PRINTF1(_L("Prompt RunL called"));

		isCombination		= EFalse;
		isPrompt			= ETrue;
		TInt	eventType	= iKey.Type();
		TInt	scanCode	= iKey.Code();

		if (iFailTest)
			{
			if(eventType == TRawEvent::EKeyDown && scanCode != iPassKey)
				{
				ERR_PRINTF1(_L("Test failed - User verifies that the result is not as expected"));
				SetBlockResult(EFail);
				furtherRequest = EFalse;
				}
			else if (eventType == TRawEvent::EKeyDown && scanCode == iPassKey)
				{
				INFO_PRINTF1(_L("Test passed - User verifies that the result is as expected"));
				furtherRequest = EFalse;
				}
			else if (eventType == TRawEvent::EKeyUp)
				{
				furtherRequest = ETrue;
				}
			else
				{
				ERR_PRINTF2(_L("Unexpected Raw Event: %d"), eventType);
				SetBlockResult(EFail);
				furtherRequest = ETrue;
				}
			}
		else
			{
			if(eventType == TRawEvent::EKeyDown)
				{
				INFO_PRINTF1(_L("Continuing..."));
				furtherRequest = EFalse;
				}
			else if (eventType == TRawEvent::EKeyUp)
				{
				furtherRequest = ETrue;
				}
			else
				{
				ERR_PRINTF2(_L("Unexpected Raw Event: %d"), eventType);
				SetBlockResult(EFail);
				furtherRequest = ETrue;
				}
			}
		}
	else
		{
		ERR_PRINTF1(_L("An unchecked active object completed"));
		SetBlockResult(EFail);
		straySignal = ETrue;
		}

	if( !straySignal )
		{
		TInt	err = aActive->iStatus.Int();
		if( err != KErrNone )
			{
			if(aActive == iActiveKeypadState && err == KErrCancel)
				{
				if(iExpectedState == 1)
					{
					ERR_PRINTF1(_L("No key press detected. Expected state is 1 (on)"));
					SetBlockResult(EFail);
					}
				else
					{
					INFO_PRINTF2(_L("Key press result is corresponding to the expected state: %d"), iExpectedState);
					}
				}
			else
				{
				ERR_PRINTF2(_L("RunL Error %d"), err);
				SetAsyncError( aIndex, err );
				}
			}
		else
			{
			if(furtherRequest)// check if re-issue required
				{
				iConsole.ClearScreen();
				INFO_PRINTF1(_L("Re-issuing Read"));

				if(!isCombination && !isPrompt)
					{
					iConsole.Write(_L("Press the key corresponding to the "));
					iConsole.Write(iAllPossibleKeysStrStore[iKeyCount]);

					iConsole.Read(iKey, iActiveKey->iStatus);
					iActiveKey->Activate(aIndex);
					IncOutstanding();
					}
				else if (!isPrompt)
					{
					iConsole.Write(_L("Press the following keys in the same sequence as displayed:\n"));
					iConsole.Write(iCombinationStrStore[iCombinationCount]);
					iConsole.Write(_L("\n"));
					iConsole.Write(_L("and release them in the same sequence."));

					iConsole.Read(iKey, iActiveCombination->iStatus);
					iActiveCombination->Activate(aIndex);
					IncOutstanding();
					}
				else{
					iConsole.Write(iUserPromptStr);

					if(iFailTest)
						{
						iConsole.Write(_L("Press "));
						iConsole.Write(iPassKeyStr);
						iConsole.Write(_L(" to pass the test or any other key to fail->>>"));
						}
					else
						{
						iConsole.Write(_L("Press any key to continue->>>"));
						}

					iConsole.Read(iKey, iActivePrompt->iStatus);
					iActivePrompt->Activate( aIndex );
					IncOutstanding();
					}
				}
			else
				{
				if(aActive == iActiveKeypadState)
					{
					if ( iExpectedState == 0 )
						{
						ERR_PRINTF1(_L("Expected state is 0 (off). Verifying keypad state should have been cancelled."));
						SetBlockResult(EFail);
						}
					else
						{
						INFO_PRINTF2(_L("Key press result is corresponding to the expected state: %d"), iExpectedState);
						}
					}
				else
					{
					INFO_PRINTF1(_L("RunL call completed successfully"));
					ProcessResults();	//process results
					}
				}
			}
		}
	}

void CT_KeypadDriverData::ProcessResults()
	{
	INFO_PRINTF1(_L("Processing results"));

	if (iActualStore.Count() == iExpectedStore.Count())
		{
		for(TInt i = 0; i < iActualStore.Count(); i+=2)
			{
			if(	iActualStore[i] != iExpectedStore[i]	)//compare event
				{
				ERR_PRINTF3(_L("Expected event (%d) does not match Actual event (%d)! "), iExpectedStore[i], iActualStore[i]);
				SetBlockResult(EFail);
				}
			else
				{
				INFO_PRINTF2(_L("Expected event was: %d, Actual event matches the expected one."), iExpectedStore[i]);
				}

			if(iActualStore[i+1] == iExpectedStore[i+1])
				{
				INFO_PRINTF2(_L("Expected scancode was: %d. Actual scancode matches the expected one."), iExpectedStore[i+1]);
				}
			else
				{
				ERR_PRINTF3(_L("Actual scancode (%d) != expected scancode (%d)"), iActualStore[i+1], iExpectedStore[i+1]);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF3(_L("Actual results array count (%d) != expected results array count (%d)"), iActualStore.Count(), iExpectedStore.Count());
		SetBlockResult(EFail);
		}
	}
