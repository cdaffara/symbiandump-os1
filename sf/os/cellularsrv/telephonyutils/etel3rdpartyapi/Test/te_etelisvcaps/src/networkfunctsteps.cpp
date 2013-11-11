// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implmentation of all test objects required to verify capability check on 
// ETel3rdParty network functionality part.
// 
//

/**
 @file 
 @internalTechnology
*/

#include "networkfunctsteps.h"

//
CGetNetworkRegistrationStatusTestStep::CGetNetworkRegistrationStatusTestStep()
	:iNetworkRegistrationPckg(iNetworkRegistration)
	{
	}
   
TInt CGetNetworkRegistrationStatusTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetNetworkRegistrationStatus(aStatus, iNetworkRegistrationPckg);	
	return KErrNone;
	}

TInt CGetNetworkRegistrationStatusTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetNetworkRegistrationStatusCancel);
	return KErrNone;
	}

//
CGetCurrentNetworkInfoTestStep::CGetCurrentNetworkInfoTestStep()
	:iNetworkInfoPckg(iNetworkInfo)
	{
	}
   
TInt CGetCurrentNetworkInfoTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetCurrentNetworkInfo(aStatus, iNetworkInfoPckg);	
	return KErrNone;
	}

TInt CGetCurrentNetworkInfoTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
	return KErrNone;
	}

//
CGetCurrentNetworkNameTestStep::CGetCurrentNetworkNameTestStep()
	:iNetworkNamePckg(iNetworkName)
	{
	}
   
TInt CGetCurrentNetworkNameTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetCurrentNetworkName(aStatus, iNetworkNamePckg);	
	return KErrNone;
	}

TInt CGetCurrentNetworkNameTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkNameCancel);
	return KErrNone;
	}

//
CGetOperatorNameTestStep::CGetOperatorNameTestStep()
	:iOperatorNamePckg(iOperatorName)
	{
	}
   
TInt CGetOperatorNameTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetOperatorName(aStatus, iOperatorNamePckg);	
	return KErrNone;
	}

TInt CGetOperatorNameTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetOperatorNameCancel);
	return KErrNone;
	}

//
