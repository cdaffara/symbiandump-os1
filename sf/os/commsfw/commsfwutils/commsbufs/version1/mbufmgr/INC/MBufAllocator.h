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
 @publishedPartner
 @released
*/

#ifndef MBUFALLOCATOR_H_
#define MBUFALLOCATOR_H_

class CMBufManager;

class RMBufAllocator
/**
RMBufAllocator provides a handle that can be used to speed RMBufChain allocation.
Without this handle every allocation internally accesses thread local storage.
Creation of the RMBufAllocator handle uses thread local storage but subsequent
RMBufChain allocations using this handle don't.

RMBufAllocator also provides information about the state of the RMBuf free pools,
each size of RMBuf has a different free pool.

@publishedPartner
@released
*/
	{

public:
    IMPORT_C RMBufAllocator();

	IMPORT_C TInt BytesAvailable() const;
	IMPORT_C TInt BytesAvailable(TInt aSize) const;
	IMPORT_C TInt NextMBufSize(TInt aSize) const;
	IMPORT_C TInt LargestMBufSize() const;

/**
 * @internalTechnology
 */
	inline CMBufManager& MBufManager(){return iManager;}
private:
    CMBufManager& iManager;
	};

#endif /*MBUFALLOCATOR_H_*/
