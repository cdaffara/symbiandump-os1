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
// ASN.1 packed encoding rules - Encoding & Decoding of Choice value
// 
//

#include "asn1per.h"



CAsn1PerEncodingFragment* CAsn1PerChoice::EncodeL(TUint aExistingBitLength,
												const TInt aChoice,
												const TInt aMaxChoice,
												const TBool aIsExtensible,
												const TBool aIsExtended)
	{
	return CAsn1PerEnum::EncodeL(aExistingBitLength,
								aChoice,
								aMaxChoice,
								aIsExtensible,
								aIsExtended,
								EFalse
								);		
	}

CAsn1PerEncodingFragment* CAsn1PerChoice::EncodeLC(TUint aExistingBitLength,
												const TInt aChoice,
												const TInt aMaxChoice,
												const TBool aIsExtensible,
												const TBool aIsExtended)
	{
	CAsn1PerEncodingFragment* enc=CAsn1PerEnum::EncodeL(aExistingBitLength,
														aChoice,
														aMaxChoice,
														aIsExtensible,
														aIsExtended,
														EFalse
														);
	CleanupStack::PushL(enc);
	return enc;
	}




void CAsn1PerChoice::DecodeL(const TDesC8& aBuf,
							const TInt aStartBit,
							const TInt aMaxChoice,
							const TBool aIsExtensible,
							TInt& aBitsDecoded,
							TBool& aIsExtended,
							TInt& aChoiceValue
							)
							
	{
	TBool isPadded(EFalse);
	CAsn1PerEnum::DecodeL(aBuf,
						aStartBit,
						aMaxChoice,
						aIsExtensible,
						isPadded,
						aBitsDecoded,
						aIsExtended,
						aChoiceValue
						);		
	}




