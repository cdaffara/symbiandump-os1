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

#include "tdirectgdi_server.h"
#include "tbitblt.h"
#include "tbitbltmasked.h"
#include "tdrawbitmap.h"
#include "tdrawbitmapmasked.h"
#include "tgeneraldrawing.h"
#include "tclear.h"
#include "tdrawshapes.h"
#include "tbrushandpen.h"
#include "tclipping.h"
#include "tcopyrect.h"
#include "tdrawtext.h"
#include "tdirectgdidriver.h"
#include "tdirectgdicontext.h"
#include "tpanictests.h"
#include "tdirectgdicontexttarget.h"
#include "tmultithread.h"
#include "tglyphimagecache.h"
#include "tvgimagecache.h"
#include "tdirectgdiresource.h"
#include "tdirectgdioom.h"
#include "tinterleaving.h"
#include "tdirectgdinoadapter.h"
#include "textendedbitmap.h"
#include <graphics/fbsdefs.h>

_LIT(KServerName,"tdirectgdi_server");


CTDirectGdiServer* CTDirectGdiServer::NewL()
	{
	CTDirectGdiServer* server = new (ELeave) CTDirectGdiServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{ 	
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTDirectGdiServer* server = NULL;
	
	// Create the CTestServer derived server
	server = CTDirectGdiServer::NewL();
	// startup fbs server
	FbsStartup();
	// Sync with the client and enter the active scheduler
	RProcess::Rendezvous(KErrNone);
	sched->Start();

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
		RDebug::Print(_L("TDirectGdiServer::MainL - Error: %d"), err);
	   	User::Panic(KServerName,err);
	    }
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }


CTestStep* CTDirectGdiServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if (aStepName == KTDirectGdiBitBltStep)
		{
		testStep = new CTBitBlt();
		}
	else if(aStepName == KTDirectGdiBitBltMaskedStep)
		{
		testStep = new CTBitBltMasked();
		}
	else if(aStepName == KTDirectGdiDrawBitmapStep)
		{
		testStep = new CTDrawBitmap();
		}
	else if(aStepName == KTDirectGdiDrawBitmapMaskedStep)
		{
		testStep = new CTDrawBitmapMasked();
		}
	else if(aStepName == KTDirectGdiDriverStep)
		{
		testStep = new CTDirectGdiDriver();
		}
	else if(aStepName == KTDirectGdiContextStep)
		{
		testStep = new CTDirectGdiContext();
		}
	else if(aStepName == KTDirectGdiGeneralDrawingStep)
		{
		testStep = new CTGeneralDrawing();
		}
	else if(aStepName == KTDirectGdiClearStep)
		{
		testStep = new CTClear();
		}
	else if(aStepName == KTDirectGdiDrawShapeStep)
		{
		testStep = new CTDrawShapes();
		}
	else if(aStepName == KTDirectGdiBrushAndPenStep)
		{
		testStep = new CTBrushAndPen();
		}		
	else if(aStepName == KTDirectGdiDrawTextStep)
		{
		testStep = new CTDrawText();
		}
	else if(aStepName == KTDirectGdiClippingStep)
		{
		testStep = new CTClipping();
		}
	else if(aStepName == KTDirectGdiCopyRectStep)
		{
		testStep = new CTCopyRect();
		}
	else if(aStepName == KTDirectGdiPanicTestsStep)
		{
		testStep = new CTPanicTests();
		}
	else if(aStepName == KTDirectGdiContextTargetStep)
		{
		testStep = new CTDirectGdiContextTarget();
		}
	else if(aStepName == KTMultiThreadStep)
		{
		testStep = new CTMultiThread();
		}
	else if(aStepName == KTGlyphImageCacheStep)
		{
		testStep = new CTGlyphImageCache();
		}
	else if(aStepName == KTDirectGdiVgImageCacheStep)
		{
		testStep = new CTVgImageCache();
		}
	else if(aStepName == KTDirectGdiResourceStep)
		{
		testStep = new CTDirectGdiResource();
		}
	else if(aStepName == KTDirectGdiOom)
		{
		testStep = new CTDirectGdiOom();
		}
	else if(aStepName == KTDirectGdiInterleavingStep)
		{
		testStep = new CTInterleaving();
		}
	else if(aStepName == KTDirectGdiNoAdapter)
		{
		testStep = new CTNoAdapter();
		}
	else if(aStepName == KTDirectGdiExtendedBitmapStep)
		{
		testStep = new CTExtendedBitmap();
		}
	return testStep;
	}

