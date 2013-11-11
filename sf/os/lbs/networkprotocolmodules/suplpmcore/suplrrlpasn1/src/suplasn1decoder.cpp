// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Location Based Services SUPL ASN1 Decoder API
// 
//

/**
 @file
 @internalTechnology
 
*/

#include <e32base.h>
#include "suplasn1decoder.h"
#include "suplasn1decoderimpl.h"
#include "supldevloggermacros.h" 


/**
Provides SUPL ASN1 decoding functionality to the SUPL Protocol Module.

@return an Instance of the SUPL ASN1 decoder. The calling application becomes 
the owner of the returned instance and is responsible for its disposal.

@internalTechnology
*/
EXPORT_C CSuplAsn1Decoder* CSuplAsn1Decoder::NewL()
	{
	SUPLLOG(ELogP1, "CSuplAsn1Decoder::NewL() Begin\n");
	CSuplAsn1Decoder* newObj = new (ELeave) CSuplAsn1Decoder();
	CleanupStack::PushL(newObj);
	newObj->ConstructL();
	SUPLLOG(ELogP1, "CSuplAsn1Decoder::NewL() End\n");
	CleanupStack::Pop(newObj);
	return newObj;
	}
	
/**
Destructor

@internalTechnology
*/
CSuplAsn1Decoder::~CSuplAsn1Decoder()
	{
	SUPLLOG(ELogP1, "CSuplAsn1Decoder::~CSuplAsn1Decoder() Begin\n");
	delete iImpl;
	iImpl = NULL;
	SUPLLOG(ELogP1, "CSuplAsn1Decoder::~CSuplAsn1Decoder() End\n");
	}
	
/**
Default constructor.

@internalTechnology
*/
CSuplAsn1Decoder::CSuplAsn1Decoder()
	{
	}

/**
2nd phase constructor. 
Creates and assigns all the required internal resources.

@internalTechnology

*/
void CSuplAsn1Decoder::ConstructL()
	{
	iImpl = CSuplAsn1DecoderImpl::NewL();
	}

/**
Passes an ASN1 encoded SUPL message to be decoded.

A system requiring concurrent access to a channel must provide its own access 
control mechanism.

@return a CSuplMessageBase derived object encapsulating the decoded message
data. The calling application becomes the owner of the returned object and 
is responsible for its disposal.

@internalTechnology

*/
EXPORT_C CSuplMessageBase* CSuplAsn1Decoder::DecodeL(const TPtrC8* aBuf, TInt& aError)
	{
	return iImpl->DecodeL(aBuf, aError);
	}

