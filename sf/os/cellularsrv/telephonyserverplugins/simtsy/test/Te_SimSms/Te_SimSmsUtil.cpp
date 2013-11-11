// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <etelmm.h> // for RMobilePhone::TMobileAddress
#include "Te_SimSmsUtil.h"
#include "Te_SimSmsPanic.h"

//
// Currently this file can not include the matstd file due to a dependency that
// the test code t_sms has on this. So the constants needed are duplicated below.
_LIT8(KMEStorage,"ME");
_LIT8(KMTStorage,"MT");
_LIT8(KSMStorage,"SM");

//
// Panic Function
//
GLDEF_C void Panic(TTSYPanic aPanicNumber)
	{
	_LIT(panicText,"MM TSY");
	User::Panic(panicText,aPanicNumber);
	}

RMobilePhone::TMobileTON CATSmsUtils::ConvertTypeOfNumber(TInt aValue) 
/**
 * @param aValue should be an ETSI Type-Of-Number field
 * @return The equivalent RMobilePhone::TMobileTON value for aValue
 */
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

TInt CATSmsUtils::ConvertTypeOfNumber(RMobilePhone::TMobileTON aEnum) 
/**
 * @param aEnum must be a n RMobile::TMobileTON value 
 * @return The equivalent ETSI Type-Of-Number value for aEnum
 */
	{
	switch(aEnum)
		{
		// The below 'magic numbers' come from the ETSI 03.40
		// specification for Address Fields (section 9.1.2.5)
	case RMobilePhone::EInternationalNumber:
		return 1;
	case RMobilePhone::ENationalNumber:
		return 2;
	case RMobilePhone::ENetworkSpecificNumber:
		return 3;
	case RMobilePhone::ESubscriberNumber:
		return 4;
	case RMobilePhone::EUnknownNumber:
	case RMobilePhone::EAbbreviatedNumber:
	default:
		return 0;
		}
	}

RMobilePhone::TMobileNPI CATSmsUtils::ConvertNumberingPlan(TInt aValue)
/**
 * @param aValue should be an ETSI Numbering-Plan-Identification field
 * @return The equivalent RMobilePhone::TMobileNPI value for aValue
 */
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


TInt CATSmsUtils::ConvertNumberingPlan(RMobilePhone::TMobileNPI aEnum)  
/**
 * @param aEnum must be a n RMobile::TMobileNPI value 
 * @return The equivalent ETSI Numbering-Plan-Identification value for aEnum
 */
	{
	switch(aEnum)
		{
		// The below 'magic numbers' come from the ETSI 03.40
		// specification for Address Fields (section 9.1.2.5)
	case RMobilePhone::EIsdnNumberPlan:
		return 1;
	case RMobilePhone::EDataNumberPlan:
		return 3;
	case RMobilePhone::ETelexNumberPlan:
		return 4;
	case RMobilePhone::ENationalNumberPlan:
		return 8;
	case RMobilePhone::EPrivateNumberPlan:
		return 9;
	case RMobilePhone::EUnknownNumberingPlan:
	default:
		return 0;
		}
	}



TInt CATSmsUtils::AppendAddressToAscii(TDes8& aAscii,const RMobilePhone::TMobileAddress& aAddress,TBool aUse0340Format)
/**
 * Default operation is to code Address-Length according to=
 * 04.11 spec (ie. Address-Length=number of digits in Address-Value).
 *
 * If aUse0340Format argument is ETrue then Address Length will be coded using 
 * 03.40 format (ie. Address-Length=number of octets used for Address-Type
 * and Address-Value). 03.40 is typically only used when prefixing an SCA to a PDU.
 *
 * @return Standard KErr... values
 */
	{
	// Duplicate tel number, removing all the weird chars
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> telNumber;
	const TInt count(aAddress.iTelNumber.Length());
	TInt i;
	for(i=0;i<count;++i)
		{
		if(IsAddressChar(TChar(aAddress.iTelNumber[i])))
			telNumber.Append(aAddress.iTelNumber[i]);
		}

	const TInt telNumberLength(telNumber.Length());

	// Validate the size of the supplied SCA

	// Calculate the number of ascii chars we'll need
	// We need 4 chars to code the Address-Length and Address-Type fields.
	// We need to add on an extra 'padding' char if the total number of chars is odd.
	const TInt neededAsciiChars=(4+telNumberLength)+(telNumberLength%2);
	if((aAscii.MaxLength()-aAscii.Length())<neededAsciiChars)	
		return KErrOverflow;

	// Code Address-Length
	if(aUse0340Format)
		AppendOctet(1+(telNumberLength/2)+(telNumberLength%2),aAscii);
	else
		AppendOctet(telNumberLength,aAscii);

	// Code Type-Of-Address
	TInt typeOfNumber=ConvertTypeOfNumber(aAddress.iTypeOfNumber);
	TInt numberingPlan=ConvertNumberingPlan(aAddress.iNumberPlan);
	AppendOctet(0x80+(typeOfNumber<<4)+(numberingPlan),aAscii);

	// Code Address-Value
	TInt highSemiOctet;
	TInt lowSemiOctet;
	const TInt octets(telNumberLength/2);	// This division will be rounded down
	for(i=0;i<octets;++i)
		{
		// See ETSI 03.40 section 9.1.2.3
		// Address digits are coded into octets as pairs.
		lowSemiOctet=ConvertAddressChar(TChar(telNumber[i*2]));
		highSemiOctet=ConvertAddressChar(TChar(telNumber[(i*2)+1]));
		AppendOctet((highSemiOctet<<4)+lowSemiOctet,aAscii);
		}
				
	// If number of semi octects is odd then process the final octet
	if(telNumberLength%2==1)		
		{
		lowSemiOctet=ConvertAddressChar(TChar(telNumber[telNumberLength-1]));
		AppendOctet(0xf0+lowSemiOctet,aAscii);
		}
				
	__ASSERT_DEBUG(aAscii.Length()%2==0,Panic(EATSmsUtilsOddNumberOfSemiOctets));
	return KErrNone;
	}


void CATSmsUtils::AppendOctet(TInt aOctet,TDes8& aAscii)
/**
 * Converts a TInt octet value into ASCII representation and then appends that
 * ASCII representation to the end of the given ASCII string.
 *
 * @param aOctet the octet value to append
 * @param aAscii the ASCII string to which aOctet value should be appended
 */
	{
	// Ensure client has only passed us a octet (ie. low 8 bits only)
	aOctet=aOctet&0xff;

	// Append octet 
	// (prefix '0' if the octets value only uses one digit as final octet coding must use two digits)
	if(aOctet<=0x0f)
		aAscii.Append(TChar('0'));
	aAscii.AppendNum(aOctet,EHex);
	}


TInt CATSmsUtils::ConvertAddressChar(TChar aChar)
/**
 * Returns the equivalent numeric value for a given ASCII address character.
 *
 * @param aChar the address character to be converted
 * @return The numeric value equivalent of the given address character.
 */
 	{
	aChar.LowerCase();
	if(aChar-TChar('0')<=9)  
		return aChar-TChar('0');		// Assumes digit characters are one after each other
	else if(aChar==TChar('*'))
		return 10;
	else if(aChar==TChar('#'))
		return 11;
	else if(aChar==TChar('a'))
		return 12;
	else if(aChar==TChar('b'))
		return 13;
	else if(aChar==TChar('c'))
		return 14;
	return 15;
	}
			
TChar CATSmsUtils::ConvertAddressChar(TInt aBinary)
/**
 * Returns the equivalent ASCII address character for a given address value.
 *
 * @param aBinary the numerix value of the address character to be returned
 * @return The ASCII charcater which represents the given address numeric value.
 */
	{
	if(aBinary>=0 && aBinary<=9)
		return TChar(aBinary)+TChar('0');  	// Assumes digit characters are one after each other
	else if(aBinary==10)
		return TChar('*');
	else if(aBinary==11)
		return TChar('#');
	else if(aBinary==12)
		return TChar('a');
	else if(aBinary==13)
		return TChar('b');
	else if(aBinary==14)
		return TChar('c');
	return TChar(0);		// This is the cloest I can find to a NULL char 
	}



TBool CATSmsUtils::IsAddressChar(TChar aChar)
/**
 * Returns ETrue if, and only if, the given ASCII charcater is valid as an ASCII address character.
 */
 	{
	if(aChar.IsDigit())
		return ETrue;
	if(aChar==TChar('*') ||
	   aChar==TChar('#') ||
	   aChar==TChar('a') ||
	   aChar==TChar('b') ||
	   aChar==TChar('c'))
		return ETrue;
	return EFalse;
	}


void CATSmsUtils::AppendDataToAscii(TDes8& aAscii,const TDesC8& aData)
/**
 * Appends the binary data (aData) onto the end of an ASCII string (aAscii) in ASCII format.
 */
 	{
	const TInt count(aData.Length());
	__ASSERT_DEBUG((aAscii.MaxLength()-aAscii.MaxLength())<=(count*2),Panic(EATSmsUtilsDescriptorOverflow));

	for(TInt i=0;i<count;++i)
		AppendOctet(aData[i],aAscii);

	__ASSERT_DEBUG(aAscii.Length()%2==0,Panic(EATSmsUtilsOddNumberOfSemiOctets));
	}



TInt CATSmsUtils::ConvertAsciiToBinary(const TDesC8& aAscii,TDes8& aData)
/**
 * Converts aAscii ASCII chars to Semi Octets in aData.
 * One ASCII char (8bits in aAscii) is translated to one Semi-Octet (4bits in aData).
 * @return Standard KErr... values 
 */
	{
	__ASSERT_DEBUG(aAscii.Length()%2==0,Panic(EATSmsUtilsOddNumberOfSemiOctets));
	__ASSERT_DEBUG(aData.MaxLength()>=(aAscii.Length()/2),Panic(EATSmsUtilsDescriptorOverflow));

	aData.Zero();
	TLex8 lex;
	TUint8 val;
	TInt ret;
	const TInt count(aAscii.Length());
	for(TInt i=0;i<count;i=i+2)
		{
		lex=aAscii.Mid(i,2);
		ret=lex.Val(val,EHex);
		if(ret!=KErrNone)
			return ret;

		aData.Append(val);
		}
		
	return KErrNone;
	}


TInt CATSmsUtils::ReadAddressFromAscii(const TDesC8& aAscii,RMobilePhone::TMobileAddress& aAddress,TBool aPduUses0340Format)
/**
 * Default operation assumes has prefixed SCA which uses 04.11 format
 * (ie. Address-Length=number of digits in Address-Value).
 *
 * If aUse0340Format argument is ETrue then Address Length will be coded using 
 * 03.40 format (ie. Address-Length=number of octets used for Address-Type
 * and Address-Value). 03.40 is typically only used when prefixing an SCA to a PDU.
 *
 * @return Standard KErr... values
 */
	{
	TLex8 lex;
	TInt ret;
	TUint8 val;

	// Address-length
	lex=aAscii.Mid(0,2);
	ret=lex.Val(val,EHex);
	if(ret!=KErrNone)
		return ret;
	
	TInt addrLen;	// Will hold number of octets used to code Address-Value
	if(aPduUses0340Format)
		addrLen=val-1;
	else
		addrLen=(val/2)+(val%2);

	// Type-Of-Number
	lex=aAscii.Mid(2,1);
	ret=lex.Val(val,EHex);
	if(ret!=KErrNone)
		return ret;
	aAddress.iTypeOfNumber=ConvertTypeOfNumber(val&0x07);	// Highest bit is not part of Type-Of-Number

	// Number-Plan
	lex=aAscii.Mid(3,1);
	ret=lex.Val(val,EHex);
	if(ret!=KErrNone)
		return ret;
	aAddress.iNumberPlan=ConvertNumberingPlan(val);
	
	// Address (loop for each octet ie. two hex chars from aAscii)
	aAddress.iTelNumber.Zero();
	for(TInt i=0;i<addrLen;++i)		
		{
		// Process semi-octet
		lex=aAscii.Mid((i*2)+5,1);
		ret=lex.Val(val,EHex);
		if(ret!=KErrNone)
			return ret;
		if(val<=14)
			aAddress.iTelNumber.Append(ConvertAddressChar(val));

		// Process semi-octet
		lex=aAscii.Mid((i*2)+4,1);
		ret=lex.Val(val,EHex);
		if(ret!=KErrNone)
			return ret;
		if(val<=14)
			aAddress.iTelNumber.Append(ConvertAddressChar(val));
		}

	return KErrNone;
	}
	

TInt CATSmsUtils::ReadAndRemoveAddressFromAscii(TDes8& aAscii,RMobilePhone::TMobileAddress& aAddress,TBool aPduUses0340Format)
/**
 * Reads an address from the front of the ASCII string (aAscii) and fills up Address structure (aAddress).
 * The address read from the ASCII string is removed from the ASCII string.
 */
	{
	__ASSERT_DEBUG(aAscii.Length()%2==0,Panic(EATSmsUtilsOddNumberOfSemiOctets));
	TInt ret(KErrNone);

	ret=ReadAddressFromAscii(aAscii,aAddress,aPduUses0340Format);
	if(ret==KErrNone)
		{
		// Delete address from aAscii (using Address-length at start of string)
		TLex8 lex(aAscii.Mid(0,2));
		TUint val;
		ret=lex.Val(val,EHex);
		if(ret==KErrNone)
			{
			if(aPduUses0340Format)
				{
				// +1 to include Address-Length octect
				val=val+1;		
				
				// double value to change from 'octets used' to 'ASCII chars used'
				val=val*2;
				}
			else	
				{
				// Allow for the case where the last digit is actually a dummy digit
				if(val%2!=0)
					++val;
				// +4 to include the Address-Length and Address-Type octets
				val=val+4; 
				}

			aAscii.Delete(0,val);		
			__ASSERT_DEBUG(aAscii.Length()%2==0,Panic(EATSmsUtilsOddNumberOfSemiOctets));
			}
		}
			
	return ret;
	}


TInt CATSmsUtils::CopyAddressStringToAddressStruct(const TDesC8& aAddressValueString,const TDesC8& aAddressTypeString,RMobilePhone::TMobileAddress& aAddress)
/**
 * Analyse rx results for a CSCA response and attempt to parse into provided tel
 * number object
 * An example CSCA response would be '+CSCA: "00447785016005",129'.
 */
	{
	// Process Address-Type
	TLex8 lex(aAddressTypeString);
	TUint8 val;
	TInt ret=lex.Val(val,EDecimal);
	if(ret!=KErrNone)
		return ret;
	
	aAddress.iTypeOfNumber=ConvertTypeOfNumber((TInt)((val&0x70)>>4));
	aAddress.iNumberPlan=ConvertNumberingPlan((TInt)(val&0x0f));
	
	// Process Address-Value, filtering out non telephone chars
	aAddress.iTelNumber.Zero();
	const TInt len=aAddressValueString.Length();
	TChar c;
	for(TInt i=0;i<len;++i)
		{
		c=aAddressValueString[i];
		if(IsAddressChar(c))
			aAddress.iTelNumber.Append(c);
		}
			
	return KErrNone;
	}


void CATSmsUtils::GetTypeOfAddressInDecimal(const RMobilePhone::TMobileAddress& aAddress, TUint& aVal)
/**
 * Reads the type-of-number and numbering-plan contents of the address structure (aAddress) and
 * saves the equivalent decimal value in aVal.
 *
 * For example...
 *  if type-of-number is 'international' and numbering-plan is 'telephone' then aVal will be 145.
 *  if type-of-number is 'unknown' and numbering-plan is 'telephone' then aVal will be 129.
 */
 	{
	const TInt typeOfNumber=ConvertTypeOfNumber(aAddress.iTypeOfNumber);
	const TInt numberingPlan=ConvertNumberingPlan(aAddress.iNumberPlan);
	aVal=0x80+(typeOfNumber<<4)+numberingPlan;
	}


void CATSmsUtils::SetTypeOfAddressFromDecimal(RMobilePhone::TMobileAddress& aAddress,const TUint& aVal)
/**
 * Uses the aVal decimal value and set the type-of-number and numbering-plan fields
 * of aAddress accordingly.
 * This is the opposite functionality to ::GetTypeOfAddressInDecimal
 */
	{
	aAddress.iTypeOfNumber=ConvertTypeOfNumber((aVal&0x70)>>4);
	aAddress.iNumberPlan=ConvertNumberingPlan(aVal&0x0f);
	}



void CATSmsUtils::ConvertStoreNameToPhoneVersion(const TDesC& aEtelMMVersion,TDes8& aPhoneVersion)
/**
 * Convert from Etel MultiMode API SMS store names to ETSI GSM store names
 */
 	{
	if(aEtelMMVersion.Compare(KETelMeSmsStore)==0)
		{
		aPhoneVersion.Copy(KMEStorage);
		return;
		}
	if(aEtelMMVersion.Compare(KETelIccSmsStore)==0)
		{
		aPhoneVersion.Copy(KSMStorage);
		return;
		}
	if(aEtelMMVersion.Compare(KETelCombinedSmsStore)==0)
		{
		aPhoneVersion.Copy(KMTStorage);
		return;
		}

	// Panic if we are in debug build
	__ASSERT_DEBUG(EFalse,Panic(EATSmsUtilsUnknownStoreName));

	// Return the combined store if we are in release build
	aPhoneVersion.Copy(KMTStorage);
	}


void CATSmsUtils::ConvertStoreNameToEtelMMVersion(TDes& aEtelMMVersion,const TDesC8& aPhoneVersion)
/**
 * Convert from ETSI GSM store names to Etel MultiMode API SMS store names
 */
	{
	if(aPhoneVersion.Compare(KMEStorage)==0)
		{
		aEtelMMVersion.Copy(KETelMeSmsStore);
		return;
		}
	if(aPhoneVersion.Compare(KSMStorage)==0)
		{
		aEtelMMVersion.Copy(KETelIccSmsStore);
		return;
		}
	if(aPhoneVersion.Compare(KMTStorage)==0)
		{
		aEtelMMVersion.Copy(KETelCombinedSmsStore);
		return;
		}

	// Panic if we are in debug build
	__ASSERT_DEBUG(EFalse,Panic(EATSmsUtilsUnknownStoreName));

	// Return the combined store if we are in release build
	aEtelMMVersion.Copy(KETelCombinedSmsStore);
	}

