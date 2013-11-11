// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <cflog.h>
#include <rsshared.h>
#include <commschan.h>
#include <cfshared.h>
#include <cfforwardmsg.h>
using namespace CommsFW;
#include "bm_defs.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemRootServerbm, "ElemRootServerbm");
#endif





CPersistentSubModuleAddress::CPersistentSubModuleAddress(const TCFSubModuleAddress& aSubAddr,CPersistentSubModuleAddressCon* aContainerRef) :
	iSubAddr(aSubAddr),
	iContainerRef(aContainerRef),
	iAccessCount(1)
// aContainerRef - the container that will store this object
	{
	}



CPersistentSubModuleAddress* CPersistentSubModuleAddress::NewL(const TCFSubModuleAddress& aSubAddr, CPersistentSubModuleAddressCon* aContainerRef)
	{
	CPersistentSubModuleAddress* self = new (ELeave) CPersistentSubModuleAddress(aSubAddr,aContainerRef);
	return self;
	}


void CPersistentSubModuleAddress::Close(TInt aIndex)
// close a reference to this persistent sub module address
// aIndex - if specified, Close will assume this element is at the specified index in the container
//  otherwise we need to find ourselves in the container.
	{
	--iAccessCount;
	__ASSERT_DEBUG(iAccessCount >= 0, User::Panic(KSpecAssert_ElemRootServerbm, 1));
	if(iAccessCount == 0)
		{
		// remove
		if (aIndex >= 0)
			{
			// caller has already done work of finding which index
			iContainerRef->iPersistentModuleAddresses.Remove(aIndex);
			}
		else
			{
			TInt sessIdx = iContainerRef->iPersistentModuleAddresses.Find(this);
			if(sessIdx >= 0)
				{
				iContainerRef->iPersistentModuleAddresses.Remove(sessIdx);
				}
			}
		delete this;
		}
	}



void CloseObject(TAny* aObject)
/**
 * This method simply closes an object from the cleanup stack.  The object must contain
 * a Close method.
 *
 * @param	aObject - a TAny pointer to the object to close.
 * @return	None
 */
	{
	((CPersistentSubModuleAddress*)aObject)->Close();
	}



CPersistentSubModuleAddressCon* CPersistentSubModuleAddressCon::NewL()
	{
	CPersistentSubModuleAddressCon* self = new (ELeave) CPersistentSubModuleAddressCon();
	return self;
	}

CPersistentSubModuleAddressCon::~CPersistentSubModuleAddressCon()
	{
	__ASSERT_DEBUG(iPersistentModuleAddresses.Count() == 0, User::Panic(KSpecAssert_ElemRootServerbm, 2));
	iPersistentModuleAddresses.ResetAndDestroy();
	}


TCFSubModuleAddress& CPersistentSubModuleAddressCon::SetOrRetrieveNameLC(const TCFSubModuleAddress& aRef)
// aRef - module address to add and return a reference to
// pushes on the cleanup stack an operation to release this ref count object
// returns - ref to the persistent module address created. Increments count on this module address.
	{
	// find name
	TInt countBack = iPersistentModuleAddresses.Count();

	while (countBack > 0)
		{
		--countBack;

		CPersistentSubModuleAddress* modAddrRef = iPersistentModuleAddresses[countBack];

		if (modAddrRef->SubAddrRef()==aRef)
			{
			modAddrRef->Open();
			CleanupStack::PushL(TCleanupItem(CloseObject, modAddrRef));
			return modAddrRef->SubAddrRef();
			}
		}


	//not found so create copy of addr and add
	CPersistentSubModuleAddress* newModuleAddr;
	newModuleAddr = CPersistentSubModuleAddress::NewL(aRef,this);
	CleanupStack::PushL(newModuleAddr);
#ifdef _DEBUG
	TInt ret = iPersistentModuleAddresses.Append(newModuleAddr);
	__ASSERT_DEBUG(ret!=KErrAlreadyExists, User::Panic(KSpecAssert_ElemRootServerbm, 3));
	if (ret!=KErrNone)
		{
		User::Leave(ret);
		}
#else
	iPersistentModuleAddresses.AppendL(newModuleAddr);
#endif
	CleanupStack::Pop(newModuleAddr);
	CleanupStack::PushL(TCleanupItem(CloseObject, newModuleAddr));
	return newModuleAddr->SubAddrRef();

	}



void CPersistentSubModuleAddressCon::Remove(const TCFSubModuleAddress& aRef)
// panics in debug if not found
	{

	//find name
	TInt countBackIdx = iPersistentModuleAddresses.Count();

	while (countBackIdx > 0)
		{
		--countBackIdx;
		if (iPersistentModuleAddresses[countBackIdx]->SubAddrRef()==aRef)
			{
			iPersistentModuleAddresses[countBackIdx]->Close(countBackIdx);
			return;
			}
		}
	__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemRootServerbm, 4)); // we should never get a request to remove a non-existent element. In release, under releasing is tolerable (leak), over releasing might panic elsewhere (speculative)
	}




CBindManager* CBindManager::NewL(MRootServerNotify& aRootServerNotifier)
/** Creates new CBindManager object.
@param aRootServerNotifier Callback object when an unknown message is received
@return Pointer to the CBindManager object
@leave KErrNoMemory
*/
	{
    __CFLOG(KLogSubSysRS, KLogCode, _L8("CBindManager::NewL"));
    CBindManager* pBm = new (ELeave) CBindManager();
    pBm->iNotify = &aRootServerNotifier;

	CleanupStack::PushL(pBm);
	pBm->ConstructL();
	CleanupStack::Pop(pBm);

    return pBm;
	}


void CBindManager::ConstructL()
	{
    iModuleNameStor = CPersistentSubModuleAddressCon::NewL();
	}


CBindManager::~CBindManager()
/** Destructor of CBindManager object.
*/
	{
    TDblQueIter<CModule> moduleIter(iModuleList);
    CModule *pModule;
    while((pModule = moduleIter++) != NULL)
		{
		delete pModule;
		}

    TDblQueIter<CJob> jobIter(iJobList);
    CJob *pJob;
    while((pJob = jobIter++) != NULL)
		{
		delete pJob;
		}
	delete iModuleNameStor;

    TDblQueIter<CBindInfo> bindIter(iBindList);
    CBindInfo *pBind;
    while((pBind = bindIter++) != NULL)
		{
		delete pBind;
		}
	}

void CBindManager::CreateModuleL(TRequestStatus& aStatus,
								 const TCFModuleNameF& aModule,
								 const RCFChannel::TMsgQueues& aInputQueues,
								 const RCFChannel::TMsgQueues& aOutputQueues)
/** Called by rootserver to inform the bindmanager of a new module and the queues
to be used for communicating with it. The bindmanager will send a Discovery
message to the module and will not complete the request until a valid
DiscoveryResp is received or timeout occurs.
@param aStatus status which will be completed when finished
@param aModule name of module to create
@param aInputQueues queues on which messages should sent to the module
@param aOutputQueues queues on which messages will be received from the module
@leave KErrNoMemory
@leave ERSModuleAlreadyExist
*/
	{
	// first check this module not already in my list
	TRAPD(ret, GetModulePtrL(aModule));
	if (ret != KErrRSModuleUnknown)
		{
	 	__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CBindManager::CreateModuleL - %S already exists"), &aModule);
		User::Leave(KErrRSModuleAlreadyExist);
		}

	// ok, so add it to my list
	CModule* pModule = CModule::NewL(this, aModule, iTransactionId,
						            aInputQueues, aOutputQueues);
	CleanupStack::PushL(pModule);

	// now start a discovery request
	TUint taskId;
	pModule->DoDiscoverL(taskId);

	// and add the task to a job
	CJob *pJob = CJob::NewL(aStatus, this);
	CleanupStack::PushL(pJob);
	pJob->AddTaskL(taskId, pModule->Name());
	CleanupStack::Pop(pJob);

	// now put module in list
	CleanupStack::Pop(pModule);
	iModuleList.AddLast(*pModule);

	// put job in main list and start its timer
	iJobList.AddLast(*pJob);
	pJob->StartTimeout(KJobTimeoutMicroseconds);
	}

void CBindManager::SendL( const TCFModuleNameF& aModule, const RMessage2& aMessage )
	{
	CModule* pModule = GetModulePtrL( aModule );
	TCFForwardMsg forwardMsg(aMessage);
	pModule->SendL( forwardMsg );
	}

void CBindManager::UnbindAllModuleL(TRequestStatus& aStatus,
								  const TCFModuleNameF& aModule,
								  const TBool aModuleIsDead,
								  CJob*& pJob)
/** Send Unbind requests for every binding involving the specified module. If
the ModuleIsDead parameter is true then no messages will be sent to the
named target module.
@param aStatus status which will be completed when finished
@param aModule name of module to unbind
@param aModuleIsDead indicates whether the target module is dead, and therefore
should not be sent any messages
@param pJob var parameter which will be set to point to a new job if one is
created
@leave KErrNoMemory
@leave ERSModuleUnknown
@leave ERSSubModuleUnknown
@leave ERSBindingInProgress
*/
	{
	// first validate that at least one binding exists and
	// all bindings can be unbound
	TBool bindingFound = EFalse;
    TDblQueIter<CBindInfo> bindIter(iBindList);
    CBindInfo *pBind;
    while((pBind = bindIter++) != NULL)
		{
		if (pBind->SubModuleAddr1().Module() == aModule ||
			pBind->SubModuleAddr2().Module() == aModule)
			{
			bindingFound = ETrue;

			if (pBind->State1() == CBindManager::EBindInProgress ||
			    pBind->State2() == CBindManager::EBindInProgress)
				{
				User::Leave(KErrRSBindingInProgress);
				}
			}
		}

	if (!bindingFound)
		{
		// no tasks to perform so complete request and return immediately
		TRequestStatus *status = &aStatus;
		User::RequestComplete(status, KErrNone);
		pJob = 0;
		return;
		}

	// create a new job
	pJob = CJob::NewL(aStatus, this);
	CleanupStack::PushL(pJob);

	// now remove the bindings
	TBool tasksAdded = EFalse;
    bindIter.SetToFirst();
    while((pBind = bindIter++) != NULL)
		{
		if (pBind->SubModuleAddr1().Module() != aModule &&
			pBind->SubModuleAddr2().Module() != aModule)
			{
			// this binding not relevant
			continue;
			}

		// Bugfix: changed to ignore any binding to/from the module in question
		// if its state is >= EUnbindInProgress. Reasoning - this means that an unbind
		// has already been sent. Sending another can do no further good, and means we
		// have to cope with (legitimately) receiving multiple unbind responses.
		CBindManager::TBMBindState endpt1State = pBind->State1();
		CBindManager::TBMBindState endpt2State = pBind->State2();
		if(endpt1State == EUnbindInProgress || endpt2State == EUnbindInProgress)
			{
			continue;
			}
		TBool unbindModule1 = ETrue;
		if (endpt1State >= CBindManager::EUnbindInProgress ||
			(pBind->SubModuleAddr1().Module() == aModule && aModuleIsDead))
			{
			unbindModule1 = EFalse;
			pBind->SetState1(CBindManager::EUnbound);
			pBind->SetResult1(KErrNone);
			}

		TBool unbindModule2 = ETrue;
		if (endpt2State >= CBindManager::EUnbindInProgress ||
			(pBind->SubModuleAddr2().Module() == aModule && aModuleIsDead))
			{
			unbindModule2 = EFalse;
			pBind->SetState2(CBindManager::EUnbound);
			pBind->SetResult2(KErrNone);
			}

		if (!unbindModule1 && !unbindModule2)
			{
			// must be two binds within dead or not properly bound module -
			// nothing we can do but kill binding and look for next
		   __CFLOG_3(KLogSubSysRS, KLogCode, _L8("CBindManager::UnbindAllModuleL() - deleting CBindInfo(%08x) - es1=%d, es2=%d"), pBind, endpt1State, endpt2State);
			delete pBind;
			continue;
			}

		CTask* pTask1 = 0;
		TBool newTask1 = EFalse;
		CModule* pModule1 = 0;
		if (unbindModule1)
			{
			pModule1 = GetModulePtrL(pBind->SubModuleAddr1());
			const TCFSubModuleAddress& subModule1Ref1 = iModuleNameStor->SetOrRetrieveNameLC(pModule1->SubModuleNameRefL(pBind->SubModuleAddr1()));


			const TCFSubModuleAddress& subModule1Ref2 = iModuleNameStor->SetOrRetrieveNameLC(pModule1->SubModuleNameRefL(pBind->SubModuleAddr2()));


			newTask1 = pModule1->GetUnbindTaskL(
							subModule1Ref1,
							subModule1Ref2,
							aModuleIsDead,
							pTask1);
			if (newTask1)
				{
				CleanupStack::PushL(pTask1);
				}
			// coverity [leave_without_push] - existing tasks have already been allocated
			// and owned, therefore we don't need to push them onto the cleanup stack.
			pBind->SetState1(CBindManager::EUnbindInProgress);
			pJob->AddTaskL(pTask1->TaskId(), pBind->SubModuleAddr1().Module());
			tasksAdded = ETrue;
			}

		CTask* pTask2 = 0;
		TBool newTask2 = EFalse;
		CModule* pModule2 = 0;
		if (unbindModule2)
			{
			pModule2 = GetModulePtrL(pBind->SubModuleAddr2());
			//if we leave we will have this outstanding ref. Need to push onto cleanup stack until
			// we are clear of the "DoStartTaskL".

			const TCFSubModuleAddress& subModule2Ref1 = iModuleNameStor->SetOrRetrieveNameLC(pModule2->SubModuleNameRefL(pBind->SubModuleAddr1()));

			const TCFSubModuleAddress& subModule2Ref2 = iModuleNameStor->SetOrRetrieveNameLC(pModule2->SubModuleNameRefL(pBind->SubModuleAddr2()));


			newTask2 = pModule2->GetUnbindTaskL(
							subModule2Ref2,
							subModule2Ref1,
							aModuleIsDead,
							pTask2);
			if (newTask2)
				{
				CleanupStack::PushL(pTask2);
				}
			
			pBind->SetState2(CBindManager::EUnbindInProgress);
			// coverity [leave_without_push] - existing tasks have already been allocated
			// and owned, therefore we don't need to push them onto the cleanup stack.
			pJob->AddTaskL(pTask2->TaskId(), pBind->SubModuleAddr2().Module());
			tasksAdded = ETrue;

			if (newTask2)
				{
				pModule2->DoStartTaskL(pTask2);
				CleanupStack::Pop(pTask2);
				CleanupStack::Pop(2);     // moduleAddr refs from SetOrRetrieveNameLC calls
				}
			}

		if (newTask1)
			{
			__ASSERT_DEBUG(pModule1, User::Panic(KSpecAssert_ElemRootServerbm, 5));	// LINT is skeptical
			pModule1->DoStartTaskL(pTask1);
			CleanupStack::Pop(pTask1);
			CleanupStack::Pop(2);     // moduleAddr refs from SetOrRetrieveNameLC calls
			}

		} // end while

	if (tasksAdded)
		{
		iJobList.AddLast(*pJob);
		pJob->StartTimeout(KJobTimeoutMicroseconds);
		CleanupStack::Pop(pJob);
		}
	else
		{
		// no tasks to perform so complete request immediately
		TRequestStatus *status = &aStatus;
		User::RequestComplete(status, KErrNone);

		CleanupStack::PopAndDestroy(pJob);
		pJob = 0;
		}
	}


void CBindManager::ConvertRSBindType(const TRSBindType aType,
											TCFBindType& aUpperBindType,
											TCFBindType& aLowerBindType)
/** Convert a rootserver style binding type to the equivalent comms framework
type
@param aType a RootServer binding type
@param aUpperBindType comms framework bind type for upper submodule
@param aLowerBindType comms framework bind type for lower submodule
*/
	{
	switch(aType)
		{
	case EHierarchical:
		aUpperBindType = EHierarchicalUpper;
		aLowerBindType = EHierarchicalLower;
		break;
	case ECustom:
		aUpperBindType = EHorizontal;
		aLowerBindType = EHorizontal;
		break;
	default:
		aUpperBindType = EOther;
		aLowerBindType = EOther;
		break;
		}
	}

CBindInfo* CBindManager::FindBindInfo(const TCFSubModuleAddress& aUpperSubmodule,
									  const TCFSubModuleAddress& aLowerSubmodule)
/** If a bind exists between the two specified modules, then return a pointer to
it.
@param aUpperSubmodule the upper module in the bindinfo
@param aLowerSubmodule the lower module in the bindinfo
@return Ptr to a matching bindinfo or 0 is none found
*/
	{
    TDblQueIter<CBindInfo> bindIter(iBindList);
	CBindInfo* pBind = 0;
    while((pBind = bindIter++) != NULL)
		{
		// Bugfix; there's one bindinfo for two mirror-imaged requests, hence need to test both possibilities
		if((pBind->SubModuleAddr1() == aUpperSubmodule && pBind->SubModuleAddr2() == aLowerSubmodule) ||
		   (pBind->SubModuleAddr2() == aUpperSubmodule && pBind->SubModuleAddr1() == aLowerSubmodule))
			{
			break;
			}
		}
	return pBind;
	}

void CBindManager::BindSubmodulesL(TRequestStatus& aStatus,
					 const TRSBindType aType,
					 const TCFSubModuleAddress& aUpperSubmodule,
					 const TCFSubModuleAddress& aLowerSubmodule,
					 TInt aForwardQLength,
					 TInt aReverseQLength)
/** Called by rootserver to bind two submodules. If the request is valid the
bindmanager will send a Bind message to the module associated with each
submodule (which may be the same for both) and will complete the request
when a valid response is received from both, or timeout occurs.
If a cancelled bind request exists for the same modules and type, then it
is reactivated with the new aStatus.
@param aStatus status which will be completed when finished
@param aType type of binding defined by rootserver
@param aUpperSubmodule first submodule to bind - the upper module in
hierarchical bindings
@param aLowerSubmodule second submodule to bind
@param aForwardQLength xxx
@param aReverseQLength xx
@leave KErrNoMemory
@leave ERSModuleUnknown
@leave ERSSubModuleUnknown
@leave ERSModuleNotReady
@leave ERSAlreadyBound
@leave ERSBindingInProgress
*/
	{
	// check modules exist and are ready for binding
	CModule* pUpperModule = GetModulePtrL(aUpperSubmodule);
	CModule* pLowerModule = GetModulePtrL(aLowerSubmodule);
	if (pUpperModule->Initialising() ||
		pLowerModule->Initialising())
		{
		User::Leave(KErrRSModuleNotReady);
		}

	// see if binding already exists and if so can be tried again
    CBindInfo *pBind = FindBindInfo(aUpperSubmodule, aLowerSubmodule);
	TBool bindExists = (pBind != 0);
	if (bindExists)
		{
		if (pBind->State1() == CBindManager::EBound &&
			pBind->State2() == CBindManager::EBound)
			{
			User::Leave(KErrRSAlreadyBound);
			}
		if (pBind->State1() == CBindManager::EBindInProgress ||
			pBind->State2() == CBindManager::EBindInProgress)
			{
			User::Leave(KErrRSBindingInProgress);
			}
		}
	else
		{
		// create a bind object - note this also creates the channels
		pBind = CBindInfo::NewL(aType,
					pUpperModule->SubModuleNameRefL(aUpperSubmodule),
					pLowerModule->SubModuleNameRefL(aLowerSubmodule),
					aForwardQLength,aReverseQLength);
		CleanupStack::PushL(pBind);
		pBind->SetState1(CBindManager::EBindInProgress);
		pBind->SetState2(CBindManager::EBindInProgress);
		}

	TCFBindType upperBindType;
	TCFBindType lowerBindType;
	ConvertRSBindType(aType, upperBindType, lowerBindType);

	RCFChannel::TMsgQueues queues1;
	pBind->Channel1().GetMsgQueues(queues1);
	RCFChannel::TMsgQueues queues2;
	pBind->Channel2().GetMsgQueues(queues2);

	// create a job and add to the list
	CJob *pJob = CJob::NewL(aStatus, this);
	CleanupStack::PushL(pJob);

	// get modules to create tasks or return existing ones check the bind
	// state in case an identical job was cancelled but one task was
	// finished, ie the task doesn't exist any more, but we don't want to
	// send another - if it ended in error then we will send another
	CTask* pLowerTask = 0;
	TBool newLowerTask = EFalse;
	if (pBind->State2() != CBindManager::EBound)
		{
		const TCFSubModuleAddress& state2LowerSubModuleRef = iModuleNameStor->SetOrRetrieveNameLC(pLowerModule->SubModuleNameRefL(aLowerSubmodule));

		const TCFSubModuleAddress& state2UpperSubModuleRef = iModuleNameStor->SetOrRetrieveNameLC(pUpperModule->SubModuleNameRefL(aUpperSubmodule));

		newLowerTask = pLowerModule->GetBindTaskL(queues2, queues1,
					state2LowerSubModuleRef,
					state2UpperSubModuleRef,
								lowerBindType,
								pLowerTask);
		if (newLowerTask)
			{
			CleanupStack::PushL(pLowerTask);
			}
		// coverity [leave_without_push] - existing tasks have already been allocated
		// and owned, therefore we don't need to push them onto the cleanup stack.
		pJob->AddTaskL(pLowerTask->TaskId(), aLowerSubmodule.Module());
		}

	CTask* pUpperTask = 0;
	TBool newUpperTask = EFalse;
	if (pBind->State1() != CBindManager::EBound)
		{
		const TCFSubModuleAddress& state1UpperSubModuleRef = iModuleNameStor->SetOrRetrieveNameLC(pUpperModule->SubModuleNameRefL(aUpperSubmodule));

		const TCFSubModuleAddress& state1LowerSubModuleRef = iModuleNameStor->SetOrRetrieveNameLC(pLowerModule->SubModuleNameRefL(aLowerSubmodule));

		newUpperTask = pUpperModule->GetBindTaskL(queues1, queues2,
								state1UpperSubModuleRef,
								state1LowerSubModuleRef,
								upperBindType,
								pUpperTask);
		if (newUpperTask)
			{
			CleanupStack::PushL(pUpperTask);
			}
		// coverity [leave_without_push] - existing tasks have already been allocated
		// and owned, therefore we don't need to push them onto the cleanup stack.
		pJob->AddTaskL(pUpperTask->TaskId(), aUpperSubmodule.Module());
		}

	// Check that both messages can be sent before trying anything
	User::LeaveIfError(pUpperModule->ReserveSpaceInSendQueue());
	User::LeaveIfError(pLowerModule->ReserveSpaceInSendQueue());

	// now we can actually send messages
	if (newUpperTask)
		{
		pUpperModule->DoStartTaskL(pUpperTask);
		CleanupStack::Pop(pUpperTask);
		CleanupStack::Pop(2);  // upper and lower SubModuleObj from moduleAddr SetOrRetrieveNameLC calls
		}

	if (newLowerTask)
		{
		pLowerModule->DoStartTaskL(pLowerTask);
		CleanupStack::Pop(pLowerTask);
		CleanupStack::Pop(2);  // upper and lower SubModuleObj from moduleAddr SetOrRetrieveNameLC calls
		}

	CleanupStack::Pop(pJob);
	iJobList.AddLast(*pJob);
	pJob->StartTimeout(KJobTimeoutMicroseconds);

	if (!bindExists)
		{
		CleanupStack::Pop(pBind);
		iBindList.AddLast(*pBind);
		}
	}

void CBindManager::UnbindSubmodulesL(TRequestStatus& aStatus,
					   const TCFSubModuleAddress& aUpperSubmodule,
					   const TCFSubModuleAddress& aLowerSubmodule)
/** Called by rootserver to unbind two submodules. If the request is valid the
bindmanager will send an Unbind message to the module associated with each
submodule (which may be the same for both) and will complete the request
when a valid response is received from both, or timeout occurs.
@param aStatus status which will be completed when finished
@param aUpperSubmodule first submodule to unbind
@param aLowerSubmodule second submodule to unbind
@leave KErrNoMemory
@leave ERSBindingUnknown
@leave ERSModuleUnknown
@leave ERSSubModuleUnknown
@leave ERSBindingInProgress
*/
	{
	// check binding exists and is in a suitable state for unbinding
    CBindInfo *pBind = FindBindInfo(aUpperSubmodule, aLowerSubmodule);
	if (!pBind)
		{
		User::Leave(KErrRSBindingUnknown);
		}
	else if (pBind->State1() == CBindManager::EBindInProgress ||
			 pBind->State2() == CBindManager::EBindInProgress)
		{
		User::Leave(KErrRSBindingInProgress);
		}

	// check modules exist
	CModule* pUpperModule = GetModulePtrL(aUpperSubmodule);
	CModule* pLowerModule = GetModulePtrL(aLowerSubmodule);

	// create a new job and add to list
	CJob* pJob = CJob::NewL(aStatus, this);
	CleanupStack::PushL(pJob);

	// get modules to create tasks or return existing ones check the bind
	// state in case an identical job was cancelled but one task was
	// finished, ie the task doesn't exist any more, but we don't want to
	// send another - if it ended in error then we will send another
	CTask* pLowerTask = 0;
	TBool newLowerTask = EFalse;
	if (pBind->State2() != CBindManager::EUnbound)
		{
		const TCFSubModuleAddress& state2LowerSubModuleRef = iModuleNameStor->SetOrRetrieveNameLC(pLowerModule->SubModuleNameRefL(aLowerSubmodule));

		const TCFSubModuleAddress& state2UpperSubModuleRef = iModuleNameStor->SetOrRetrieveNameLC(pUpperModule->SubModuleNameRefL(aUpperSubmodule));

		newLowerTask = pLowerModule->GetUnbindTaskL(
								state2LowerSubModuleRef,
								state2UpperSubModuleRef,
								EFalse,
								pLowerTask);
		if (newLowerTask)
			{
			CleanupStack::PushL(pLowerTask);
			}
		// coverity [leave_without_push] - existing tasks have already been allocated
		// and owned, therefore we don't need to push them onto the cleanup stack.
		pBind->SetState2(CBindManager::EUnbindInProgress);
		pJob->AddTaskL(pLowerTask->TaskId(), aLowerSubmodule.Module());
		}

	CTask* pUpperTask = 0;
	TBool newUpperTask = EFalse;
	if (pBind->State1() != CBindManager::EUnbound)
		{
		const TCFSubModuleAddress& state1UpperSubModuleRef = iModuleNameStor->SetOrRetrieveNameLC(pUpperModule->SubModuleNameRefL(aUpperSubmodule));

		const TCFSubModuleAddress& state1LowerSubModuleRef = iModuleNameStor->SetOrRetrieveNameLC(pLowerModule->SubModuleNameRefL(aLowerSubmodule));
		newUpperTask = pUpperModule->GetUnbindTaskL(
								state1UpperSubModuleRef,
								state1LowerSubModuleRef,
								EFalse,
								pUpperTask);
		if (newUpperTask)
			{
			CleanupStack::PushL(pUpperTask);
			}
		// coverity [leave_without_push] - existing tasks have already been allocated
		// and owned, therefore we don't need to push them onto the cleanup stack.
		pBind->SetState1(CBindManager::EUnbindInProgress);
		pJob->AddTaskL(pUpperTask->TaskId(), aUpperSubmodule.Module());
		}

	// Check that both messages can be sent before trying anything
	User::LeaveIfError(pUpperModule->ReserveSpaceInSendQueue());
	User::LeaveIfError(pLowerModule->ReserveSpaceInSendQueue());

	// now we can actually send messages
	if (newUpperTask)
		{
		pUpperModule->DoStartTaskL(pUpperTask);
		CleanupStack::Pop(pUpperTask);
		CleanupStack::Pop(2);  // upper and lower SubModuleObj
		}

	if (newLowerTask)
		{
		pLowerModule->DoStartTaskL(pLowerTask);
		CleanupStack::Pop(pLowerTask);
		CleanupStack::Pop(2);  // upper and lower SubModuleObj
		}

	CleanupStack::Pop(pJob);
	iJobList.AddLast(*pJob);
	pJob->StartTimeout(KJobTimeoutMicroseconds);
	}



void CBindManager::UnbindAlreadyDeadL(TRequestStatus& aStatus,
						const TCFModuleNameF& aModule)
/** Called by rootserver to unbind all submodules from a specific module where
the module is known to be dead. The bindmanager will send Unbind unbind
messages for each existing binding, except when the destination is the dead
module. The request will complete when a valid response is received for each
message or timeout occurs.
@param aStatus status which will be completed when finished
@param aModule dead module to unbind
@leave KErrNoMemory
@leave ERSModuleUnknown
@leave ERSBindingInProgress
*/
	{
	CJob *pJob;
	UnbindAllModuleL(aStatus, aModule, ETrue, pJob);
	}

TInt CBindManager::EnumerateBindings(const TCFSubModuleAddress& aSubModule, TBool aRestart, TBindingInfo& aInfo)
/** Enumerates bindings by returning one at a time until there is no more.
@param aSubModule submodule or module to count bindings for
@param aRestart whether to start from the beginning
@param aInfo reference to a binding info
*/
	{
	// for each bindinfo, if a module matches then add it to aInfo
    TDblQueIter<CBindInfo> bindIter(iBindList);
    CBindInfo *pBind;

	TInt res = KErrEof;

	if(aRestart)
		{
		iEnumBindingsIdx=0;
		}

	TInt count=0;
	// while [not found] && [there is a next]
    while( (KErrNone!=res) && ((pBind = bindIter++) != NULL) )
		{
		// if one of the <module:submodule> addressess have an eact match
		// OR we don't specify a submodule and then have a match on module level only.
		if ( pBind->SubModuleAddr1() == aSubModule ||
			 pBind->SubModuleAddr2() == aSubModule ||
			 ( 0==aSubModule.SubModule().Length() &&
			   (pBind->SubModuleAddr1().Module() == aSubModule.Module() ||
			    pBind->SubModuleAddr2().Module() == aSubModule.Module())))
			{

			// If at extraction-point
			if(count==iEnumBindingsIdx)
				{
				aInfo.iType = pBind->BindType();
				aInfo.iSubModuleAddr1 = pBind->SubModuleAddr1();
				aInfo.iSubModuleAddr2 = pBind->SubModuleAddr2();
				aInfo.iSubModuleState1 = pBind->State1();
				aInfo.iSubModuleState2 = pBind->State2();
				aInfo.iSubModuleResult1 = pBind->Result1();
				aInfo.iSubModuleResult2 = pBind->Result2();
				res = KErrNone; // It was found, end loop and return KErrNone
				++iEnumBindingsIdx;
				}
			++count;
			}
		}
	return res;
	}


void CBindManager::UnbindAllAndShutDownL(TRequestStatus& aStatus,
						   const TCFModuleNameF& aModule, TCFShutdownType aType)
/** Called by rootserver to unbind all submodules from a specific module and then
send a shutdown message.
The bindmanager will send a pair of Unbind messages for each existing binding
followed by a shutdown only to the module specified. The request will complete
when a valid response is received for each message or timeout occurs.
@param aStatus status which will be completed when finished
@param aModule module to unbind and shut down
@param aType xx
@leave KErrNoMemory
@leave ERSModuleUnknown
@leave ERSModuleNotReady
@leave ERSBindingInProgress
*/
	{
	// can't do this if we have an outstanding discovery request
	CModule *pModule = GetModulePtrL(aModule);
	if (pModule->Initialising())
		{
		User::Leave(KErrRSModuleNotReady);
		}

	CJob *pJob;
	UnbindAllModuleL(aStatus, aModule, EFalse, pJob);

	// send the shutdown request
	TCFShutdownMsg shutDownMsg(aType);
	pModule->SendL(shutDownMsg);

	}

void CBindManager::ModuleEndedL(const TCFModuleNameF& aModule)
/** Called by rootserver to indicate that a module thread has exited and therefore
any information stored about it should be deleted.
@param aModule module to end
@leave KErrNoMemory
@leave ERSModuleUnknown
@leave ERSBindingExists indicates that a binding still exists for the module
and should be removed by calling UnbindAlreadyDeadL to avoid an inconsistent
state in module to which it is bound
*/
	{
    __CFLOG_1(KLogSubSysRS, KLogCode, _L8("CBindManager::ModuleEndedL - %S ended"), &aModule);

	CModule* pModule = GetModulePtrL(aModule);

	TInt leaveCode = KErrNone;
    TDblQueIter<CBindInfo> bindIter(iBindList);
    CBindInfo *pBind;
	pModule->EndAllTasks( KErrCancel );

    while((pBind = bindIter++) != NULL)
		{
		TBool firstModOfBinding = pBind->SubModuleAddr1().Module() == aModule;
		TBool secondModOfBinding = pBind->SubModuleAddr2().Module() == aModule;

		if(firstModOfBinding)
		    {
		    pBind->SetState1(CBindManager::EUnbound);
		    if(pBind->State2() >= CBindManager::EUnbindError)
		        {
		        delete pBind;
		        }
		    }
		else if(secondModOfBinding)
		    {
		    pBind->SetState2(CBindManager::EUnbound);
		    if(pBind->State1() >= CBindManager::EUnbindError)
		        {
		        delete pBind;
		        }
		    }
		}

	// if any valid and current bindings still exist then deleting the module
	// will mess up our state - so leave now
	User::LeaveIfError(leaveCode);
	delete pModule;
	}

TInt CBindManager::Cancel(TRequestStatus& aStatus)
/** Called by rootserver to cancel the request with the specified TRequestStatus.
The bindmanager will continue to process responses from the modules and
update the internal state information accordingly.
@param aStatus status to be cancelled
@return KErrNoMemory
@return ERSStatusUnknown
*/
	{
	// find job and mark it as cancelled
    TDblQueIter<CJob> jobIter(iJobList);
    CJob *pJob;
	TInt ret = KErrRSStatusUnknown;
    while((pJob = jobIter++) != NULL)
		{
		if (pJob->HasStatus(aStatus))
			{
			pJob->Cancel();
			ret = KErrNone;
			break;
			}
		}
	return ret;
	}


TInt CBindManager::EnumerateSubModules(const TCFModuleNameF& aModuleName, TInt aPosition, TCFSubModuleNameF& aSubModuleName)
/** Return a list of sub modules, starting at the point specified by aFrom
@param aModuleName The module to enumerate submodules from
@param aPosition Start writing submodules names from here
@param aSubModuleName The descriptor to write to
@return an error code if module not found
*/
	{
	CModule* module = NULL;
	// check module/submodule exists
	TRAPD(ret, module = GetModulePtrL(aModuleName);)
	if (KErrNone != ret)
		{
		return ret;
		}
	return module->EnumerateSubModules( aPosition, aSubModuleName );
	}

TInt CBindManager::NumSubModules( const TCFModuleNameF& aName, TInt& aNumSubs )
/** Return the number of submodules which exist for the specified module
@param aName module to count submodules for
@param aNumSubs number of submodules for module
@return An error code if module not found
*/
	{
	CModule* module = NULL;
	// check module/submodule exists
	TRAPD(ret, module = GetModulePtrL(aName);)
	if (KErrNone != ret)
		{
		return ret;
		}
	aNumSubs = module->NumSubModules();
	return KErrNone;
	}


CBindManager::CBindManager() :
	iModuleList(_FOFF(CModule, iLink)),
	iJobList(_FOFF(CJob, iLink)),
	iBindList(_FOFF(CBindInfo, iLink)),
	iEnumBindingsIdx(0)
/** CBindManager constructor
*/
	{
	}

CModule* CBindManager::GetModulePtrL(const TCFModuleNameF& aModule)
/** Search method to find a cmodule ptr from a module name
@param aModule name of module to find
@return a pointer to the cmodule
@leave ERSModuleUnknown
*/
	{
    TDblQueIter<CModule> moduleIter(iModuleList);
    CModule *pModule;
    while((pModule = moduleIter++) != NULL)
		{
		if (pModule->Name() == aModule)
			{
			break;
			}
		}
	if (!pModule)
		{
		User::Leave(KErrRSModuleUnknown);
		}
	return pModule;
	}


CModule* CBindManager::GetModulePtrL(const TCFSubModuleAddress& aSubModuleAddr)
/** Search method to find a cmodule ptr from a submodule address
@param aSubModuleAddr full name of sub module to find
@return a pointer to the cmodule
@leave ERSModuleUnknown
@leave ERSSubModuleUnknown
*/
	{
    CModule *pModule = GetModulePtrL(aSubModuleAddr.Module());
	if (!pModule->HasSubModule(aSubModuleAddr.SubModule()))
		{
		User::Leave(KErrRSSubModuleUnknown);
		}
	return pModule;
	}


void CBindManager::TaskCompleted(CModule &aModule, const CTask &aTask,
								 TInt aStatus)
/** Called by a cmodule when a task is completed. Bindmanager passes the
information to all jobs and bindinfos
@param aModule the cmodule which owns the completed task
@param aTask the task which has finished
@param aStatus the status returned by the completed task
*/
	{
    __CFLOG_VAR((KLogSubSysRS, KLogCode, _L8("CBindManager::TaskCompleted(%X = %S, %d, %d)"), &aModule, &aModule.Name(), aTask.TaskId(), aStatus));

	// inform all jobs
    TDblQueIter<CJob> jobIter(iJobList);
    CJob *pJob;
    while((pJob = jobIter++) != NULL)
		{
		pJob->TaskCompleted(aModule.Name(), aTask.TaskId(), aStatus);
		}

	// update any bindinfos
    TDblQueIter<CBindInfo> bindIter(iBindList);
    CBindInfo *pBind;
    while((pBind = bindIter++) != NULL)
		{
		pBind->TaskCompleted(aTask, aStatus);
		}

	// if a discoveryresp completed without error then update module state
	if (aTask.Message().Code() == TCFCommsMessage::ECodeDiscover &&
		aStatus == KErrNone)
		{
		__ASSERT_DEBUG(aModule.Initialising(), User::Panic(KBindMgrPanic, EDiscoveryRespOutOfSequence));
		aModule.SetInitialising(EFalse);
		}
	else if (aTask.Message().Code() == TCFCommsMessage::ECodeBind) // remove refs to moduleAddr. message now becomes unusable.
		{
		const TCFBindMsg& bmsg =  reinterpret_cast<const TCFBindMsg&>(aTask.Message());
		iModuleNameStor->Remove(*bmsg.SubModule1());
		iModuleNameStor->Remove(*bmsg.SubModule2());
		}
	else if (aTask.Message().Code() == TCFCommsMessage::ECodeUnbind)
		{
		const TCFUnbindMsg& ubmsg = reinterpret_cast<const TCFUnbindMsg&>(aTask.Message());
		iModuleNameStor->Remove(*ubmsg.SubModule1());
		iModuleNameStor->Remove(*ubmsg.SubModule2());
		}
	}


void CBindManager::ReceivedMessage(const TCFModuleNameF& aModule,
								   const TCFMessage& aNewMsg)
/** Called by a cmodule when a message with no associated task is received. The
bindmanager passes the message to the rootserver.
@param aModule the name of the module which received the message
@param aNewMsg the received message
*/
	{
    __CFLOG(KLogSubSysRS, KLogCode, _L8("CBindManager::ReceivedMessage"));
	iNotify->ReceivedMessage(aModule, aNewMsg);
	}


void CBindManager::JobTimedOut(const TCFModuleNameF &aModuleName, const TUint aTaskId)
/** Called by a cjob for each taskid when it times out. It informs the associated
module.
@param aModuleName the name of the module associated with the task
@param aTaskId the id of the task
*/
	{
    __CFLOG_2(KLogSubSysRS, KLogCode, _L8("CBindManager::JobTimedOut(%S, %d)"), &aModuleName, aTaskId);

	CModule* pModule = 0;
	TRAPD(ret, pModule = GetModulePtrL(aModuleName));
	ret = ret;	// suppress "unused var" warning from release builds
	__ASSERT_DEBUG(KErrNone==ret, User::Panic(KBindMgrPanic, EModuleRefMissing));
	if (pModule)
		{
		pModule->EndTask(aTaskId, KErrRSRequestTimedOut);
		}
	}


