// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef BEARERSECURITY_H
#define BEARERSECURITY_H

#include <e32base.h>

/**
Wraps a bearer UID and the bearer's security policy.
*/
class TBearerSecurity
	{
public:
	/** Link between elements of this type in a TSglQue. If this member 
	changes offset, remconserver must be rebuilt. */
	TSglQueLink iLink;

public:
	/** Constructor. */
	IMPORT_C TBearerSecurity(TUid aUid, const TSecurityPolicy& aPolicy);

	/** Destructor. */
	IMPORT_C ~TBearerSecurity();

public:
	/** 
	Accessor for bearer UID.
	@return The bearer UID.
	*/
	IMPORT_C TUid BearerUid() const;

	/**
	Accessor for the bearer's security policy.
	@return The security policy.
	*/
	IMPORT_C const TSecurityPolicy& SecurityPolicy() const;

private:
	/** The bearer UID. */
	const TUid iBearerUid;

	/** The bearer's security policy. */
	const TSecurityPolicy iSecurityPolicy;

	/** 
	Pad for BC-friendly future change.
	*/
	TUint32 iPad;
	};

#endif // BEARERSECURITY_H
