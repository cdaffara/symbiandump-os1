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
//

/**
 @file
 @internalTechnology
 
*/
#ifndef SUPL_ASN1_ERROR_H
#define SUPL_ASN1_ERROR_H

/** TSuplAsn1Error typedef.
@internalTechnology
*/
typedef TUint32 TSuplAsn1Error;

/**
Error codes that may be returned during ASN1 operations

Note that positive error codes in the region 0 < err < KLbsAsn1ErrorBase
are errors returned by calls to the encode/decode operations, and represent
failure to encode/decode the ASN1 stream as indicated by the error codes
described in rtxErrCodes.h and asn1ErrCodes.h. Note that these error codes
are negated (made positive) before return to calling code to differentiate 
them from standard Symbian error codes.

The exception to the above is when a memory allocation has failed, in this
case the standard Symbian error code KErrNoMemory is returned.

@see rtxErrCodes.h
@see asn1ErrCodes.h

@internalTechnology
*/
enum _TSuplAsn1Error
	{
		/** Not used */
	ESuplAsn1ErrBase 				  	   = 1000,
		/** A SUPL message that is not supported by this implementation */
	ESuplAsn1ErrUnsupportedSuplMessage     = 1001,
		/** A pos payload of a protocol that is not supported by this implementation */
	ESuplAsn1ErrUnsupportedPosProtocol     = 1002,
		/** A pos payload message type that is not supported by this implementation */
	ESuplAsn1ErrUnsupportedPosMessageType  = 1003
	};

#endif // SUPL_ASN1_ERROR_H

