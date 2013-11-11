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
// sd_apiextension.cpp
// 
//

/**
 @file
 @internalTechnology
*/

#include "sd_apiextensionclient.h"
#include "sd_apiextensionipc.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenApiXCC, "ElemSvrDenApiXCC");
#endif

using namespace Den;
using namespace Elements;


_LIT(KDenApiExtPanic, "API Extension Interface");
/**
	API extension interface panic codes

	@internalTechnology
*/

enum TDenApiExtPanic
	{
	EDenApiExtBadSessionHandle = 0,	//< The session handle of the extension interface is invalid
	EDenApiExtBadSubSessionHandle,	//< The subsession handle of the extension interface is invalid
	EDenApiExtBadHandle,	//< The session handle of the extension interface is invalid
	EDenApiExtMetaOverflow,			//< Storing a message into a descriptor has overflowed
	};


// Implementation interface id for messages
static const TInt KApiExtMsgImplementationUid = 0x10285c89;

enum
	{
	EApiExtReqMsg = 1,
	EApiExtSubSessionReqMsg,
	EApiExtSessionReqMsg,
	EApiExtRespMsg
	};

//
//TApiExtRespMsg
//EXPORT_REGISTER_TYPEID(TApiExtReqMsg, KReqMsgImplementationUid, EApiExtReqMsg)

//
//TApiExtSubSessionReqMsg

EXPORT_REGISTER_TYPEID(TApiExtSubSessionReqMsg, KApiExtMsgImplementationUid, EApiExtSubSessionReqMsg)

//
//TApiExtHostRoutingReqMsg

EXPORT_REGISTER_TYPEID(TApiExtSessionReqMsg, KApiExtMsgImplementationUid, EApiExtSessionReqMsg)


//
//TApiExtRespMsg
EXPORT_C TApiExtRespMsg* TApiExtRespMsg::NewL(const Meta::STypeId& aTypeId)
	{
	return static_cast<TApiExtRespMsg*>(Meta::SMetaDataECom::NewInstanceL(aTypeId));
	}

EXPORT_C TApiExtRespMsg* TApiExtRespMsg::NewL(const TDesC8& aMsgBuffer)
	{
	TPtrC8 buf(aMsgBuffer.Ptr(), aMsgBuffer.Length());
	return static_cast<TApiExtRespMsg*>(Meta::SMetaDataECom::LoadL(buf));
	}

EXPORT_REGISTER_TYPEID(TApiExtRespMsg, KApiExtMsgImplementationUid, EApiExtRespMsg)

//
//RApiExtBase
TInt RApiExtCommonBase::PrepareForSending(TApiExtReqMsg& aRequestMsg, TRBuf8*& aBuffer) const
	{
	//clean up all used up buffers
	for (TInt index = iBuffers.Count() - 1; index >= 0; --index)
		{
		if (iBuffers[index]->Length()==0)
			{
			delete iBuffers[index];
			iBuffers.Remove(index);
			}
		}
	TInt error = KErrNoMemory;
	TRBuf8* buffer = TRBuf8::New(aRequestMsg.Length());
	if (buffer!=NULL)
		{
		error = iBuffers.Append(buffer); //stick new buffer into the array
		if (error!=KErrNone)
			{
			delete buffer;
			buffer = NULL;
			}
		else
			{
			error = aRequestMsg.Store(*buffer);
			if (error!=KErrNone)
				{
				__ASSERT_DEBUG(iBuffers[iBuffers.Count()-1]==buffer, User::Panic(KSpecAssert_ElemSvrDenApiXCC, 1)); //make sure we are removing what we believe we are removing
				iBuffers.Remove(iBuffers.Count()-1); //remove the buffer
				delete buffer;
				buffer = NULL;
				}
			else
				{
				aBuffer = buffer;
				}
			}
		}
	return error;
	}

void RApiExtCommonBase::Close()
	{
#if defined( _DEBUG )
	for (TInt index = iBuffers.Count() - 1; index >= 0; --index)
		{
		if (iBuffers[index]->Length()!=0)
			{
			User::Invariant(); //The request has not been processed by the server yet
			}
		}
#endif

	iBuffers.ResetAndDestroy();
	iIdentification=TApiExtIdentification(0,0);
	}

//
//RApiExtSubSessionBase
EXPORT_C TInt RApiExtSubSessionBase::Open(RExtendableSubSessionBase& aExtendableInterface, TInt aInterfaceId)
	{
	__ASSERT_ALWAYS(aExtendableInterface.SubSessionHandle(), User::Panic(KDenApiExtPanic, EDenApiExtBadSessionHandle));

	TApiExtIdentification identification(aInterfaceId);
	TInt error = aExtendableInterface.SendReceive(EIpcSubSessionApiExtIfaceOpen, TIpcArgs(&identification));
	if (error == KErrNone)
		{
		iSubSession = aExtendableInterface;
		iIdentification = identification;
		}
	return error;
	}

EXPORT_C void RApiExtSubSessionBase::Close()
	{
	if (!iSubSession.SubSessionHandle())
		return;

	iSubSession.SendReceive(EIpcSubSessionApiExtIfaceClose, TIpcArgs(&iIdentification));
	RApiExtCommonBase::Close();
	}

//Calling this function, the client is responsible for providing a buffer
//that is big enough to store first the request message and then the response message.
EXPORT_C void RApiExtSubSessionBase::SendReceive(TApiExtReqMsg& aMsg, TDes8& aReqAndRespBuf, TRequestStatus& aStatus) const
	{
	__ASSERT_ALWAYS(iSubSession.SubSessionHandle(), User::Panic(KDenApiExtPanic, EDenApiExtBadSubSessionHandle));
	aReqAndRespBuf.Zero();
	TInt error = aMsg.Store(aReqAndRespBuf);
	if (error==KErrNone)
		{
		iSubSession.SendReceive(EIpcSubSessionApiExtIfaceSendReceive, TIpcArgs(&iIdentification, &aReqAndRespBuf), aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,error);
		}
	}

EXPORT_C void RApiExtSubSessionBase::SendReceive(TApiExtReqMsg& aMsg, TDes8& aReqAndRespBuf, TDes8& aReqAndRespBuf2, TRequestStatus& aStatus) const
	{
	__ASSERT_ALWAYS(iSubSession.SubSessionHandle(), User::Panic(KDenApiExtPanic, EDenApiExtBadSubSessionHandle));
	aReqAndRespBuf.Zero();
	TInt error = aMsg.Store(aReqAndRespBuf);
	if (error==KErrNone)
		{
		iSubSession.SendReceive(EIpcSubSessionApiExtIfaceSendReceive, TIpcArgs(&iIdentification, &aReqAndRespBuf, &aReqAndRespBuf2), aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,error);
		}
	}

//Calling this function, the client does not have to be concerned about the
//buffer management that happens underneath
EXPORT_C void RApiExtSubSessionBase::Send(TApiExtReqMsg& aMsg, TRequestStatus& aStatus) const
	{
	__ASSERT_ALWAYS(iSubSession.SubSessionHandle(), User::Panic(KDenApiExtPanic, EDenApiExtBadSubSessionHandle));
	TRBuf8* buffer = NULL;
	TInt error = RApiExtCommonBase::PrepareForSending(aMsg, buffer);
	if (error==KErrNone)
		{
		iSubSession.SendReceive(EIpcSubSessionApiExtIfaceSendReceive, TIpcArgs(&iIdentification, buffer), aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,error);
		}
	}

EXPORT_C void RApiExtSubSessionBase::Send(TApiExtReqMsg& aMsg, TDes8& aReqAndRespBuf2, TRequestStatus& aStatus) const
	{
	__ASSERT_ALWAYS(iSubSession.SubSessionHandle(), User::Panic(KDenApiExtPanic, EDenApiExtBadSubSessionHandle));
	TRBuf8* buffer = NULL;
	TInt error = RApiExtCommonBase::PrepareForSending(aMsg, buffer);
	if (error==KErrNone)
		{
		iSubSession.SendReceive(EIpcSubSessionApiExtIfaceSendReceive, TIpcArgs(&iIdentification, buffer, &aReqAndRespBuf2), aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,error);
		}
	}

EXPORT_C void RApiExtSubSessionBase::Send(TApiExtReqMsg& aMsg, const RHandleBase& aHandleBase, TRequestStatus& aStatus) const
	{
	__ASSERT_ALWAYS(iSubSession.SubSessionHandle(), User::Panic(KDenApiExtPanic, EDenApiExtBadSubSessionHandle));
	TRBuf8* buffer = NULL;
	TInt error = RApiExtCommonBase::PrepareForSending(aMsg, buffer);
	if (error==KErrNone)
		{
		iSubSession.SendReceive(EIpcSubSessionApiExtIfaceSendReceive, TIpcArgs(&iIdentification, buffer, aHandleBase), aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,error);
		}
	}

//
//RApiExtSessionBase
EXPORT_C TInt RApiExtSessionBase::Open(RExtendableSessionBase& aExtendableInterface, TInt aInterfaceId)
	{
	__ASSERT_ALWAYS(aExtendableInterface.Handle(), User::Panic(KDenApiExtPanic, EDenApiExtBadSubSessionHandle));

	TApiExtIdentification identification(aInterfaceId);
	TInt error = aExtendableInterface.SendReceive(EIpcSessionApiExtIfaceOpen, TIpcArgs(&identification));
	if (error == KErrNone)
		{
		iSession = aExtendableInterface;
		iIdentification = identification;
		}
	return error;
	}

EXPORT_C void RApiExtSessionBase::Close()
	{
	if (!iSession.Handle())
		return;

	iSession.SendReceive(EIpcSessionApiExtIfaceClose, TIpcArgs(&iIdentification));
	RApiExtCommonBase::Close();
	}

//Calling this function, the client is responsible for providing a buffer
//that is big enough to store first the request message and then the response message.
EXPORT_C void RApiExtSessionBase::SendReceive(TApiExtReqMsg& aMsg, TDes8& aReqAndRespBuf, TRequestStatus& aStatus) const
	{
	__ASSERT_ALWAYS(iSession.Handle(), User::Panic(KDenApiExtPanic, EDenApiExtBadSubSessionHandle));
	aReqAndRespBuf.Zero();
	TInt error = aMsg.Store(aReqAndRespBuf);
	if (error==KErrNone)
		{
		iSession.SendReceive(EIpcSessionApiExtIfaceSendReceive, TIpcArgs(&iIdentification, &aReqAndRespBuf), aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,error);
		}
	}

EXPORT_C void RApiExtSessionBase::SendReceive(TApiExtReqMsg& aMsg, TDes8& aReqAndRespBuf, TDes8& aReqAndRespBuf2, TRequestStatus& aStatus) const
	{
	__ASSERT_ALWAYS(iSession.Handle(), User::Panic(KDenApiExtPanic, EDenApiExtBadSubSessionHandle));
	aReqAndRespBuf.Zero();
	TInt error = aMsg.Store(aReqAndRespBuf);
	if (error==KErrNone)
		{
		iSession.SendReceive(EIpcSessionApiExtIfaceSendReceive, TIpcArgs(&iIdentification, &aReqAndRespBuf, &aReqAndRespBuf2), aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,error);
		}
	}

//Calling this function, the client does not have to be concerned about the
//buffer management that happens underneath
EXPORT_C void RApiExtSessionBase::Send(TApiExtReqMsg& aMsg, TRequestStatus& aStatus) const
	{
	__ASSERT_ALWAYS(iSession.Handle(), User::Panic(KDenApiExtPanic, EDenApiExtBadSubSessionHandle));

	TRBuf8* buffer = NULL;
	TInt error = RApiExtCommonBase::PrepareForSending(aMsg, buffer);
	if (error==KErrNone)
		{
		iSession.SendReceive(EIpcSessionApiExtIfaceSendReceive, TIpcArgs(&iIdentification, buffer), aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,error);
		}
	}

EXPORT_C void RApiExtSessionBase::Send(TApiExtReqMsg& aMsg, TDes8& aReqAndRespBuf2, TRequestStatus& aStatus) const
	{
	__ASSERT_ALWAYS(iSession.Handle(), User::Panic(KDenApiExtPanic, EDenApiExtBadSubSessionHandle));

	TRBuf8* buffer = NULL;
	TInt error = RApiExtCommonBase::PrepareForSending(aMsg, buffer);
	if (error==KErrNone)
		{
		iSession.SendReceive(EIpcSessionApiExtIfaceSendReceive, TIpcArgs(&iIdentification, buffer, &aReqAndRespBuf2), aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,error);
		}
	}

EXPORT_C void RApiExtSessionBase::Send(TApiExtReqMsg& aMsg, const RHandleBase& aHandleBase, TRequestStatus& aStatus) const
	{
	__ASSERT_ALWAYS(iSession.Handle(), User::Panic(KDenApiExtPanic, EDenApiExtBadSubSessionHandle));

	TRBuf8* buffer = NULL;
	TInt error = RApiExtCommonBase::PrepareForSending(aMsg, buffer);
	if (error==KErrNone)
		{
		iSession.SendReceive(EIpcSessionApiExtIfaceSendReceive, TIpcArgs(&iIdentification, buffer, aHandleBase), aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,error);
		}
	}


