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

#include "cmdcoopsysperformrestartactions.h"
#include <barsread2.h>

CCustomCmdCoopSysPerformRestartActions* CCustomCmdCoopSysPerformRestartActions::NewL()
	{
	CCustomCmdCoopSysPerformRestartActions* self = new (ELeave) CCustomCmdCoopSysPerformRestartActions();
	return self;
	}

CCustomCmdCoopSysPerformRestartActions::CCustomCmdCoopSysPerformRestartActions()
	{
	}

CCustomCmdCoopSysPerformRestartActions::~CCustomCmdCoopSysPerformRestartActions()
	{
	}

TInt CCustomCmdCoopSysPerformRestartActions::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	//Connect to RSsmStateAdaptation
	return iSsmStateAdaptation.Connect();
	}

void CCustomCmdCoopSysPerformRestartActions::Close()
	{
	//Cancel the request if there is any
	iSsmStateAdaptation.RequestCancel();
	
	//Close RSsmStateAdaptation
	iSsmStateAdaptation.Close();
	}

void CCustomCmdCoopSysPerformRestartActions::Release()
	{
	delete this;
	}

void CCustomCmdCoopSysPerformRestartActions::Execute(const TDesC8& aParams, TRequestStatus& aStatus)
	{
	TInt reason = 0;

	//Get the reason value
	TRAPD(err, reason = ExecuteL(aParams));

	if(KErrNone == err)
		{
		//Request coop system to perform restart actions
		iSsmStateAdaptation.RequestCoopSysPerformRestartActions(reason, aStatus);
		}
	else
		{
		//In case of error complete the request with error code
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
	}

TInt CCustomCmdCoopSysPerformRestartActions::ExecuteL(const TDesC8& aParams)
	{
	RResourceReader reader;

	//Open the descriptor data
	reader.OpenLC(aParams);

	//Read the reason value from param data
	const TInt reason = reader.ReadInt32L();
	CleanupStack::PopAndDestroy(&reader);
	return reason;
	}

void CCustomCmdCoopSysPerformRestartActions::ExecuteCancel()
	{
	//Cancel the request if there is any
	iSsmStateAdaptation.RequestCancel();
	}
