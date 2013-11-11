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
// @file testltsysmsatutil.cpp
// This contains SmsAtUtil which is util for Sms At command
// 

// user include
#include "testltsysmsatutil.h"

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

// ---------------------------------------------------------------------------
// SmsAtUtil::AppendDataToAscii
// other items were commented in a header
// ---------------------------------------------------------------------------
void SmsAtUtil::AppendDataToAscii(TDes8& aAscii,const TDesC8& aData)
 	{
	const TInt count(aData.Length());
//	__ASSERT_DEBUG((aAscii.MaxLength()-aAscii.MaxLength())<=(count*2),Panic(EATSmsUtilsDescriptorOverflow));

	for(TInt i = 0; i < count; ++i)
		{
		AppendOctet(aData[i],aAscii);
		}
		
//	__ASSERT_DEBUG(aAscii.Length()%2==0,Panic(EATSmsUtilsOddNumberOfSemiOctets));
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
		return TChar(aBinary) + TChar('0');  	// Assumes digit characters are one after each other
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
