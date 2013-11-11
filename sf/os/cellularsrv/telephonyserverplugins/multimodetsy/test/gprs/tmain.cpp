// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Main test file for the automated testing
// usage:   t_gprs --keepgoing   for automated testing 
// 
//

/**
 @file
*/

#include "tmain.h"

#include <commsdattypesv1_1.h>

#include <autotest.h>
#include <plpsess.h>        // RRemoteLink

GLDEF_D RTest gTest(_L("GPRS Test Code"));

GLDEF_D     RPhone          gPhone;
GLDEF_D     RTelServer      gServer;
//GLREF_D     RConnection		conn;
//GLDEF_D     RGenericAgent	gGenericAgent;
GLDEF_D     RSocketServ		gSocketServer;
GLDEF_D		RPacketService	gPacketService;
GLDEF_D		RPacketContext	gPacketContext[20];
// Inifile object.
GLDEF_D		CConfigParams* testConfigParams;

GLDEF_D		TPhoneBrand GPhoneBrand;

LOCAL_C void InitGlobals()
/**
 * Initialise global variables.
 */
    {
#ifdef __WINS__
    User::After(5*KOneSecond);      // allow emulator time to start up
#endif
    gTest.Start(_L("Load TSY"));
    gTest(gServer.Connect()==KErrNone);
    gTest(gServer.LoadPhoneModule(KtsyName)==KErrNone);
    gTest.Next(_L("Contact the Phone"));
    // Run the tests    
  //  gTest(gPhone .Open(gServer,KPhoneName)== KErrNone);
	gTest(gPhone.Open(gServer, KPhoneName)==KErrNone);
	gTest(gPhone .Initialise()==KErrNone);

	gTest(gSocketServer.Connect() == KErrNone);
//	gTest(gGenericAgent.Open() == KErrNone);
	gTest(conn.Open(gSocketServer, KAfInet) == KErrNone);

	// Open the ini file with the web settings.
	TRAPD(err,testConfigParams = CConfigParams::NewL(KTGPRSConfigFile()));
	gTest(err == KErrNone);
	const TPtrC phoneBrand = testConfigParams->FindAlphaVar(KTestCfgPhoneMake,KNullDesC);
	if(phoneBrand == KTestCfgPhoneMotorola)
		GPhoneBrand = EMotorola;
	else if(phoneBrand == KTestCfgPhoneEricsson)
		GPhoneBrand = EEricsson;
	else 
		{
    gTest.Printf(_L("There is no phone specified in ini file or the phone is not supported"));
		User::Panic(_L("There is no phone specified in ini file or the phone is not supported."), KErrNotSupported);
		}
	}


LOCAL_C void DestroyGlobals()
/**
 * Free global variables
 */
    {
    gPhone .Close();
    gServer.Close();
	
//	gGenericAgent.Close();
	conn.Close();
	gSocketServer.Close();
	delete testConfigParams;
	testConfigParams = NULL;
    }


LOCAL_C void RunTestsL()
/**
 * Run all the tests
 */
    {
    InitGlobals();
	AutoTestExecute();
    DestroyGlobals();
    }


#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define PDD_NAME2 _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif


int CommInit()
/**
 * This function does the actual loading of the drivers needed.
 */
    {
    RFs fs;
    TInt err=fs.Connect();  // make sure the FileServer is alive (only needed for WINS test code)
    fs.Close();

    gTest.Printf(_L("CommInit: Loading PDD ") );
    gTest.Printf(PDD_NAME);
    gTest.Printf(_L("\n"));
    err=User::LoadPhysicalDevice(PDD_NAME);
    if (err!=KErrNone && err!=KErrAlreadyExists)
        return(err);

    gTest.Printf(_L("CommInit: Loading LDD ") );
    gTest.Printf(LDD_NAME);
    gTest.Printf(_L("\n"));
    err=User::LoadLogicalDevice(LDD_NAME );
    if (err!=KErrNone && err!=KErrAlreadyExists)
        return(err);

    gTest.Printf(_L("CommInit: Starting C32 \n") );
 	// When bootstrapping C32 we have to avoid the PhBkSyncServer being started, since
 	// it needs a different CommDB
 	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	err = StartC32WithCMISuppressions(KPhbkSyncCMI);
    return (err == KErrNone || err == KErrAlreadyExists) ? KErrNone : err;
    }


TInt E32Main()
/**
 * @test Main program tsmsmsg
 */
    {

    CTrapCleanup* cleanup = CTrapCleanup::New();
    CActiveScheduler* theActiveScheduler = new CActiveScheduler();
    CActiveScheduler::Install(theActiveScheduler);

    __UHEAP_MARK;

    gTest.SetLogged(EFalse);    // to avoid garbage on Com port
    gTest.Title();
    TInt ret = CommInit();
    if(ret != KErrNone)
        {
        gTest.Printf(_L("ERROR: Leave %d\n"), ret);
        }
    else
        {
        gTest.Start(_L("Starting tests..."));
        TRAPD(err,RunTestsL());
        if (err!=KErrNone)
            gTest.Printf(_L("ERROR: Leave %d\n"),err);
         

        }

    gTest.End();
    gTest.Close();

    __UHEAP_MARKEND;

    delete cleanup;
    delete theActiveScheduler;
    return KErrNone;
    }



/**
 * for the EXEDLL targettype
 */

EXPORT_C TInt WinsMain()
    {
#if defined(__WINS__)
    E32Main();
#endif
    return KErrNone;
    }



// EOF - TSMMAIN.CPP



