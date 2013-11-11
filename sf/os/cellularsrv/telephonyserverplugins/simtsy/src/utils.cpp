// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains a number of utility classes that are of general use
// thoughout the TSY.
// 
//

/**
 @file
*/

#include "utils.h"
#include "CSimPhone.h"


RCall::THookStatus ConvertStateToHook(RMobileCall::TMobileCallStatus aStatus)
/**
	Convert a call state to a hook state.
*/
	{
	if(aStatus==RMobileCall::EStatusUnknown)
		return RCall::EHookStatusUnknown;
	if((aStatus==RMobileCall::EStatusIdle)||
	   (aStatus==RMobileCall::EStatusRinging))
	   return RCall::EHookStatusOn;
	return RCall::EHookStatusOff;
	}

RPhone::TMode ConvertStateToMode(RMobileCall::TMobileCallStatus aStatus)
/**
	Convert a call state to a RPhone::TMode state.
	Note that the mapping table is not entirely clear here.
	The following mappings have been used here:
		EStatusUnknown		=>	EModeUnknown
		EStatusIdle			=>	EModeIdle
		EStatusDialling		=>	EModeEstablishingLink
		EStatusRinging		=>	EModeEstablishingLink
		EStatusAnswering	=>	EModeEstablishingLink
		EStatusConnecting	=>	EModeEstablishingLink
		EStatusConnected	=>	EModeOnlineData
		EStatusHold			=>	EModeOnlineData
		EStatusHangingUp	=>	EModeOnlineData

  There is currently no mapping for EModeOnlineCommand, so this mode state will
  never be returned.
*/
	{
	if(aStatus==RMobileCall::EStatusUnknown)
		return RPhone::EModeUnknown;
	if(aStatus==RMobileCall::EStatusIdle)
		return RPhone::EModeIdle;
	if((aStatus==RMobileCall::EStatusDialling) ||
		(aStatus==RMobileCall::EStatusAnswering) ||
		(aStatus==RMobileCall::EStatusRinging) ||
		(aStatus==RMobileCall::EStatusConnecting))
		return RPhone::EModeEstablishingLink;
	if(aStatus==RMobileCall::EStatusConnected)
		return RPhone::EModeOnlineData;
	if(aStatus==RMobileCall::EStatusDisconnecting)
		return RPhone::EModeOnlineData;
	if(aStatus==RMobileCall::EStatusHold)
		return RPhone::EModeOnlineData;
	return RPhone::EModeUnknown;		// Dummy final return value.
	}

TBool IsStateActive(RMobileCall::TMobileCallStatus aStatus)
	{
	if((aStatus==RMobileCall::EStatusUnknown) ||
	   (aStatus==RMobileCall::EStatusIdle) ||
	   (aStatus==RMobileCall::EStatusRinging))
	   return EFalse;
	return ETrue;
	}

void ConvertAsciiSms(const TDesC8& aAsciiPdu, TDes8& aOctetPdu)
/**
 * Populate the SMS PDU from an ASCII representation, such as that stored in the configuration file.
 * This simply involves converting the ASCII representation in the configuration file
 * into a hex representation to complete the request.
 */
	{
	// Check that the ASCII PDU length is even
	__ASSERT_ALWAYS((aAsciiPdu.Length()&0x1)==0x0,SimPanic(EIllegalOddNumberOfCharactersInConfigFilePdu));

	TUint8 digit;
	TInt i;
	for(i=0;i<aAsciiPdu.Length();i+=2)
		{
		TLex8 lex(aAsciiPdu.Mid(i,2));
		TInt ret=lex.Val(digit,EHex);
		__ASSERT_ALWAYS(ret==KErrNone,SimPanic(EIllegalHexCharacterInConfigFilePdu));
		aOctetPdu.Append(&digit,1);
		}
	}

/**
Converts a buffer containing string of the hexadecimal characters,
representing the binary data, into this binary data!

@param aSrc The buffer containing text representation.
@param aDst Binary data will be written to this buffer.
*/
void ConvertTextToBinary(const TDesC& aSrc, TDes8& aDst)
	{
	// Check that the ASCII PDU length is even
	__ASSERT_ALWAYS((aSrc.Length()&0x1)==0x0, SimPanic(EInvalidParameterFormatInConfigFile));

	aDst.SetLength(aSrc.Length() / 2);

	for (TInt ii = 0; ii < aSrc.Length(); ii += 2)
		{
		TInt val = 0;
		if ((aSrc[ii] >= '0') && (aSrc[ii] <= '9'))
			{
			val = ((aSrc[ii] - '0') << 4);
			}
		else if ((aSrc[ii] >= 'A') && (aSrc[ii] <= 'F'))
			{
			val = ((aSrc[ii] - 'A' + 10) << 4);
			}

		if ((aSrc[ii+1] >= '0') && (aSrc[ii+1] <= '9'))
			{
			val += (aSrc[ii+1] - '0');
			}
		else if ((aSrc[ii+1] >= 'A') && (aSrc[ii+1] <= 'F'))
			{
			val += (aSrc[ii+1] - 'A' + 10);
			}

		aDst[ii/2] = (TUint8) val;
		}
	}

TInt AsciiToNum(const TPtrC8 aParam, TUint8& aVal)
/**
* Converts a hexadecimal or decimal value stored in a TPtrC to its TUint value
*
* @param aParam		TPtr pointing to the hexadecimal or decimal number to convert.
* @param aVal		TUint reference to the val converted.
* @return TInt		Standard error return.
*/
	{
	_LIT8(KHexPrefix,"0x");
	TInt ret=KErrNone;
	if(aParam.Left(2).Compare(KHexPrefix)==0)
		{
		TLex8 lex(aParam.Mid(2));
		ret=lex.Val(aVal,EHex);
		}
	else
		{
		TLex8 lex(aParam);
		ret=lex.Val(aVal,EDecimal);
		}
	return ret;
	}

TInt AsciiToNum(const TPtrC8 aParam, TUint16& aVal)
/**
* Converts a hexadecimal or decimal value stored in a TPtrC to its TUint value
*
* @param aParam		TPtr pointing to the hexadecimal or decimal number to convert.
* @param aVal		TUint reference to the val converted.
* @return TInt		Standard error return.
*/
	{
	_LIT8(KHexPrefix,"0x");
	TInt ret=KErrNone;
	if(aParam.Left(2).Compare(KHexPrefix)==0)
		{
		TLex8 lex(aParam.Mid(2));
		ret=lex.Val(aVal,EHex);
		}
	else
		{
		TLex8 lex(aParam);
		ret=lex.Val(aVal,EDecimal);
		}
	return ret;
	}

TInt AsciiToNum(const TPtrC8 aParam, TUint32& aVal)
/**
* Converts a hexadecimal or decimal value stored in a TPtrC to its TUint value
*
* @param aParam		TPtr pointing to the hexadecimal or decimal number to convert
* @param aVal		TUint reference to the val converted.
* @return TInt		Standard error return.
*/
	{
	_LIT8(KHexPrefix,"0x");
	TInt ret=KErrNone;
	if(aParam.Left(2).Compare(KHexPrefix)==0)
		{
		TLex8 lex(aParam.Mid(2));
		ret=lex.Val(aVal,EHex);
		}
	else
		{
		TLex8 lex(aParam);
		ret=lex.Val(aVal,EDecimal);
		}
	return ret;
	}

TInt AsciiToNum(const TPtrC8 aParam, TInt32& aVal)
/**
* Converts a hexadecimal or decimal value stored in a TPtrC to its TUint value
*
* @param aParam		TPtr pointing to the hexadecimal or decimal number to convert
* @param aVal		TUint reference to the val converted.
* @return TInt		Standard error return.
*/
	{
	_LIT8(KHexPrefix,"0x");
	TInt ret=KErrNone;
	if(aParam.Left(2).Compare(KHexPrefix)==0)
		{
		TLex8 lex(aParam.Mid(2));
		ret=lex.Val(aVal,EHex);
		}
	else
		{
		TLex8 lex(aParam);
		ret=lex.Val(aVal,EDecimal);
		}
	return ret;
	}
	
TInt ParseMixedBinaryAsciiDataL(TDes8& aTextToConvert)
/**
Parses aTextToConvert based on the following rules:
'\\' (double backslash) is used to denote a single '\'
(single backslash)
'\xnn' denote a byte of binary data where nn is in hex-decimal.
The '\xnn' in aTextToConvert is replaced by the binary byte
that it represents.

For example: If aTextToConvert contains "abc\\def\xFF",
after parsing, it will contain "abc\def?" where ? = 0xFF.

@param aTextToConvert Modifiable buffer which will be parsed. 

@return KErrNone if aTextToConvert is in valid
        EAdditionalParamDataFormatMixedBinaryAndAscii format.
        KErrArgument if aTextToConvert is in an incorrect format.

@panic KErrNoMemory if there is not enough memory to do the parsing.
*/
	{
	// Pointer to unparsed portion of additionalParamDataBuffer
	HBufC8* resultBuffer = HBufC8::NewLC(aTextToConvert.Length());

	__ASSERT_ALWAYS(resultBuffer, PanicClient(KErrNoMemory));
	
	TPtr8 result(resultBuffer->Des());
	
	// Position of backslash
	TInt pos = 0;

	while ((pos = aTextToConvert.Locate('\\')) != KErrNotFound)
		{
		// Check that the backslash is followed by at least one more character
		if ((pos+1) >= aTextToConvert.Length())
			{
			return KErrArgument;
			}

		TUint8 modifier = aTextToConvert[pos+1];

		// Parse depending on character after the backslash
		switch (modifier)
			{
		case '\\':
			// Next character after the '\' is another '\'.
			// Replace it with a single '\' and move
			// on.
			result.Append(aTextToConvert.Left(pos+1));
			aTextToConvert.Delete(0, pos+2);
			break;
		case 'x':
			// Next character is an 'x' so check that there are three 
			// characters after the backslash (one for the x and two
			// characters of HEX.
			if ((pos+3) >= aTextToConvert.Length()) 
				{
				return KErrArgument;
				}
			// Convert those to HEX and replace '\xNN' with this.
			result.Append(aTextToConvert.Left(pos));
			TUint8 hexAsInt;
			if (AsciiHexToNum(aTextToConvert.MidTPtr(pos+2,2), hexAsInt) != KErrNone)
				{
				return KErrArgument;
				}
			// Append the raw byte to the result
			result.SetLength(result.Length()+1);
			result[result.Length()-1] = hexAsInt;
			aTextToConvert.Delete(0, pos+4);
			break;		
			}
		} // End while
	aTextToConvert.Insert(0, result);
	
	CleanupStack::PopAndDestroy(resultBuffer);
	return KErrNone;
	}

/**
Returns the numerical value that corresponds to the numeric character
of its parameter (in base 17).

@param aDigit A char between '0' to '9' or between 'A' to 'G'
*/
TInt CharToSeptNumL(const TChar& aChar)
	{
	TInt ret = 0;

	if((aChar >= '0') && (aChar <= '9'))
		{
		ret = TUint(aChar) - '0';
		}
	else if((aChar >= 'A') && (aChar <= 'G'))
		{
		ret = TUint(aChar) - 'A' + 10;
		}
	else
		{
		User::Leave(KErrArgument);
		}

	return ret;
	}
