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



//  INCLUDES
#include "ctlbsclientpostp208.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>

// CONSTANTS
static const TInt KNrOfClients = 5;
static const TInt KNrOfRuns = 20;

static const TInt KServerMinHeapSize =  4096;
static const TInt KServerMaxHeapSize =  128000;


_LIT(KThread2Name, "req_location1");
_LIT(KThread3Name, "req_location2");
_LIT(KErrMsgThreadCreation, "failed to create thread");

// ================= MEMBER FUNCTIONS =======================

// Constructor.
CT_LbsClientPosTp208::CT_LbsClientPosTp208(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {  
    _LIT(KTestName, "TP208-Multi Thread Connection");
    SetTestStepName(KTestName); 
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp208::~CT_LbsClientPosTp208()
	{
	}

// ---------------------------------------------------------
// CPosTp208::CloseTest
// Always called after the test, even if the test leaves
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp208::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }

// Struct passed to the thread in a TAny* 
struct MyAnyData
{
    TBool     iFullTest; // If fulltest defined
    TInt      iNrOfRuns; // number if requests to make for each thread
};

//
// Performes the test by connecting to MLFW  
// (and makes a number of Location requests if aFullTest is true
//
void DoTestL(TBool aFullTest, TInt aNumberOfRuns, TInt *aResult)
    {
    RPositionServer	posServer;
	TInt errorCode = errorCode = posServer.Connect();

    if (errorCode != KErrNone)
        {
        *aResult = KErrCouldNotConnect;
        return;
        }
    CleanupClosePushL(posServer);

    RPositioner positioner;


    // Full test means requesting position updates
    if (aFullTest)
        {
        TPositionInfo positionInfo = TPositionInfo();
        const TInt32 KUidMultiPsy = 0x01010176;
        TUid uidMultiPsy;
        uidMultiPsy.iUid = KUidMultiPsy;
        errorCode = positioner.Open(posServer, uidMultiPsy);
        
        if (errorCode != KErrNone) 
        {
            *aResult = errorCode;
            return;
        }
        CleanupClosePushL(positioner);
        
        _LIT(KService ,"Service");
        errorCode = positioner.SetRequestor(CRequestor::ERequestorService,
            CRequestor::EFormatApplication, KService);
        
        if (errorCode != KErrNone)
        {
            *aResult = 1001;
            return;
        }
        
        TRequestStatus status;
        TLocality loca(TCoordinate(0,0,0),0);
        TPosition pos(loca, TTime(0));

        for (TInt i = 0; i < aNumberOfRuns; i++)
        {
            positionInfo.SetPosition(pos);
            positioner.NotifyPositionUpdate(positionInfo, status);
            User::WaitForRequest(status);
            TInt err = status.Int();
            if (err != KErrNone)
            {
                *aResult = err;
            }
            TPosition result;
            positionInfo.GetPosition(result);
            
            //sanity check
            if (result.Latitude() == pos.Latitude() ||
                result.Longitude() == pos.Longitude() ||
                result.Altitude() == pos.Altitude())
            {
                //_LIT(KErrorPositon, "ERROR:: The postion was not updated");
                errorCode = 1005;
            }   
        }
        positioner.Close();
        CleanupStack::PopAndDestroy(&positioner);
    }
    
    posServer.Close();
    CleanupStack::PopAndDestroy(&posServer);
    }

// ---------------------------------------------------------
// LOCAL_C TInt MyThreadFunctionL(TAny* aData)
// Runs the DoTestL in a TRAP nest
// Panics this thread if any errors occurred
// ---------------------------------------------------------
//
LOCAL_C TInt MyThreadFunctionL(TAny* aData)
  {
  __UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack

    // Read parameters from TAny*
    MyAnyData* info = (MyAnyData*) aData;

    TInt result = KErrNone;
    TInt err = KErrNone;
    // Why must this function be TRAPPED?
    TRAP(err, DoTestL(info->iFullTest, info->iNrOfRuns, &result));

    _LIT(KErrorConnect, "Error when connecting to RPositionServer");
    __ASSERT_ALWAYS(!err, User::Panic(KErrorConnect, err));
    __ASSERT_ALWAYS(result == KErrNone, User::Panic(KErrorConnect, result));

	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
  return 0;
  }

// ---------------------------------------------------------
// CPosTp208::StartThreads
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsClientPosTp208::StartThreads(TBool aFullTest)
    {
    TFixedArray<TRequestStatus, KNrOfClients> statusArray;
    TFixedArray<RThread, KNrOfClients> threadArray;
    TFixedArray<TInt, KNrOfClients> retArray;
   
    TBuf<150> buf;
    TInt errorsFound = KErrNone;
    _LIT(KInfoCreate, "Creating threads");
    INFO_PRINTF1(KInfoCreate);
    _LIT(KThreadName, "Thread %d");

    // Introduce parameters for each thread.
    MyAnyData info;

    info.iNrOfRuns = KNrOfRuns;
    info.iFullTest = aFullTest;
    // Make sure that if one of the threads panic, whole emulator is not
    // paniced (this has effect only in Wins).
    TBool jit = User::JustInTime();
    User::SetJustInTime(EFalse);

    TInt i=0;
    for (i=0; i<KNrOfClients; i++)
        {
        iThreadCount++;
        buf.Zero();
        buf.Format(KThreadName, iThreadCount);
        retArray[i] = threadArray[i].Create(buf, MyThreadFunctionL, KDefaultStackSize, KServerMinHeapSize, KServerMaxHeapSize, &info);
        if (retArray[i] != KErrNone)
            {
            _LIT(KErr, "Error when creating thread%d, errorcode %d");
            buf.Format(KErr, i, retArray[i]);
            INFO_PRINTF1(buf);
            errorsFound++;
            }
        else 
            {
            _LIT(KErr, "Created thread %d");
            buf.Format(KErr, i);
            INFO_PRINTF1(buf);
            threadArray[i].Logon(statusArray[i]); 
            }
        }
 
    for (TInt j=0; j<KNrOfClients; j++)
        {
        if (retArray[j] == KErrNone) 
            {
            buf.Zero();
            _LIT(KInfoResume, "Resuming thread %d");
            buf.Format(KInfoResume, j);
            //if (bDEBUG) 
            INFO_PRINTF1(buf);
            User::After(6000000);
            threadArray[j].Resume();
            }
        }
	for (i = KNrOfClients-1; i>=0; i--)
        {

        if (retArray[i] == KErrNone) User::WaitForRequest(statusArray[i]);
        
        buf.Zero();
        buf.Format(KThreadName, i);

        _LIT(KWaitForRequest, " User::WaitForRequest ");
        buf.Append(KWaitForRequest);
        INFO_PRINTF1(buf);

        buf.Zero();
        buf.Format(KThreadName, i);

        if (threadArray[i].ExitType() == EExitPanic)
            {
            _LIT(KPanicExit, " EExitPanic ");
            buf.Append(KPanicExit); 
            INFO_PRINTF1(buf);
            errorsFound++;
            }
        else if (threadArray[i].ExitType() == EExitKill)
            {
            _LIT(KKillExit, " EExitKill");
            buf.Append(KKillExit); 
            INFO_PRINTF1(buf);
            }
        else if (threadArray[i].ExitType() == EExitPending)
            {
            _LIT(KPendingExit, " EExitPending");
            buf.Append(KPendingExit); 
            INFO_PRINTF1(buf);
            errorsFound++;
            }
        else
            {
            _LIT(KOtherExit, " other ");
            buf.Append(KOtherExit);
            INFO_PRINTF1(buf);
            errorsFound++;
            }
        }

      for (i=0; i<KNrOfClients; i++)
        {
         buf.Zero();
         _LIT(KDEBUG, "Wrong reasoncode from Thread %d reasoncode %d");
         buf.Format(KDEBUG, i, threadArray[i].ExitReason());
         if (threadArray[i].ExitReason() != KErrNone) INFO_PRINTF1(buf);
         threadArray[i].Close();
        }

    // Only affected on WINS emulator
    User::SetJustInTime(jit);

    return errorsFound;
    }


/**
 * Connection is opened to location server,and closed
 *
 * If anything fails during the execution of this test, then function
 * leaves with proper error code.
 *
 * Parameters:
 **/
void LocationRequestThreadMainPartL(TDes& aErrorBuf)
    {
    // Open connection to Location server
    RPositionServer server;

    TInt err = server.Connect(); 

    if (err != KErrNone)
        {
        _LIT(KErrOpen, "Server open failed with code %d\r\n");
        aErrorBuf.Format(KErrOpen, err);
        }
    else
        {
        _LIT(KOkOpen, "Server open ok\r\n");
        aErrorBuf.Copy(KOkOpen);
        }

    User::LeaveIfError(err);

    CleanupClosePushL(server);


    CleanupStack::PopAndDestroy(1); // server, positioner

    _LIT(KInfTestComplet, "Test completed ok for this thread\r\n");
    aErrorBuf.Append(KInfTestComplet);
    }

/**
 * This is the main function for threads that connects and
 * disconnect from MLFW
 *
 * Function returns 0 if running of the test did not leave.
 *Otherwise the leave code is returned.
 **/
LOCAL_C TInt LocationRequestThread(TAny* aData)
{
    // Create cleanupstack for this thread
    CTrapCleanup* cleanup = CTrapCleanup::New();

    // No parameters to read, just start main function.

    HBufC* buf = HBufC::NewL(1024);

    TPtr ptr = buf->Des();

    // Run the actual test, and trap possible leave
    TRAPD(leave, LocationRequestThreadMainPartL(ptr));

    // Open connection to file server
    RFs fs;
    User::LeaveIfError(fs.Connect());

    TInt index = *((TInt*) aData);

    TBuf<30> errorFile;
    _LIT(KInfThread, "c:\\logs\\thread%derrinfo.txt");
    errorFile.Format(KInfThread, index);

    // Open log file
    RFile file;

    TInt err = file.Replace(fs,errorFile,EFileStreamText|EFileWrite);

    if (err != KErrNone)
        {
        User::Leave(err);
        }

    _LIT(KErrLeave, "*** This thread (thread %d) leave code: %d\r\n");
    ptr.AppendFormat(KErrLeave, index, leave);

    TFileText fileText;
    fileText.Set(file);

    fileText.Write(ptr);

    // Close log file
    file.Close();
    fs.Close();

    // Delete cleanup stack
    delete cleanup;

    // Exit this thread with leave code (KErrNone in successful case)
    User::Exit(leave);

    return 0;
}

// ---------------------------------------------------------
// CPosTp208::DoFastConnectionL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsClientPosTp208::DoFastConnectionL()
    {   
    TInt err;

    // Create two different threads. Each thread connects
    // and disconnect from MLFW very fast.
    const TInt KMinHeapSize =  4096;
    const TInt KMaxHeapSize =  128000;

    RThread thread2;
    RThread thread3;

    TInt thread2Id = 2;
    TInt thread3Id = 3;

    // Make sure that if one of the threads panic, whole emulator is not
    // paniced (this has effect only in Wins).
    User::SetJustInTime(EFalse);

    _LIT(KInfo, "Create two threads");
    INFO_PRINTF1(KInfo);

    err = thread2.Create(KThread2Name, LocationRequestThread, KDefaultStackSize, 
    KMinHeapSize, KMaxHeapSize, &thread2Id);

    AssertTrueL(err == KErrNone, KErrMsgThreadCreation, err);   

    err = thread3.Create(KThread3Name, LocationRequestThread, KDefaultStackSize, 
    KMinHeapSize, KMaxHeapSize, &thread3Id);

    AssertTrueL(err == KErrNone, KErrMsgThreadCreation, err);   

    // Logon to each thread, so we can get a notification when they kill
    // themselves.
    TRequestStatus status2, status3;
    thread2.Logon(status2);
    thread3.Logon(status3);

    _LIT(KStart, "Start the threads will connect to and disconnect to MLFW,");
    INFO_PRINTF1(KStart);

    // Start all treads.
    thread2.Resume();
    User::After(2000000);
    thread3.Resume();


    // Wait for all threads to exit 
    User::WaitForRequest(status2); 
    User::WaitForRequest(status3); 

    // Actual test is now run.
    _LIT(KInfoExit, "All threads have exited, exit reasons:");
    INFO_PRINTF1(KInfoExit);

    // Check that all threads had correct exit reason. Also log
    // all the exit reasons into log file.
    TInt exitReasonsCorrect = 0;

    exitReasonsCorrect |= thread2.ExitReason();
    exitReasonsCorrect |= thread3.ExitReason();

    // Close all handles to threads.
    thread2.Close();
    thread3.Close();

    // Return back to normal mode
    User::SetJustInTime(ETrue);  
    return exitReasonsCorrect;
    }

// ---------------------------------------------------------
// CPosTp208::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp208::StartL()
    {
    //first do a simple connection test
    TInt fasterr = DoFastConnectionL();
    _LIT(KFailConnect, "Fast connection failed");
    AssertTrueL(fasterr == KErrNone, KFailConnect, fasterr);  

    SetupPsyL(iUidMultiPsy);

    TInt nrOfRounds = 1;
    TBuf<100> buf;
    
    _LIT(KPsitionUpdate, 
    		">>>>>>>>>>Running StartThreads(ETrue) requesting position updates<<<<<<<<");
    INFO_PRINTF1(KPsitionUpdate);

    _LIT(KEmptySpace, "");
    _LIT(KRound, ">>>>>>Round nr %d :");
    _LIT(KErrorsFound, "Errors found!!!");
    
    for (TInt i=0; i<nrOfRounds; i++)
        {
        INFO_PRINTF1(KEmptySpace);
        buf.Format(KRound, i);
        INFO_PRINTF1(buf);
        TTime now, startTime;
        TTimeIntervalMicroSeconds requestTime;
        
        startTime.UniversalTime();
        TInt res = StartThreads(ETrue);
        if (res != KErrNone) 
        	LogErrorAndLeaveL(KErrorsFound, res);
        now.UniversalTime();
        
        requestTime = now.MicroSecondsFrom(startTime);
        _LIT(KDebug, "%d requests from %d threads took: %d microsecs.");
        buf.Zero();
        TInt64 reqTime = requestTime.Int64();
        buf.Format(KDebug, KNrOfClients * KNrOfRuns, KNrOfClients, reqTime);
        INFO_PRINTF1(buf);
      
        }

    _LIT(KConnectDisconnect, 
    		">>>>>>>>>>Running StartThreads(EFalse) connect/disconnect Epos<<<<<<<<");
    INFO_PRINTF1(KConnectDisconnect);
    nrOfRounds = 5;
    for (TInt j=0; j<nrOfRounds; j++)
        {
        INFO_PRINTF1(KEmptySpace);
        buf.Format(KRound, j);
        INFO_PRINTF1(buf);
        TTime now, startTime;
        TTimeIntervalMicroSeconds requestTime;
       
        startTime.UniversalTime();

        // Threads only connects to Epos server and then disconnect
        TInt res = StartThreads(EFalse);
        if (res != KErrNone) 
        	LogErrorAndLeaveL(KErrorsFound, res);
        now.UniversalTime();
        
        requestTime = now.MicroSecondsFrom(startTime);
        _LIT(KDebug, "%d threads connecting and disconnecting to Epos server took: %d microsecs.");
        buf.Zero();
        TInt64 reqTime = requestTime.Int64();
        buf.Format(KDebug, KNrOfClients, reqTime);
        INFO_PRINTF1(buf);
        }
    }

// End of File
