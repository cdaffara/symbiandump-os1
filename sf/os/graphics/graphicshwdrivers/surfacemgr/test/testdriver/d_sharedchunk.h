// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __D_SHAREDCHUNK_H__
#define __D_SHAREDCHUNK_H__

#include <e32cmn.h>
#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif

_LIT(KSharedChunkLddName,"D_SHAREDCHUNK");

enum TCreateFlags
	{
	ESingle				= 0x00,
	EMultiple			= 0x01,
	EOwnsMemory			= 0x02,
	EBadType			= 0x80,
	ECreateFlagsMask	= 0xff
	};

enum TCacheAttribute
	{
	/** CPU cached */
	ECached = 0,
	/** Non CPU cached */
	ENotCached = 1
	};

enum TCommitType
	{
	ENonContiguous			= 0x00,
	EContiguous				= 0x01,
	ECommitTypeMask			= 0x0f
	};

class RSharedChunkLdd : public RBusLogicalChannel
	{
public:
	enum TControl
		{
		ECreateChunk,
		EGetChunkHandle,
		ECommitMemory,
		ECloseChunk,
		ECacheAttribute,
		EContiguousAttribute,
		ECloseChunkHandle
		};


#ifndef __KERNEL_MODE__
public:
	inline TInt Open()
		{ return DoCreate(KSharedChunkLddName,TVersion(),KNullUnit,NULL,NULL,EOwnerProcess,ETrue); }
	inline TInt CreateChunk(TInt aSize, TAny** aKernelAddress=0)
		{ return DoControl(ECreateChunk,(TAny*)aSize,aKernelAddress); }
	inline TInt GetChunkHandle(RChunk& aChunk)
		{ return aChunk.SetReturnedHandle(DoControl(EGetChunkHandle)); }
	inline TInt CommitMemory(TInt aOffset, TInt aSize)
		{ return DoControl(ECommitMemory,(TAny*)aOffset,(TAny*)aSize); }
	inline TInt CloseChunk()
		{ return DoControl(ECloseChunk); }
	inline TInt GetCacheAttribute()
		{return DoControl(ECacheAttribute);}
	inline TBool GetContiguousAttribute()
		{return DoControl(EContiguousAttribute);}
	inline TInt CloseChunkHandle(RChunk aChunk)
		{ return DoControl(ECloseChunkHandle,(TAny*)aChunk.Handle()); }
#endif
	};


#endif
