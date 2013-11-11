// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TBitgdiServer.h"
#include "TAUTO.H"
#include "tbase.h"
#include "TClip.h"
#include "tparam.h"
#include "tswitch.h"
#include "TAccelerator.h"
#include "TDefect.h"
#include "TDefect2.h"
#include "tbitblt.h"
#include "TBitgdiScaling.h"
#include "TAlphaBlend.h"
#include "tfontselect.h"
#include "TFont.h"
#include "toutlineandshadowfonts.h"
#include "textendedbitmap.h"
#include "tmultiplescreens.h"

 

_LIT(KServerName,"TBitgdiServer");

CTBitgdiServer* CTBitgdiServer::NewL()
	{
	CTBitgdiServer * server = new (ELeave) CTBitgdiServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


LOCAL_C void MainL()
	{
 	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTBitgdiServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTBitgdiServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

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
		RDebug::Print(_L("TBitgdiServer::MainL - Error: %d"), err);
	   	User::Panic(KServerName,err);
	    }
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }



CTestStep* CTBitgdiServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KTAutoStep)
		{
		testStep = new CTAutoStep();
		}
	else if(aStepName == KTGdiStep)
		{
		testStep = new CTGdiStep();
		}
	else if(aStepName == KTClipStep)
		{
		testStep = new CTClipStep();
		}
	else if(aStepName == KTClip2Step)
		{
		testStep = new CTClip2Step();
		}
	else if(aStepName == KTParamStep)
		{
		testStep = new CTParamStep();
		}
	else if(aStepName == KTSwitchStep)
		{
		testStep = new CTSwitchStep();
		}
	else if(aStepName == KTAcceleratorStep)
		{
		testStep = new CTAcceleratorStep();
		}
	else if(aStepName == KTDefectStep)
		{
		testStep = new CTDefectStep();
		}
	else if(aStepName == KTDefect2Step)
		{
		testStep = new CTDefect2Step();
		}
	else if(aStepName == KTBitBltStep)
		{
		testStep = new CTBitBltStep();
		}
	else if(aStepName == KTBitgdiScalingStep)
		{
		testStep = new CTBitgdiScalingStep();
		}
	else if(aStepName == KTAlphaBlendingStep)
		{
		testStep = new CTAlphaBlendingStep();
		}
	else if(aStepName == KTFontSelectStep)
		{
		testStep = new CTFontSelectStep();
		}		
	else if(aStepName == KTFontStep)
		{
		testStep = new CTFontStep();
		}		
	else if(aStepName == KTOutlineAndShadowFontsStep)
		{
		testStep = new CTOutlineAndShadowFontsStep();
		}
	else if(aStepName == KTExtendedBitmapStep)
		{
		testStep = new CTExtendedBitmapStep();
		}
	else if(aStepName == KTExtendedBitmapNegativeStep)
		{
		testStep = new CTExtendedBitmapNegativeStep();
		}
	else if(aStepName == KTMultipleScreensStep)
		{
		testStep = new CTMultipleScreensStep();
		}
	TBuf<64> log;
	_LIT(KBitgdi,"BITGDI Test %S ");
	_LIT(KSucess,"created sucessfully.");
	_LIT(KFail,"failed to create.");
	log.Format(KBitgdi,&aStepName);
	if (testStep)
		log.Append(KSucess);
	else
		{
		log.Append(KFail);
		INFO_PRINTF1(log);
		}
	RDebug::Print(log);
	return testStep;
	}

