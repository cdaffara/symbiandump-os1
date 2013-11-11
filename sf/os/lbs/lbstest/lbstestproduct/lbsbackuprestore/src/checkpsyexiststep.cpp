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
 @file CheckPSYExistStep.cpp
 @internalTechnology
*/
#include "checkpsyexiststep.h"
#include "te_lbsbackuprestoresuitedefs.h"

CCheckPSYExistStep::~CCheckPSYExistStep()
/**
 * Destructor
 */
    {
    }

CCheckPSYExistStep::CCheckPSYExistStep()
/**
 * Constructor
 */
    {
    SetTestStepName(KCheckPSYExistStep);
    }

CCheckPSYExistStep* CCheckPSYExistStep::NewLC()
	{
	CCheckPSYExistStep* self = new (ELeave)CCheckPSYExistStep();
	CleanupStack::PushL(self);
	return self;
	}

CCheckPSYExistStep* CCheckPSYExistStep::NewL ()
	{
	CCheckPSYExistStep* self=CCheckPSYExistStep::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

TVerdict CCheckPSYExistStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    CTe_lbsbackuprestoreSuiteStepBase::doTestStepPreambleL();
    SetTestStepResult(EPass);
    return TestStepResult();
    }


TVerdict CCheckPSYExistStep::doTestStepL()
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
    TUint32 metadataTag = KBackupKey;
    TInt scarkey = KErrNotFound;
    
    for (TInt i = 0; i < numberOfPSY; i++)
        {
        lineBuf.Delete(0,lineBuf.Length());
        lineBuf.AppendFormat(KUIDString, i);
        err = GetIntFromConfig(KPSYList, lineBuf, psyUid);
        err = idList->Find(TUid::Uid(psyUid));
        if(KErrNotFound==err)
		{
        	INFO_PRINTF2(_L("CCheckPSYExistStep:can't find (%d) in idList "), psyUid);
		SetTestStepResult(EFail);
		}
        CRepository* centrepentry = CRepository::NewL(TUid::Uid(psyUid));
        err = centrepentry->Get(brandKey, scarkey);
        if(KErrNotFound!=err)
		{
		centrepentry->GetMeta(brandKey, metadataTag);
		if(metadataTag >= KMetatagBackupAndRestoreValidValue)
			{
			if(scarkey!=KValidValue)
				{
	             		INFO_PRINTF3(_L("scarkey(%d)!=KValidValue(%d)"), scarkey,KValidValue);
	                	SetTestStepResult(EFail);
	                	}
            		}
		}
        else
        	{
        	err = centrepentry->Set(brandKey, scarkey);
        	if(KErrPermissionDenied == err)
            		{
     			INFO_PRINTF2(_L("%d is read only"), psyUid);
            		}
		else
			{
         		INFO_PRINTF1(_L("CCheckPSYExistStep:centrepentry->Get(brandKey, scarkey) NOT found"));
        		}
		}
        }
    
    CleanupStack::PopAndDestroy(3, modules);
    return TestStepResult();
    }

TVerdict CCheckPSYExistStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    return TestStepResult();
    }
