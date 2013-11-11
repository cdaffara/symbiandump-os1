/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : TSatUtility.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* This file contains the Sat Utility class definition.
* Version     : 1.0
*
*/



#ifndef TSATUTILITY_H
#define TSATUTILITY_H

//  INCLUDES
#include <etelsat.h>    // Symbian Etel Sat related types
#include <cstktsy/CBerTlv.h>    // BerTlv    

// CONSTANTS
const TUint8 KZeroCharacterCode		        = 0x30;
const TUint8 KPacked7BitTextMask	        = 0x0C;
const TUint8 KCrShiftedOneBitToLeft 	    = 0x1A;
const TUint8 KUCS2TextCodingMask    	    = 0x08;
const TUint8 KUCS2ArabicCoding 	    	    = 0x80;
const TUint8 KUCS2GreekCoding	    	    = 0x81;
const TUint8 KUCS2TurkishCoding	    	    = 0x82;
// The card ID is always zero as long as only one supported
const TUint8 KCardId0                        = 0x00;

// From 3GPP TS 51.11, 9.4.2 Responses to commands which are postponed
const TUint8 KSw1SatBusy 		    	    = 0x93;

// From 3GPP TS 03.40, 9.2.3.22	TP Failure Cause (TP FCS) 
// SMS-PP SIM DDL error
const TUint8 KSmsPpSimDdlError 	    	    = 0xD5;
// SMSP-PP SAT busy
const TUint8 KSmsPpSatBusy 		    	    = 0xD4;

// From 3GPP TS 11.11, 9.4	Status conditions returned by the card
const TUint8 KSw1NormalEnding               = 0x90;
const TUint8 KSw2NormalEnding               = 0x00;

// From 3GPP TS 11.10-4, 27.22.8 MO SHORT MESSAGE CONTROL BY SIM 
const TUint8 KMoSmCtrlResultAllowed 					= 0x00;
const TUint8 KMoSmCtrlResultNotAllowed 					= 0x01;
const TUint8 KMoSmCtrlResultAllowedWithModifications 	= 0x02;

// From 3GPP TS 22.030 7.0.0 Structure of the MMI (Annex B)
const TUint16 KSsAllForwardings             = 0x0002; 
const TUint16 KSsAllCondForwardings         = 0x0004; 
const TUint16 KSsForwUnconditional          = 0x0015; 
const TUint16 KSsForwBusy                   = 0x0043; 
const TUint16 KSsForwNoReply                = 0x003D; 
const TUint16 KSsForwNotReachable           = 0x003E;
 
// TON NPI
// This just indicates that the TON NPI value is not set, TSY specific def.
const TUint8 KTonNpiNotSet                  = 0xff;               
// From 3GPP TS 27.007-460, 8.33 Set Voice Call Number +CSVM
const TUint8 KTonNpiInternational           = 0x91;
const TUint8 KTonNpiUnknown                 = 0x81;

// General string length limitations
const TUint8 KMaxAddressLength          	= 255;
// From 3GPP TS 23.038-450, 6.1.2.3	USSD packing
// String max length 160b + additional info in TR (1b) and DCS in PCmd (1b)
const TUint8 KMaxUssdStringLengthInChars    = 182;
const TUint8 KMaxUssdStringLengthInBytes    = 161;
// Etel accepts SS strings with maximum length 242
const TUint8 KMaxSSStringLength             = 242;
const TUint8 KSizeOfConversionArray         = 63;
const TUint8 KNumberMaxLength 		        = 123;
const TUint8 KOperatorCodeLength            = 3;
const TUint8 KAsciiBcdTableLength           = 15;

// This applies for Parameters, DCS and Protocol ID (3GPP TS 23.038, 5 CBS Data
// Coding Scheme; 3GPP TS 23.040, 9.2.3.9 TP Protocol Identifier (TP PID);
// 3GPP TS 23.040, 9.2.3.1 - 9.2.3.5
const TUint8 KSmsTpduByteUnknownOrReserved  = 0xFF;

// Constants used in bit operations
const TUint8 KMaskF0                        = 0xF0;

// Bcd->ascii translation table
static const TUint8 KAscii[16] =
    {
     '0', '1', '2', '3', '4', '5', '6', '7',
     '8', '9', '*', '#', 'p', 'w', '.',  0
    };

// 3GPP TS 11.11, EFadn
static const TUint8 KAsciiToBCD[KAsciiBcdTableLength][2] =
    { 
    { '0', 0x00 }, { '1', 0x01 }, { '2', 0x02 }, { '3', 0x03 },
    { '4', 0x04 }, { '5', 0x05 }, { '6', 0x06 }, { '7', 0x07 },
    { '8', 0x08 }, { '9', 0x09 }, { '*', 0x0A }, { '#', 0x0B },
    { 'p', 0x0C }, /*DTMF*/ 
    { 'w', 0x0D }, /*Wild*/ 
    { '.', 0x0E }, /*RFU*/ };
    
// GSM 7-bit ->unicode translation table
// See 3GPP TS 23.038
static const TUint16 KUnicode[128] =
    {
    0x0040, 0x00A3, 0x0024, 0x00A5, 0x00E8, 0x00E9, 0x00F9, 0x00EC, 
    0x00F2, 0x00C7, 0x000A, 0x00D8, 0x00F8, 0x000D, 0x00C5, 0x00E5,
    0x0394, 0x005F, 0x03A6, 0x0393, 0x039B, 0x03A9, 0x03A0, 0x03A8, 
    0x03A3, 0x0398, 0x039E, 0x001B, 0x00C6, 0x00E6, 0x00DF, 0x00C9,
    0x0020, 0x0021, 0x0022, 0x0023, 0x00A4, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x00A1, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 
    0x0058, 0x0059, 0x005A, 0x00C4, 0x00D6, 0x00D1, 0x00DC, 0x00A7, 
    0x00BF, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 
    0x0078, 0x0079, 0x007A, 0x00E4, 0x00F6, 0x00F1, 0x00FC, 0x00E0  
    };


static const TUint16 KUnicode16ToSms7[63][2] =
    { 
    { 0x0040, 0x00 }, { 0x00A3, 0x01 }, { 0x0024, 0x02 }, { 0x00A5, 0x03 },
    { 0x00E8, 0x04 }, { 0x00E9, 0x05 }, { 0x00F9, 0x06 }, { 0x00EC, 0x07 },
    { 0x00F2, 0x08 }, { 0x00C7, 0x09 }, { 0x00D8, 0x0B }, { 0x00F8, 0x0C },
    { 0x21B5, 0x0D }, { 0x000D, 0x0D }, { 0x2029, 0x0D }, { 0x00C5, 0x0E },
    { 0x00E5, 0x0F }, { 0x0394, 0x10 }, { 0x005F, 0x11 }, { 0x03A6, 0x12 },
    { 0x0393, 0x13 }, { 0x039B, 0x14 }, { 0x03A9, 0x15 }, { 0x03A0, 0x16 },
    { 0x03A8, 0x17 }, { 0x03A3, 0x18 }, { 0x0398, 0x19 }, { 0x039E, 0x1A },
    { 0x00C6, 0x1C }, { 0x00E6, 0x1D }, { 0x00DF, 0x1E }, { 0x00C9, 0x1F },
    { 0x00A4, 0x24 }, { 0x00A1, 0x40 }, { 0x00C4, 0x5B }, { 0x00D6, 0x5C },
    { 0x00D1, 0x5D }, { 0x00DC, 0x5E }, { 0x00A7, 0x5F }, { 0x00BF, 0x60 },
    { 0x00E4, 0x7B }, { 0x00F6, 0x7C }, { 0x00F1, 0x7D }, { 0x00FC, 0x7E },
    { 0x00E0, 0x7F }, { 0x007B, 0x1B }, { 0x007B, 0x28 }, { 0x007D, 0x1B },
    { 0x007D, 0x29 }, { 0x005B, 0x1B }, { 0x005B, 0x3C }, { 0x007E, 0x1B },
    { 0x007E, 0x3D }, { 0x005D, 0x1B }, { 0x005D, 0x3E }, { 0x005C, 0x1B },
    { 0x005C, 0x2F }, { 0x005E, 0x1B }, { 0x005E, 0x14 }, { 0x007C, 0x1B },
    { 0x007C, 0x40 }, { 0x20AC, 0x1B }, { 0x20AC, 0x65 } 
    };

// SMS data coding schemes
enum TSmsDcs
	{
	ESmsUnknownOrReservedDcs = KSmsTpduByteUnknownOrReserved,
	ESms7BitDcs = 0x00,
	ESms8BitDcs = 0x04,
	ESms16BitDcs = 0x08
	};

// DESCRIPTION
/**
* This class includes small aid fuctions
* @lib simatktsy
* @since S60 3.1
*/
class TSatUtility
    {

    public: // New functions

        /**
        * Map TON and NPI to RSat TON and NPI values
        * @param aTonAndNpi: Source TON and NPI
        * @param aTon: Mapped Type of number
        * @param aNpi: Mapped Numbering plan
        */
		IMPORT_C static void TonAndNpi( TInt aTonAndNpi, 
            RSat::TTypeOfNumber* aTon,
            RSat::TNumberingPlan* aNpi );

        /**
        * Converts 7-bit packed string to 8-bit unpacked format
        * @param aSource: Pointer to the const data
        * @param aTarget converted 8-bit unpacked string
        * @return KErrNone or KErrOverflow if string was too long
        */
        IMPORT_C static TInt Packed7to8Unpacked( const TPtrC8 aSource, 
            TDes8& aTarget );

        /**
        * Converts UCS2 string to 7-bit packed format
        * @param aInput: is source
        * @param aOutput: converted 7-bit packed string
        */
        IMPORT_C static void UCSToPacked7( const TPtrC aInput, 
            TDes8& aOutput );
        
        /**
        * Converts Binary Coded Decimal to ASCII
        * @param aInput is source
        * @param aOutput converted 8-bit string
        * @return KErrNone or KErrOverflow if string was too long
        */
        IMPORT_C static TInt BCDToAscii( const TPtrC8 aInput, TDes8& aOutput );	

        /**
        * Converts Ascii string to Binary Coded Decimal
        * @param aInput is source
        * @param aOutput converted 8-bit string
        * @return KErrNone or KErrOverflow if string was too long
        */
        IMPORT_C static TInt AsciiToBCD( const TDesC8& aInput, 
            TDes8& aOutput );

        /**
        * Remove Wild 'w' and Expansion digit '.' from EFadn string.
        * @param aInput is source
        * @param aOutput converted 8-bit string
        */
        IMPORT_C static void RemoveWildAndExpansionDigit( const TPtrC8 aInput,
            TDes8& aOutput );

        /**
        * Set Alpha identifier as a Unicode text string
        * and according to the alphabet used
        * @param aRawData input data that was extracted with 
        * ETLV_AlphaIdentifier
        * @param aAlphaId output
        */
        IMPORT_C static void SetAlphaId( const TPtrC8 aRawData, TDes& aAlphaId );		

        /**
        * Convert integer to BCD format. If number is more than 100 only last
        * two digits is converted.
        * @param aTime time
        * @return converted BCD value
        */
        IMPORT_C static TUint8 ConvertToSemiOctet( const TInt aTime );

        /**
        * Browse a text in Unicode format, and for some specific characters 
        * replace with the correct byte value
        * @param aInput: GSM 7 bit default alphabet string
        * @param aOutput: Unicode 16 string
        */
        IMPORT_C static void Convert7BitToUnicode16( const TDesC8& aInput,
            TDes16& aOutput );

        /**
        * Converts unicode16 string to 7 bit character mode
        * @param aInput: Unicode 16 string
        * @param aOutput: Resulting GSM 7 bit default alphabet string
        * @return KErrNone or KErrOverflow (input string is too long) 
        */
        IMPORT_C static TInt ConvertUnicode16To7Bit( const TDesC16& aInput,
            TDes8& aOutput );

        /**
        * Fill in a TDuration structure
        * @param aBerTlv tlv containing duration data
        * @param aTDuration duration structure
        */
        IMPORT_C static void FillDurationStructure( CBerTlv& aBerTlv, 
            RSat::TDuration& aTDuration );

        /**
        * Fill in a TIconId structure
        * @param aBerTlv data containing icon address data
        * @param aTIconId icon id structure
        * @param aItemNmb item number
        */
        IMPORT_C static void FillIconStructure( CBerTlv& aBerTlv, 
            RSat::TIconId& aTIconId, const TInt aItemNmb = 0 );

        /**
        * Set Text string as a Unicode text string
        * @param aTextString text string
        * @param aText unicode output text string
        */
        IMPORT_C static void SetText( CTlv& aTextTlv, TDes& aUnicodeOutput );

        /**
        * Convert Alpha field text string to a Unicode text string. 
        * @param aSource text string
        * @param aText unicode output text string
        */
        IMPORT_C static void ConvertAlphaFieldsToUnicode( 
            const TDesC8& aSource, TDes& aTarget );
            
        /**
        * Copy from 16 bit descriptor to 8 bit descriptor with little endian.
        * Function uses Append method.
        * @param aSource: source
        * @param aTarget: target
        * @return KErrNone or KErrOverflow if string was too long
        */
		IMPORT_C static TInt Copy16to8LE( const TDesC16& aSource, 
            TDes8& aTarget );

        /**
        * Copy from 8 bit descriptor to 16 bit descriptor with little endian.
        * Function uses Append method.
        * @param aSource: source
        * @param aTarget: target
        * @return KErrNone or KErrOverflow if string was too long
        */
        IMPORT_C static TInt Copy8to16LE( const TDesC8& aSource, 
            TDes16& aTarget );

        /**
        * Finds whether the data coding scheme, coded in CBS format,
        * is 7-bit, 8-bit or 16-bit
        * @param aDcs Input data coding scheme
        * @return Converted DCS ( SMS format )
        */
        IMPORT_C static TSmsDcs DecodeCbsDcs( const TUint8 aDcs ); 
        
        /**
        * Gets 8 bit byte and copy it to the 16 bit byte with right endian.
        * @param aSource: source
        * @param aTarget: target
        * @param aIndex: index
        * @return KErrNone or KErrOverflow if string was too long
        */
        IMPORT_C static TInt CopyTwo8toOne16LE( const TDesC8& aSource, 
            TUint16& aTarget, const TInt aIndex ); 
	};


#endif // TSATUTILITY_H

// End of File
