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
// Test step code for the ESOCK PlatSec tests
// 
//

/**
 @file
 @internalAll PlatSec test code for the ESOCK
*/

#include <e32base.h>
#include "EsockPlatSecTest.h"
#include "TE_EsockPlatSecStep.h"


// constructor
CEsockPlatSecTestStep::CEsockPlatSecTestStep(CEsockPlatSecTestServer *apTestServer/* = NULL*/)
                     :ipTestServer(apTestServer)
    {

    }

// destructor
CEsockPlatSecTestStep::~CEsockPlatSecTestStep()
    {
    }



//---------------------------------------------------------------------------


CHelperProcess::CHelperProcess(CTestExecuteLogger &aLogger)
               : CBase(), iLogger(aLogger)
{
    ResetSecInfo();
}


CHelperProcess::~CHelperProcess()
{
    iProcess.Close();
}


/**
Create the process. Probably with overrodden PlatSec attributes.
@return Standard error code.
*/
TInt CHelperProcess::Create(const TDesC& aMsgQueueName)
{
     TInt   nRes = KErrNone;

     TName  tmpBuf;
     TName  tmpBuf1;
     TName  cmdLine; //-- command line for SetCap utility

     TBuf<32>  capsBuf;
     TBuf<32>  sidBuf;
     TBuf<32>  vidBuf;

     tmpBuf.Copy(_L("Creating process: "));
     tmpBuf.Append(iImageName);
     iLogger.Write(tmpBuf);

     cmdLine.Copy(iImageName);

     //-- find out if SID, VID or capabilities were specified and if yes, run SetCap tool for the
     //-- helper process. It will create the new .exe in c:\sys\bin
     nRes = KErrNone;
     TBool fSecAttrOvr = ifCapsValid || ifSidValid || ifVidValid;

     if(fSecAttrOvr)
     {
        tmpBuf.Copy(_L("Trying to override security attributes: "));

        if(ifCapsValid)
        {//-- process's capabilities have been overridden
            SCapabilitySet &capsSet = (SCapabilitySet&)iCaps;

            tmpBuf1.Copy(_L("Caps:"));
            //capsBuf.Format(_L("%08x%08x "),capsSet[0],capsSet[1]);
            capsBuf.Format(_L("%08x "),capsSet[0]);

            tmpBuf1.Append(capsBuf);
            tmpBuf.Append(tmpBuf1);

            cmdLine.Append(_L(" "));
            cmdLine.Append(capsBuf);
        }

        if(ifSidValid)
        {//-- process's SID has been overridden
            tmpBuf1.Copy(_L(" SID:"));
            sidBuf.Format(_L("%08x "),iSecureId);

            tmpBuf1.Append(sidBuf);
            tmpBuf.Append(tmpBuf1);

            cmdLine.Append(_L(" -SID "));
            cmdLine.Append(sidBuf);

        }

        if(ifVidValid)
        {//-- process's SID has been overridden
            tmpBuf1.Copy(_L(" VID:"));
            vidBuf.Format(_L("%08x "),iVendorId);

            tmpBuf1.Append(vidBuf);
            tmpBuf.Append(tmpBuf1);

            cmdLine.Append(_L(" -VID "));
            cmdLine.Append(vidBuf);

        }

        iLogger.Write(tmpBuf);

        //-- try to call SetCap for the specified process
        tmpBuf.Copy(_L("Calling "));
        tmpBuf.Append(_L(" "));
        tmpBuf.Append(KSetCapProcName);
        tmpBuf.Append(_L(" "));
        tmpBuf.Append(cmdLine);

        iLogger.Write(tmpBuf);

        //--
        RProcess        procSetCap;
        TRequestStatus  rqStat;

        //-- run  "SetCap.exe" with specified command line
        nRes = procSetCap.Create(KSetCapProcName, cmdLine);
        if(nRes != KErrNone)
        {
            iLogger.WriteFormat(_L("Error creating SetCap.exe process! code: %d"), nRes);
            return nRes;
        }

        //-- resume the process and wait for its completion
        procSetCap.Logon(rqStat);
        procSetCap.Resume();
        User::WaitForRequest(rqStat); // sorry, no timeout implemented yet...

        nRes = rqStat.Int();
        if(nRes != KErrNone)
        {
            iLogger.WriteFormat(_L("SetCap.exe finished with error! code: %d"), nRes);
        }
        else
        iLogger.Write(_L("OK."));

        procSetCap.Close();

     }

    if(nRes != KErrNone)
        return nRes;

     //-- try to create the helper process.
     //-- if SetCap has processed it already, it will be somewhere in c:\sys\bin

     cmdLine.Zero();

     if(fSecAttrOvr)
     {//-- PlatSec attributes for the process have been overridden, and it has been moved to c:\sys\bin
        cmdLine.Copy(_L("c:\\sys\\bin\\"));
     }

     cmdLine.Append(iImageName);
     nRes = iProcess.Create(cmdLine, aMsgQueueName);

     if(nRes != KErrNone)
     {
       TPtrC ptr(iImageName);
       iLogger.WriteFormat(_L("Error creating %S process! code: %d"), &ptr, nRes);
     }


     return nRes;
}

/**
Resume the helper process and wait for Rendezvous

@return the result code of the helper process startup
*/
TInt    CHelperProcess::Start(void)
{
    TRequestStatus  rqStat;

    iProcess.Resume();
    iProcess.Rendezvous(rqStat);
    User::WaitForRequest(rqStat); //sorry, timeout isn't implemented yet...

    return rqStat.Int();
}

//---------------------------------------------------------------------------












