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
//

#include "tsu_3gplibrary.h"
#include "tsu3gplibraryapi.h"
#include "tsu3gplibraryapinegative.h"
#include "tsu_3gplibrary_composer_api.h"
#include "tsu_3gplibrary_parse_compose.h"
#include "tsu_3gplibrary_parse_and_check.h"

_LIT(KServerName, "tsu_3gplibrary");

C3GPLibraryServer* C3GPLibraryServer::NewL()
	{
	C3GPLibraryServer* server = new (ELeave) C3GPLibraryServer();
	CleanupStack::PushL(server);	
	server-> ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched = NULL;
	sched = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(sched);
	C3GPLibraryServer* server = NULL;
	// Create the C3GPLibraryServer derived server
	TRAPD(err,server = C3GPLibraryServer::NewL());
	if(!err)
		{
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}
	
GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }
   
CTestStep* C3GPLibraryServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	// C++ wrapper parser API tests
	if(aStepName == KTestCase_0001)
		{
		testStep = new C3GPLibParser_0001();
		}
	else if(aStepName == KTestCase_0002)
		{
		testStep = new C3GPLibParser_0002();
		}
	else if(aStepName == KTestCase_0003)
		{
		testStep = new C3GPLibParser_0003();
		}
	else if(aStepName == KTestCase_0004)
		{
		testStep = new C3GPLibParser_0004();
		}
	else if(aStepName == KTestCase_0005)
		{
		testStep = new C3GPLibParser_0005();
		}
	else if(aStepName == KTestCase_0006)
		{
		testStep = new C3GPLibParser_0006();
		}
	else if(aStepName == KTestCase_0007)
		{
		testStep = new C3GPLibParser_0007();
		}
	else if(aStepName == KTestCase_0008)
		{
		testStep = new C3GPLibParser_0008();
		}
	else if(aStepName == KTestCase_0011)
		{
		testStep = new C3GPLibParser_0011();
		}
	else if(aStepName == KTestCase_0014)
		{
		testStep = new C3GPLibParser_0014();
		}
	else if(aStepName == KTestCase_0015)
		{
		testStep = new C3GPLibParser_0015();
		}
	else if(aStepName == KTestCase_0016)
		{
		testStep = new C3GPLibParser_0016();
		}
	else if(aStepName == KTestCase_0017)
		{
		testStep = new C3GPLibParser_0017();
		}
	else if(aStepName == KTestCase_0021)
		{
		testStep = new C3GPLibParser_0021();
		}
	else if(aStepName == KTestCase_0023)
		{
		testStep = new C3GPLibParser_0023();
		}
	else if(aStepName == KTestCase_0027)
		{
		testStep = new C3GPLibParser_0027();
		}
	else if(aStepName == KTestCase_0031)
		{
		testStep = new C3GPLibParser_0031();
		}
	else if(aStepName == KTestCase_0032)
		{
		testStep = new C3GPLibParser_0032();
		}
	else if(aStepName == KTestCase_0033)
		{
		testStep = new C3GPLibParser_0033();
		}
	else if(aStepName == KTestCase_0034)
		{
		testStep = new C3GPLibParser_0034();
		}
	else if(aStepName == KTestCase_0035)
		{
		testStep = new C3GPLibParser_0035();
		}
	else if(aStepName == KTestCase_0036)
		{
		testStep = new C3GPLibParser_0036();
		}
	else if(aStepName == KTestCase_0037)
		{
		testStep = new C3GPLibParser_0037();
		}
	else if(aStepName == KTestCase_0038)
		{
		testStep = new C3GPLibParser_0038();
		}
	else if(aStepName == KTestCase_0039)
		{
		testStep = new C3GPLibParser_0039();
		}
	else if(aStepName == KTestCase_0041)
		{
		testStep = new C3GPLibParser_0041();
		}
	else if(aStepName == KTestCase_0043)
		{
		testStep = new C3GPLibParser_0043();
		}
	else if(aStepName == KTestCase_0045)
		{
		testStep = new C3GPLibParser_0045();
		}
	else if(aStepName == KTestCase_0047)
		{
		testStep = new C3GPLibParser_0047();
		}
	else if(aStepName == KTestCase_0101)
		{
		testStep = new C3GPLibParser_0101();
		}
	else if(aStepName == KTestCase_0102)
		{
		testStep = new C3GPLibParser_0102();
		}
	else if(aStepName == KTestCase_0104)
		{
		testStep = new C3GPLibParser_0104();
		}
	else if(aStepName == KTestCase_0106)
		{
		testStep = new C3GPLibParser_0106();
		}
	else if(aStepName == KTestCase_0107)
		{
		testStep = new C3GPLibParser_0107();
		}
	else if(aStepName == KTestCase_0108)
		{
		testStep = new C3GPLibParser_0108();
		}
	else if(aStepName == KTestCase_0109)
		{
		testStep = new C3GPLibParser_0109();
		}
	else if(aStepName == KTestCase_0110)
		{
		testStep = new C3GPLibParser_0110();
		}
	else if(aStepName == KTestCase_0111)
		{
		testStep = new C3GPLibParser_0111();
		}
	else if(aStepName == KTestCase_0119)
		{
		testStep = new C3GPLibParser_0119();
		}
	else if(aStepName == KTestCase_0120)
		{
		testStep = new C3GPLibParser_0120();
		}
	else if(aStepName == KTestCase_0126)
		{
		testStep = new C3GPLibParser_0126();
		}
	else if(aStepName == KTestCase_0127)
		{
		testStep = new C3GPLibParser_0127();
		}
	else if(aStepName == KTestCase_0130)
		{
		testStep = new C3GPLibParser_0130();
		}
	else if(aStepName == KTestCase_0131)
		{
		testStep = new C3GPLibParser_0131();
		}
	else if(aStepName == KTestCase_0135)
		{
		testStep = new C3GPLibParser_0135();
		}
	else if(aStepName == KTestCase_0136)
		{
		testStep = new C3GPLibParser_0136();
		}
	else if(aStepName == KTestCase_0137)
		{
		testStep = new C3GPLibParser_0137();
		}
	else if(aStepName == KTestCase_0138)
		{
		testStep = new C3GPLibParser_0138();
		}
	else if(aStepName == KTestCase_0139)
		{
		testStep = new C3GPLibParser_0139();
		}
	else if (aStepName == K3GPSingleDataSourceTest)
		{		
		testStep = new C3GPSingleDataSourceTest();
		}
	else if (aStepName == KTestUserDataAtomRetrieval)	
		{		
		testStep = new C3GPUserDataAtomRetrieval();
		}

	// Composer: API Tests
	else if (aStepName == K3GPComposeFilename)
		{
		testStep = new C3GPLibComposeFilename();
		}
	else if(aStepName == K3GPComposeWithoutOpen)
		{
		testStep = new C3GPLibComposeWithoutOpen();
		}	
	else if(aStepName == K3GPComposeFileWithFileFormatCheck)
		{
		testStep = new C3GPLibComposeFileWithFileFormatCheck();
		}
	else if (aStepName == K3GPComposeFrameDependency)
		{
		testStep = new C3GPLibComposeFrameDependency();
		}
	else if (aStepName == K3GPComposeVideoProperties)
		{
		testStep = new C3GPLibComposeVideoProperties();
		}
	else if (aStepName == K3GPComposeWithSpecificBufferSize)
		{
		testStep = new C3GPLibComposeWithSpecificBufferSize();
		}
	else if (aStepName == K3GPComposeUserData)
		{
		testStep = new C3GPLibComposeUserData();
		}
	else if (aStepName == K3GPComposeWithFlag)
		{
		testStep = new C3GPLibComposeWithFlag();
		}
	else if (aStepName == K3GPComposeAudioProperties)
		{
		testStep = new C3GPLibComposeAudioProperties();
		}
	else if (aStepName == K3GPComposeWithNoAudioVideo)
		{
		testStep = new C3GPLibComposeWithNoAudioVideo();
		}
	else if (aStepName == K3GPComposeWithNoAudioVideo)
		{
		testStep = new C3GPLibComposeWithNoAudioVideo();
		}
	else if (aStepName == K3GPComposeOpenAgain)
		{
		testStep = new C3GPLibComposeOpenAgain();
		}
	else if (aStepName == K3GPComposeAudioFrames)
		{
		testStep = new C3GPLibComposeAudioFrames();
		}
	else if (aStepName == K3GPComposeVideoFrame)
		{
		testStep = new C3GPLibComposeVideoFrame();
		}
	else if (aStepName == K3GPComposeOpenReadOnlyFile)
		{
		testStep = new C3GPLibComposeOpenReadOnlyFile();
		}
	else if (aStepName == K3GPComposeOpenedFile)
		{
		testStep = new C3GPLibComposeOpenedFile();
		}
	else if (aStepName == K3GPComposeMultiComposeOnSameFile)
		{
		testStep = new C3GPLibComposeMultiComposeOnSameFile();
		}
	else if (aStepName == K3GPComposeMultiComposers)
		{
		testStep = new C3GPLibComposeMultiComposers();
		}
	else if (aStepName == K3GPComposeCloseComposerWithoutComplete)
		{
		testStep = new C3GPLibComposeCloseComposerWithoutComplete();
		}
	else if (aStepName == K3GPComposePanic)
		{
		testStep = new C3GPLibComposePanic();
		}		
	else if (aStepName == K3GPComposeLargeFile)
		{
		testStep = new C3GPLibComposeLargeFile();
		}
	else if (aStepName == K3GPComposeLargeFileUserData)
		{
		testStep = new C3GPLibComposeLargeFileUserData();
		}
	else if (aStepName == K3GPComposeLargeFile32bitAPI)
		{
		testStep = new C3GPLibComposeLargeFile32bitAPI();
		}
	else if (aStepName == K3GPComposeLargeFileDiskFull)
		{
		testStep = new C3GPLibComposeLargeFileDiskFull();
		}
	// Composer: file format test
	else if (aStepName == K3GPParseComposeFile)
		{
		testStep = new C3GPLibParseComposeFile();
		}
	// AVC profile test
	else if (aStepName == K3GPComposeFileWithAvcProfileCheck)
	    {
	    testStep = new C3GPLibComposeFileWithAvcProfileCheck();
	    }
	// Main & High profile parse
	else if (aStepName == K3GPLibParseAndCheckTypes)
	    {
	    testStep = new C3GPLibParseAndCheckTypes();
	    }

	return testStep;
	}
