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
 
inline CSystemSharedBufPool::CSystemSharedBufPool(MCommsBufPondIntf& aPondIntf, TInt aSize)
: CCommsBufPool(aPondIntf, aSize)
	{				
	}

inline RShPool CSystemSharedBufPool::Pool() const
	{
	return iPool;
	}

#ifdef  SYMBIAN_ZEROCOPY_BUF_FREELIST
inline CSystemSharedBufPool::CSystemSharedBufPool(MCommsBufPondIntf& aPondIntf, TInt aSize, TInt aFreeListCount)
: CCommsBufPool(aPondIntf, aSize),
iMaxFreeListCount(aFreeListCount)
    {
    
    }
#endif // SYMBIAN_ZEROCOPY_BUF_FREELIST
