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

/**
 @file
*/

#include "TE_smsprtstressbase.h"

#include <sacls.h>
#include <cfshared.h>
#include <c32root.h>
#include <connect/sbdefs.h>

//using namespace conn;

TVerdict CSmsPrtStressTestStep::doTestStepPreambleL()
	{
    //base class preamble - marks the heap
//    CSmsBaseTestStep::doTestStepPreambleL();

//	iSecureBackupEngine = CSBEClient::NewL();
//	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
//									 EBURNormal, ENoBackup);

    return TestStepResult() ;
	}

TVerdict CSmsPrtStressTestStep::doTestStepPostambleL()
	{
//	delete iSecureBackupEngine;
//	iSecureBackupEngine = NULL;

	//base class postamble - unmarks the heap
//	CSmsBaseTestStep::doTestStepPostambleL();

	return TestStepResult() ;
	}
