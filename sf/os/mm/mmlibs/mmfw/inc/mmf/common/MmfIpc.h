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
// include\mmf\common\mmfipc.h
// Thin interface layer to use either V1 or V2 IPC calls as appropriate
// 
//

#ifndef __MMF_COMMON_MMFIPC_H__
#define __MMF_COMMON_MMFIPC_H__

#include <e32base.h>

#ifdef __IPC_V2_PRESENT__
// if V2 code present than use it
#define __MMF_USE_IPC_V2__
#endif //__IPC_V2_PRESENT__


#ifdef __MMF_USE_IPC_V2__
typedef RMessage2 RMmfIpcMessage;
#else // __MMF_USE_IPC_V2__
// otherwise always use V1
typedef RMessage RMmfIpcMessage;
#endif // __MMF_USE_IPC_V2__

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif


// The following declarations are used instead of the native ones on the particular system variant:
//    RMmfIpcMessage replaces RMessage or RMessage2
//    CMmfIpcServer replaces CServer or CServer2
//    CMmfIpcSession replaces CSharableSession or CSession2
// They are used internally to the MMF component to provide configurability for the particular subsystem


#if defined(__VC32__)
#pragma warning( disable : 4097 )	// typedef-name used as synonym for class-name 
#endif


/**
 * RSessionBase adaptor layer 
 * Provides typechecking for client-side calls. 
 *
 * @publishedAll
 */

class RMmfSessionBase : public RSessionBase
	{
protected:
	// no parameter variants
	TInt SendReceive(TInt aFunction) const;
	void SendReceive(TInt aFunction, TRequestStatus& aStatus) const;
	// send number of parameters - no "result" parameter, just error/status 
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
	// "last" parameter is result, so is passed as TDes8&
	TInt SendReceiveResult(TInt aFunction, TDes8& aResult) const;
	TInt SendReceiveResult(TInt aFunction, const TDesC8& aParam0, const TDesC8& aParam1, TDes8& aResult) const;
	TInt SendReceiveResult(TInt aFunction, const TDesC8& aParam0, const TDesC8& aParam1, const TDesC8& aParam2, TDes8& aResult) const;
	void SendReceiveResult(TInt aFunction, TDes8& aResult, TRequestStatus& aStatus) const;
	void SendReceiveResult(TInt aFunction, const TDesC8& aParam0, const TDesC8& aParam1, TDes8& aResult, TRequestStatus& aStatus) const;
	void SendReceiveResult(TInt aFunction, const TDesC8& aParam0, const TDesC8& aParam1, const TDesC8& aParam2, TDes8& aResult, TRequestStatus& aStatus) const;

	//TDesC versions
	void SendReceive(TInt aFunction, const TDesC& aParam0, TRequestStatus& aStatus) const;
	TInt SendReceive(TInt aFunction, const TDesC& aParam0) const;
	TInt SendReceiveResult(TInt aFunction, const TDesC8& aParam0, const TDesC8& aParam1, TDes& aResult) const;

	// custom command versions for non-UID commands
	TInt SendReceive(TInt aFunction, TInt aParam0, const TDesC8& aParam1, const TDesC8& aParam2) const;
	TInt SendReceiveResult(TInt aFunction, TInt aParam0, const TDesC8& aParam1, const TDesC8& aParam2, TDes8& aResult) const;
	void SendReceive(TInt aFunction, TInt aParam0, const TDesC8& aParam1, const TDesC8& aParam2, TRequestStatus& aStatus) const;
	void SendReceiveResult(TInt aFunction, TInt aParam0, const TDesC8& aParam1, const TDesC8& aParam2, TDes8& aResult, TRequestStatus& aStatus) const;
	};
	
#include <mmf/common/mmfipc.inl>

#endif // __MMF_COMMON_MMFIPC_H__
