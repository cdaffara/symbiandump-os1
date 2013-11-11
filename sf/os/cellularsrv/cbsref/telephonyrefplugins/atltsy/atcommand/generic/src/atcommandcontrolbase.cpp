// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CATCommandControlBase which is composite class of a number classes
// 

#include "mslogger.h"
#include "atcommandbase.h"
#include "atcommandcontrolbase.h"


CATCommandControlBase::CATCommandControlBase(CGlobalPhonemanager& aGloblePhone, 
                                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                                             :CRequestBase(aGloblePhone),
                                             iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
//	LOGTEXT(_L8("[Ltsy] Starting CATCommandControlBase::CATCommandControlBase()"));
	iIsDel = EAutoDelete;
	}

CATCommandControlBase::~CATCommandControlBase()
	{
//	LOGTEXT(_L8("[Ltsy] Starting CATCommandControlBase::~CATCommandControlBase()"));
	if (EAutoDelete == iIsDel)
		{
		CleanAndDeleteAllAtCommands();
		}
	
	iArray.Close();
	}

TInt CATCommandControlBase::AddAtCommand(const CAtCommandBase* aAtCommand)
	{
//	LOGTEXT(_L8("[Ltsy] Starting CATCommandControlBase::AddAtCommand()"));
	if (NULL == aAtCommand)
		{
		return KErrArgument;
		}
	
	TInt nRes = iArray.Append(aAtCommand);
	return nRes;
	}

void CATCommandControlBase::RemoveAtCommand(const CAtCommandBase* aAtCommand)
	{	
	if (NULL != aAtCommand)
		{
		TInt nFind = iArray.Find(aAtCommand);
		if (nFind != KErrNotFound)
			{
			iArray.Remove(nFind);
			iArray.Compress();
			}
		}
	}

void CATCommandControlBase::RemoveAtCommandByAtType(TLtsyATCommandType aType)
	{
	TBool bFlag(ETrue);
	
	for (TInt n = 0; bFlag && n < iArray.Count(); n++)
		{
		if (iArray[n]->AtType() == aType)
			{
			iArray.Remove(n);
			iArray.Compress();
			bFlag = EFalse;
			}
		}
	}

CAtCommandBase& CATCommandControlBase::GetAtCommandByAtTypeL(TLtsyATCommandType aType)
	{
	TBool bFlag(ETrue);
	CAtCommandBase *cAtBase(NULL);
	
	for (TInt n = 0; bFlag && n < iArray.Count(); n++)
		{
		if (iArray[n]->AtType() == aType)
			{
			cAtBase = iArray[n];
			}
		}
	
	User::LeaveIfNull(cAtBase);
	
	return (*cAtBase);
	}


void CATCommandControlBase::SetIsAutoDelete(TAutoDeleteAtCommand aIsDel)
	{
	iIsDel = aIsDel;
	}

CATCommandControlBase::TAutoDeleteAtCommand CATCommandControlBase::GetIsAutoDelete()
	{
	return iIsDel;
	}

void CATCommandControlBase::CleanAllAtCommands()
	{
	iArray.Reset();
	}

void CATCommandControlBase::CleanAndDeleteAllAtCommands()
	{
	iArray.ResetAndDestroy();
	}
// End of file
















