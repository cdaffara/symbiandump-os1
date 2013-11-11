// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <commschan.h>
#include <cfshared.h>
#include "bm_defs.h"
using namespace CommsFW;

CBindInfo* CBindInfo::NewL(const TRSBindType aType, 
						   const TCFSubModuleAddress& aSubModule1,
						   const TCFSubModuleAddress& aSubModule2,
						   TInt aChannel1Length, TInt aChannel2Length)
/**
@internalComponent 
Creates new CBindInfo object.

@param aType the type of binding
@param aSubModule1 the address of the first bound module
@param aSubModule2 the address of the second bound module
@param aChannel1Length xx
@param aChannel2Length xx

@return CBindInfo* Pointer to the new CBindInfo object.
*/
	{
    CBindInfo* pBi = new(ELeave) CBindInfo(aType, aSubModule1, aSubModule2);
    CleanupStack::PushL(pBi);
    pBi->ConstructL(aChannel1Length,aChannel2Length);
    CleanupStack::Pop(pBi);
    return pBi;
	}


CBindInfo::CBindInfo(const TRSBindType aType, 
						   const TCFSubModuleAddress& aSubModule1,
						   const TCFSubModuleAddress& aSubModule2) :
		iType(aType),
		iSubModule1(aSubModule1),
		iSubModule2(aSubModule2)
/**
@internalComponent 
Constructor for CBindInfo

@param aType the type of binding
@param aSubModule1 the address of the first bound module
@param aSubModule2 the address of the second bound module
*/
	{
    __CFLOG_SMADDR2(( KLogSubSysRS, KLogCode, _L8("CBindInfo::CBindInfo(%X) module1 %S, module2 %S"), this,
		&SubModuleAddr1().Printable(__FullModName1), &SubModuleAddr2().Printable(__FullModName2) ));
	}

CBindInfo::~CBindInfo()
/**
@internalComponent 
Destructor for CBindInfo
*/
	{
    __CFLOG_SMADDR2(( KLogSubSysRS, KLogCode, _L8("CBindInfo::~CBindInfo(%X) module1 %S, module2 %S"), this,
		&SubModuleAddr1().Printable(__FullModName1), &SubModuleAddr2().Printable(__FullModName2) ));
	Dequeue();
	iChannel1.Close();
	iChannel2.Close();
	}

void CBindInfo::ConstructL(TInt aChannel1Length,TInt aChannel2Length)
/**
@internalComponent 
Second phase constructor for CBindInfo

@leave
*/
	{
	User::LeaveIfError(iChannel1.Create(aChannel1Length));
	User::LeaveIfError(iChannel2.Create(aChannel2Length));
	}
CBindInfo::TSubModuleDetails::TSubModuleDetails(const TCFSubModuleAddress& aSubModule) :
		iSubModule(aSubModule),
		iState(CBindManager::EUnbound),
		iResult(KErrNone)
/**
@internalComponent 
Constructor for CBindInfo::TSubModuleDetails
*/
	{
	}

void CBindInfo::Dequeue()
/**
@internalComponent 
Remove this from the linked list
*/
	{
	iLink.Deque();
	}

const RCFChannel& CBindInfo::Channel1() const
/**
@internalComponent 
Accessor method for first RCFChannel in binding

@return RCFChannel& the channel requested
*/
	{
	return iChannel1;
	}

const RCFChannel& CBindInfo::Channel2() const
/**
@internalComponent 
Accessor method for second RCFChannel in binding

@return RCFChannel& the channel requested
*/
	{
	return iChannel2;
	}

const TCFSubModuleAddress& CBindInfo::SubModuleAddr1() const
/**
@internalComponent 
Accessor method for first submodule address in the binding

@return TCFSubModuleAddress& the address requested
*/
	{
	return iSubModule1.iSubModule;
	}

const TCFSubModuleAddress& CBindInfo::SubModuleAddr2() const
/**
@internalComponent 
Accessor method for second submodule address in the binding

@return TCFSubModuleAddress& the address requested
*/
	{
	return iSubModule2.iSubModule;
	}

const TRSBindType& CBindInfo::BindType() const
/**
@internalComponent 
Accessor method for the binding type

@return TRSBindType& the binding type requested
*/
	{
	return iType;
	}

CBindManager::TBMBindState CBindInfo::State1() const
/**
@internalComponent 
Accessor method for state of first submodule in the binding

@return CBindManager::TBMBindState& the state requested
*/
	{
	return iSubModule1.iState;
	}

CBindManager::TBMBindState CBindInfo::State2() const
/**
@internalComponent 
Accessor method for state of second submodule in the binding

@return CBindManager::TBMBindState& the state requested
*/
	{
	return iSubModule2.iState;
	}

void CBindInfo::SetState1(const CBindManager::TBMBindState aState)
/**
@internalComponent 
Set method for state of first submodule in the binding

@param aState new state
*/
	{
	iSubModule1.iState = aState;
	}

void CBindInfo::SetState2(const CBindManager::TBMBindState aState)
/**
@internalComponent 
Set method for state of second submodule in the binding

@param aState new state
*/
	{
	iSubModule2.iState = aState;
	}

TInt CBindInfo::Result1() const
/**
@internalComponent 
Accessor method for result code of first submodule in the binding

@return TInt& the result code requested
*/
	{
	return iSubModule1.iResult;
	}

TInt CBindInfo::Result2() const
/**
@internalComponent 
Accessor method for result code of second submodule in the binding

@return TInt& the result code requested
*/
	{
	return iSubModule2.iResult;
	}

void CBindInfo::SetResult1(const TInt aResult)
/**
@internalComponent 
Set method for result code of first submodule in the binding

@param aResult the new result code
*/
	{
	iSubModule1.iResult = aResult;
	}

void CBindInfo::SetResult2(const TInt aResult)
/**
@internalComponent 
Set method for result code of second submodule in the binding

@param aResult the new result code
*/
	{
	iSubModule2.iResult = aResult;
	}

void CBindInfo::TaskCompleted(const CTask &aTask, const TInt aStatus)
/**
@internalComponent 
Method called when a task has completed and therefore the state of the
binding may need updating

@param aTask the task that has completed
@param aStatus the status code the task returned
*/
	{
	switch(aTask.Message().Code())
		{
	case TCFCommsMessage::ECodeBind:
		{
		const TCFBindMsg& bmsg =  reinterpret_cast<const TCFBindMsg&>(aTask.Message());
		if ((SubModuleAddr1() == *bmsg.SubModule1() &&
				SubModuleAddr2() == *bmsg.SubModule2()) ||
			(SubModuleAddr2() == *bmsg.SubModule1() && 
				SubModuleAddr1() == *bmsg.SubModule2()) )
			{
			// first module in msg is always source of msg, ie the one that's
			// indicated completion
			CBindManager::TBMBindState newState;
			if (aStatus == KErrNone)
				{
				newState = CBindManager::EBound;
				}
			else
				{
				newState = CBindManager::EBindError;
				}
			int portion;
			if (SubModuleAddr1() == *bmsg.SubModule1())
				{
				// There is the singular case of binding to itself, in which we can only use the in progress
				// state to tell which end is responding
				if(SubModuleAddr1() != SubModuleAddr2())
				    {
					portion = 1;
					}
				else if(State1() == CBindManager::EBindInProgress)
					{
					portion = 1;
					}
				else
					{
					portion = 2;
					}
				}
			else
				{
				portion = 2;
				}
			if(portion == 1)
				{
				SetState1(newState);
				SetResult1(aStatus);
				}
			else
				{
				SetState2(newState);
				SetResult2(aStatus);
				}
			}
		}
		break;

	case TCFCommsMessage::ECodeUnbind:
		{
		const TCFUnbindMsg& ubmsg = reinterpret_cast<const TCFUnbindMsg&>(aTask.Message());		
		if ((SubModuleAddr1() == *ubmsg.SubModule1() && 
				SubModuleAddr2() == *ubmsg.SubModule2()) ||
			(SubModuleAddr2() == *ubmsg.SubModule1() && 
				SubModuleAddr1() == *ubmsg.SubModule2()) ) 
			{
			// first module in msg is always source of msg, ie the one that's
			// indicated completion
			CBindManager::TBMBindState newState;
			if (aStatus == KErrNone)
				{
				newState = CBindManager::EUnbound;
				}
			else
				{
				newState = CBindManager::EUnbindError;
				}
			int portion;
			if (SubModuleAddr1() == *ubmsg.SubModule1())
				{
				// There is the singular case of unbinding from itself, in which we can only use the in progress
				// state to tell which end is responding
				if(SubModuleAddr1() != SubModuleAddr2())
					portion = 1;
				else if(State1() == CBindManager::EUnbindInProgress)
					{
					portion = 1;
					}
				else
					{
					portion = 2;
					}
				}
			else
				{
				portion = 2;
				}
			if(portion == 1)
				{
				__ASSERT_DEBUG(CBindManager::EUnbindInProgress == State1(),
								User::Panic(KBindMgrPanic, EBadState));
				SetState1(newState);
				SetResult1(aStatus);
				}
			else
				{
				__ASSERT_DEBUG(CBindManager::EUnbindInProgress == State2(),
								User::Panic(KBindMgrPanic, EBadState));
				SetState2(newState);
				SetResult2(aStatus);
				}
			}
		// if everything is unbound, then clear up 
		if (CBindManager::EUnbound == State1() && 
			CBindManager::EUnbound == State2())
			{
			delete this;
			}
		}
		break;

	default:
		break;
		}
    }



