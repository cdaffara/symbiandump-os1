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
// This contains CActiveCommandStore which store one and only one solicited active at command and a number of active 
// unsolicited at commands
// 

#include "activecommandstore.h"
#include "mslogger.h"

//const TInt KLtsyActiveCommandStoreReserve = 5;

CActiveCommandStore* CActiveCommandStore::NewL()
	{
	LOGTEXT(_L8("[Ltsy] Starting CActiveCommandStore::NewL()"));
	CActiveCommandStore *self = CActiveCommandStore::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CActiveCommandStore* CActiveCommandStore::NewLC()
	{
	LOGTEXT(_L8("[Ltsy] Starting CActiveCommandStore::NewLC()"));
	CActiveCommandStore *self = new (ELeave) CActiveCommandStore;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CActiveCommandStore::~CActiveCommandStore()
	{
	LOGTEXT(_L8("[Ltsy] Starting CActiveCommandStore::~CActiveCommandStore()"));
	iArray.Close();
	}

CActiveCommandStore::CActiveCommandStore()
	{
	LOGTEXT(_L8("[Ltsy] Starting CActiveCommandStore::CActiveCommandStore()"));
	}

void CActiveCommandStore::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy] Starting CActiveCommandStore::ConstructL()"));
	iArray.Reset();
	iArray.AppendL(NULL);
	//iArray.ReserveL(KLtsyActiveCommandStoreReserve);
	LOGTEXT2(_L8("[Ltsy] Active Store Count = %d"), iArray.Count());
	}

TInt CActiveCommandStore::AddUnsolicitedAtCommand(CAtCommandBase *aAtCommand)
	{
	LOGTEXT(_L8("[Ltsy] Starting CActiveCommandStore::AddUnsolicitedAtCommand()"));
	if (aAtCommand == NULL)
		{
		return KErrArgument;
		}
	
	TInt nRes(KErrNone);
	nRes = iArray.Append(aAtCommand);
	LOGTEXT2(_L8("[Ltsy] Active Store Count = %d"), iArray.Count());
	return nRes;	
	}

TInt CActiveCommandStore::AddSolicitedAtCommand(CAtCommandBase *aAtCommand)
	{
	LOGTEXT(_L8("[Ltsy] Starting CActiveCommandStore::AddSolicitedAtCommand()"));
	if (aAtCommand == NULL)
		{
		return KErrArgument;
		}
	
	iArray[0] = aAtCommand;
	LOGTEXT2(_L8("[Ltsy] Active Store Count = %d"), iArray.Count());
	return KErrNone;	
	}

TInt CActiveCommandStore::RemoveActiveAtCommand(const CAtCommandBase *aAtCommand)
	{
	LOGTEXT(_L8("[Ltsy] Starting CActiveCommandStore::RemoveActiveAtCommand()"));
	if (aAtCommand == NULL) 
		{
		return KErrArgument;
		}
	
	TInt nFind = iArray.Find(aAtCommand);
	if (nFind != KErrNotFound)
		{
		if (nFind == 0)
			{
			iArray[0] = NULL;
			}
		else
			{
			iArray.Remove(nFind);
			}
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}
	}

RPointerArray<CAtCommandBase>& CActiveCommandStore::GetActiveCommandArray()
	{
	return iArray;
	}

//End of file











