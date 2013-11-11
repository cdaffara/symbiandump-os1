// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include "sd_log.h"
#include "sd_roles.h"
#include "sd_std.h"

using namespace Den;

/** Because we strive to maintain a single owner for a message and to pass by reference when ownership
it's generally an error for the handle to be set upon destruction.
*/

EXPORT_C RSafeMessage::~RSafeMessage()
	{
#ifdef _DEBUG
	if(Handle() != KNullHandle)
		{
		COMMONLOG((CommsFW::KInvalidWorkerId, KECommonErrorTag, _L8("RSafeMessage(%08x) - WARNING! handle still set in destructor, may be a leak, session %08x"), Handle(), Session()) );
		}
#endif
	}


/** Complete the message with the given reason code, with detailed logging
*/
EXPORT_C void RSafeMessage::Complete(TInt aReason) const
	{
	//TODO	LOG(ESockLog::Printf(KESockServerTag, _L8("RSafeMessage(%08x)::Complete(%d) - session %08x"), Handle(), aReason, Session()) );
	COMMONLOG((CommsFW::KInvalidWorkerId, KECommonSessDetailTag, _L8("RSafeMessage(%08x)::Complete(%08x) with %d"), this, Handle(), aReason) );
	RMessage2::Complete(aReason);
	}

/** Take over a message. Beyond being a simple copy this resets the source's handle so that double-ownership is avoided
*/
EXPORT_C void RSafeMessage::Adopt(RSafeMessage& aSrc)
	{
	*this = aSrc;
	aSrc.ResetHandle();
	}

/** Duplicate a message, including aliasing the handle & so raising the risk of a double-complete
*/
EXPORT_C void RSafeMessage::Duplicate(const RSafeMessage& aSrc)
	{
	*this = aSrc;
	}

/** Null the handle, so this safe message no longer references a client request. This is normally done automatically
by completing or panicking the client or transferring message ownership, so this will seldom need to be called
explicitly
*/
EXPORT_C void RSafeMessage::ResetHandle()
	{
	iHandle = KNullHandle;
	}

RSafeMessage& RSafeMessage::operator=(const RSafeMessage& aSrc)
	{
	RMessage2::operator=(aSrc);
	return *this;
	}

EXPORT_C void RSafeMessage::PanicClient(const TDesC& aCategory, TInt aReason) const
	{
	RThread t;
	RMessage2::Client(t);
	TUint tid = t.Id();
	TExitType exittype=t.ExitType();
	t.Close();
	if(exittype == EExitPending) // is client alive?
		{
#ifdef SYMBIAN_TRACE_ENABLE
		TBuf8<64> messBuf;
		Logging::IPCMessName((TInt) RMessage2::Function(), messBuf);
		COMMONLOG((CommsFW::KInvalidWorkerId, KECommonErrorTag, _L8("RSafeMessage:\tPanicClient, Reason %d, Tid %d, RMessage2 %08x [%S], Args %08x %08x %08x %08x, ClientStatus %08x"),
				aReason, tid, RMessage2::Handle(), &messBuf, RMessage2::Int0(), RMessage2::Int1(), RMessage2::Int2(), RMessage2::Int3(), RMessage2::ClientStatus()));
#endif
		RMessage2::Panic(aCategory, aReason);
		}
	}

/**
Perform a read and kill the client if it has supplied a bad descriptor
*/
EXPORT_C void RSafeMessage::ReadL(TInt aSrcParamIndex, TDes8& aDes, TInt anOffset) const
	{
	User::LeaveIfError(Read(aSrcParamIndex, aDes, anOffset));
	}

EXPORT_C TInt RSafeMessage::Read(TInt aSrcParamIndex, TDes8& aDes, TInt anOffset) const
/**
 Perform a read and kill the client if it has supplied a bad descriptor
*/
	{
	TInt res= RMessage2::Read(aSrcParamIndex,aDes,anOffset);
	__ASSERT_DEBUG(res!=KErrArgument, User::Panic(KDenFaultPanic, ECommonBadState));
	if (res!=KErrNone)
		{
		PanicClient(KWorkerClientPanic, ECommonBadDescriptorRead);
		}
	return res;
	}

/**
Perform a write and kill the client if it has supplied a bad descriptor
*/
EXPORT_C void RSafeMessage::WriteL(TInt aDstParamIndex, const TDesC8& aDes, TInt anOffset) const
	{
	User::LeaveIfError(Write(aDstParamIndex, aDes, anOffset));
	}

/**
Perform a write and kill the client if it has supplied a bad descriptor
*/
EXPORT_C TInt RSafeMessage::Write(TInt aDstParamIndex, const TDesC8& aDes, TInt anOffset) const
	{
	TInt res = RMessage2::Write(aDstParamIndex,aDes,anOffset);
	__ASSERT_DEBUG(res!=KErrArgument,User::Panic(KDenFaultPanic, ECommonBadState));
	if (res == KErrBadDescriptor)
		{
		PanicClient(KWorkerClientPanic,ECommonBadDescriptorWrite);
		}
	return res;
	}

/**
Perform a write and kill the client if it has supplied a bad descriptor
*/
EXPORT_C void RSafeMessage::WriteL(TInt aDstParamIndex, const TPckg<TUint32>& aDes, TInt anOffset) const
	{
	User::LeaveIfError(Write(aDstParamIndex, aDes, anOffset));
	}

/**
Perform a write and kill the client if it has supplied a bad descriptor
*/
EXPORT_C TInt RSafeMessage::Write(TInt aDstParamIndex, const TPckg<TUint32>& aDes, TInt anOffset) const
	{
	TInt res = RMessage2::Write(aDstParamIndex,aDes,anOffset);
	__ASSERT_DEBUG(res!=KErrArgument,User::Panic(KDenFaultPanic, ECommonBadState));
	if (res == KErrBadDescriptor)
		{
		PanicClient(KWorkerClientPanic,ECommonBadDescriptorWrite);
		}
	return res;
	}

/**
Perform a read and kill the client if it has supplied a bad descriptor
*/
EXPORT_C void RSafeMessage::ReadL(TInt aSrcParamIndex,TDes16& aDes,TInt anOffset) const
	{
	User::LeaveIfError(Read(aSrcParamIndex, aDes, anOffset));
	}

/**
Perform a read and kill the client if it has supplied a bad descriptor
*/
EXPORT_C TInt RSafeMessage::Read(TInt aSrcParamIndex, TDes16& aDes, TInt anOffset) const
	{
#ifdef _UNICODE
	TInt res= RMessage2::Read(aSrcParamIndex,aDes,anOffset);
	__ASSERT_DEBUG(res!=KErrArgument, User::Panic(KDenFaultPanic, ECommonBadState));
	if (res!=KErrNone)
		{
		PanicClient(KWorkerClientPanic, ECommonBadDescriptorRead);
		}
	return res;
#else
	return KErrNotSupported;
#endif
	}

/**
Perform a write and kill the client if it has supplied a bad descriptor
*/
EXPORT_C void RSafeMessage::WriteL(TInt aDstParamIndex,const TDesC16& aDes,TInt anOffset) const
	{
	User::LeaveIfError(Write(aDstParamIndex, aDes, anOffset));
	}

/**
 Perform a write and kill the client if it has supplied a bad descriptor
*/
EXPORT_C TInt RSafeMessage::Write(TInt aDstParamIndex,const TDesC16& aDes,TInt anOffset) const
	{
#ifdef _UNICODE
	TInt res = RMessage2::Write(aDstParamIndex,aDes,anOffset);
	__ASSERT_DEBUG(res!=KErrArgument,User::Panic(KDenFaultPanic, ECommonBadState));
	if (res == KErrBadDescriptor)
		{
		PanicClient(KWorkerClientPanic,ECommonBadDescriptorWrite);
		}
	return res;
#else
	return KErrNotSupported;
#endif
	}

/**
Perform a GetDesLength and kill the client if it has supplied a bad descriptor
*/
EXPORT_C TInt RSafeMessage::GetDesLengthL(TInt aParam) const
	{
	TInt res = RMessage2::GetDesLength(aParam);
	__ASSERT_DEBUG(res!=KErrArgument, User::Panic(KDenFaultPanic, ECommonBadState));
	if (res<0)
		{
		PanicClient(KWorkerClientPanic, ECommonBadDescriptorLength);
		User::Leave(res);
		}
	return res;
	}


EXPORT_C TInt RSafeMessage::GetDesMaxLengthL(TInt aParam) const
	{
	TInt res = RMessage2::GetDesMaxLength(aParam);
	__ASSERT_DEBUG(res!=KErrArgument, User::Panic(KDenFaultPanic, ECommonBadState));
	if (res < 0)
		{
		PanicClient(KWorkerClientPanic, ECommonBadDescriptorLength);
		User::Leave(res);
		}
	return res;		
	}

