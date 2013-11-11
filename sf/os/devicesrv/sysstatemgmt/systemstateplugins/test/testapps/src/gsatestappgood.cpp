// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code
*/

#include <e32property.h>
#include <e32debug.h>
#include "gsatestappgood.h"
#include "gtestapps.h"
#include "gsatestprocgoodsession.h"
#include <apacmdln.h>

static void PublishPriority()
	{
	const TInt KMaxCommandLength = 256;
    CApaCommandLine* apaCmdLine = NULL;
    CApaCommandLine::GetCommandLineFromProcessEnvironment(apaCmdLine);
	TBuf8<KMaxCommandLength> commandLine(apaCmdLine->TailEnd());
	TLex8 flagLex(commandLine);
	TInt priority = 0;
	while(!flagLex.Eos())
		{
		TPtrC8 token(flagLex.NextToken());
		TLex8 temp(token);
		temp.Val(priority);
		}
	RDebug::Printf("Priority of this application is : %d",priority);
	delete apaCmdLine;
	}

static void MainL()
    {
    PublishPriority();
    // Try and connect to ProcGoodSession. If it is successful, set a property. 
    // Else dont set the property. 
    RTestProcGoodSession session;
    TInt err = session.Connect();
    if(KErrNone == err)
        {
        _LIT_SECURITY_POLICY_PASS(KTestReadPolicy);
        _LIT_SECURITY_POLICY_S0(KTestWritePolicy, RProcess().SecureId());  
        
        TInt err = RProperty::Define(KTestAppUidCategory, KTestPropertyKey, RProperty::EInt, KTestReadPolicy, KTestWritePolicy);
        err = RProperty::Set(KTestAppUidCategory, KTestPropertyKey, KPropertySetValue);
        }
    
    if (User::CommandLineLength() > 0)
        {
        TBuf<KTestCommandLineMaxLength> commandLine;
        if(User::CommandLineLength() > commandLine.MaxLength())
            User::Leave(KErrTooBig);
        
        User::CommandLine(commandLine);
        
        TLex flagLex(commandLine);
        while(!flagLex.Eos())
            {
            TPtrC token(flagLex.NextToken());
            if(token == KLaunchProcessAndWait)
                {
                CActiveScheduler* sched = new(ELeave) CActiveScheduler;
                CActiveScheduler::Install(sched);
                
                RDebug::Printf("Rendezvous this application");
                RProcess::Rendezvous(KErrNone);
                
                RSemaphore sem;
                TInt err = sem.OpenGlobal(KStartTestProcGoodSem);
                if(err == KErrNone)
                    {
                    sem.Signal();
                    sem.Close();
                    }

                // Start the active scheduler, so that the process would be running until the test case
                // sets up the monitoring and would kill the process explicitly.
                CActiveScheduler::Start();
                delete sched;
                }
            }
        }    
    }


/**
Standard DLL entry point function.

@return KErrNone
*/
TInt E32Main()
	{
    __UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt r=KErrNoMemory;
    if (cleanup)
        {
        TRAP(r, MainL());
        delete cleanup;
        }

    __UHEAP_MARKEND;
    return r;
	}
