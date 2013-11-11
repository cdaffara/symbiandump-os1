// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @released
*/

#include "cmdcoopsysperformshutdownactions.h"
#include <barsread2.h>

CCustomCmdCoopSysPerformShutdownActions* CCustomCmdCoopSysPerformShutdownActions::NewL()
	{
	CCustomCmdCoopSysPerformShutdownActions* self = new (ELeave) CCustomCmdCoopSysPerformShutdownActions();
	return self;
	}

CCustomCmdCoopSysPerformShutdownActions::CCustomCmdCoopSysPerformShutdownActions()
	{
	}

CCustomCmdCoopSysPerformShutdownActions::~CCustomCmdCoopSysPerformShutdownActions()
	{
	}

TInt CCustomCmdCoopSysPerformShutdownActions::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	//Connect to RSsmStateAdaptation
	return iSsmStateAdaptation.Connect();
	}

void CCustomCmdCoopSysPerformShutdownActions::Close()
	{
	//Cancel the request if there is any
	iSsmStateAdaptation.RequestCancel();

	//Close RSsmStateAdaptation
	iSsmStateAdaptation.Close();
	}

void CCustomCmdCoopSysPerformShutdownActions::Release()
	{
	delete this;
	}

void CCustomCmdCoopSysPerformShutdownActions::Execute(const TDesC8& aParams, TRequestStatus& aStatus)
	{
	TInt reason = 0;

	//Get the reason value
	TRAPD(err, reason = ExecuteL(aParams));

	if(KErrNone == err)
		{
		//Request coop system to perform restart actions
		iSsmStateAdaptation.RequestCoopSysPerformShutdownActions(reason, aStatus);
		}
	else
		{
		//In case of error complete the request with error code
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
	}

TInt CCustomCmdCoopSysPerformShutdownActions::ExecuteL(const TDesC8& aParams)
	{
	RResourceReader reader;

	//Open the descriptor data
	reader.OpenLC(aParams);

	//Read the reason value from param data
	const TInt reason = reader.ReadInt32L();
	CleanupStack::PopAndDestroy(&reader);
	return reason;
	}

void CCustomCmdCoopSysPerformShutdownActions::ExecuteCancel()
	{
	//Cancel the request if there is any
	iSsmStateAdaptation.RequestCancel();
	}
