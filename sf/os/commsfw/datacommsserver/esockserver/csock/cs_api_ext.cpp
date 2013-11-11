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
//

/**
 @file
*/

#include <comms-infras/es_api_ext.h>
#include <comms-infras/es_connectionservermessages.h>
#include <comms-infras/api_ext_msg.h>
#include <comms-infras/sockmes.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCSockcsp_xt, "ESockCSockcsp_xt");
#endif

using namespace Elements;


CCommsSessionApiExtProvider::CCommsSessionApiExtProvider(RCommsSession& session)
: iSession(session)
	{
	}
	
CCommsSessionApiExtProvider *CCommsSessionApiExtProvider::Open(
		RCommsSession& aExtensionProvider, TSupportedCommsApiExt aInterfaceId, TInt &error)
	{
	__ASSERT_ALWAYS(aExtensionProvider.Handle(), User::Panic(KApiExtPanic, EApiExtBadHandle));
	CCommsSessionApiExtProvider *provider = new CCommsSessionApiExtProvider(aExtensionProvider);
	if (!provider)
		{
		error = KErrNoMemory;
		return NULL;
		}
		
	error = aExtensionProvider.SendReceive(ECMApiExtBindIface, TIpcArgs(aInterfaceId));

	if (KErrNone != error)
		{
		delete provider;
		return NULL;
		}
	return provider;
	}
	
TInt CCommsSessionApiExtProvider::Close(const TIpcArgs& aArgs) const	
	{
	return iSession.SendReceive(ECMApiExtIfaceClose, aArgs);
	}

TInt CCommsSessionApiExtProvider::Handle() const
	{
	return iSession.Handle();
	}

TInt CCommsSessionApiExtProvider::Send(const TIpcArgs& aArgs) const	
	{
	return iSession.SendReceive(ECMApiExtIfaceSend, aArgs);
	}

void CCommsSessionApiExtProvider::SendReceive(const TIpcArgs& aArgs,TRequestStatus& aStatus) const
	{
	iSession.SendReceive(ECMApiExtIfaceSendReceive, aArgs, aStatus);
	}

void CCommsSessionApiExtProvider::Release()
	{
	delete this;
	}

CCommsSubSessionApiExtProvider::CCommsSubSessionApiExtProvider(RCommsSubSession& subsession)
: iSubSession(subsession)
	{
	}

CCommsSubSessionApiExtProvider *CCommsSubSessionApiExtProvider::Open(
		RCommsSubSession& aExtensionProvider, TSupportedCommsApiExt aInterfaceId, TInt &error)
	{
	__ASSERT_ALWAYS(aExtensionProvider.SubSessionHandle(), User::Panic(KApiExtPanic, EApiExtBadHandle));
	CCommsSubSessionApiExtProvider *provider = new CCommsSubSessionApiExtProvider(aExtensionProvider);
	if (!provider)
		{
		error = KErrNoMemory;
		return NULL;
		}
		
	error = aExtensionProvider.SendReceive(ECommsApiExtBindIface, TIpcArgs(aInterfaceId));

	if (KErrNone != error)
		{
		delete provider;
		return NULL;
		}
	return provider;
	}

TInt CCommsSubSessionApiExtProvider::Close(const TIpcArgs& aArgs) const	
	{
	return iSubSession.SendReceive(ECommsApiExtIfaceClose, aArgs);
	}

TInt CCommsSubSessionApiExtProvider::Handle() const
	{
	return iSubSession.SubSessionHandle();
	}

TInt CCommsSubSessionApiExtProvider::Send(const TIpcArgs& aArgs) const	
	{
	return iSubSession.SendReceive(ECommsApiExtIfaceSend, aArgs);
	}

void CCommsSubSessionApiExtProvider::SendReceive(const TIpcArgs& aArgs,TRequestStatus& aStatus) const
	{
	iSubSession.SendReceive(ECommsApiExtIfaceSendReceive, aArgs, aStatus);
	}

void CCommsSubSessionApiExtProvider::Release()
	{
	delete this;
	}


EXPORT_C RCommsApiExtensionBase::RCommsApiExtensionBase()
: iProvider(0)
    {
    }

EXPORT_C TInt RCommsApiExtensionBase::Open(RCommsSession& aExtensionProvider, TSupportedCommsApiExt aInterfaceId)
	{
	if (iProvider)
		{
		return KErrInUse;
		}

	TInt error = KErrNone;
	iProvider = CCommsSessionApiExtProvider::Open(aExtensionProvider, aInterfaceId, error);
	
	if (error == KErrNone)
		{
		iInterfaceId = aInterfaceId;
		}	
	return error;
	}

EXPORT_C TInt RCommsApiExtensionBase::Open(RCommsSubSession& aExtensionProvider, TSupportedCommsApiExt aInterfaceId)
	{
	if (iProvider)
		{
		return KErrInUse;
		}

	TInt error = KErrNone;
	iProvider = CCommsSubSessionApiExtProvider::Open(aExtensionProvider, aInterfaceId, error);
	
	if (error == KErrNone)
		{
		iInterfaceId = aInterfaceId;
		}	
	return error;
	}


EXPORT_C void RCommsApiExtensionBase::Close()
	{
	if (iProvider && iProvider->Handle())
		{
		iProvider->Close(TIpcArgs(iInterfaceId));
		iProvider->Release();
		iProvider = NULL;
		}
	
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
	}

EXPORT_C void RCommsApiExtensionBase::SendRequest(TCommsApiExtReqMsg& aRequestMsg, TDes8& aResponseBuf, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iProvider && iProvider->Handle(), User::Panic(KApiExtPanic, EApiExtBadHandle));

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
		if (error==KErrNone)
			{
			error = aRequestMsg.Store(*buffer);
			if (error==KErrNone)
				{
				iProvider->SendReceive(TIpcArgs(iInterfaceId,buffer,&aResponseBuf), aStatus);
				}
			else
				{
				__ASSERT_DEBUG(iBuffers[iBuffers.Count()-1]==buffer, User::Panic(KSpecAssert_ESockCSockcsp_xt, 1)); //make sure we are removing what we believe we are removing
				iBuffers.Remove(iBuffers.Count()-1); //remove the buffer
				delete buffer;
				buffer = NULL;
				}
			}
		else
			{
			delete buffer;
			buffer = NULL;
			}
		}

	if (error!=KErrNone)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,error);
		return;
		}
	}

EXPORT_C void RCommsApiExtensionBase::SendMessage(TCommsApiExtReqMsg& aRequestMsg)
	{
	__ASSERT_ALWAYS(iProvider && iProvider->Handle(), User::Panic(KApiExtPanic, EApiExtBadHandle));
	__ASSERT_ALWAYS((aRequestMsg.Length() <= KMaxExtApiIpcMsgLength), User::Panic(KApiExtPanic, EApiExtMetaOverflow));

	TBuf8<KMaxExtApiIpcMsgLength> buffer;
	if (aRequestMsg.Store(buffer) != KErrNone)
		{
		User::Panic(KApiExtPanic, EApiExtMetaOverflow);
		}
		
	iProvider->Send(TIpcArgs(iInterfaceId, &buffer));
	}

