// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file CorruptPSYStep.cpp
 @internalTechnology
*/
#include "corruptpsystep.h"
#include "te_lbsbackuprestoresuitedefs.h"

CCorruptPSYStep::~CCorruptPSYStep()
/**
 * Destructor
 */
    {
    }

CCorruptPSYStep::CCorruptPSYStep()
/**
 * Constructor
 */
    {
    SetTestStepName(KCorruptPSYStep);
    }

CCorruptPSYStep* CCorruptPSYStep::NewLC()
	{
	CCorruptPSYStep* self = new (ELeave)CCorruptPSYStep();
	CleanupStack::PushL(self);
	return self;
	}

CCorruptPSYStep* CCorruptPSYStep::NewL()
	{
	CCorruptPSYStep* self=CCorruptPSYStep::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

TVerdict CCorruptPSYStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    CTe_lbsbackuprestoreSuiteStepBase::doTestStepPreambleL();
    SetTestStepResult(EPass);
    return TestStepResult();
    }


TVerdict CCorruptPSYStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
    {
    TInt err = KErrNone;
    TPtrC iniFileName;
    TInt numberOfPSY = KErrNotFound;
    
    err = GetIntFromConfig(KPSYList, KPSYListSize, numberOfPSY);
    
    CPosModules* modules = CPosModules::OpenL();
    CleanupStack::PushL(modules);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
    CPosModuleIdList* idList = modules->ModuleIdListLC();
    
    if(numberOfPSY!=idList->Count())
        {
        SetTestStepResult(EFail);
        }
    
    TInt psyUid = KErrNotFound;
    TBuf<128> lineBuf;
    TUint32 brandKey = KBackupKey;
    TInt scarkey = KCorruptedValue;
    
    for (TInt i = 0; i < numberOfPSY; i++)
        {
        lineBuf.Delete(0,lineBuf.Length());
        lineBuf.AppendFormat(KUIDString, i);
        err = GetIntFromConfig(KPSYList, lineBuf, psyUid);
        err = idList->Find(TUid::Uid(psyUid));
        if(KErrNotFound==err)
            {
            SetTestStepResult(EFail);
            }
        CRepository* centrepentry = CRepository::NewL(TUid::Uid(psyUid));
        err = centrepentry->Set(brandKey, scarkey);
        if(!((KErrNone==err) || (KErrPermissionDenied==err)))
            {
            SetTestStepResult(EFail);
            }
        }
    
    CleanupStack::PopAndDestroy(3, modules);
    return TestStepResult();
    }

TVerdict CCorruptPSYStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    return TestStepResult();
    }
