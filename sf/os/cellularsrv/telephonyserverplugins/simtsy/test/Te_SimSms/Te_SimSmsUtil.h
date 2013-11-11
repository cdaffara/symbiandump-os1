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
 @internalComponent 
*/

#ifndef __T_SMSUTIL_H__
#define	__T_SMSUTIL_H__

#include <etelmm.h>		// for RMobilePhone::TMobileAddress

const TInt KMaxAsciiPduLength=400;	
const TInt KMaxAsciiSCALength=80;	

class CATParamListEntry;

class CATSmsUtils : public CBase
	{
public:
	static TInt AppendAddressToAscii(TDes8& aAscii,const RMobilePhone::TMobileAddress& aAddress,TBool aUse0340Format=EFalse);
	static void AppendDataToAscii(TDes8& aAscii,const TDesC8& aData);
	static TInt ConvertAsciiToBinary(const TDesC8& aAscii,TDes8& aData);
	static TInt ReadAddressFromAscii(const TDesC8& aAscii,RMobilePhone::TMobileAddress& aAddress,TBool aPduUses0340Format=EFalse);
	static TInt ReadAndRemoveAddressFromAscii(TDes8& aAscii,RMobilePhone::TMobileAddress& aAddress,TBool aPduUses0340Format=EFalse);
	static TInt CopyAddressStringToAddressStruct(const TDesC8& aAddressValueString,const TDesC8& aAddressTypeString,RMobilePhone::TMobileAddress& aAddress);
	static void GetTypeOfAddressInDecimal(const RMobilePhone::TMobileAddress& aAddress, TUint& aVal);
	static void SetTypeOfAddressFromDecimal(RMobilePhone::TMobileAddress& aAddress,const TUint& aVal);
	static TBool IsAddressChar(TChar aChar);
	static void ConvertStoreNameToEtelMMVersion(TDes& aEtelMMVersion,const TDesC8& aPhoneVersion);
	static void ConvertStoreNameToPhoneVersion(const TDesC& aEtelMMVersion,TDes8& aPhoneVersion);


private:
	// Enum conversion functions 
	static RMobilePhone::TMobileTON ConvertTypeOfNumber(TInt aValue);
	static TInt ConvertTypeOfNumber(RMobilePhone::TMobileTON aEnum);
	static RMobilePhone::TMobileNPI ConvertNumberingPlan(TInt aValue);
	static TInt ConvertNumberingPlan(RMobilePhone::TMobileNPI aEnum);

	// ASCII/Binary conversion functions
	static void AppendOctet(TInt aOctet,TDes8& aAscii);
	static TInt ConvertAddressChar(TChar aChar);
	static TChar ConvertAddressChar(TInt aBinary);
	};





#endif
