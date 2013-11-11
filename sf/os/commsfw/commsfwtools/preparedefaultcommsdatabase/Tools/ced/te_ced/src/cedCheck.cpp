// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file ced0Step.cpp
 @internalTechnology
*/
#include "cedCheck.h"
#include "Te_cedSuiteDefs.h"

#include <bautils.h>


CcedCheck::~CcedCheck()
/**
 * Destructor
 */
	{
	}

CcedCheck::CcedCheck()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KcedCheck);
	}

TVerdict CcedCheck::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


/**
 * ----------------------------- DESCRIPTION ------------------------------------
 *
 * With this test step the DB creation with the ced command can be verified.
 * BUT important that the CED should have a '-v' parameter to produce the correct
 * ced.log file which is the base of this verification. Currently this log file 
 * is serched on the 'c:\' drive only - no parameter can be processed curerntly
 * related to the location of the log file.
 *
 * -------------------------------------------------------------------------------
 */
TVerdict CcedCheck::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    SetTestStepResult(EPass);

    //Wait here to synchronize with the CED process...
        

    RFs fsSession;
    CleanupClosePushL(fsSession);
    
    RFile file;
    CleanupClosePushL(file);

    //fsSession.Connect();
    TInt errorCode = 0;
    
    if ( KErrNone == (errorCode = fsSession.Connect()) )
    {
        TPtrC logFileName(KLogFile);
        
        if ( KErrNone == (errorCode = file.Open(fsSession,logFileName,EFileRead)) )
            {
            /**
             * Important!!!!!!
             * This read operation assumes that there are no unicode characters
             * in the log file. This code has to be modified if such kind of
             * a requirement is coming...
             */
             
            TInt size = 0;
            
            TDesC string(KStringToCheck);
            
            HBufC8* buffer = HBufC8::NewLC(TPtrC(KStringToCheck).Length());
            
            TPtr8 destString(buffer->Des());
            
            if ( KErrNone == (errorCode = file.Size(size)) )
                {
                if ( KErrNone == (errorCode = file.Read(size-TPtrC(KStringToCheck).Length()-KFileEndSign,
                                                        destString,
                                                        TPtrC(KStringToCheck).Length())) )
                /**
                 * Finally we got the string from the end of the file.
                 */
                    {
                    HBufC* resultString = HBufC::NewLC(destString.Length());
                    TPtr result(resultString->Des());
                    
                    result.Copy(destString);
                    
                    if (result != TPtrC(KStringToCheck))
                        SetTestStepResult(EFail);
                    
                    CleanupStack::PopAndDestroy(resultString);
                    
                    }
                else
                    INFO_PRINTF2(_L("Error while trying to open the file 'ced.log' - %d"), errorCode);
                }
            else
                INFO_PRINTF2(_L("Error while trying to get the file size of the 'ced.log' file - %d"), errorCode);
            
            CleanupStack::PopAndDestroy(buffer);
            
            }
        else
            INFO_PRINTF2(_L("Couldn't find the 'ced.log' file in c:\\ced.log. ErrorCode: %d"), errorCode);
    }
    else
        INFO_PRINTF2(_L("Error while connecting to the file server. ErrorCode: %d"), errorCode);
        

    CleanupStack::PopAndDestroy(&file);
    CleanupStack::PopAndDestroy(&fsSession);
    
    
    return TestStepResult();
	}



TVerdict CcedCheck::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_cedSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
