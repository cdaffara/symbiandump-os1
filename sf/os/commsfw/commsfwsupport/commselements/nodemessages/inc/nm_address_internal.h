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

/**
 @file
 @internalTechnology
*/

#ifndef SYMBIAN_NM_ADDRESS_INTERNAL_H
#define SYMBIAN_NM_ADDRESS_INTERNAL_H

#include <elements/nm_interfaces.h>	// which includes nm_address.h


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNodeMessAdrI, "ElemNodeMessAdrI");
#endif

namespace Messages
{
//
//TRuntimeCtxIdOp
class TRuntimeCtxIdOp
/*
@internalTechnology
*/
	{
public:
	TRuntimeCtxIdOp(TRuntimeCtxId& aRuntimeCtxId)
	:	iRuntimeCtxId(aRuntimeCtxId)
		{
		}

	//panics when already set
	inline void Set(TUint8 aSize)
		{
		__ASSERT_DEBUG(aSize > 0 && aSize <= TRuntimeCtxId::KMaxInlineAddressSize,
			User::Panic(Messages::KMessagesPanic,Messages::EAddressCastPanic)); //Clearing is not allowed by design
		__ASSERT_DEBUG(iRuntimeCtxId.iSize == 0, User::Panic(KSpecAssert_ElemNodeMessAdrI, 1));
		iRuntimeCtxId.iSize = aSize | TRuntimeCtxId::KIsNullMask8; //Set as not null at the same time
		}

	//panics when already set
	inline void Set(TUint8 aSalt, TUint16 aThread)
		{
		__ASSERT_DEBUG(iRuntimeCtxId.IsNull(), User::Panic(KSpecAssert_ElemNodeMessAdrI, 3));
		__ASSERT_DEBUG(iRuntimeCtxId.iThread == 0, User::Panic(KSpecAssert_ElemNodeMessAdrI, 4));
		__ASSERT_DEBUG(iRuntimeCtxId.iSalt == 0, User::Panic(KSpecAssert_ElemNodeMessAdrI, 5));
		iRuntimeCtxId.iSalt = aSalt;
		iRuntimeCtxId.iThread = aThread;
		iRuntimeCtxId.iSize |= TRuntimeCtxId::KIsNullMask8;
		}

protected:
	TRuntimeCtxId& iRuntimeCtxId;
	};

//
//TNodeIdOp
class TNodeIdOp
/*
@internalTechnology
*/
	{
public:
	TNodeIdOp(TNodeId& aNodeId)
	:	iNodeId(aNodeId)
		{
		}

	//panics when already set
	inline void SetPtr(TAny* aInterface)
		{
		__ASSERT_DEBUG(iNodeId.iPtr == NULL, User::Panic(KSpecAssert_ElemNodeMessAdrI, 6));
		iNodeId.iPtr = aInterface;
		}

protected:
	TNodeId& iNodeId;
	};

//
//TNodeCtxIdOp
class TNodeCtxIdOp
/*
@internalTechnology
*/
	{
public:
	TNodeCtxIdOp(TNodeCtxId& aNodeCtxId)
	:	iNodeCtxId(aNodeCtxId)
		{
		}

	inline void SetNodeCtx(TUint16 aNodeCtx)
		{
		iNodeCtxId.iNodeCtx = aNodeCtx;
		}

protected:
	TNodeCtxId& iNodeCtxId;
	};

//
//TNodeOid
class TNodeOid : protected TNodeId
    {
    friend class ANodeId;

public:
	IMPORT_C explicit TNodeOid(ANode& aObj);
	IMPORT_C ~TNodeOid();

private:
	IMPORT_C explicit TNodeOid(TAny* aObj);
	IMPORT_C void UnregisterSelf();
    };


} //namespace Messages

#endif
	//SYMBIAN_NM_ADDRESS_INTERNAL_H

