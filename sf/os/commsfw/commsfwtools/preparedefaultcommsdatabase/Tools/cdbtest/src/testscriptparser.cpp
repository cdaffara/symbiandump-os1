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

/**
 @file testscriptparser.cpp
 @internalComponent
*/

#include <e32base.h>
#include "testscriptparser.h"
#include "testlog.h"


CTestScriptParser* CTestScriptParser::FactoryLC( TDesC& aFile, CTestLog* aLog)
    {
	CTestScriptParser* p = NULL;
    
    p = new(ELeave) CTestScriptParser(aFile, aLog);

	CleanupStack::PushL(p);
	return p;
    }


CTestScriptParser::CTestScriptParser(TDesC& aFile, CTestLog* aLog) :
    iFile(aFile),
    iLog(aLog),
    iPtr(NULL, 0)
    {
    }

/**
*/
void CTestScriptParser::OpenScriptFileL()
{
	// Connect to file server
	//
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();

	// Open file
	//
	TAutoClose<RFile> file;
	TInt size;
	User::LeaveIfError(file.iObj.Open(fs.iObj, iFile, EFileStreamText | EFileRead));
	file.PushL();


	// Get file size and read in
	//
	User::LeaveIfError(file.iObj.Size(size));
	TText* data = (TText*)User::AllocL(size);
	iPtr.Set(data, TUint(size)/sizeof(TText), TUint(size)/sizeof(TText));
	TPtr8 dest((TUint8*)data, 0, size);
	User::LeaveIfError(file.iObj.Read(dest));
	TUint8* ptr = (TUint8*)data;

	//
	// This is orderred as FEFF assuming the processor is Little Endian
	// The data in the file is FFFE.		PRR 28/9/98
	//
	if(size>=(TInt)sizeof(TText) && iPtr[0]==0xFEFF)
		{
		// UNICODE Text file so lose the FFFE
		Mem::Copy(ptr, ptr+sizeof(TText), size-sizeof(TText));
		iPtr.Set(data, TUint(size)/sizeof(TText)-1, TUint(size)/sizeof(TText)-1);
		}
	else if(size)
		{
		// NON-UNICODE so convert to UNICODE
		TText* newdata = (TText*)User::AllocL(size*sizeof(TText));
		iPtr.Set(newdata, size, size);
		TInt i;
		for(i=0 ; i<size ; ++i)
			{
			iPtr[i]=ptr[i];
			}
		delete data;
		}
	
	iLex.Assign(iPtr);

	file.Pop();
	fs.Pop();
}

/**
*/
TBool CTestScriptParser::GetNextTestStepL(TScriptAction &runAction, TDes &params)
	{
    TBuf<256> stepAction;
    stepAction.Append(iLex.NextToken());
    runAction = ENoAction;

    if (stepAction.Compare(_L("//")) == 0)
        {
        runAction = ENoAction;
        }
    else if (stepAction.Compare(_L("RUN_PROGRAM")) == 0)
	    {
	    // Parse the input file and get the command line
    	runAction = ERunProgram;
	    }
    else if (stepAction.Compare(_L("RUN_TEST_STEP")) == 0)
	    {
    	runAction = ERunTestStep;
	    }
    else if ((stepAction.Compare(_L("PRINT")) == 0)  ||
		(stepAction.Compare(_L("START_TESTCASE")) == 0) ||
		(stepAction.Compare(_L("END_TESTCASE")) == 0))
    {
    	runAction = EPrint;
	    }
    else if (stepAction.Compare(_L("LOAD_SUITE")) == 0)
	    {
    	runAction = ELoadTestSuite;
	    }
    
    // Skip past any leading white spaces
   	iLex.SkipSpace();
    iLex.Mark();

    while(!iLex.Eos())
        {
        TChar peek = iLex.Peek();
            
        // Line is terminated with a carriage return or line feed
        if((peek == '\n') || (peek == '\r'))
            {
            break;
            }
           else
       	    {
       	    iLex.Inc();
       	    }
        }
    
    params.Append(iLex.MarkedToken());

    return iLex.Eos();
	}


void CTestScriptParser::Exit(TInt aErr)
    {
	iLog->Msg(_L("Error: Script file [%S] could not be opened"), &iFile);
	User::Leave(aErr);
    }


//EOF
