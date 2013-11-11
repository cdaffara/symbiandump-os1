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


inline CCommsBufPool::CCommsBufPool(MCommsBufPondIntf& aPondIntf, TInt aSize)
: iPondIntf(aPondIntf),
iBufSize(aSize)
	{
	}

inline MCommsBufPondIntf& CCommsBufPool::Pond() const
	{
	return iPondIntf;
	}

inline TInt CCommsBufPool::BufSize() const
	{
	return iBufSize;
	}

inline TInt CCommsBufPool::Id() const
	{
	return reinterpret_cast<TInt>(this);
	}

