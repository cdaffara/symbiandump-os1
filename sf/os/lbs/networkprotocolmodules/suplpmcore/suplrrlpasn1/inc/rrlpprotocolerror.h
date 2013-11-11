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
#ifndef RRLP_POROTOCOL_ERROR_H
#define RRLP_POROTOCOL_ERROR_H

#include "rrlpmessagebase.h"

/** TRrlpErrorCode typedef.
@internalTechnology
*/
typedef TUint32 TRrlpErrorCode;

/**
Enumeration for RRLP Error Code

@internalTechnology
*/
enum _TRrlpErrorCode
	{
	ERrlpErrorUnDefined = 0,
	ERrlpErrorMissingComponet = 1,			
	ERrlpErrorIncorrectData = 2,			
	ERrlpErrorMissingIEorComponentElement = 3,		
	ERrlpErrorMessageTooShort = 4,			
	ERrlpErrorUnknowReferenceNumber = 5	
	};


/**
Encapsulation for outgoing/received RRLP Protocol Error messages.

@internalTechnology
*/
NONSHARABLE_CLASS(CRrlpProtocolError) : public CRrlpMessageBase
	{
public:
	IMPORT_C static CRrlpProtocolError* NewL(TBool aIsOutgoing);
		
protected:
	CRrlpProtocolError(TBool aIsOutgoing);

public:
	virtual ~CRrlpProtocolError();
	
public:

	/** Populates outgoing error code according to aErrorCode (for outgoing messages) */
	IMPORT_C TInt SetProtocolError(const TRrlpErrorCode& aErrorCode);

	/** Populates aErrorCode with the received error code (for received messages) */
	IMPORT_C TInt GetProtocolError(TRrlpErrorCode& aErrorCode);

protected:

	/** set/get extended reference fields */
	TInt SetExtendedReference(const TRrlpReference& aRrlpRef);
	TInt GetExtendedReference(TRrlpReference& aRrlpRef);

private:
	/** second stage constructor */
	void ConstructL();
	
	/** Prohibit copy constructor */
	CRrlpProtocolError(const CRrlpProtocolError&);

	/** Prohibit assigment operator */
	CRrlpProtocolError& operator= (const CRrlpProtocolError&);
	};

#endif // RRLP_POROTOCOL_ERROR_H

