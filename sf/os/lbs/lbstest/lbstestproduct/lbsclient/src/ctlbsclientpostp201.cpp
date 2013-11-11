// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "ctlbsclientpostp201.h"
#include <lbspositioninfo.h>
#include <lbs.h>
#include <lbscommon.h>
#include <lbsrequestor.h>
#include <s32file.h>
#include "ctlbsclientobserver.h"

// CONSTANTS
const TInt KSecond = 1*1000*1000;
const TInt KMaxSentenceLength = 60; //Max length of each line in EXEClient-logfile
const TInt KNoOfEXCalls = 3; // Nr of calls to ExeClient
_LIT(KExeClientPath, "ctlbsexeclient");
_LIT(KExeResultPath, "c:\\logs\\execlientresult%d.txt");
_LIT(KSucce, "Test %d was successful");
_LIT(KFail, "Test %d failed");
_LIT8(KSuccess, "SUCCESS"); //String to search for that indicates the test was succesful

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp201::CT_LbsClientPosTp201(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "TP201 - Exe Client");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp201::~CT_LbsClientPosTp201()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp201::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp201::StartL()
    {
	
    //This test can be run on emulator now.
	TInt result;
	TInt i;
	TInt foundsucces; 
	RProcess execlient[KNoOfEXCalls];
	RFile file;
	RFileReadStream fstream;
	RFs fileSession;
	User::LeaveIfError(fileSession.Connect());
	CleanupClosePushL(fileSession);
	CleanupClosePushL(fstream);
	CleanupClosePushL(file);
	CFileMan* fileMan = CFileMan::NewL(fileSession);
	CleanupStack::PushL(fileMan);

	_LIT(KRemoveFiles, "Remove files");
	INFO_PRINTF1(KRemoveFiles);

	//1. First check for old ExeClientResult-files to remove.
	TBuf<40> filepath;
    filepath.Append(KExeResultPath);
	for ( i = 1; i<=KNoOfEXCalls; i++)
		{
		filepath.Format(KExeResultPath,i);
		TInt err = fileMan->Delete(filepath, CFileMan::EOverWrite);
		if (err != KErrNone && err != KErrNotFound)
			{
			_LIT(KErrRemoveDll, "Removal of a previous ExeClientResult-file failed!");
			TBuf<50> buf;
			buf.Format(KErrRemoveDll, err);
			LogErrorAndLeaveL(buf);
			}
		}	
	
	_LIT(KStartClients, "Start clients");
	INFO_PRINTF1(KStartClients);

	//2. Make 3 calls to ExeClient.
	for ( i = 1; i<= KNoOfEXCalls; i++)
		{
		//Start the execlient-process
		result = execlient[i-1].Create(KExeClientPath, KNullDesC);
		User::LeaveIfError(result);
		execlient[i-1].Resume();	//make the execlient visible
		User::After(6000000);
		}
	
	CT_LbsClientObserver::WaitForEPosShutdown(); //Wait for Epos to complete
    User::After(KSecond); //Wait for completion of writing to files. 1 minute.
	for ( i = 0; i < KNoOfEXCalls; i++)
		{	
		TRequestStatus closed;
		execlient[i].Logon(closed);
		User::WaitForRequest(closed);

	    TBuf<100> exit;
	    _LIT(KExitClient, "Client %d exited: Type %d, Reason %d");
	    exit.Format(KExitClient, 
            	    i+1, execlient[i].ExitType(), execlient[i].ExitReason());
        INFO_PRINTF1(exit);

		execlient[i].Close(); //Close handle to exe-client
		}
	_LIT(KCheckResults, "Check results");
	INFO_PRINTF1(KCheckResults);

	//3. See if all calls were successes
	TBuf<40> successinfo;
	successinfo.Append(KSucce);
	TBuf<40> failinfo;
	failinfo.Append(KFail);
	for ( i = 1; i <= KNoOfEXCalls; i++)
		{
		filepath.Format(KExeResultPath,i);
		User::LeaveIfError(file.Open(fileSession, filepath, EFileShareReadersOnly));//open read only
		fstream.Attach(file);

		TBuf8<KMaxSentenceLength> dbuffer; //Buffer to place read data from file
		TBuf8<KMaxSentenceLength> prevbuffer; //Buffer to place previous read data from file
		TBool search = ETrue;
		const TChar eoline(';');
		while (search) 
			{ 
			dbuffer.Delete(0, KMaxSentenceLength);
			TRAPD(ferr, (fstream.ReadL(dbuffer,eoline)));
			if ( ferr == KErrEof) 
				{
				search = EFalse; //stop searching
				}
			else if ( ferr == KErrNone)
				{
				prevbuffer.Delete(0, KMaxSentenceLength);
				prevbuffer = dbuffer;
				}
			else 
				{
				_LIT(KReadError, "Error when reading from log-file.");
				LogErrorAndLeaveL(KReadError);
				}
			}
		//Check if success
		foundsucces = prevbuffer.Find(KSuccess);
		if ( foundsucces != KErrNotFound)
			{
			successinfo.Format(KSucce,i);
			INFO_PRINTF1(successinfo);
			}
		else
			{
			failinfo.Format(KFail,i);
			LogErrorAndLeaveL(failinfo);
			}
		} //for

	_LIT(KDone, "Done.");
	INFO_PRINTF1(KDone);

	//4. Let's clean...
	CleanupStack::PopAndDestroy(fileMan); //fileMan
	CleanupStack::PopAndDestroy(&file); //file
	CleanupStack::PopAndDestroy(&fstream); //fstream
	CleanupStack::PopAndDestroy(&fileSession); //fileSession
	}

// End of File
