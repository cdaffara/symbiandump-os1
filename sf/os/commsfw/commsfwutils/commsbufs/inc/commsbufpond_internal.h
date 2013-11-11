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

#ifndef __COMMSBUFPOND_INTERNAL_H__
#define __COMMSBUFPOND_INTERNAL_H__

class TShPool;

/**
@internalTechnology
*/
class TPoolRecord
	{
public:
	TPoolRecord() : iShPool(NULL), iCommsShPoolHandle(0), iCommsOpaqueHandle(0), iCommsBufSize(0) {}
#ifdef __KERNEL_MODE__
	TPoolRecord(TShPool* aShPool, TInt aCommsShPoolHandle, TInt aCommsOpaqueHandle, TInt aCommsBufSize) :
		iShPool(aShPool),
		iCommsShPoolHandle(aCommsShPoolHandle),
		iCommsOpaqueHandle(aCommsOpaqueHandle),
		iCommsBufSize(aCommsBufSize)
		{
		}
#else
	TPoolRecord(TInt aCommsShPoolHandle, TInt aCommsOpaqueHandle, TInt aCommsBufSize) :
		iShPool(NULL),
		iCommsShPoolHandle(aCommsShPoolHandle),
		iCommsOpaqueHandle(aCommsOpaqueHandle),
		iCommsBufSize(aCommsBufSize)
		{
		}
#endif

#ifdef __KERNEL_MODE__
	TShPool* iShPool;
#else
	TAny* iShPool;
#endif
	TInt iCommsShPoolHandle;
	TInt iCommsOpaqueHandle; // Meaningful user (comms) side only. Maintained only - kernel side
	TInt iCommsBufSize;
	};


/**
@internalTechnology
*/
class TCommsPond
	{
	friend class CSystemSharedBufPond;
	friend class DCommsPond;

public:
	TCommsPond() : iNumPools(0)
		{}

	static const TInt KMaxPoolsPerPond = 8;

	TInt NumPools() const
		{
		return iNumPools;
		}

private:
	TInt iNumPools;
	TPoolRecord iPoolRecords[KMaxPoolsPerPond];
	};

#endif __COMMSBUFPOND_INTERNAL_H__
