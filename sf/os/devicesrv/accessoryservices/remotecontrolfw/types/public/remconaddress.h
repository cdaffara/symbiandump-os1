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
 @publishedAll
 @released
*/

#ifndef REMCONADDRESS_H
#define REMCONADDRESS_H

#include <e32base.h>

/**
Represents a single remote device address in terms of the bearer used to 
connect to the device and some bearer-specific connection information.
*/
class TRemConAddress
	{
public:
	/** This is maximum supported size for a bearer-specific remote device 
	address (cf. TSockAddr). */
	static const TUint KMaxAddrSize = 0x1c;

	/** Link between elements of this type in a TSglQue. */
	TSglQueLink iLink;

public:
	/** Constructor. */
	IMPORT_C TRemConAddress();

	/** Destructor. */
	IMPORT_C ~TRemConAddress();

public:
	/**
	The address is null if the bearer UID is the null UID.
	@return ETrue if the address is invalid/null, EFalse otherwise.
	*/
	IMPORT_C TBool IsNull() const;

	/**
	@return The bearer UID. Set this to null to make the address
	null.
	*/
	IMPORT_C TUid& BearerUid();

	/**
	@return The bearer UID.
	*/
	IMPORT_C TUid BearerUid() const;

	/**
	@return The bearer-specific address information.
	*/
	IMPORT_C TBuf8<KMaxAddrSize>& Addr();

	/**
	@return The bearer-specific address information.
	*/
	IMPORT_C const TBuf8<KMaxAddrSize>& Addr() const;

	/**
	Equality operator.
	@param An address to compare with.
	@return ETrue if both bearer UIDs and sets of bearer-specific information 
	match, EFalse otherwise.
	*/
	IMPORT_C TBool operator==(const TRemConAddress& aRhs) const;

private:
	/** Implementation UID of bearer (ECOM) plugin. */
	TUid iBearerUid;

	/** Buffer for bearer-specific connection address information. */
	TBuf8<KMaxAddrSize> iAddr;

	/** 
	Pad for BC-friendly future change.
	*/
	TUint32 iPad;
	};

#endif // REMCONADDRESS_H
