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

#include "sd_mintercept.h"
#include <elements/sd_log.h>
#include <elements/metatype.h>
#include <e32debug.h>
#include <elements/interfacetable.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenMIntC, "ElemSvrDenMIntC");
#endif

using namespace Messages;
using namespace Meta;
using namespace NetInterfaces;
using namespace Elements;
using namespace Den;

_LIT(KMessageInterceptPanic, "message intercept panic");
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
_LIT(KInitalisationTimePanic, "Panic during initialisation of message intercept");
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE

// Constants
//----------
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
const TUint KMaxExpectedRegisteredNodes = 32;
const TUint KMaxExpectedPatterns = 32;
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE

#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
// Helper function for comparing two TMessageId
//---------------------------------------------
TInt CompareTMessageId(const TNodeSignal::TMessageId& aMsg1, const TNodeSignal::TMessageId& aMsg2)
	{
    // return 0 is the objects are equal
    // return negative value if first is less than second
    // return positive if first is greater than second
	if (aMsg1.Realm()==aMsg2.Realm())
    	{
    	return aMsg1.MessageId() - aMsg2.MessageId();
    	}
    return aMsg1.Realm() - aMsg2.Realm();
	}
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE

// AFallibleTestControl (control interface)
//-----------------------------------------
void AFallibleTestControl::InitL(const RArray<TNodeSignal::TMessageId>& aArray)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	//copying the memory, not the array as its lifetime is temporary
	iFallibleMessages.Reset();
	TLinearOrder<TNodeSignal::TMessageId> order(CompareTMessageId);
	for (TInt i=0; i<aArray.Count(); i++)
		{
		const TNodeSignal::TMessageId& msg = aArray[i];
		// not allowing duplicate, leave with KErrAlreadyExists if so
		iFallibleMessages.InsertInOrderL(msg, order);
		}
#else
	// Fixing unused local variable warnings.
	(void)aArray;

	User::Leave(KErrNotSupported);
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	};

// CMessageInterceptRegister
//--------------------------
CMessageInterceptRegister::CMessageInterceptRegister()
:	TIfStaticFetcherNearestInHierarchy(this),
	iPatternList(),
	iRegisteredNodeTable(),
	iState(EResetState)
	{}

CMessageInterceptRegister* CMessageInterceptRegister::SetGlobal(CMessageInterceptRegister* aContext)
	{
	CMessageInterceptRegister* previous = reinterpret_cast<CMessageInterceptRegister*>(Dll::Tls());
	__ASSERT_DEBUG(!previous || !aContext, User::Panic(KSpecAssert_ElemSvrDenMIntC, 1));
	Dll::SetTls(aContext);
	return previous;
	}

//Only to be used by CCommonWorkerThread
CMessageInterceptRegister& CMessageInterceptRegister::GetGlobal()
	{
	CMessageInterceptRegister* context = reinterpret_cast<CMessageInterceptRegister*>(Dll::Tls());
	__ASSERT_DEBUG(context, User::Panic(KSpecAssert_ElemSvrDenMIntC, 2));
	return *context;
	}

TInt CMessageInterceptRegister::MatchIncomingEvent(const TEventSummaryTriple& aEvent, TAction& aAction)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// Fetch the current expression (from the current pattern) that we are waiting on next
	const TEventExpression& expression = iPatternList.CurrentPattern().CurrentExpression();

	// First need to look up sender and receiver in the node register
	// as both need to have been registered to make anything of the incoming event
	TNodeSpecifier senderNode;
	TNodeSpecifier receiverNode;

	TInt errorTx = FetchRegisteredNodeInfo(aEvent.SenderId(), senderNode);
	TInt errorRx = FetchRegisteredNodeInfo(aEvent.ReceiverId(), receiverNode);

	TInt matchResult;

	if(errorTx == KErrNone && errorRx == KErrNone)
		{
		// Create full event info (incoming event info + provider info fetched from node register)
		TEventInfo fullEventInfo(senderNode, aEvent.Message(), receiverNode);

		// Compare against current expression we are matching against and return any action if a match
		matchResult = CompareEventWithExpression(fullEventInfo, expression);

		// If we matched then advance the list of expressions
		if(matchResult == KErrNone)
			{
			iPatternList.CurrentPattern().Expressions().Advance();

			// Advance the pattern itself if necessary
			if(iPatternList.CurrentPattern().IsComplete())
				{
				aAction = iPatternList.CurrentPattern().Action();
				iPatternList.Advance();
				return KErrNone;
				}
			else
				return KErrNotFound;
			}
		else
			return KErrNotFound;
		}
	else
		{
		return KErrNotFound;
		}
#else
	// Fixing unused local variable warnings.
	(void)aEvent;
	(void)aAction;

	return KErrNotSupported;
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}


TInt CMessageInterceptRegister::CompareEventWithExpression(
	const TEventInfo& aEvent,
	const TEventExpression& aExpression)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// Compare MESSAGE type first (quickest test and more often than not only a single test needed)
	TBool msgMatch = aExpression.MessageExpression().Compare(aEvent.Message());

	// If we are already not matched then no use continuing
	if(!msgMatch)
		{
		return KErrNotFound;
		}
	else
		{
		// Now compare the SENDER properties against what we are expecting
		TBool senderMatch = aExpression.SenderExpression().Compare(aEvent.Sender());

		// If we aren't matched then no use continuing
		if(!senderMatch)
			{
			return KErrNotFound;
			}
		else
			{
			// Firstly do we need to update tag in the node register
			if(aExpression.SenderExpression().SetTagOnMatch())
				{
				iRegisteredNodeTable.SetNodeTag(aEvent.Sender().Id(), aExpression.SenderExpression().TagToSet());
				}

			// Now compare the RECEIVER
			TBool receiverMatch = aExpression.ReceiverExpression().Compare(aEvent.Receiver());

			// If we aren't matched then no use continuing
			if(!receiverMatch)
				{
				return KErrNotFound;
				}
			else
				{
				// Firstly do we need to update tag in the node register
				if(aExpression.ReceiverExpression().SetTagOnMatch())
					{
					iRegisteredNodeTable.SetNodeTag(aEvent.Receiver().Id(), aExpression.ReceiverExpression().TagToSet());
					}

				// ### COMPLETE MATCH ###
				// We have every element so a full match
				return KErrNone;
				}
			}
		}
#else
	// Fixing unused local variable warnings.
	(void)aEvent;
	(void)aExpression;

	return KErrNotSupported;
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

CMessageInterceptRegister* CMessageInterceptRegister::NewL()
	{
	CMessageInterceptRegister* self = new(ELeave) CMessageInterceptRegister();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CMessageInterceptRegister::ConstructL()
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// Create the worker lock used to synchronise access to the message intercept register
	// We absolutely need it so out of here if we can't create it
	TInt error = iSynchronisedUseLock.CreateLocal();
	User::LeaveIfError(error);

	// Resize the node register as we need it
	iRegisteredNodeTable.ReserveL(KMaxExpectedRegisteredNodes);

	// Need to make sure we reserve space for at least as many patterns to register
	// Don't want to be allocating memory during operation
	iPatternList.ReserveL(KMaxExpectedPatterns);
	iPatternList.SetOwner(this);
#else
	User::Leave(KErrNotSupported);
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

/** Look up a previously registered node */
TInt CMessageInterceptRegister::FetchRegisteredNodeInfo(const TNodeId& aNodeId, TNodeSpecifier& aNode)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	TInt index = iRegisteredNodeTable.FindNode(aNodeId);
	if (index < 0)
		{
		return index;
		}
	else
		{
		aNode = iRegisteredNodeTable[index];
		return KErrNone;
		}
#else
	// Fixing unused local variable warnings.
	(void)aNodeId;
	(void)aNode;

	return KErrNotSupported;
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

// Destructor
CMessageInterceptRegister::~CMessageInterceptRegister()
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	iRegisteredNodeTable.Reset();
	iPatternList.ResetAndDestroy();
	iSynchronisedUseLock.Close();
	iFallibleMessages.Close();
#endif//#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

void CMessageInterceptRegister::ResetStatusVariables()
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	iState = EResetState;
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

void CMessageInterceptRegister::SetComplete(TAny* aFrom)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	(void)(aFrom != NULL); //keep compiler happy
	__ASSERT_DEBUG(static_cast<TAny*>(aFrom) == static_cast<TAny*>(&iPatternList), User::Panic(KSpecAssert_ElemSvrDenMIntC, 3));
	iState = EPassedState;
	COMMONLOG((TWorkerThreadPublicInfo::EMainThread, KECommonServerTag, _L8("Register set complete:")));
#else
	// Fixing unused local variable warnings.
	(void)aFrom;
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

#ifdef _DEBUG
void CMessageInterceptRegister::PrintAllFallibleMessages()
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	for (TInt i = 0; i < iFallibleMessages.Count(); i++)
		{
		RDebug::Print( _L("Fallible Message: %d, Id = %d, Realm = %d\n"), i+1, iFallibleMessages[i].MessageId(),
						iFallibleMessages[i].Realm());
		}
#endif //#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}
#endif // _DEBUG

void CMessageInterceptRegister::RelinguishAccessOperation(TAny* aPtr)
	{
	CMessageInterceptRegister *obj = reinterpret_cast<CMessageInterceptRegister*> (aPtr);
	__ASSERT_DEBUG(obj, User::Panic(KSpecAssert_ElemSvrDenMIntC, 4));
	obj->RelinquishAccess();
	}

// EXPORTED

EXPORT_C NetInterfaces::TInterfaceControl& CMessageInterceptRegister::GetInterfaceControl()
	{
	CMessageInterceptRegister* context = reinterpret_cast<CMessageInterceptRegister*>(Dll::Tls());
	__ASSERT_DEBUG(context, User::Panic(KSpecAssert_ElemSvrDenMIntC, 5));
	return *context;
	}

// Enable the register
EXPORT_C TInt CMessageInterceptRegister::Enable()
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// ### SYNCHRONISE ACCESS ###
	WaitForAccess();

	// Must reset before re-enabling if already been completed
	__ASSERT_DEBUG(iState == EResetState, User::Panic(KSpecAssert_ElemSvrDenMIntC, 6));
	iState = ERunningState;
	COMMONLOG((TWorkerThreadPublicInfo::EMainThread, KECommonServerTag, _L8("Enabled:")));
	iMode = EPatternMatch;

	// ### SYNCHRONISE ACCESS ###
	RelinquishAccess();

	return KErrNone;
#else
	return KErrNotSupported;
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

// MPatternMatchControl (control interface)
//-----------------------------------------

// Reset the message intercept register (but retain the register of nodes, including any tags applied)
EXPORT_C TInt CMessageInterceptRegister::SoftReset()
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// ### SYNCHRONISE ACCESS ###
	WaitForAccess();

	// Disable the register (stop it intercepting if it is currently)
	Disable();

	// Clear the pattern table
	ResetStatusVariables();
	iPatternList.ResetForReuse(ETrue);
	COMMONLOG((TWorkerThreadPublicInfo::EMainThread, KECommonServerTag, _L8("Soft reset:")));

	// ### SYNCHRONISE ACCESS ###
	RelinquishAccess();

	return KErrNone;
#else
	return KErrNotSupported;
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

// Reset the whole register (patterns and node instances)
EXPORT_C TInt CMessageInterceptRegister::HardReset()
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// ### SYNCHRONISE ACCESS ###
	WaitForAccess();

	// Disable the register
	Disable();

	// Clear both registers
	ResetStatusVariables();
	iPatternList.ResetForReuse(ETrue);
	iRegisteredNodeTable.Reset();
	COMMONLOG((TWorkerThreadPublicInfo::EMainThread, KECommonServerTag, _L8("Hard reset:")));

	// ### SYNCHRONISE ACCESS ###
	RelinquishAccess();

	return KErrNone;
#else
	return KErrNotSupported;
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

// Allow the client to query the completion of the message pattern matching
EXPORT_C TInt CMessageInterceptRegister::QueryComplete()
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// ### SYNCHRONISE ACCESS ###
	WaitForAccess();

	TInt state = static_cast<TInt>(iState);
	//COMMONLOG(_L8("Query complete: status:%d"), state);

	// ### SYNCHRONISE ACCESS ###
	RelinquishAccess();

	return state;
#else
	return KErrNotSupported;
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

EXPORT_C TInt CMessageInterceptRegister::AppendPattern(const CPattern* aPatternToAppend)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// ### SYNCHRONISE ACCESS ###
	WaitForAccess();

	// Take ownership by appending to our list of patterns
	TRAPD(error, iPatternList.AppendL(aPatternToAppend));
	//COMMONLOG(_L8("Append pattern: err:%d"), error);

	// ### SYNCHRONISE ACCESS ###
	RelinquishAccess();

	return error;
#else
	// Fixing unused local variable warnings.
	(void)aPatternToAppend;

	return KErrNotSupported;
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

// Append a new pattern to the pattern matching schedule
EXPORT_C TInt CMessageInterceptRegister::AppendExpression(const TEventExpression& aExpression)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	TInt error = KErrNone;

	// ### SYNCHRONISE ACCESS ###
	WaitForAccess();

	// Do we need a new temporary holding list
	if(!iExpressionHoldingList)
		{
		iExpressionHoldingList = new RExpressionList();
		if(!iExpressionHoldingList)
			{
			User::Panic(KInitalisationTimePanic, KErrNoMemory);
			}

		TRAPD(error, iExpressionHoldingList->ReserveDefaultSpaceL());
		if(error != KErrNone)
			{
			User::Panic(KInitalisationTimePanic, error);
			}
		}

	// Append our expression to the list
	error = iExpressionHoldingList->Append(aExpression);
	if(error != KErrNone)
		{
		User::Panic(KInitalisationTimePanic, error);
		}

	//COMMONLOG(_L8("Append expression: err:%d"), error);

	// ### SYNCHRONISE ACCESS ###
	RelinquishAccess();

	return KErrNone;
#else
	// Fixing unused local variable warnings.
	(void)aExpression;

	return KErrNotSupported;
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

EXPORT_C TInt CMessageInterceptRegister::AppendAction(const TAction& aAction)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// ### SYNCHRONISE ACCESS ###
	WaitForAccess();

	// Must have a list of expressions that we are terminating with this action
	__ASSERT_DEBUG(iExpressionHoldingList, User::Panic(KSpecAssert_ElemSvrDenMIntC, 7));
	__ASSERT_DEBUG(iExpressionHoldingList->Count() > 0, User::Panic(KSpecAssert_ElemSvrDenMIntC, 8));
	TInt error;
	CPattern* newPattern = NULL;
	TRAP(error, newPattern = CPattern::NewL(*iExpressionHoldingList, aAction));
	if(error == KErrNone)
		{
		TRAP(error, iPatternList.AppendL(newPattern));
		}

	//COMMONLOG(_L8("Appended action:0x%x ErrorCode:0x%x err:%d"), aAction.Action(), aAction.Error(), error);

	// Refresh the holding store for new expressions
	delete iExpressionHoldingList;
	iExpressionHoldingList = NULL;

	// ### SYNCHRONISE ACCESS ###
	RelinquishAccess();

	return error;
#else
	// Fixing unused local variable warnings.
	(void)aAction;

	return KErrNotSupported;
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

// Enable the register using the counter
EXPORT_C TInt CMessageInterceptRegister::Enable(const TInt aMsgCount)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	// ### SYNCHRONISE ACCESS ###
	WaitForAccess();

	Disable();
	ResetStatusVariables();

	// Must reset before re-enabling if already been completed
	__ASSERT_DEBUG(iState == EResetState, User::Panic(KSpecAssert_ElemSvrDenMIntC, 9));
	iState = ERunningState;
	iCount = aMsgCount;
	iErrInjectedFlag = EFalse;

	COMMONLOG((TWorkerThreadPublicInfo::EMainThread, KECommonServerTag, _L8("Enabled with counter:")));

	iMode = EFallibleTest;

	// ### SYNCHRONISE ACCESS ###
	RelinquishAccess();

	return KErrNone;
#else
	// Fixing unused local variable warnings.
	(void)aMsgCount;

	return KErrNotSupported;
#endif //__ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

// Check
EXPORT_C TBool CMessageInterceptRegister::CheckFinished()
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	TBool Result = EFalse;

	// ### SYNCHRONISE ACCESS ###
	WaitForAccess();

	if (iCount > 0)
		{
		Result = ETrue;
		}

	// ### SYNCHRONISE ACCESS ###
	RelinquishAccess();

	return Result;
#else
	return EFalse;
#endif //__ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

// MNodeIntercept (interface for nodes in to the register during opertion)
//------------------------------------------------------------------------

// Register a newly constructed node
EXPORT_C TInt CMessageInterceptRegister::RegisterNewNode(const TNodeSpecifier& aNodeToRegister)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	TInt error = KErrNone;

	// Cheaply return if not running
	if(!IsRunning())
		return KErrNone;

	// ### SYNCHRONISE ACCESS ###
	WaitForAccess();

	// Yes we need to check again as we were not under the protection of the register's mutex
	// when we decided not to cheaply return above
	if(IsRunning())
		{
		// Something wrong if we already exist in the table
		__ASSERT_DEBUG(iRegisteredNodeTable.FindNode(aNodeToRegister.Id()) < 0, User::Panic(KSpecAssert_ElemSvrDenMIntC, 10));
		error = iRegisteredNodeTable.InsertNode(aNodeToRegister);

		//COMMONLOG(_L8("Registered node: 0x%x err:%d"), aNodeToRegister.Id().Ptr(), error);
		}

	// ### SYNCHRONISE ACCESS ###
	RelinquishAccess();

	return error;
#else
	// Fixing unused local variable warnings.
	(void)aNodeToRegister;

	return KErrNotSupported;
#endif //__ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

// Perform message interception on an incoming message
EXPORT_C void CMessageInterceptRegister::QueryMessageInterceptL(const TEventSummaryTriple& aIncomingEvent)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	TAction resultingAction;

	// Cheaply return if not running
	if(!IsRunning())
		return;

	switch (iMode)
		{
		case EFallibleTest:
			{
			// Perform message interception on an incoming message for testing all fallible messages

			// ### SYNCHRONISE ACCESS ###
			WaitForAccess();
			CleanupStack::PushL(TCleanupItem(RelinguishAccessOperation, this));

			if (iErrInjectedFlag)
				{
				CleanupStack::PopAndDestroy();  // RelinquishAccess() is called
				return; // Ignore error propagation messages
				}

			// Ignore messages already tested
			if (iCount > 0)
				{
				--iCount;
				}

			if (iCount == 0)
				{
				TLinearOrder<TNodeSignal::TMessageId> order(CompareTMessageId);
				if (iFallibleMessages.FindInOrder(aIncomingEvent.Message().MessageId(), order) != KErrNotFound)
					{
					iErrInjectedFlag = ETrue;

					// TBC Error configured by client
					User::Leave(KErrNoMemory);
					}
				else
					{
					// set back the counter which is used by CheckFinished
					// this tells that the last msg is not a fallible one
					iCount = 1;
					}
				}
			CleanupStack::Pop(); // simply pop the TCleanupItem
			// ### SYNCHRONISE ACCESS ###
			RelinquishAccess();
			}
		break;

		case EPatternMatch:
			{
			TInt error = KErrNotFound;

			// ### SYNCHRONISE ACCESS ###
			WaitForAccess();

				// We must be running at least
			// Yes we need to check again as we were not under the protection of the register's mutex
			// when we decided not to cheaply return above
			if(IsRunning())
				{
				// Query if the new event matches the event we are waiting for
				// Need to peek inside the sender as NodePtr will fail as sender not necessarily in it own thread
				//COMMONLOG(_L8("Event: tx:0x%x msg:%d rx:0x%x err:%d"), aIncomingEvent.SenderId().Ptr(), aIncomingEvent.Message().MessageId().MessageId(), aIncomingEvent.ReceiverId().Ptr(), error);
				error = MatchIncomingEvent(aIncomingEvent, resultingAction);
				}

			// ### SYNCHRONISE ACCESS ###
			RelinquishAccess();

			// If have found Cause action specified by querying the register or return as expected
			if(error == KErrNone)
				{
				//COMMONLOG(_L8("Executing action:0x%x ErrorCode:0x%x"), resultingAction.Action(), resultingAction.Error());
				resultingAction.ExecuteActionL();
				}
			}
		break;

		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemSvrDenMIntC, 11));
			break;
	}
#else
	// Fixing unused local variable warnings.
	(void)aIncomingEvent;

	User::Leave(KErrNotSupported);
#endif //__ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
}


// RExpressionList
//----------------
void RExpressionList::Advance()
	{
	// Can't advance an empty list
	__ASSERT_DEBUG(Count(), User::Panic(KSpecAssert_ElemSvrDenMIntC, 12));
	__ASSERT_DEBUG(iCurrentIndex < Count(), User::Panic(KSpecAssert_ElemSvrDenMIntC, 13));

	COMMONLOG((TWorkerThreadPublicInfo::EMainThread, KECommonServerTag, _L8("Expression list advanced:")));

	if((++iCurrentIndex) == Count())
		{
		__ASSERT_DEBUG(iOwner, User::Panic(KSpecAssert_ElemSvrDenMIntC, 14));
		iOwner->SetComplete(this);
		}
	}

void CPattern::SetComplete(TAny* aFrom)
	{
	(void)(aFrom != NULL); //keep compiler happy
	__ASSERT_DEBUG(static_cast<TAny*>(aFrom) == static_cast<TAny*>(&iExpressions), User::Panic(KSpecAssert_ElemSvrDenMIntC, 15));
	iIsComplete = ETrue;
	COMMONLOG((TWorkerThreadPublicInfo::EMainThread, KECommonServerTag, _L8("Pattern set complete:")));
	}


const TEventExpression& RExpressionList::CurrentExpression() const
	{
	// Get the current expression
	__ASSERT_DEBUG(iCurrentIndex < Count(), User::Panic(KSpecAssert_ElemSvrDenMIntC, 16));
	return this->operator[](iCurrentIndex);
	}

// TMsgExpression
//---------------
TBool TMsgExpression::Compare(const TNodeSignal& aMessage) const
	{
	// We consider the messages to be the same if they have the same type id
	// ie. they are the same type of message and we don't consider any parameters
	return (aMessage.MessageId().MessageId() == iMessageId) && (aMessage.MessageId().Realm() == iRealm);
	}

// RPatternList
//-------------
CPattern& RPatternList::CurrentPattern()
	{
	// Get the current pattern
	__ASSERT_DEBUG(iCurrentIndex < Count(), User::Panic(KSpecAssert_ElemSvrDenMIntC, 17));
	return *(this->operator[](iCurrentIndex));
	}

void RPatternList::Advance()
	{
	// Advance the list of expressions which will notify the parent register if necessary
	// Shouldn't be trying to advance an empty list
	__ASSERT_DEBUG(Count(), User::Panic(KSpecAssert_ElemSvrDenMIntC, 18));
	__ASSERT_DEBUG(iCurrentIndex < Count(), User::Panic(KSpecAssert_ElemSvrDenMIntC, 19));

	COMMONLOG((TWorkerThreadPublicInfo::EMainThread, KECommonServerTag, _L8("Pattern list advanced:")));

	if((++iCurrentIndex) == Count())
		{
		__ASSERT_DEBUG(iOwner, User::Panic(KSpecAssert_ElemSvrDenMIntC, 20));
		iOwner->SetComplete(this);
		}
	}

void RPatternList::ResetForReuse(TBool aSameOwner)
	{
	ResetAndDestroy();
	iCurrentIndex = 0;
	if(!aSameOwner)
		{
		iOwner = NULL;
		}
	}

// CPattern
//---------
EXPORT_C CPattern* CPattern::NewL(const RExpressionList& aExpressionList, const TAction& aAction)
	{
	CPattern* self = new(ELeave) CPattern(aExpressionList, aAction);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CPattern::ConstructL()
	{
	}

CPattern::~CPattern()
	{
	iExpressions.Reset();
	}

const TEventExpression& CPattern::CurrentExpression() const
	{
	return iExpressions.CurrentExpression();
	}

TInt CPattern::AppendExpression(TEventExpression& aExpression)
	{
	return iExpressions.Append(aExpression);
	}

void CPattern::InitAction(const TAction& aAction)
	{
	// Simply assign the new action and mark ourselves as now terminated
	__ASSERT_DEBUG(!iIsTerminated, User::Panic(KSpecAssert_ElemSvrDenMIntC, 21));
	SetAction(aAction);
	iIsTerminated = ETrue;
	}

TBool CPattern::IsTerminated() const
	{
	return iIsTerminated;
	}

void CPattern::SetAction(const TAction& aAction)
	{
	iAction = aAction;
	}

// RRegisteredNodeList
//--------------------
TInt RRegisteredNodeList::SetNodeTag(const TNodeId& aNodeId, TNodeTag aTag)
	{
	// Lookup the tag in the
	TInt index = FindInUnsignedKeyOrder(aNodeId);

	// We are in some sort of trouble if we couldn't find the
	if(index < 0)
		return index;
	else
		{
		// Set the tag - can't already be tagged
		__ASSERT_DEBUG(operator[](index).Tag().Length() == 0, User::Panic(KSpecAssert_ElemSvrDenMIntC, 22));
		operator[](index).SetTag(aTag);
		}

	return KErrNone;
	}

TInt RRegisteredNodeList::Compare(const TNodeSpecifier& aLHS, const TNodeSpecifier& aRHS)
	{
	// How about just being ultra sure that nothing has changed in the world of TCookie
	__ASSERT_DEBUG(aLHS.Id().Size() == 12, User::Panic(KSpecAssert_ElemSvrDenMIntC, 23));

	// Simply compare the cookies
	const TUint* LHS = (reinterpret_cast<const TUint*>(&(aLHS.Id())));
	const TUint* RHS = (reinterpret_cast<const TUint*>(&(aRHS.Id())));

	if(LHS[0] < RHS[0])
		{
		return -1;
		}
	else if(LHS[0] > RHS[0])
		{
		return 1;
		}
	else if(LHS[1] > RHS[1])
		{
		return 1;
		}
	return 0;
	}


// TNodeExpression
//----------------
TBool TNodeExpression::Compare(const TNodeSpecifier& aNode) const
	{
	TBool areEqual(EFalse);

	switch (iMatchType)
		{
		case EMatchByInstance:
			{
			// Compare only on the basis of earier applied identity (descriptor tag)
			areEqual = (iMatchTag == aNode.Tag());
			}
		break;

		case EMatchByUid:
			{
			// Compare only on the basis of UID
			areEqual = (iUid == aNode.Uid());
			}
		break;

		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemSvrDenMIntC, 24));
			break;
		}

	return areEqual;
	}

// TAction
//--------
TInt TAction::ExecuteActionL()
	{
	// Simply execute the action specified
	switch(iAction)
		{
		case ENoAction:
			return KErrNone;
			//break;

		case ELeaveAction:
			User::Leave(iError);
			break;

		case EPanicAction:
			User::Panic(KMessageInterceptPanic, iError);
			break;

		default:
			return KErrArgument;
			//break;
		}

	// For what its worth
	return KErrNone;
	}



