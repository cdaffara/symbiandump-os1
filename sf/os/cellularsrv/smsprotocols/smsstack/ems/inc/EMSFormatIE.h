/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* Implements the Enhanced SMS Format Information Element. Includes 
* Version 5 Color information.
*/


/**
 *  @file
 *  @internalComponent
 *  Defines  CEmsFormatIE  class
 */

#ifndef __EMSFormatIE_H__
#define __EMSFormatIE_H__

#include <emsinformationelement.h>


class CEmsFormatIE : public CEmsInformationElement
/**
 *  CEmsFormatIE wraps the creation & modification of an EMS format
 *  information element into a single class.
 *  Format of Extended Object.
 *  ----------------------------
 *  
 *  Position (Octet)		Bit		Description
 *  1							Start Position
 *  2							Text Formatting Length
 *  3							Formatting mode
 *  3					1..0	Alignment
 *  1..0		0 0	Left
 *  1..0		0 1	Center
 *  1..0		1 0	Right
 *  1..0		1 1	Lang depend (default)
 *  
 *  3..2	FontSize
 *  3..2		0 0	Normal
 *  3..2		0 1	Large
 *  3..2		1 0	Small
 *  3..2		1 1	reserved
 *  
 *  4		Style Bold (1=on)
 *  5		Style Italic (1=on)
 *  6		Style Underline (1=on)
 *  7		Style Strikethrough (1=on)
 *  
 *  4					3..0	Text Color (See header for values) 3gpp Vrs 5
 *  7..4	Background Text Color (See header for values) 3gpp Vrs 5
 *  
 *  @see 3gpp Spec V5 section 9.2.3.24.10.1.1
 *  @internalComponent
 */
	{
public:
	// Note that sizes do NOT include start position
	enum {EEnhancedFormatSizeV4=0x02, EEnhancedFormatSizeV5=0x03};

	enum TEmsFormat {
		EAlignmentMask = 0x03,
		EFontSizeMask = 0x0c,
		EBold = 0x10,
		EItalic = 0x20,
		EUnderline = 0x40,
		EStrikethrough = 0x80
		};

	enum TAlignment {
		ELeft = 0x00,
		ECenter = 0x01,
		ERight = 0x02,
		ELangDepend = 0x03
		};

	enum TFontSize {
		ENormal = 0x00,
		ELarge = 0x01,
		ESmall = 0x02,
		EReserved = 0x03
		};

	enum TColor {
		EBlack = 0x00,
		EDarkGrey = 0x01,
		EDarkRed = 0x02,
		EDarkYellow = 0x03,
		EDarkGreen = 0x04,
		EDarkCyan = 0x05,
		EDarkBlue = 0x06,
		EDarkMagenta = 0x07,
		EGrey = 0x08,
		EWhite = 0x09,
		EBrightRed = 0x0A,
		EBrightYellow = 0x0B,
		EBrightGreen = 0x0C,
		EBrightCyan = 0x0D,
		EBrightBlue = 0x0E,
		EBrightMagenta = 0x0F
		};

	IMPORT_C static CEmsFormatIE* NewL();
	IMPORT_C CEmsInformationElement* DuplicateL() const;
	IMPORT_C void CopyL(const CEmsFormatIE& aSrc);

	IMPORT_C void  SetFormatLength(TUint aLength) ;
	IMPORT_C TUint FormatLength() const;

	IMPORT_C void SetBold(TBool aBold);
	IMPORT_C TBool Bold() const;

	IMPORT_C void SetItalic(TBool aItalic) ;
	IMPORT_C TBool Italic() const ;

	IMPORT_C void SetUnderline(TBool aUnderline) ;
	IMPORT_C TBool Underline() const ;

	IMPORT_C void SetStrikethrough(TBool aStrikethrough) ;
	IMPORT_C TBool Strikethrough() const ;

	IMPORT_C void SetAlignment(TAlignment aAlignment) ;
	IMPORT_C TAlignment Alignment() const ;

	IMPORT_C void SetFontSize(TFontSize aSize) ;
	IMPORT_C TFontSize FontSize() const ;


private:
	CEmsFormatIE();
	virtual void EncodeBodyL(TPtr8 aPtr, TBool aIsForSerialisation) const;
	virtual void DecodeBodyL(const TPtrC8 aPtr, TBool aIsFromSerialisation);

	// Overriden function since there is one extra byte to be stored when
	// serialised compared to when encoded
	virtual TInt SerialisedBodyLength() const;

	TUint iFormatLength;
	TUint8 iFormat;
	};


#endif // __EMSFormatIE_H__
