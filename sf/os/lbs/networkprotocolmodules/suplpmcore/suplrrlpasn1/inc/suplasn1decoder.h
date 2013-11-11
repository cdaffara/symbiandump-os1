// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
 
*/
#ifndef SUPL_ASN1_DECODER_H
#define SUPL_ASN1_DECODER_H

#include <e32base.h>

// forward declarations
class CSuplMessageBase;
class CSuplAsn1DecoderImpl;

/**
The CSuplAsn1Decoder class provides an interface to the SUPL Protocol Module
enabling the decoding of ASN1 encoded SUPL messages. Such messages arrive through
either via SMS or WAP push protocols in the case of SUPL INIT messages, or via 
a TCP/IP connection once a SUPL session has been established.

The decoder provides synchronous methods for decoding received messages. Once
Decode() has been called passing the input buffer, the buffer may be re-used as
the decoded data is fully extracted to objects owned by the CSuplMessageBase
derived classes.

On completion of the Decode() operation, the appropriate CSuplMessageBase object
is constructed, encapsulating the decoded message. Ownership of this object is
passed to the calling function.

@internalTechnology
@see CSuplMessageBase
*/
NONSHARABLE_CLASS(CSuplAsn1Decoder) : public CBase
	{
public:
	IMPORT_C static CSuplAsn1Decoder* NewL();
	virtual ~CSuplAsn1Decoder();
	
public:
	IMPORT_C CSuplMessageBase* DecodeL(const TPtrC8* aBuf, TInt& aError);
	
private:
	CSuplAsn1Decoder();
	void ConstructL();
	
	/** Prohibit copy constructor */
	CSuplAsn1Decoder(const CSuplAsn1Decoder&);
	/** Prohibit assigment operator */
	CSuplAsn1Decoder& operator= (const CSuplAsn1Decoder&);

private:
	/** Handle to the Implementation */
	CSuplAsn1DecoderImpl* iImpl;
	};

#endif // SUPL_ASN1_DECODER_H
