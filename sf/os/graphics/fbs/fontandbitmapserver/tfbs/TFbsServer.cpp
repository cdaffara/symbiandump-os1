// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TFbsServer.h"
#include "TALLOC.H"
#include "TBitmap.h"
#include "TCLEAN.H"
#include "TFBS.H"
#include "tcompressed.h"
#include "TRalc.h"
#include "TFBSDefect.h"
#include "TStreamIdCache.h"
#include "TSecureFBS.h"
#include "trfile.h"
#include "TGetAllBitmapsCapability.h"
#include "tipctest.h"
#include "textendedbitmap.h"
#include "textendedbitmapnegative.h"
#include "textendedbitmaplegacy.h"
#include "textendedbitmappanic.h"
#include "twdp.h"
#include "tfonttableandglyph.h"
#include "tfbsglyphdata.h"
#include "tfbsglyphdatapanic.h"
#include "tfbsglyphdataoutlineshadow.h"
#include "tglyphatlas.h"
#include "tfbsoogm.h"

/* Path to the script

z:\GraphicsTest\fbstest.script

*/

CTFbsServer* CTFbsServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
	{
	CTFbsServer * server = new (ELeave) CTFbsServer();
	CleanupStack::PushL(server);
	
	// Get server name from process so we can use SetCap to change the capabilites and use this server with a different filename
	RProcess handle = RProcess();
	TParsePtrC serverName(handle.FileName());
	
	// CServer base class call
	server->StartL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}


LOCAL_C void MainL()
//
// Secure variant
// Much simpler, uses the new Rendezvous() call to sync with the client
//
	{
 	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTFbsServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTFbsServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/** @return - Standard Epoc error code on process exit
    Secure variant only
    Process entry point. Called by client using RProcess API
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	if (err)
	    {
	    RProcess handle = RProcess();
		TParsePtrC serverName(handle.FileName());
		RDebug::Print(_L("CTFbsServer::MainL - Error: %d"), err);
	   	User::Panic(serverName.Name(),err);
	    }
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }

CTestStep* CTFbsServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTAllocStep)
		{
		testStep = new CTAllocStep();
		}
	else if(aStepName == KTBitmapStep)
		{
		testStep = new CTBitmapStep();
		}
	else if(aStepName == KTCleanStep)
		{
		testStep = new CTCleanStep();
		}
	else if(aStepName == KTFbsStep)
		{
		testStep = new CTFbsStep();
		}
	else if(aStepName == KTCompressedStep)
		{
		testStep = new CTCompressedStep();
		}
	else if(aStepName == KTRalcStep)
		{
		testStep = new CTRalcStep();
		}
	else if(aStepName == KTFbsDefectStep)
		{
		testStep = new CTFbsDefectStep();
		}
	else if(aStepName == KTStreamIdCacheStep)
		{
		testStep = new CTStreamIdCacheStep();
		}
	else if(aStepName == KTFbsSecureStep)
		{
		testStep = new CTFbsSecureStep();
		}
	else if(aStepName == KTFileStep)
		{
		testStep = new CTFileStep();
		}
	else if(aStepName == KTGetAllBitmapsCapabilityStep)
		{
		testStep = new CTGetAllBitmapsCapabilityStep();
		}	
	else if(aStepName == KTIPCTestStep)
		{
		testStep = new CTIPCTestStep();
		}
	else if(aStepName == KTExtendedBitmapStep)
		{
		testStep = new CTExtendedBitmapStep();
		}
	else if(aStepName == KTExtendedBitmapNegativeStep)
		{
		testStep = new CTExtendedBitmapNegativeStep();
		}
	else if(aStepName == KTExtendedBitmapLegacyStep)
		{
		testStep = new CTExtendedBitmapLegacyStep();
		}
	else if(aStepName == KTExtendedBitmapPanicStep)
		{
		testStep = new CTExtendedBitmapPanicStep();
		}
	else if(aStepName == KTWDPStep)
		{
		testStep = new CTWDPStep();
		}
   	else if(aStepName == KTFontTableAndGlyphStep)
        {
        testStep = new CTFontAndGlyphStep();
        }
	else if(aStepName == KTFbsGlyphDataStep)
		{
		testStep = new CTFbsGlyphDataStep();
		}
    else if(aStepName == KTFbsGlyphDataPanicStep)
        {
        testStep = new CTFbsGlyphDataPanicStep();
        }
    else if(aStepName == KTFbsGlyphDataOutlineAndShadowStep)
        {
        testStep = new CTFbsGlyphDataOutlineAndShadowStep();
        }
    else if(aStepName == KTGlyphAtlasStep)
    	{
		testStep = new CTGlyphAtlasStep();
    	}
    else if(aStepName == KTFbsOogmStep)
        {
        testStep = new CTFbsOogmStep();
        }

	return testStep;
	}
