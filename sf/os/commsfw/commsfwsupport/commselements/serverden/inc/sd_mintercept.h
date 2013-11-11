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

#ifndef SYMBIAN_ELEMENTS_MSGINTERCEPT_H
#define SYMBIAN_ELEMENTS_MSGINTERCEPT_H

#include <elements/interfacetable.h>
#include <elements/rworkerlock.h>
#include <elements/cftransport.h>
#include <elements/sd_apiextension.h>
#include <elements/sd_minterceptmsgs.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenMIntH, "ElemSvrDenMIntH");
#endif

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
	// Matching result codes
	enum TState {EResetState, ERunningState, EStoppedState, EPassedState, EFailedState};
	// Use of Message Interceptor
	enum TMode {EPatternMatch, EFallibleTest};

	// Interfaces
	//-----------
	// Interface for test client control thru self despatching msg interface
	//No need for CExtItfMsgPluginInfo for MI since messages already registered
	class APatternMatchControl : public AIpcExtensionInterface<0,0>
		{
		public:
		    static const TInt KInterfaceId = 0x10285CB9; //Allocated 18/04/2008

		public:
			virtual TInt Enable() = 0;
			virtual TInt SoftReset() = 0;
			virtual TInt HardReset() = 0;
			virtual TInt QueryComplete() = 0;
			virtual TInt AppendPattern(const CPattern* aPatternToAppend) = 0;
			virtual TInt AppendExpression(const TEventExpression& aExpressionToAppend) = 0;
			virtual TInt AppendAction(const TAction& aExpressionToAppend) = 0;
			virtual TInt Open(TUint /*aClientId*/)
				{
				return KErrNone;
				};
			virtual void Close(TUint /*aClientId*/)
				{
				};
		};

	// Interface thru which fallible messages are tested
	//No need for CExtItfMsgPluginInfo for MI since messages already registered
	class AFallibleTestControl : public AIpcExtensionInterface<0,0>
		{
		public:
		    static const TInt KInterfaceId = 0x10285CBA; //Allocated 18/04/2008

		public:
			virtual TInt Enable(const TInt aMsgCount) = 0;
			virtual TBool CheckFinished() = 0;
			void InitL(const RArray<Messages::TNodeSignal::TMessageId>& aArray);
			virtual TInt Open(TUint /*aClientId*/)
				{
				return KErrNone;
				};
			virtual void Close(TUint /*aClientId*/)
				{
				};

		protected:
			// List of fallible messages.  TBC Lookup from a configuration file.
			RArray<Messages::TNodeSignal::TMessageId> iFallibleMessages;
		};

	// Interface thru which nodes call in to the message intercept and register with it
	//No need for CExtItfMsgPluginInfo for MI since messages already registered
	class ANodeIntercept : public AIpcExtensionInterface<0,0>
		{
		public:
		    static const TInt KInterfaceId = 0x10285CBB; //Allocated 18/04/2008

		public:
			virtual TInt RegisterNewNode(const TNodeSpecifier& aNodeToRegister) = 0;
			virtual void QueryMessageInterceptL(const TEventSummaryTriple& aIncomingEvent) = 0;
			virtual TInt Open(TUint /*aClientId*/)
				{
				return KErrNone;
				};
			virtual void Close(TUint /*aClientId*/)
				{
				};
		};

	class MCompleteNotify
		{
		public:
		    static const TInt KInterfaceId = 0x10285CBC; //Allocated 18/04/2008

		public:
			virtual void SetComplete(TAny* aFrom) = 0;
		};

	//
	//
	class RPatternList : public RPointerArray<CPattern>
		{
		public:
			RPatternList() : RPointerArray<CPattern>() {}
			CPattern& CurrentPattern();
			void Advance();
			void AppendEmptyPattern();
			void SetOwner(MCompleteNotify* aToBeNotified) {__ASSERT_DEBUG(aToBeNotified, User::Panic(KSpecAssert_ElemSvrDenMIntH, 1)); iOwner = aToBeNotified;}
			void ResetForReuse(TBool aSameOwner);

		private:
			TUint iCurrentIndex;
			MCompleteNotify* iOwner;
		};


	class RExpressionList : public RArray<TEventExpression>
		{
		public:
			RExpressionList() :
				RArray<TEventExpression>(), iCurrentIndex(0), iOwner(0) {}
			const TEventExpression& CurrentExpression() const;
			void Advance();
			void ReserveDefaultSpaceL()
				{
				ReserveL(KMaxExpectedExpressions);
				}

			void SetOwner(MCompleteNotify* aToBeNotified) {__ASSERT_DEBUG(aToBeNotified, User::Panic(KSpecAssert_ElemSvrDenMIntH, 2)); iOwner = aToBeNotified;}

		private:
			static const TUint KMaxExpectedExpressions = 16;
			TUint iCurrentIndex;

			MCompleteNotify* iOwner;
		};

	class TNodeSpecifier
		{
		public:
			TNodeSpecifier(TUid aUid, const Messages::TNodeId& aNode) :
				iUid(aUid), iNodeId(aNode) {}

			TNodeSpecifier(const Messages::TNodeId& aNodeId) :
				iUid(TUid::Uid(0)), iNodeId(aNodeId) {}

			TNodeSpecifier() : iUid(TUid::Uid(0)) {}
			const Messages::TNodeId& Id() const {return iNodeId;}
			const TNodeTag& Tag() const {return iTag;}
			const TUid Uid() const {return iUid;}

			void SetTag(TNodeTag& aTag) {iTag = aTag;}
			static TInt Compare(const TNodeSpecifier& aLHS, const TNodeSpecifier& aRHS);

		private:
			Messages::TNodeId iNodeId;
			enum TNodeType {EGeneralPurposeNode, EMeshBasedNode};
			TUid iUid;
			TNodeTag iTag;
		};

	/** Ordered list (by TNodeId) of nodes that have registered */
	class RRegisteredNodeList : public RArray<TNodeSpecifier>
		{
		public:
			TInt SetNodeTag(const Messages::TNodeId& aNodeId, TNodeTag aTag);
			RRegisteredNodeList() : RArray<TNodeSpecifier>() {}

			TInt FindNode(const Messages::TNodeId& aCookieOfNodeToFind)
				{
				return FindInOrder(TNodeSpecifier(aCookieOfNodeToFind), TLinearOrder<TNodeSpecifier>(Compare));
				}

			TInt InsertNode(const TNodeSpecifier& aNodeToInsert)
				{
				return RArray<TNodeSpecifier>::InsertInOrder(aNodeToInsert, Compare);
				}

		private:
			static TInt Compare(const TNodeSpecifier& aLHS, const TNodeSpecifier& aRHS);
		};

	class TEventSummaryTriple
		{
		public:
			TEventSummaryTriple(Messages::TNodeId aSender, Messages::TNodeSignal& aMessage, Messages::TNodeId aReceiver) :
				iSenderId(aSender), iMessage(aMessage), iReceiverId(aReceiver) {}

			Messages::TNodeSignal& Message() const {return iMessage;};
			const Messages::TNodeId& SenderId() const {return iSenderId;};
			const Messages::TNodeId& ReceiverId() const {return iReceiverId;};

		private:
			Messages::TNodeId iSenderId;
			Messages::TNodeSignal& iMessage;
			Messages::TNodeId iReceiverId;
		};

	class CPattern : public CBase, public MCompleteNotify
		{
		public:
			IMPORT_C static CPattern* NewL(const RExpressionList& aExpressionList, const TAction& aAction);
			CPattern(const RExpressionList& aList, const TAction& aAction) :
				iExpressions(aList), iAction(aAction), iIsComplete(EFalse)
				{iExpressions.SetOwner(this);}
			~CPattern();

			// Accessors
			const TAction Action () const {return iAction;};
			const RExpressionList& Expressions() const {return iExpressions;}
			RExpressionList& Expressions() {return iExpressions;}
			const TEventExpression& CurrentExpression() const;
			TBool IsTerminated() const;
			TBool IsComplete() const {return iIsComplete;}

			// Mutators
			TInt AppendExpression(TEventExpression& aExpression);
			void InitAction(const TAction& aAction);

		private:
			void ConstructL();
			void SetAction(const TAction& aAction);

			// MCompleteNotify
			void SetComplete(TAny* aFrom);

			// List of event expressions and single resulting action
			RExpressionList iExpressions;
			TAction iAction;
			TBool iIsComplete;
			TBool iIsTerminated;
		};

	/** Encapsulates full record of incoming event, as opposed to summary which is only the TCookies of the messages */
	class TEventInfo
		{
		public:
			TEventInfo(TNodeSpecifier& aSender, Messages::TNodeSignal& aMsg, TNodeSpecifier& aReceiver) :
				iSenderNode(aSender), iMessage(aMsg), iReceiverNode(aReceiver) {}

			const Messages::TNodeSignal& Message() const {return iMessage;}
			const TNodeSpecifier& Sender() const {return iSenderNode;}
			const TNodeSpecifier& Receiver() const {return iReceiverNode;}

		private:
			TNodeSpecifier& iSenderNode;
			Messages::TNodeSignal& iMessage;
			TNodeSpecifier& iReceiverNode;
		};

	//
	//CMessageInterceptRegister
	class CMessageInterceptRegister : public CBase, public NetInterfaces::TInterfaceControl,
	                                  public APatternMatchControl, public AFallibleTestControl, public ANodeIntercept, public MCompleteNotify,
                                      public ITFHIERARCHY_3(CMessageInterceptRegister, APatternMatchControl, AFallibleTestControl, ANodeIntercept)
		{
			friend class CCommonWorkerThread; //consruction/destruction/broadcast

		public:
			typedef ITFHIERARCHY_3(CMessageInterceptRegister, APatternMatchControl, AFallibleTestControl, ANodeIntercept) TIfStaticFetcherNearestInHierarchy;

		public:
			// Context
			IMPORT_C static NetInterfaces::TInterfaceControl& GetInterfaceControl();
			virtual ~CMessageInterceptRegister();

			void ReturnInterfacePtrL(APatternMatchControl*& aInterface)
				{
				aInterface = this;
				}

			void ReturnInterfacePtrL(AFallibleTestControl*& aInterface)
				{
				aInterface = this;
				}

			void ReturnInterfacePtrL(ANodeIntercept*& aInterface)
				{
				aInterface = this;
				}

		private:
			//Only to be used by CCommonWorkerThread, must stay private
			static CMessageInterceptRegister* NewL();
			static CMessageInterceptRegister& GetGlobal();
			static CMessageInterceptRegister* SetGlobal(CMessageInterceptRegister* aContext);
			void ConstructL();
			CMessageInterceptRegister();

			// MCompleteNotify
			//----------------
			void SetComplete(TAny* aFrom);

			// MPatternMatchControl
			//---------------------
			IMPORT_C TInt Enable();
			IMPORT_C TInt SoftReset();
			IMPORT_C TInt HardReset();
			IMPORT_C TInt QueryComplete();

			IMPORT_C TInt AppendPattern(const CPattern* aPatternToAppend);
			IMPORT_C TInt AppendExpression(const TEventExpression& aExpressionToAppend);
			IMPORT_C TInt AppendAction(const TAction& aExpressionToAppend);

			// AFallibleTestControl
			//---------------------
			IMPORT_C TInt Enable(const TInt aMsgCount);
			IMPORT_C TBool CheckFinished();

			// MNodeIntercept
			//---------------
			IMPORT_C TInt RegisterNewNode(const TNodeSpecifier& aNodeToRegister);
			IMPORT_C void QueryMessageInterceptL(const TEventSummaryTriple& aIncomingEvent);

			TInt MatchIncomingEvent(const TEventSummaryTriple& aEvent, TAction& aAction);
			TInt FetchRegisteredNodeInfo(const Messages::TNodeId& aNodeId, TNodeSpecifier& aNode);
			TInt CompareEventWithExpression(
				const TEventInfo& aEventInfo,
				const TEventExpression& aEventExpression);

			void ResetStatusVariables();
			void Disable() {iState = EStoppedState;}
			// Caution: This routine used outside the protection of mutex that otherwise
			// manages concurrent access of the whole message register
			TBool IsRunning() const {return (iState == ERunningState);}
			void WaitForAccess() {iSynchronisedUseLock.Wait();}
			void RelinquishAccess() {iSynchronisedUseLock.Signal();}

			static void RelinguishAccessOperation(TAny* aPtr);

			// Print out the array of fallible messages
			void PrintAllFallibleMessages();

			RPatternList iPatternList;
			RExpressionList* iExpressionHoldingList;
			RRegisteredNodeList iRegisteredNodeTable;
			RWorkerLock iSynchronisedUseLock;

			TState iState;
			TMode iMode;

			TInt iCount;
			TBool iErrInjectedFlag;
		};
	} // Den

#endif //SYMBIAN_ELEMENTS_MSGINTERCEPT_H


