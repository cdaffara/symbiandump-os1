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

#include "SystemStartupStateInfo.h"

//
// Standard Symbian factory functions/destructor
//

CSystemStartupStateInfo* CSystemStartupStateInfo::NewL()
	{
	CSystemStartupStateInfo* self = CSystemStartupStateInfo::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CSystemStartupStateInfo* CSystemStartupStateInfo::NewLC()
	{
	CSystemStartupStateInfo* self = new (ELeave) CSystemStartupStateInfo;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSystemStartupStateInfo::~CSystemStartupStateInfo()
	{
	TInt i = 0;
	for (; i < Count(); ++i)
		{
		iCommandList[i]->Release();
		}
	iCommandList.Close();
	
	delete iName;
	}

//
// Public member functions
//

void CSystemStartupStateInfo::SetCommandList(CommandList aCommandList)
	{
	for (TInt i = 0; i < Count(); ++i)
		{
		delete iCommandList[i];
		}
	iCommandList.Close();
	iCommandList = aCommandList;
	}

void CSystemStartupStateInfo::SetStateId(TUint8 aStateId)
	{
	iStateId = aStateId;
	}
	
void CSystemStartupStateInfo::SetName(const TDesC& aName)
	{
	delete iName;
	iName = aName.Alloc();
	}
	
void CSystemStartupStateInfo::SetNoOfRetries(TNoOfRetries aNoOfRetries)
	{
	iNoOfRetries = aNoOfRetries;
	}
	
void CSystemStartupStateInfo::SetActionOnStateTransitionFailure(TActionOnStateTransitionFailure aActionOnStateTransitionFailure)
	{
	iActionOnStateTransitionFailure = aActionOnStateTransitionFailure;
	}
	
TInt CSystemStartupStateInfo::AddCommand(MStartupCommand* aAppInfo)
	{
	return iCommandList.Append(aAppInfo);
	}

//
// Inherited virtual overrides (MStartupStateInfo)
//

TUint8 CSystemStartupStateInfo::StateId() const
	{
	return iStateId;
	}
		
TPtrC CSystemStartupStateInfo::Name() const
	{
	TPtrC retval;
	if (iName)
		{
		retval.Set(*iName);
		}
	return retval;
	}

TNoOfRetries CSystemStartupStateInfo::NoOfRetries() const  
 	{
 	return iNoOfRetries;
 	}
 
TActionOnStateTransitionFailure CSystemStartupStateInfo::ActionOnStateTransitionFailure() const
	{
	return iActionOnStateTransitionFailure;
	}
    
 
const CommandList CSystemStartupStateInfo::GetCommandList() const
	{
	return iCommandList;
	}
	
 
TInt CSystemStartupStateInfo::Count() const
	{
	return iCommandList.Count();
	}

MStartupCommand* CSystemStartupStateInfo::GetCommand(TInt aIndex) const
	{
	return iCommandList[aIndex];
	}

//
// Private member functions
//

CSystemStartupStateInfo::CSystemStartupStateInfo()
: iNoOfRetries(ERetry0), iActionOnStateTransitionFailure(EIgnoreFailure)
	{
	}

void CSystemStartupStateInfo::ConstructL()
	{
	}

