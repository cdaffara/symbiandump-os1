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
 @file ListPsyEntriesStep.cpp
 @internalTechnology
*/
#include "listpsyentriesstep.h"
#include "te_lbsbackuprestoresuitedefs.h"

CListPsyEntriesStep::~CListPsyEntriesStep()
/**
 * Destructor
 */
	{
	}

CListPsyEntriesStep::CListPsyEntriesStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KListPsyEntriesStep);
	}

CListPsyEntriesStep* CListPsyEntriesStep::NewLC()
	{
	CListPsyEntriesStep* self = new (ELeave)CListPsyEntriesStep();
	CleanupStack::PushL(self);
	return self;
	}

CListPsyEntriesStep* CListPsyEntriesStep::NewL()
	{
	CListPsyEntriesStep* self=CListPsyEntriesStep::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

TVerdict CListPsyEntriesStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_lbsbackuprestoreSuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CListPsyEntriesStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{  
    CPosModules* modules = CPosModules::OpenL();
    CleanupStack::PushL(modules);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
    CPosModuleIdList* idList = modules->ModuleIdListLC();
    
    INFO_PRINTF2(KLogPSYListSize, idList->Count());
    TBuf<128> lineBuf;
    

    WriteIntToConfig(KPSYList, KPSYListSize, idList->Count());

    
    TBuf<KPositionMaxModuleName> modName;
    
    for (TInt i = 0; i < idList->Count(); i++)
        {
        TPositionModuleInfo info;
        modules->GetModuleInfoL((*idList)[i], info);
        INFO_PRINTF3(KLogUIDString, i, info.ModuleId().iUid);
        lineBuf.Delete(0,lineBuf.Length());
        lineBuf.AppendFormat(KUIDString, i);

		TInt uid = static_cast<TInt>(info.ModuleId().iUid);
        WriteIntToConfig(KPSYList, lineBuf, uid);
            
        info.GetModuleName(modName);
        INFO_PRINTF3(KLogNameString, i, &modName);
        lineBuf.Delete(0,lineBuf.Length());
        lineBuf.AppendFormat(KNameString, i);
        WriteStringToConfig(KPSYList, lineBuf, modName);
        }
    CleanupStack::PopAndDestroy(3, modules);
    return TestStepResult();
	}


TVerdict CListPsyEntriesStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
