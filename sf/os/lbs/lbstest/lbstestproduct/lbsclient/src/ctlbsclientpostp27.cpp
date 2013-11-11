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
#include "ctlbsclientpostp27.h"
#include "ctlbsposclientholder.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <e32keys.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp27::CT_LbsClientPosTp27(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName,"TP27 - Cancel Request");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp27::~CT_LbsClientPosTp27()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp27::CloseTest
// Always called after the test, even if the test leaves
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp27::CloseTest()
    {
    ClosePositioner();
	Disconnect();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp27::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp27::StartL()
    {
    SetupPsyL(iUidDynDbTestPsy);
    TTimeIntervalMicroSeconds timeLimit(5000000);
    ConnectL();
    User::LeaveIfError(OpenPositioner());

    TPositionInfo posInfo = TPositionInfo();
    _LIT(KService, "service");
    TInt err = PerformSyncRequest(KService, &posInfo);

    if (err != KErrNone)
        {
        _LIT(KErrorRequest, "The request was not completed with KErrorNone");
        LogErrorAndLeaveL(KErrorRequest);
        }
	ClosePositioner();
	Disconnect();
	//Cancel quick (most likely before the PSY gets involved
	MakeCancelRequestL(TTimeIntervalMicroSeconds32(100), timeLimit );
	//Cancel after 1 second so that the PSY gets involved.
	MakeCancelRequestL(TTimeIntervalMicroSeconds32(1000000), timeLimit);

	CheckCancelRequestL();

    }

// ---------------------------------------------------------
// CT_LbsClientPosTp27::MakeCancelRequestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp27::MakeCancelRequestL(TTimeIntervalMicroSeconds32 aCancelTime, TTimeIntervalMicroSeconds aTimeLimit)
	{	
	const TInt KNrOfClients = 1;
    CT_LbsPosClientHolder* clients = CT_LbsPosClientHolder::NewLC(KNrOfClients);

    clients->SetRequesterWillCancelL();
	clients ->SetCancelTime(aCancelTime);
    clients->MakeRequests();

    TInt status;

    TPositionInfo myModuleInfo;
    TTimeIntervalMicroSeconds reqTime;
    clients->GetResult(status, myModuleInfo, reqTime);

    if (status != KErrCancel)
        {
        _LIT(KReqWasNotCancelled, "The request was not cancelled, return code %d");
        TBuf<100> buf;
        buf.Format(KReqWasNotCancelled, status);
        LogErrorAndLeaveL(buf);
        }
   
    if (reqTime > aTimeLimit)
        {
        _LIT(KError, "The request took too long, %d microsecs. It was probably never cancelled");
        TBuf<100> buf;
        buf.Format(KError, reqTime.Int64());
        LogErrorAndLeaveL(buf);
        }

    _LIT(KMsg, "The request was cancelled after %d microsecs.");
    TBuf<100> buf;
    buf.Format(KMsg, reqTime.Int64());
    INFO_PRINTF1(buf);
    CleanupStack::PopAndDestroy(clients); //clients
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp27::MakeCancelRequest2L
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp27::MakeCancelRequest2L(TTimeIntervalMicroSeconds32 aCancelTime, TTimeIntervalMicroSeconds aTimeLimit)
	{	
	_LIT(KCancelRequest,"MakeCancelRequest2L");
	INFO_PRINTF1(KCancelRequest);

	RFs fs;
	TInt err = fs.Connect();
    CleanupClosePushL(fs);

    DisableAllPsyL();
	ConnectL();
	// TestPsy9 waits for 6 sec before returning a position
    EnablePsyL(iUidTestPsy9);
    err = OpenPositionerByName(iUidTestPsy9);
	if (err != KErrNone)
		{
		TBuf<150> buf;
		_LIT(KErr, "OpenPositionerByName failed with %d");
		buf.Format(KErr, err);
		LogErrorAndLeaveL(buf);
		}
	
	_LIT(KPerformRequest,"The request info");
	PerformRequestL(KPerformRequest, CRequestor::ERequestorService, CRequestor::EFormatApplication);

	const TInt KNrOfClients = 2;
	// TestPsy6 waits for 5 sec before returning a position
    EnablePsyL(iUidTestPsy6);
    CT_LbsPosClientHolder* clients = CT_LbsPosClientHolder::NewLC(KNrOfClients, iUidTestPsy6, ETrue);

	clients->SetRequesterWillCancelL();
	clients->SetCancelTime(aCancelTime);

    clients->MakeRequests();
	CheckRequestResultL(KErrNone);

    TInt status;
    TPositionInfo myModuleInfo;
    TTimeIntervalMicroSeconds reqTime;
    clients->GetResult(status, myModuleInfo, reqTime);

    if (status != KErrCancel)
        {
        _LIT(KReqWasNotCancelled, "The request was not cancelled, return code %d");
        TBuf<100> buf;
        buf.Format(KReqWasNotCancelled, status);
        LogErrorAndLeaveL(buf);
        }

    if (reqTime > aTimeLimit)
        {
        _LIT(KError, "The request took too long, %d microsecs. It was probably never cancelled");
        TBuf<100> buf;
        buf.Format(KError, reqTime.Int64());
        LogErrorAndLeaveL(buf);
        }

    _LIT(KMsg, "The request was cancelled after %d microsecs.");
    TBuf<100> buf;
    buf.Format(KMsg, reqTime.Int64());
    INFO_PRINTF1(buf);
    
	ClosePositioner();
	Disconnect();

	//Check request log
	RArray<TInt> arrayId;
	CleanupClosePushL(arrayId);
	err = KErrNone;

	if (err != KErrNone)
		{
		TBuf<150> buf;
		_LIT(KErr, "GetLogEntryListL() returned error, %d");
		buf.Format(KErr, err);
		LogErrorAndLeaveL(buf);
		}

	// All requests should have been logged, including the cancelled requests
	TInt numberOfElements = arrayId. Count();

	if (numberOfElements != 3)
	{
	buf.Zero();
	_LIT(KErrLog,"ERROR: The log contains wrong number of entries, was %d");
	buf.Format(KErrLog, numberOfElements);
	INFO_PRINTF1(buf);
	_LIT(KErrAndLeave, "Cancelled request has not been logged!!!");
	LogErrorAndLeaveL(KErrAndLeave);
	}

	CleanupStack::PopAndDestroy(&arrayId);
	CleanupStack::PopAndDestroy(clients); //clients
	CleanupStack::PopAndDestroy(1); // privCtrl
	CleanupStack::PopAndDestroy(1); //fs
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp27::CheckCancelRequestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp27::CheckCancelRequestL()
	{
	_LIT(KFunctionName, "We are inside CheckCancelRequestL");
	INFO_PRINTF1(KFunctionName);

	RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

	// TestPsy9 waits for 6 sec before returning a position
	SetupPsyL(iUidTestPsy9);
	TTimeIntervalMicroSeconds32 cancelTime(2000000); // 2 sec
	const TInt KNrOfClients = 1;
    CT_LbsPosClientHolder* clients = CT_LbsPosClientHolder::NewLC(KNrOfClients, iUidTestPsy9);

    clients->SetRequesterWillCancelL();
	clients->SetCancelTime(cancelTime);
    clients->MakeRequests();

    TInt status;

    TPositionInfo myModuleInfo;
    TTimeIntervalMicroSeconds reqTime;
    clients->GetResult(status, myModuleInfo, reqTime);

    if (status != KErrCancel)
        {
        _LIT(KReqWasNotCancelled, "The request was not cancelled, return code %d");
        TBuf<100> buf;
        buf.Format(KReqWasNotCancelled, status);
        LogErrorAndLeaveL(buf);
        }

    _LIT(KMsg, "The request was cancelled after %d microsecs.");
    TBuf<100> buf;
    buf.Format(KMsg, reqTime.Int64());
    INFO_PRINTF1(buf);

	//Check request log
	RArray<TInt> arrayId;
	CleanupClosePushL(arrayId);
	
	CleanupStack::PopAndDestroy(&arrayId);
	CleanupStack::PopAndDestroy(clients); //clients
	CleanupStack::PopAndDestroy(1); // privCtrl
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp27::DisableAllPsyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp27::DisableAllPsyL()
    {
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    // Disable PSY
    moduleUpdate->SetUpdateAvailability(EFalse);
    CPosModuleIdList* prioList = db->ModuleIdListLC();

    // Disable all PSY:s
    for (TInt i = 0 ; i < prioList->Count(); i++)
        {
        db->UpdateModuleL((*prioList)[i], *moduleUpdate);
        }

    CleanupStack::PopAndDestroy(prioList);
    CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp27::DisableAllPsyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp27::EnablePsyL(const TUid aPsyUid)
    {
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    // Enable the PSY that came as an in parameter
    moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(aPsyUid, *moduleUpdate);

    CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
    }
