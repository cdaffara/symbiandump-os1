// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleidlist.h>
#include "ctlbsclientpostp256.h"
#include <e32std.h>

// CONSTANTS
const TInt KNoMultipleClients=2;
const TUint KMaxHeapSize=0x80000;

// Structure for passing parameters to threads
struct MyThreadData
    {
    TBool iTestFlag;
    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp256::CT_LbsClientPosTp256(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "Tp256 - Location Settings and Multiple Clients");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp256::~CT_LbsClientPosTp256()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp256::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp256::StartL()
    {
    _LIT(KMultipleErr, "Error %d when multiple clients access the CPosModules, err from client:%d");
    _LIT(KSuccess, "Client %d is successful");
    _LIT(KAllErr, "All clients gets error when multiple clients access the CPosModules");
    _LIT(KPanicErr, "Panic when multiple clients access the CPosModules,  Thread: %d panics");
    
    TFixedArray<TRequestStatus, KNoMultipleClients> statuses;
    //make a request just to make sure att all databases are created.

	SetupPsyL(iUidMultiPsy);

    //synchronous request
    ConnectL();

    User::LeaveIfError(OpenPositionerByName(iUidMultiPsy));

    TPositionInfo info = TPositionInfo();

	RequestL(info);

    //potenial memory leak if above function leav
    ClosePositioner();
    Disconnect();

    //End first request.


    TInt errorsFound = KErrNone;
    CreateThreadsL();

    for (TInt i=0; i<iThreads.Count(); i++)
        { 
        iThreads[i].Logon(statuses[i]);
        iThreads[i].Resume();
        }
    
    //Waiting while all threads are completed
    for(TBool allDone = EFalse;!allDone;)
    	{
    	allDone = ETrue;
    	for (TInt i=0; i<iThreads.Count(); i++)
    		{
    		if(iThreads[i].ExitType()==EExitPending)
    			{
    			allDone = EFalse;
    			User::After(100000);
    			break;
    			}
    		}
    	}
    
    for (TInt t=0; t<iThreads.Count(); t++)
        {   
        TInt exitReason = iThreads[t].ExitReason();
        
        TBuf<150> buf;
        if (exitReason != KErrNone) 
            {      
            buf.Format(KMultipleErr, exitReason, t);
            INFO_PRINTF1(buf);

            errorsFound++;
            }
        else
            {
            buf.Format(KSuccess, t);
            INFO_PRINTF1(buf);
            }
        AssertTrueL(iThreads[t].ExitType() != EExitPanic, KPanicErr, t);
        
        
        }
    
    AssertTrueL(errorsFound < KNoMultipleClients, KAllErr, KErrGeneral);   
    
    _LIT(KPass, "CT_LbsClientPosTp256 passed!!!");
    INFO_PRINTF1(KPass);
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp256::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp256::CloseTest()
    {
    for (TInt i=0; i<iThreads.Count(); i++)
        {
        iThreads[i].Close();
        }

    iThreads.Close();
    iNames.ResetAndDestroy();
    }

// ---------------------------------------------------------
// void RunTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void RunTestL(TAny* aData)
    {
    const TInt KNrOfRounds = 50;
   
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    TInt nrOfModules = 0;

    // Read parameters from TAny*
    MyThreadData* info = (MyThreadData*) aData;
    TInt i = 0;
    for (i=0;i<KNrOfRounds; i++)
        {
        //db->ResetAllModulesL();
        CPosModuleIdList* myDebugList = db->ModuleIdListLC();
        
        nrOfModules = myDebugList->Count();
        TPositionModuleInfo debugModuleInfo;
        TBuf<40> moduleName;
        TPositionQuality quality;
        
        for (TInt i=0;i<nrOfModules;i++)
            {
            db->GetModuleInfoL(myDebugList->At(i), debugModuleInfo);
            moduleName.Zero();
            debugModuleInfo.GetModuleName(moduleName);
            debugModuleInfo.GetPositionQuality(quality);
            }
            CleanupStack::PopAndDestroy(myDebugList);
        }

   for (i=0;i<KNrOfRounds; i++)
        {
        // The first thread does this
        if (info->iTestFlag)
            {
            for (TInt j=0;j<nrOfModules;j++)
                {
                // Just change priority
                db->SetModulePriorityL(nrOfModules-j-1, j);
                }
            }
        // And all the other thread does this
        else
            {
            CPosModuleIdList* myDebugList = db->ModuleIdListLC();
            
            TInt debugCount = myDebugList->Count();
            TPositionModuleInfo debugModuleInfo;
            TBuf<32> moduleName;
            TPositionQuality quality;
            
            for (TInt i=0;i<debugCount;i++)
                {
                db->GetModuleInfoL(myDebugList->At(i), debugModuleInfo);
                moduleName.Zero();
                debugModuleInfo.GetModuleName(moduleName);
                debugModuleInfo.GetPositionQuality(quality);
                }
            CleanupStack::PopAndDestroy(myDebugList);
            }
        }
        

    CleanupStack::PopAndDestroy(db);
    }

// ---------------------------------------------------------
// LOCAL_C TInt ThreadFunction
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
LOCAL_C TInt ThreadFunction(TAny* aData)
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
    
    if (cleanup == NULL)
        {
        return KErrNoMemory;
        }
    // we need an ActiveScheduler 
    CActiveScheduler scheduler;
    CActiveScheduler::Install(&scheduler);   

    TRAPD(err, RunTestL(aData));
    
    delete cleanup; // destroy clean-up stack
    __UHEAP_MARKEND;
    return err;
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp256::CreateThreadsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp256::CreateThreadsL()
    {
    _LIT(KThreadName, "TP256 test thread%d");
    _LIT(KCreateThreadErr, "Create thread failed with %d");

    for (TInt i=0; i<KNoMultipleClients; i++)
        {
        // Introduce parameters for each thread.
        MyThreadData info;
        
        if (i == 0) info.iTestFlag = ETrue;
        else info.iTestFlag = EFalse;

        RThread thread;
        TBuf<32> name;
	    name.Format(KThreadName, 1+iThreads.Count());

        TInt err;
        err = thread.Create(name, ThreadFunction, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, reinterpret_cast<TAny*>(&info));
        AssertTrueL(err == KErrNone, KCreateThreadErr, err);                                                                  
        
        iThreads.Append(thread);
        }
    }


void CT_LbsClientPosTp256::RequestL(TPositionInfoBase& aInfoBase)
	{
	_LIT(KService, "service");
    TInt err = PerformSyncRequest(KService, &aInfoBase);

    if (err != KErrNone)
        {
        _LIT(KErrorRequest, "The request was not completed with KErrorNone, errCode = %d");
		TBuf<100> buf;
		buf.Format(KErrorRequest, err);
        LogErrorAndLeaveL(buf);
        }

	_LIT(KIncorrectPsySyncRequestError,
          "TP2. Position from wrong PSY received (sync request).");
    if (iUidMultiPsy != aInfoBase.ModuleId())
        {
        LogErrorAndLeaveL(KIncorrectPsySyncRequestError);
        }
	}
    
//  End of File
