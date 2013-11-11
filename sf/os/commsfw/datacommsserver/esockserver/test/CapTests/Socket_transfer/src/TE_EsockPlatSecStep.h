// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test step header file for the ESOCK PlatSec tests
// 
//

/**
 @file
 @internalAll PlatSec test code for the ESOCK
*/

#ifndef     __TE_ESOCK_PLATSEC_STEP_H__
#define     __TE_ESOCK_PLATSEC_STEP_H__

#include <test/testexecutestepbase.h>
#include "TE_EsockPlatSecServer.h"
#include "EsockPlatSecTest.h"
//-- all test steps logging will be dumped to the file with this name.
//_LIT(KTestStep_Report,"EsockPlatSecStepReport");

//---------------------------------------------------------------------------------------------------------

_LIT(KNewLine, "\n");



/**
*  Base calss - Esock Plat Sec test step.
*/
class CEsockPlatSecTestStep: public CTestStep
    {
    public:
        CEsockPlatSecTestStep(CEsockPlatSecTestServer *apTestServer = NULL);
       ~CEsockPlatSecTestStep();

    protected:

        CEsockPlatSecTestServer *ipTestServer; //< pointer to the server which owns this test step

    };



/**
Helper class for starting a process possibly with its security attributes overriding
*/
class CHelperProcess : public CBase
{

public:

    CHelperProcess(CTestExecuteLogger &aLogger);
   ~CHelperProcess(void);

    /** set the name of the helper process
    @param  aExeName name of the process .exe file
    */
    void    SetName(const TDesC& aExeName)              {iImageName.Copy(aExeName);}

    /** override helper process capabilities
    @param  aCapsSet specifies new capabilities
    */
    void    SetCaps(const TCapabilitySet &aCapsSet)     {iCaps = aCapsSet; ifCapsValid = ETrue;}

    /** override SID of the helper process*/
    void    SetSID(TUint32 aSID)                        {iSecureId = aSID; ifSidValid = ETrue;}

    /** override VID of the helper process*/
    void    SetVID(TUint32 aVID)                        {iVendorId = aVID; ifVidValid = ETrue;}

    /** remove helper process PlatSec attributes overriding */
    void    ResetSecInfo(void)                          {ifCapsValid = ifSidValid = ifVidValid = EFalse;}

    TInt    Create(const TDesC& aMsgQueueName);
    TInt    Start(void);

    /** @return Handle of the helper process */
    RProcess& Handle(void)  {return iProcess;}

protected:

    CHelperProcess();
    CHelperProcess& operator=(const CHelperProcess&);

protected:

    CTestExecuteLogger &iLogger;   //< ref. to the logger from the test step
    RProcess            iProcess;   //< process handle
    TName               iImageName; //< name of the .exe file

    TCapabilitySet      iCaps;		//< Caps set that will overrige default process's one
    TUint32	            iSecureId;  //< new SID
    TUint32             iVendorId;  //< new VID

    //-- bit flags indicating that some of the Plat Sec attributes for the process have been overridden
    TBool       ifCapsValid:1;
    TBool       ifSidValid:1;
    TBool       ifVidValid:1;


};



#endif // __TE_ESOCK_PLATSEC_STEP_H__






