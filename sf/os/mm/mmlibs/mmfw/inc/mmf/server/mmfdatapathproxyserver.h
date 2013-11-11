// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFDATAPATHPROXY_SERVER_H__
#define __MMFDATAPATHPROXY_SERVER_H__

#include <e32base.h>
#include <e32std.h>
#include <ecom/ecom.h>
#include <mmf/server/mmfdatapath.h>
#include <mmf/server/mmfsubthreadbase.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/server/mmfsubthreadbaseimpl.h>
#endif


/**
@internalComponent
 
The server running in the datapath subthread.
The subthread is owned by the main thread and shares the same heap.
*/
class CMMFDataPathProxyServer : public CMMFSubThreadServer
	{
public:
	static CMMFDataPathProxyServer* NewL();
	~CMMFDataPathProxyServer();
	static TInt StartThread(TAny* aAny);
	static void DoStartThreadL();
private:
	CMmfIpcSession* NewSessionL(const TVersion& aVersion) const;
	CMMFDataPathProxyServer();
	void ConstructL();
	};


/**
@internalComponent

The session used to transmit messages between the main thread and the subthread.
Only one session can be created with a datapath subthread.
Once this session is closed, the subthread will be killed.
*/
class CMMFDataPathProxySession : public CMMFSubThreadSession
	{
public:
	static CMMFDataPathProxySession* NewL();
	~CMMFDataPathProxySession();
	void ServiceL(const RMmfIpcMessage& aMessage);
private:
	CMMFDataPathProxySession();
	TBool LoadDataPathByL(const RMmfIpcMessage& aMessage);
	TBool LoadDataPathByMediaIdL(const RMmfIpcMessage& aMessage);
	TBool LoadDataPathByCodecUidL(const RMmfIpcMessage& aMessage);
	TBool LoadDataPathByMediaIdCodecUidL(const RMmfIpcMessage& aMessage);
	TBool AddDataSourceL(const RMmfIpcMessage& aMessage);
	TBool AddDataSinkL(const RMmfIpcMessage& aMessage);
	TBool PrimeL(const RMmfIpcMessage& aMessage);
	TBool PlayL(const RMmfIpcMessage& aMessage);
	TBool PauseL(const RMmfIpcMessage& aMessage);
	TBool StopL(const RMmfIpcMessage& aMessage);
	TBool GetPositionL(const RMmfIpcMessage& aMessage) const;
	TBool SetPositionL(const RMmfIpcMessage& aMessage);
	TBool SetPlayWindowL(const RMmfIpcMessage& aMessage) ;
	TBool ClearPlayWindowL(const RMmfIpcMessage& aMessage) ;
	TBool StateL(const RMmfIpcMessage& aMessage) ;

	void CheckDataPathExistsL() const {if (!iDataPath) User::Leave(KErrNotReady);};
private:
	CMMFDataPath* iDataPath;
	};



#endif

