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
// wins\specific\keyboard.cpp
// 
//

#include "gui.h"

class KeySet
	{
public:
	struct SEntry
		{
		const char* iName;
		TInt iVal;
		};
public:
	inline KeySet(const SEntry aMap[], TInt aSize)
		:iMap(aMap), iSize(aSize)
		{}
	TInt operator[](const TDesC8& aName) const;
private:
	const SEntry* iMap;
	TInt iSize;
	};


const TUint8 StandardKeyMap[KStandardKeyMapSize]=
	{
	EStdKeyNull,
	EStdKeyEscape,
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	EStdKeyMinus,
	EStdKeyEquals,
	EStdKeyBackspace,
	EStdKeyTab,
	'Q',
	'W',
	'E',
	'R',
	'T',
	'Y',
	'U',
	'I',
	'O',
	'P',
	EStdKeySquareBracketLeft,
	EStdKeySquareBracketRight,
	EStdKeyEnter,
	EStdKeyLeftCtrl,
	'A',
	'S',
	'D',
	'F',
	'G',
	'H',
	'J',
	'K',
	'L',
	EStdKeySemiColon,
	EStdKeySingleQuote,
	EStdKeyXXX,
	EStdKeyLeftShift,
	EStdKeyHash,
	'Z',
	'X',
	'C',
	'V',
	'B',
	'N',
	'M',
	EStdKeyComma,
	EStdKeyFullStop,
	EStdKeyForwardSlash,
	EStdKeyRightShift,
	EStdKeyNkpAsterisk,
	EStdKeyLeftFunc,        // Alt key used as Func Key
	EStdKeySpace,
	EStdKeyCapsLock,
	EStdKeyMenu,
	EStdKeyHelp,
	EStdKeyDial,
	EStdKeyF4,
	EStdKeyF5,
    EStdKeyF6,
    EStdKeyF7,
    EStdKeyF8,
	EStdKeyOff,
	EStdKeyF10,
	EStdKeyPause,
	EStdKeyScrollLock,
	EStdKeyNkp7,
	EStdKeyNkp8,
	EStdKeyNkp9,
	EStdKeyNkpMinus,
	EStdKeyNkp4,
	EStdKeyNkp5,
	EStdKeyNkp6,
	EStdKeyNkpPlus,
	EStdKeyNkp1,
	EStdKeyNkp2,
	EStdKeyNkp3,
	EStdKeyNkp0,
	EStdKeyNkpFullStop,
	EStdKeyNull,
	EStdKeyNull,
	EStdKeyBackSlash,
	EStdKeyF11,
	EStdKeyF12
	};

const TUint8 ExtendedKeyMap[]=
	{
//	EStdKeyNumLock,
//	EStdKeyNull,
//	EStdKeyHome,
//	EStdKeyUpArrow,
//	EStdKeyPageUp,
//	EStdKeyNull,
//	EStdKeyLeftArrow,
//	EStdKeyNull,
//	EStdKeyRightArrow,
//	EStdKeyNull,
//	EStdKeyEnd,
//	EStdKeyDownArrow,
//	EStdKeyPageDown,
//	EStdKeyInsert,
//	EStdKeyDelete
	/*1c*/	EStdKeyNkpEnter,
	/*1d*/	EStdKeyRightCtrl,
	/*1e*/	EStdKeyNull,
	/*1f*/	EStdKeyNull,
	/*20*/	EStdKeyNull,
	/*21*/	EStdKeyNull,
	/*22*/	EStdKeyNull,
	/*23*/	EStdKeyNull,
	/*24*/	EStdKeyNull,
	/*25*/	EStdKeyNull,
	/*26*/	EStdKeyNull,
	/*27*/	EStdKeyNull,
	/*28*/	EStdKeyNull,
	/*29*/	EStdKeyNull,
	/*2a*/	EStdKeyNull,
	/*2b*/	EStdKeyNull,
	/*2c*/	EStdKeyNull,
	/*2d*/	EStdKeyNull,
	/*2e*/	EStdKeyNull,
	/*2f*/	EStdKeyNull,
	/*30*/	EStdKeyNull,
	/*31*/	EStdKeyNull,
	/*32*/	EStdKeyNull,
	/*33*/	EStdKeyNull,
	/*34*/	EStdKeyNull,
	/*35*/	EStdKeyNkpForwardSlash,
	/*36*/	EStdKeyNull,
	/*37*/	EStdKeyPrintScreen, // Actually e0 2a e0 37
	/*38*/	EStdKeyRightFunc,
	/*39*/	EStdKeyNull,
	/*3a*/	EStdKeyNull,
	/*3b*/	EStdKeyNull,
	/*3c*/	EStdKeyNull,
	/*3d*/	EStdKeyNull,
	/*3e*/	EStdKeyNull,
	/*3f*/	EStdKeyNull,
	/*40*/	EStdKeyNull,
	/*41*/	EStdKeyNull,
	/*42*/	EStdKeyNull,
	/*43*/	EStdKeyNull,
	/*44*/	EStdKeyNull,
	/*45*/	EStdKeyNumLock,
	/*46*/	EStdKeyNull,
	/*47*/	EStdKeyHome,
	/*48*/	EStdKeyUpArrow,
	/*49*/	EStdKeyPageUp,
	/*4a*/	EStdKeyNull,
	/*4b*/	EStdKeyLeftArrow,
	/*4c*/	EStdKeyNull,
	/*4d*/	EStdKeyRightArrow,
	/*4e*/	EStdKeyNull,
	/*4f*/	EStdKeyEnd,
	/*50*/	EStdKeyDownArrow,
	/*51*/	EStdKeyPageDown,
	/*52*/	EStdKeyInsert,
	/*53*/	EStdKeyDelete,
	/*54*/	EStdKeyNull,
	/*55*/	EStdKeyNull,
	/*56*/	EStdKeyNull,
	/*57*/	EStdKeyNull,
	/*58*/	EStdKeyNull,
	/*59*/	EStdKeyNull,
	/*5a*/	EStdKeyNull,
	/*5b*/	EStdKeyLeftAlt,	//left windows key
	/*5c*/	EStdKeyMenu,	//right windows key
	/*5d*/	EStdKeyHelp		//right mouse button key
	};


const KeySet::SEntry EPOCKeyNameToKeyMap[] =
	{
	{"EStdKeyNull",EStdKeyNull},
	{"EStdKeyBackspace",EStdKeyBackspace},
	{"EStdKeyTab",EStdKeyTab},
	{"EStdKeyEnter",EStdKeyEnter},
	{"EStdKeyEscape",EStdKeyEscape},
	{"EStdKeySpace",EStdKeySpace},
	{"EStdKeyPrintScreen",EStdKeyPrintScreen},
	{"EStdKeyPause",EStdKeyPause},
	{"EStdKeyHome",EStdKeyHome},
	{"EStdKeyEnd",EStdKeyEnd},
	{"EStdKeyPageUp",EStdKeyPageUp},
	{"EStdKeyPageDown",EStdKeyPageDown},
	{"EStdKeyInsert",EStdKeyInsert},
	{"EStdKeyDelete",EStdKeyDelete},
	{"EStdKeyLeftArrow",EStdKeyLeftArrow},
	{"EStdKeyRightArrow",EStdKeyRightArrow},
	{"EStdKeyUpArrow",EStdKeyUpArrow},
	{"EStdKeyDownArrow",EStdKeyDownArrow},
	{"EStdKeyLeftShift",EStdKeyLeftShift},
	{"EStdKeyRightShift",EStdKeyRightShift},
	{"EStdKeyLeftAlt",EStdKeyLeftAlt},
	{"EStdKeyRightAlt",EStdKeyRightAlt},
	{"EStdKeyLeftCtrl",EStdKeyLeftCtrl},
	{"EStdKeyRightCtrl",EStdKeyRightCtrl},
	{"EStdKeyLeftFunc",EStdKeyLeftFunc},
	{"EStdKeyRightFunc",EStdKeyRightFunc},
	{"EStdKeyCapsLock",EStdKeyCapsLock},
	{"EStdKeyNumLock",EStdKeyNumLock},
	{"EStdKeyScrollLock",EStdKeyScrollLock},
	{"EStdKeyF1",EStdKeyF1},
	{"EStdKeyF2",EStdKeyF2},
	{"EStdKeyF3",EStdKeyF3},
	{"EStdKeyF4",EStdKeyF4},
	{"EStdKeyF5",EStdKeyF5},
	{"EStdKeyF6",EStdKeyF6},
	{"EStdKeyF7",EStdKeyF7},
	{"EStdKeyF8",EStdKeyF8},
	{"EStdKeyF9",EStdKeyF9},
	{"EStdKeyF10",EStdKeyF10},
	{"EStdKeyF11",EStdKeyF11},
	{"EStdKeyF12",EStdKeyF12},
	{"EStdKeyF13",EStdKeyF13},
	{"EStdKeyF14",EStdKeyF14},
	{"EStdKeyF15",EStdKeyF15},
	{"EStdKeyF16",EStdKeyF16},
	{"EStdKeyF17",EStdKeyF17},
	{"EStdKeyF18",EStdKeyF18},
	{"EStdKeyF19",EStdKeyF19},
	{"EStdKeyF20",EStdKeyF20},
	{"EStdKeyF21",EStdKeyF21},
	{"EStdKeyF22",EStdKeyF22},
	{"EStdKeyF23",EStdKeyF23},
	{"EStdKeyF24",EStdKeyF24},
	{"EStdKeyXXX",EStdKeyXXX},
	{"EStdKeyComma",EStdKeyComma},
	{"EStdKeyFullStop",EStdKeyFullStop},
	{"EStdKeyForwardSlash",EStdKeyForwardSlash},
	{"EStdKeyBackSlash",EStdKeyBackSlash},
	{"EStdKeySemiColon",EStdKeySemiColon},
	{"EStdKeySingleQuote",EStdKeySingleQuote},
	{"EStdKeyHash",EStdKeyHash},
	{"EStdKeySquareBracketLeft",EStdKeySquareBracketLeft},
	{"EStdKeySquareBracketRight",EStdKeySquareBracketRight},
	{"EStdKeyMinus",EStdKeyMinus},
	{"EStdKeyEquals",EStdKeyEquals},
	{"EStdKeyNkpForwardSlash",EStdKeyNkpForwardSlash},
	{"EStdKeyNkpAsterisk",EStdKeyNkpAsterisk},
	{"EStdKeyNkpMinus",EStdKeyNkpMinus},
	{"EStdKeyNkpPlus",EStdKeyNkpPlus},
	{"EStdKeyNkpEnter",EStdKeyNkpEnter},
	{"EStdKeyNkp1",EStdKeyNkp1},
	{"EStdKeyNkp2",EStdKeyNkp2},
	{"EStdKeyNkp3",EStdKeyNkp3},
	{"EStdKeyNkp4",EStdKeyNkp4},
	{"EStdKeyNkp5",EStdKeyNkp5},
	{"EStdKeyNkp6",EStdKeyNkp6},
	{"EStdKeyNkp7",EStdKeyNkp7},
	{"EStdKeyNkp8",EStdKeyNkp8},
	{"EStdKeyNkp9",EStdKeyNkp9},
	{"EStdKeyNkp0",EStdKeyNkp0},
	{"EStdKeyNkpFullStop",EStdKeyNkpFullStop},
	{"EStdKeyMenu",EStdKeyMenu},
	{"EStdKeyBacklightOn",EStdKeyBacklightOn},
	{"EStdKeyBacklightOff",EStdKeyBacklightOff},
	{"EStdKeyBacklightToggle",EStdKeyBacklightToggle},
	{"EStdKeyIncContrast",EStdKeyIncContrast},
	{"EStdKeyDecContrast",EStdKeyDecContrast},
	{"EStdKeySliderDown",EStdKeySliderDown},
	{"EStdKeySliderUp",EStdKeySliderUp},
	{"EStdKeyDictaphonePlay",EStdKeyDictaphonePlay},
	{"EStdKeyDictaphoneStop",EStdKeyDictaphoneStop},
	{"EStdKeyDictaphoneRecord",EStdKeyDictaphoneRecord},
	{"EStdKeyHelp",EStdKeyHelp},
	{"EStdKeyOff",EStdKeyOff},
	{"EStdKeyDial",EStdKeyDial},
	{"EStdKeyIncVolume",EStdKeyIncVolume},
	{"EStdKeyDecVolume",EStdKeyDecVolume},
	{"EStdKeyDevice0",EStdKeyDevice0},
	{"EStdKeyDevice1",EStdKeyDevice1},
	{"EStdKeyDevice2",EStdKeyDevice2},
	{"EStdKeyDevice3",EStdKeyDevice3},
	{"EStdKeyDevice4",EStdKeyDevice4},
	{"EStdKeyDevice5",EStdKeyDevice5},
	{"EStdKeyDevice6",EStdKeyDevice6},
	{"EStdKeyDevice7",EStdKeyDevice7},
	{"EStdKeyDevice8",EStdKeyDevice8},
	{"EStdKeyDevice9",EStdKeyDevice9},
	{"EStdKeyDeviceA",EStdKeyDeviceA},
	{"EStdKeyDeviceB",EStdKeyDeviceB},
	{"EStdKeyDeviceC",EStdKeyDeviceC},
	{"EStdKeyDeviceD",EStdKeyDeviceD},
	{"EStdKeyDeviceE",EStdKeyDeviceE},
	{"EStdKeyDeviceF",EStdKeyDeviceF},
	{"EStdKeyDevice10",EStdKeyDevice10},
	{"EStdKeyDevice11",EStdKeyDevice11},
	{"EStdKeyDevice12",EStdKeyDevice12},
	{"EStdKeyDevice13",EStdKeyDevice13},
	{"EStdKeyDevice14",EStdKeyDevice14},
	{"EStdKeyDevice15",EStdKeyDevice15},
	{"EStdKeyDevice16",EStdKeyDevice16},
	{"EStdKeyDevice17",EStdKeyDevice17},
	{"EStdKeyDevice18",EStdKeyDevice18},
	{"EStdKeyDevice19",EStdKeyDevice19},
	{"EStdKeyDevice1A",EStdKeyDevice1A},
	{"EStdKeyDevice1B",EStdKeyDevice1B},
	{"EStdKeyDevice1C",EStdKeyDevice1C},
	{"EStdKeyDevice1D",EStdKeyDevice1D},
	{"EStdKeyDevice1E",EStdKeyDevice1E},
	{"EStdKeyDevice1F",EStdKeyDevice1F},
	{"EStdKeyDevice20",EStdKeyDevice20},
	{"EStdKeyDevice21",EStdKeyDevice21},
	{"EStdKeyDevice22",EStdKeyDevice22},
	{"EStdKeyDevice23",EStdKeyDevice23},
	{"EStdKeyDevice24",EStdKeyDevice24},
	{"EStdKeyDevice25",EStdKeyDevice25},
	{"EStdKeyDevice26",EStdKeyDevice26},
	{"EStdKeyDevice27",EStdKeyDevice27},
	{"EStdKeyApplication0",EStdKeyApplication0},
	{"EStdKeyApplication1",EStdKeyApplication1},
	{"EStdKeyApplication2",EStdKeyApplication2},
	{"EStdKeyApplication3",EStdKeyApplication3},
	{"EStdKeyApplication4",EStdKeyApplication4},
	{"EStdKeyApplication5",EStdKeyApplication5},
	{"EStdKeyApplication6",EStdKeyApplication6},
	{"EStdKeyApplication7",EStdKeyApplication7},
	{"EStdKeyApplication8",EStdKeyApplication8},
	{"EStdKeyApplication9",EStdKeyApplication9},
	{"EStdKeyApplicationA",EStdKeyApplicationA},
	{"EStdKeyApplicationB",EStdKeyApplicationB},
	{"EStdKeyApplicationC",EStdKeyApplicationC},
	{"EStdKeyApplicationD",EStdKeyApplicationD},
	{"EStdKeyApplicationE",EStdKeyApplicationE},
	{"EStdKeyApplicationF",EStdKeyApplicationF},
	{"EStdKeyApplication10",EStdKeyApplication10},
	{"EStdKeyApplication11",EStdKeyApplication11},
	{"EStdKeyApplication12",EStdKeyApplication12},
	{"EStdKeyApplication13",EStdKeyApplication13},
	{"EStdKeyApplication14",EStdKeyApplication14},
	{"EStdKeyApplication15",EStdKeyApplication15},
	{"EStdKeyApplication16",EStdKeyApplication16},
	{"EStdKeyApplication17",EStdKeyApplication17},
	{"EStdKeyApplication18",EStdKeyApplication18},
	{"EStdKeyApplication19",EStdKeyApplication19},
	{"EStdKeyApplication1A",EStdKeyApplication1A},
	{"EStdKeyApplication1B",EStdKeyApplication1B},
	{"EStdKeyApplication1C",EStdKeyApplication1C},
	{"EStdKeyApplication1D",EStdKeyApplication1D},
	{"EStdKeyApplication1E",EStdKeyApplication1E},
	{"EStdKeyApplication1F",EStdKeyApplication1F},
	{"EStdKeyApplication20",EStdKeyApplication20},
	{"EStdKeyApplication21",EStdKeyApplication21},
	{"EStdKeyApplication22",EStdKeyApplication22},
	{"EStdKeyApplication23",EStdKeyApplication23},
	{"EStdKeyApplication24",EStdKeyApplication24},
	{"EStdKeyApplication25",EStdKeyApplication25},
	{"EStdKeyApplication26",EStdKeyApplication26},
	{"EStdKeyApplication27",EStdKeyApplication27},
	{"EStdKeyYes",EStdKeyYes},
	{"EStdKeyNo",EStdKeyNo},
	{"EStdKeyIncBrightness",EStdKeyIncBrightness},
	{"EStdKeyDecBrightness",EStdKeyDecBrightness},
	{"A", 'A'},
	{"B", 'B'},
	{"C", 'C'},
	{"D", 'D'},
	{"E", 'E'},
	{"F", 'F'},
	{"G", 'G'},
	{"H", 'H'},
	{"I", 'I'},
	{"J", 'J'},
	{"K", 'K'},
	{"L", 'L'},
	{"M", 'M'},
	{"N", 'N'},
	{"O", 'O'},
	{"P", 'P'},
	{"Q", 'Q'},
	{"R", 'R'},
	{"S", 'S'},
	{"T", 'T'},
	{"U", 'U'},
	{"V", 'V'},
	{"W", 'W'},
	{"X", 'X'},
	{"Y", 'Y'},
	{"Z", 'Z'},
	{"0", '0'},
	{"1", '1'},
	{"2", '2'},
	{"3", '3'},
	{"4", '4'},
	{"5", '5'},
	{"6", '6'},
	{"7", '7'},
	{"8", '8'},
	{"9", '9'},
	{"EKeyScreenDimension0",EKeyScreenDimension0<<16},
	{"EKeyScreenDimension1",EKeyScreenDimension1<<16},
	{"EKeyScreenDimension2",EKeyScreenDimension2<<16},
	{"EKeyScreenDimension3",EKeyScreenDimension3<<16}
	};

const KeySet::SEntry RealKeyNames[] = 
	{
	{"Escape",1},
	{"1",2},
	{"2",3},
	{"3",4},
	{"4",5},
	{"5",6},
	{"6",7},
	{"7",8},
	{"8",9},
	{"9",10},
	{"0",11},
	{"Minus",12},
	{"Equals",13},
	{"BackSpace",14},
	{"Tab",15},
	{"Q",16},
	{"W",17},
	{"E",18},
	{"R",19},
	{"T",20},
	{"Y",21},
	{"U",22},
	{"I",23},
	{"O",24},
	{"P",25},
	{"SquareBracketLeft",26},
	{"SquareBracketRight",27},
	{"Enter",28},
	{"LeftCtrl",29},
	{"A",30},
	{"S",31},
	{"D",32},
	{"F",33},
	{"G",34},
	{"H",35},
	{"J",36},
	{"K",37},
	{"L",38},
	{"SemiColon",39},
	{"SingleQuote",40},
	{"BackTick",41},
	{"LeftShift",42},
	{"Hash",43},
	{"Z",44},
	{"X",45},
	{"C",46},
	{"V",47},
	{"B",48},
	{"N",49},
	{"M",50},
	{"Comma",51},
	{"FullStop",52},
	{"ForwardSlash",53},
	{"RightShift",54},
	{"NkpAsterisk",55},
	{"LeftAlt",56},        // Alt key used as Func Key
	{"Space",57},
	{"CapsLock",58},
	{"F1",59},
	{"F2",60},
	{"F3",61},
	{"F4",62},
	{"F5",63},
    {"F6",64},
    {"F7",65},
    {"F8",66},
	{"F9",67},
	{"F10",68},
	{"Pause",69},
	{"ScrollLock",70},
	{"Nkp7",71},
	{"Nkp8",72},
	{"Nkp9",73},
	{"NkpMinus",74},
	{"Nkp4",75},
	{"Nkp5",76},
	{"Nkp6",77},
	{"NkpPlus",78},
	{"Nkp1",79},
	{"Nkp2",80},
	{"Nkp3",81},
	{"Nkp0",82},
	{"NkpFullStop",83},
	{"BackSlash",86},
	{"F11",87},
	{"F12",88}
	};

const KeySet::SEntry RealExtendedKeyNames[] = 
	{
	{"NkpEnter",0},
	{"RightCtrl",1},
	{"NkpForwardSlash", 25},
	{"PrintScreen",27},
	{"RightAlt", 28},
	{"NumLock", 41},
	{"Home",43},
	{"UpArrow",44},
	{"PageUp",45},
	{"LeftArrow",47},
	{"RightArrow",49},
	{"End",51},
	{"DownArrow",52},
	{"PageDown",53},
	{"Insert",54},
	{"Delete",55},
	{"LeftWindows",63},
	{"RightWindows",64},
	{"ContextMenu",65}
	};

const KeySet EpocKeys(EPOCKeyNameToKeyMap, sizeof(EPOCKeyNameToKeyMap)/sizeof(KeySet::SEntry));
const KeySet StandardKeys(RealKeyNames, sizeof(RealKeyNames)/sizeof(KeySet::SEntry));
const KeySet ExtendedKeys(RealExtendedKeyNames, sizeof(RealExtendedKeyNames)/sizeof(KeySet::SEntry));

TInt KeySet::operator[](const TDesC8& aName) const
	{
	TInt len = aName.Length();
	const char* p = (const char*)aName.Ptr();
	for (const SEntry* e = iMap + iSize; --e >= iMap; )
		{
		if (_strnicmp(e->iName, p, len) == 0 && e->iName[len] == '\0')
			return e->iVal;
		}
	return KErrNotFound;
	}

// Class Alias

Alias::Alias()
	:iMap(10)
	{}

TInt Alias::Add(const TDesC8& aAlias, const TDesC8& aName)
//
// add a new alias
//
	{
	TInt r = KErrNoMemory;
	SEntry e;
	e.iAlias = HBuf8::New(aAlias);
	if (e.iAlias)
		{
		e.iName = HBuf8::New(aName);
		if (e.iName)
			{
			r = iMap.InsertInOrder(e, &Alias::Compare);
			if (r == KErrNone)
				return KErrNone;
			delete e.iName;
			}
		delete e.iAlias;
		}
	return r;
	}

const TDesC8* Alias::operator[](const TDesC8& aAlias)
//
// Look up an alias
//
	{
	SEntry e;
	e.iAlias = const_cast<TDesC8*>(&aAlias);
	TInt ix = iMap.FindInOrder(e, &Alias::Compare);
	return ix >= 0 ? iMap[ix].iName : NULL;
	}

TInt Alias::Compare(const SEntry& aLhs, const SEntry& aRhs)
	{
	return CompareI(*aLhs.iAlias, *aRhs.iAlias);
	}



// keyboard mapping

DWinsKeyboard::DWinsKeyboard()
	{
	memcpy(iStandardKeyMap, StandardKeyMap, KStandardKeyMapSize);
	memcpy(iExtendedKeyMap, ExtendedKeyMap, KExtendedKeyMapSize);
	memcpy(iAltStandardKeyMap, StandardKeyMap, KStandardKeyMapSize);
	memcpy(iAltExtendedKeyMap, ExtendedKeyMap, KExtendedKeyMapSize);
	memcpy(iCtrlStandardKeyMap, StandardKeyMap, KStandardKeyMapSize);
	memcpy(iCtrlExtendedKeyMap, ExtendedKeyMap, KExtendedKeyMapSize);
	}

TInt DWinsKeyboard::Init(TInt aId)
//
// Initialise the alias and key maps from the properties
//
	{
	char property[50];
	wsprintfA(property, "Configuration[%d]DefineKeyName",aId);
	TInt r = MultiProperty(&DWinsKeyboard::DoDefineAlias, this, property);
	if (r != KErrNone)
		return r;
	wsprintfA(property, "Configuration[%d]KeyMap",aId);
	r = MultiProperty(&DWinsKeyboard::DoMapKey, this, property);
	return r;
	}

TInt DWinsKeyboard::DoDefineAlias(TAny* aPtr, const char* aValue)
	{
	return static_cast<DWinsKeyboard*>(aPtr)->DefineAlias(aValue);
	}

TInt DWinsKeyboard::DefineAlias(const char* aValue)
//
// The character string terminates in a ';' or a '\0'
//
	{
	//get the alias name
	const char* beg = skipws(aValue);
	const char* end = skiptok(beg);
	TPtrC8 alias((const TUint8*)beg, end-beg);

	//get the real name
	beg = skipws(end);
	end = skiptok(beg);
	TPtrC8 name((const TUint8*)beg, end-beg);

	//prevent an alias being made of an alias
	if (iAliasedKeys[name] != NULL)
		return KErrArgument;

	// ensure this is valid name
	TInt r = GetEPOCKeyCode(name);
	if (r == KErrNotFound)
		return r;

	//now we need to record the alias name and the real key name
	return iAliasedKeys.Add(alias, name);
	}


TInt DWinsKeyboard::DoMapKey(TAny* aPtr, const char* aValue)
	{
	return static_cast<DWinsKeyboard*>(aPtr)->MapKey(aValue);
	}

TInt DWinsKeyboard::MapKey(const char* aValue)
	{
	//get the win32 name
	const char* beg = skipws(aValue);
	const char* end = skiptok(beg);

	// check to see if a modifier key has been specified
	TBool altModifier = !_strnicmp(beg, "LeftAlt", 7);
	altModifier |= !_strnicmp(beg, "RightAlt", 8);
	TBool ctrlModifier = !_strnicmp(beg, "LeftCtrl", 8);
	ctrlModifier |= !_strnicmp(beg, "RightCtrl", 9);

	// if modifier was found, skip to next token
	if (altModifier || ctrlModifier)
		{
		beg = skipws(end);
		end = skiptok(beg);
		}

	TPtrC8 name((const TUint8*)beg, end-beg);

	//get the epoc key
	beg = skipws(end);
	end = skiptok(beg);
	TInt key = GetEPOCKeyCode(TPtrC8((const TUint8*)beg, end-beg));
	if (key == KErrNotFound)
		return key;

	TInt scancode = StandardKeys[name];

	// if alt modifier was specified, change the alt key map
	if (altModifier)
		{
		if (scancode >= 0)
			iAltStandardKeyMap[scancode] = (TUint8)key;
		else
			{
			scancode = ExtendedKeys[name];
			if (scancode >= 0)
				iAltExtendedKeyMap[scancode] = (TUint8)key;
			else
				return KErrNotFound;
			}
		}

	// if ctrl modifier was specified, change the ctrl key map
	else if (ctrlModifier)
		{
		if (scancode >= 0)
			iCtrlStandardKeyMap[scancode] = (TUint8)key;
		else
			{
			scancode = ExtendedKeys[name];
			if (scancode >= 0)
				iCtrlExtendedKeyMap[scancode] = (TUint8)key;
			else
				return KErrNotFound;
			}
		}

	// no modifier, change the default key map
	else
		{
		if (scancode >= 0)
			iStandardKeyMap[scancode] = (TUint8)key;
		else
			{
			scancode = ExtendedKeys[name];
			if (scancode >= 0)
				iExtendedKeyMap[scancode] = (TUint8)key;
			else
				return KErrNotFound;
			}
		}

	return KErrNone;
	}

TInt DWinsKeyboard::GetEPOCKeyCode(const TDesC8& aStr)
	{
	// look for an alias
	const TDesC8* arg = iAliasedKeys[aStr];
	return EpocKeys[arg ? *arg : aStr];
	}

TInt DWinsKeyboard::ScanCodeToKey(TInt aScanCode, const TUint8* aStandardMap, const TUint8* aExtendedMap)
//
// Map Windows scan codes to Epoc key
//
	{
	TInt stdCode = EStdKeyNull;
	TInt extended = aScanCode&KKeyExtendedBit;
	aScanCode &= KKeyNormalBits;
	if (extended)
		{
		if (aScanCode >= KExtendedKeyBase && aScanCode <= KMaxExtendedKey)
			stdCode = aExtendedMap[aScanCode - KExtendedKeyBase];
		}
	else if (aScanCode < KStandardKeyMapSize)
		stdCode = aStandardMap[aScanCode];
	return stdCode;
	}

TInt DWinsKeyboard::ScanCodeToStandardKey(TInt aScanCode)
//
// Map Windows scan codes to standard keys
//
	{
	return ScanCodeToKey(aScanCode, StandardKeyMap, ExtendedKeyMap );
	}

TInt DWinsKeyboard::ScanCodeToRemappedKey(TInt aScanCode)
//
// Map Windows scan codes to remapped keys
//
	{
	if (HIBYTE(GetKeyState(VK_MENU)))      // Alt key is pressed
		return ScanCodeToKey(aScanCode, iAltStandardKeyMap, iAltExtendedKeyMap );

	if (HIBYTE(GetKeyState(VK_CONTROL)))   // Ctrl key is pressed
		return ScanCodeToKey(aScanCode, iCtrlStandardKeyMap, iCtrlExtendedKeyMap );

	return ScanCodeToKey(aScanCode, iStandardKeyMap, iExtendedKeyMap );
	}




TInt DWinsKeyboard::GetScanCode(const TDesC8& aStr)
	{
	return  StandardKeys[aStr];	
	}

