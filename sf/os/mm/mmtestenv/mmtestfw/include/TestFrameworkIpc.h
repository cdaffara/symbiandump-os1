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
// Extensions to mmfipc. for testframework - support Des16's as well as Des8's
// 
//

#ifndef __TESTFRAMEWORKIPC_H__
#define __TESTFRAMEWORKIPC_H__

#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
/**
 * Add additional SendReceive() call to RMmfSessionBase.
 * This class adds a TDesC16 call to the standard set.
 *
 * @internalTechnology
 */
 
class RMmfSessionBaseX : public RMmfSessionBase
	{
protected:
	// following replaces "using RMmfSessionBase::SendReceive;" since does not work on gcc
	TInt SendReceive(TInt aFunction) const;
	void SendReceive(TInt aFunction, TRequestStatus& aStatus) const;
	TInt SendReceive(TInt aFunction, TInt aParam0) const;
	TInt SendReceive(TInt aFunction, TInt aParam0, TInt aParam1) const;
	void SendReceive(TInt aFunction, TInt aParam0, TRequestStatus& aStatus) const;
	void SendReceive(TInt aFunction, TInt aParam0, TInt aParam1, TRequestStatus& aStatus) const;
	TInt SendReceive(TInt aFunction, const TDesC8& aParam0) const;
	TInt SendReceive(TInt aFunction, const TDesC8& aParam0, const TDesC8& aParam1) const;
	TInt SendReceive(TInt aFunction, const TDesC8& aParam0, const TDesC8& aParam1, const TDesC8& aParam2) const;
	void SendReceive(TInt aFunction, const TDesC8& aParam0, TRequestStatus& aStatus) const;
	void SendReceive(TInt aFunction, const TDesC8& aParam0, const TDesC8& aParam1, TRequestStatus& aStatus) const;
	void SendReceive(TInt aFunction, const TDesC8& aParam0, const TDesC8& aParam1, const TDesC8& aParam2, TRequestStatus& aStatus) const;

	// additional call
	TInt SendReceive(TInt aFunction, const TDesC& aParam0, TInt aParam1) const;
	};

/**
 * Add additional calls to MmfMessageUtil.
 * This class adds TDes(C)16 calls to the basic set.
 * 
 * @internalTechnology
 */

class MmfMessageUtilX : public MmfMessageUtil
	{
public:
	// import existing API
	// the following replaces "using MmfMessageUtil::..." since that does not on gcc
	static TInt Read(const RMmfIpcMessage& aMessage, TInt aParam, TDes8& aResult);
	static TInt Write(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC8& aValue);
	static void ReadL(const RMmfIpcMessage& aMessage, TInt aParam, TDes8& aResult);
	static void WriteL(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC8& aValue);

	// new 16-bit descriptor variants
	static TInt Read(const RMmfIpcMessage& aMessage, TInt aParam, TDes16& aResult);
	static TInt Write(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC16& aValue);
	static void ReadL(const RMmfIpcMessage& aMessage, TInt aParam, TDes16& aResult);
	static void WriteL(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC16& aValue);
	};

#include <testframeworkipc.inl>

#endif // __TESTFRAMEWORKIPC_H__
