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
// messageintercept.cpp
// 
//

/**
 @file
*/

#include "sd_minterceptmsgs.h"
#include <elements/sd_mintercept.h>
#include <elements/metatype.h>
#include <elements/metatypevariablelen.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenMIntMC, "ElemSvrDenMIntMC");
#endif

using namespace Den;
using namespace Messages;
using namespace Elements;

// Implementation UID for MessageIntercept messages
static const TInt KMessageInterceptRequestMsgImplementationUid = 0x10283063;

enum
	{
	EResetMsg = 0,
	EEnableMsg,
	EEnableFallibleMsgModeMsg,
	EDisableMsg,
	EAppendExpressionMsg,
	EAppendActionMsg,
	EQueryCompleteMsg,
	EInitFallibleMsgModeMsg,
	ECheckFallibleMsgModeMsg
	};

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSoftResetMsg, KMessageInterceptRequestMsgImplementationUid, EResetMsg)
END_ATTRIBUTE_TABLE_BASE(TApiExtSessionReqMsg, EApiExtSessionReqMsg)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TEnableMsg, KMessageInterceptRequestMsgImplementationUid, EEnableMsg)
END_ATTRIBUTE_TABLE_BASE(TApiExtSessionReqMsg, EApiExtSessionReqMsg)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TEnableFallibleMsgModeMsg, KMessageInterceptRequestMsgImplementationUid, EEnableFallibleMsgModeMsg)
	REGISTER_ATTRIBUTE(TEnableFallibleMsgModeMsg, iCount, TMeta<TInt>)
END_ATTRIBUTE_TABLE_BASE(TApiExtSessionReqMsg, EApiExtSessionReqMsg)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TDisableMsg, KMessageInterceptRequestMsgImplementationUid, EDisableMsg)
END_ATTRIBUTE_TABLE_BASE(TApiExtSessionReqMsg, EApiExtSessionReqMsg)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TAppendExpressionMsg, KMessageInterceptRequestMsgImplementationUid, EAppendExpressionMsg)
	REGISTER_ATTRIBUTE(TAppendExpressionMsg, iExpression, TMeta<TEventExpression>)
END_ATTRIBUTE_TABLE_BASE(TApiExtSessionReqMsg, EApiExtSessionReqMsg)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TAppendActionMsg, KMessageInterceptRequestMsgImplementationUid, EAppendActionMsg)
	REGISTER_ATTRIBUTE(TAppendActionMsg, iAction, TMeta<TAction>)
END_ATTRIBUTE_TABLE_BASE(TApiExtSessionReqMsg, EApiExtSessionReqMsg)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TQueryCompleteMsg, KMessageInterceptRequestMsgImplementationUid, EQueryCompleteMsg)
END_ATTRIBUTE_TABLE_BASE(TApiExtSessionReqMsg, EApiExtSessionReqMsg)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TInitFallibleMsgModeMsg, KMessageInterceptRequestMsgImplementationUid, EInitFallibleMsgModeMsg)
	REGISTER_ATTRIBUTE(TInitFallibleMsgModeMsg, iArray, TMetaVarLen8<TBuf8<1024> >)
END_ATTRIBUTE_TABLE_BASE(TApiExtSessionReqMsg, EApiExtSessionReqMsg)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TCheckFallibleMsgModeMsg, KMessageInterceptRequestMsgImplementationUid, ECheckFallibleMsgModeMsg)
END_ATTRIBUTE_TABLE_BASE(TApiExtSessionReqMsg, EApiExtSessionReqMsg)

// Message implementations
//---------------------------------
EXPORT_C TEnableFallibleMsgModeMsg::TEnableFallibleMsgModeMsg()
:	iCount(0)
	{
	}

EXPORT_C TEnableFallibleMsgModeMsg::TEnableFallibleMsgModeMsg(TInt aCount)
:	iCount(aCount)
	{
	}

EXPORT_C TAppendExpressionMsg::TAppendExpressionMsg()
	{
	}

EXPORT_C TAppendExpressionMsg::TAppendExpressionMsg(const TEventExpression& aExpression)
:	iExpression(aExpression)
	{
	}

EXPORT_C TAppendActionMsg::TAppendActionMsg(const TAction& aAction)
:	iAction(aAction)
	{
	}

EXPORT_C void TSoftResetMsg::DispatchL(NetInterfaces::TInterfaceControl& /*aIntfCtl*/,
			                           TSessionUniqueId /*aClientUniqueId*/,
	                                   RResponseMsg& /*aResponseMsg*/)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// Simply reset the register
	User::Leave(KErrNotSupported);
	//return FetchMsgInterceptControl().SoftReset();
#else
	User::Leave(KErrNotSupported);
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

EXPORT_C void TEnableMsg::DispatchL(NetInterfaces::TInterfaceControl& /*aIntfCtl*/,
			                        TSessionUniqueId /*aClientUniqueId*/,
	                                RResponseMsg& /*aResponseMsg*/)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// Enable the intercept register
	//return FetchMsgInterceptControl().Enable();
	User::Leave(KErrNotSupported);
#else
	User::Leave(KErrNotSupported);
#endif // #ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

EXPORT_C void TEnableFallibleMsgModeMsg::DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
			                                       TSessionUniqueId /*aClientUniqueId*/,
	                                               RResponseMsg& aResponseMsg)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	AFallibleTestControl* tc = static_cast<AFallibleTestControl*>(aIntfCtl.FetchInterfaceL(aResponseMsg.InterfaceId()));
	aResponseMsg.Complete(tc->Enable(iCount));
#else
	// Fixing unused local variable warnings.
	(void)aIntfCtl;
	(void)aResponseMsg;

	User::Leave(KErrNotSupported);
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

EXPORT_C TSoftResetMsg::TSoftResetMsg()
	{
	}

EXPORT_C TEnableMsg::TEnableMsg()
	{
	}

EXPORT_C void TInitFallibleMsgModeMsg::DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
			                                     TSessionUniqueId /*aClientUniqueId*/,
	                                             RResponseMsg& aResponseMsg)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	AFallibleTestControl* tc = static_cast<AFallibleTestControl*>(aIntfCtl.FetchInterfaceL(aResponseMsg.InterfaceId()));
	const RArray<TNodeSignal::TMessageId> array(sizeof(TNodeSignal::TMessageId), reinterpret_cast<TNodeSignal::TMessageId*>(const_cast<TUint8*>(iArray.Ptr())), iArray.Size()/sizeof(TNodeSignal::TMessageId));
	tc->InitL(array);
	aResponseMsg.Complete(KErrNone);
#else
	// Fixing unused local variable warnings.
	(void)aIntfCtl;
	(void)aResponseMsg;

	User::Leave(KErrNotSupported);
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

EXPORT_C void TInitFallibleMsgModeMsg::InitL(RArray<TNodeSignal::TMessageId>& aArray)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	__ASSERT_DEBUG(aArray.Count(), User::Panic(KSpecAssert_ElemSvrDenMIntMC, 1));
	for(TInt i=0 ; i<aArray.Count() ; i++)
		{
		iArray.Append((TUint8*)&aArray[i], sizeof(TNodeSignal::TMessageId));
		}
#else
	// Fixing unused local variable warnings.
	(void)aArray;

	User::Leave(KErrNotSupported);
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

EXPORT_C TInitFallibleMsgModeMsg::TInitFallibleMsgModeMsg()
	{
	}

EXPORT_C TCheckFallibleMsgModeMsg::TCheckFallibleMsgModeMsg()
	{
	}

EXPORT_C void TCheckFallibleMsgModeMsg::DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
			                                       TSessionUniqueId /*aClientUniqueId*/,
	                                               RResponseMsg& aResponseMsg)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	AFallibleTestControl* tc = static_cast<AFallibleTestControl*>(aIntfCtl.FetchInterfaceL(aResponseMsg.InterfaceId()));
	TCheckFallibleMsgModeResponseMsg msg;
	msg.iCheckResult = tc->CheckFinished();
	TBuf8<TMIMessages::KMaxExternalisedReqAndRespMsgSize> msgDst;
	aResponseMsg.Complete(msg, msgDst);
	msg.~TCheckFallibleMsgModeResponseMsg();
#else
	// Fixing unused local variable warnings.
	(void)aIntfCtl;
	(void)aResponseMsg;

	User::Leave(KErrNotSupported);
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

EXPORT_C void TDisableMsg::DispatchL(NetInterfaces::TInterfaceControl& /*aIntfCtl*/,
			                         TSessionUniqueId /*aClientUniqueId*/,
	                                 RResponseMsg& /*aResponseMsg*/)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// Disable the intercept register
	User::Leave(KErrNotSupported);
#else
	User::Leave(KErrNotSupported);
#endif//#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

EXPORT_C TDisableMsg::TDisableMsg()
	{
	}

EXPORT_C void TAppendExpressionMsg::DispatchL(NetInterfaces::TInterfaceControl& /*aIntfCtl*/,
			                                  TSessionUniqueId /*aClientUniqueId*/,
	                                          RResponseMsg& /*aResponseMsg*/)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// Append expression to the intercept register (it should go on the current pattern)
	//return FetchMsgInterceptControl().AppendExpression(iExpression);
	User::Leave(KErrNotSupported);
#else
	User::Leave(KErrNotSupported);
#endif
	}

EXPORT_C TAppendActionMsg::TAppendActionMsg()
	{
	}

EXPORT_C void TAppendActionMsg::DispatchL(NetInterfaces::TInterfaceControl& /*aIntfCtl*/,
			                              TSessionUniqueId /*aClientUniqueId*/,
	                                      RResponseMsg& /*aResponseMsg*/)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// Append action to the intercept register (it should terminate the current pattern)
	//return FetchMsgInterceptControl().AppendAction(iAction);
	User::Leave(KErrNotSupported);
#else
	User::Leave(KErrNotSupported);
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

EXPORT_C TQueryCompleteMsg::TQueryCompleteMsg()
	{
	}

EXPORT_C void TQueryCompleteMsg::DispatchL(NetInterfaces::TInterfaceControl& /*aIntfCtl*/,
			                               TSessionUniqueId /*aClientUniqueId*/,
	                                       RResponseMsg& /*aResponseMsg*/)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// Query completion of the register
	//return FetchMsgInterceptControl().QueryComplete();
	User::Leave(KErrNotSupported);
#else
	User::Leave(KErrNotSupported);
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

DEFINE_MVIP_CTR(TSoftResetMsg);
DEFINE_MVIP_CTR(TEnableMsg);
DEFINE_MVIP_CTR(TEnableFallibleMsgModeMsg);
DEFINE_MVIP_CTR(TDisableMsg);
DEFINE_MVIP_CTR(TAppendExpressionMsg);
DEFINE_MVIP_CTR(TAppendActionMsg);
DEFINE_MVIP_CTR(TQueryCompleteMsg);
DEFINE_MVIP_CTR(TInitFallibleMsgModeMsg);
DEFINE_MVIP_CTR(TCheckFallibleMsgModeMsg);

/** Message interface registration table */
const TImplementationProxy ImplementationTable[] =
	{
	MVIP_CTR_ENTRY(EResetMsg, TSoftResetMsg),
	MVIP_CTR_ENTRY(EEnableMsg, TEnableMsg),
	MVIP_CTR_ENTRY(EEnableFallibleMsgModeMsg, TEnableFallibleMsgModeMsg),
	MVIP_CTR_ENTRY(EDisableMsg, TDisableMsg),
	MVIP_CTR_ENTRY(EAppendExpressionMsg, TAppendExpressionMsg),
	MVIP_CTR_ENTRY(EAppendActionMsg, TAppendActionMsg),
	MVIP_CTR_ENTRY(EQueryCompleteMsg, TQueryCompleteMsg),
	MVIP_CTR_ENTRY(EInitFallibleMsgModeMsg, TInitFallibleMsgModeMsg),
	MVIP_CTR_ENTRY(ECheckFallibleMsgModeMsg, TCheckFallibleMsgModeMsg)
	};


TUid TMIMessages::ImplementationUid()
	{
	return TUid::Uid(KMessageInterceptRequestMsgImplementationUid);
	}

TInt TMIMessages::ImplementationProxyTableSize()
	{
	return (sizeof(ImplementationTable) / sizeof(ImplementationTable[0]));
	};

const TImplementationProxy* TMIMessages::ImplementationProxyTable()
	{
	return ImplementationTable;
	};


//*************************************************************************
//
//Response messages
//
//*************************************************************************

EXPORT_C TCheckFallibleMsgModeResponseMsg* TCheckFallibleMsgModeResponseMsg::NewL() //For the ecom factory
	{
	return new (ELeave) TCheckFallibleMsgModeResponseMsg;
	}

EXPORT_C TCheckFallibleMsgModeResponseMsg::TCheckFallibleMsgModeResponseMsg()
:	iCheckResult(EFalse)
	{
	}

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TCheckFallibleMsgModeResponseMsg, KMessageInterceptResponseMsgImplementationUid, ECheckFallibleMsgModeResponseMsg )
	REGISTER_ATTRIBUTE( TCheckFallibleMsgModeResponseMsg, iCheckResult, TMeta<TBool>)
END_ATTRIBUTE_TABLE_BASE( TApiExtRespMsg, 0 )


