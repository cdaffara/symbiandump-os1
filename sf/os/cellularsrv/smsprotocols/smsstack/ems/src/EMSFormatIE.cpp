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
* Implements the Enhanced SMS Format Information Element.
*
*/


/**
 *  @file
 *  
 *  Implements  CEmsFormatIE  class
 */

#include <emsformatie.h>

/**
 *  Creates a default ems format object.
 *  
 *  @return CEmsFormatIE* newly created object
 */
EXPORT_C CEmsFormatIE* CEmsFormatIE::NewL()
/**
 *  @capability None
 */
	{
	CEmsFormatIE* self = new (ELeave) CEmsFormatIE();
	self->SetAlignment(ELangDepend);
	return self;
	}

/**
 *  Creates an exact copy of the object.
 *  
 *  @return CEmsFormatIE* Newly created object.
 */
EXPORT_C CEmsInformationElement* CEmsFormatIE::DuplicateL() const
/**
 *  @capability None
 */
	{
	CEmsFormatIE* copy = new (ELeave) CEmsFormatIE();
	CleanupStack::PushL(copy);
	copy->CopyL(*this);
	CleanupStack::Pop(copy);
	return copy;
	}

EXPORT_C void CEmsFormatIE::SetBold(TBool aBold)
/**
 *  @capability None
 */
	{
	iFormat=(TUint8)(aBold? iFormat|EBold: iFormat&(~EBold));
	}

EXPORT_C TBool CEmsFormatIE::Bold() const
/**
 *  @capability None
 */
	{
	return (EBold&iFormat)?ETrue:EFalse;
	}

EXPORT_C void CEmsFormatIE::SetItalic(TBool aItalic)
/**
 *  @capability None
 */
	{
	iFormat=(TUint8)(aItalic? iFormat|EItalic: iFormat&(~EItalic));
	}

EXPORT_C TBool CEmsFormatIE::Italic() const
/**
 *  @capability None
 */
	{
	return (EItalic&iFormat)?ETrue:EFalse;
	}

EXPORT_C void CEmsFormatIE::SetUnderline(TBool aUnderline)
/**
 *  @capability None
 */
	{
	iFormat=(TUint8)(aUnderline? iFormat|EUnderline: iFormat&(~EUnderline));
	}

EXPORT_C TBool CEmsFormatIE::Underline() const
/**
 *  @capability None
 */
	{
	return (EUnderline&iFormat)?ETrue:EFalse;
	}

EXPORT_C void CEmsFormatIE::SetStrikethrough(TBool aStrikethrough)
/**
 *  @capability None
 */
	{
	iFormat=(TUint8)(aStrikethrough? iFormat|EStrikethrough: iFormat&(~EStrikethrough));
	}

EXPORT_C TBool CEmsFormatIE::Strikethrough() const
/**
 *  @capability None
 */
	 {
	return (EStrikethrough&iFormat)?ETrue:EFalse;
	}

EXPORT_C void CEmsFormatIE::SetAlignment(TAlignment aAlignment)
/**
 *  @capability None
 */
	{
	iFormat=(TUint8)(iFormat&~EAlignmentMask);
	iFormat=(TUint8)(iFormat|aAlignment);
	}

EXPORT_C CEmsFormatIE::TAlignment CEmsFormatIE::Alignment() const
/**
 *  @capability None
 */
	{
	return (TAlignment)(EAlignmentMask&iFormat);
	}

EXPORT_C void CEmsFormatIE::SetFontSize(TFontSize aSize)
/**
 *  @capability None
 */
	{
	iFormat=(TUint8)(iFormat&~EFontSizeMask);
	iFormat=(TUint8)(iFormat|(aSize<<2));
	}

EXPORT_C CEmsFormatIE::TFontSize CEmsFormatIE::FontSize() const
/**
 *  @capability None
 */
	{
	return (TFontSize)((EFontSizeMask&iFormat)>>2);
	}


EXPORT_C void  CEmsFormatIE::SetFormatLength(TUint aLength)
/**
 *  @capability None
 */
	{
	iFormatLength = aLength;
	}

EXPORT_C TUint CEmsFormatIE::FormatLength() const
/**
 *  @capability None
 */
	{
	return iFormatLength;
	}


CEmsFormatIE::CEmsFormatIE() :CEmsInformationElement(CSmsInformationElement::ESmsEnhancedTextFormatting),iFormat(0){iEncodedBodyLength=EEnhancedFormatSizeV4;}

/**
 *  Encodes the format object into its raw format. (no IE id)
 *  
 *  @param aPtr the buffer to be used which is to contain the data
 *  @param aIsForSerialisation boolean to indicate if it is for serialisation or encoding
 */
void CEmsFormatIE::EncodeBodyL(TPtr8 aPtr, TBool aIsForSerialisation) const
	{
	// This is the only information element which needs to check the
	// aIsForSerialisation flag. If it is for PDU encoding, the
	// length field is only 8 bits. Otherwise it is 16. (This is
	// because format objects when inserted can extend over a PDU
	// boundary, but are split on encoding)
	if (aIsForSerialisation)
		{
		aPtr.Append(HI_BYTE(iFormatLength));
		}

	aPtr.Append(LO_BYTE(iFormatLength));
	aPtr.Append(iFormat);
	}

/**
 *  Decodes the raw data out of an information element into this class.
 *  
 *  @param aPtr The raw predefined animation data
 *  @param aIsFromSerialisation boolean to indicate if it is from serialisation
 *  @leave KErrargument If the size of the data does not match what is expected.
 */
void CEmsFormatIE::DecodeBodyL(const TPtrC8 aPtr, TBool aIsFromSerialisation)
	{
	__ASSERT_ALWAYS((aPtr.Length() >= EEnhancedFormatSizeV4), User::Leave(KErrArgument));

	// index into byte array we are currently looking at
	TInt pos = 0;
	iFormatLength = 0;

	if (aIsFromSerialisation)
		{
		iFormatLength = static_cast<TUint>(aPtr[pos] * 256);
		++pos;
		}
	iFormatLength += static_cast<TUint>(aPtr[pos]);
	++pos;
	iFormat=aPtr[pos];
	}

TInt CEmsFormatIE::SerialisedBodyLength() const
	{
	// extra byte required for 16-bit length field when serialised
	return iEncodedBodyLength + 1;
	}

EXPORT_C void CEmsFormatIE::CopyL(const CEmsFormatIE& aSrc)
/**
 *  @capability None
 */
	{
	CEmsInformationElement::CopyL(aSrc);
	iFormatLength = aSrc.iFormatLength;
	iFormat=aSrc.iFormat;
	}
