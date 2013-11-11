// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSMSSTORETSYTESTHELPER_H
#define TSMSSTORETSYTESTHELPER_H


#include "ttsytesthelperbase.h"
	

/* This PDU can be decoded with the utility such as PDU spy
PDU LENGTH IS 113 BYTES
NO SMSC ADDRESS PRESENT
MESSAGE HEADER FLAGS
	MESSAGE TYPE :	SMS DELIVER
	MSGS WAITING IN SC :	NO
	SEND STATUS REPORT :	YES
	USER DATA HEADER :	NO UDH
	REPLY PATH :	NO

ORIGINATING ADDRESS
	NUMBER IS :	01632960000
	TYPE OF NR. :	International
	NPI :	ISDN/Telephone (E.164/163)

PROTOCOL IDENTIFIER	(0x00)
	MESSAGE ENTITIES :	SME-to-SME
	PROTOCOL USED :	Implicit / SC-specific

DATA CODING SCHEME	(0x00)
	AUTO-DELETION :	OFF
	COMPRESSION :	OFF
	MESSAGE CLASS :	NONE
	ALPHABET USED :	7bit default

SMSC TIMESTAMP :	10/09/07 17:15 GMT+1.00

USER DATA PART OF SM
	USER DATA LENGTH : 	107 septets
	USER DATA (TEXT) :	TravelAlert 17:15: Nrthrn: Minor delays 
		on the H Barnet branch due to a 
		passenger incident at FinchleyCntl.
*/
_LIT8(KValidSmsPdu1, "\x24\x0c\x91\x44\x77\x79\x08\x80\x25\x00\x00\x70\x90\x01\x71\x51\x81\x40\x6b\x54\x79\xd8\x5e\x66\x07\xd9\x65\x39\x1d\x14\xbb\xe9\x62\x35\x1d\xc8\x29\xa7\xa3\xe5\x6e\x1d\xa8\x99\x76\xbf\xe5\x20\x72\x99\x1d\xce\xcf\x41\x6f\x37\x88\x8e\x2e\x83\x90\x20\x61\x58\xee\x2e\xd3\x41\x62\x79\xd8\x3d\x46\x83\xc8\xf5\x32\x88\xfe\x06\x85\x41\xf0\xf0\x7c\x5e\x76\x9f\xcb\x72\x50\xda\x3d\x4e\x93\xcb\x6e\x3a\x28\x4c\x07\x19\xd3\xee\x31\x9a\x5d\xce\x0f\xdd\x74\xb6\x0b");


class TSmsStoreTsyTestHelper : public TTsyTestHelperBase
	{
public:
	TSmsStoreTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep);

	void WaitForSmsStoreNotifyStoreEvent(RMobileSmsStore& aStore,
										TEtelRequestBase& aRequestStatus,
										TUint32 aEvent,
										TUint32 aExpectedEvent,
										TInt aIndex,
										TInt aExpectedStatus = KErrNone);
			
		// Utility functions
	void GsmLibCharConversion( TUint aDirection, 
	    					   TDes8& aString, 
	    				       TUint16 aLength );

	TUint16 GsmLibSmsPackMessage( TDes8& aDest,
    						  	  TDes8& aSrc,
    						      TUint16 aByteCount );	

	void GsmConvUnicodeTo0340Addr( TUint8 aTon,        
    							   TUint8 aNpi,        
    							   TDes8& aDest,       
    							   TDesC16& aSource );   
    							     
	void GsmConvUnicodeToAddr( TUint8 aTon,
   							   TUint8 aNpi,
     						   TDes8& aDest,
     						   TDesC16& aSource,
   							   TBool a0340Address );    
   							   
	void GsmUnicodeAddrToAlphanumericAddr( TDesC16& aSource,
    									   TDes8& aDestination,
    									   TUint8& aAmountOfSemiOctets );

	enum TDataCodingScheme
	{
	ENoDataCodingScheme = 0x00,
	EClass0DataCodingScheme = 0x10,
	EClass1DataCodingScheme = 0x11,
	EClass2DataCodingScheme = 0x12,
	EClass3DataCodingScheme = 0x13
	};

	void CreateValidSmsPdu(TDes8& aString, TDesC16& aPhoneNumber, TDes8& aPdu, 
			TDataCodingScheme aDataCodingScheme = ENoDataCodingScheme,
			TBool aStatusReportRequest = EFalse);
	TUint8 GsmLibPackAlphanumericAddress( TDes8& aDest, TDes8& aSource );
	TBool ToText(TDes &aRet,const TDesC8 &aOrigin,TBool aIsRecv);
	TBool EnsureStoreFullL(RMobileSmsStore& aSmsStore);
    

	}; // class TSmsStoreTsyTestHelper


#define COUNT_ONE 1
#define MAX_CONTEXTS 11


const TUint  TKGsmLibConvertToGsmFormat = 1;
const TUint  TKGsmLibConvertToNmpFormat = 2;
/** Maximum amount of digits in address data */
const TUint8 KMaxLengthOfAddressData = 10;

const TUint8 KMaxAmountOfDigits = 20;

typedef struct
    {
    TUint8 iSmsChar;
    TUint8 iHandsetChar;
    } GsmLibConv;
  
typedef struct
    {
    TUint8 iGsmChar;
    TUint16 iUnicodeChar;
    } TGsmLibConv;
      
/**
 * The number of codes that exist in
 * both character sets (GSM <--> NMP),
 * i.e. bidirectional conversions.
 */
const TUint8 TKGsmLibBidirTableSize = 34;
    
/**
 * The number of all conversion pairs,
 * both GSM <--> NMP and NMP --> substitute.
 */
const TUint8 TKGsmLibConvTableSize = 86;


const TUint8 TKGsmExtensionConvTableSize = 8;
static const GsmLibConv TKGsmExtensionConvTable[TKGsmExtensionConvTableSize] =
    {
    /*1*/    { 0x3C, 0x5B }, // [ //
    /*2*/    { 0x2F, 0x5C }, // \ //
    /*3*/    { 0x3E, 0x5D }, // ] //
    /*4*/    { 0x14, 0x5E }, // ^ //
    /*5*/    { 0x28, 0x7B }, // { //
    /*6*/    { 0x40, 0x7C }, // | //
    /*7*/    { 0x29, 0x7D }, // } //
    /*8*/    { 0x3D, 0x7E }  // ~ //
    };
    
const TUint8 KGsmExtensionConvTableSize = 10;   
const TUint8 KGsmLibConvTableSize = 92;

static const TGsmLibConv KGsmExtensionConvTable[
    KGsmExtensionConvTableSize ] =
    {
        { 0x0A, 0x000A }, /* Convert Page Break to Line Feed*/
        { 0x3C, 0x005B }, /* [ */
        { 0x2F, 0x005C }, /* \ */
        { 0x3E, 0x005D }, /* ] */
        { 0x14, 0x005E }, /* ^ */
        { 0x28, 0x007B }, /* { */
        { 0x40, 0x007C }, /* | */
        { 0x29, 0x007D }, /* } */
        { 0x3D, 0x007E }, /* ~ */
        { 0x65, 0x20AC }  /* Euro currency symbol */
    };
static const TGsmLibConv KGsmLibConvTable[ KGsmLibConvTableSize ] =
    {
        { 0x20, 0x0020 },
        // SPACE, this pair must be here at index #1 to avoid
        // converting NMP(space) to GSM(0x11) or to GSM(0x1B)
        // - see indeces 16 and 17 in the table below.

        { 0x00, 0x0040 }, /* @ */
        { 0x01, 0x00A3 }, /* » */
        { 0x02, 0x0024 }, /* $ */
        { 0x03, 0x00A5 }, /* À */
        { 0x04, 0x00E8 }, /* “ */
        { 0x05, 0x00E9 }, /* ’ */
        { 0x06, 0x00F9 }, /* ™ */
        { 0x07, 0x00EC }, /*  */
        { 0x08, 0x00F2 }, /* ° */
        { 0x09, 0x00C7 }, /* ˙ */
        { 0x0A, 0x000A }, /* LF */
        { 0x0B, 0x00D8 }, /* ƒ */
        { 0x0C, 0x00F8 }, /* Ω */
        { 0x0D, 0x000D }, /* CR */
        { 0x0E, 0x00C5 }, /* È */
        { 0x0F, 0x00E5 }, /* Õ */
        { 0x10, 0x0394 }, /* Greek: DELTA */
        { 0x11, 0x005F }, /* _ */
        { 0x12, 0x03A6 }, /* Greek: PHI */
        { 0x13, 0x0393 }, /* Greek: GAMMA */
        { 0x14, 0x039B }, /* Greek: LAMDA */
        { 0x15, 0x03A9 }, /* Greek: OMEGA */
        { 0x16, 0x03A0 }, /* Greek: PI */
        { 0x17, 0x03A8 }, /* Greek: PSI */
        { 0x18, 0x03A3 }, /* Greek: SIGMA */
        { 0x19, 0x0398 }, /* Greek: THETA */
        { 0x1A, 0x039E }, /* Greek: XI */
        /*0x1B             Extension code */
        { 0x1C, 0x00C6 }, /* Û */
        { 0x1D, 0x00E6 }, /* Ã */
        { 0x1E, 0x00DF }, /* beta */
        { 0x1F, 0x00C9 }, /* Ë */
        /*0x20             SPACE */
        { 0x21, 0x0021 }, /* ! */
        { 0x22, 0x0022 }, /* " */
        { 0x23, 0x0023 }, /* # */
        { 0x24, 0x00A4 }, /*   */
        /*0x25 - 0x3F:     Unicode and GSM sets are similar */
        { 0x40, 0x00A1 }, /* ¿ */
        /*0x41 - 0x5A:     A..Z */
        { 0x5B, 0x00C4 }, /* · */
        { 0x5C, 0x00D6 }, /* ¯ */
        { 0x5D, 0x00D1 }, /* Ó */
        { 0x5E, 0x00DC }, /* … */
        { 0x5F, 0x00A7 }, /* œ */
        { 0x60, 0x00BF }, /* ¢ */
        /*0x61 - 0x7A:     a..z */
        { 0x7B, 0x00E4 }, /*  */
        { 0x7C, 0x00F6 }, /*  */
        { 0x7D, 0x00F1 }, /* ˛ */
        { 0x7E, 0x00FC }, /*  */
        { 0x7F, 0x00E0 }, /* ¡ */

/**
 * The characters in the list below can be found from 8bit Unicode set
 * but not from GSM character set. Part of them are changed to spaces,
 * part of them to the 'nearest' equivalent (by removing the acute/grave/
 * tilde/etc. marks.
 */
        { 0x63, 0x00A2 },   /* Unicode: ¬, GSM: c */
        /*      0x5B-0x5E These chars can be found GSM extension table: [\]^*/
        { 0x20, 0x0060 },   /* Unicode: ', GSM: using SPACE */
        /*      0x7B-0x7E These chars can be found GSM extension table: {|}~*/
        { 0x20, 0x007F },   /* Unicode: DEL, GSM: using SPACE */
        /*      0x80...0xBF: see conversion methods */
        { 0x41, 0x00C0 },   /* Unicode: ‚, GSM: A */
        { 0x41, 0x00C1 },   /* Unicode: Í, GSM: A */
        { 0x41, 0x00C2 },   /* Unicode: Ù, GSM: A */
        { 0x41, 0x00C3 },   /* Unicode: ˚, GSM: A */
        /*      0xC4:        · */
        /*      0xC5:        È */
        /*      0xC6:        Û */
        /*      0xC7:        ˙ */
        { 0x45, 0x00C8 },   /* Unicode: ‡, GSM: E */
        /*      0xC9:        Ë */
        { 0x45, 0x00CA },   /* Unicode: Ú, GSM: E */
        { 0x45, 0x00CB },   /* Unicode: ˘, GSM: E */
        { 0x49, 0x00CC },   /* Unicode: ‰, GSM: I */
        { 0x49, 0x00CD },   /* Unicode: Î, GSM: I */
        { 0x49, 0x00CE },   /* Unicode: ˆ, GSM: I */
        { 0x49, 0x00CF },   /* Unicode: ¸, GSM: I */
        { 0x44, 0x00D0 },   /* Unicode: D with line, GSM: D */
        /*      0xD1:        Ó */
        { 0x4F, 0x00D2 },   /* Unicode: ÿ, GSM: O */
        { 0x4F, 0x00D3 },   /* Unicode: ∆, GSM: O */
        { 0x4F, 0x00D4 },   /* Unicode: Â, GSM: O */
        { 0x4F, 0x00D5 },   /* Unicode: Ì, GSM: O */
        /*      0xD6:        ¯ */
        { 0x78, 0x00D7 },   /* Unicode: Ê, GSM: x */
        /*      0xD8:        ƒ */
        { 0x55, 0x00D9 },   /* Unicode: Ï, GSM: U */
        { 0x55, 0x00DA },   /* Unicode: ÷, GSM: U */
        { 0x55, 0x00DB },   /* Unicode: ‹, GSM: U */
        /*      0xDC:        … */
        { 0x59, 0x00DD },   /* Unicode: Y with ', GSM: Y */
        { 0x20, 0x00DE },   /* Unicode: Latin capital thorn, GSM: using SPACE*/
        /*      0xDF:        beta */
        /*      0xE0:        ¡ */
        { 0x61, 0x00E1 },   /* Unicode: √, GSM: a */
        { 0x61, 0x00E2 },   /* Unicode: „, GSM: a */
        { 0x61, 0x00E3 },   /* Unicode: –, GSM: a */
        /*      0xE4:         */
        /*      0xE5:        Õ */
        /*      0xE6:        Ã */
        { 0x63, 0x00E7 },   /* Unicode: ”, GSM: c */
        /*      0xE8:        “ */
        /*      0xE9:        ’ */
        { 0x65, 0x00EA },   /* Unicode: ı, GSM: e */
        { 0x65, 0x00EB },   /* Unicode: e with two dots, GSM: e */
        /*      0xEC:          */
        { 0x69, 0x00ED },   /* Unicode: ⁄, GSM: i */
        { 0x69, 0x00EE },   /* Unicode: , GSM: i */
        { 0x69, 0x00EF },   /* Unicode: ˇ, GSM: i */
        { 0x20, 0x00F0 },   /* Unicode: Latin small eth, GSM: using SPACE */
        /*      0xF1:        ˛ */
        /*      0xF2:        ° */
        { 0x6F, 0x00F3 },   /* NMP: æ, GSM: o */
        { 0x6F, 0x00F4 },   /* NMP: ú, GSM: o */
        { 0x6F, 0x00F5 },   /* NMP: û, GSM: o */
        /*      0xF6:          */
        { 0x20, 0x00F7 },   /* Unicode: ü, GSM: using SPACE */
        /*      0xF8:        Ω */
        /*      0xF9:        ™ */
        { 0x75, 0x00FA },   /* Unicode: ∫, GSM: u */
        { 0x75, 0x00FB },   /* Unicode: ç, GSM: u */

        { 0x79, 0x00FD },   /* Unicode: y with ', GSM: y */
        { 0x20, 0x00FE },   /* Unicode: Latin small thorn, GSM: using SPACE */
        { 0x79, 0x00FF }    /* Unicode: y with two dots, GSM: y */
    };
    
static const GsmLibConv TKGsmLibConvTable[ TKGsmLibConvTableSize ] =
    {
    /*1*/  { 0x20, 0x20 },
    // SPACE, this pair must be here at index #1 to avoid
    // converting NMP(space) to GSM(0x11) or to GSM(0x1B)
    // - see indeces 16 and 17 in the table below.

    /*2*/  { 0x00, 0x40 },   /* @ */
    /*3*/  { 0x01, 0x1C },   /* » */
    /*4*/  { 0x02, 0x24 },   /* $ */
    /*5*/  { 0x03, 0x5C },   /* À */
    /*6*/  { 0x04, 0xE8 },   /* “ */
    /*7*/  { 0x05, 0xE9 },   /* ’ */
    /*8*/  { 0x06, 0xF9 },   /* ™ */
    /*9*/  { 0x07, 0xEC },   /*  */
    /*10*/ { 0x08, 0xF2 },   /* ° */
    /*11*/ { 0x09, 0xC7 },   /* ˙ */
    /*       0x0A:             LF */
    /*12*/ { 0x0B, 0xD8 },   /* ƒ */
    /*13*/ { 0x0C, 0xF8 },   /* Ω */
    /*       0x0D:             CR */
    /*14*/ { 0x0E, 0xC5 },   /* È */
    /*15*/ { 0x0F, 0xE5 },   /* Õ */
    /*       0x10:              NMP and GSM: delta */
    /*16*/ { 0x11, 0x5F },   /* Underscore   */
    /*17*/ { 0x1B, 0x7B },   /*   */
    /*       0x12 - 0x1A:       NMP and GSM: greek letters */
    /*18*/ { 0x1C, 0xC6 },   /* Û */
    /*19*/ { 0x1D, 0xE6 },   /* Ã */
    /*20*/ { 0x1E, 0xDF },   /* beta */
    /*21*/ { 0x1F, 0xC9 },   /* Ë */
    /*       0x20:              space */
    /*       0x21:              ! */
    /*       0x22:              " */
    /*       0x23:              # */
    /*22*/ { 0x24, 0x1F },   /* ¥ */
    /*       0x25 - 0x3F:       NMP and GSM sets are similar */
    /*23*/ { 0x40, 0x11 },   /* ¿ */
    /*       0x41 - 0x5A:    A..Z */
    /*24*/ { 0x5B, 0xC4 },   /* · */
    /*25*/ { 0x5C, 0xD6 },   /* ¯ */
    /*26*/ { 0x5D, 0xD1 },   /* Ó */
    /*27*/ { 0x5E, 0xDC },   /* … */
    /*28*/ { 0x5F, 0x1B },   /* œ */
    /*29*/ { 0x60, 0x1D },

    /*30*/ { 0x7B, 0xE4 },   /*  */
    /*31*/ { 0x7C, 0xF6 },   /*  */
    /*32*/ { 0x7D, 0xF1 },   /* ˛ */
    /*33*/ { 0x7E, 0xFC },
    /*34*/ { 0x7F, 0xE0 },   /* ¡ */

    // The characters in the list below exist in NMP character set
    // but not in GSM character set. Part of them are changed to spaces,
    // part of them to the 'nearest' equivalent (by removing the acute/grave/
    // tilde/etc. marks.

    /*35*/ { 0x63, 0x1E },   /* NMP: ¬, GSM: c */
    /*36*/ { 0x20, 0x5B },   /* no equivalent in GSM char set, using SPACE */
    /*37*/ { 0x20, 0x5D },   /* no equivalent in GSM char set, using SPACE */
    /*38*/ { 0x10, 0x5E },
    /* NMP: filled triangle, GSM: delta (unfilled triangle)*/
    /*39*/ { 0x20, 0x5F },   /* NMP: _, GSM: SPACE */
    /*40*/ { 0x20, 0x60 },   /* no equivalent in GSM char set, using SPACE*/
    /*41*/ { 0x20, 0x7B },   /* no equivalent in GSM char set, using SPACE */
    /*42*/ { 0x20, 0x7C },   /* no equivalent in GSM char set, using SPACE */
    /*43*/ { 0x20, 0x7D },   /* no equivalent in GSM char set, using SPACE */
    /*44*/ { 0x20, 0x7E },   /* no equivalent in GSM char set, using SPACE */
    /*45*/ { 0x20, 0x7F },   /* no equivalent in GSM char set, using SPACE */
    /*       0x20, 0x80...0xBF: see line *3000* in the function. */
    /*46*/ { 0x41, 0xC0 },   /* NMP: ‚, GSM: A */
    /*47*/ { 0x41, 0xC1 },   /* NMP: Í, GSM: A */
    /*48*/ { 0x41, 0xC2 },   /* NMP: Ù, GSM: A */
    /*49*/ { 0x41, 0xC3 },   /* NMP: ˚, GSM: A */
    /*       0xC4:        · */
    /*       0xC5:        È */
    /*       0xC6:        Û */
    /*       0xC7:        ˙ */
    /*50*/ { 0x45, 0xC8 },   /* NMP: ‡, GSM: E */
    /*             0xC9:        Ë */
    /*51*/ { 0x45, 0xCA },   /* NMP: Ú, GSM: E */
    /*52*/ { 0x45, 0xCB },   /* NMP: ˘, GSM: E */
    /*53*/ { 0x49, 0xCC },   /* NMP: ‰, GSM: I */
    /*54*/ { 0x49, 0xCD },   /* NMP: Î, GSM: I */
    /*55*/ { 0x49, 0xCE },   /* NMP: ˆ, GSM: I */
    /*56*/ { 0x49, 0xCF },   /* NMP: ¸, GSM: I */
    /*57*/ { 0x45, 0xD0 },   /* NMP: E with tilde, GSM: E */
    /*             0xD1:        Ó */
    /*58*/ { 0x4F, 0xD2 },   /* NMP: ÿ, GSM: O */
    /*59*/ { 0x4F, 0xD3 },   /* NMP: ∆, GSM: O */
    /*60*/ { 0x4F, 0xD4 },   /* NMP: Â, GSM: O */
    /*61*/ { 0x4F, 0xD5 },   /* NMP: Ì, GSM: O */
    /*             0xD6:        ¯ */
    /*62*/ { 0x4F, 0xD7 },   /* NMP: Ê, GSM: O */
    /*             0xD8:        ƒ */
    /*63*/ { 0x55, 0xD9 },   /* NMP: Ï, GSM: U */
    /*64*/ { 0x55, 0xDA },   /* NMP: ÷, GSM: U */
    /*65*/ { 0x55, 0xDB },   /* NMP: ‹, GSM: U */
    /*             0xDC:        … */
    /*66*/ { 0x20, 0xDD },
    /* (dotted cursor) no equivalent in GSM, SPACE used */
    /*67*/ { 0x77, 0xDE },   /* NMP: w with caret (^), GSM: w */
    /*             0xDF:        lower case beta */
    /*             0xE0:        ¡ */
    /*68*/ { 0x61, 0xE1 },   /* NMP: √, GSM: a */
    /*69*/ { 0x61, 0xE2 },   /* NMP: „, GSM: a */
    /*70*/ { 0x61, 0xE3 },   /* NMP: –, GSM: a */
    /*             0xE4:         */
    /*             0xE5:        Õ */
    /*             0xE6:        Ã */
    /*71*/ { 0x63, 0xE7 },   /* NMP: ”, GSM: c */
    /*             0xE8:        “ */
    /*             0xE9:        ’ */
    /*72*/ { 0x65, 0xEA },   /* NMP: ı, GSM: e */
    /*73*/ { 0x65, 0xEB },   /* NMP: e with two dots, GSM: e */
    /*             0xEC:          */
    /*74*/ { 0x69, 0xED },   /* NMP: ⁄, GSM: i */
    /*75*/ { 0x69, 0xEE },   /* NMP: , GSM: i */
    /*76*/ { 0x69, 0xEF },   /* NMP: ˇ, GSM: i */
    /*77*/ { 0x65, 0xF0 },   /* NMP: e with tidle (~), GSM: e */
    /*             0xF1:        ˛ */
    /*             0xF2:        ° */
    /*78*/ { 0x6F, 0xF3 },   /* NMP: æ, GSM: o */
    /*79*/ { 0x6F, 0xF4 },   /* NMP: ú, GSM: o */
    /*80*/ { 0x6F, 0xF5 },   /* NMP: û, GSM: o */
    /*             0xF6:          */
    /*81*/ { 0x6F, 0xF7 },   /* NMP: ü, GSM: o */
    /*             0xF8:        Ω */
    /*             0xF9:        ™ */
    /*82*/ { 0x75, 0xFA },   /* NMP: ∫, GSM: u */
    /*83*/ { 0x75, 0xFB },   /* NMP: ç, GSM: u */
    /*             0xFC:          */
    /*84*/ { 0x79, 0xFD },   /* NMP: y with ', GSM: y */
    /*85*/ { 0x79, 0xFE },   /* NMP: y with ^, GSM: y */
    /*86*/ { 0x79, 0xFF }    /* NMP: ù, GSM: y */
    };


#endif /*TSMSSTORETSYTESTHELPER_H*/

