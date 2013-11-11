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
*/

#include <elements/nm_address_internal.h>
#include <elements/nm_transport.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNodeMessAdrC, "ElemNodeMessAdrC");
#endif

using namespace Messages;

//
//TRuntimeCtxId

/******************************************************************************************************
*
*
* TRuntimeCtxId
*
*
*******************************************************************************************************/
namespace Mem4NullRuntimeCtxId
{
	static const TUint8 mem[sizeof(TRuntimeCtxId)] = { sizeof(TRuntimeCtxId) & ~TRuntimeCtxId::KIsNullMask8, 0};
}

EXPORT_C const TRuntimeCtxId& TRuntimeCtxId::NullId()
	{
	return *reinterpret_cast<const TRuntimeCtxId *>(Mem4NullRuntimeCtxId::mem);
	}

EXPORT_C TRuntimeCtxId::TRuntimeCtxId(const TRuntimeCtxId& aRuntimeCtxId)
	: iSize(sizeof(TRuntimeCtxId))
	{
	*this = aRuntimeCtxId;
	}

EXPORT_C TRuntimeCtxId::TRuntimeCtxId()
:	iSize(sizeof(TRuntimeCtxId))
	{
	Mem::FillZ((TUint8*)this + _FOFF(TRuntimeCtxId, iSalt), sizeof(TRuntimeCtxId) - _FOFF(TRuntimeCtxId, iSalt));
	}

//protected
EXPORT_C TRuntimeCtxId::TRuntimeCtxId(TUint8 aSize)
:	iSize(aSize)
	{
	Mem::FillZ((TUint8*)this + _FOFF(TRuntimeCtxId, iSalt), sizeof(TRuntimeCtxId) - _FOFF(TRuntimeCtxId, iSalt));
	}

EXPORT_C const TRuntimeCtxId& TRuntimeCtxId::operator=(const TRuntimeCtxId& aRHS)
	{
	if (Size())
		{
		//Valid address already, reassign only the matching part + isnull flag
		iSize = aRHS.IsNull()? iSize & ~KIsNullMask8 : iSize | KIsNullMask8;
		}
	else
		{
		iSize = aRHS.iSize; //This will also set/not set KIsNullMask8!
		}

	Mem::Copy((TUint8*)this + _FOFF(TRuntimeCtxId, iSalt), (const TUint8*)&aRHS + _FOFF(TRuntimeCtxId, iSalt), Min(Size(), aRHS.Size()) - _FOFF(TRuntimeCtxId, iSalt));

	return *this;
	}

EXPORT_C TBool TRuntimeCtxId::operator==(const TRuntimeCtxId& aRHS) const
	{
	return ComparePart(aRHS, Size());
	}

EXPORT_C TBool TRuntimeCtxId::ComparePart(const TRuntimeCtxId& aRHS, TUint8 aSize) const
	{
	__ASSERT_DEBUG(aSize <= Size(), User::Panic(KSpecAssert_ElemNodeMessAdrC, 1));

	//No need to compare bits if either or both operands are NULL.
	TInt comparesize = (IsNull() || aRHS.IsNull()) ? 0 : (Min(aSize, aRHS.Size()) - _FOFF(TRuntimeCtxId, iSalt));
	if (comparesize>0)
		{
		return Mem::Compare((const TUint8*)this + _FOFF(TRuntimeCtxId, iSalt), comparesize, (const TUint8*)&aRHS + _FOFF(TRuntimeCtxId, iSalt), comparesize) == 0;
		}

	//If both are NULL, then we can say they're equal, otherwise they're not.
	return IsNull() && aRHS.IsNull();
	}

EXPORT_C TInt TRuntimeCtxId::Store(TDes8& aDes) const
	{
	if (Size() > aDes.MaxSize())
		{
		return KErrOverflow;
		}
	aDes.Append((TUint8*)this, Size());
	return KErrNone;
	}

EXPORT_C void TRuntimeCtxId::PostTo(const TRuntimeCtxId& aPostFrom, const TSignalBase& aMessage) const
	{
	RClientInterface::OpenPostMessageClose(aPostFrom, *this, aMessage);
	}





/******************************************************************************************************
*
*
* TNodeId
*
*
*******************************************************************************************************/
namespace Mem4NullNodeId
{
	static const TUint8 mem[sizeof(TNodeId)] = { sizeof(TNodeId) & ~TRuntimeCtxId::KIsNullMask8, 0};
}

EXPORT_C const TNodeId& TNodeId::NullId()
	{
	return *reinterpret_cast<const TNodeId *>(Mem4NullNodeId::mem);
	}

EXPORT_C TNodeId::TNodeId()
:	TRuntimeCtxId(sizeof(TNodeId)),
	iPtr(NULL)
	{
	}

EXPORT_C TNodeId::TNodeId(const TNodeId& aNodeId)
:	TRuntimeCtxId(sizeof(TNodeId))
	{
	*this = aNodeId;
	}

//protected
EXPORT_C TNodeId::TNodeId(TUint8 aSize)
:	TRuntimeCtxId(aSize),
	iPtr(NULL)
	{
	}

EXPORT_C const TNodeId& TNodeId::operator=(const TRuntimeCtxId& aRHS)
	{
	return address_cast<TNodeId>(TRuntimeCtxId::operator=(aRHS));
	}

/******************************************************************************************************
* TNodeIdRemainder
*******************************************************************************************************/
EXPORT_C TNodeIdRemainder::TNodeIdRemainder(const TRuntimeCtxId& aFrom)
	{
	*this = aFrom;
	}

EXPORT_C TNodeIdRemainder::TNodeIdRemainder()
:iSize(0)
	{
	Mem::FillZ(iRemainder, sizeof(iRemainder));
	}

EXPORT_C const TNodeIdRemainder& TNodeIdRemainder::operator=(const TRuntimeCtxId& aRHS)
	{
	//Here we assign only the remainder bit off aRHS;
	iSize = aRHS.Size() - sizeof(TNodeId);
	if (iSize > 0)
		{
		//No need to assert no sizes, as no TRuntimeCtxId can exceed KMaxInlineAddressSize + sizeof(TUint)
		//and TNodeIdRemainder::EPayloadSize is calculated based on that. There will always be room.
		Mem::Copy(iRemainder, (const TUint8*)&aRHS + sizeof(TNodeId), iSize);
		}
	else
		{
		//Zero all including size
		Mem::FillZ(iRemainder, sizeof(iRemainder));
		iSize = 0;
		}
	return *this;
	}

EXPORT_C TBool TNodeIdRemainder::operator==(const TRuntimeCtxId& aRHS) const
	{
	TInt comparablePayloadSize = Min(aRHS.Size() - sizeof(TNodeId), iSize);
	return Mem::Compare(iRemainder, comparablePayloadSize, (const TUint8*)&aRHS + sizeof(TNodeId), comparablePayloadSize) == 0;
	}

EXPORT_C TBool TNodeIdRemainder::operator==(const TNodeIdRemainder& aRHS) const
	{
	TInt comparablePayloadSize = Min(aRHS.Size(), iSize);
	return Mem::Compare(iRemainder, comparablePayloadSize, (const TUint8*)&aRHS.iRemainder, comparablePayloadSize) == 0;
	}





/******************************************************************************************************
*
*
* TNodeCtxId
*
*
*******************************************************************************************************/
namespace Mem4NullNodeCtxId
{
	static const TUint8 mem[sizeof(TNodeCtxId)] = { sizeof(TNodeCtxId) & ~TRuntimeCtxId::KIsNullMask8, 0};
}

EXPORT_C const TNodeCtxId& TNodeCtxId::NullId()
	{
	return *reinterpret_cast<const TNodeCtxId *>(Mem4NullNodeCtxId::mem);
	}

EXPORT_C TNodeCtxId::TNodeCtxId()
:	TNodeId(sizeof(TNodeCtxId)),
	iNodeCtx(0),
	iReserved(0)
	{
	}

EXPORT_C TNodeCtxId::TNodeCtxId(TUint16 aNodeCtx, const TNodeId& aNodeId)
:	TNodeId(sizeof(TNodeCtxId)),
	iNodeCtx(aNodeCtx),
	iReserved(0)
	{
	__ASSERT_DEBUG(address_cast<TNodeCtxId>(&aNodeId)==NULL, User::Panic(KSpecAssert_ElemNodeMessAdrC, 2)); //Be careful! You are overriding your aNodeCtx! Use TNodeCtxId(const TNodeCtxId& aNodeCtxId).
	*this = aNodeId;
	}

EXPORT_C TNodeCtxId::TNodeCtxId(const TNodeCtxId& aNodeCtxId)
:	TNodeId(sizeof(TNodeCtxId))
	{
	*this = aNodeCtxId;
	}

//protected
EXPORT_C TNodeCtxId::TNodeCtxId(TUint8 aSize)
:	TNodeId(aSize),
	iNodeCtx(0)
	{
	}

EXPORT_C const TNodeCtxId& TNodeCtxId::operator=(const TRuntimeCtxId& aRHS)
	{
	return address_cast<TNodeCtxId>(TRuntimeCtxId::operator=(aRHS));
	}

/******************************************************************************************************
*
*
* TNodeOid
*
*
*******************************************************************************************************/

EXPORT_C TNodeOid::TNodeOid(TAny* aObj)
	{
	TNodeIdOp(*this).SetPtr(aObj);
	TlsGlobals::Get().TransportReceiver().RegisterAddress(*this);
	}

EXPORT_C TNodeOid::TNodeOid(ANode& aObj)
	{
	TNodeIdOp(*this).SetPtr(static_cast<TAny*>(&aObj));
	TlsGlobals::Get().TransportReceiver().RegisterAddress(*this);
	}

EXPORT_C TNodeOid::~TNodeOid()
	{
	// We allow TNodeOid objects to be unregistered early so we have to permit null TNodeOid
	// objects to exist.  We avoid the assertion that tests for null in UnregisterSelf by
	// checking to make sure that the TNodeOid is not null before calling it.
	// We do not remove the assertion because we still want to guard against repeated calls
	// to UnregisterSelf.
	if(!IsNull())
		{
		UnregisterSelf();
		}
	}

/** Usually a node remains registered with the transport mechanism for its lifetime, however some special
cases need to unregister early (eg to avoid registration at all).
*/
EXPORT_C void TNodeOid::UnregisterSelf()
	{
	__ASSERT_ALWAYS(!IsNull(), User::Panic(KMessagesPanic, EAddressNotValidPanic));
	TlsGlobals::Get().TransportReceiver().DeregisterAddress(*this);
	}





