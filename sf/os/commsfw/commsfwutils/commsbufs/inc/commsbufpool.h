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
// Description: Comms buffer pool
//

#ifndef __COMMSBUFPOOL_H__
#define __COMMSBUFPOOL_H__

#include <e32base.h>     // for std types
class MCommsBufPondIntf;
class RCommsBuf;
/**
Base class for the comms buffer pool. 

@internalTechnology
 */
class CCommsBufPool : public CBase
	{
	public:
	inline CCommsBufPool(MCommsBufPondIntf& aPondIntf, TInt aSize);
	inline MCommsBufPondIntf& Pond() const;
	inline TInt BufSize() const;
	inline TInt Id() const;
	protected:
	TInt				iBufSize;
	MCommsBufPondIntf& iPondIntf;
	};

#include "commsbufpool.inl"
#endif // __COMMSBUFPOOL_H__
