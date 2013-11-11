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

#include "ModemChangeObserver.h"

// System includes
#include <cdbcols.h>



//
// ------> CModemChangeObserver (source)
//

EXPORT_C CModemChangeObserver::CModemChangeObserver(MModemChangeObserver& aObserver)
:	CActive(EPriorityNormal), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C CModemChangeObserver::~CModemChangeObserver()
	{
	Cancel();
	}

EXPORT_C void CModemChangeObserver::ConstructL()
	{
	User::LeaveIfError(iModemChangeProperty.Attach(KUidSystemCategory, KUidCommDbModemPhoneServicesAndSMSChange.iUid));	
	IssueRequestL();
	}

//
//
//

void CModemChangeObserver::IssueRequestL()
	{
	// Queue for notifications
	iModemChangeProperty.Subscribe(iStatus);
	//
	SetActive();
	}

//
//
//

void CModemChangeObserver::RunL()
	{
	// Notify client
	iObserver.HandleModemChangedL();

	// Re-issue request
	IssueRequestL();
	}

void CModemChangeObserver::DoCancel()
	{
	iModemChangeProperty.Cancel();
	}


TInt CModemChangeObserver::RunError(TInt aError)
	{
	if	(aError != KErrNotSupported)
		return KErrNone;
	return aError;
	}

