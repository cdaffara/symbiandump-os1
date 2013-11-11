// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file smsatutil.cpp
// This contains SmsAtUtil which is util for Sms At command
// 

// user include
#include "smsatutil.h"
#include "ltsycommondefine.h"
#include "panic.h"

/**
 * Receive PDU example 
 * 0891683108200805F0040D91683188902848F4000850208151754500108FD9662F4E0067616D4B8BD577ED4FE1
 * 08 ocet length including 91
 * 91 Identify Address Type
 * 683108200805F0 SCA address
 * 04
 * 0D91683188902848F4
 * 00
 * 08
 * 50208151754500
 * 10   
 * 8FD9662F4E0067616D4B8BD577ED4FE1
 */
// ---------------------------------------------------------------------------
// SmsAtUtil::ConvertAsciiToBinary
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt SmsAtUtil::ConvertAsciiToBinary(const TDesC8& aAscii,TDes8& aData)
	{
	aData.Zero();
	TLex8 lex;
	TUint8 val;
	TInt ret;
	const TInt count(aAscii.Length());
	for(TInt i = 0;i < count;i = i + 2)
		{
		lex = aAscii.Mid(i,2);
		ret = lex.Val(val,EHex);
		if(ret != KErrNone)
			{
			return ret;
			}
		aData.Append(val);
		}
	return KErrNone;
	}

// -------------------------------------------------------------------------------
// SmsAtUtil::AppendAddressToAscii
// other items were commented in a header
// -------------------------------------------------------------------------------
TInt SmsAtUtil::AppendAddressToAscii(TDes8& aAscii,
		                             const RMobilePhone::TMobileAddress& aAddress)
	{
	// Duplicate tel number, removing all the weird chars
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> telNumber;
	const TInt count(aAddress.iTelNumber.Length());
	// Validate the size of the supplied SCA
    if(count >= RMobilePhone::KMaxMobileTelNumberSize)
    	{
    	return KErrOverflow;
    	}
	TInt i;
	for(i = 0;i < count;++i)
		{
		if(IsAddressChar(TChar(aAddress.iTelNumber[i])))
			telNumber.Append(aAddress.iTelNumber[i]);
		}

	const TInt telNumberLength(telNumber.Length());
	
	// Code Address-Length
	AppendOctet(1 + (telNumberLength/2) + (telNumberLength%2),aAscii);

	// Code Type-Of-Address
	TInt typeOfNumber = ConvertTypeOfNumber(aAddress.iTypeOfNumber);
	TInt numberingPlan = ConvertNumberingPlan(aAddress.iNumberPlan);
	AppendOctet(0x80+(typeOfNumber<<4) + (numberingPlan),aAscii);

	// Code Address-Value
	TInt highSemiOctet;
	TInt lowSemiOctet;
	const TInt octets(telNumberLength/2);	// This division will be rounded down
	for(i = 0; i < octets; ++i)
		{
		// See ETSI 03.40 section 9.1.2.3
		// Address digits are coded into octets as pairs.
		lowSemiOctet = ConvertAddressChar(TChar(telNumber[i*2]));
		highSemiOctet = ConvertAddressChar(TChar(telNumber[(i*2)+1]));
		AppendOctet((highSemiOctet<<4)+lowSemiOctet,aAscii);
		}
				
	// If number of semi octects is odd then process the final octet
	if(telNumberLength%2 == 1)		
		{
		lowSemiOctet = ConvertAddressChar(TChar(telNumber[telNumberLength-1]));
		AppendOctet(0xf0+lowSemiOctet,aAscii);
		}
				
	__ASSERT_DEBUG(aAscii.Length()%2 == 0,Panic(EATSmsUtilsOddNumberOfSemiOctets));
	return KErrNone;
	}


// ---------------------------------------------------------------------------
// SmsAtUtil::AppendDataToAscii
// other items were commented in a header
// ---------------------------------------------------------------------------
void SmsAtUtil::AppendDataToAscii(TDes8& aAscii,const TDesC8& aData)
 	{
	const TInt count(aData.Length());
	__ASSERT_DEBUG((aAscii.MaxLength()-aAscii.MaxLength())<=(count*2),Panic(EATSmsUtilsDescriptorOverflow));

	for(TInt i = 0; i < count; ++i)
		{
		AppendOctet(aData[i],aAscii);
		}
		
	__ASSERT_DEBUG(aAscii.Length()%2==0,Panic(EATSmsUtilsOddNumberOfSemiOctets));
	}

// ----------------------------------------------------------------------------------
// SmsAtUtil::ReadAndRemoveAddressFromAscii
// other items were commented in a header
// ----------------------------------------------------------------------------------
TInt SmsAtUtil::ReadAndRemoveAddressFromAscii(TDes8& aAscii,
		                                      RMobilePhone::TMobileAddress& aAddress)
	{
	__ASSERT_DEBUG(aAscii.Length()%2==0,Panic(EATSmsUtilsOddNumberOfSemiOctets));
	TInt ret(KErrNone);

	ret = ReadAddressFromAscii(aAscii,aAddress);
	
	if(ret==KErrNone)
		{
		// Delete address from aAscii (using Address-length at start of string)
		TLex8 lex(aAscii.Mid(0,2));
		TUint val;
		ret=lex.Val(val,EHex);
		if(ret == KErrNone)
			{
			// +1 to include 1 octect of Address-Length 
			val = val+1;		
			
			// double value to change from 'octets used' to 'ASCII chars used'
			val = val*2;
			
			aAscii.Delete(0,val);		
			__ASSERT_DEBUG(aAscii.Length()%2==0,Panic(EATSmsUtilsOddNumberOfSemiOctets));
			}
		}
	return ret;
	}

// ---------------------------------------------------------------------------
// SmsAtUtil::ReadAddressFromAscii
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt SmsAtUtil::ReadAddressFromAscii(const TDesC8& aAscii,
		                             RMobilePhone::TMobileAddress& aAddress)
	{
	TLex8 lex;
	TInt ret;
	TUint8 val;
	
	// get address length + 1 octet Address Type(91)
	lex = aAscii.Mid(0,2);
	ret = lex.Val(val,EHex);
	if(ret != KErrNone)
		{
		return ret;
		}
		
	// we exclude the Address Type as address should not include this 
	TInt addrLen = val-1;

	// Type-Of-Number (see example above as 9)
	lex = aAscii.Mid(2,1);
	ret = lex.Val(val,EHex);
	if(ret != KErrNone)
		{
		return ret;
		}
	
	// Highest bit is not part of Type-Of-Number
	aAddress.iTypeOfNumber = ConvertTypeOfNumber(val&0x07);	

	// Number-Plan
	lex = aAscii.Mid(3,1);
	ret = lex.Val(val,EHex);
	if(ret != KErrNone)
		{
		return ret;
		}
	
	// we got Number-Plan which should be compatiable with Symbian defination
	aAddress.iNumberPlan = ConvertNumberingPlan(val);
	
	// Address (loop for each octet ie. two hex chars from aAscii)
	aAddress.iTelNumber.Zero();
	for(TInt i = 0; i < addrLen; ++i)		
		{
		// Process semi-octet
		lex = aAscii.Mid((i*2)+5,1);
		ret = lex.Val(val,EHex);
		if(ret != KErrNone)
			{
			return ret;
			}
		// 0xF is consider a pedding when the address length is odd number, so we do not need to append it to address field
		if(val < 0xF)
			aAddress.iTelNumber.Append(ConvertAddressChar(val));

		// Process semi-octet
		lex = aAscii.Mid((i*2)+4,1);
		ret = lex.Val(val,EHex);
		if(ret != KErrNone)
			{
			return ret;
			}
		
		// 0xF is consider a pedding when the address length is odd number, so we do not need to append it to address field
		if(val < 0xF)
			aAddress.iTelNumber.Append(ConvertAddressChar(val));
		}
	return KErrNone;
	}

// ----------------------------------------------------------------------------------------
// SmsAtUtil::ReadAndRemoveAddressFromPdu
// other items were commented in a header
// ----------------------------------------------------------------------------------------
void SmsAtUtil::ReadAndRemoveAddressFromPdu(TPtrC8& aPdu, 
		                                    RMobilePhone::TMobileAddress& aGsmServiceCentre)
	{
	const TUint8 KTONBitMask = 0x70;
	const TUint8 KNPIBitMask = 0x0f;
	const TUint8 KTONBitShift = 4;
	const TUint8 KNPIBitShift = 0;
	_LIT(KInternationalPrefix,"+");

	aGsmServiceCentre.iTypeOfNumber = (RMobilePhone::TMobileTON)0;
	aGsmServiceCentre.iNumberPlan = (RMobilePhone::TMobileNPI)0;
	aGsmServiceCentre.iTelNumber.Zero();
	
	TUint8 len = aPdu[0];
	
	if(len == 0)
		{
		// A zero length SCA has been prepended - just strip this first byte off
		aPdu.Set(aPdu.Mid(len+1));
		return;
		}

	TUint8 numDes = aPdu[1];
	aGsmServiceCentre.iTypeOfNumber = (RMobilePhone::TMobileTON)((numDes&KTONBitMask)>>KTONBitShift);
	aGsmServiceCentre.iNumberPlan = (RMobilePhone::TMobileNPI)((numDes&KNPIBitMask)>>KNPIBitShift);

	if(aGsmServiceCentre.iTypeOfNumber == RMobilePhone::EInternationalNumber)
		aGsmServiceCentre.iTelNumber.Append(KInternationalPrefix);

	TInt i;
	TUint16 digit;
	for(i = 2;i<(len+1);i++)
		{
		digit = (TUint16)((aPdu[i]&0x0f) + 0x30);
		aGsmServiceCentre.iTelNumber.Append(digit);
		digit = (TUint16)(((aPdu[i]&0xf0)>>4) + 0x30);
		if(digit == 0x003f)		// 'F' is the padding digit at the end of a number
			break;
		aGsmServiceCentre.iTelNumber.Append(digit);
		}
	aPdu.Set(aPdu.Mid(len + 1));
	}

// ---------------------------------------------------------------------------
// SmsAtUtil::IsAddressChar
// other items were commented in a header
// ---------------------------------------------------------------------------
TBool SmsAtUtil::IsAddressChar(TChar aChar)
 	{
	if(aChar.IsDigit())
		{
		return ETrue;
		}
	if(aChar == TChar('*') ||
	   aChar == TChar('#') ||
	   aChar == TChar('a') ||
	   aChar == TChar('b') ||
	   aChar == TChar('c'))
		{
		return ETrue;
		}
	return EFalse;
	}

// ---------------------------------------------------------------------------
// SmsAtUtil::ConvertNumberingPlan
// other items were commented in a header
// ---------------------------------------------------------------------------
RMobilePhone::TMobileNPI SmsAtUtil::ConvertNumberingPlan(TInt aValue)
	{
	switch(aValue)
		{
		// The below 'magic numbers' come from the ETSI 03.40
		// specification for Address Fields (section 9.1.2.5)
	case 1:
		return RMobilePhone::EIsdnNumberPlan;
	case 3:
		return RMobilePhone::EDataNumberPlan;
	case 4:
		return RMobilePhone::ETelexNumberPlan;
	case 8:
		return RMobilePhone::ENationalNumberPlan;
	case 9:
		return RMobilePhone::EPrivateNumberPlan;
	default:
		return RMobilePhone::EUnknownNumberingPlan;
		}
	}

// ---------------------------------------------------------------------------
// SmsAtUtil::ConvertTypeOfNumber
// other items were commented in a header
// ---------------------------------------------------------------------------
RMobilePhone::TMobileTON SmsAtUtil::ConvertTypeOfNumber(TInt aValue) 
	{
	switch(aValue)
		{
		// The below 'magic numbers' come from the ETSI 03.40
		// specification for Address Fields (section 9.1.2.5)
	case 0:		
		return RMobilePhone::EUnknownNumber;
	case 1:		
		return RMobilePhone::EInternationalNumber;
	case 2:
		return RMobilePhone::ENationalNumber;
	case 3:
		return RMobilePhone::ENetworkSpecificNumber;
	case 4:
		return RMobilePhone::ESubscriberNumber;
	default:
		return RMobilePhone::EUnknownNumber;
		}
	}

// ---------------------------------------------------------------------------
// SmsAtUtil::AppendOctet
// other items were commented in a header
// ---------------------------------------------------------------------------
void SmsAtUtil::AppendOctet(TInt aOctet,TDes8& aAscii)
	{
	// Ensure client has only passed us a octet (ie. low 8 bits only)
	aOctet = aOctet&0xff;
	// Append octet 
	// (prefix '0' if the octets value only uses one digit as final 
    //  octet coding must use two digits)
	if(aOctet <= 0x0f)
		{
		aAscii.Append(TChar('0'));
		}
	
	aAscii.AppendNum(aOctet,EHex);
	}

// ---------------------------------------------------------------------------
// SmsAtUtil::ConvertAddressChar
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt SmsAtUtil::ConvertAddressChar(TChar aChar)
 	{
	aChar.LowerCase();
	if(aChar-TChar('0') <= 9)
		{
		// it should be digital number from 0~9
		return aChar-TChar('0');
		}
	else if(aChar == TChar('*'))
		{
		return 10;
		}
	else if(aChar == TChar('#'))
		{
		return 11;
		}
	else if(aChar == TChar('a'))
		{
		return 12;
		}
	else if(aChar == TChar('b'))
		{
		return 13;
		}
	else if(aChar == TChar('c'))
		{
		return 14;
		}
	return 15;
	}	

// ---------------------------------------------------------------------------
// SmsAtUtil::ConvertAddressChar
// other items were commented in a header
// ---------------------------------------------------------------------------
TChar SmsAtUtil::ConvertAddressChar(TInt aBinary)
	{
	if(aBinary >= 0 && aBinary <= 9)
		{
		return aBinary + TChar('0');  	// Assumes digit characters are one after each other
		}
	else if(aBinary == 10)
		{
		return TChar('*');
		}
	else if(aBinary == 11)
		{
		return TChar('#');
		}
	else if(aBinary == 12)
		{
		return TChar('a');
		}
	else if(aBinary == 13)
		{
		return TChar('b');
		}
	else if(aBinary == 14)
		{
		return TChar('c');
		}
	return TChar(0);		// This is the cloest I can find to a NULL char 
	}

// End of file
