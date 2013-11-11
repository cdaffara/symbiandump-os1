// Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements utility classes private to the GSMU dll
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "gsmuprivTraces.h"
#endif

#include "gsmupriv.h"
#include "Gsmumain.h"
#include <exterror.h>

//
// TSmsPacker - packs and unpacks data encoded in an SMS alphabet
//

/**
 *  Constructor
 */
TSmsAlphabetPacker::TSmsAlphabetPacker(TSmsDataCodingScheme::TSmsAlphabet aAlphabet,TBool aIsBinary,TInt aStartBit)
	: iAlphabet(aAlphabet),iIsBinary(aIsBinary),iStartBit(aStartBit)
	{
	// NOP
	} // TSmsAlphabetPacker::TSmsAlphabetPacker


/**
 *  Packs user data units from aIn and appends to aOut.
 */
TInt TSmsAlphabetPacker::PackL(TDes8& aOut,const TDesC8& aIn)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSALPHABETPACKER_PACKL_1, "TSmsAlphabetPacker::PackL()");

	// Ensure we've got the right length
	TInt packedOctetsRequired=PackedOctetsRequiredL(aIn.Length());
	if (packedOctetsRequired>(aOut.MaxLength()-aOut.Length()))
		User::Leave(KErrOverflow);
	// Do the conversion
	TInt elementSizeInBits=ElementSizeInBitsL();
	if (elementSizeInBits==8)
		{
		// Straight copy here
		aOut.Append(aIn);
		}
	else if (elementSizeInBits==7)
		{
		// Get raw pointers and do packing
		TUint8* outPtr=(TUint8*)aOut.Ptr()+aOut.Length();
		const TUint8* inPtr=aIn.Ptr();

		outPtr[0]=0;
		for (TInt i=0; i<aIn.Length(); i++)
			{
			TUint8 to=inPtr[i];
			*outPtr|=(to<<iStartBit);
			if (iStartBit)
				{
				outPtr++;
				*outPtr=(TUint8) (to>>(8-iStartBit));
				}
			iStartBit=(iStartBit+7)%8;
			}
		// Increment the length for the packed data
		aOut.SetLength(aOut.Length()+packedOctetsRequired);
		}
	else
		{
		__ASSERT_DEBUG(EFalse,Panic(KGsmuPanicPackAlphabetInvalid));
		}
	// Return number of bytes used
	return packedOctetsRequired;
	} // TSmsAlphabetPacker::PackL


/**
 *  Unpacks user data units from aIn and appends to aOut.
 */
TInt TSmsAlphabetPacker::UnpackL(const TDesC8& aIn,TDes8& aOut,TInt aNumUDUnits)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSALPHABETPACKER_UNPACKL_1, "TSmsAlphabetPacker::UnpackL()");

	TInt length=aNumUDUnits;
	// Ensure we've got enough input and output buffer
 	// defect fix for
 	// EDNOPMA-4YPJ34	Short message with alphanumeric address in From field is not received
 	// aIn.length is constant (KSmsAddressMaxAddressValueLength = 10)
 	// PackedOctetsRequiredL is dependant on aNumUDUnits
 	// otheriwse if a short alphanumeric address is received GSMU leaves
 	// and the smsprot returns protocol error to the service center
 	// if (PackedOctetsRequiredL(aIn.Length())>aNumUDUnits)
 	if (PackedOctetsRequiredL(aNumUDUnits)>aIn.Length())
		User::Leave(KErrCorrupt);
	if (aOut.Length()+length>aOut.MaxLength())
		User::Leave(KErrCorrupt);
	TInt elementSizeInBits=ElementSizeInBitsL();
	if (elementSizeInBits==8)
		{
		aOut.Append(aIn);
		}
	else if (elementSizeInBits==7)
		{
		// Get raw pointers and do packing
		TUint8* outPtr=(TUint8*)aOut.Ptr()+aOut.Length();
		const TUint8* inPtr=aIn.Ptr();

		for (TInt i=0; i<length; i++)
			{
			TInt from=(*inPtr>>iStartBit) & 0x7F;
			if (iStartBit)
				{
				inPtr++;
				from|=(*inPtr<<(8-iStartBit)) & 0x7F;
				}
			outPtr[i]=(TUint8) from;
			iStartBit=(iStartBit+7)%8;
			}
		aOut.SetLength(aOut.Length()+length);
		}
	else
		{
		__ASSERT_DEBUG(EFalse,Panic(KGsmuPanicPackAlphabetInvalid));
		}
	return length;
	} // TSmsAlphabetPacker::UnpackL


/**
 *  Converts then packs the input data, aIn, and appends to aOut
 */
TInt TSmsAlphabetPacker::ConvertAndPackL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,TDes8& aOut,const TDesC& aIn,TInt& aConvertedNumUDUnits)
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSALPHABETPACKER_CONVERTANDPACKL_1, "TSmsAlphabetPacker::ConvertAndPackL()");

	// Do the conversion
	// VEP Fix for defect EXT-568BMW, when length of alphanumeric destination address
	// was set wrong and also special characters defined in 7-bit default aplhabet (ä,ö...)
	// were converted incorrectly
	CSmsAlphabetConverter* converter=CSmsAlphabetConverter::NewLC(aCharacterSetConverter,aFs,iAlphabet,iIsBinary);
	TPtrC8 convertedPtr=converter->ConvertFromNativeL(aIn);
	aConvertedNumUDUnits=convertedPtr.Length();
	// Do the packing
	TInt octetsUsed=PackL(aOut,convertedPtr);
	// Cleanup and return
	CleanupStack::PopAndDestroy(converter);
	return octetsUsed;
    } // TSmsAlphabetPacker::ConvertAndPackL


/**
 *  Unpacks the converts the input data, aIn, and appends to aOut
 */
TInt TSmsAlphabetPacker::UnpackAndConvertL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,const TDesC8& aIn,TDes& aOut,TInt aNumUDUnits)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSALPHABETPACKER_UNPACKANDCONVERTL_1, "TSmsAlphabetPacker::UnpackAndConvertL()");

	// Unpack first
	HBufC8* unpackedBuffer=HBufC8::NewLC(aNumUDUnits);
	TPtr8 unpackedBufferPtr(unpackedBuffer->Des());
	UnpackL(aIn,unpackedBufferPtr,aNumUDUnits);
	// Convert
	CSmsAlphabetConverter* converter=CSmsAlphabetConverter::NewLC(aCharacterSetConverter,aFs,iAlphabet,iIsBinary);
	TPtrC convertedPtr=converter->ConvertToNativeL(*unpackedBuffer);
	if (convertedPtr.Length()>(aOut.MaxLength()-aOut.Length()))
		User::Leave(KErrCorrupt);
	// Cleanup and return
	aOut.Append(convertedPtr);
	CleanupStack::PopAndDestroy(2);	// unpackedBuffer,converter
	return aNumUDUnits;
	} // TSmsAlphabetPacker::UnpackAndConvertL


/**
 *  Returns the number of octets needed to pack the specified number of
 */
TInt TSmsAlphabetPacker::PackedOctetsRequiredL(TInt aNumUDUnits) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSALPHABETPACKER_PACKEDOCTETSREQUIREDL_1, "TSmsAlphabetPacker::PackedOctetsRequiredL()");

	TInt octetsRequired=0;
	TInt elementSizeInBits=ElementSizeInBitsL();
	if (elementSizeInBits==8)
		octetsRequired=aNumUDUnits;
	else
		octetsRequired=(iStartBit+aNumUDUnits*elementSizeInBits + 7)/8;	// Rounds up
	return octetsRequired;
	} // TSmsAlphabetPacker::PackedOctetsRequiredL

/**
 * Returns the number of UD units that are packed in the specified number of octets
 */
TInt TSmsAlphabetPacker::NumUDUnitsL(TInt aOctets) const
	{
	TInt numUD=0;
	TInt elementSizeInBits=ElementSizeInBitsL();
	if (elementSizeInBits==8)
		numUD=aOctets;
	else
		numUD=(8*aOctets - iStartBit)/elementSizeInBits;
	return numUD;
	}

/**
 *  Returns the size in bits of a UDL element for the alphabet.  Leaves if
 *  invalid data coding scheme.
 */
TInt TSmsAlphabetPacker::ElementSizeInBitsL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSMSALPHABETPACKER_ELEMENTSIZEINBITSL_1, "TSmsAlphabetPacker::ElementSizeInBitsL()");

    TInt ret = 8;

	if (iIsBinary)
		return ret;
	switch (iAlphabet)
		{
		case TSmsDataCodingScheme::ESmsAlphabet7Bit:
			{
			ret = 7;
            break;
			}
		case TSmsDataCodingScheme::ESmsAlphabet8Bit:
		case TSmsDataCodingScheme::ESmsAlphabetUCS2:
			{
			ret = 8;
            break;
			}
		default:
			{
            User::Leave(KErrGsmSMSDataCodingSchemeNotSupported);
			}
		}
    return ret;
	} // TSmsAlphabetPacker::ElementSizeInBitsL
