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
// ETel3rdParty line functionality part.
// 
//

/**
 @file 
 @internalTechnology
*/

#include "linefunctsteps.h"

//
CGetLineStatusTestStep::CGetLineStatusTestStep()
	:iCallStatusPckg(iCallStatus)
	{
	}

TInt CGetLineStatusTestStep::IssueRequestL(TRequestStatus& aStatus )
	{
	aStatus = iTelephony->GetLineStatus(CTelephony::EVoiceLine,iCallStatusPckg);
	return KErrNone;
	}
	
TInt CGetLineStatusTestStep::CancelRequestL()
	{
	// sync call
	return KErrNone;
	}

//
CGetCallInfoTestStep::CGetCallInfoTestStep()
	:iCallInfoPckg(iCallInfo), 
	 iRCallInfoPckg(iRCallInfo),
	 iCallSelectionPckg(iCallSelection)
	{
	iCallSelection.iSelect = CTelephony::EActiveCall;
	iCallSelection.iLine = CTelephony::EVoiceLine;
	}
TInt CGetCallInfoTestStep::IssueRequestL(TRequestStatus& aStatus )
	{
	aStatus = iTelephony->GetCallInfo(iCallSelectionPckg, iCallInfoPckg, iRCallInfoPckg);
	return KErrNone;
	}
	
TInt CGetCallInfoTestStep::CancelRequestL()
	{
	// sync call
	return KErrNone;
	}

//
