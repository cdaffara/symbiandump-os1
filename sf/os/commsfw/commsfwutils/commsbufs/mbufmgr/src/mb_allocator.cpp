// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <comms-infras/mbufallocator.h>
#include <comms-infras/commsbufpondop.h>
#include "commsbufpondintf.h"

EXPORT_C RMBufAllocator::RMBufAllocator() 
: TCommsBufAllocator(TCommsBufPondTLSOp::Get())
	{}

EXPORT_C TInt RMBufAllocator::BytesAvailable() const
/**
Obtains the total available bytes available in the mbuf system in it's entirety.
@return the number of available bytes within all of the MBuf pools.
*/
    {
	return iPond.BytesAvailable ();
	}

EXPORT_C TInt RMBufAllocator::BytesAvailable(TInt aSize) const
/**
Obtains the total bytes available in the pool of the given sized MBuf.
@param aSize one of the sizes returned by RMBufAllocator::NextMBufSize().
@return the number of available bytes within the MBuf pool of MBufs given by the size aSize.
*/
    {
	return iPond.BytesAvailable (aSize);
	}

EXPORT_C TInt RMBufAllocator::NextMBufSize(TInt aSize) const
/**
Returns the first MBuf size that is greater than aSize, returns KErrNotFound if there isn't an MBuf
whose size is larger than aSize. Can be used to find the size of each of the MBuf pools by starting
at 0 and repeatedly passing in the result of the previous call until KErrNotFound occurs.
@param aSize a starting size to search upwards from.
@return the size of the first MBuf whose size is greater than aSize.
*/
    {
	return iPond.NextBufSize (aSize);
	}

EXPORT_C TInt RMBufAllocator::LargestMBufSize() const
/**
@return the size of the largest MBuf that is registered with the sytem.
*/
    {
	return iPond.LargestBufSize ();
	} 

