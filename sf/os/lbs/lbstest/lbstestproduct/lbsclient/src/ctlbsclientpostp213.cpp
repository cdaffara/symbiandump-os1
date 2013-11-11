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
#include "ctlbsclientpostp213.h"
#include "ctlbsclientrequestor.h"
#include "ctlbsclienteventtimer.h"
#include <lbspositioninfo.h>
#include <lbs.h>
#include <lbscommon.h>
#include <lbsrequestor.h>
#include <s32file.h>

// CONSTANTS
_LIT(KClient1, "RunL client1");
_LIT(KClient2, "RunL client2");
_LIT(KClient3, "RunL client3");

_LIT(KClient1Default, "RunL client1, default psy");
_LIT(KClient2Default, "RunL client2, default psy");
_LIT(KClient3Default, "RunL client3, default psy");

//Error logs
_LIT( KErrStatusMismatch, "Statuses does not conform to each other!" );
_LIT( KEventReceived, "Client 3 received an event that shouldn't be received!" );
_LIT( KCanceledRequest, "Client 2 didn't receive KErrCancel, but %d!" );
_LIT( KFileNotDeleted, "File created by PSY NOT deleted! PSY really unloaded from memory?" );

//File name. Beware, also declared in EPos_CTestStatusPsy.h!!!!
_LIT(KDummyFileName, "c:\\TestStatusPsyIsNotUnloaded.txt");

// ================= MEMBER FUNCTIONS =======================

// Constructor.
CT_LbsClientPosTp213::CT_LbsClientPosTp213(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {  
    _LIT(KTestName, "TP213 - Disabling psy during request");
    SetTestStepName(KTestName); 
    }

// Destructor
CT_LbsClientPosTp213::~CT_LbsClientPosTp213()
    {
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp213::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp213::CloseTest()
    {
	delete ieventtimer;
	ieventtimer = NULL;

	delete iclient1;
	delete iclient2;
	delete iclient3;
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp213::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp213::StartL()
    {
	TBool usedefaultpsy = FALSE; // Indicating if the default psy should be used
	iNoFurtherEvents = FALSE;	// Client should receive events!
	iErrorDuringExecution = FALSE;  // True if an error occurs 

	//Enable PSY 
	CPosModules* idb = CPosModules::OpenL();
	CleanupStack::PushL(idb);
	_LIT(KEnablePSY, "Enabling Test Status PSY");
	INFO_PRINTF1(KEnablePSY);
    EnablePsyL(*idb, iUidTestStatusPsy);
	
	// Event timer
	ieventtimer = CT_LbsClientEventTimer::NewL(Logger());
	ieventtimer->SetObserver(this);

	// 3 clients
	iclient1 = CT_LbsClientRequestor::NewL(iUidTestStatusPsy, this, KClient1, Logger());
	iclient2 = CT_LbsClientRequestor::NewL(iUidTestStatusPsy, this, KClient2, Logger());
	iclient3 = CT_LbsClientRequestor::NewL(iUidTestStatusPsy, this, KClient3, Logger());
	
	// Make a request with client 2
	_LIT(KRequestClient2, "Calling StartRequestL for client2");
	INFO_PRINTF1(KRequestClient2);
	iclient2->StartRequestL(usedefaultpsy);
	
	// Make client3 listen for status events
	iclient3->RequestModuleStatusEvent();

	// Disable Psy 3
	User::After(100000); //a small delay
	_LIT(KDisablePSY, "Disabling Test Status PSY");
	INFO_PRINTF1(KDisablePSY);
	DisablePsyL(*idb, iUidTestStatusPsy);
	
	// Client1 reads Module Status
	// Client API
	TInt clientapistatus = iclient1->ReadModuleStatusClientApi(iUidTestStatusPsy);
	_LIT(KClientApi, "Module Status from Client API: %d.");
	TBuf<100> bufclientapi;
	bufclientapi.Format(KClientApi, clientapistatus);
	INFO_PRINTF1(bufclientapi);
	
	// Modules API
	TInt modulesapistatus = iclient1->ReadModuleStatusModulesApi(idb, iUidTestStatusPsy);
	_LIT(KModulesApi, "Module Status from Module API: (IsAvailable=) %d.");
	TBuf<100> bufmodulesapi;
	bufmodulesapi.Format(KModulesApi, modulesapistatus);
	INFO_PRINTF1(bufmodulesapi);
	// Compare received statuses
	if ( !((modulesapistatus == 0) && (clientapistatus == TPositionModuleStatus::EDeviceDisabled)) )
		{
		iErrorDuringExecution = TRUE;
		ERR_PRINTF1(KErrStatusMismatch);
		}

	iclient2->ClosePositioner();

	delete iclient1;
	delete iclient2;
	delete iclient3; 

	// Default PSY //
	
	// Do the same as before but now use the default PSY
	iNoFurtherEvents = FALSE;	// Client should receive events!
	_LIT(KDefaultPsyStart, "****** Testing Default PSY ******");
	INFO_PRINTF1(KDefaultPsyStart);

	// Enable Psy
	INFO_PRINTF1(KEnablePSY);
	EnablePsyL(*idb, iUidTestStatusPsy);

	// 3 clients
	iclient1 = CT_LbsClientRequestor::NewL(iUidTestStatusPsy, this, KClient1Default, Logger());
	iclient2 = CT_LbsClientRequestor::NewL(iUidTestStatusPsy, this, KClient2Default, Logger());
	iclient3 = CT_LbsClientRequestor::NewL(iUidTestStatusPsy, this, KClient3Default, Logger());

	// Make a request with client 2
	INFO_PRINTF1(KRequestClient2);
	iclient2->StartRequestL(usedefaultpsy);
	
	// Make client3 listen for status events
	iclient3->RequestModuleStatusEvent();

	// Disable Psy 3
	User::After(1000000); //A small delay
	usedefaultpsy = TRUE; //Use default PSY
	INFO_PRINTF1(KDisablePSY);
	DisablePsyL(*idb, iUidTestStatusPsy);
				
	// Client1 reads Module Status
	// Client API
	clientapistatus = iclient1->ReadModuleStatusClientApi(iUidTestStatusPsy);
	bufclientapi.Format(KClientApi, clientapistatus);
	INFO_PRINTF1(bufclientapi);
	
	// Modules API
	modulesapistatus = iclient1->ReadModuleStatusModulesApi(idb, iUidTestStatusPsy);
	bufmodulesapi.Format(KModulesApi, modulesapistatus);
	INFO_PRINTF1(bufmodulesapi);
	// Compare received statuses
	if ( !((modulesapistatus == 0) && (clientapistatus == TPositionModuleStatus::EDeviceDisabled)) )	
		{
		iErrorDuringExecution = TRUE;
		ERR_PRINTF1(KErrStatusMismatch);
		}
	
	iclient2->ClosePositioner();

	delete iclient1;
    iclient1 = NULL;
	delete iclient2;
    iclient2 = NULL;
	delete iclient3;
    iclient3 = NULL;
 
	// Connect to EPos
    User::LeaveIfError(iPosServer.Connect());

	// Enable Psy
	INFO_PRINTF1(KEnablePSY);
	EnablePsyL(*idb, iUidTestStatusPsy);
	
    // Open subsession to Status Psy that still is disabled!!!
	_LIT(KInfoStatusPSY, "Calling OpenPositionerByName for Test Status PSY");
    INFO_PRINTF1(KInfoStatusPSY);  
	TInt err = OpenPositionerByName(iUidTestStatusPsy);
	User::After(100000); //A small delay

	if (err != KErrNone)
		{
		_LIT(KErrorRequest, "OpenPositionerByName failed, errCode = %d");
		TBuf<100> buf;
		buf.Format(KErrorRequest, err);
		LogErrorAndLeaveL(buf);
		}
	
	// Make a position request
	_LIT(KService, "service");
	TPositionInfo positionInfo = TPositionInfo();
	_LIT(KPerformSyncRequest, "Calling PerformSyncRequest");
	INFO_PRINTF1(KPerformSyncRequest);
    err = PerformSyncRequest(KService, &positionInfo);

    if (err != KErrNone)
        {
        _LIT(KErrorRequest, "The last request was not completed with KErrorNone, errCode = %d");
		TBuf<100> buf;
		buf.Format(KErrorRequest, err);
        LogErrorAndLeaveL(buf);
        }
	
	//Close Positioner
	ClosePositioner();

	//Disconnect from Epos
	Disconnect();

	CleanupStack::PopAndDestroy(idb);

	//Checking if any errors occured during execution
	if (iErrorDuringExecution)
		{
		_LIT(KErrorAndLeave, "Error(s) occured during execution, closing test!");
		LogErrorAndLeaveL(KErrorAndLeave);
		}
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp213::EnablePsyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp213::EnablePsyL(
    CPosModules&   aDb,
    TUid             aPsy)
    {
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
    // Enable Psy
    moduleUpdate->SetUpdateAvailability(ETrue);
    aDb.UpdateModuleL(aPsy, *moduleUpdate);
    CleanupStack::PopAndDestroy(moduleUpdate);
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp213::DisablePsyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp213::DisablePsyL(
    CPosModules&   aDb,
    TUid             aPsy)
    {
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
    // Disable Psy
    moduleUpdate->SetUpdateAvailability(EFalse);
    aDb.UpdateModuleL(aPsy, *moduleUpdate);
    CleanupStack::PopAndDestroy(moduleUpdate);
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp213::RequestCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp213::RequestCompleteL()
    {
	if (iclient2->CompletionCode() != KNoEvent)
		{
		_LIT(KClient2Compl, "Client 2 request completed with %d.");
		TBuf<100> bufclient2comp;
		bufclient2comp.Format(KClient2Compl, iclient2->CompletionCode());
		INFO_PRINTF1(bufclient2comp);
		
		if (iclient2->CompletionCode() != KErrCancel)
			{
			TBuf<80> errorMessage;
			errorMessage.Format( KCanceledRequest, iclient2->CompletionCode() );
			ERR_PRINTF1( errorMessage );
			}

		/*Testing if PSY is unloaded from memory. PSY created a temp-file that only should 
			exist during its lifespan. */
		User::After(5000000);
		RFs fileserver;
		RFile file;
		User::LeaveIfError(fileserver.Connect());
		TInt err = file.Open(fileserver, KDummyFileName, EFileWrite);
		if (err == KErrNone || err == KErrInUse) 
            {
            ERR_PRINTF1(KFileNotDeleted);
            }
		file.Close();
		fileserver.Close();

		iclient2->ClosePositioner();
		iclient2->ResetCompletionCode();
		}

	else if (iclient3->CompletionCode() != KNoEvent)
		{	
		iclient3->ResetCompletionCode();
		_LIT(KClient3Compl, "Client 3 request completed, ");
		INFO_PRINTF1(KClient3Compl);

		if (iNoFurtherEvents) //Client 3 received event but should NOT have!
			{	
			iErrorDuringExecution = TRUE;
			ERR_PRINTF1(KEventReceived);
			}

		//Read what event was received!
		TInt eventReading = iclient3->ReadModuleStatusEvent();

		if (eventReading != TPositionModuleStatus::EDeviceDisabled) //NO Disable Event!
			{	
			_LIT(KClientStatusRead, "Event received by client not EDeviceDisabled, but %d.");
			TBuf<100> bufstatusread;
			bufstatusread.Format(KClientStatusRead, eventReading);
			INFO_PRINTF1(bufstatusread);

			//Make client3 listen for status events again!!
			iclient3->ContinueRequesting();
			}
		else //A Disable Event!
			{
			_LIT(KClient3ComplDisabled, "Client 3 received EDeviceDisabled event!");
			INFO_PRINTF1(KClient3ComplDisabled);

			//Make client3 listen for status events again!!
			iclient3->ContinueRequesting();
			
			/*Wait 5 s for events before halting the scheduler. 
				No further events for client 3 should be received! */
			ieventtimer->Start(5000000);
			iNoFurtherEvents = TRUE;
			//Start Timer
			_LIT(KStartTiming, "Start Timer, 5 s.");
			INFO_PRINTF1(KStartTiming);
			}
		}
	else if (ieventtimer->CompletionCode() != KNoEvent) //Time-out
		{
		ieventtimer->ResetCompletionCode() ;

		//Stop Scheduler
		_LIT(KTimeOut, "Timer Stopped.");
		INFO_PRINTF1(KTimeOut);
		CActiveScheduler::Stop();
		}
	else 
		{
		_LIT(KUnknownEvent, "Unknown request was completed!");
		LogErrorAndLeaveL(KUnknownEvent);
		}
    }

// End of File
