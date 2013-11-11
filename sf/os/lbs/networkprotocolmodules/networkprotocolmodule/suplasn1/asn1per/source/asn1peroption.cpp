// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CASN1PEROPTION.CPP
// ASN.1 packed encoding rules - Option
// 
//

#include "asn1per.h"

CAsn1PerEncodingFragment* CAsn1PerOption::EncodeL(TUint aExistingBitLength, const TBool aOption)
	{
	CAsn1PerEncodingFragment* opt=CAsn1PerBoolean::EncodeL(aExistingBitLength,
														aOption,
														EFalse);
	return opt;
	}
	
CAsn1PerEncodingFragment* CAsn1PerOption::EncodeLC(TUint aExistingBitLength, const TBool aOption)
	{
	CAsn1PerEncodingFragment* opt=EncodeL(aExistingBitLength, aOption);
	CleanupStack::PushL(opt);
	return opt;
	}	

void CAsn1PerOption::DecodeL(const TDesC8& aBuf, 
							const TInt aStartBit, 
							TInt& aBitDecoded, 
							TBool& aOption)
	{
	aOption=EFalse;
	aBitDecoded=0;
	CAsn1PerBoolean::DecodeL(aBuf, 
							EFalse, 
							aStartBit, 
							aBitDecoded, 
							aOption);
	}

