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



#include <e32base.h>
#include <f32file.h>
#include <lbspositioninfo.h>
#include <lbs.h>
#include <lbscommon.h>
#include <lbsrequestor.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>

TInt DoTestL();

TInt TestMainL()
    {
     // Create and install the active scheduler 
    CActiveScheduler* exampleScheduler=new (ELeave) CActiveScheduler; 
    CleanupStack::PushL(exampleScheduler); 
    CActiveScheduler::Install(exampleScheduler); 
    // Call function
    TInt result = DoTestL();  
    CleanupStack::PopAndDestroy(exampleScheduler);
    return result;
    }

void TraceL(const TDesC& aMsg, RFile& aFile) 
    {
    const TInt KBufSize = 200;
    TBuf8<KBufSize> tmp;
    tmp.Copy(aMsg);
    _LIT(KEnd,";\r\n");
    tmp.Append(KEnd);
    aFile.Write(tmp);
    }

/**
* Setup PSY
*/
void SetupPsyL(const TUid aPsyUid)
    {
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    CPosModuleUpdate* enable = CPosModuleUpdate::NewLC();
    CPosModuleUpdate* disable = CPosModuleUpdate::NewLC();

    disable->SetUpdateAvailability(EFalse);
    enable->SetUpdateAvailability(ETrue);

    CPosModuleIdList* prioList = db->ModuleIdListLC();

    for (TInt i = 0 ; i < prioList->Count(); i++)
        {
        if ((*prioList)[i] != aPsyUid)
            {
            // Disable all PSYs except one given as parameter
            db->UpdateModuleL((*prioList)[i], *disable);
            }
        else
            {
            // Enable the PSY that came as an in parameter
            db->UpdateModuleL((*prioList)[i], *enable);
            }
        }

    CleanupStack::PopAndDestroy(prioList);
    CleanupStack::PopAndDestroy(disable);
    CleanupStack::PopAndDestroy(enable);
    CleanupStack::PopAndDestroy(db);
    }

/**
* Method used to generate a file name for trace.
* If a trace file exist an index number i counted
* one step until a trace file can be created.
* Maximun number of exe client are currently hard
* coded to 10;
*/
TBuf<40> GenerateFileNameForTraceL()
    {
    _LIT(KFileTrace, "c:\\logs\\execlientresult%d.txt");
    RFs fileserver;
    RFile file;
    User::LeaveIfError(fileserver.Connect());
    TBuf<40> ff;
    ff.Append(KFileTrace);
 
    for (TInt i = 1; i<10;i++) 
        {
        ff.Format(KFileTrace,i);
        TInt err = file.Open(fileserver,ff, EFileWrite);
        if (err == KErrNotFound) 
            {
            User::LeaveIfError(file.Create(fileserver,ff, EFileWrite));
            break;
            }
        }
    file.Close();
    fileserver.Close();
    return ff;
}

/**
* Error checking and output error to Trace File.
*/
void AssertTrueL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode, RFile& aFile)
  {
  if (!aCondition)
		{
		TBuf<100> buf;
		buf.Format(aErrorMsg, aErrorCode);
		TraceL(buf, aFile);
        //User::Leave(aErrorCode);
		}
  }

/**
* Performes the test by connecting to MLFW and makes 50 Location request
*/
TInt DoTestL()
    {
    TBuf<40> traceFile = GenerateFileNameForTraceL();

    RFs fileserver;
    RFile file;
    User::LeaveIfError(fileserver.Connect());
    CleanupClosePushL(fileserver);
    User::LeaveIfError(file.Open(fileserver, traceFile, EFileWrite));
    CleanupClosePushL(file);
    _LIT(KClientStarted, "Client Started");
    TraceL(KClientStarted, file);

    const TInt32 KUidMultiPsy = 0x01010176;
    TUid uidMultiPsy;
    uidMultiPsy.iUid = KUidMultiPsy;      
	SetupPsyL(uidMultiPsy);

    _LIT(KMultiPsySetup, "MultiPsy set up");
    TraceL(KMultiPsySetup, file);

	TInt numberOfRuns = 50;
    
    RPositionServer	posServer;
    CleanupClosePushL(posServer);
    RPositioner positioner;
    CleanupClosePushL(positioner);
    TPositionInfo positionInfo = TPositionInfo();

	_LIT(KConnectErr, "ERROR: Error when connecing to EPos server,  %d");
	TInt err = posServer.Connect();
	AssertTrueL(err == KErrNone, KConnectErr, err, file);

	_LIT(KOpenErr, "ERROR: Error when opening positioner,  %d");
	err = positioner.Open(posServer, uidMultiPsy);
	AssertTrueL(err == KErrNone, KOpenErr, err, file);

    _LIT(KService ,"Service");
	err = positioner.SetRequestor(CRequestor::ERequestorService,
							 CRequestor::EFormatApplication, KService);
    _LIT(KRequestor, "ERROR: Not possible to set requestor");
    AssertTrueL(err == KErrNone, KRequestor, err, file);
    
    TTime now, startTime;
    TTimeIntervalMicroSeconds requestTime;
    TRequestStatus status;
    TInt64 reqTime;
    TCoordinate corr(0,0,0);
    TLocality loca(corr,0);
    TPosition pos(loca, TTime(0));
    TBool success = ETrue;
    _LIT(KStartingRequests, "Starting requests");
    TraceL(KStartingRequests, file);

    for (TInt i = 0; i < numberOfRuns; i++)
        {
        startTime.UniversalTime();
        positionInfo.SetPosition(pos);
        positioner.NotifyPositionUpdate(positionInfo, status);
        User::WaitForRequest(status);
        err = status.Int();
        if (err != KErrNone)
            {
            success=EFalse;
            }
        now.UniversalTime();
        requestTime = now.MicroSecondsFrom(startTime);
        _LIT(KError, "Request time, %d µs. Error code from Notify = %d");
        TBuf<100> buf;
        reqTime = requestTime.Int64();
        buf.Format(KError, reqTime, err);
        TraceL(buf, file);
        TPosition result;
        positionInfo.GetPosition(result);
        // Sanity check
        if (result.Latitude() == pos.Latitude() ||
		    result.Longitude() == pos.Longitude() ||
		    result.Altitude() == pos.Altitude())
            {
             success = EFalse;
             _LIT(KErrorPositon, "ERROR:: The postion was not updated");
             TraceL(KErrorPositon, file);
            }
        }
    
    CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&posServer);

    if (success)
        {
        _LIT(KOk, "SUCCESS");
        TraceL(KOk, file);
        }
    else
        {
        _LIT(KErr, "FAILED");
        TraceL(KErr, file);
        }

    CleanupStack::PopAndDestroy(&file);
    CleanupStack::PopAndDestroy(&fileserver);
    
    return (success) ? 0 : 1;
    }

// Cleanup stack harness
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    TInt result = 1;
    TRAPD(error, result = TestMainL());
    _LIT(KLbsExeClient, "CT Lbs Exe Client");
    __ASSERT_ALWAYS(!error, User::Panic(KLbsExeClient, error));
    delete cleanupStack;
    __UHEAP_MARKEND;
    return result;
    }

