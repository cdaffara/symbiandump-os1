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
// Internal implementation of the SUPL ASN1 Decoder
// 
//

/**
 @file
 @internalComponent
 
*/
#ifndef SUPL_ASN1_DECODER_IMPL_H
#define SUPL_ASN1_DECODER_IMPL_H

#include <e32std.h>
#include "suplasn1decoder.h"

// forward declarations
class CSuplMessageBase;
class ASN1PERDecodeBuffer;
class ASN1T_ULP_PDU;
class ASN1C_ULP_PDU;

/**
The class provides an internal implementation of the functionality exposed 
by the CSuplAsn1Decoder interface.

@see CSuplAsn1Decoder
@see CSuplMessageBase

@internalComponent
*/
NONSHARABLE_CLASS(CSuplAsn1DecoderImpl) : public CBase
	{
public:
	IMPORT_C static CSuplAsn1DecoderImpl* NewL();
	virtual ~CSuplAsn1DecoderImpl();

public:
	IMPORT_C CSuplMessageBase* DecodeL(const TPtrC8* aBuf, TInt& aError);
	
private:
	CSuplAsn1DecoderImpl();
	void ConstructL();
	
	TInt ProcessAsn1Error(TInt aError);
	TInt AdditionalMessageValidation();
private:
	// Objective Systems compiled object types.
	// Note that these data members object names do not meet Symbian's coding
	// standards as they are created using a third party ASN1 compilation tool.
	
	// ASN1 Decode Buffer Management
	ASN1PERDecodeBuffer* iDecodeBuffer;
	
	// SUPL message data structure and C++ management class for the data.
	// These objects are owned during the decode process. Ownership is passed
	// to a constructed CSuplMessageBase derived wrapper class, which is
	// returned to the calling function on completion of the decode operation.
	ASN1T_ULP_PDU* iData;
	ASN1C_ULP_PDU* iControl;
	};

#endif //SUPL_ASN1_DECODER_IMPL_H
