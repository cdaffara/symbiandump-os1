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
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <bttypes.h>
#include <es_mbman.h>
#include "TBnepBTDevAddr.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

TInt TBnepBTDevAddr::SetAddress(RMBufChain& aChain, TInt aOffset)
	{
	TInt rerr = KErrNone;

	if(aChain.Length() < aOffset + KBTDevAddrSize)
		{
		rerr = KErrUnderflow;
		}
	else
		{
		RMBuf* buf = aChain.First();
		if(buf->Length() < aOffset + KBTDevAddrSize)
			{
			aChain.Align(aOffset + KBTDevAddrSize);
			}

	    iAddress = 0;
	    for(TInt i=0;i<KBTDevAddrSize;i++)
	        {
	        iAddress <<= 8;
	        iAddress += static_cast<TInt>(buf->Get(aOffset + i));
	        LOG3(_L8("Index %d, iAddress 0x%016Lx, Byte %02x"),i, iAddress, buf->Get(aOffset + i));
	        }
		}
	return rerr;
	}

TBnepBTDevAddr::TBnepBTDevAddr(const TBTDevAddr& aAddr)
	{
    iAddress = 0;
    for(TInt i=0;i<KBTDevAddrSize;i++)
        {
        iAddress <<= 8;
        iAddress += static_cast<TInt>(aAddr[i]);
        LOG3(_L8("Index %d, iAddress 0x%016Lx, Byte %02x"),i, iAddress, aAddr[i]);
        }
	}
