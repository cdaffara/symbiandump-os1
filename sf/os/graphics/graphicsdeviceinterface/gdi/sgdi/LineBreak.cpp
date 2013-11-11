// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file was last edited on 09/04/2007. Please read the 'Unicode Character Data and Line Break data Update History.doc' file
// for the latest updates.
// 
//

#include "LineBreak.h"
#include "LineBreakImp.h"
#include <e32std.h>
#include <e32svr.h>

_LIT(KLinebreakPanicCategory,"LineBreak");

GLDEF_C void Panic(TInt aError)
	{
	User::Panic(KLinebreakPanicCategory,aError);
	}

const TUint KOpFlag = 1 << MLineBreaker::EOpLineBreakClass;
const TUint KClFlag = 1 << MLineBreaker::EClLineBreakClass;
const TUint KNsFlag = 1 << MLineBreaker::ENsLineBreakClass;
const TUint KExFlag = 1 << MLineBreaker::EExLineBreakClass;
const TUint KSyFlag = 1 << MLineBreaker::ESyLineBreakClass;
const TUint KIsFlag = 1 << MLineBreaker::EIsLineBreakClass;
const TUint KPrFlag = 1 << MLineBreaker::EPrLineBreakClass;
const TUint KPoFlag = 1 << MLineBreaker::EPoLineBreakClass;
const TUint KNuFlag = 1 << MLineBreaker::ENuLineBreakClass;
const TUint KAlFlag = 1 << MLineBreaker::EAlLineBreakClass;
const TUint KIdFlag = 1 << MLineBreaker::EIdLineBreakClass;
const TUint KInFlag = 1 << MLineBreaker::EInLineBreakClass;
const TUint KBbFlag = 1 << MLineBreaker::EBbLineBreakClass;
const TUint KB2Flag = 1 << MLineBreaker::EB2LineBreakClass;
const TUint KZwFlag = 1 << MLineBreaker::EZwLineBreakClass;
const TUint KCmFlag = 1 << MLineBreaker::ECmLineBreakClass;
const TUint KAiFlag = 1 << MLineBreaker::EAiLineBreakClass;

const TUint KAiAlFlags = KAiFlag | KAlFlag;

const TUint KAllBreaks = 0xFFFFFFFF;
const TUint KUsualForbidden = KClFlag | KExFlag | KSyFlag | KIsFlag | KZwFlag;
const TUint KUsualAllowed = KOpFlag | KPrFlag | KPoFlag | KNuFlag | KAiAlFlags | KIdFlag | KInFlag | KBbFlag | KB2Flag;

const TUint KAlAllowed = KOpFlag | KPrFlag | KPoFlag | KIdFlag | KBbFlag | KB2Flag;

// A table of break rules, indexed by the class of the character before the possible break.
static const TLineBreakRule TheLineBreakRuleTable[MLineBreaker::ELineBreakClasses] =
	{
	{ KAllBreaks - KCmFlag,0 },														// Op
	{ KUsualForbidden | KNsFlag,KUsualAllowed - KPoFlag },							// Cl
	{ KUsualForbidden | KOpFlag,0 },												// Qu
	{ KUsualForbidden, 0 },															// Gl
	{ KUsualForbidden, KUsualAllowed },												// Ns
	{ KUsualForbidden, KUsualAllowed },												// Ex
	{ KUsualForbidden, KUsualAllowed - KNuFlag },									// Sy
	{ KUsualForbidden, KUsualAllowed - KNuFlag },									// Is
	{ KUsualForbidden, KPrFlag | KPoFlag | KInFlag | KBbFlag | KB2Flag },			// Pr
	{ KUsualForbidden, KUsualAllowed },												// Po
	{ KUsualForbidden, KOpFlag | KPrFlag | KIdFlag | KBbFlag | KB2Flag },			// Nu
	{ KUsualForbidden, KAlAllowed }, // Al
	{ KUsualForbidden, KUsualAllowed - KPoFlag - KInFlag },							// Id
	{ KUsualForbidden, KUsualAllowed - KInFlag },									// In
	{ KUsualForbidden, KUsualAllowed },												// Hy
	{ KUsualForbidden, KUsualAllowed },												// Ba
	{ KUsualForbidden, 0 },															// Bb
	{ KUsualForbidden | KB2Flag, KUsualAllowed },									// B2
	{ KZwFlag, KAllBreaks - KZwFlag - KCmFlag},										// Zw
	{ KUsualForbidden, KOpFlag | KPrFlag | KPoFlag | KIdFlag | KBbFlag | KB2Flag }, // Cm
	{ 0, 0, },																		// Bk
	{ 0, 0, }, // Cr
	{ 0, 0, }, // Lf
	{ 0, 0, },																		// Sg
	{ 0, 0, },																		// Cb
	{ KAllBreaks, 0, },																// Sp
	{ 0, 0, },																		// Sa
	{ KUsualForbidden, KAlAllowed, }, // Ai
	{ 0, 0, }																		// Xx
	};

/**
A sorted table of ranges of characters, with their line breaking classes.
A range includes all codes for iStart <= code < iEnd.
All these items have fixed range, please don't split any item into more or 
combine consecutive items even if they have the same property,
because these change will cause mismatch with customer definition.
*/
static const TLineBreakRange TheLineBreakRangeTable[] = 
	{
    { 0x0000, 0x0009, MLineBreaker::ECmLineBreakClass },  //<control>
    { 0x0009, 0x000a, MLineBreaker::EBaLineBreakClass },  //<control>
    { 0x000a, 0x000b, MLineBreaker::ELfLineBreakClass },  //<control>
    { 0x000b, 0x000c, MLineBreaker::ECmLineBreakClass },  //<control>
    { 0x000c, 0x000d, MLineBreaker::EBkLineBreakClass },  //<control>
    { 0x000d, 0x000e, MLineBreaker::ECrLineBreakClass },  //<control>
    { 0x000e, 0x0020, MLineBreaker::ECmLineBreakClass },  //<control>
    { 0x0020, 0x0021, MLineBreaker::ESpLineBreakClass },  //space
    { 0x0021, 0x0022, MLineBreaker::EExLineBreakClass },  //exclamation mark
    { 0x0022, 0x0023, MLineBreaker::EQuLineBreakClass },  //quotation mark
    { 0x0023, 0x0024, MLineBreaker::EAlLineBreakClass },  //number sign
    { 0x0024, 0x0025, MLineBreaker::EPrLineBreakClass },  //dollar sign
    { 0x0025, 0x0026, MLineBreaker::EPoLineBreakClass },  //percent sign
    { 0x0026, 0x0027, MLineBreaker::EAlLineBreakClass },  //ampersand
    { 0x0027, 0x0028, MLineBreaker::EQuLineBreakClass },  //apostrophe
    { 0x0028, 0x0029, MLineBreaker::EOpLineBreakClass },  //left parenthesis
    { 0x0029, 0x002a, MLineBreaker::EClLineBreakClass },  //right parenthesis
    { 0x002a, 0x002b, MLineBreaker::EAlLineBreakClass },  //asterisk
    { 0x002b, 0x002c, MLineBreaker::EPrLineBreakClass },  //plus sign
    { 0x002c, 0x002d, MLineBreaker::EIsLineBreakClass },  //comma
    { 0x002d, 0x002e, MLineBreaker::EHyLineBreakClass },  //hyphen-minus
    { 0x002e, 0x002f, MLineBreaker::EIsLineBreakClass },  //full stop
    { 0x002f, 0x0030, MLineBreaker::ESyLineBreakClass },  //solidus
    { 0x0030, 0x003a, MLineBreaker::ENuLineBreakClass },  //digit zero
    { 0x003a, 0x003c, MLineBreaker::EIsLineBreakClass },  //colon
    { 0x003c, 0x003f, MLineBreaker::EAlLineBreakClass },  //less-than sign
    { 0x003f, 0x0040, MLineBreaker::EExLineBreakClass },  //question mark
    { 0x0040, 0x005b, MLineBreaker::EAlLineBreakClass },  //commercial at
    { 0x005b, 0x005c, MLineBreaker::EOpLineBreakClass },  //left square bracket
    { 0x005c, 0x005d, MLineBreaker::EPrLineBreakClass },  //reverse solidus
    { 0x005d, 0x005e, MLineBreaker::EClLineBreakClass },  //right square bracket
    { 0x005e, 0x005f, MLineBreaker::EAlLineBreakClass },  //circumflex accent
    { 0x005f, 0x0060, MLineBreaker::EAlLineBreakClass },  //low line (underscore)
	{ 0x0060, 0x007b, MLineBreaker::EAlLineBreakClass },  //grave accent  
	{ 0x007b, 0x007c, MLineBreaker::EOpLineBreakClass },  //left curly bracket
    { 0x007c, 0x007d, MLineBreaker::EBaLineBreakClass },  //vertical line
    { 0x007d, 0x007e, MLineBreaker::EClLineBreakClass },  //right curly bracket
    { 0x007e, 0x007f, MLineBreaker::EAlLineBreakClass },  //tilde
    { 0x007f, 0x00a0, MLineBreaker::ECmLineBreakClass },  //<control>
    { 0x00a0, 0x00a1, MLineBreaker::EGlLineBreakClass },  //no-break space
    { 0x00a1, 0x00a2, MLineBreaker::EAiLineBreakClass },  //inverted exclamation mark
    { 0x00a2, 0x00a3, MLineBreaker::EPoLineBreakClass },  //cent sign
    { 0x00a3, 0x00a6, MLineBreaker::EPrLineBreakClass },  //pound sign
    { 0x00a6, 0x00a7, MLineBreaker::EAlLineBreakClass },  //broken bar
    { 0x00a7, 0x00a9, MLineBreaker::EAiLineBreakClass },  //section sign
    { 0x00a9, 0x00aa, MLineBreaker::EAlLineBreakClass },  //copyright sign
    { 0x00aa, 0x00ab, MLineBreaker::EAiLineBreakClass },  //feminine ordinal indicator
    { 0x00ab, 0x00ac, MLineBreaker::EQuLineBreakClass },  //left-pointing double angle quotation mark
    { 0x00ac, 0x00ad, MLineBreaker::EAlLineBreakClass },  //not sign
    { 0x00ad, 0x00ae, MLineBreaker::EBaLineBreakClass },  //soft hyphen
    { 0x00ae, 0x00b0, MLineBreaker::EAlLineBreakClass },  //registered sign
    { 0x00b0, 0x00b1, MLineBreaker::EPoLineBreakClass },  //degree sign
    { 0x00b1, 0x00b2, MLineBreaker::EPrLineBreakClass },  //plus-minus sign
    { 0x00b2, 0x00b4, MLineBreaker::EAiLineBreakClass },  //superscript two
    { 0x00b4, 0x00b5, MLineBreaker::EBaLineBreakClass },  //acute accent
    { 0x00b5, 0x00b6, MLineBreaker::EAlLineBreakClass },  //micro sign
    { 0x00b6, 0x00bb, MLineBreaker::EAiLineBreakClass },  //pilcrow sign
    { 0x00bb, 0x00bc, MLineBreaker::EQuLineBreakClass },  //right-pointing double angle quotation mark
    { 0x00bc, 0x00c0, MLineBreaker::EAiLineBreakClass },  //vulgar fraction one quarter
    { 0x00c0, 0x00c6, MLineBreaker::EAlLineBreakClass },  //latin capital letter a with grave
    { 0x00c6, 0x00c7, MLineBreaker::EAiLineBreakClass },  //latin capital letter ae
    { 0x00c7, 0x00d0, MLineBreaker::EAlLineBreakClass },  //latin capital letter c with cedilla
    { 0x00d0, 0x00d1, MLineBreaker::EAiLineBreakClass },  //latin capital letter eth
    { 0x00d1, 0x00d7, MLineBreaker::EAlLineBreakClass },  //latin capital letter n with tilde
    { 0x00d7, 0x00d9, MLineBreaker::EAiLineBreakClass },  //multiplication sign
    { 0x00d9, 0x00de, MLineBreaker::EAlLineBreakClass },  //latin capital letter u with grave
    { 0x00de, 0x00e2, MLineBreaker::EAiLineBreakClass },  //latin capital letter thorn
    { 0x00e2, 0x00e6, MLineBreaker::EAlLineBreakClass },  //latin small letter a with circumflex
    { 0x00e6, 0x00e7, MLineBreaker::EAiLineBreakClass },  //latin small letter ae
    { 0x00e7, 0x00e8, MLineBreaker::EAlLineBreakClass },  //latin small letter c with cedilla
    { 0x00e8, 0x00eb, MLineBreaker::EAiLineBreakClass },  //latin small letter e with grave
    { 0x00eb, 0x00ec, MLineBreaker::EAlLineBreakClass },  //latin small letter e with diaeresis
    { 0x00ec, 0x00ee, MLineBreaker::EAiLineBreakClass },  //latin small letter i with grave
    { 0x00ee, 0x00f0, MLineBreaker::EAlLineBreakClass },  //latin small letter i with circumflex
    { 0x00f0, 0x00f1, MLineBreaker::EAiLineBreakClass },  //latin small letter eth
    { 0x00f1, 0x00f2, MLineBreaker::EAlLineBreakClass },  //latin small letter n with tilde
    { 0x00f2, 0x00f4, MLineBreaker::EAiLineBreakClass },  //latin small letter o with grave
    { 0x00f4, 0x00f7, MLineBreaker::EAlLineBreakClass },  //latin small letter o with circumflex
    { 0x00f7, 0x00fb, MLineBreaker::EAiLineBreakClass },  //division sign
    { 0x00fb, 0x00fc, MLineBreaker::EAlLineBreakClass },  //latin small letter u with circumflex
    { 0x00fc, 0x00fd, MLineBreaker::EAiLineBreakClass },  //latin small letter u with diaeresis
    { 0x00fd, 0x00fe, MLineBreaker::EAlLineBreakClass },  //latin small letter y with acute
    { 0x00fe, 0x00ff, MLineBreaker::EAiLineBreakClass },  //latin small letter thorn
    { 0x00ff, 0x0101, MLineBreaker::EAlLineBreakClass },  //latin small letter y with diaeresis
    { 0x0101, 0x0102, MLineBreaker::EAiLineBreakClass },  //latin small letter a with macron
    { 0x0102, 0x0111, MLineBreaker::EAlLineBreakClass },  //latin capital letter a with breve
    { 0x0111, 0x0112, MLineBreaker::EAiLineBreakClass },  //latin small letter d with stroke
    { 0x0112, 0x0113, MLineBreaker::EAlLineBreakClass },  //latin capital letter e with macron
    { 0x0113, 0x0114, MLineBreaker::EAiLineBreakClass },  //latin small letter e with macron
    { 0x0114, 0x011b, MLineBreaker::EAlLineBreakClass },  //latin capital letter e with breve
    { 0x011b, 0x011c, MLineBreaker::EAiLineBreakClass },  //latin small letter e with caron
    { 0x011c, 0x0126, MLineBreaker::EAlLineBreakClass },  //latin capital letter g with circumflex
    { 0x0126, 0x0128, MLineBreaker::EAiLineBreakClass },  //latin capital letter h with stroke
    { 0x0128, 0x012b, MLineBreaker::EAlLineBreakClass },  //latin capital letter i with tilde
    { 0x012b, 0x012c, MLineBreaker::EAiLineBreakClass },  //latin small letter i with macron
    { 0x012c, 0x0131, MLineBreaker::EAlLineBreakClass },  //latin capital letter i with breve
    { 0x0131, 0x0134, MLineBreaker::EAiLineBreakClass },  //latin small letter dotless i
    { 0x0134, 0x0138, MLineBreaker::EAlLineBreakClass },  //latin capital letter j with circumflex
    { 0x0138, 0x0139, MLineBreaker::EAiLineBreakClass },  //latin small letter kra
    { 0x0139, 0x013f, MLineBreaker::EAlLineBreakClass },  //latin capital letter l with acute
    { 0x013f, 0x0143, MLineBreaker::EAiLineBreakClass },  //latin capital letter l with middle dot
    { 0x0143, 0x0144, MLineBreaker::EAlLineBreakClass },  //latin capital letter n with acute
    { 0x0144, 0x0145, MLineBreaker::EAiLineBreakClass },  //latin small letter n with acute
    { 0x0145, 0x0148, MLineBreaker::EAlLineBreakClass },  //latin capital letter n with cedilla
    { 0x0148, 0x014b, MLineBreaker::EAiLineBreakClass },  //latin small letter n with caron
    { 0x014b, 0x014d, MLineBreaker::EAlLineBreakClass },  //latin small letter eng
    { 0x014d, 0x014e, MLineBreaker::EAiLineBreakClass },  //latin small letter o with macron
    { 0x014e, 0x0152, MLineBreaker::EAlLineBreakClass },  //latin capital letter o with breve
    { 0x0152, 0x0154, MLineBreaker::EAiLineBreakClass },  //latin capital ligature oe
    { 0x0154, 0x0166, MLineBreaker::EAlLineBreakClass },  //latin capital letter r with acute
    { 0x0166, 0x0168, MLineBreaker::EAiLineBreakClass },  //latin capital letter t with stroke
    { 0x0168, 0x016b, MLineBreaker::EAlLineBreakClass },  //latin capital letter u with tilde
    { 0x016b, 0x016c, MLineBreaker::EAiLineBreakClass },  //latin small letter u with macron
    { 0x016c, 0x01ce, MLineBreaker::EAlLineBreakClass },  //latin capital letter u with breve
    { 0x01ce, 0x01cf, MLineBreaker::EAiLineBreakClass },  //latin small letter a with caron
    { 0x01cf, 0x01d0, MLineBreaker::EAlLineBreakClass },  //latin capital letter i with caron
    { 0x01d0, 0x01d1, MLineBreaker::EAiLineBreakClass },  //latin small letter i with caron
    { 0x01d1, 0x01d2, MLineBreaker::EAlLineBreakClass },  //latin capital letter o with caron
    { 0x01d2, 0x01d3, MLineBreaker::EAiLineBreakClass },  //latin small letter o with caron
    { 0x01d3, 0x01d4, MLineBreaker::EAlLineBreakClass },  //latin capital letter u with caron
    { 0x01d4, 0x01d5, MLineBreaker::EAiLineBreakClass },  //latin small letter u with caron
    { 0x01d5, 0x01d6, MLineBreaker::EAlLineBreakClass },  //latin capital letter u with diaeresis and macron
    { 0x01d6, 0x01d7, MLineBreaker::EAiLineBreakClass },  //latin small letter u with diaeresis and macron
    { 0x01d7, 0x01d8, MLineBreaker::EAlLineBreakClass },  //latin capital letter u with diaeresis and acute
    { 0x01d8, 0x01d9, MLineBreaker::EAiLineBreakClass },  //latin small letter u with diaeresis and acute
    { 0x01d9, 0x01da, MLineBreaker::EAlLineBreakClass },  //latin capital letter u with diaeresis and caron
    { 0x01da, 0x01db, MLineBreaker::EAiLineBreakClass },  //latin small letter u with diaeresis and caron
    { 0x01db, 0x01dc, MLineBreaker::EAlLineBreakClass },  //latin capital letter u with diaeresis and grave
    { 0x01dc, 0x01dd, MLineBreaker::EAiLineBreakClass },  //latin small letter u with diaeresis and grave
    { 0x01dd, 0x0250, MLineBreaker::EAlLineBreakClass },  //latin small letter turned e
    { 0x0250, 0x0251, MLineBreaker::EAlLineBreakClass },  //latin small letter turned a
    { 0x0251, 0x0252, MLineBreaker::EAiLineBreakClass },  //latin small letter alpha
    { 0x0252, 0x0261, MLineBreaker::EAlLineBreakClass },  //latin small letter turned alpha
    { 0x0261, 0x0262, MLineBreaker::EAiLineBreakClass },  //latin small letter script g
    { 0x0262, 0x02c7, MLineBreaker::EAlLineBreakClass },  //latin letter small capital g
    { 0x02c7, 0x02c8, MLineBreaker::EAiLineBreakClass },  //caron
    { 0x02c8, 0x02c9, MLineBreaker::EBbLineBreakClass },  //modifier letter vertical line
    { 0x02c9, 0x02cc, MLineBreaker::EAiLineBreakClass },  //modifier letter macron
    { 0x02cc, 0x02cd, MLineBreaker::EBbLineBreakClass },  //modifier letter low vertical line
    { 0x02cd, 0x02ce, MLineBreaker::EAiLineBreakClass },  //modifier letter low macron
    { 0x02ce, 0x02d0, MLineBreaker::EAlLineBreakClass },  //modifier letter low grave accent
    { 0x02d0, 0x02d1, MLineBreaker::EAiLineBreakClass },  //modifier letter triangular colon
    { 0x02d1, 0x02d8, MLineBreaker::EAlLineBreakClass },  //modifier letter half triangular colon
    { 0x02d8, 0x02dc, MLineBreaker::EAiLineBreakClass },  //breve
    { 0x02dc, 0x02dd, MLineBreaker::EAlLineBreakClass },  //small tilde
    { 0x02dd, 0x02de, MLineBreaker::EAiLineBreakClass },  //double acute accent
    { 0x02de, 0x0300, MLineBreaker::EAlLineBreakClass },  //modifier letter rhotic hook
    { 0x0300, 0x034f, MLineBreaker::ECmLineBreakClass },  //combining grave accent
    { 0x034f, 0x0350, MLineBreaker::EGlLineBreakClass },  //COMBINING GRAPHEME JOINER
    { 0x0350, 0x035c, MLineBreaker::ECmLineBreakClass },  //COMBINING RIGHT ARROWHEAD ABOVE
    { 0x035c, 0x0360, MLineBreaker::EGlLineBreakClass },  //COMBINING DOUBLE BREVE BELOW
    { 0x0360, 0x0370, MLineBreaker::ECmLineBreakClass },  //combining double tilde
    { 0x0374, 0x0376, MLineBreaker::EAlLineBreakClass },  //greek numeral sign
    { 0x037a, 0x037b, MLineBreaker::EAlLineBreakClass },  //greek ypogegrammeni
    { 0x037b, 0x037e, MLineBreaker::EAlLineBreakClass },  //GREEK SMALL REVERSED LUNATE SIGMA SYMBOL
    { 0x037e, 0x037f, MLineBreaker::EAlLineBreakClass },  //greek question mark
    { 0x0384, 0x038b, MLineBreaker::EAlLineBreakClass },  //greek tonos
    { 0x038c, 0x038d, MLineBreaker::EAlLineBreakClass },  //greek capital letter omicron with tonos
    { 0x038e, 0x0391, MLineBreaker::EAlLineBreakClass },  //greek capital letter upsilon with tonos
    { 0x0391, 0x03a2, MLineBreaker::EAiLineBreakClass },  //greek capital letter alpha
    { 0x03a3, 0x03aa, MLineBreaker::EAiLineBreakClass },  //greek capital letter sigma
    { 0x03aa, 0x03b1, MLineBreaker::EAlLineBreakClass },  //greek capital letter iota with dialytika
    { 0x03b1, 0x03c2, MLineBreaker::EAiLineBreakClass },  //greek small letter alpha
    { 0x03c2, 0x03c3, MLineBreaker::EAlLineBreakClass },  //greek small letter final sigma
    { 0x03c3, 0x03ca, MLineBreaker::EAiLineBreakClass },  //greek small letter sigma
    { 0x03ca, 0x03cf, MLineBreaker::EAlLineBreakClass },  //greek small letter iota with dialytika
    { 0x03d0, 0x0400, MLineBreaker::EAlLineBreakClass },  //greek beta symbol
    { 0x0400, 0x0401, MLineBreaker::EAlLineBreakClass },  //cyrillic capital letter ie with grave
    { 0x0401, 0x0402, MLineBreaker::EAiLineBreakClass },  //cyrillic capital letter io
    { 0x0402, 0x0410, MLineBreaker::EAlLineBreakClass },  //cyrillic capital letter dje
    { 0x0410, 0x0450, MLineBreaker::EAiLineBreakClass },  //cyrillic capital letter a
    { 0x0450, 0x0451, MLineBreaker::EAlLineBreakClass },  //cyrillic small letter ie with grave
    { 0x0451, 0x0452, MLineBreaker::EAiLineBreakClass },  //cyrillic small letter io
    { 0x0452, 0x0483, MLineBreaker::EAlLineBreakClass },  //cyrillic small letter dje
    { 0x0483, 0x0487, MLineBreaker::ECmLineBreakClass },  //combining cyrillic titlo
    { 0x0488, 0x048a, MLineBreaker::ECmLineBreakClass },  //combining cyrillic hundred thousands sign
    { 0x048a, 0x0514, MLineBreaker::EAlLineBreakClass },  //CYRILLIC CAPITAL LETTER SHORT I WITH TAIL
    { 0x0531, 0x0557, MLineBreaker::EAlLineBreakClass },  //armenian capital letter ayb
    { 0x0559, 0x0560, MLineBreaker::EAlLineBreakClass },  //armenian modifier letter left half ring
    { 0x0561, 0x0588, MLineBreaker::EAlLineBreakClass },  //armenian small letter ayb
    { 0x0589, 0x058a, MLineBreaker::EIsLineBreakClass },  //armenian full stop
    { 0x058a, 0x058b, MLineBreaker::EBaLineBreakClass },  //armenian hyphen
    { 0x0591, 0x05be, MLineBreaker::ECmLineBreakClass },  //hebrew accent etnahta
    { 0x05be, 0x05bf, MLineBreaker::EAlLineBreakClass },  //hebrew punctuation maqaf
    { 0x05bf, 0x05c0, MLineBreaker::ECmLineBreakClass },  //hebrew point rafe
    { 0x05c0, 0x05c1, MLineBreaker::EAlLineBreakClass },  //hebrew punctuation paseq
    { 0x05c1, 0x05c3, MLineBreaker::ECmLineBreakClass },  //hebrew point shin dot
    { 0x05c3, 0x05c4, MLineBreaker::EAlLineBreakClass },  //hebrew punctuation sof pasuq
    { 0x05c4, 0x05c5, MLineBreaker::ECmLineBreakClass },  //hebrew mark upper dot
    { 0x05c5, 0x05c6, MLineBreaker::ECmLineBreakClass },  //HEBREW MARK LOWER DOT
    { 0x05c6, 0x05c7, MLineBreaker::EExLineBreakClass },  //HEBREW PUNCTUATION NUN HAFUKHA
    { 0x05c7, 0x05c8, MLineBreaker::ECmLineBreakClass },  //HEBREW POINT QAMATS QATAN
    { 0x05d0, 0x05eb, MLineBreaker::EAlLineBreakClass },  //hebrew letter alef
    { 0x05f0, 0x05f5, MLineBreaker::EAlLineBreakClass },  //hebrew ligature yiddish double vav
    { 0x0600, 0x0604, MLineBreaker::EAlLineBreakClass },  //ARABIC NUMBER SIGN
    { 0x060b, 0x060c, MLineBreaker::EPoLineBreakClass },  //AFGHANI SIGN
    { 0x060c, 0x060d, MLineBreaker::EExLineBreakClass },  //arabic comma
    { 0x060d, 0x060e, MLineBreaker::EIsLineBreakClass },  //ARABIC DATE SEPARATOR
    { 0x060e, 0x0610, MLineBreaker::EAlLineBreakClass },  //ARABIC POETIC VERSE SIGN
    { 0x0610, 0x0616, MLineBreaker::ECmLineBreakClass },  //ARABIC SIGN SALLALLAHOU ALAYHE WASSALLAM
    { 0x061b, 0x061c, MLineBreaker::EExLineBreakClass },  //arabic semicolon
    { 0x061e, 0x061f, MLineBreaker::EExLineBreakClass },  //ARABIC TRIPLE DOT PUNCTUATION MARK
    { 0x061f, 0x0620, MLineBreaker::EExLineBreakClass },  //arabic question mark
    { 0x0621, 0x063b, MLineBreaker::EAlLineBreakClass },  //arabic letter hamza
    { 0x0640, 0x064b, MLineBreaker::EAlLineBreakClass },  //arabic tatweel
    { 0x064b, 0x065f, MLineBreaker::ECmLineBreakClass },  //arabic fathatan
    { 0x0660, 0x066a, MLineBreaker::ENuLineBreakClass },  //arabic-indic digit zero
    { 0x066a, 0x066b, MLineBreaker::EExLineBreakClass },  //arabic percent sign
    { 0x066b, 0x066d, MLineBreaker::ENuLineBreakClass },  //arabic decimal & thousands separator
    { 0x066d, 0x066e, MLineBreaker::EAlLineBreakClass },  //arabic five pointed star
    { 0x066e, 0x0670, MLineBreaker::EAlLineBreakClass },  //arabic five pointed star
    { 0x0670, 0x0671, MLineBreaker::ECmLineBreakClass },  //arabic letter superscript alef
    { 0x0671, 0x06d4, MLineBreaker::EAlLineBreakClass },  //arabic letter alef wasla
    { 0x06d4, 0x06d5, MLineBreaker::EExLineBreakClass },  //arabic full stop
    { 0x06d5, 0x06d6, MLineBreaker::EAlLineBreakClass },  //arabic letter ae
    { 0x06d6, 0x06e5, MLineBreaker::ECmLineBreakClass },  //arabic small high ligature sad with lam with alef maksura
    { 0x06e5, 0x06e7, MLineBreaker::EAlLineBreakClass },  //arabic small waw
    { 0x06e7, 0x06e9, MLineBreaker::ECmLineBreakClass },  //arabic small high yeh
    { 0x06e9, 0x06ea, MLineBreaker::EAlLineBreakClass },  //arabic place of sajdah
    { 0x06ea, 0x06ee, MLineBreaker::ECmLineBreakClass },  //arabic empty centre low stop
    { 0x06ee, 0x06f0, MLineBreaker::EAlLineBreakClass },  //ARABIC LETTER DAL WITH INVERTED V
    { 0x06f0, 0x06fa, MLineBreaker::ENuLineBreakClass },  //extended arabic-indic digit zero
    { 0x06fa, 0x070e, MLineBreaker::EAlLineBreakClass },  //arabic letter sheen with dot below
    { 0x070f, 0x0710, MLineBreaker::ECmLineBreakClass },  //syriac abbreviation mark
    { 0x0710, 0x0711, MLineBreaker::EAlLineBreakClass },  //syriac letter alaph
    { 0x0711, 0x0712, MLineBreaker::ECmLineBreakClass },  //syriac letter superscript alaph
    { 0x0712, 0x0730, MLineBreaker::EAlLineBreakClass },  //syriac letter beth
    { 0x0730, 0x074b, MLineBreaker::ECmLineBreakClass },  //syriac pthaha above
    { 0x074d, 0x076e, MLineBreaker::EAlLineBreakClass },  //SYRIAC LETTER SOGDIAN ZHAIN
    { 0x0780, 0x07a6, MLineBreaker::EAlLineBreakClass },  //thaana letter haa
    { 0x07a6, 0x07b1, MLineBreaker::ECmLineBreakClass },  //thaana abafili
    { 0x07b1, 0x07b2, MLineBreaker::EAlLineBreakClass },  //THAANA LETTER NAA
    { 0x07c0, 0x07ca, MLineBreaker::ENuLineBreakClass },  //NKO DIGIT ZERO
    { 0x07ca, 0x07eb, MLineBreaker::EAlLineBreakClass },  //NKO LETTER A
    { 0x07eb, 0x07f4, MLineBreaker::ECmLineBreakClass },  //NKO COMBINING SHORT HIGH TONE
    { 0x07f4, 0x07f8, MLineBreaker::EAlLineBreakClass },  //NKO HIGH TONE APOSTROPHE
    { 0x07f8, 0x07f9, MLineBreaker::EIsLineBreakClass },  //NKO COMMA
    { 0x07f9, 0x07fa, MLineBreaker::EExLineBreakClass },  //NKO EXCLAMATION MARK
    { 0x07fa, 0x07fb, MLineBreaker::EAlLineBreakClass },  //NKO LAJANYALAN
    { 0x0901, 0x0904, MLineBreaker::ECmLineBreakClass },  //devanagari sign candrabindu
    { 0x0904, 0x093a, MLineBreaker::EAlLineBreakClass },  //devanagari letter short a: CR1080
    { 0x093c, 0x093d, MLineBreaker::ECmLineBreakClass },  //devanagari sign nukta
    { 0x093d, 0x093e, MLineBreaker::EAlLineBreakClass },  //devanagari sign avagraha
    { 0x093e, 0x094e, MLineBreaker::ECmLineBreakClass },  //devanagari vowel sign aa
    { 0x0950, 0x0951, MLineBreaker::EAlLineBreakClass },  //devanagari om
    { 0x0951, 0x0955, MLineBreaker::ECmLineBreakClass },  //devanagari stress sign udatta
    { 0x0958, 0x0962, MLineBreaker::EAlLineBreakClass },  //devanagari letter qa
    { 0x0962, 0x0964, MLineBreaker::ECmLineBreakClass },  //devanagari vowel sign vocalic l
    { 0x0964, 0x0966, MLineBreaker::EAlLineBreakClass },  //devanagari danda
    { 0x0966, 0x0970, MLineBreaker::ENuLineBreakClass },  //devanagari digit zero
    { 0x0970, 0x0971, MLineBreaker::EAlLineBreakClass },  //devanagari abbreviation sign
    { 0x097b, 0x0980, MLineBreaker::EAlLineBreakClass },  //devanagari letter gga: CR1080
    { 0x0981, 0x0984, MLineBreaker::ECmLineBreakClass },  //bengali sign candrabindu
    { 0x0985, 0x098d, MLineBreaker::EAlLineBreakClass },  //bengali letter a
    { 0x098f, 0x0991, MLineBreaker::EAlLineBreakClass },  //bengali letter e
    { 0x0993, 0x09a9, MLineBreaker::EAlLineBreakClass },  //bengali letter o
    { 0x09aa, 0x09b1, MLineBreaker::EAlLineBreakClass },  //bengali letter pa
    { 0x09b2, 0x09b3, MLineBreaker::EAlLineBreakClass },  //bengali letter la
    { 0x09b6, 0x09ba, MLineBreaker::EAlLineBreakClass },  //bengali letter sha
    { 0x09bc, 0x09bd, MLineBreaker::ECmLineBreakClass },  //bengali sign nukta
    { 0x09bd, 0x09be, MLineBreaker::EAlLineBreakClass },  //bengali sign avagraha: CR1080
    { 0x09be, 0x09c5, MLineBreaker::ECmLineBreakClass },  //bengali vowel sign aa
    { 0x09c7, 0x09c9, MLineBreaker::ECmLineBreakClass },  //bengali vowel sign e
    { 0x09cb, 0x09ce, MLineBreaker::ECmLineBreakClass },  //bengali vowel sign o
    { 0x09ce, 0x09cf, MLineBreaker::EAlLineBreakClass },  //bengali letter khanda ta: CR1080
    { 0x09d7, 0x09d8, MLineBreaker::ECmLineBreakClass },  //bengali au length mark
    { 0x09dc, 0x09de, MLineBreaker::EAlLineBreakClass },  //bengali letter rra
    { 0x09df, 0x09e2, MLineBreaker::EAlLineBreakClass },  //bengali letter yya
    { 0x09e2, 0x09e4, MLineBreaker::ECmLineBreakClass },  //bengali vowel sign vocalic l
    { 0x09e6, 0x09f0, MLineBreaker::ENuLineBreakClass },  //bengali digit zero
    { 0x09f0, 0x09f2, MLineBreaker::EAlLineBreakClass },  //bengali letter ra with middle diagonal
    { 0x09f2, 0x09f4, MLineBreaker::EPrLineBreakClass },  //bengali rupee mark
    { 0x09f4, 0x09fb, MLineBreaker::EAlLineBreakClass },  //bengali currency numerator one
    { 0x0a01, 0x0a04, MLineBreaker::ECmLineBreakClass },  //gurmukhi sign adak bindi: CR1080
    { 0x0a05, 0x0a0b, MLineBreaker::EAlLineBreakClass },  //gurmukhi letter a
    { 0x0a0f, 0x0a11, MLineBreaker::EAlLineBreakClass },  //gurmukhi letter ee
    { 0x0a13, 0x0a29, MLineBreaker::EAlLineBreakClass },  //gurmukhi letter oo
    { 0x0a2a, 0x0a31, MLineBreaker::EAlLineBreakClass },  //gurmukhi letter pa
    { 0x0a32, 0x0a34, MLineBreaker::EAlLineBreakClass },  //gurmukhi letter la
    { 0x0a35, 0x0a37, MLineBreaker::EAlLineBreakClass },  //gurmukhi letter va
    { 0x0a38, 0x0a3a, MLineBreaker::EAlLineBreakClass },  //gurmukhi letter sa
    { 0x0a3c, 0x0a3d, MLineBreaker::ECmLineBreakClass },  //gurmukhi sign nukta
    { 0x0a3e, 0x0a43, MLineBreaker::ECmLineBreakClass },  //gurmukhi vowel sign aa
    { 0x0a47, 0x0a49, MLineBreaker::ECmLineBreakClass },  //gurmukhi vowel sign ee
    { 0x0a4b, 0x0a4e, MLineBreaker::ECmLineBreakClass },  //gurmukhi vowel sign oo
    { 0x0a59, 0x0a5d, MLineBreaker::EAlLineBreakClass },  //gurmukhi letter khha
    { 0x0a5e, 0x0a5f, MLineBreaker::EAlLineBreakClass },  //gurmukhi letter fa
    { 0x0a66, 0x0a70, MLineBreaker::ENuLineBreakClass },  //gurmukhi digit zero
    { 0x0a70, 0x0a72, MLineBreaker::ECmLineBreakClass },  //gurmukhi tippi
    { 0x0a72, 0x0a75, MLineBreaker::EAlLineBreakClass },  //gurmukhi iri
    { 0x0a81, 0x0a84, MLineBreaker::ECmLineBreakClass },  //gujarati sign candrabindu
    { 0x0a85, 0x0a8d, MLineBreaker::EAlLineBreakClass },  //gujarati letter a: CR1080
    { 0x0a8d, 0x0a8e, MLineBreaker::EAlLineBreakClass },  //gujarati vowel candra e
    { 0x0a8f, 0x0a92, MLineBreaker::EAlLineBreakClass },  //gujarati letter e
    { 0x0a93, 0x0aa9, MLineBreaker::EAlLineBreakClass },  //gujarati letter o
    { 0x0aaa, 0x0ab1, MLineBreaker::EAlLineBreakClass },  //gujarati letter pa
    { 0x0ab2, 0x0ab4, MLineBreaker::EAlLineBreakClass },  //gujarati letter la
    { 0x0ab5, 0x0aba, MLineBreaker::EAlLineBreakClass },  //gujarati letter va
    { 0x0abc, 0x0abd, MLineBreaker::ECmLineBreakClass },  //gujarati sign nukta
    { 0x0abd, 0x0abe, MLineBreaker::EAlLineBreakClass },  //gujarati sign avagraha
    { 0x0abe, 0x0ac6, MLineBreaker::ECmLineBreakClass },  //gujarati vowel sign aa
    { 0x0ac7, 0x0aca, MLineBreaker::ECmLineBreakClass },  //gujarati vowel sign e
    { 0x0acb, 0x0ace, MLineBreaker::ECmLineBreakClass },  //gujarati vowel sign o
    { 0x0ad0, 0x0ad1, MLineBreaker::EAlLineBreakClass },  //gujarati om
    { 0x0ae0, 0x0ae2, MLineBreaker::EAlLineBreakClass },  //gujarati letter vocalic rr: CR1080
    { 0x0ae2, 0x0ae4, MLineBreaker::ECmLineBreakClass },  //gujarati vowel sign vocalic l: CR1080
    { 0x0ae6, 0x0af0, MLineBreaker::ENuLineBreakClass },  //gujarati digit zero
    { 0x0af1, 0x0af2, MLineBreaker::EPrLineBreakClass },  //gujarati rupee sign: CR1080
    { 0x0b01, 0x0b04, MLineBreaker::ECmLineBreakClass },  //oriya sign candrabindu
    { 0x0b05, 0x0b0d, MLineBreaker::EAlLineBreakClass },  //oriya letter a
    { 0x0b0f, 0x0b11, MLineBreaker::EAlLineBreakClass },  //oriya letter e
    { 0x0b13, 0x0b29, MLineBreaker::EAlLineBreakClass },  //oriya letter o
    { 0x0b2a, 0x0b31, MLineBreaker::EAlLineBreakClass },  //oriya letter pa
    { 0x0b32, 0x0b34, MLineBreaker::EAlLineBreakClass },  //oriya letter la
    { 0x0b35, 0x0b36, MLineBreaker::EAlLineBreakClass },  //oriya letter va: CR1080
    { 0x0b36, 0x0b3a, MLineBreaker::EAlLineBreakClass },  //oriya letter sha
    { 0x0b3c, 0x0b3d, MLineBreaker::ECmLineBreakClass },  //oriya sign nukta
    { 0x0b3d, 0x0b3e, MLineBreaker::EAlLineBreakClass },  //oriya sign avagraha
    { 0x0b3e, 0x0b44, MLineBreaker::ECmLineBreakClass },  //oriya vowel sign aa
    { 0x0b47, 0x0b49, MLineBreaker::ECmLineBreakClass },  //oriya vowel sign e
    { 0x0b4b, 0x0b4e, MLineBreaker::ECmLineBreakClass },  //oriya vowel sign o
    { 0x0b56, 0x0b58, MLineBreaker::ECmLineBreakClass },  //oriya ai length mark
    { 0x0b5c, 0x0b5e, MLineBreaker::EAlLineBreakClass },  //oriya letter rra
    { 0x0b5f, 0x0b62, MLineBreaker::EAlLineBreakClass },  //oriya letter yya
    { 0x0b66, 0x0b70, MLineBreaker::ENuLineBreakClass },  //oriya digit zero
    { 0x0b70, 0x0b71, MLineBreaker::EAlLineBreakClass },  //oriya isshar
    { 0x0b71, 0x0b72, MLineBreaker::EAlLineBreakClass },  //oriya letter wa: CR1080
    { 0x0b82, 0x0b84, MLineBreaker::ECmLineBreakClass },  //tamil sign anusvara
    { 0x0b85, 0x0b8b, MLineBreaker::EAlLineBreakClass },  //tamil letter a
    { 0x0b8e, 0x0b91, MLineBreaker::EAlLineBreakClass },  //tamil letter e
    { 0x0b92, 0x0b96, MLineBreaker::EAlLineBreakClass },  //tamil letter o
    { 0x0b99, 0x0b9b, MLineBreaker::EAlLineBreakClass },  //tamil letter nga
    { 0x0b9c, 0x0b9d, MLineBreaker::EAlLineBreakClass },  //tamil letter ja
    { 0x0b9e, 0x0ba0, MLineBreaker::EAlLineBreakClass },  //tamil letter nya
    { 0x0ba3, 0x0ba5, MLineBreaker::EAlLineBreakClass },  //tamil letter nna
    { 0x0ba8, 0x0bab, MLineBreaker::EAlLineBreakClass },  //tamil letter na
    { 0x0bae, 0x0bba, MLineBreaker::EAlLineBreakClass },  //tamil letter ma: CR1080
    { 0x0bbe, 0x0bc3, MLineBreaker::ECmLineBreakClass },  //tamil vowel sign aa
    { 0x0bc6, 0x0bc9, MLineBreaker::ECmLineBreakClass },  //tamil vowel sign e
    { 0x0bca, 0x0bce, MLineBreaker::ECmLineBreakClass },  //tamil vowel sign o
    { 0x0bd7, 0x0bd8, MLineBreaker::ECmLineBreakClass },  //tamil au length mark
    { 0x0be6, 0x0bf0, MLineBreaker::ENuLineBreakClass },  //tamil digit one: CR1080
    { 0x0bf0, 0x0bf9, MLineBreaker::EAlLineBreakClass },  //tamil number ten: CR1080
    { 0x0bf9, 0x0bfa, MLineBreaker::EPrLineBreakClass },  //tamil rupee sign: CR1080
    { 0x0bfa, 0x0bfb, MLineBreaker::EAlLineBreakClass },  //tamil number sign: CR1080
    { 0x0c01, 0x0c04, MLineBreaker::ECmLineBreakClass },  //telugu sign candrabindu
    { 0x0c05, 0x0c0d, MLineBreaker::EAlLineBreakClass },  //telugu letter a
    { 0x0c0e, 0x0c11, MLineBreaker::EAlLineBreakClass },  //telugu letter e
    { 0x0c12, 0x0c29, MLineBreaker::EAlLineBreakClass },  //telugu letter o
    { 0x0c2a, 0x0c34, MLineBreaker::EAlLineBreakClass },  //telugu letter pa
    { 0x0c35, 0x0c3a, MLineBreaker::EAlLineBreakClass },  //telugu letter va
    { 0x0c3e, 0x0c45, MLineBreaker::ECmLineBreakClass },  //telugu vowel sign aa
    { 0x0c46, 0x0c49, MLineBreaker::ECmLineBreakClass },  //telugu vowel sign e
    { 0x0c4a, 0x0c4e, MLineBreaker::ECmLineBreakClass },  //telugu vowel sign o
    { 0x0c55, 0x0c57, MLineBreaker::ECmLineBreakClass },  //telugu length mark
    { 0x0c60, 0x0c62, MLineBreaker::EAlLineBreakClass },  //telugu letter vocalic rr
    { 0x0c66, 0x0c70, MLineBreaker::ENuLineBreakClass },  //telugu digit zero
    { 0x0c82, 0x0c84, MLineBreaker::ECmLineBreakClass },  //kannada sign anusvara
    { 0x0c85, 0x0c8d, MLineBreaker::EAlLineBreakClass },  //kannada letter a
    { 0x0c8e, 0x0c91, MLineBreaker::EAlLineBreakClass },  //kannada letter e
    { 0x0c92, 0x0ca9, MLineBreaker::EAlLineBreakClass },  //kannada letter o
    { 0x0caa, 0x0cb4, MLineBreaker::EAlLineBreakClass },  //kannada letter pa
    { 0x0cb5, 0x0cba, MLineBreaker::EAlLineBreakClass },  //kannada letter va
    { 0x0cbc, 0x0cbd, MLineBreaker::ECmLineBreakClass },  //kannada sign nukta: CR1080
    { 0x0cbd, 0x0cbe, MLineBreaker::EAlLineBreakClass },  //kannada sign avagraha: CR1080
    { 0x0cbe, 0x0cc5, MLineBreaker::ECmLineBreakClass },  //kannada vowel sign aa
    { 0x0cc6, 0x0cc9, MLineBreaker::ECmLineBreakClass },  //kannada vowel sign e
    { 0x0cca, 0x0cce, MLineBreaker::ECmLineBreakClass },  //kannada vowel sign o
    { 0x0cd5, 0x0cd7, MLineBreaker::ECmLineBreakClass },  //kannada length mark
    { 0x0cde, 0x0cdf, MLineBreaker::EAlLineBreakClass },  //kannada letter fa
    { 0x0ce0, 0x0ce2, MLineBreaker::EAlLineBreakClass },  //kannada letter vocalic rr
    { 0x0ce2, 0x0ce4, MLineBreaker::ECmLineBreakClass },  //kannada vowel sign vocalic l: CR1080
    { 0x0ce6, 0x0cf0, MLineBreaker::ENuLineBreakClass },  //kannada digit zero
    { 0x0cf1, 0x0cf3, MLineBreaker::EAlLineBreakClass },  //kannada sign jihvamuliya: CR1080
    { 0x0d02, 0x0d04, MLineBreaker::ECmLineBreakClass },  //malayalam sign anusvara
    { 0x0d05, 0x0d0d, MLineBreaker::EAlLineBreakClass },  //malayalam letter a
    { 0x0d0e, 0x0d11, MLineBreaker::EAlLineBreakClass },  //malayalam letter e
    { 0x0d12, 0x0d29, MLineBreaker::EAlLineBreakClass },  //malayalam letter o
    { 0x0d2a, 0x0d3a, MLineBreaker::EAlLineBreakClass },  //malayalam letter pa
    { 0x0d3e, 0x0d44, MLineBreaker::ECmLineBreakClass },  //malayalam vowel sign aa
    { 0x0d46, 0x0d49, MLineBreaker::ECmLineBreakClass },  //malayalam vowel sign e
    { 0x0d4a, 0x0d4e, MLineBreaker::ECmLineBreakClass },  //malayalam vowel sign o
    { 0x0d57, 0x0d58, MLineBreaker::ECmLineBreakClass },  //malayalam au length mark
    { 0x0d60, 0x0d62, MLineBreaker::EAlLineBreakClass },  //malayalam letter vocalic rr
    { 0x0d66, 0x0d70, MLineBreaker::ENuLineBreakClass },  //malayalam digit zero
    { 0x0d82, 0x0d84, MLineBreaker::ECmLineBreakClass },  //sinhala sign anusvaraya
    { 0x0d85, 0x0d97, MLineBreaker::EAlLineBreakClass },  //sinhala letter ayanna
    { 0x0d9a, 0x0db2, MLineBreaker::EAlLineBreakClass },  //sinhala letter alpapraana kayanna
    { 0x0db3, 0x0dbc, MLineBreaker::EAlLineBreakClass },  //sinhala letter sanyaka dayanna
    { 0x0dbd, 0x0dbe, MLineBreaker::EAlLineBreakClass },  //sinhala letter dantaja layanna
    { 0x0dc0, 0x0dc7, MLineBreaker::EAlLineBreakClass },  //sinhala letter vayanna
    { 0x0dca, 0x0dcb, MLineBreaker::ECmLineBreakClass },  //sinhala sign al-lakuna
    { 0x0dcf, 0x0dd5, MLineBreaker::ECmLineBreakClass },  //sinhala vowel sign aela-pilla
    { 0x0dd6, 0x0dd7, MLineBreaker::ECmLineBreakClass },  //sinhala vowel sign diga paa-pilla
    { 0x0dd8, 0x0de0, MLineBreaker::ECmLineBreakClass },  //sinhala vowel sign gaetta-pilla
    { 0x0df2, 0x0df4, MLineBreaker::ECmLineBreakClass },  //sinhala vowel sign diga gaetta-pilla
    { 0x0df4, 0x0df5, MLineBreaker::EAlLineBreakClass },  //sinhala punctuation kunddaliya
    { 0x0e01, 0x0e31, MLineBreaker::ESaLineBreakClass },  //thai character ko kai
    { 0x0e31, 0x0e32, MLineBreaker::ECmLineBreakClass },  //thai character mai han-akat
    { 0x0e32, 0x0e34, MLineBreaker::ESaLineBreakClass },  //thai character sara aa
    { 0x0e34, 0x0e3b, MLineBreaker::ECmLineBreakClass },  //thai character sara i
    { 0x0e3f, 0x0e40, MLineBreaker::EPrLineBreakClass },  //thai currency symbol baht
    { 0x0e40, 0x0e47, MLineBreaker::ESaLineBreakClass },  //thai character sara e
    { 0x0e47, 0x0e4f, MLineBreaker::ECmLineBreakClass },  //thai character maitaikhu
    { 0x0e4f, 0x0e50, MLineBreaker::EAlLineBreakClass },  //thai character fongman
    { 0x0e50, 0x0e5a, MLineBreaker::ENuLineBreakClass },  //thai digit zero
    { 0x0e5a, 0x0e5c, MLineBreaker::ENsLineBreakClass },  //thai character angkhankhu
    { 0x0e81, 0x0e83, MLineBreaker::ESaLineBreakClass },  //lao letter ko
    { 0x0e84, 0x0e85, MLineBreaker::ESaLineBreakClass },  //lao letter kho tam
    { 0x0e87, 0x0e89, MLineBreaker::ESaLineBreakClass },  //lao letter ngo
    { 0x0e8a, 0x0e8b, MLineBreaker::ESaLineBreakClass },  //lao letter so tam
    { 0x0e8d, 0x0e8e, MLineBreaker::ESaLineBreakClass },  //lao letter nyo
    { 0x0e94, 0x0e98, MLineBreaker::ESaLineBreakClass },  //lao letter do
    { 0x0e99, 0x0ea0, MLineBreaker::ESaLineBreakClass },  //lao letter no
    { 0x0ea1, 0x0ea4, MLineBreaker::ESaLineBreakClass },  //lao letter mo
    { 0x0ea5, 0x0ea6, MLineBreaker::ESaLineBreakClass },  //lao letter lo loot
    { 0x0ea7, 0x0ea8, MLineBreaker::ESaLineBreakClass },  //lao letter wo
    { 0x0eaa, 0x0eac, MLineBreaker::ESaLineBreakClass },  //lao letter so sung
    { 0x0ead, 0x0eb1, MLineBreaker::ESaLineBreakClass },  //lao letter o
    { 0x0eb1, 0x0eb2, MLineBreaker::ECmLineBreakClass },  //lao vowel sign mai kan
    { 0x0eb2, 0x0eb4, MLineBreaker::ESaLineBreakClass },  //lao vowel sign aa
    { 0x0eb4, 0x0eba, MLineBreaker::ECmLineBreakClass },  //lao vowel sign i
    { 0x0ebb, 0x0ebd, MLineBreaker::ECmLineBreakClass },  //lao vowel sign mai kon
    { 0x0ebd, 0x0ebe, MLineBreaker::ESaLineBreakClass },  //lao semivowel sign nyo
    { 0x0ec0, 0x0ec5, MLineBreaker::ESaLineBreakClass },  //lao vowel sign e
    { 0x0ec6, 0x0ec7, MLineBreaker::ESaLineBreakClass },  //lao ko la
    { 0x0ec8, 0x0ece, MLineBreaker::ECmLineBreakClass },  //lao tone mai ek
    { 0x0ed0, 0x0eda, MLineBreaker::ENuLineBreakClass },  //lao digit zero
    { 0x0edc, 0x0ede, MLineBreaker::ESaLineBreakClass },  //lao ho no
    { 0x0f00, 0x0f0b, MLineBreaker::EAlLineBreakClass },  //tibetan syllable om
    { 0x0f0b, 0x0f0c, MLineBreaker::EBaLineBreakClass },  //tibetan mark intersyllabic tsheg
    { 0x0f0c, 0x0f0d, MLineBreaker::EGlLineBreakClass },  //tibetan mark delimiter tsheg bstar
    { 0x0f0d, 0x0f18, MLineBreaker::EAlLineBreakClass },  //tibetan mark shad
    { 0x0f18, 0x0f1a, MLineBreaker::ECmLineBreakClass },  //tibetan astrological sign -khyud pa
    { 0x0f1a, 0x0f20, MLineBreaker::EAlLineBreakClass },  //tibetan sign rdel dkar gcig
    { 0x0f20, 0x0f2a, MLineBreaker::ENuLineBreakClass },  //tibetan digit zero
    { 0x0f2a, 0x0f35, MLineBreaker::EAlLineBreakClass },  //tibetan digit half one
    { 0x0f35, 0x0f36, MLineBreaker::ECmLineBreakClass },  //tibetan mark ngas bzung nyi zla
    { 0x0f36, 0x0f37, MLineBreaker::EAlLineBreakClass },  //tibetan mark caret -dzud rtags bzhi mig can
    { 0x0f37, 0x0f38, MLineBreaker::ECmLineBreakClass },  //tibetan mark ngas bzung sgor rtags
    { 0x0f38, 0x0f39, MLineBreaker::EAlLineBreakClass },  //tibetan mark che mgo
    { 0x0f39, 0x0f3a, MLineBreaker::ECmLineBreakClass },  //tibetan mark tsa -phru
    { 0x0f3a, 0x0f3b, MLineBreaker::EOpLineBreakClass },  //tibetan mark gug rtags gyon
    { 0x0f3b, 0x0f3c, MLineBreaker::EClLineBreakClass },  //tibetan mark gug rtags gyas
    { 0x0f3c, 0x0f3d, MLineBreaker::EOpLineBreakClass },  //tibetan mark ang khang gyon
    { 0x0f3d, 0x0f3e, MLineBreaker::EClLineBreakClass },  //tibetan mark ang khang gyas
    { 0x0f3e, 0x0f40, MLineBreaker::ECmLineBreakClass },  //tibetan sign yar tshes
    { 0x0f40, 0x0f48, MLineBreaker::EAlLineBreakClass },  //tibetan letter ka
    { 0x0f49, 0x0f6b, MLineBreaker::EAlLineBreakClass },  //tibetan letter nya
    { 0x0f71, 0x0f85, MLineBreaker::ECmLineBreakClass },  //tibetan vowel sign aa
    { 0x0f85, 0x0f86, MLineBreaker::EAlLineBreakClass },  //tibetan mark paluta
    { 0x0f86, 0x0f88, MLineBreaker::ECmLineBreakClass },  //tibetan sign lci rtags
    { 0x0f88, 0x0f8c, MLineBreaker::EAlLineBreakClass },  //tibetan sign lce tsa can
    { 0x0f90, 0x0f98, MLineBreaker::ECmLineBreakClass },  //tibetan subjoined letter ka
    { 0x0f99, 0x0fbd, MLineBreaker::ECmLineBreakClass },  //tibetan subjoined letter nya
    { 0x0fbe, 0x0fc6, MLineBreaker::EAlLineBreakClass },  //tibetan ku ru kha
    { 0x0fc6, 0x0fc7, MLineBreaker::ECmLineBreakClass },  //tibetan symbol padma gdan
    { 0x0fc7, 0x0fcd, MLineBreaker::EAlLineBreakClass },  //tibetan symbol rdo rje rgya gram
    { 0x0fcf, 0x0fd0, MLineBreaker::EAlLineBreakClass },  //tibetan sign rdel nag gsum
    { 0x0fd0, 0x0fd2, MLineBreaker::EBbLineBreakClass },  //TIBETAN MARK BSKA- SHOG GI MGO RGYAN
    { 0x1000, 0x1022, MLineBreaker::ESaLineBreakClass },  //myanmar letter ka
    { 0x1023, 0x1028, MLineBreaker::ESaLineBreakClass },  //myanmar letter i
    { 0x1029, 0x102b, MLineBreaker::ESaLineBreakClass },  //myanmar letter o
    { 0x102c, 0x1033, MLineBreaker::ECmLineBreakClass },  //myanmar vowel sign aa
    { 0x1036, 0x103a, MLineBreaker::ECmLineBreakClass },  //myanmar sign anusvara
    { 0x1040, 0x104a, MLineBreaker::ENuLineBreakClass },  //myanmar digit zero
    { 0x104a, 0x1050, MLineBreaker::EAlLineBreakClass },  //myanmar sign little section
    { 0x1050, 0x1056, MLineBreaker::ESaLineBreakClass },  //myanmar letter sha
    { 0x1056, 0x105a, MLineBreaker::ECmLineBreakClass },  //myanmar vowel sign vocalic r
    { 0x10a0, 0x10c6, MLineBreaker::EAlLineBreakClass },  //georgian capital letter an
    { 0x10d0, 0x10fb, MLineBreaker::EAlLineBreakClass },  //georgian letter an
    { 0x10fb, 0x10fc, MLineBreaker::EAlLineBreakClass },  //georgian paragraph separator
    { 0x10fc, 0x10fd, MLineBreaker::EAlLineBreakClass },  //MODIFIER LETTER GEORGIAN NAR
    { 0x1100, 0x115a, MLineBreaker::EIdLineBreakClass },  //hangul choseong kiyeok
    { 0x115f, 0x1160, MLineBreaker::EIdLineBreakClass },  //hangul choseong filler
    { 0x1160, 0x11a3, MLineBreaker::ECmLineBreakClass },  //hangul jungseong filler
    { 0x11a8, 0x11fa, MLineBreaker::ECmLineBreakClass },  //hangul jongseong kiyeok
    { 0x1200, 0x1248, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable ha
    { 0x1248, 0x1249, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable qwa
    { 0x124a, 0x124e, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable qwi
    { 0x1250, 0x1257, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable qha
    { 0x1258, 0x1259, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable qhwa
    { 0x125a, 0x125e, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable qhwi
    { 0x1260, 0x1288, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable ba
    { 0x1288, 0x1289, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable xwa
    { 0x128a, 0x128e, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable xwi
    { 0x1290, 0x12b0, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable na
    { 0x12b0, 0x12b1, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable kwa
    { 0x12b2, 0x12b6, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable kwi
    { 0x12b8, 0x12bf, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable kxa
    { 0x12c0, 0x12c1, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable kxwa
    { 0x12c2, 0x12c6, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable kxwi
    { 0x12c8, 0x12d7, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable wa
    { 0x12d8, 0x1310, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable za
    { 0x1310, 0x1311, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable gwa
    { 0x1312, 0x1316, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable gwi
    { 0x1318, 0x135b, MLineBreaker::EAlLineBreakClass },  //ethiopic syllable gga
    { 0x135f, 0x1360, MLineBreaker::ECmLineBreakClass },  //ETHIOPIC COMBINING GEMINATION MARK
    { 0x1360, 0x1361, MLineBreaker::EAlLineBreakClass },  //ETHIOPIC SECTION MARK
    { 0x1361, 0x1362, MLineBreaker::EBaLineBreakClass },  //ethiopic wordspace
    { 0x1362, 0x1369, MLineBreaker::EAlLineBreakClass },  //ethiopic full stop
    { 0x1369, 0x1372, MLineBreaker::ENuLineBreakClass },  //ethiopic digit one
    { 0x1372, 0x137d, MLineBreaker::EAlLineBreakClass },  //ethiopic number ten
    { 0x1380, 0x139a, MLineBreaker::EAlLineBreakClass },  //ETHIOPIC SYLLABLE SEBATBEIT MWA
    { 0x13a0, 0x13f5, MLineBreaker::EAlLineBreakClass },  //cherokee letter a
    { 0x1401, 0x1677, MLineBreaker::EAlLineBreakClass },  //canadian syllabics e
    { 0x1680, 0x1681, MLineBreaker::EBaLineBreakClass },  //ogham space mark
    { 0x1681, 0x169b, MLineBreaker::EAlLineBreakClass },  //ogham letter beith
    { 0x169b, 0x169c, MLineBreaker::EOpLineBreakClass },  //ogham feather mark
    { 0x169c, 0x169d, MLineBreaker::EClLineBreakClass },  //ogham reversed feather mark
    { 0x16a0, 0x16f1, MLineBreaker::EAlLineBreakClass },  //runic letter fehu feoh fe f
    { 0x1700, 0x170d, MLineBreaker::EAlLineBreakClass },  //TAGALOG LETTER A
    { 0x170e, 0x1712, MLineBreaker::EAlLineBreakClass },  //TAGALOG LETTER LA
    { 0x1712, 0x1715, MLineBreaker::ECmLineBreakClass },  //TAGALOG VOWEL SIGN I
    { 0x1720, 0x1732, MLineBreaker::EAlLineBreakClass },  //HANUNOO LETTER A
    { 0x1732, 0x1735, MLineBreaker::ECmLineBreakClass },  //HANUNOO VOWEL SIGN I
    { 0x1735, 0x1737, MLineBreaker::EBaLineBreakClass },  //PHILIPPINE SINGLE PUNCTUATION
    { 0x1740, 0x1752, MLineBreaker::EAlLineBreakClass },  //BUHID LETTER A
    { 0x1752, 0x1754, MLineBreaker::ECmLineBreakClass },  //BUHID VOWEL SIGN I
    { 0x1760, 0x176d, MLineBreaker::EAlLineBreakClass },  //TAGBANWA LETTER A
    { 0x176e, 0x1771, MLineBreaker::EAlLineBreakClass },  //TAGBANWA LETTER LA
    { 0x1772, 0x1774, MLineBreaker::ECmLineBreakClass },  //TAGBANWA VOWEL SIGN I
    { 0x1780, 0x17b4, MLineBreaker::ESaLineBreakClass },  //khmer letter ka
    { 0x17b4, 0x17d4, MLineBreaker::ECmLineBreakClass },  //khmer vowel inherent aq
    { 0x17d4, 0x17d5, MLineBreaker::ENsLineBreakClass },  //khmer sign khan
    { 0x17d5, 0x17d6, MLineBreaker::EBaLineBreakClass },  //khmer sign bariyoosan
    { 0x17d6, 0x17db, MLineBreaker::ENsLineBreakClass },  //khmer sign camnuc pii kuuh
    { 0x17db, 0x17dc, MLineBreaker::EPrLineBreakClass },  //khmer currency symbol riel
    { 0x17dc, 0x17dd, MLineBreaker::EAlLineBreakClass },  //khmer sign avakrahasanya
    { 0x17dd, 0x17de, MLineBreaker::ESaLineBreakClass },  //KHMER SIGN ATTHACAN
    { 0x17e0, 0x17ea, MLineBreaker::ENuLineBreakClass },  //khmer digit zero
    { 0x17f0, 0x17fa, MLineBreaker::EAlLineBreakClass },  //KHMER SYMBOL LEK ATTAK SON
    { 0x1800, 0x1806, MLineBreaker::EAlLineBreakClass },  //mongolian birga
    { 0x1806, 0x1807, MLineBreaker::EBbLineBreakClass },  //mongolian todo soft hyphen
    { 0x1807, 0x180b, MLineBreaker::EAlLineBreakClass },  //mongolian sibe syllable boundary marker
    { 0x180b, 0x180f, MLineBreaker::ECmLineBreakClass },  //mongolian free variation selector one
    { 0x1810, 0x181a, MLineBreaker::ENuLineBreakClass },  //mongolian digit zero
    { 0x1820, 0x1878, MLineBreaker::EAlLineBreakClass },  //mongolian letter a
    { 0x1880, 0x18a9, MLineBreaker::EAlLineBreakClass },  //mongolian letter ali gali anusvara one
    { 0x18a9, 0x18aa, MLineBreaker::ECmLineBreakClass },  //mongolian letter ali gali dagalga
    { 0x1900, 0x191d, MLineBreaker::EAlLineBreakClass },  //LIMBU VOWEL-CARRIER LETTER
    { 0x1920, 0x192c, MLineBreaker::ECmLineBreakClass },  //LIMBU VOWEL SIGN A
    { 0x1930, 0x193c, MLineBreaker::ECmLineBreakClass },  //LIMBU SMALL LETTER KA
    { 0x1940, 0x1941, MLineBreaker::EAlLineBreakClass },  //LIMBU SIGN LOO
    { 0x1944, 0x1946, MLineBreaker::EExLineBreakClass },  //LIMBU EXCLAMATION MARK
    { 0x1946, 0x1950, MLineBreaker::ENuLineBreakClass },  //LIMBU DIGIT ZERO
    { 0x1950, 0x196e, MLineBreaker::ESaLineBreakClass },  //TAI LE LETTER KA
    { 0x1970, 0x1975, MLineBreaker::ESaLineBreakClass },  //TAI LE LETTER TONE-2
    { 0x1980, 0x19aa, MLineBreaker::ESaLineBreakClass },  //NEW TAI LUE LETTER HIGH QA
    { 0x19b0, 0x19ca, MLineBreaker::ESaLineBreakClass },  //NEW TAI LUE VOWEL SIGN VOWEL SHORTENER
    { 0x19d0, 0x19da, MLineBreaker::ENuLineBreakClass },  //NEW TAI LUE DIGIT ZERO
    { 0x19de, 0x19e0, MLineBreaker::ESaLineBreakClass },  //NEW TAI LUE SIGN LAE
    { 0x19e0, 0x1a17, MLineBreaker::EAlLineBreakClass },  //KHMER SYMBOL PATHAMASAT
    { 0x1a17, 0x1a1c, MLineBreaker::ECmLineBreakClass },  //BUGINESE VOWEL SIGN I
    { 0x1a1e, 0x1a1f, MLineBreaker::EBaLineBreakClass },  //BUGINESE PALLAWA
    { 0x1a1f, 0x1a20, MLineBreaker::EAlLineBreakClass },  //BUGINESE END OF SECTION
    { 0x1b00, 0x1b05, MLineBreaker::ECmLineBreakClass },  //BALINESE SIGN ULU RICEM
    { 0x1b05, 0x1b34, MLineBreaker::EAlLineBreakClass },  //BALINESE LETTER AKARA
    { 0x1b34, 0x1b45, MLineBreaker::ECmLineBreakClass },  //BALINESE SIGN REREKAN
    { 0x1b45, 0x1b4c, MLineBreaker::EAlLineBreakClass },  //BALINESE LETTER KAF SASAK
    { 0x1b50, 0x1b5a, MLineBreaker::ENuLineBreakClass },  //BALINESE DIGIT ZERO
    { 0x1b5a, 0x1b61, MLineBreaker::EBaLineBreakClass },  //BALINESE PANTI
    { 0x1b61, 0x1b6b, MLineBreaker::EAlLineBreakClass },  //BALINESE MUSICAL SYMBOL DONG
    { 0x1b6b, 0x1b74, MLineBreaker::ECmLineBreakClass },  //BALINESE MUSICAL SYMBOL COMBINING TEGEH
    { 0x1b74, 0x1b7d, MLineBreaker::EAlLineBreakClass },  //BALINESE MUSICAL SYMBOL RIGHT-HAND OPEN DUG
    { 0x1d00, 0x1dc0, MLineBreaker::EAlLineBreakClass },  //LATIN LETTER SMALL CAPITAL A
    { 0x1dc0, 0x1dcb, MLineBreaker::ECmLineBreakClass },  //COMBINING DOTTED GRAVE ACCENT
    { 0x1dfe, 0x1e00, MLineBreaker::ECmLineBreakClass },  //COMBINING LEFT ARROWHEAD ABOVE
    { 0x1e00, 0x1e9c, MLineBreaker::EAlLineBreakClass },  //latin capital letter a with ring below
    { 0x1ea0, 0x1efa, MLineBreaker::EAlLineBreakClass },  //latin capital letter a with dot below
    { 0x1f00, 0x1f16, MLineBreaker::EAlLineBreakClass },  //greek small letter alpha with psili
    { 0x1f18, 0x1f1e, MLineBreaker::EAlLineBreakClass },  //greek capital letter epsilon with psili
    { 0x1f20, 0x1f46, MLineBreaker::EAlLineBreakClass },  //greek small letter eta with psili
    { 0x1f48, 0x1f4e, MLineBreaker::EAlLineBreakClass },  //greek capital letter omicron with psili
    { 0x1f50, 0x1f58, MLineBreaker::EAlLineBreakClass },  //greek small letter upsilon with psili
    { 0x1f59, 0x1f5a, MLineBreaker::EAlLineBreakClass },  //greek capital letter upsilon with dasia
    { 0x1f5b, 0x1f5c, MLineBreaker::EAlLineBreakClass },  //greek capital letter upsilon with dasia and varia
    { 0x1f5d, 0x1f5e, MLineBreaker::EAlLineBreakClass },  //greek capital letter upsilon with dasia and oxia
    { 0x1f5f, 0x1f7e, MLineBreaker::EAlLineBreakClass },  //greek capital letter upsilon with dasia and perispomeni
    { 0x1f80, 0x1fb5, MLineBreaker::EAlLineBreakClass },  //greek small letter alpha with psili and ypogegrammeni
    { 0x1fb6, 0x1fc5, MLineBreaker::EAlLineBreakClass },  //greek small letter alpha with perispomeni
    { 0x1fc6, 0x1fd4, MLineBreaker::EAlLineBreakClass },  //greek small letter eta with perispomeni
    { 0x1fd6, 0x1fdc, MLineBreaker::EAlLineBreakClass },  //greek small letter iota with perispomeni
    { 0x1fdd, 0x1ff0, MLineBreaker::EAlLineBreakClass },  //greek dasia and varia
    { 0x1ff2, 0x1ff5, MLineBreaker::EAlLineBreakClass },  //greek small letter omega with varia and ypogegrammeni
    { 0x1ff6, 0x1fff, MLineBreaker::EAlLineBreakClass },  //greek small letter omega with perispomeni
    { 0x2000, 0x2007, MLineBreaker::EBaLineBreakClass },  //en quad
    { 0x2007, 0x2008, MLineBreaker::EGlLineBreakClass },  //figure space
    { 0x2008, 0x200b, MLineBreaker::EBaLineBreakClass },  //punctuation space
    { 0x200b, 0x200c, MLineBreaker::EZwLineBreakClass },  //zero width space
    { 0x200c, 0x2010, MLineBreaker::ECmLineBreakClass },  //zero width non-joiner
    { 0x2010, 0x2011, MLineBreaker::EBaLineBreakClass },  //hyphen
    { 0x2011, 0x2012, MLineBreaker::EGlLineBreakClass },  //non-breaking hyphen
    { 0x2012, 0x2014, MLineBreaker::EBaLineBreakClass },  //figure dash
    { 0x2014, 0x2015, MLineBreaker::EB2LineBreakClass },  //em dash
    { 0x2015, 0x2016, MLineBreaker::EAlLineBreakClass },  //horizontal bar
    { 0x2016, 0x2017, MLineBreaker::EAiLineBreakClass },  //double vertical line
    { 0x2017, 0x2018, MLineBreaker::EAlLineBreakClass },  //double low line
    { 0x2018, 0x201a, MLineBreaker::EQuLineBreakClass },  //left single quotation mark
    { 0x201a, 0x201b, MLineBreaker::EOpLineBreakClass },  //single low-9 quotation mark
    { 0x201b, 0x201e, MLineBreaker::EQuLineBreakClass },  //single high-reversed-9 quotation mark
    { 0x201e, 0x201f, MLineBreaker::EOpLineBreakClass },  //double low-9 quotation mark
    { 0x201f, 0x2020, MLineBreaker::EQuLineBreakClass },  //double high-reversed-9 quotation mark
    { 0x2020, 0x2022, MLineBreaker::EAiLineBreakClass },  //dagger
    { 0x2022, 0x2024, MLineBreaker::EAlLineBreakClass },  //bullet
    { 0x2024, 0x2027, MLineBreaker::EInLineBreakClass },  //one dot leader
    { 0x2027, 0x2028, MLineBreaker::EBaLineBreakClass },  //hyphenation point
    { 0x2028, 0x202a, MLineBreaker::EBkLineBreakClass },  //line separator
    { 0x202a, 0x202f, MLineBreaker::ECmLineBreakClass },  //left-to-right embedding
    { 0x202f, 0x2030, MLineBreaker::EGlLineBreakClass },  //narrow no-break space
    { 0x2030, 0x2038, MLineBreaker::EPoLineBreakClass },  //per mille sign
    { 0x2038, 0x2039, MLineBreaker::EAlLineBreakClass },  //caret
    { 0x2039, 0x203b, MLineBreaker::EQuLineBreakClass },  //single left-pointing angle quotation mark
    { 0x203b, 0x203c, MLineBreaker::EAiLineBreakClass },  //reference mark
    { 0x203c, 0x203d, MLineBreaker::ENsLineBreakClass },  //double exclamation mark
    { 0x203d, 0x2044, MLineBreaker::EAlLineBreakClass },  //interrobang
    { 0x2044, 0x2045, MLineBreaker::ENsLineBreakClass },  //fraction slash
    { 0x2045, 0x2046, MLineBreaker::EOpLineBreakClass },  //left square bracket with quill
    { 0x2046, 0x2047, MLineBreaker::EClLineBreakClass },  //right square bracket with quill
    { 0x2047, 0x2048, MLineBreaker::ENsLineBreakClass },  //DOUBLE QUESTION MARK
    { 0x2048, 0x2056, MLineBreaker::EAlLineBreakClass },  //question exclamation mark
    { 0x2056, 0x2057, MLineBreaker::EBaLineBreakClass },  //THREE DOT PUNCTUATION
    { 0x2057, 0x2058, MLineBreaker::EAlLineBreakClass },  //QUADRUPLE PRIME
    { 0x2058, 0x205c, MLineBreaker::EBaLineBreakClass },  //FOUR DOT PUNCTUATION
    { 0x205c, 0x205d, MLineBreaker::EAlLineBreakClass },  //DOTTED CROSS
    { 0x205d, 0x2060, MLineBreaker::EBaLineBreakClass },  //TRICOLON
    { 0x2060, 0x2061, MLineBreaker::EGlLineBreakClass },  //WORD JOINER
    { 0x2061, 0x2064, MLineBreaker::EAlLineBreakClass },  //FUNCTION APPLICATION
    { 0x206a, 0x2070, MLineBreaker::ECmLineBreakClass },  //inhibit symmetric swapping
    { 0x2070, 0x2071, MLineBreaker::EAlLineBreakClass },  //superscript zero
    { 0x2071, 0x2072, MLineBreaker::EAlLineBreakClass },  //SUPERSCRIPT LATIN SMALL LETTER I
    { 0x2074, 0x2075, MLineBreaker::EAiLineBreakClass },  //superscript four
    { 0x2075, 0x207d, MLineBreaker::EAlLineBreakClass },  //superscript five
    { 0x207d, 0x207e, MLineBreaker::EOpLineBreakClass },  //superscript left parenthesis
    { 0x207e, 0x207f, MLineBreaker::EClLineBreakClass },  //superscript right parenthesis
    { 0x207f, 0x2080, MLineBreaker::EAiLineBreakClass },  //superscript latin small letter n
    { 0x2080, 0x2081, MLineBreaker::EAlLineBreakClass },  //subscript zero
    { 0x2081, 0x2085, MLineBreaker::EAiLineBreakClass },  //subscript one
    { 0x2085, 0x208d, MLineBreaker::EAlLineBreakClass },  //subscript five
    { 0x208d, 0x208e, MLineBreaker::EOpLineBreakClass },  //subscript left parenthesis
    { 0x208e, 0x208f, MLineBreaker::EClLineBreakClass },  //subscript right parenthesis
    { 0x2090, 0x2095, MLineBreaker::EAlLineBreakClass },  //LATIN SUBSCRIPT SMALL LETTER A
    { 0x20a0, 0x20a7, MLineBreaker::EPrLineBreakClass },  //euro-currency sign
    { 0x20a7, 0x20a8, MLineBreaker::EPoLineBreakClass },  //peseta sign
    { 0x20a8, 0x20b6, MLineBreaker::EPrLineBreakClass },  //rupee sign
    { 0x20d0, 0x20f0, MLineBreaker::ECmLineBreakClass },  //combining left harpoon above
    { 0x2100, 0x2103, MLineBreaker::EAlLineBreakClass },  //account of
    { 0x2103, 0x2104, MLineBreaker::EPoLineBreakClass },  //degree celsius
    { 0x2104, 0x2105, MLineBreaker::EAlLineBreakClass },  //centre line symbol
    { 0x2105, 0x2106, MLineBreaker::EAiLineBreakClass },  //care of
    { 0x2106, 0x2109, MLineBreaker::EAlLineBreakClass },  //cada una
    { 0x2109, 0x210a, MLineBreaker::EPoLineBreakClass },  //degree fahrenheit
    { 0x210a, 0x2113, MLineBreaker::EAlLineBreakClass },  //script small g
    { 0x2113, 0x2114, MLineBreaker::EAiLineBreakClass },  //script small l
    { 0x2114, 0x2116, MLineBreaker::EAlLineBreakClass },  //l b bar symbol
    { 0x2116, 0x2117, MLineBreaker::EPrLineBreakClass },  //numero sign
    { 0x2117, 0x2121, MLineBreaker::EAlLineBreakClass },  //sound recording copyright
    { 0x2121, 0x2123, MLineBreaker::EAiLineBreakClass },  //telephone sign
    { 0x2123, 0x2126, MLineBreaker::EAlLineBreakClass },  //versicle
    { 0x2126, 0x2127, MLineBreaker::EPoLineBreakClass },  //ohm sign
    { 0x2127, 0x212b, MLineBreaker::EAlLineBreakClass },  //inverted ohm sign
    { 0x212b, 0x212c, MLineBreaker::EAiLineBreakClass },  //angstrom sign
    { 0x212c, 0x214f, MLineBreaker::EAlLineBreakClass },  //script capital b
    { 0x2153, 0x2154, MLineBreaker::EAlLineBreakClass },  //vulgar fraction one third
    { 0x2154, 0x2156, MLineBreaker::EAiLineBreakClass },  //vulgar fraction two thirds
    { 0x2156, 0x215b, MLineBreaker::EAlLineBreakClass },  //vulgar fraction two fifths
    { 0x215b, 0x215c, MLineBreaker::EAiLineBreakClass },  //vulgar fraction one eighth
    { 0x215c, 0x215e, MLineBreaker::EAlLineBreakClass },  //vulgar fraction three eighths
    { 0x215e, 0x215f, MLineBreaker::EAiLineBreakClass },  //vulgar fraction seven eighths
    { 0x215f, 0x2160, MLineBreaker::EAlLineBreakClass },  //fraction numerator one
    { 0x2160, 0x216c, MLineBreaker::EAiLineBreakClass },  //roman numeral one
    { 0x216c, 0x2170, MLineBreaker::EAlLineBreakClass },  //roman numeral fifty
    { 0x2170, 0x217a, MLineBreaker::EAiLineBreakClass },  //small roman numeral one
    { 0x217a, 0x2185, MLineBreaker::EAlLineBreakClass },  //small roman numeral eleven
    { 0x2190, 0x219a, MLineBreaker::EAiLineBreakClass },  //leftwards arrow
    { 0x219a, 0x21d2, MLineBreaker::EAlLineBreakClass },  //leftwards arrow with stroke
    { 0x21d2, 0x21d3, MLineBreaker::EAiLineBreakClass },  //rightwards double arrow
    { 0x21d3, 0x21d4, MLineBreaker::EAlLineBreakClass },  //downwards double arrow
    { 0x21d4, 0x21d5, MLineBreaker::EAiLineBreakClass },  //left right double arrow
    { 0x21d5, 0x2200, MLineBreaker::EAlLineBreakClass },  //up down double arrow
    { 0x2200, 0x2201, MLineBreaker::EAiLineBreakClass },  //for all
    { 0x2201, 0x2202, MLineBreaker::EAlLineBreakClass },  //complement
    { 0x2202, 0x2204, MLineBreaker::EAiLineBreakClass },  //partial differential
    { 0x2204, 0x2207, MLineBreaker::EAlLineBreakClass },  //there does not exist
    { 0x2207, 0x2209, MLineBreaker::EAiLineBreakClass },  //nabla
    { 0x2209, 0x220b, MLineBreaker::EAlLineBreakClass },  //not an element of
    { 0x220b, 0x220c, MLineBreaker::EAiLineBreakClass },  //contains as member
    { 0x220c, 0x220f, MLineBreaker::EAlLineBreakClass },  //does not contain as member
    { 0x220f, 0x2210, MLineBreaker::EAiLineBreakClass },  //n-ary product
    { 0x2210, 0x2211, MLineBreaker::EAlLineBreakClass },  //n-ary coproduct
    { 0x2211, 0x2212, MLineBreaker::EAiLineBreakClass },  //n-ary summation
    { 0x2212, 0x2214, MLineBreaker::EPrLineBreakClass },  //minus sign
    { 0x2214, 0x2215, MLineBreaker::EAlLineBreakClass },  //dot plus
    { 0x2215, 0x2216, MLineBreaker::EAiLineBreakClass },  //division slash
    { 0x2216, 0x221a, MLineBreaker::EAlLineBreakClass },  //set minus
    { 0x221a, 0x221b, MLineBreaker::EAiLineBreakClass },  //square root
    { 0x221b, 0x221d, MLineBreaker::EAlLineBreakClass },  //cube root
    { 0x221d, 0x2221, MLineBreaker::EAiLineBreakClass },  //proportional to
    { 0x2221, 0x2223, MLineBreaker::EAlLineBreakClass },  //measured angle
    { 0x2223, 0x2224, MLineBreaker::EAiLineBreakClass },  //divides
    { 0x2224, 0x2225, MLineBreaker::EAlLineBreakClass },  //does not divide
    { 0x2225, 0x2226, MLineBreaker::EAiLineBreakClass },  //parallel to
    { 0x2226, 0x2227, MLineBreaker::EAlLineBreakClass },  //not parallel to
    { 0x2227, 0x222d, MLineBreaker::EAiLineBreakClass },  //logical and
    { 0x222d, 0x222e, MLineBreaker::EAlLineBreakClass },  //triple integral
    { 0x222e, 0x222f, MLineBreaker::EAiLineBreakClass },  //contour integral
    { 0x222f, 0x2234, MLineBreaker::EAlLineBreakClass },  //surface integral
    { 0x2234, 0x2238, MLineBreaker::EAiLineBreakClass },  //therefore
    { 0x2238, 0x223c, MLineBreaker::EAlLineBreakClass },  //dot minus
    { 0x223c, 0x223e, MLineBreaker::EAiLineBreakClass },  //tilde operator
    { 0x223e, 0x2248, MLineBreaker::EAlLineBreakClass },  //inverted lazy s
    { 0x2248, 0x2249, MLineBreaker::EAiLineBreakClass },  //almost equal to
    { 0x2249, 0x224c, MLineBreaker::EAlLineBreakClass },  //not almost equal to
    { 0x224c, 0x224d, MLineBreaker::EAiLineBreakClass },  //all equal to
    { 0x224d, 0x2252, MLineBreaker::EAlLineBreakClass },  //equivalent to
    { 0x2252, 0x2253, MLineBreaker::EAiLineBreakClass },  //approximately equal to or the image of
    { 0x2253, 0x2260, MLineBreaker::EAlLineBreakClass },  //image of or approximately equal to
    { 0x2260, 0x2262, MLineBreaker::EAiLineBreakClass },  //not equal to
    { 0x2262, 0x2264, MLineBreaker::EAlLineBreakClass },  //not identical to
    { 0x2264, 0x2268, MLineBreaker::EAiLineBreakClass },  //less-than or equal to
    { 0x2268, 0x226a, MLineBreaker::EAlLineBreakClass },  //less-than but not equal to
    { 0x226a, 0x226c, MLineBreaker::EAiLineBreakClass },  //much less-than
    { 0x226c, 0x226e, MLineBreaker::EAlLineBreakClass },  //between
    { 0x226e, 0x2270, MLineBreaker::EAiLineBreakClass },  //not less-than
    { 0x2270, 0x2282, MLineBreaker::EAlLineBreakClass },  //neither less-than nor equal to
    { 0x2282, 0x2284, MLineBreaker::EAiLineBreakClass },  //subset of
    { 0x2284, 0x2286, MLineBreaker::EAlLineBreakClass },  //not a subset of
    { 0x2286, 0x2288, MLineBreaker::EAiLineBreakClass },  //subset of or equal to
    { 0x2288, 0x2295, MLineBreaker::EAlLineBreakClass },  //neither a subset of nor equal to
    { 0x2295, 0x2296, MLineBreaker::EAiLineBreakClass },  //circled plus
    { 0x2296, 0x2299, MLineBreaker::EAlLineBreakClass },  //circled minus
    { 0x2299, 0x229a, MLineBreaker::EAiLineBreakClass },  //circled dot operator
    { 0x229a, 0x22a5, MLineBreaker::EAlLineBreakClass },  //circled ring operator
    { 0x22a5, 0x22a6, MLineBreaker::EAiLineBreakClass },  //up tack
    { 0x22a6, 0x22bf, MLineBreaker::EAlLineBreakClass },  //assertion
    { 0x22bf, 0x22c0, MLineBreaker::EAiLineBreakClass },  //right triangle
    { 0x22c0, 0x2312, MLineBreaker::EAlLineBreakClass },  //n-ary logical and
    { 0x2312, 0x2313, MLineBreaker::EAiLineBreakClass },  //arc
    { 0x2313, 0x2329, MLineBreaker::EAlLineBreakClass },  //segment
    { 0x2329, 0x232a, MLineBreaker::EOpLineBreakClass },  //left-pointing angle bracket
    { 0x232a, 0x232b, MLineBreaker::EClLineBreakClass },  //right-pointing angle bracket
    { 0x232b, 0x23e8, MLineBreaker::EAlLineBreakClass },  //erase to the left
    { 0x2400, 0x2427, MLineBreaker::EAlLineBreakClass },  //symbol for null
    { 0x2440, 0x244b, MLineBreaker::EAlLineBreakClass },  //ocr hook
    { 0x2460, 0x24c0, MLineBreaker::EAiLineBreakClass },  //circled digit one
    { 0x24c0, 0x24d0, MLineBreaker::EAlLineBreakClass },  //circled latin capital letter k
    { 0x24d0, 0x24ea, MLineBreaker::EAiLineBreakClass },  //circled latin small letter a
    { 0x24ea, 0x24eb, MLineBreaker::EAlLineBreakClass },  //circled digit zero
    { 0x24eb, 0x24ff, MLineBreaker::EAiLineBreakClass },  //NEGATIVE CIRCLED NUMBER ELEVEN
    { 0x24ff, 0x2500, MLineBreaker::EAlLineBreakClass },  //NEGATIVE CIRCLED DIGIT ZERO
    { 0x2500, 0x254c, MLineBreaker::EAiLineBreakClass },  //box drawings light horizontal
    { 0x254c, 0x2550, MLineBreaker::EAlLineBreakClass },  //box drawings light double dash horizontal
    { 0x2550, 0x2575, MLineBreaker::EAiLineBreakClass },  //box drawings double horizontal
    { 0x2575, 0x2580, MLineBreaker::EAlLineBreakClass },  //box drawings light up
    { 0x2580, 0x2590, MLineBreaker::EAiLineBreakClass },  //upper half block
    { 0x2590, 0x2592, MLineBreaker::EAlLineBreakClass },  //right half block
    { 0x2592, 0x2596, MLineBreaker::EAiLineBreakClass },  //medium shade
    { 0x2596, 0x25a0, MLineBreaker::EAlLineBreakClass },  //QUADRANT LOWER LEFT
    { 0x25a0, 0x25a2, MLineBreaker::EAiLineBreakClass },  //black square
    { 0x25a2, 0x25a3, MLineBreaker::EAlLineBreakClass },  //white square with rounded corners
    { 0x25a3, 0x25aa, MLineBreaker::EAiLineBreakClass },  //white square containing black small square
    { 0x25aa, 0x25b2, MLineBreaker::EAlLineBreakClass },  //black small square
    { 0x25b2, 0x25b4, MLineBreaker::EAiLineBreakClass },  //black up-pointing triangle
    { 0x25b4, 0x25b6, MLineBreaker::EAlLineBreakClass },  //black up-pointing small triangle
    { 0x25b6, 0x25b8, MLineBreaker::EAiLineBreakClass },  //black right-pointing triangle
    { 0x25b8, 0x25bc, MLineBreaker::EAlLineBreakClass },  //black right-pointing small triangle
    { 0x25bc, 0x25be, MLineBreaker::EAiLineBreakClass },  //black down-pointing triangle
    { 0x25be, 0x25c0, MLineBreaker::EAlLineBreakClass },  //black down-pointing small triangle
    { 0x25c0, 0x25c2, MLineBreaker::EAiLineBreakClass },  //black left-pointing triangle
    { 0x25c2, 0x25c6, MLineBreaker::EAlLineBreakClass },  //black left-pointing small triangle
    { 0x25c6, 0x25c8, MLineBreaker::EAiLineBreakClass },  //black diamond
    { 0x25c8, 0x25cb, MLineBreaker::EAlLineBreakClass },  //white diamond containing black small diamond
    { 0x25cb, 0x25cc, MLineBreaker::EAiLineBreakClass },  //white circle
    { 0x25cc, 0x25ce, MLineBreaker::EAlLineBreakClass },  //dotted circle
    { 0x25ce, 0x25d2, MLineBreaker::EAiLineBreakClass },  //bullseye
    { 0x25d2, 0x25e2, MLineBreaker::EAlLineBreakClass },  //circle with lower half black
    { 0x25e2, 0x25e6, MLineBreaker::EAiLineBreakClass },  //black lower right triangle
    { 0x25e6, 0x25ef, MLineBreaker::EAlLineBreakClass },  //white bullet
    { 0x25ef, 0x25f0, MLineBreaker::EAiLineBreakClass },  //large circle
    { 0x25f0, 0x2605, MLineBreaker::EAlLineBreakClass },  //white square with upper left quadrant
    { 0x2605, 0x2607, MLineBreaker::EAiLineBreakClass },  //black star
    { 0x2607, 0x2609, MLineBreaker::EAlLineBreakClass },  //lightning
    { 0x2609, 0x260a, MLineBreaker::EAiLineBreakClass },  //sun
    { 0x260a, 0x260e, MLineBreaker::EAlLineBreakClass },  //ascending node
    { 0x260e, 0x2610, MLineBreaker::EAiLineBreakClass },  //black telephone
    { 0x2610, 0x2614, MLineBreaker::EAlLineBreakClass },  //ballot box
    { 0x2614, 0x2618, MLineBreaker::EAiLineBreakClass },  //UMBRELLA WITH RAIN DROPS
    { 0x2618, 0x261c, MLineBreaker::EAlLineBreakClass },  //SHAMROCK
    { 0x261c, 0x261d, MLineBreaker::EAiLineBreakClass },  //white left pointing index
    { 0x261d, 0x261e, MLineBreaker::EAlLineBreakClass },  //white up pointing index
    { 0x261e, 0x261f, MLineBreaker::EAiLineBreakClass },  //white right pointing index
    { 0x261f, 0x2640, MLineBreaker::EAlLineBreakClass },  //white down pointing index
    { 0x2640, 0x2641, MLineBreaker::EAiLineBreakClass },  //female sign
    { 0x2641, 0x2642, MLineBreaker::EAlLineBreakClass },  //earth
    { 0x2642, 0x2643, MLineBreaker::EAiLineBreakClass },  //male sign
    { 0x2643, 0x2660, MLineBreaker::EAlLineBreakClass },  //jupiter
    { 0x2660, 0x2662, MLineBreaker::EAiLineBreakClass },  //black spade suit
    { 0x2662, 0x2663, MLineBreaker::EAlLineBreakClass },  //white diamond suit
    { 0x2663, 0x2666, MLineBreaker::EAiLineBreakClass },  //black club suit
    { 0x2666, 0x2667, MLineBreaker::EAlLineBreakClass },  //black diamond suit
    { 0x2667, 0x266b, MLineBreaker::EAiLineBreakClass },  //white club suit
    { 0x266b, 0x266c, MLineBreaker::EAlLineBreakClass },  //beamed eighth notes
    { 0x266c, 0x266e, MLineBreaker::EAiLineBreakClass },  //beamed sixteenth notes
    { 0x266e, 0x266f, MLineBreaker::EAlLineBreakClass },  //music natural sign
    { 0x266f, 0x2670, MLineBreaker::EAiLineBreakClass },  //music sharp sign
    { 0x2670, 0x269d, MLineBreaker::EAlLineBreakClass },  //west syriac cross
    { 0x26a0, 0x26b3, MLineBreaker::EAlLineBreakClass },  //WARNING SIGN
    { 0x2701, 0x2705, MLineBreaker::EAlLineBreakClass },  //upper blade scissors
    { 0x2706, 0x270a, MLineBreaker::EAlLineBreakClass },  //telephone location sign
    { 0x270c, 0x2728, MLineBreaker::EAlLineBreakClass },  //victory hand
    { 0x2729, 0x274c, MLineBreaker::EAlLineBreakClass },  //stress outlined white star
    { 0x274d, 0x274e, MLineBreaker::EAlLineBreakClass },  //shadowed white circle
    { 0x274f, 0x2753, MLineBreaker::EAlLineBreakClass },  //lower right drop-shadowed white square
    { 0x2756, 0x2757, MLineBreaker::EAlLineBreakClass },  //black diamond minus white x
    { 0x2758, 0x275f, MLineBreaker::EAlLineBreakClass },  //light vertical bar
    { 0x2761, 0x2768, MLineBreaker::EAlLineBreakClass },  //curved stem paragraph sign ornament
    { 0x2768, 0x2769, MLineBreaker::EOpLineBreakClass },  //MEDIUM LEFT PARENTHESIS ORNAMENT
    { 0x2769, 0x276a, MLineBreaker::EClLineBreakClass },  //MEDIUM RIGHT PARENTHESIS ORNAMENT
    { 0x276a, 0x276b, MLineBreaker::EOpLineBreakClass },  //MEDIUM FLATTENED LEFT PARENTHESIS ORNAMENT
    { 0x276b, 0x276c, MLineBreaker::EClLineBreakClass },  //MEDIUM FLATTENED RIGHT PARENTHESIS ORNAMENT
    { 0x276c, 0x276d, MLineBreaker::EOpLineBreakClass },  //MEDIUM LEFT-POINTING ANGLE BRACKET ORNAMENT
    { 0x276d, 0x276e, MLineBreaker::EClLineBreakClass },  //MEDIUM RIGHT-POINTING ANGLE BRACKET ORNAMENT
    { 0x276e, 0x276f, MLineBreaker::EOpLineBreakClass },  //HEAVY LEFT-POINTING ANGLE QUOTATION MARK ORNAMENT
    { 0x276f, 0x2770, MLineBreaker::EClLineBreakClass },  //HEAVY RIGHT-POINTING ANGLE QUOTATION MARK ORNAMENT
    { 0x2770, 0x2771, MLineBreaker::EOpLineBreakClass },  //HEAVY LEFT-POINTING ANGLE BRACKET ORNAMENT
    { 0x2771, 0x2772, MLineBreaker::EClLineBreakClass },  //HEAVY RIGHT-POINTING ANGLE BRACKET ORNAMENT
    { 0x2772, 0x2773, MLineBreaker::EOpLineBreakClass },  //LIGHT LEFT TORTOISE SHELL BRACKET ORNAMENT
    { 0x2773, 0x2774, MLineBreaker::EClLineBreakClass },  //LIGHT RIGHT TORTOISE SHELL BRACKET ORNAMENT
    { 0x2774, 0x2775, MLineBreaker::EOpLineBreakClass },  //MEDIUM LEFT CURLY BRACKET ORNAMENT
    { 0x2775, 0x2776, MLineBreaker::EClLineBreakClass },  //MEDIUM RIGHT CURLY BRACKET ORNAMENT
    { 0x2776, 0x2795, MLineBreaker::EAlLineBreakClass },  //dingbat negative circled digit one
    { 0x2798, 0x27b0, MLineBreaker::EAlLineBreakClass },  //heavy south east arrow
    { 0x27b1, 0x27bf, MLineBreaker::EAlLineBreakClass },  //notched upper right-shadowed white rightwards arrow
    { 0x27c0, 0x27c5, MLineBreaker::EAlLineBreakClass },  //THREE DIMENSIONAL ANGLE
    { 0x27c5, 0x27c6, MLineBreaker::EOpLineBreakClass },  //LEFT S-SHAPED BAG DELIMITER
    { 0x27c6, 0x27c7, MLineBreaker::EClLineBreakClass },  //RIGHT S-SHAPED BAG DELIMITER
    { 0x27c7, 0x27cb, MLineBreaker::EAlLineBreakClass },  //OR WITH DOT INSIDE
    { 0x27d0, 0x27e6, MLineBreaker::EAlLineBreakClass },  //WHITE DIAMOND WITH CENTRED DOT
    { 0x27e6, 0x27e7, MLineBreaker::EOpLineBreakClass },  //MATHEMATICAL LEFT WHITE SQUARE BRACKET
    { 0x27e7, 0x27e8, MLineBreaker::EClLineBreakClass },  //MATHEMATICAL RIGHT WHITE SQUARE BRACKET
    { 0x27e8, 0x27e9, MLineBreaker::EOpLineBreakClass },  //MATHEMATICAL LEFT ANGLE BRACKET
    { 0x27e9, 0x27ea, MLineBreaker::EClLineBreakClass },  //MATHEMATICAL RIGHT ANGLE BRACKET
    { 0x27ea, 0x27eb, MLineBreaker::EOpLineBreakClass },  //MATHEMATICAL LEFT DOUBLE ANGLE BRACKET
    { 0x27eb, 0x27ec, MLineBreaker::EClLineBreakClass },  //MATHEMATICAL RIGHT DOUBLE ANGLE BRACKET
    { 0x27f0, 0x2983, MLineBreaker::EAlLineBreakClass },  //UPWARDS QUADRUPLE ARROW
    { 0x2983, 0x2984, MLineBreaker::EOpLineBreakClass },  //LEFT WHITE CURLY BRACKET
    { 0x2984, 0x2985, MLineBreaker::EClLineBreakClass },  //RIGHT WHITE CURLY BRACKET
    { 0x2985, 0x2986, MLineBreaker::EOpLineBreakClass },  //LEFT WHITE PARENTHESIS
    { 0x2986, 0x2987, MLineBreaker::EClLineBreakClass },  //RIGHT WHITE PARENTHESIS
    { 0x2987, 0x2988, MLineBreaker::EOpLineBreakClass },  //Z NOTATION LEFT IMAGE BRACKET
    { 0x2988, 0x2989, MLineBreaker::EClLineBreakClass },  //Z NOTATION RIGHT IMAGE BRACKET
    { 0x2989, 0x298a, MLineBreaker::EOpLineBreakClass },  //Z NOTATION LEFT BINDING BRACKET
    { 0x298a, 0x298b, MLineBreaker::EClLineBreakClass },  //Z NOTATION RIGHT BINDING BRACKET
    { 0x298b, 0x298c, MLineBreaker::EOpLineBreakClass },  //LEFT SQUARE BRACKET WITH UNDERBAR
    { 0x298c, 0x298d, MLineBreaker::EClLineBreakClass },  //RIGHT SQUARE BRACKET WITH UNDERBAR
    { 0x298d, 0x298e, MLineBreaker::EOpLineBreakClass },  //LEFT SQUARE BRACKET WITH TICK IN TOP CORNER
    { 0x298e, 0x298f, MLineBreaker::EClLineBreakClass },  //RIGHT SQUARE BRACKET WITH TICK IN BOTTOM CORNER
    { 0x298f, 0x2990, MLineBreaker::EOpLineBreakClass },  //LEFT SQUARE BRACKET WITH TICK IN BOTTOM CORNER
    { 0x2990, 0x2991, MLineBreaker::EClLineBreakClass },  //RIGHT SQUARE BRACKET WITH TICK IN TOP CORNER
    { 0x2991, 0x2992, MLineBreaker::EOpLineBreakClass },  //LEFT ANGLE BRACKET WITH DOT
    { 0x2992, 0x2993, MLineBreaker::EClLineBreakClass },  //RIGHT ANGLE BRACKET WITH DOT
    { 0x2993, 0x2994, MLineBreaker::EOpLineBreakClass },  //LEFT ARC LESS-THAN BRACKET
    { 0x2994, 0x2995, MLineBreaker::EClLineBreakClass },  //RIGHT ARC GREATER-THAN BRACKET
    { 0x2995, 0x2996, MLineBreaker::EOpLineBreakClass },  //DOUBLE LEFT ARC GREATER-THAN BRACKET
    { 0x2996, 0x2997, MLineBreaker::EClLineBreakClass },  //DOUBLE RIGHT ARC LESS-THAN BRACKET
    { 0x2997, 0x2998, MLineBreaker::EOpLineBreakClass },  //LEFT BLACK TORTOISE SHELL BRACKET
    { 0x2998, 0x2999, MLineBreaker::EClLineBreakClass },  //RIGHT BLACK TORTOISE SHELL BRACKET
    { 0x2999, 0x29d8, MLineBreaker::EAlLineBreakClass },  //DOTTED FENCE
    { 0x29d8, 0x29d9, MLineBreaker::EOpLineBreakClass },  //LEFT WIGGLY FENCE
    { 0x29d9, 0x29da, MLineBreaker::EClLineBreakClass },  //RIGHT WIGGLY FENCE
    { 0x29da, 0x29db, MLineBreaker::EOpLineBreakClass },  //LEFT DOUBLE WIGGLY FENCE
    { 0x29db, 0x29dc, MLineBreaker::EClLineBreakClass },  //RIGHT DOUBLE WIGGLY FENCE
    { 0x29dc, 0x29fc, MLineBreaker::EAlLineBreakClass },  //INCOMPLETE INFINITY
    { 0x29fc, 0x29fd, MLineBreaker::EOpLineBreakClass },  //LEFT-POINTING CURVED ANGLE BRACKET
    { 0x29fd, 0x29fe, MLineBreaker::EClLineBreakClass },  //RIGHT-POINTING CURVED ANGLE BRACKET
    { 0x29fe, 0x2b1b, MLineBreaker::EAlLineBreakClass },  //TINY
    { 0x2b20, 0x2b24, MLineBreaker::EAlLineBreakClass },  //WHITE PENTAGON
    { 0x2c00, 0x2c2f, MLineBreaker::EAlLineBreakClass },  //GLAGOLITIC CAPITAL LETTER AZU
    { 0x2c30, 0x2c5f, MLineBreaker::EAlLineBreakClass },  //GLAGOLITIC SMALL LETTER AZU
    { 0x2c60, 0x2c6d, MLineBreaker::EAlLineBreakClass },  //LATIN CAPITAL LETTER L WITH DOUBLE BAR
    { 0x2c74, 0x2c78, MLineBreaker::EAlLineBreakClass },  //LATIN SMALL LETTER V WITH CURL
    { 0x2c80, 0x2ceb, MLineBreaker::EAlLineBreakClass },  //COPTIC CAPITAL LETTER ALFA
    { 0x2cf9, 0x2cfd, MLineBreaker::EBaLineBreakClass },  //COPTIC OLD NUBIAN FULL STOP
    { 0x2cfd, 0x2cfe, MLineBreaker::EAlLineBreakClass },  //COPTIC FRACTION ONE HALF
    { 0x2cfe, 0x2d00, MLineBreaker::EBaLineBreakClass },  //COPTIC FULL STOP
    { 0x2d00, 0x2d26, MLineBreaker::EAlLineBreakClass },  //GEORGIAN SMALL LETTER AN
    { 0x2d30, 0x2d66, MLineBreaker::EAlLineBreakClass },  //TIFINAGH LETTER YA
    { 0x2d6f, 0x2d70, MLineBreaker::EAlLineBreakClass },  //TIFINAGH MODIFIER LETTER LABIALIZATION MARK
    { 0x2d80, 0x2d97, MLineBreaker::EAlLineBreakClass },  //ETHIOPIC SYLLABLE LOA
    { 0x2da0, 0x2da7, MLineBreaker::EAlLineBreakClass },  //ETHIOPIC SYLLABLE SSA
    { 0x2da8, 0x2daf, MLineBreaker::EAlLineBreakClass },  //ETHIOPIC SYLLABLE CCA
    { 0x2db0, 0x2db7, MLineBreaker::EAlLineBreakClass },  //ETHIOPIC SYLLABLE ZZA
    { 0x2db8, 0x2dbf, MLineBreaker::EAlLineBreakClass },  //ETHIOPIC SYLLABLE CCHA
    { 0x2dc0, 0x2dc7, MLineBreaker::EAlLineBreakClass },  //ETHIOPIC SYLLABLE QYA
    { 0x2dc8, 0x2dcf, MLineBreaker::EAlLineBreakClass },  //ETHIOPIC SYLLABLE KYA
    { 0x2dd0, 0x2dd7, MLineBreaker::EAlLineBreakClass },  //ETHIOPIC SYLLABLE XYA
    { 0x2dd8, 0x2ddf, MLineBreaker::EAlLineBreakClass },  //ETHIOPIC SYLLABLE GYA
    { 0x2e00, 0x2e0e, MLineBreaker::EQuLineBreakClass },  //RIGHT ANGLE SUBSTITUTION MARKER
    { 0x2e0e, 0x2e16, MLineBreaker::EBaLineBreakClass },  //EDITORIAL CORONIS
    { 0x2e16, 0x2e17, MLineBreaker::EAlLineBreakClass },  //DOTTED RIGHT-POINTING ANGLE
    { 0x2e17, 0x2e18, MLineBreaker::EBaLineBreakClass },  //DOUBLE OBLIQUE HYPHEN
    { 0x2e1c, 0x2e1e, MLineBreaker::EQuLineBreakClass },  //LEFT LOW PARAPHRASE BRACKET
    { 0x2e80, 0x2e9a, MLineBreaker::EIdLineBreakClass },  //cjk radical repeat
    { 0x2e9b, 0x2ef4, MLineBreaker::EIdLineBreakClass },  //cjk radical choke
    { 0x2f00, 0x2fd6, MLineBreaker::EIdLineBreakClass },  //kangxi radical one
    { 0x2ff0, 0x2ffc, MLineBreaker::EIdLineBreakClass },  //ideographic description character left to right
    { 0x3000, 0x3001, MLineBreaker::EIdLineBreakClass },  //ideographic space
    { 0x3001, 0x3003, MLineBreaker::EClLineBreakClass },  //ideographic comma
    { 0x3003, 0x3005, MLineBreaker::EIdLineBreakClass },  //ditto mark
    { 0x3005, 0x3006, MLineBreaker::ENsLineBreakClass },  //ideographic iteration mark
    { 0x3006, 0x3008, MLineBreaker::EIdLineBreakClass },  //ideographic closing mark
    { 0x3008, 0x3009, MLineBreaker::EOpLineBreakClass },  //left angle bracket
    { 0x3009, 0x300a, MLineBreaker::EClLineBreakClass },  //right angle bracket
    { 0x300a, 0x300b, MLineBreaker::EOpLineBreakClass },  //left double angle bracket
    { 0x300b, 0x300c, MLineBreaker::EClLineBreakClass },  //right double angle bracket
    { 0x300c, 0x300d, MLineBreaker::EOpLineBreakClass },  //left corner bracket
    { 0x300d, 0x300e, MLineBreaker::EClLineBreakClass },  //right corner bracket
    { 0x300e, 0x300f, MLineBreaker::EOpLineBreakClass },  //left white corner bracket
    { 0x300f, 0x3010, MLineBreaker::EClLineBreakClass },  //right white corner bracket
    { 0x3010, 0x3011, MLineBreaker::EOpLineBreakClass },  //left black lenticular bracket
    { 0x3011, 0x3012, MLineBreaker::EClLineBreakClass },  //right black lenticular bracket
    { 0x3012, 0x3014, MLineBreaker::EIdLineBreakClass },  //postal mark
    { 0x3014, 0x3015, MLineBreaker::EOpLineBreakClass },  //left tortoise shell bracket
    { 0x3015, 0x3016, MLineBreaker::EClLineBreakClass },  //right tortoise shell bracket
    { 0x3016, 0x3017, MLineBreaker::EOpLineBreakClass },  //left white lenticular bracket
    { 0x3017, 0x3018, MLineBreaker::EClLineBreakClass },  //right white lenticular bracket
    { 0x3018, 0x3019, MLineBreaker::EOpLineBreakClass },  //left white tortoise shell bracket
    { 0x3019, 0x301a, MLineBreaker::EClLineBreakClass },  //right white tortoise shell bracket
    { 0x301a, 0x301b, MLineBreaker::EOpLineBreakClass },  //left white square bracket
    { 0x301b, 0x301c, MLineBreaker::EClLineBreakClass },  //right white square bracket
    { 0x301c, 0x301d, MLineBreaker::ENsLineBreakClass },  //wave dash
    { 0x301d, 0x301e, MLineBreaker::EOpLineBreakClass },  //reversed double prime quotation mark
    { 0x301e, 0x3020, MLineBreaker::EClLineBreakClass },  //double prime quotation mark
    { 0x3020, 0x302a, MLineBreaker::EIdLineBreakClass },  //postal mark face
    { 0x302a, 0x3030, MLineBreaker::ECmLineBreakClass },  //ideographic level tone mark
    { 0x3030, 0x303b, MLineBreaker::EIdLineBreakClass },  //wavy dash
    { 0x303b, 0x303d, MLineBreaker::ENsLineBreakClass },  //VERTICAL IDEOGRAPHIC ITERATION MARK
    { 0x303d, 0x3040, MLineBreaker::EIdLineBreakClass },  //PART ALTERNATION MARK
    { 0x3041, 0x3042, MLineBreaker::ENsLineBreakClass },  //hiragana letter small a
    { 0x3042, 0x3043, MLineBreaker::EIdLineBreakClass },  //hiragana letter a
    { 0x3043, 0x3044, MLineBreaker::ENsLineBreakClass },  //hiragana letter small i
    { 0x3044, 0x3045, MLineBreaker::EIdLineBreakClass },  //hiragana letter i
    { 0x3045, 0x3046, MLineBreaker::ENsLineBreakClass },  //hiragana letter small u
    { 0x3046, 0x3047, MLineBreaker::EIdLineBreakClass },  //hiragana letter u
    { 0x3047, 0x3048, MLineBreaker::ENsLineBreakClass },  //hiragana letter small e
    { 0x3048, 0x3049, MLineBreaker::EIdLineBreakClass },  //hiragana letter e
    { 0x3049, 0x304a, MLineBreaker::ENsLineBreakClass },  //hiragana letter small o
    { 0x304a, 0x3063, MLineBreaker::EIdLineBreakClass },  //hiragana letter o
    { 0x3063, 0x3064, MLineBreaker::ENsLineBreakClass },  //hiragana letter small tu
    { 0x3064, 0x3083, MLineBreaker::EIdLineBreakClass },  //hiragana letter tu
    { 0x3083, 0x3084, MLineBreaker::ENsLineBreakClass },  //hiragana letter small ya
    { 0x3084, 0x3085, MLineBreaker::EIdLineBreakClass },  //hiragana letter ya
    { 0x3085, 0x3086, MLineBreaker::ENsLineBreakClass },  //hiragana letter small yu
    { 0x3086, 0x3087, MLineBreaker::EIdLineBreakClass },  //hiragana letter yu
    { 0x3087, 0x3088, MLineBreaker::ENsLineBreakClass },  //hiragana letter small yo
    { 0x3088, 0x308e, MLineBreaker::EIdLineBreakClass },  //hiragana letter yo
    { 0x308e, 0x308f, MLineBreaker::ENsLineBreakClass },  //hiragana letter small wa
    { 0x308f, 0x3095, MLineBreaker::EIdLineBreakClass },  //hiragana letter wa
    { 0x3095, 0x3097, MLineBreaker::ENsLineBreakClass },  //HIRAGANA LETTER SMALL KA
    { 0x3099, 0x309b, MLineBreaker::ECmLineBreakClass },  //combining katakana-hiragana voiced sound mark
    { 0x309b, 0x309f, MLineBreaker::ENsLineBreakClass },  //katakana-hiragana voiced sound mark
    { 0x309f, 0x30a0, MLineBreaker::EIdLineBreakClass },  //HIRAGANA DIGRAPH YORI
    { 0x30a0, 0x30a1, MLineBreaker::ENsLineBreakClass },  //KATAKANA-HIRAGANA DOUBLE HYPHEN
    { 0x30a1, 0x30a2, MLineBreaker::ENsLineBreakClass },  //katakana letter small a
    { 0x30a2, 0x30a3, MLineBreaker::EIdLineBreakClass },  //katakana letter a
    { 0x30a3, 0x30a4, MLineBreaker::ENsLineBreakClass },  //katakana letter small i
    { 0x30a4, 0x30a5, MLineBreaker::EIdLineBreakClass },  //katakana letter i
    { 0x30a5, 0x30a6, MLineBreaker::ENsLineBreakClass },  //katakana letter small u
    { 0x30a6, 0x30a7, MLineBreaker::EIdLineBreakClass },  //katakana letter u
    { 0x30a7, 0x30a8, MLineBreaker::ENsLineBreakClass },  //katakana letter small e
    { 0x30a8, 0x30a9, MLineBreaker::EIdLineBreakClass },  //katakana letter e
    { 0x30a9, 0x30aa, MLineBreaker::ENsLineBreakClass },  //katakana letter small o
    { 0x30aa, 0x30c3, MLineBreaker::EIdLineBreakClass },  //katakana letter o
    { 0x30c3, 0x30c4, MLineBreaker::ENsLineBreakClass },  //katakana letter small tu
    { 0x30c4, 0x30e3, MLineBreaker::EIdLineBreakClass },  //katakana letter tu
    { 0x30e3, 0x30e4, MLineBreaker::ENsLineBreakClass },  //katakana letter small ya
    { 0x30e4, 0x30e5, MLineBreaker::EIdLineBreakClass },  //katakana letter ya
    { 0x30e5, 0x30e6, MLineBreaker::ENsLineBreakClass },  //katakana letter small yu
    { 0x30e6, 0x30e7, MLineBreaker::EIdLineBreakClass },  //katakana letter yu
    { 0x30e7, 0x30e8, MLineBreaker::ENsLineBreakClass },  //katakana letter small yo
    { 0x30e8, 0x30ee, MLineBreaker::EIdLineBreakClass },  //katakana letter yo
    { 0x30ee, 0x30ef, MLineBreaker::ENsLineBreakClass },  //katakana letter small wa
    { 0x30ef, 0x30f5, MLineBreaker::EIdLineBreakClass },  //katakana letter wa
    { 0x30f5, 0x30f7, MLineBreaker::ENsLineBreakClass },  //katakana letter small ka
    { 0x30f7, 0x30fb, MLineBreaker::EIdLineBreakClass },  //katakana letter va
    { 0x30fb, 0x30fc, MLineBreaker::ENsLineBreakClass },  //katakana middle dot
    { 0x30fc, 0x30fd, MLineBreaker::EIdLineBreakClass },  //katakana-hiragana prolonged sound mark
    { 0x30fd, 0x30fe, MLineBreaker::ENsLineBreakClass },  //katakana iteration mark
    { 0x30fe, 0x30ff, MLineBreaker::EIdLineBreakClass },  //katakana voiced iteration mark
    { 0x30ff, 0x3100, MLineBreaker::EIdLineBreakClass },  //KATAKANA DIGRAPH KOTO
    { 0x3105, 0x312d, MLineBreaker::EIdLineBreakClass },  //bopomofo letter b
    { 0x3131, 0x318f, MLineBreaker::EIdLineBreakClass },  //hangul letter kiyeok
    { 0x3190, 0x31b8, MLineBreaker::EIdLineBreakClass },  //ideographic annotation linking mark
    { 0x31c0, 0x31d0, MLineBreaker::EIdLineBreakClass },  //CJK STROKE T
    { 0x31f0, 0x3200, MLineBreaker::ENsLineBreakClass },  //KATAKANA LETTER SMALL KU
    { 0x3200, 0x321f, MLineBreaker::EIdLineBreakClass },  //parenthesized hangul kiyeok
    { 0x3220, 0x3244, MLineBreaker::EIdLineBreakClass },  //parenthesized ideograph one
    { 0x3250, 0x32ff, MLineBreaker::EIdLineBreakClass },  //PARTNERSHIP SIGN
    { 0x3300, 0x4db6, MLineBreaker::EIdLineBreakClass },  //square apaato
    { 0x4dc0, 0x4e00, MLineBreaker::EAlLineBreakClass },  //HEXAGRAM FOR THE CREATIVE HEAVEN
    { 0x4e00, 0x9fbb, MLineBreaker::EIdLineBreakClass },  //<cjk ideograph,
    { 0xa000, 0xa48d, MLineBreaker::EIdLineBreakClass },  //yi syllable it
    { 0xa490, 0xa4c6, MLineBreaker::EIdLineBreakClass },  //yi radical qot
    { 0xa4c6, 0xa4c7, MLineBreaker::EIdLineBreakClass },  //yi radical ke
    { 0xa700, 0xa71b, MLineBreaker::EAlLineBreakClass },  //MODIFIER LETTER CHINESE TONE YIN PING
    { 0xa720, 0xa722, MLineBreaker::EAlLineBreakClass },  //MODIFIER LETTER STRESS AND HIGH TONE
    { 0xa800, 0xa802, MLineBreaker::EAlLineBreakClass },  //SYLOTI NAGRI LETTER A
    { 0xa802, 0xa803, MLineBreaker::ECmLineBreakClass },  //SYLOTI NAGRI SIGN DVISVARA
    { 0xa803, 0xa806, MLineBreaker::EAlLineBreakClass },  //SYLOTI NAGRI LETTER U
    { 0xa806, 0xa807, MLineBreaker::ECmLineBreakClass },  //SYLOTI NAGRI SIGN HASANTA
    { 0xa807, 0xa80b, MLineBreaker::EAlLineBreakClass },  //SYLOTI NAGRI LETTER KO
    { 0xa80b, 0xa80c, MLineBreaker::ECmLineBreakClass },  //SYLOTI NAGRI SIGN ANUSVARA
    { 0xa80c, 0xa823, MLineBreaker::EAlLineBreakClass },  //SYLOTI NAGRI LETTER CO
    { 0xa823, 0xa828, MLineBreaker::ECmLineBreakClass },  //SYLOTI NAGRI VOWEL SIGN A
    { 0xa828, 0xa82c, MLineBreaker::EAlLineBreakClass },  //SYLOTI NAGRI POETRY MARK-1
    { 0xa840, 0xa874, MLineBreaker::EAlLineBreakClass },  //PHAGS-PA LETTER KA
    { 0xa874, 0xa876, MLineBreaker::EBbLineBreakClass },  //PHAGS-PA SINGLE HEAD MARK
    { 0xa876, 0xa878, MLineBreaker::EExLineBreakClass },  //PHAGS-PA MARK SHAD
    { 0xac00, 0xd7a4, MLineBreaker::EIdLineBreakClass },  //<hangul syllable,
    { 0xd800, 0xe000, MLineBreaker::ESgLineBreakClass },  //<non private use high surrogate,
    { 0xe000, 0xf900, MLineBreaker::EAiLineBreakClass },  //<private use,
    { 0xf900, 0xfa2e, MLineBreaker::EIdLineBreakClass },  //cjk compatibility ideograph-f900
    { 0xfa30, 0xfa6b, MLineBreaker::EIdLineBreakClass },  //CJK COMPATIBILITY IDEOGRAPH-FA30
    { 0xfa70, 0xfada, MLineBreaker::EIdLineBreakClass },  //CJK COMPATIBILITY IDEOGRAPH-FA70
    { 0xfb00, 0xfb07, MLineBreaker::EAlLineBreakClass },  //latin small ligature ff
    { 0xfb13, 0xfb18, MLineBreaker::EAlLineBreakClass },  //armenian small ligature men now
    { 0xfb1d, 0xfb1e, MLineBreaker::EAlLineBreakClass },  //hebrew letter yod with hiriq
    { 0xfb1e, 0xfb1f, MLineBreaker::ECmLineBreakClass },  //hebrew point judeo-spanish varika
    { 0xfb1f, 0xfb37, MLineBreaker::EAlLineBreakClass },  //hebrew ligature yiddish yod yod patah
    { 0xfb38, 0xfb3d, MLineBreaker::EAlLineBreakClass },  //hebrew letter tet with dagesh
    { 0xfb3e, 0xfb3f, MLineBreaker::EAlLineBreakClass },  //hebrew letter mem with dagesh
    { 0xfb40, 0xfb42, MLineBreaker::EAlLineBreakClass },  //hebrew letter nun with dagesh
    { 0xfb43, 0xfb45, MLineBreaker::EAlLineBreakClass },  //hebrew letter final pe with dagesh
    { 0xfb46, 0xfbb2, MLineBreaker::EAlLineBreakClass },  //hebrew letter tsadi with dagesh
    { 0xfbd3, 0xfd3e, MLineBreaker::EAlLineBreakClass },  //arabic letter ng isolated form
    { 0xfd3e, 0xfd3f, MLineBreaker::EOpLineBreakClass },  //ornate left parenthesis
    { 0xfd3f, 0xfd40, MLineBreaker::EClLineBreakClass },  //ornate right parenthesis
    { 0xfd50, 0xfd90, MLineBreaker::EAlLineBreakClass },  //arabic ligature teh with jeem with meem initial form
    { 0xfd92, 0xfdc8, MLineBreaker::EAlLineBreakClass },  //arabic ligature meem with jeem with khah initial form
    { 0xfdf0, 0xfdfc, MLineBreaker::EAlLineBreakClass },  //arabic ligature salla used as koranic stop sign isolated form
    { 0xfdfc, 0xfdfd, MLineBreaker::EPoLineBreakClass },  //RIAL SIGN
    { 0xfdfd, 0xfdfe, MLineBreaker::EAlLineBreakClass },  //ARABIC LIGATURE BISMILLAH AR-RAHMAN AR-RAHEEM
    { 0xfe00, 0xfe10, MLineBreaker::ECmLineBreakClass },  //VARIATION SELECTOR-1
    { 0xfe10, 0xfe11, MLineBreaker::EIsLineBreakClass },  //PRESENTATION FORM FOR VERTICAL COMMA
    { 0xfe11, 0xfe13, MLineBreaker::EClLineBreakClass },  //PRESENTATION FORM FOR VERTICAL IDEOGRAPHIC COMMA
    { 0xfe13, 0xfe15, MLineBreaker::EIsLineBreakClass },  //PRESENTATION FORM FOR VERTICAL COLON
    { 0xfe15, 0xfe17, MLineBreaker::EExLineBreakClass },  //PRESENTATION FORM FOR VERTICAL EXCLAMATION MARK
    { 0xfe17, 0xfe18, MLineBreaker::EOpLineBreakClass },  //PRESENTATION FORM FOR VERTICAL LEFT WHITE LENTICULAR BRACKET
    { 0xfe18, 0xfe19, MLineBreaker::EClLineBreakClass },  //PRESENTATION FORM FOR VERTICAL RIGHT WHITE LENTICULAR BRAKCET
    { 0xfe19, 0xfe1a, MLineBreaker::EInLineBreakClass },  //PRESENTATION FORM FOR VERTICAL HORIZONTAL ELLIPSIS
    { 0xfe20, 0xfe24, MLineBreaker::ECmLineBreakClass },  //combining ligature left half
    { 0xfe30, 0xfe35, MLineBreaker::EIdLineBreakClass },  //presentation form for vertical two dot leader
    { 0xfe35, 0xfe36, MLineBreaker::EOpLineBreakClass },  //presentation form for vertical left parenthesis
    { 0xfe36, 0xfe37, MLineBreaker::EClLineBreakClass },  //presentation form for vertical right parenthesis
    { 0xfe37, 0xfe38, MLineBreaker::EOpLineBreakClass },  //presentation form for vertical left curly bracket
    { 0xfe38, 0xfe39, MLineBreaker::EClLineBreakClass },  //presentation form for vertical right curly bracket
    { 0xfe39, 0xfe3a, MLineBreaker::EOpLineBreakClass },  //presentation form for vertical left tortoise shell bracket
    { 0xfe3a, 0xfe3b, MLineBreaker::EClLineBreakClass },  //presentation form for vertical right tortoise shell bracket
    { 0xfe3b, 0xfe3c, MLineBreaker::EOpLineBreakClass },  //presentation form for vertical left black lenticular bracket
    { 0xfe3c, 0xfe3d, MLineBreaker::EClLineBreakClass },  //presentation form for vertical right black lenticular bracket
    { 0xfe3d, 0xfe3e, MLineBreaker::EOpLineBreakClass },  //presentation form for vertical left double angle bracket
    { 0xfe3e, 0xfe3f, MLineBreaker::EClLineBreakClass },  //presentation form for vertical right double angle bracket
    { 0xfe3f, 0xfe40, MLineBreaker::EOpLineBreakClass },  //presentation form for vertical left angle bracket
    { 0xfe40, 0xfe41, MLineBreaker::EClLineBreakClass },  //presentation form for vertical right angle bracket
    { 0xfe41, 0xfe42, MLineBreaker::EOpLineBreakClass },  //presentation form for vertical left corner bracket
    { 0xfe42, 0xfe43, MLineBreaker::EClLineBreakClass },  //presentation form for vertical right corner bracket
    { 0xfe43, 0xfe44, MLineBreaker::EOpLineBreakClass },  //presentation form for vertical left white corner bracket
    { 0xfe44, 0xfe45, MLineBreaker::EClLineBreakClass },  //presentation form for vertical right white corner bracket
    { 0xfe45, 0xfe47, MLineBreaker::EIdLineBreakClass },  //SESAME DOT
    { 0xfe47, 0xfe48, MLineBreaker::EOpLineBreakClass },  //PRESENTATION FORM FOR VERTICAL LEFT SQUARE BRACKET
    { 0xfe48, 0xfe49, MLineBreaker::EClLineBreakClass },  //PRESENTATION FORM FOR VERTICAL RIGHT SQUARE BRACKET
    { 0xfe49, 0xfe50, MLineBreaker::EIdLineBreakClass },  //dashed overline
    { 0xfe50, 0xfe51, MLineBreaker::EClLineBreakClass },  //small comma
    { 0xfe51, 0xfe52, MLineBreaker::EIdLineBreakClass },  //small ideographic comma
    { 0xfe52, 0xfe53, MLineBreaker::EClLineBreakClass },  //small full stop
    { 0xfe54, 0xfe56, MLineBreaker::ENsLineBreakClass },  //small semicolon
    { 0xfe56, 0xfe58, MLineBreaker::EExLineBreakClass },  //small question mark
    { 0xfe58, 0xfe59, MLineBreaker::EIdLineBreakClass },  //small em dash
    { 0xfe59, 0xfe5a, MLineBreaker::EOpLineBreakClass },  //small left parenthesis
    { 0xfe5a, 0xfe5b, MLineBreaker::EClLineBreakClass },  //small right parenthesis
    { 0xfe5b, 0xfe5c, MLineBreaker::EOpLineBreakClass },  //small left curly bracket
    { 0xfe5c, 0xfe5d, MLineBreaker::EClLineBreakClass },  //small right curly bracket
    { 0xfe5d, 0xfe5e, MLineBreaker::EOpLineBreakClass },  //small left tortoise shell bracket
    { 0xfe5e, 0xfe5f, MLineBreaker::EClLineBreakClass },  //small right tortoise shell bracket
    { 0xfe5f, 0xfe67, MLineBreaker::EIdLineBreakClass },  //small number sign
    { 0xfe68, 0xfe69, MLineBreaker::EIdLineBreakClass },  //small reverse solidus
    { 0xfe69, 0xfe6a, MLineBreaker::EPrLineBreakClass },  //small dollar sign
    { 0xfe6a, 0xfe6b, MLineBreaker::EPoLineBreakClass },  //small percent sign
    { 0xfe6b, 0xfe6c, MLineBreaker::EAlLineBreakClass },  //small commercial at
    { 0xfe70, 0xfe74, MLineBreaker::EAlLineBreakClass },  //arabic fathatan isolated form
    { 0xfe74, 0xfe75, MLineBreaker::EAlLineBreakClass },  //arabic kasratan isolated form
    { 0xfe76, 0xfefd, MLineBreaker::EAlLineBreakClass },  //arabic fatha isolated form
    { 0xfeff, 0xff00, MLineBreaker::EGlLineBreakClass },  //zero width no-break space
    { 0xff01, 0xff02, MLineBreaker::EExLineBreakClass },  //fullwidth exclamation mark
    { 0xff02, 0xff04, MLineBreaker::EIdLineBreakClass },  //fullwidth quotation mark
    { 0xff04, 0xff05, MLineBreaker::EPrLineBreakClass },  //fullwidth dollar sign
    { 0xff05, 0xff06, MLineBreaker::EPoLineBreakClass },  //fullwidth percent sign
    { 0xff06, 0xff08, MLineBreaker::EIdLineBreakClass },  //fullwidth ampersand
    { 0xff08, 0xff09, MLineBreaker::EOpLineBreakClass },  //fullwidth left parenthesis
    { 0xff09, 0xff0a, MLineBreaker::EClLineBreakClass },  //fullwidth right parenthesis
    { 0xff0a, 0xff0c, MLineBreaker::EIdLineBreakClass },  //fullwidth asterisk
    { 0xff0c, 0xff0d, MLineBreaker::EClLineBreakClass },  //fullwidth comma
    { 0xff0d, 0xff0e, MLineBreaker::EIdLineBreakClass },  //fullwidth hyphen-minus
    { 0xff0e, 0xff0f, MLineBreaker::EClLineBreakClass },  //fullwidth full stop
    { 0xff0f, 0xff1a, MLineBreaker::EIdLineBreakClass },  //fullwidth solidus
    { 0xff1a, 0xff1c, MLineBreaker::ENsLineBreakClass },  //fullwidth colon
    { 0xff1c, 0xff1f, MLineBreaker::EIdLineBreakClass },  //fullwidth less-than sign
    { 0xff1f, 0xff20, MLineBreaker::EExLineBreakClass },  //fullwidth question mark
    { 0xff20, 0xff3b, MLineBreaker::EIdLineBreakClass },  //fullwidth commercial at
    { 0xff3b, 0xff3c, MLineBreaker::EOpLineBreakClass },  //fullwidth left square bracket
    { 0xff3c, 0xff3d, MLineBreaker::EIdLineBreakClass },  //fullwidth reverse solidus
    { 0xff3d, 0xff3e, MLineBreaker::EClLineBreakClass },  //fullwidth right square bracket
    { 0xff3e, 0xff5b, MLineBreaker::EIdLineBreakClass },  //fullwidth circumflex accent
    { 0xff5b, 0xff5c, MLineBreaker::EOpLineBreakClass },  //fullwidth left curly bracket
    { 0xff5c, 0xff5d, MLineBreaker::EIdLineBreakClass },  //fullwidth vertical line
    { 0xff5d, 0xff5e, MLineBreaker::EClLineBreakClass },  //fullwidth right curly bracket
    { 0xff5e, 0xff5f, MLineBreaker::EIdLineBreakClass },  //fullwidth tilde
    { 0xff5f, 0xff60, MLineBreaker::EOpLineBreakClass },  //FULLWIDTH LEFT WHITE PARENTHESIS
    { 0xff60, 0xff61, MLineBreaker::EClLineBreakClass },  //FULLWIDTH RIGHT WHITE PARENTHESIS
    { 0xff61, 0xff62, MLineBreaker::EClLineBreakClass },  //halfwidth ideographic full stop
    { 0xff62, 0xff63, MLineBreaker::EOpLineBreakClass },  //halfwidth left corner bracket
    { 0xff63, 0xff65, MLineBreaker::EClLineBreakClass },  //halfwidth right corner bracket
    { 0xff65, 0xff66, MLineBreaker::ENsLineBreakClass },  //halfwidth katakana middle dot
    { 0xff66, 0xff67, MLineBreaker::EAlLineBreakClass },  //halfwidth katakana letter wo
    { 0xff67, 0xff71, MLineBreaker::ENsLineBreakClass },  //halfwidth katakana letter small a
    { 0xff71, 0xff9e, MLineBreaker::EAlLineBreakClass },  //halfwidth katakana letter a
    { 0xff9e, 0xffa0, MLineBreaker::ENsLineBreakClass },  //halfwidth katakana voiced sound mark
    { 0xffa0, 0xffbf, MLineBreaker::EAlLineBreakClass },  //halfwidth hangul filler
    { 0xffc2, 0xffc8, MLineBreaker::EAlLineBreakClass },  //halfwidth hangul letter a
    { 0xffca, 0xffd0, MLineBreaker::EAlLineBreakClass },  //halfwidth hangul letter yeo
    { 0xffd2, 0xffd8, MLineBreaker::EAlLineBreakClass },  //halfwidth hangul letter yo
    { 0xffda, 0xffdd, MLineBreaker::EAlLineBreakClass },  //halfwidth hangul letter eu
    { 0xffe0, 0xffe1, MLineBreaker::EPoLineBreakClass },  //fullwidth cent sign
    { 0xffe1, 0xffe2, MLineBreaker::EPrLineBreakClass },  //fullwidth pound sign
    { 0xffe2, 0xffe5, MLineBreaker::EIdLineBreakClass },  //fullwidth not sign
    { 0xffe5, 0xffe7, MLineBreaker::EPrLineBreakClass },  //fullwidth yen sign
    { 0xffe8, 0xffef, MLineBreaker::EAlLineBreakClass },  //halfwidth forms light vertical
    { 0xfff9, 0xfffc, MLineBreaker::ECmLineBreakClass },  //interlinear annotation anchor
    { 0xfffc, 0xfffd, MLineBreaker::ECbLineBreakClass },  //object replacement character
    { 0xfffd, 0xfffe, MLineBreaker::EAiLineBreakClass },  //replacement character
    { 0x10000, 0x1000c, MLineBreaker::EAlLineBreakClass },  //LINEAR B SYLLABLE B008 A
    { 0x1000d, 0x10027, MLineBreaker::EAlLineBreakClass },  //LINEAR B SYLLABLE B036 JO
    { 0x10028, 0x1003b, MLineBreaker::EAlLineBreakClass },  //LINEAR B SYLLABLE B060 RA
    { 0x1003c, 0x1003e, MLineBreaker::EAlLineBreakClass },  //LINEAR B SYLLABLE B017 ZA
    { 0x1003f, 0x1004e, MLineBreaker::EAlLineBreakClass },  //LINEAR B SYLLABLE B020 ZO
    { 0x10050, 0x1005e, MLineBreaker::EAlLineBreakClass },  //LINEAR B SYMBOL B018
    { 0x10080, 0x100fb, MLineBreaker::EAlLineBreakClass },  //LINEAR B IDEOGRAM B100 MAN
    { 0x10100, 0x10103, MLineBreaker::EBaLineBreakClass },  //AEGEAN WORD SEPARATOR LINE
    { 0x10107, 0x10134, MLineBreaker::EAlLineBreakClass },  //AEGEAN NUMBER ONE
    { 0x10137, 0x1018b, MLineBreaker::EAlLineBreakClass },  //AEGEAN WEIGHT BASE UNIT
    { 0x10300, 0x1031f, MLineBreaker::EAlLineBreakClass },  //OLD ITALIC LETTER A
    { 0x10320, 0x10324, MLineBreaker::EAlLineBreakClass },  //OLD ITALIC NUMERAL ONE
    { 0x10330, 0x1034b, MLineBreaker::EAlLineBreakClass },  //GOTHIC LETTER AHSA
    { 0x10380, 0x1039e, MLineBreaker::EAlLineBreakClass },  //UGARITIC LETTER ALPA
    { 0x1039f, 0x103a0, MLineBreaker::EBaLineBreakClass },  //UGARITIC WORD DIVIDER
    { 0x103a0, 0x103c4, MLineBreaker::EAlLineBreakClass },  //OLD PERSIAN SIGN A
    { 0x103c8, 0x103d0, MLineBreaker::EAlLineBreakClass },  //OLD PERSIAN SIGN AURAMAZDAA
    { 0x103d0, 0x103d1, MLineBreaker::EBaLineBreakClass },  //OLD PERSIAN WORD DIVIDER
    { 0x103d1, 0x103d6, MLineBreaker::EAlLineBreakClass },  //OLD PERSIAN NUMBER ONE
    { 0x10400, 0x1049e, MLineBreaker::EAlLineBreakClass },  //DESERET CAPITAL LETTER LONG I
    { 0x104a0, 0x104aa, MLineBreaker::ENuLineBreakClass },  //OSMANYA DIGIT ZERO
    { 0x10800, 0x10806, MLineBreaker::EAlLineBreakClass },  //CYPRIOT SYLLABLE A
    { 0x10808, 0x10809, MLineBreaker::EAlLineBreakClass },  //CYPRIOT SYLLABLE JO
    { 0x1080a, 0x10836, MLineBreaker::EAlLineBreakClass },  //CYPRIOT SYLLABLE KA
    { 0x10837, 0x10839, MLineBreaker::EAlLineBreakClass },  //CYPRIOT SYLLABLE XA
    { 0x1083c, 0x1083d, MLineBreaker::EAlLineBreakClass },  //CYPRIOT SYLLABLE ZA
    { 0x1083f, 0x10840, MLineBreaker::EAlLineBreakClass },  //CYPRIOT SYLLABLE ZO
    { 0x10900, 0x1091a, MLineBreaker::EAlLineBreakClass },  //PHOENICIAN LETTER ALF
    { 0x1091f, 0x10920, MLineBreaker::EBaLineBreakClass },  //PHOENICIAN WORD SEPARATOR
    { 0x10a00, 0x10a01, MLineBreaker::EAlLineBreakClass },  //KHAROSHTHI LETTER A
    { 0x10a01, 0x10a04, MLineBreaker::ECmLineBreakClass },  //KHAROSHTHI VOWEL SIGN I
    { 0x10a05, 0x10a07, MLineBreaker::ECmLineBreakClass },  //KHAROSHTHI VOWEL SIGN E
    { 0x10a0c, 0x10a10, MLineBreaker::ECmLineBreakClass },  //KHAROSHTHI VOWEL LENGTH MARK
    { 0x10a10, 0x10a14, MLineBreaker::EAlLineBreakClass },  //KHAROSHTHI LETTER KA
    { 0x10a15, 0x10a18, MLineBreaker::EAlLineBreakClass },  //KHAROSHTHI LETTER CA
    { 0x10a19, 0x10a34, MLineBreaker::EAlLineBreakClass },  //KHAROSHTHI LETTER NYA
    { 0x10a38, 0x10a3b, MLineBreaker::ECmLineBreakClass },  //KHAROSHTHI SIGN BAR ABOVE
    { 0x10a3f, 0x10a40, MLineBreaker::ECmLineBreakClass },  //KHAROSHTHI VIRAMA
    { 0x10a40, 0x10a48, MLineBreaker::EAlLineBreakClass },  //KHAROSHTHI DIGIT ONE
    { 0x10a50, 0x10a58, MLineBreaker::EBaLineBreakClass },  //KHAROSHTHI PUNCTUATION DOT
    { 0x10a58, 0x10a59, MLineBreaker::EAlLineBreakClass },  //KHAROSHTHI PUNCTUATION LINES
    { 0x12000, 0x1236f, MLineBreaker::EAlLineBreakClass },  //CUNEIFORM SIGN A
    { 0x12400, 0x12463, MLineBreaker::EAlLineBreakClass },  //CUNEIFORM NUMERIC SIGN TWO ASH
    { 0x12470, 0x12474, MLineBreaker::EBaLineBreakClass },  //CUNEIFORM PUNCTUATION SIGN OLD ASSYRIAN WORD DIVIDER
    { 0x1d000, 0x1d0f6, MLineBreaker::EAlLineBreakClass },  //BYZANTINE MUSICAL SYMBOL PSILI
    { 0x1d100, 0x1d127, MLineBreaker::EAlLineBreakClass },  //MUSICAL SYMBOL SINGLE BARLINE
    { 0x1d12a, 0x1d165, MLineBreaker::EAlLineBreakClass },  //MUSICAL SYMBOL DOUBLE SHARP
    { 0x1d165, 0x1d16a, MLineBreaker::ECmLineBreakClass },  //MUSICAL SYMBOL COMBINING STEM
    { 0x1d16a, 0x1d16d, MLineBreaker::EAlLineBreakClass },  //MUSICAL SYMBOL FINGERED TREMOLO-1
    { 0x1d16d, 0x1d183, MLineBreaker::ECmLineBreakClass },  //MUSICAL SYMBOL COMBINING AUGMENTATION DOT
    { 0x1d183, 0x1d185, MLineBreaker::EAlLineBreakClass },  //MUSICAL SYMBOL ARPEGGIATO UP
    { 0x1d185, 0x1d18c, MLineBreaker::ECmLineBreakClass },  //MUSICAL SYMBOL COMBINING DOIT
    { 0x1d18c, 0x1d1aa, MLineBreaker::EAlLineBreakClass },  //MUSICAL SYMBOL RINFORZANDO
    { 0x1d1aa, 0x1d1ae, MLineBreaker::ECmLineBreakClass },  //MUSICAL SYMBOL COMBINING DOWN BOW
    { 0x1d1ae, 0x1d1de, MLineBreaker::EAlLineBreakClass },  //MUSICAL SYMBOL PEDAL MARK
    { 0x1d200, 0x1d242, MLineBreaker::EAlLineBreakClass },  //GREEK VOCAL NOTATION SYMBOL-1
    { 0x1d242, 0x1d245, MLineBreaker::ECmLineBreakClass },  //COMBINING GREEK MUSICAL TRISEME
    { 0x1d245, 0x1d246, MLineBreaker::EAlLineBreakClass },  //GREEK MUSICAL LEIMMA
    { 0x1d300, 0x1d357, MLineBreaker::EAlLineBreakClass },  //MONOGRAM FOR EARTH
    { 0x1d360, 0x1d372, MLineBreaker::EAlLineBreakClass },  //COUNTING ROD UNIT DIGIT ONE
    { 0x1d400, 0x1d455, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL BOLD CAPITAL A
    { 0x1d456, 0x1d49d, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL ITALIC SMALL I
    { 0x1d49e, 0x1d4a0, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL SCRIPT CAPITAL C
    { 0x1d4a2, 0x1d4a3, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL SCRIPT CAPITAL G
    { 0x1d4a5, 0x1d4a7, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL SCRIPT CAPITAL J
    { 0x1d4a9, 0x1d4ad, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL SCRIPT CAPITAL N
    { 0x1d4ae, 0x1d4ba, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL SCRIPT CAPITAL S
    { 0x1d4bb, 0x1d4bc, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL SCRIPT SMALL F
    { 0x1d4bd, 0x1d4c4, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL SCRIPT SMALL H
    { 0x1d4c5, 0x1d506, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL SCRIPT SMALL P
    { 0x1d507, 0x1d50b, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL FRAKTUR CAPITAL D
    { 0x1d50d, 0x1d515, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL FRAKTUR CAPITAL J
    { 0x1d516, 0x1d51d, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL FRAKTUR CAPITAL S
    { 0x1d51e, 0x1d53a, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL FRAKTUR SMALL A
    { 0x1d53b, 0x1d53f, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL DOUBLE-STRUCK CAPITAL D
    { 0x1d540, 0x1d545, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL DOUBLE-STRUCK CAPITAL I
    { 0x1d546, 0x1d547, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL DOUBLE-STRUCK CAPITAL O
    { 0x1d54a, 0x1d551, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL DOUBLE-STRUCK CAPITAL S
    { 0x1d552, 0x1d6a6, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL DOUBLE-STRUCK SMALL A
    { 0x1d6a8, 0x1d7cc, MLineBreaker::EAlLineBreakClass },  //MATHEMATICAL BOLD CAPITAL ALPHA
    { 0x1d7ce, 0x1d800, MLineBreaker::ENuLineBreakClass },  //MATHEMATICAL BOLD DIGIT ZERO
    { 0x20000, 0x2a6d6, MLineBreaker::EIdLineBreakClass },  //<CJK Ideograph Extension B, First>..<CJK Ideograph Extension B, Last>
    { 0x2f800, 0x2fa1e, MLineBreaker::EIdLineBreakClass },  //CJK COMPATIBILITY IDEOGRAPH-2F800
    { 0xe0001, 0xe0002, MLineBreaker::ECmLineBreakClass },  //LANGUAGE TAG
    { 0xe0020, 0xe0080, MLineBreaker::ECmLineBreakClass },  //TAG SPACE
    { 0xe0100, 0xe01f0, MLineBreaker::ECmLineBreakClass },  //VARIATION SELECTOR-17
    { 0xf0000, 0xffffd, MLineBreaker::EXxLineBreakClass },  //<Plane 15 Private Use, First>..<Plane 15 Private Use, Last>
    { 0x100000, 0x10fffd, MLineBreaker::EXxLineBreakClass },  //<Plane 16 Private Use, First>..<Plane 16 Private Use, Last>
	};

static const TInt TheLineBreakRanges = sizeof(TheLineBreakRangeTable) / sizeof(TheLineBreakRangeTable[0]);

void TestLineBreakTables(void)
	{
#ifdef _DEBUG
	TUint prev_end = 0;
	for (int i = 0; i < TheLineBreakRanges; i++)
		{
		const TLineBreakRange& r = TheLineBreakRangeTable[i];
		if (r.iStart >= r.iEnd || r.iClass >= MLineBreaker::ELineBreakClasses || r.iStart < prev_end)
			Panic(ELineBreakPanic_BadTable);
		prev_end = r.iEnd;
		}
#endif
	}

TUint TLineBreakClassCache::LineBreakClass(TUint aChar)
	{
	if (aChar >= iStart && aChar < iEnd)
		return iClass;
	TUint new_start, new_end;
	TUint new_class = iBreaker.LineBreakClass(aChar,new_start,new_end);
	if (new_start < new_end - 1)
		{
		iStart = new_start;
		iEnd = new_end;
		iClass = new_class;
		}
	return new_class;
	}

EXPORT_C TBool MLineBreaker::LineBreakPossible(
	TUint aPrevClass, TUint aNextClass, TBool aHaveSpaces) const
/** Returns whether line breaks are legal between a character of line breaking 
class aPrevClass and aNextClass, with intervening spaces or not as determined 
by aHaveSpaces. 

@param aPrevClass The class before the potential line break.
@param aNextClass The class after the potential line break.
@param aHaveSpaces Whether spaces separate the characters.
@return ETrue if and only if a line break is legal in the situation described. */
	{
	if (aPrevClass >= ELineBreakClasses || aNextClass >= ELineBreakClasses)
		return FALSE;
	const TLineBreakRule& rule = TheLineBreakRuleTable[aPrevClass];
	TUint flag = 1 << aNextClass;
	if (rule.iForbid & flag)
		return FALSE;
	return aHaveSpaces || (rule.iAllow & flag);
	}

EXPORT_C TUint MLineBreaker::LineBreakClass(TUint aCode,TUint& aRangeStart,TUint& aRangeEnd) const
/** Converts Unicode character into line breaking class. 

@param aCode Input Unicode value.
@param aRangeStart Returns the start of the contiguous range of characters 
including aCode that have the returned line breaking class.
@param aRangeEnd Returns the end of the contiguous range of characters including 
aCode that have the returned line breaking class.
@return The line breaking class for aCode. */
	{
	aRangeStart = aCode;
	aRangeEnd = aCode + 1;

	// First check for a space; this needs to be fast.
	if (aCode == 0x0020)
		return ESpLineBreakClass;

	// Now search the table.
	const TLineBreakRange* base = TheLineBreakRangeTable;
	const TLineBreakRange* end = base + TheLineBreakRanges;
	while (base < end)
		{
		int n = end - base;
		const TLineBreakRange* r = &base[n / 2];
		if (r->iStart > aCode)
			end = r;
		else if (r->iEnd <= aCode)
			base = r + 1;
		else
			{
			aRangeStart = r->iStart;
			aRangeEnd = r->iEnd;
			return r->iClass;
			}
		}

	// If the character was not in the table determine the line break class using the Unicode character category.
	TChar::TCategory cat = TChar(aCode).GetCategory();
	switch (cat)
		{
		case TChar::ELuCategory:
		case TChar::ELlCategory:
		case TChar::ELtCategory:
		case TChar::ELoCategory:
		case TChar::ESmCategory:
		case TChar::ESoCategory:
			return EAlLineBreakClass;

		case TChar::ESkCategory:
			if (TChar(aCode).GetCjkWidth() == TChar::EWide)
				return ENsLineBreakClass;
			else
				return EAlLineBreakClass;

		case TChar::ELmCategory:
			{
			TChar::TCjkWidth cjk_width = TChar(aCode).GetCjkWidth();
			if (cjk_width == TChar::EWide || cjk_width == TChar::EHalfWidth)
				return ENsLineBreakClass;
			}
			break;

		case TChar::EMnCategory:
		case TChar::EMcCategory:
		case TChar::EMeCategory:
		case TChar::ECcCategory:
		case TChar::ECfCategory:
			return ECmLineBreakClass;

		case TChar::ENdCategory:
			return ENuLineBreakClass;

		case TChar::EPsCategory:
			return EOpLineBreakClass;

		case TChar::EPeCategory:
			return EClLineBreakClass;

		case TChar::EScCategory:
			return EPrLineBreakClass;

		case TChar::EPfCategory:
		case TChar::EPiCategory:
			return EQuLineBreakClass;

		case TChar::ECsCategory:
			return ESgLineBreakClass;

		default:
			break;
		}

	// If it has still not been found it may be an ideographic character not covered above.
	if (aCode >= 0x3000 && aCode <= 0x3FFF)
		return EIdLineBreakClass;

	// Or it may be a surrogate; assume plane 1 is alphabetic, plane 2 is ideographic.
	if (aCode >= 0x10000 && aCode <= 0x1FFFF)
		return EAlLineBreakClass;
	if (aCode >= 0x20000 && aCode <= 0x2FFFF)
		return EIdLineBreakClass;

	// Return the catch-all class.
	return EXxLineBreakClass;
	}


EXPORT_C TBool MLineBreaker::IsHangingCharacter(TUint) const
/** Returns ETrue if the character value may be positioned outside the margins. 
	
@param aChar Chatacter to test
@return ETrue if the character value may be positioned outside the margins. */
    {
	return EFalse;
	}

TInt MakeSurrogate(TInt aHigh, TInt aLow)
	{
	return (aHigh << 10) + (aLow & 0x3FF)
		+ (0x10000 - 0xD800*0x400);
	}

inline TBool IsSurrogate(TInt aChar)
	{
	return (aChar & 0xF800) == 0xD800;
	}

inline TBool IsHighSurrogate(TInt aChar)
	{
	return (aChar & 0xFC00) == 0xD800;
	}

inline TBool IsLowSurrogate(TInt aChar)
	{
	return (aChar & 0xFC00) == 0xDC00;
	}

/**
Move past the current character and return the next.
@param aPtr
	Pointer to the current character. On return, contains a pointer to the next
	character.
@param aLimit
	Limit of the iteration. For positive aOffset (moving forwards) this should
	be one less than the end of the text. For negative aOffset (moving
	backwards) this should be the start of the text.
@param aOffset
	1 for moving forward, -1 for moving backward.
@return
	Unicode character value or -1 on error.
@internalComponent
*/
TInt MoveTextPtr(const TText*& aPtr, const TText* aLimit, TInt aOffset)
	{
	// if (aPtr - aLimit) has the same sign as aOffset then we are
	// already too far.
	if (0 <= ((aPtr - aLimit) ^ aOffset)
		&& aLimit)
		return -1;
	TInt c = 0;
	for (;;)
		{
		if (aPtr == aLimit)
			return -1;
		aPtr += aOffset;
		TInt lastc = c;
		c = *aPtr;
		if (!IsSurrogate(c))
			return c;
		if (aOffset < 0)
			{
			if (IsHighSurrogate(c) && IsLowSurrogate(lastc))
				return MakeSurrogate(c, lastc);
			}
		else
			{
			if (IsHighSurrogate(lastc) && IsLowSurrogate(c))
				{
				// found a surrogate pair
				// Set the pointer to the first of the pair
				--aPtr;
				return MakeSurrogate(lastc, c);
				}
			}
		}
	}

/** Find the line break class of the latest character within [aFirst, aP+1]
to have non-CM line break class.
@pre aP points to a character of CM class.
*/
inline TUint GetGlyphClusterLineBreakClass(const TText* aP,
	const TText* aFirst, TLineBreakClassCache& aBreaker)
	{
	TInt ch = MoveTextPtr(aP, aFirst, -1);
	while (0 <= ch)
		{
		TUint cl = aBreaker.LineBreakClass(ch);
		if (cl != MLineBreaker::ECmLineBreakClass)
			return cl;
		ch = MoveTextPtr(aP, aFirst, -1);
		}
	return MLineBreaker::ECmLineBreakClass;
	}

void TLineBreakClassIterator::Set(const TText* aFirst, const TText* aText, TLineBreakClassCache& aBreaker)
	{
	iFirst = aFirst;
	iCurrent = aText;
	TInt c = *aText;
	if (IsHighSurrogate(c))
		{
		if (IsLowSurrogate(aText[1]))
			c = MakeSurrogate(c, aText[1]);
		}
	iClass = aBreaker.LineBreakClass(c);
	if (iClass == MLineBreaker::ECmLineBreakClass)
		{
		TUint baseLbCls = GetGlyphClusterLineBreakClass(iCurrent, iFirst, aBreaker);
		if (baseLbCls ==  MLineBreaker::ESaLineBreakClass)
			iClass = MLineBreaker::ESaLineBreakClass;
		}
	}

void TLineBreakClassIterator::SetNull()
	{
	iFirst = iCurrent = 0;
	}

TBool TLineBreakClassIterator::Move(const TText* aLimit,
	const TText* aLimitAfterSpaces, TInt aOffset,
	TBool& aHasSpaces, TLineBreakClassCache& aBreaker)
	{
	TInt c = MoveTextPtr(iCurrent, aLimit, aOffset);
	if (c < 0)
		return EFalse;
	TUint lbClass = aBreaker.LineBreakClass(c);
	if (lbClass == MLineBreaker::ESpLineBreakClass)
		{
		aHasSpaces = ETrue;
		while (lbClass == MLineBreaker::ESpLineBreakClass)
			{
			c = MoveTextPtr(iCurrent, aLimitAfterSpaces, aOffset);
			// We have hit the limit, but there might be more text to search
			// after the spaces.
			if (c < 0)
				{
				iClass = MLineBreaker::ESpLineBreakClass;
				return ETrue;
				}
			lbClass = aBreaker.LineBreakClass(c);
			}
		iClass = lbClass;
		return ETrue;
		}
		
	// Cm class characters taken on their base character's 
	// line break property in the combining character sequence
	// See D14 in Conformance chapter of Unicode 3.0 book.
	// We do this just for Complex content to maintain backwards
	// compatibility otherwise original test cases fail.
	if (lbClass == MLineBreaker::ECmLineBreakClass)
		{
		TUint baseLbCls = GetGlyphClusterLineBreakClass(iCurrent, iFirst, aBreaker);
	    if (baseLbCls ==  MLineBreaker::ESaLineBreakClass)
	        lbClass = MLineBreaker::ESaLineBreakClass;
		}
	    
	aHasSpaces = EFalse;
	if (lbClass == MLineBreaker::ESaLineBreakClass
		&& iClass == MLineBreaker::ESaLineBreakClass)
		{
		// We have at least two SA characters, so we need to find the
		// total extent of the SA block. GetLineBreakInContext will be asked
		// to find the break.
		const TText* p = iCurrent;
		while (0 <= c && lbClass == MLineBreaker::ESaLineBreakClass)
			{
			iCurrent = p;
			c = MoveTextPtr(p, aOffset < 0? iFirst : aLimitAfterSpaces, aOffset);
			lbClass = aBreaker.LineBreakClass(c);
			
			// Cm class characters taken on their base character's 
	        // line break property. See above.
        	if (lbClass == MLineBreaker::ECmLineBreakClass)
        		{
				TUint baseLbCls = GetGlyphClusterLineBreakClass(p, iFirst, aBreaker);
        	    if (baseLbCls ==  MLineBreaker::ESaLineBreakClass)
        	        lbClass = MLineBreaker::ESaLineBreakClass;
        		}
        	if (lbClass == MLineBreaker::EZwLineBreakClass)
   				{
   				iCurrent = p;
   				}
			}
		return ETrue;
		}
	iClass = lbClass;
	return ETrue;
	}

TLineBreakIterator::TLineBreakIterator(TLineBreakClassCache& aBreaker,
	const TText* aText, TInt aLength, TBool aForwards,
	TInt aMinBreakPos, TInt aMaxBreakPos)
	: iBreaker(aBreaker), iText(aText), iTextLength(aLength), iHasSpaces(EFalse)
	{
	if (aMinBreakPos < 1)
		aMinBreakPos = 1;
	// Find the next valid character and see if it can hang
	// over the margin.
	const TText* endText = aText + aLength;
	const TText* max = aText + aMaxBreakPos;
	if (endText <= max && aText < endText)
		{
		// let 'max' point to last character
		max = endText - 1;
		if (IsLowSurrogate(*max))
			{
			ASSERT(aLength > 1);
			--max;
			ASSERT(IsHighSurrogate(*max));
			}
		}
	TInt c = 0xFFFF;
	while (max < endText)
		{
		TInt lastc = c;
		c = *max;
		if ((c & 0xF800) != 0xD800)
			break;
		if ((lastc & 0xFC00) == 0xD800
			&& (c & 0xFC00) == 0xDC00)
			{
			c = MakeSurrogate(lastc, c);
			break;
			}
		++max;
		}
	if (c != 0xFFFF && max + 1 < endText
		&& aBreaker.Breaker().IsHangingCharacter(c))
		// Set max to just beyond hanging character
		++max;
	// till here, 'max' points to the "end" of a character

	// set up the iteration.
	iMinBreakPos = aText + aMinBreakPos;
	iMaxBreakPos = aText + aMaxBreakPos;
	// Increment or Decrement must be called to complete set up.
	if (max < aText + aMinBreakPos)
		{
		// no possible line breaks
		iLimit = 0;
		iAfterBreak.SetNull();
		iBeforeBreak.SetNull();
		}
	else if (aForwards)
		{
		iLimit = max;	// point to the 'end' of the last character (low surrogate if supplementary)
		iLimitAfterSpaces = endText - 1;
		iAfterBreak.Set(iText, aText + aMinBreakPos - 1, aBreaker);
		}
	else
		{
		iLimit = aText + aMinBreakPos - 1;
		iLimitAfterSpaces = iLimit;
		iBeforeBreak.Set(iText, max - 1, aBreaker);
		iBeforeBreak.Move(endText - 1, endText - 1, 1, iHasSpaces, aBreaker);
		}
	}

TBool TLineBreakIterator::IsBreak(TBool aForwards)
	{
	TInt beforeClass = iBeforeBreak.Class();
	TInt afterClass = iAfterBreak.Class();
	const MLineBreaker& breaker = iBreaker.Breaker();

	// Line break characters overhang the margin
	// along with the spaces that precede them.
	if (afterClass == MLineBreaker::EBkLineBreakClass)
		{
		const TText* p = iAfterBreak.Ptr();
		MoveTextPtr(p, iText + iTextLength, 1);
		iAfterBreak.Set(iText, p, iBreaker);
		return ETrue;
		}

	// Find break within this run of SA text
	const TText* startOfSaText = iBeforeBreak.Ptr();
	const TText* endOfSaText = iAfterBreak.Ptr();
	
	if ((beforeClass != MLineBreaker::ESaLineBreakClass && *startOfSaText != KWhiteSpace) || 
			(afterClass != MLineBreaker::ESaLineBreakClass))
		// Complex (South-East Asian) line breaking rules not required.
		return breaker.LineBreakPossible(beforeClass, afterClass, iHasSpaces);

	MoveTextPtr(endOfSaText, iText + iTextLength, 1);
	const TText* minBreak = startOfSaText < iMinBreakPos ?
		iMinBreakPos : startOfSaText + 1;
	const TText* maxBreak = iMaxBreakPos < endOfSaText ?
		iMaxBreakPos : endOfSaText - 1;
	TInt breakPos;
	TPtrC saText(startOfSaText, endOfSaText - startOfSaText);
	if (!breaker.GetLineBreakInContext(saText,
		minBreak - startOfSaText, maxBreak - startOfSaText,
		aForwards, breakPos))
		return EFalse;

	// Convert the break position within SA text into iterator positions.
	const TText* p = startOfSaText + breakPos;
		iAfterBreak.Set(iText, p, iBreaker);
		if (*p != KZeroWidthSpace) MoveTextPtr(p, 0, -1);
		iBeforeBreak.Set(iText, p, iBreaker);

		return ETrue;
	}

TBool TLineBreakIterator::HasContingentBreak() const
	{
	return iBeforeBreak.Class() == MLineBreaker::ECbLineBreakClass
		|| iAfterBreak.Class() == MLineBreaker::ECbLineBreakClass;
	}

TInt TLineBreakIterator::PreviousClass() const
	{
	return iBeforeBreak.Class();
	}

TInt TLineBreakIterator::NextClass() const
	{
	return iAfterBreak.Class();
	}

TBool TLineBreakIterator::HasSpaces() const
	{
	return iHasSpaces;
	}

TBool TLineBreakIterator::Decrement()
	{
	iAfterBreak = iBeforeBreak;
	return iBeforeBreak.Move(iLimit, iLimitAfterSpaces, -1, iHasSpaces, iBreaker);
	}

TBool TLineBreakIterator::Increment()
	{
	iBeforeBreak = iAfterBreak;
	return iAfterBreak.Move(iLimit, iLimitAfterSpaces, 1, iHasSpaces, iBreaker);
	}

TInt TLineBreakIterator::BreakPos() const
	{
	const TText* t = iBeforeBreak.Ptr();
	MoveTextPtr(t, 0, 1);
	return t - iText;
	}

TInt TLineBreakIterator::BeforeBreakPos() const
	{
	return iBeforeBreak.Ptr() - iText;
	}

TInt TLineBreakIterator::AfterBreakPos() const
	{
	return iAfterBreak.Ptr() - iText;
	}

TBool HasContingentBreak(TLineBreakIterator& aIterator,
	MContingentLineBreaker& aCbDelegate)
	{
	if (aIterator.PreviousClass() == MLineBreaker::ECbLineBreakClass
		&& aCbDelegate.IsLegalBreakAfter(
		aIterator.BeforeBreakPos(), aIterator.NextClass(),
		aIterator.HasSpaces()))
		return ETrue;
	if (aIterator.NextClass() == MLineBreaker::ECbLineBreakClass
		&& aCbDelegate.IsLegalBreakBefore(
		aIterator.AfterBreakPos(), aIterator.PreviousClass(),
		aIterator.HasSpaces()))
		return ETrue;
	return EFalse;
	}

TBool FindBreak(TLineBreakIterator& aIterator, TBool aForwards,
	MContingentLineBreaker* aCbDelegate)
	{
	while (aForwards? aIterator.Increment() : aIterator.Decrement())
		{
		if (aCbDelegate && aIterator.HasContingentBreak())
			{
			if (HasContingentBreak(aIterator, *aCbDelegate))
				{
				return ETrue;
				}
			}
		else if (aIterator.IsBreak(aForwards))
			{
			return ETrue;
			}
		}
	return EFalse;
	}

TBool HasContingentBreakL(TLineBreakIterator& aIterator,
	MContingentLineBreakerL& aCbDelegate)
	{
	if (aIterator.PreviousClass() == MLineBreaker::ECbLineBreakClass
		&& aCbDelegate.IsLegalBreakAfterL(
		aIterator.BeforeBreakPos(), aIterator.NextClass(),
		aIterator.HasSpaces()))
		return ETrue;
	if (aIterator.NextClass() == MLineBreaker::ECbLineBreakClass
		&& aCbDelegate.IsLegalBreakBeforeL(
		aIterator.AfterBreakPos(), aIterator.PreviousClass(),
		aIterator.HasSpaces()))
		return ETrue;
	return EFalse;
	}

TBool FindBreakL(TLineBreakIterator& aIterator, TBool aForwards,
	MContingentLineBreakerL* aCbDelegate)
	{
	while (aForwards? aIterator.Increment() : aIterator.Decrement())
		{
		if (aCbDelegate && aIterator.HasContingentBreak())
			{
			if (HasContingentBreakL(aIterator, *aCbDelegate))
				return ETrue;
			}
		else if (aIterator.IsBreak(aForwards))
			return ETrue;
		}
	return EFalse;
	}

EXPORT_C TBool MLineBreaker::GetLineBreak(const TDesC& aText,
	TInt aMinBreakPos, TInt aMaxBreakPos, TBool aForwards,
	MContingentLineBreaker* aCbDelegate,
	TInt& aBreakPos, TInt& aBreakPosAfterSpaces) const
/** Finds the line break according to the rules in the virtual functions. 

@param aText The text to be broken.
@param aMinBreakPos The smallest allowed result.
@param aMaxBreakPos The position of the first character that fails to fit the 
measure.
@param aForwards ETrue to find the earliest legal break opportunity, EFalse 
to find the last.
@param aCbDelegate Object to resolve breaks around characters with the CB (cointingent 
break) line breaking class. Typically this class is only used for character 
0xFFFD, which usually marks the position of embedded pictures. May be NULL, 
in which case LineBreakPossible() determines whether these breaks are legal.
@param aBreakPos Returns the position of the break.
@param aBreakPosAfterSpaces Returns the position of the start of the next line.
@return ETrue if successful, EFalse if no legal line breaks were found. */
    {
	TLineBreakClassCache cache(*this);
	const TText* text = aText.Ptr();
	TLineBreakIterator it(cache, text, aText.Length(),
		aForwards, aMinBreakPos, aMaxBreakPos);
	if (!FindBreak(it, aForwards, aCbDelegate))
		return EFalse;
	aBreakPos = it.BreakPos();
	aBreakPosAfterSpaces = it.AfterBreakPos();
	return ETrue;
	}

EXPORT_C TBool MLineBreaker::GetLineBreakL(const TDesC& aText,
	TInt aMinBreakPos, TInt aMaxBreakPos, TBool aForwards,
	MContingentLineBreakerL* aCbDelegate,
	TInt& aBreakPos, TInt& aBreakPosAfterSpaces) const
/** Exactly the same as GetLineBreak, but aCbDelegate's functions may leave so 
the whole function may leave. 

@param aText The text to be broken.
@param aMinBreakPos The smallest allowed result.
@param aMaxBreakPos he position of the first character that fails to fit the 
measure.
@param aForwards ETrue to find the earliest legal break opportunity, EFalse 
to find the last.
@param aCbDelegate Object to resolve breaks around characters with the CB (cointingent 
break) line breaking class. Typically this class is only used for character 
0xFFFD, which usually marks the position of embedded pictures. May be NULL, 
in which case LineBreakPossible() determines whether these breaks are legal.
@param aBreakPos Returns the position of the break.
@param aBreakPosAfterSpaces Returns the position of the start of the next line.
@return ETrue if successful, EFalse if no legal line breaks were found. */
	{
	if (aMaxBreakPos < aMinBreakPos)
		return EFalse;
	TLineBreakClassCache cache(*this);
	const TText* text = aText.Ptr();
	TLineBreakIterator it(cache, text, aText.Length(),
		aForwards, aMinBreakPos, aMaxBreakPos);
	if (!FindBreakL(it, aForwards, aCbDelegate))
		return EFalse;
	aBreakPos = it.BreakPos();
	aBreakPosAfterSpaces = it.AfterBreakPos();
	return ETrue;
	}
EXPORT_C void* MLineBreaker::ExtendedInterface(TUid&) const 
/** Returns an interface to more functions, or 0 if these are unsupported.
Currently there are no other functions, so 0 is always returned. If this
function is overridden, it must base call if aUid is not recognised,
rather than just returning 0.
@param aUid Identifies the interface to be returned.
@return A pointer to the extended interface, or 0 if not supported. */
	{ 
	return 0; 
	}


