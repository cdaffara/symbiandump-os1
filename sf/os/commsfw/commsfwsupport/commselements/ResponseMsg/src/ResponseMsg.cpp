// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Enable logging for debug builds or builds with the comms flogger enabled - must be defined before including e32utrace.h
#if (defined(_DEBUG) || defined(__FLOG_ACTIVE)) && !defined(SYMBIAN_TRACE_ENABLE)
#define SYMBIAN_TRACE_ENABLE
#endif
#include <e32utrace.h>

#include <elements/responsemsg.h>
#include <elements/sd_apiextensionclient.h>
#include <elements/nm_common.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemRespMessRspn, "ElemRespMessRspn");
#endif

using namespace Elements;
using namespace Den;
using namespace Messages;

#ifdef SYMBIAN_TRACE_ENABLE
class RMLogging
	{
public:
	static void Printf(TRefByValue<const TDesC8> aFmt, ...)
		{
		const TInt KPrimaryFilter = 194;
		TBuf8<200> buf;
		TDes8IgnoreOverflow overflow;
		VA_LIST list;
		VA_START(list, aFmt);
		buf.AppendFormatList(aFmt, list, &overflow);
		VA_END(list);
		UTracePfAny(KPrimaryFilter, KText, ETrue, EFalse, buf.Length(), buf.Ptr(), buf.Length());
		}
	};
#define RMLOG(logArgs) RMLogging::Printf logArgs
#else
#define RMLOG(logArgs)
#endif //#ifdef SYMBIAN_TRACE_ENABLE


EXPORT_C RResponseMsg::RResponseMsg(const RMessage2& aMessage, TInt aInterfaceId, TInt aRequestMsgParam, TInt aResponseMsgParam)
:	RMessage2(aMessage), iInterfaceId(aInterfaceId), iRequestMsgParam(aRequestMsgParam), iResponseMsgParam(aResponseMsgParam)
	{
	__ASSERT_DEBUG(aRequestMsgParam >= -1 && aRequestMsgParam < 3, User::Panic(KSpecAssert_ElemRespMessRspn, 1));
	__ASSERT_DEBUG(aResponseMsgParam >= -1 && aResponseMsgParam < 3, User::Panic(KSpecAssert_ElemRespMessRspn, 2));
	}

EXPORT_C RResponseMsg::RResponseMsg(const RMessage2& aMessage, const Den::TApiExtIdentification& aInterfaceId, TInt aRequestMsgParam, TInt aResponseMsgParam)
:	RMessage2(aMessage), iInterfaceId(aInterfaceId), iRequestMsgParam(aRequestMsgParam), iResponseMsgParam(aResponseMsgParam)
	{
	__ASSERT_DEBUG(aRequestMsgParam >= -1 && aRequestMsgParam < 3, User::Panic(KSpecAssert_ElemRespMessRspn, 3));
	__ASSERT_DEBUG(aResponseMsgParam >= -1 && aResponseMsgParam < 3, User::Panic(KSpecAssert_ElemRespMessRspn, 4));
	}

EXPORT_C TInt RResponseMsg::InterfaceId() const
	{
	return iInterfaceId.InterfaceId();
	}

EXPORT_C TInt RResponseMsg::ScopeId() const
	{
	return iInterfaceId.ScopeId();
	}

EXPORT_C void RResponseMsg::Complete(TInt aError)
	{
  	if (iHandle)
  		{
		RMLOG((_L8("RResponseMsg(%08x)::Complete(%08x) with %d"), this, iHandle, aError));
  		RMessage2::Complete(aError);
  		}
	}

EXPORT_C void RResponseMsg::Complete(Meta::SMetaDataECom& aMsg)
	{
	__ASSERT_DEBUG(iResponseMsgParam < 3, User::Panic(KSpecAssert_ElemRespMessRspn, 5));

	RBuf8 buffer;
	TInt err = buffer.Create(aMsg.Length());
	if (err == KErrNone)
		{
		Complete(aMsg, buffer);
		}
	else
		{
		Complete(err);
		}
	buffer.Close();
	}

EXPORT_C void RResponseMsg::Complete(Meta::SMetaDataECom& aMsg, TDes8& aDstBuff)
	{
	__ASSERT_DEBUG(iResponseMsgParam < 3, User::Panic(KSpecAssert_ElemRespMessRspn, 6));
	__ASSERT_DEBUG(aDstBuff.MaxLength() >= aMsg.Length(), User::Panic(KSpecAssert_ElemRespMessRspn, 7));
	aDstBuff.Zero();
	TInt err = aMsg.Store(aDstBuff);
	if (err == KErrNone)
		{
		err = RMessage2::Write(iResponseMsgParam, aDstBuff);
		}
	Complete(err);
	}

EXPORT_C void RResponseMsg::Complete(RHandleBase& aHandle) 
    {
    RMessage2::Complete(aHandle);
    }

EXPORT_C void RResponseMsg::Panic(const TDesC& aCategory, TInt aReason) const
	{
#ifdef SYMBIAN_TRACE_ENABLE
	TBuf8<16> cat;
	cat.Copy(aCategory.Left(16));
	RMLOG((_L8("RResponseMsg(%08x)::Panic(%08x) with %S:%d"), this, iHandle, &cat, aReason));
#endif
	RMessage2::Panic(aCategory, aReason);
	}

EXPORT_C Meta::SMetaDataNetCtor* RResponseMsg::ReadClientReqMsg(TDes8& aDstBuff)
	{
	//Param 3 is used for the subsession handle
	__ASSERT_DEBUG(iRequestMsgParam >= 0 && iRequestMsgParam < 3, User::Panic(KSpecAssert_ElemRespMessRspn, 8));

	Meta::SMetaDataNetCtor* msg = NULL;
	TBuf8<TApiExtReqMsg::KMaxApiExtIpcMsgExternalisedSize> msgSrc;
	if (0 >= RMessage2::GetDesLength(iRequestMsgParam))
		{
  		//Client violated semantics of this api by providing an empty
  		//buffer (or destroying the buffer prematurely).
  		//Depending on the state of communication with the client
  		//(opening an extension/extension call) the client will be
  		//errored or panicked.
  		__DEBUGGER(); //diagnostic
  		return NULL;
		}

	if (KErrNone==RMessage2::Read(iRequestMsgParam, msgSrc))
		{
		TPtrC8 msgSrcPtr(msgSrc);
		msg = TlsGlobals::Get().VirtualCtor()->New(msgSrcPtr, aDstBuff);
  		if (NULL == msg)
  			{
	  		//Something wrong with provisioning of the msg virtual constructor.
	  		//This means the client is sendng an unexpected request.
	  		//It is also possible that the request message is valid and that
	  		//it wasn't properly requstered during opening of the IPC extension.
	  		//That would usually be the fault of the client's CExtItfMsgPluginInfo.
	  		//Depending on the state the client will be errored or panicked.
  			__DEBUGGER(); //diagnostic

			// Mark the client buffer as read so that client can clean up gracefully
			RMessage2::Write(iRequestMsgParam, KNullDesC8);

  			return NULL;
  			}
  		}

	//Make a reasonable attempt to release the client's buffer
	//There is no need to care if that fails or succeeds.
	//Can be safely done even when a response is expected later on
	//(since it only has an effect on the default client side buffer
	//management used for no-responses).
	RMessage2::Write(iRequestMsgParam, KNullDesC8); // mark client buffer as read
  	return msg;
	}

