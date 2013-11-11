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
// Definitions of entry points for EtelRecorder session replayer.
// 
//

/**
 @file
*/

#include "replaymain.h"
#include <bacline.h>
#include <e32cons.h>
#include "replayfileparser.h"
#include "replaymodel.h"
	

_LIT(KTxtReplaySession,"ReplaySession");
_LIT(KFormatFailed,"failed: leave code=%d");


// public
LOCAL_D CConsoleBase* console; 
LOCAL_C void CallExeL();

CConsoleBase* iConsole;

// private
LOCAL_C void DoCallExeL(CConsoleBase* aConsole); 

using namespace ReplaySession;

void ReplaySession::Panic(TIniPanic aPanic)
	{
	_LIT(KIniData,"ReplaySess");
	User::Panic(KIniData,aPanic);
	}

GLDEF_C TInt E32Main() 
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TRAPD(error,CallExeL());
	__ASSERT_ALWAYS(!error,User::Panic(KTxtReplaySession,error));
	delete cleanup;
	__UHEAP_MARKEND;
	return 0; // and return
    }


LOCAL_C void CallExeL() // initialize and call example code under cleanup stack
    {   
	console=Console::NewL(KTxtReplaySession,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	
	// Create Active Scheduler
    CActiveScheduler* sched = NULL;
    sched = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CleanupStack::PushL(sched);
    
	TRAPD(error,DoCallExeL(console));
	if (error)
		{
		console->Printf(KFormatFailed, error);
		}
	
	CleanupStack::PopAndDestroy(2,console); // close console
    }
    
LOCAL_C void DoCallExeL(CConsoleBase* aConsole)
    {
    
    iConsole = aConsole;
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    TInt noArgs = args->Count()-1; //-1 since 1st arg is file name which we ignore
    LOGLINE2(_L("Number command line args = %d"), noArgs);
    
    TBool bRunInPreviewMode = EFalse;
    TName inputFileNameWithPath;
    for(TInt i = 1; i<=noArgs;i++)
    	{
    	TPtrC arg = args->Arg(i);
    	if(arg == _L("-p") || arg == _L("-P"))	//run in preview mode
    		{
    		bRunInPreviewMode = ETrue;
    		}
    	else 
    		{
    		//this should be the name of the input file to read
    		inputFileNameWithPath = arg;
    		}    		
    	}
    	
    CleanupStack::PopAndDestroy(args);

    //open and read in bin file
    CReplayFileParser* fileparser = CReplayFileParser::NewLC(iConsole, inputFileNameWithPath);
		
	//read in the logged messages to create the model that will be replayed.
	CReplayModel* theModel = CReplayModel::NewLC(iConsole);
	
	CRecordMessage* newmessage;	
	while((newmessage = fileparser->GetNextMessageL()) != NULL)
		{
		theModel->AddMessageToModelL(newmessage);//transfers ownership to the model
		}
		
	//now start the model
	theModel->StartL(bRunInPreviewMode);
	CActiveScheduler::Start();
	
    CleanupStack::PopAndDestroy(2,fileparser); //theModel, fileparser
	}
