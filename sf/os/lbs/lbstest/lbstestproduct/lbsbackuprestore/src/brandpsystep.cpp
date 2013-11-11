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
 @file BrandPSYStep.cpp
 @internalTechnology
*/
#include "brandpsystep.h"
#include "te_lbsbackuprestoresuitedefs.h"

CBrandPSYStep::~CBrandPSYStep()
/**
 * Destructor
 */
    {
    }

CBrandPSYStep::CBrandPSYStep()
/**
 * Constructor
 */
    {
    SetTestStepName(KBrandPSYStep);
    }

CBrandPSYStep* CBrandPSYStep::NewLC()
	{
	CBrandPSYStep* self = new (ELeave)CBrandPSYStep();
	CleanupStack::PushL(self);
	return self;
	}

CBrandPSYStep* CBrandPSYStep::NewL()
	{
	CBrandPSYStep* self=CBrandPSYStep::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

TVerdict CBrandPSYStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    CTe_lbsbackuprestoreSuiteStepBase::doTestStepPreambleL();
    SetTestStepResult(EPass);
    return TestStepResult();
    }


TVerdict CBrandPSYStep::doTestStepL()
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
 		INFO_PRINTF3(_L("CBrandPSYStep:numberOfPSY(%d)!=idList(%d"), numberOfPSY,idList->Count());
        SetTestStepResult(EFail);
        }
    
    TInt psyUid = KErrNotFound;
    TBuf<128> lineBuf;
    TUint32 brandKey = KBackupKey;
    TInt scarkey = KValidValue;
    
    for (TInt i = 0; i < numberOfPSY; i++)
        {
        lineBuf.Delete(0,lineBuf.Length());
        lineBuf.AppendFormat(KUIDString, i);
        err = GetIntFromConfig(KPSYList, lineBuf, psyUid);
        err = idList->Find(TUid::Uid(psyUid));
        if(KErrNone<=err)
        	{
      		INFO_PRINTF2(_L("found (%d)in idList "), psyUid);
        	}
        else if(KErrNotFound==err)
        	{
    		INFO_PRINTF2(_L("can't find (%d) in idList "), psyUid);
        	SetTestStepResult(EFail);
        	}
        else
        	{
    		INFO_PRINTF3(_L("err(%d) = idList->Find()%d"), err, psyUid);
		SetTestStepResult(EFail);
        	}
        CRepository* centrepentry = CRepository::NewL(TUid::Uid(psyUid));
        err = centrepentry->Set(brandKey, scarkey);
        if(KErrPermissionDenied == err)
            {
     		INFO_PRINTF2(_L("%d is read only"), psyUid);
            }
        else if(KErrNone!=err)
            {
     		INFO_PRINTF2(_L("CBrandPSYStep: err (%d)= centrepentry->Set()"), err);
		SetTestStepResult(EFail);
            }
        }
    
    CleanupStack::PopAndDestroy(3, modules);
    return TestStepResult();
    }

TVerdict CBrandPSYStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    return TestStepResult();
    }
