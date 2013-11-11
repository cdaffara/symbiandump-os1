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
 @file OOMStep.cpp
 @internalTechnology
*/
#include "OOMStep.h"
#include "Te_defproxySuiteDefs.h"

COOMStep::~COOMStep()
/**
 * Destructor
 */
	{
	}

COOMStep::COOMStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KOOMStep);
	}

TVerdict COOMStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict COOMStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    //Init time
    iTime1.UniversalTime();
    iTime2.UniversalTime();
    
    iModulesSettings = CPosModules::OpenL();
    
    InitPsySettingsL(2, 
        KIntGpsPsy1, 1,  
        KNetworkPsy2, 1);

    for(TInt i=0; i<60; i++)
        {
        //Set failrate
        RFs fs;
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);
        RFile file;
        User::LeaveIfError(file.Replace(fs, KOOMTextPath, EFileWrite));
        CleanupClosePushL(file);
        TFileText fileText;
        fileText.Set(file);
        TBuf<32> frbuf;
        frbuf.AppendNum(i);
        fileText.Write(frbuf);
        CleanupStack::PopAndDestroy(2,&fs);

        TRAPD(err, MakeLRL());
        if(KErrNone == err)
            {
            INFO_PRINTF2(KErrorCode,err);
            }
        else
            {
            ERR_PRINTF2(KErrorCode,err);
            SetTestStepResult(EFail);
            }
        }
    return TestStepResult();
	}

void COOMStep::MakeLRL()
    {
    User::LeaveIfError(iServer.Connect());
    CleanupClosePushL(iServer);
    User::LeaveIfError(iPositioner.Open(iServer));
    CleanupClosePushL(iPositioner);
    User::LeaveIfError(iPositioner.SetRequestor( 
            CRequestor::ERequestorService,       
            CRequestor::EFormatApplication,      
            KTdDefProxyTestName));               
    TPositionInfo posInfo;
    TRequestStatus status;
    iPositioner.NotifyPositionUpdate(posInfo,status);
    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());
    TPositionSatelliteInfo satInfo;
    iPositioner.NotifyPositionUpdate(satInfo,status);
    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());

    CleanupStack::PopAndDestroy(2, &iServer);
    User::After(KSecond*3);
    }


TVerdict COOMStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
