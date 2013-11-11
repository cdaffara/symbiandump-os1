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

#ifndef SYMBIAN_ELEMENTS_MSGINTERCEPTMSGS_H
#define SYMBIAN_ELEMENTS_MSGINTERCEPTMSGS_H

#include <elements/sd_mintercept_macro.h>
#include <elements/sd_apiextensionclient.h>
#include <elements/nm_signatures.h>

//[MZ]: Please consider connecting MI at the transport level.
//      The macro solution does not scale too well.

/* ELEMENTS_DEBUG_MESSAGES_THUNK_POINT
// to be reviewed since Message interceptor is decoupled from esock
Elements debug messages currently fit the MIR well as it's a disconnect global blob (mostly disconnected;
worker thread knows about it for init & cleanup & everybody else has macro hiding it). But for some
debugging features closer coupling is required & many of the key classes aren't exported, ie the
obvious home for the debugging code is inside ESOCK. Making them the debug dispatches friends isn't
very good either as it risks hard-to-debug problems where ESOCKSVR is compiled without the support but
the helper DLL is & so dereferences non-existant debug members, etc (and this is particularly likely
on hardware where people always juggle weird mixes of UDEB & UREL to get a small enough ROM & this will
be very hard to debug)

Hence the addition of code ptrs to this MIR structure - poor man's vtbl. Really this shouldn't be in the MIR
but a general debug DLL but (a) it's very slim functionality and (b) the MIR is already a bit ambiguous whether
it's "a" debug DLL or "the".
*/

namespace Den
	{
	const TUint KMaxNodeTagLength = 32;
	typedef TBuf8<KMaxNodeTagLength> TNodeTag;

	class TNodeSpecifier;
	class CPattern;
	class TEventSummaryTriple;

	// Basic types
	//------------

	//TODO: TNodeExpression needs to be based on a more dynamic mechanism
	//to allow for tech specific node descriptions
	class TNodeExpression
		{
		public:
			TNodeExpression() {}

			TNodeExpression(TUid aUid, const TDesC8& aTagToApply = KNullDesC8) :
				iMatchType(EMatchByUid), iUid(aUid), iTagToApplyOnMatch(aTagToApply) {}

			TNodeExpression(const TDesC8& aNodeTag) :
				iMatchType(EMatchByInstance), iMatchTag(aNodeTag), iTagToApplyOnMatch(KNullDesC8) {}

			TBool Compare(const TNodeSpecifier& aNode) const;
			TBool SetTagOnMatch() const {return (iTagToApplyOnMatch.Length() > 0);}
			const TNodeTag& TagToSet() const {return iTagToApplyOnMatch;}

		private:
			// Matching by node info
			TUid iUid;

			// Todo: Include when APId and TierId are implemented for Multimedia
			//TUint iAPId;
			//TUid iTierId;
			TNodeTag iTagToApplyOnMatch;

			// Matching by node name
			TNodeTag iMatchTag;

			enum TMatchType {EMatchByUid, EMatchByInstance} iMatchType;
		};

	class TMsgExpression
		{
		public:
			TMsgExpression() : iMessageId(0), iRealm(0) {};
			TMsgExpression(TUint aMessageId, TUint aRealm) : iMessageId(aMessageId), iRealm(aRealm) {}
			TBool Compare(const Messages::TNodeSignal& aMessage) const;

		private:
			TUint iMessageId;
			TUint iRealm;
		};

	class TEventExpression
		{
		public:
			TEventExpression() : iSenderExpression(), iMsgExpression(), iReceiverExpression() {}
			TEventExpression(
				const TNodeExpression& aSender,
				const TMsgExpression& aMsg,
				const TNodeExpression& aReceiver) :
					iSenderExpression(aSender), iMsgExpression(aMsg), iReceiverExpression(aReceiver)
					{}

			const TMsgExpression& MessageExpression() const {return iMsgExpression;}
			const TNodeExpression& SenderExpression() const {return iSenderExpression;}
			const TNodeExpression& ReceiverExpression() const {return iReceiverExpression;}

		private:
			TNodeExpression iSenderExpression;
			TMsgExpression iMsgExpression;
			TNodeExpression iReceiverExpression;
		};

	class TAction
		{
		public:
			typedef enum {ENoAction, ELeaveAction, EPanicAction} TActionType;

			TAction(TActionType aAction, TInt aError) :
				iAction(aAction), iError(aError) {}
			TAction() : iAction(ENoAction), iError(KErrNone) {}

			TActionType Action() const	{return iAction;}
			TInt Error() const {return iError;}

			TInt ExecuteActionL();

		private:
			TActionType iAction;
			TInt iError;
		};

	// MI messages
	//------------
	class TMIMessages
		{
		public:
			static const TInt KMaxExternalisedReqAndRespMsgSize = 240; //size of the biggest message any direction..

		public:
			static TUid ImplementationUid();
			static TInt ImplementationProxyTableSize();
			static const TImplementationProxy* ImplementationProxyTable();
		};

	// Reset
	class TSoftResetMsg : public Den::TApiExtSessionReqMsg
		{
		public:
			IMPORT_C TSoftResetMsg();
			IMPORT_C virtual void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
			                                TSessionUniqueId aClientUniqueId,
			                                Elements::RResponseMsg& aResponseMsg);
			DECLARE_MVIP_CTR(TSoftResetMsg)
			EXPORT_DATA_VTABLE_AND_FN
		};

	// Enable
	class TEnableMsg : public Den::TApiExtSessionReqMsg
		{
		public:
			IMPORT_C TEnableMsg();
			IMPORT_C virtual void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
			                                TSessionUniqueId aClientUniqueId,
	                                        Elements::RResponseMsg& aResponseMsg);
			DECLARE_MVIP_CTR(TEnableMsg)
			EXPORT_DATA_VTABLE_AND_FN
		};

	// EnableFallibleMsgMode
	class TEnableFallibleMsgModeMsg : public Den::TApiExtSessionReqMsg
		{
		public:
			IMPORT_C TEnableFallibleMsgModeMsg();
			IMPORT_C TEnableFallibleMsgModeMsg(TInt aCount);
			IMPORT_C virtual void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
			                                TSessionUniqueId aClientUniqueId,
	                                        Elements::RResponseMsg& aResponseMsg);
			DECLARE_MVIP_CTR(TEnableFallibleMsgModeMsg)
			EXPORT_DATA_VTABLE_AND_FN
		protected:
			TInt iCount;
		};

	// InitFallibleMsgMode
	class TInitFallibleMsgModeMsg : public Den::TApiExtSessionReqMsg
		{
		public:
			//TInitFallibleMsgModeMsg(RArray<Messages::TNodeSignal::TMessageId>& aArray);
			IMPORT_C TInitFallibleMsgModeMsg();
			IMPORT_C virtual void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
			                                TSessionUniqueId aClientUniqueId,
	                                        Elements::RResponseMsg& aResponseMsg);
			IMPORT_C void InitL(RArray<Messages::TNodeSignal::TMessageId>& aArray);
			DECLARE_MVIP_CTR(TInitFallibleMsgModeMsg)
			EXPORT_DATA_VTABLE_AND_FN

		private:
			TBuf8<TMIMessages::KMaxExternalisedReqAndRespMsgSize> iArray; //TODO: consider alternatives
		};

	// CheckFallibleMsgMode
	class TCheckFallibleMsgModeMsg : public Den::TApiExtSessionReqMsg
		{
		public:
			IMPORT_C TCheckFallibleMsgModeMsg();
			IMPORT_C virtual void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
			                                TSessionUniqueId aClientUniqueId,
	                                        Elements::RResponseMsg& aResponseMsg);
			DECLARE_MVIP_CTR(TCheckFallibleMsgModeMsg)
			EXPORT_DATA_VTABLE_AND_FN
		};

	// Disable
	class TDisableMsg : public Den::TApiExtSessionReqMsg
		{
		public:
			IMPORT_C TDisableMsg();
			IMPORT_C virtual void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
			                                TSessionUniqueId aClientUniqueId,
	                                        Elements::RResponseMsg& aResponseMsg);
			DECLARE_MVIP_CTR(TDisableMsg)
			EXPORT_DATA_VTABLE_AND_FN
		};

	// Append expression
	class TAppendExpressionMsg : public Den::TApiExtSessionReqMsg
		{
		public:
			IMPORT_C TAppendExpressionMsg();
			IMPORT_C TAppendExpressionMsg(const TEventExpression& aExpression);
			IMPORT_C virtual void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
			                                TSessionUniqueId aClientUniqueId,
	                                        Elements::RResponseMsg& aResponseMsg);
			DECLARE_MVIP_CTR(TAppendExpressionMsg)
			EXPORT_DATA_VTABLE_AND_FN

		protected:
			TEventExpression iExpression;
		};

	// Append action
	class TAppendActionMsg : public Den::TApiExtSessionReqMsg
		{
		public:
			IMPORT_C TAppendActionMsg();
			IMPORT_C TAppendActionMsg(const TAction& aAction);
			IMPORT_C virtual void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
			                                TSessionUniqueId aClientUniqueId,
	                                        Elements::RResponseMsg& aResponseMsg);
			DECLARE_MVIP_CTR(TAppendActionMsg)
			EXPORT_DATA_VTABLE_AND_FN

		protected:
			TAction iAction;
		};

	// Query complete
	class TQueryCompleteMsg : public Den::TApiExtSessionReqMsg
		{
		public:
			IMPORT_C TQueryCompleteMsg();
			IMPORT_C virtual void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl,
			                                TSessionUniqueId aClientUniqueId,
	                                        Elements::RResponseMsg& aResponseMsg);
			DECLARE_MVIP_CTR(TQueryCompleteMsg)
			EXPORT_DATA_VTABLE_AND_FN
		};


	//*************************************************************************
	//
	//Response messages
	//
	//*************************************************************************
	const TInt KMessageInterceptResponseMsgImplementationUid = 0x10285C8D;

	/**
	*/
	enum TMIResponseMsgImplementations
		{
		ECheckFallibleMsgModeResponseMsg = 1
		};

	/**
	Ecom response message from server to client on call to RMessageInterceptorFM::Check()
	*/
	class TCheckFallibleMsgModeResponseMsg : public Den::TApiExtRespMsg
		{
	public:
		EXPORT_DATA_VTABLE_AND_FN

	public:
		IMPORT_C static TCheckFallibleMsgModeResponseMsg* NewL(); //For the ecom factory
		IMPORT_C TCheckFallibleMsgModeResponseMsg();

	public:
		TBool iCheckResult;
		};

	} // Den


#endif
//SYMBIAN_ELEMENTS_MSGINTERCEPTMSGS_H


