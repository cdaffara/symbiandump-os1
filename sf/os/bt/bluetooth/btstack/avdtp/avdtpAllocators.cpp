// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the avdtp allocators
// 
//

/**
 @file
 @internalComponent
*/

#include "avdtpAllocators.h"

TInt TBitFieldAllocator::Get(TInt& aValue, TInt aMax, TInt aMin/*=0*/)
	{
	if (aMax<aMin)
		{
		return KErrArgument;
		}
	if ((aMax-aMin)>((8*sizeof(iBitField))-1))
		{
		// this class supports 64 values/bits at present
		return KErrTooBig;
		}
	// bit 1 represents the use of val=min,bit 2 use of val=min+1, etc
	for (TInt bit=0;bit<=(aMax-aMin);bit++)
		{
		if (!(iBitField & (1<<bit)))
			{
			iMin = aMin;
			// it's free, mark it
			iBitField |= (1<<bit);
			aValue = bit+iMin;
			return KErrNone;
			}
		}	
	return KErrInUse;
	}
	

void TBitFieldAllocator::Free(TInt aValue)
	{
	TInt bitPos = aValue-iMin;
#ifdef _DEBUG
	if (!(iBitField & (1<<bitPos)))
		{
		// freeing an unclaimed bit - maybe due to a leave, but allow checking
		__DEBUGGER();
		}
#endif
	//Clear the bit in iSpace corresponding to aBitPos
	iBitField &= (~(1<<bitPos));	
	}


