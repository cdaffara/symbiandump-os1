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

#include "cmddeactivaterfforemergencycall.h"

CCustomCmdDeactivateRfForEmergencyCall* CCustomCmdDeactivateRfForEmergencyCall::NewL()
	{
	CCustomCmdDeactivateRfForEmergencyCall* self = new (ELeave) CCustomCmdDeactivateRfForEmergencyCall();
	return self;
	}

CCustomCmdDeactivateRfForEmergencyCall::CCustomCmdDeactivateRfForEmergencyCall()
	{
	}

CCustomCmdDeactivateRfForEmergencyCall::~CCustomCmdDeactivateRfForEmergencyCall()
	{
	}

TInt CCustomCmdDeactivateRfForEmergencyCall::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	//Connect to RSsmEmergencyCallRfAdaptation
	return iSsmEmergencyCallRfAdaptation.Connect();
	}

void CCustomCmdDeactivateRfForEmergencyCall::Close()
	{
	//Cancel the request if there is any
	iSsmEmergencyCallRfAdaptation.Cancel();
	
	//Close RSsmEmergencyCallRfAdaptation
	iSsmEmergencyCallRfAdaptation.Close();
	}

void CCustomCmdDeactivateRfForEmergencyCall::Release()
	{
	delete this;
	}

void CCustomCmdDeactivateRfForEmergencyCall::Execute(const TDesC8& /*aParams*/, TRequestStatus& aStatus)
	{
	//Deactivate Rf for emergency call
	iSsmEmergencyCallRfAdaptation.DeactivateRfForEmergencyCall(aStatus);
	}

void CCustomCmdDeactivateRfForEmergencyCall::ExecuteCancel()
	{
	//Cancel the request if there is any
	iSsmEmergencyCallRfAdaptation.Cancel();
	}
