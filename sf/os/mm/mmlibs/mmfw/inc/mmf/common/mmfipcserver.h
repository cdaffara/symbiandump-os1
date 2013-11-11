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

#ifndef __MMF_COMMON_MMFIPC_SERVER_H__
#define __MMF_COMMON_MMFIPC_SERVER_H__

#include <e32base.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipc.h>
#endif

// The following declarations are used instead of the native ones on the particular system variant:
//    RMmfIpcMessage replaces RMessage or RMessage2
//    CMmfIpcServer replaces CServer or CServer2
//    CMmfIpcSession replaces CSharableSession or CSession2
// They are used internally to the MMF component to provide configurability for the particular subsystem

#ifdef __MMF_USE_IPC_V2__

//typedef RMessage2 RMmfIpcMessage;

class CMmfIpcServer; // forward declaration

/**
 * derive CMmfIpcSession from CSession2 so we can bridge the differences to old CSharableSession
 *
 * @internalTechnology
 */
 
class CMmfIpcSession : public CSession2
	{
protected:
	virtual void CreateL(const CMmfIpcServer& aServer);
	CMmfIpcSession() {}
	// from CSession2
	void CreateL();
	};

/*
 * derive CMmfIpcServer from CServer2 so we can bridge difference to old CServer
 *
 * @internalTechnology
 */
 
class CMmfIpcServer : public CServer2
	{
protected:
	virtual CMmfIpcSession* NewSessionL(const TVersion& aVersion) const=0;
	CMmfIpcServer(TInt aPriority,TServerType aType=EUnsharableSessions);
private:
	// derived from CServer2
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	};

#else // __MMF_USE_IPC_V2__

// otherwise always use V1
//typedef RMessage RMmfIpcMessage;
typedef CServer CMmfIpcServer;
typedef CSharableSession CMmfIpcSession;

#endif // __MMF_USE_IPC_V2__

#if defined(__VC32__)
#pragma warning( disable : 4097 )	// typedef-name used as synonym for class-name 
#endif


/**
 * Adapt RMessage calls between IPC variants
 *
 * @internalTechnology
 */

class MmfMessageUtil 
	{
public:
	static TInt Read(const RMmfIpcMessage& aMessage, TInt aParam, TDes8& aResult);
	static TInt Write(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC8& aValue);
	static void ReadL(const RMmfIpcMessage& aMessage, TInt aParam, TDes8& aResult);
	static void WriteL(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC8& aValue);
	};
	
#include <mmf/common/mmfipcserver.inl>

#endif // __MMF_COMMON_MMFIPC_SERVER_H__
