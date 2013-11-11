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

#include "cmdcoopsysperformrfsactions.h"
#include <barsread2.h>

CCustomCmdCoopSysPerformRfsActions* CCustomCmdCoopSysPerformRfsActions::NewL()
	{
	CCustomCmdCoopSysPerformRfsActions* self = new (ELeave) CCustomCmdCoopSysPerformRfsActions();
	return self;
	}

CCustomCmdCoopSysPerformRfsActions::CCustomCmdCoopSysPerformRfsActions()
	{
	}

CCustomCmdCoopSysPerformRfsActions::~CCustomCmdCoopSysPerformRfsActions()
	{
	}

TInt CCustomCmdCoopSysPerformRfsActions::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	//Connect to RSsmStateAdaptation
	return iSsmStateAdaptation.Connect();
	}

void CCustomCmdCoopSysPerformRfsActions::Close()
	{
	//Cancel the request if there is any
	iSsmStateAdaptation.RequestCancel();
	
	//Close RSsmStateAdaptation
	iSsmStateAdaptation.Close();
	}

void CCustomCmdCoopSysPerformRfsActions::Release()
	{
	delete this;
	}

void CCustomCmdCoopSysPerformRfsActions::Execute(const TDesC8& aParams, TRequestStatus& aStatus)
	{
	TSsmRfsType ssmRfsType = ESsmShallowRfs;

	//Internalise the param in to TSsmState
	TRAPD(err, ssmRfsType = ExecuteL(aParams));

	if(KErrNone == err)
		{
		//Request coop system to perform Rfs actions
		iSsmStateAdaptation.RequestCoopSysPerformRfsActions(ssmRfsType, aStatus);
		}
	else
		{
		//In case of error complete the request with error code
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
	}

TSsmRfsType CCustomCmdCoopSysPerformRfsActions::ExecuteL(const TDesC8& aParams)
	{
	RResourceReader reader;

	//Open the descriptor data to be internalised
	reader.OpenLC(aParams);

	//Read the type from descriptor data
	TInt32 type = reader.ReadInt32L();
	CleanupStack::PopAndDestroy(&reader);

	//Check for valid enum value
	if (type != ESsmShallowRfs && type != ESsmDeepRfs &&
		type < ESsmRfsTypeExtensionBase)
	    {
	    //Leave incase of a invalid enum value
	    User::Leave(KErrArgument);
	    }

	//Read the main state from param data
	TSsmRfsType ssmRfsType = static_cast<TSsmRfsType>(type);

	//Create a TSsmState object and return
	return ssmRfsType;
	}

void CCustomCmdCoopSysPerformRfsActions::ExecuteCancel()
	{
	//Cancel the request if there is any
	iSsmStateAdaptation.RequestCancel();
	}
