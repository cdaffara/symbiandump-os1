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
// @file smsatutil.h
// This contains SmsAtUtil which is util for Sms At command
// 

#ifndef SMSATUTIL_H
#define SMSATUTIL_H
// system include
#include <e32std.h>
#include <e32base.h>
#include <etelmm.h>


/**
*  SmsAtUtil
* 
*/
class SmsAtUtil : public CBase
	{
public:
	
	 /**
	 * Converts aAscii ASCII chars to Semi Octets in aData.
	 * One ASCII char (8bits in aAscii) is translated to one Semi-Octet (4bits in aData).
	 * 
	 * @param aAscii input ascii string
	 * @param aData  output binary data 
	 * @return Standard KErr... values 
	 */	
	static TInt ConvertAsciiToBinary(const TDesC8& aAscii,TDes8& aData);
	
	 /**
	 * Default operation is to code Address-Length according to=
	 * 04.11 spec (ie. Address-Length=number of digits in Address-Value).
	 * Append this sca adress to the ascii string  
	 *
	 * @param aAscii an ascii string 
	 * @param aAddress  a SCA adress 
	 * @return Standard KErr... values
	 */
	static TInt AppendAddressToAscii(TDes8& aAscii,const RMobilePhone::TMobileAddress& aAddress);
	
	 /**
	 * Appends the binary data (aData) onto the end of an ASCII string (aAscii) in ASCII format.
	 * 
	 * @param aAscii  an ascii string 
	 * @param aData   a  binary string 
	 */
	static void AppendDataToAscii(TDes8& aAscii,const TDesC8& aData);
	
	 /**
	 * Reads an address from the front of the ASCII string (aAscii) and fills up Address structure (aAddress).
	 * The address read from the ASCII string is removed from the ASCII string.
	 * 
	 * @param aAscii an ascii string 
	 * @param aAddress  a SCA address
	 * @return Standard KErr... values
	 */
	static TInt ReadAndRemoveAddressFromAscii(TDes8& aAscii,RMobilePhone::TMobileAddress& aAddress);
	
	/**
	 * Default operation assumes has prefixed SCA which uses 04.11 format
	 * (ie. Address-Length=number of digits in Address-Value).
	 *
	 * @param aAscii an ascii string 
	 * @param aAddress  a SCA address
	 * @return Standard KErr... values
	 */
	static TInt ReadAddressFromAscii(const TDesC8& aAscii,RMobilePhone::TMobileAddress& aAddress);
	
	 /**
	 *  get the SCA field in the aAscii structure from the SCA prepended in the PDU sring .
	 *  
	 *  @param aPdu  a PDU string 
	 *  @param aAddress a SCA address
	 */
	static void ReadAndRemoveAddressFromPdu(TPtrC8& aPdu, RMobilePhone::TMobileAddress& aGsmServiceCentre);
	 /**
	 * Judge whether a char is an address char
	 * 
	 * @param  aChar  
	 * @return ETrue if, and only if, the given ASCII charcater 
	 *  is valid as an ASCII address character.
	 */
	static TBool IsAddressChar(TChar aChar);

private:
	
	 /**
	 * @param aValue should be an ETSI Numbering-Plan-Identification field
	 * @return The equivalent RMobilePhone::TMobileNPI value for aValue
	 */
	static RMobilePhone::TMobileNPI ConvertNumberingPlan(TInt aValue);
	
	 /**
	 * @param aValue should be an ETSI Type-Of-Number field
	 * @return The equivalent RMobilePhone::TMobileTON value for aValue
	 */
	static RMobilePhone::TMobileTON ConvertTypeOfNumber(TInt aValue); 
	
	 /**
	 * Converts a TInt octet value into ASCII representation and then appends that
	 * ASCII representation to the end of the given ASCII string.
	 *
	 * @param aOctet the octet value to append
	 * @param aAscii the ASCII string to which aOctet value should be appended
	 */	
	static void AppendOctet(TInt aOctet,TDes8& aAscii);
	
	 /**
	 * Returns the equivalent numeric value for a given ASCII address character.
	 *
	 * @param aChar the address character to be converted
	 * @return The numeric value equivalent of the given address character.
	 */
	static TInt ConvertAddressChar(TChar aChar);
	
	 /**
	 * Returns the equivalent ASCII address character for a given address value.
	 *
	 * @param aBinary the numerix value of the address character to be returned
	 * @return The ASCII charcater which represents the given address numeric value.
	 */
	static TChar ConvertAddressChar(TInt aBinary);
	
	};

#endif // SMSATUTIL_H
