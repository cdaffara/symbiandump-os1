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

/**
 @file
 @internalAll 
*/


#ifndef __SIM_UTILS_H__
#define __SIM_UTILS_H__

#include <etel.h>
#include <et_phone.h>
#include <etelmm.h>
#include <etelpckt.h>

// This should always be used for calling the ReqCompleted() func when possible
#define SYMBIAN_EXTERR(aExtended, aBasic) ((aExtended << 16) | (aBasic & 0x0000FFFF))
#define SYMBIAN_REQEXTERR(aHandle, aExtended, aBasic) \
		ReqCompleted(aHandle, SYMBIAN_EXTERR(aExtended, aBasic));

RCall::THookStatus ConvertStateToHook(RMobileCall::TMobileCallStatus aStatus);
RPhone::TMode ConvertStateToMode(RMobileCall::TMobileCallStatus aStatus);
TBool IsStateActive(RMobileCall::TMobileCallStatus aStatus);
void ConvertAsciiSms(const TDesC8& aAsciiPdu, TDes8& aOctetPdu);
void ConvertTextToBinary(const TDesC& aSrc, TDes8& aDst);
TInt AsciiToNum(const TPtrC8 aParam, TUint8& aVal);
TInt AsciiToNum(const TPtrC8 aParam, TUint16& aVal);
TInt AsciiToNum(const TPtrC8 aParam, TUint32& aVal);
TInt AsciiToNum(const TPtrC8 aParam, TInt32& aVal);
TInt ParseMixedBinaryAsciiDataL(TDes8& aTextToConvert);
TInt CharToSeptNumL(const TChar& aChar);

inline TInt AsciiHexToNum(const TPtrC8 aParam, TUint8& aVal);

// Most CSim Class will need this notification data.
struct TNotifyData
	{	
	TBool iNotifyPending;  //indicates if a notification is pending
	TTsyReqHandle iNotifyHandle; //the TSY notification handle
	TAny* iNotifyData;  //Holds the data for the notification pending
	};

/**
Used to specify format of the additional parameter in the config file that will 
go into the PCO buffer.
*/
enum TConfigDataFormat
	{
	EConfigDataFormatAscii,                 // 0
	EConfigDataFormatMixedBinaryAndAscii,   // 1
	EMaxConfigDataFormat
	};

// Inline functions

inline TInt AsciiHexToNum(const TPtrC8 aParam, TUint8& aVal)
/**
* Converts a hexadecimal value stored in a TPtrC to its TUint value
*
* @param aParam		TPtr pointing to the hexadecimal or number to convert
* @param aVal		TUint reference to the val converted.
* @return TInt		Standard error return.
*/
	{
	TLex8 lex(aParam.Left(2));
	return lex.Val(aVal,EHex);
	}
	
#endif // __SIM_UTILS_H__
