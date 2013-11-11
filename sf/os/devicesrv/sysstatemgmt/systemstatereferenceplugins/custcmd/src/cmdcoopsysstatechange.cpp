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

#include "cmdcoopsysstatechange.h"
#include <ssm/ssmstate.h>
#include <barsread2.h>

CCustomCmdCoopSysStateChange* CCustomCmdCoopSysStateChange::NewL()
	{
	CCustomCmdCoopSysStateChange* self = new (ELeave) CCustomCmdCoopSysStateChange();
	return self;
	}

CCustomCmdCoopSysStateChange::CCustomCmdCoopSysStateChange()
	{
	}

CCustomCmdCoopSysStateChange::~CCustomCmdCoopSysStateChange()
	{
	}

TInt CCustomCmdCoopSysStateChange::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	//Connect to RSsmStateAdaptation
	return iSsmStateAdaptation.Connect();
	}

void CCustomCmdCoopSysStateChange::Close()
	{
	//Cancel the request if there is any
	iSsmStateAdaptation.RequestCancel();
	
	//Close RSsmStateAdaptation
	iSsmStateAdaptation.Close();
	}

void CCustomCmdCoopSysStateChange::Release()
	{
	delete this;
	}

void CCustomCmdCoopSysStateChange::Execute(const TDesC8& aParams, TRequestStatus& aStatus)
	{
	TSsmState ssmState;

	//Internalise the param in to TSsmState
	TRAPD(err, ssmState = ExecuteL(aParams));

	if(KErrNone == err)
		{
		//Request coop system to perform Rfs actions
		iSsmStateAdaptation.RequestCoopSysStateChange(ssmState, aStatus);
		}
	else
		{
		//In case of error complete the request with error code
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
	}

TSsmState CCustomCmdCoopSysStateChange::ExecuteL(const TDesC8& aParams)
	{
	RResourceReader reader;

	//Open the descriptor data to be internalised
	reader.OpenLC(aParams);

	//Read the main state from param data
	const TInt mainState = reader.ReadInt16L();

	//Read the substate from param data
	const TInt subState = reader.ReadInt16L();
	CleanupStack::PopAndDestroy(&reader); 

	//Create a TSsmState object and return
	return TSsmState(mainState, subState);
	}

void CCustomCmdCoopSysStateChange::ExecuteCancel()
	{
	//Cancel the request if there is any
	iSsmStateAdaptation.RequestCancel();
	}
