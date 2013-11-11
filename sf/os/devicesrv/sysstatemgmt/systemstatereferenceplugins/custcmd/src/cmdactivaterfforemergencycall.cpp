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

#include "cmdactivaterfforemergencycall.h"

CCustomCmdActivateRfForEmergencyCall* CCustomCmdActivateRfForEmergencyCall::NewL()
	{
	CCustomCmdActivateRfForEmergencyCall* self = new (ELeave) CCustomCmdActivateRfForEmergencyCall();
	return self;
	}

CCustomCmdActivateRfForEmergencyCall::CCustomCmdActivateRfForEmergencyCall()
	{
	}

CCustomCmdActivateRfForEmergencyCall::~CCustomCmdActivateRfForEmergencyCall()
	{
	}

TInt CCustomCmdActivateRfForEmergencyCall::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	//Connect to RSsmEmergencyCallRfAdaptation
	return iSsmEmergencyCallRfAdaptation.Connect();
	}

void CCustomCmdActivateRfForEmergencyCall::Close()
	{
	//Cancel the request if there is any
	iSsmEmergencyCallRfAdaptation.Cancel();
	
	//Close RSsmEmergencyCallRfAdaptation
	iSsmEmergencyCallRfAdaptation.Close();
	}

void CCustomCmdActivateRfForEmergencyCall::Release()
	{
	delete this;
	}

void CCustomCmdActivateRfForEmergencyCall::Execute(const TDesC8& /*aParams*/, TRequestStatus& aStatus)
	{
	//Activate Rf for emergency call
	iSsmEmergencyCallRfAdaptation.ActivateRfForEmergencyCall(aStatus);
	}

void CCustomCmdActivateRfForEmergencyCall::ExecuteCancel()
	{
	//Cancel the request if there is any
	iSsmEmergencyCallRfAdaptation.Cancel();
	}
