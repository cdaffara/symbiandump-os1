// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SYMBIAN_DEN_APIEXTENSION_CLIENT_H
#define SYMBIAN_DEN_APIEXTENSION_CLIENT_H

#include <elements/metavirtctor.h>
#include <elements/sd_std.h>
#include <elements/h_trbuf.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenApiXCH, "ElemSvrDenApiXCH");
#endif

namespace NetInterfaces
{
class TInterfaceControl;
}

namespace Elements
{
class RResponseMsg;
}

namespace Den
{

class TApiExtIdentification : public TBuf8<0x10> //flags, interface id, scope id plus reserved for future use
/**
@internalTechnology
*/
	{
public:
	//We only use this one ctr by design.
	//When used with no params the structure is not initialised and InterfaceId() reads 0.
	inline explicit TApiExtIdentification(TInt aInterfaceId = 0, TInt aScopeId = 0)
		{
		Append(reinterpret_cast<const TUint8*>(&aInterfaceId), sizeof(aInterfaceId));
		Append(reinterpret_cast<const TUint8*>(&aScopeId), sizeof(aScopeId));
		}

	inline TInt InterfaceId() const
		{
		__ASSERT_DEBUG(Length()>=2*sizeof(TInt), User::Panic(KSpecAssert_ElemSvrDenApiXCH, 1));
		return *reinterpret_cast<const TInt*>(Ptr());
		}

	inline TInt ScopeId() const
		{
		__ASSERT_DEBUG(Length()>=2*sizeof(TInt), User::Panic(KSpecAssert_ElemSvrDenApiXCH, 2));
		return *(reinterpret_cast<const TInt*>(Ptr())+1);
		}
	};

class RApiExtSubSessionBase;
class RApiExtSessionBase;

//-=========================================================
//
// Messages
//
//-=========================================================

class TApiExtReqMsg : public Meta::SMetaDataNetCtor
/**
	A self routing API extension request message.

	@internalTechnology
*/
	{
public:
	//[MZ]: Please consult before thinking of increasing any of these
	//values or classification.

	/**
		Used for in-place (on the stack) reconstruction of the message
		@internalTechnology
	*/
	static const TUint KMaxApiExtIpcMsgExternalisedSize = 512;

	/**
		Used for in-place (on the stack) reconstruction of the message.
		Usually slightly bigger than the externalised form (empty buffers
		by value, compression, etc).
		@internalTechnology
	*/
	static const TUint KMaxApiExtIpcMsgStructureSize = KMaxApiExtIpcMsgExternalisedSize + 256;
	};

class TApiExtSubSessionReqMsg : public TApiExtReqMsg
/**
	A subsession API extension request message.

	@internalTechnology
*/
	{
public:
	EXPORT_TYPEID_TABLE

public:
	virtual void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
	                       TSubSessionUniqueId aClientUniqueId,
	                       Elements::RResponseMsg& aResponseMsg) = 0;
	};

class TApiExtSessionReqMsg : public TApiExtReqMsg
/**
	A session API extension request message.

	@internalTechnology
*/
	{
public:
	EXPORT_TYPEID_TABLE

public:
	virtual void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
	                       TSessionUniqueId aClientUniqueId,
	                       Elements::RResponseMsg& aResponseMsg) = 0;
	};

class TApiExtRespMsg : public Meta::SMetaDataECom
/**	API extension response message.

	@internalTechnology
*/
	{
public:
	EXPORT_TYPEID_TABLE

public:
	/** Creates a new API extension message using ECOM.

    @param aTypeId Id of the class (Composed of Uid of the implementation and an integer sub-type).
    @return a generic pointer to a specific message if successful, otherwise leaves with system error code.
    */
	IMPORT_C static TApiExtRespMsg* NewL(const Meta::STypeId& aTypeId);

	/** Creates a new API extension message using ECOM.

    @param aMsgBuffer a buffer containing the serialised form of the message object.
    @return a generic pointer to a specific event if successful, otherwise leaves with system error code.
    */
	IMPORT_C static TApiExtRespMsg* NewL(const TDesC8& aMsgBuffer);
	};

//-=========================================================
//
// Client Side APIs
//
//-=========================================================
class RApiExtCommonBase
/**
	API extension base class

	Provides buffer management for asynchronous IPC calls.

	@internalTechnology
*/
	{
	friend class RApiExtSubSessionBase; //to access iIdentification
	friend class RApiExtSessionBase; //to access iIdentification

protected:
	TInt PrepareForSending(TApiExtReqMsg& aRequestMsg, Elements::TRBuf8*& aBuffer) const;
	void Close();

protected:
	TApiExtIdentification iIdentification; //Interface id, scope id, etc

private:
	mutable RPointerArray<Elements::TRBuf8> iBuffers;
	};

class RExtendableSubSessionBase : public RSubSessionBase
/**
	Base class for client SubSessions supporting extensions.

	@internalTechnology
*/
	{
	friend class RApiExtSubSessionBase;
	};

class RApiExtSubSessionBase : public RApiExtCommonBase
/**
	Base class for client extension interfaces, opened on RExtendableSubSessionBase

	@internalTechnology
*/
	{
public:
	IMPORT_C void Close();

protected:
	IMPORT_C TInt Open(RExtendableSubSessionBase& aExtendableInterface, TInt aInterfaceId);
	IMPORT_C void SendReceive(TApiExtReqMsg& aMsg, TDes8& aReqAndRespBuf, TRequestStatus& aStatus) const;
	IMPORT_C void SendReceive(TApiExtReqMsg& aMsg, TDes8& aReqAndRespBuf, TDes8& aReqAndRespBuf2, TRequestStatus& aStatus) const;
	IMPORT_C void Send(TApiExtReqMsg& aMsg, TRequestStatus& aStatus) const;
	IMPORT_C void Send(TApiExtReqMsg& aMsg, TDes8& aReqAndRespBuf2, TRequestStatus& aStatus) const;
	IMPORT_C void Send(TApiExtReqMsg& aMsg, const RHandleBase& aHandleBase, TRequestStatus& aStatus) const;

protected:
	RExtendableSubSessionBase iSubSession;
	};

class RExtendableSessionBase : public RSessionBase
/**
	Base class for client Sessions supporting extensions.

	@internalTechnology
*/
	{
	friend class RApiExtSessionBase;
	};

class RApiExtSessionBase : public RApiExtCommonBase
/**
	Base class for client extension interfaces, opened on RExtendableSessionBase

	@internalTechnology
*/
	{
public:
	IMPORT_C void Close();

protected:
	IMPORT_C TInt Open(RExtendableSessionBase& aExtendableInterface, TInt aInterfaceId);
	IMPORT_C void SendReceive(TApiExtReqMsg& aMsg, TDes8& aReqAndRespBuf, TRequestStatus& aStatus) const;
	IMPORT_C void SendReceive(TApiExtReqMsg& aMsg, TDes8& aReqAndRespBuf, TDes8& aReqAndRespBuf2, TRequestStatus& aStatus) const;
	IMPORT_C void Send(TApiExtReqMsg& aMsg, TRequestStatus& aStatus) const;
	IMPORT_C void Send(TApiExtReqMsg& aMsg, TDes8& aReqAndRespBuf2, TRequestStatus& aStatus) const;
	IMPORT_C void Send(TApiExtReqMsg& aMsg, const RHandleBase& aHandleBase, TRequestStatus& aStatus) const;

protected:
	RExtendableSessionBase iSession;
	};

} //namespace Den

#endif //SYMBIAN_DEN_APIEXTENSION_CLIENT_H

