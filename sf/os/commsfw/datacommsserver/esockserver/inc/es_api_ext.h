// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CS_API_EXT.H
//
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef CS_API_EXT_H
#define CS_API_EXT_H

#include <es_sock.h>
#include <comms-infras/api_ext_list.h>
#include <comms-infras/trbuf.h>

class TCommsApiExtReqMsg;

_LIT(KApiExtPanic, "ESock_ApiExtItf");

/**
	API extension interface panic codes

	@internalTechnology
*/
enum TApiExtPanic
	{
	EApiExtBadHandle = 0,	//< The session/subsession handle of the extension interface is invalid
	EApiExtMetaOverflow,			//< Storing a message into a descriptor has overflowed
	};

const TUint KMaxExtApiIpcMsgLength = 256;

class MCommsApiExtProvider
/**
Comms API extension provider interface.

@internalTechnology
*/
	{
public:
	virtual TInt Handle() const = 0;
	virtual TInt Close(const TIpcArgs& aArgs) const = 0;	
	virtual TInt Send(const TIpcArgs& aArgs) const = 0;	
	virtual void SendReceive(const TIpcArgs& aArgs,TRequestStatus& aStatus) const = 0;
	virtual void Release() = 0;
	};

class CCommsSessionApiExtProvider : public MCommsApiExtProvider
	{
	friend class RCommsApiExtensionBase;
private:
	CCommsSessionApiExtProvider(RCommsSession& session);
	static CCommsSessionApiExtProvider *Open(RCommsSession& aExtensionProvider, TSupportedCommsApiExt aInterfaceId, TInt &error);
	TInt Close(const TIpcArgs& aArgs) const;	
	TInt Handle() const;
	TInt Send(const TIpcArgs& aArgs) const;	
	void SendReceive(const TIpcArgs& aArgs,TRequestStatus& aStatus) const;
	void Release();

private:
	const RCommsSession iSession;
	};
	
class CCommsSubSessionApiExtProvider : public MCommsApiExtProvider
	{
	friend class RCommsApiExtensionBase;
private:
	CCommsSubSessionApiExtProvider(RCommsSubSession& subSession);
	static CCommsSubSessionApiExtProvider *Open(RCommsSubSession& aExtensionProvider, TSupportedCommsApiExt aInterfaceId, TInt &error);
	TInt Close(const TIpcArgs& aArgs) const;	
	TInt Handle() const;
	TInt Send(const TIpcArgs& aArgs) const;	
	void SendReceive(const TIpcArgs& aArgs,TRequestStatus& aStatus) const;
	void Release();

private:
	const RCommsSubSession iSubSession;
	};
	
	
class RCommsApiExtensionBase
/**
Base class for Comms API extensions.

@internalTechnology
@released
*/
	{
protected:
	IMPORT_C explicit RCommsApiExtensionBase();
	IMPORT_C TInt Open(RCommsSession& aExtensionProvider, TSupportedCommsApiExt aInterfaceId);
	IMPORT_C TInt Open(RCommsSubSession& aExtensionProvider, TSupportedCommsApiExt aInterfaceId);
	IMPORT_C void Close();
	IMPORT_C void SendRequest(TCommsApiExtReqMsg& aRequestMsg, TDes8& aResponseBuf, TRequestStatus& aStatus);
	IMPORT_C void SendMessage(TCommsApiExtReqMsg& aRequestMsg);

protected:
	MCommsApiExtProvider *iProvider;
	TSupportedCommsApiExt iInterfaceId;

private:
	RPointerArray<Elements::TRBuf8> iBuffers;
	};

template<TSupportedCommsApiExt T>
class RCommsApiExtension : public RCommsApiExtensionBase
/**
Comms API extensions template. New extension APIs may derive from this template.

@internalTechnology
@released
*/
	{
public:
	inline TInt Open(RCommsSession& aExtensionProvider);
	inline TInt Open(RCommsSubSession& aExtensionProvider);
	};

template<TSupportedCommsApiExt T>
TInt RCommsApiExtension<T>::Open(RCommsSession& aExtensionProvider)
	{
	return RCommsApiExtensionBase::Open(aExtensionProvider,T);
	}

template<TSupportedCommsApiExt T>
TInt RCommsApiExtension<T>::Open(RCommsSubSession& aExtensionProvider)
	{
	return RCommsApiExtensionBase::Open(aExtensionProvider,T);
	}

#endif // CS_API_EXT_H

