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
#include "ctlbsclientpostp179.h"
#include "ctlbsposclientholder.h"
#include <e32keys.h>

// CONSTANTS
_LIT(KPosPhoneTimeLocaleFormat,
     "         phone time: %F%H:%F%T:%F%S:%F%C");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp179::CT_LbsClientPosTp179(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{ 
	_LIT(KTestName, "Tp179 - One Client, same PSY");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp179::~CT_LbsClientPosTp179()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp179::CloseTest
// Always called after the test, even if the test leaves
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp179::CloseTest()
    {
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp179::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp179::StartL()
    {  
    const TInt NumClients = 3;
	SetupPsyL(iUidTestPsy12);

    //Make one request, the performance requirement are not valid for
    //the first request.
    TTime startTime, stopTime;
    TTime serverStartTime, serverStopTime;
    TTimeIntervalMicroSeconds requestTime, connectionTime;
    TBuf<150> buf;

    serverStartTime.UniversalTime();
    ConnectL();
    serverStopTime.UniversalTime();

    User::LeaveIfError(OpenPositioner());

    TPositionInfo posInfo = TPositionInfo();
    _LIT(KService, "service");
    // Request data from default psy should be test psy 2
    startTime.UniversalTime();
    TInt err = PerformSyncRequest(KService, &posInfo);
    stopTime.UniversalTime();

    if (err != KErrNone)
        {
        _LIT(KErrorRequest, "The request was not completed with KErrorNone");
        LogErrorAndLeaveL(KErrorRequest);
        }

    requestTime = stopTime.MicroSecondsFrom(startTime);
    connectionTime = serverStopTime.MicroSecondsFrom(serverStartTime);

    // Added some extra timing here
    _LIT(KDebug, "Connecting to Epos took: %d us");
    buf.Format(KDebug, connectionTime.Int64());
    INFO_PRINTF1(buf);
    _LIT(KSeparator, "********");
    INFO_PRINTF1(KSeparator);
    buf.Zero();
    _LIT(KFirstRequest, "The first request took: %d us");
    buf.Format(KFirstRequest, requestTime.Int64());
    INFO_PRINTF1(buf);
    INFO_PRINTF1(KSeparator);

    CT_LbsPosClientHolder* clients = CT_LbsPosClientHolder::NewLC(NumClients, iUidTestPsy12, ETrue);
    
    TTime starttime,stoptime;
    
    starttime.UniversalTime();
    
    clients->MakeRequests();
    
    stoptime.UniversalTime();
    
     // Substract total delay incurred due to TestPsy2...i.e 2sec
    TInt duration=stoptime.Int64()-starttime.Int64()-2000000;
    
    _LIT(KMessage,"3 simultaneous requests from 3 different sub-sessions with a psy completed in %d microsec");
    TBuf<256> message;
    message.Format(KMessage,duration);
    INFO_PRINTF1(message);
    
    if(duration>3000000)
    {
     _LIT(KTime, "Time taken>3 sec");
     INFO_PRINTF1(KTime);
    }
    
    TPositionInfo position[NumClients];

    TTimeIntervalMicroSeconds
        reqTime[NumClients],
        reqTimeLimit(5000000),
        timeDifferenceLimit(500000);
    TInt status;
    TBool error = EFalse;

	//some printing mostly used for information purpose.
    for (TInt i = 0; i < NumClients; i++)
        {
        clients->GetResult(status, position[i], reqTime[i], i);
		TPosition tmp;
		position[i].GetPosition(tmp);
        if (status != 0)
            {
            _LIT(KError, "The position requested could not be retrieved, status = %d");
            buf.Format(KError, status);
            ERR_PRINTF1(buf);
            error = ETrue;
            }
        if (reqTime[i].Int64() > reqTimeLimit.Int64())
            {
            _LIT(KErrorMsg, "The request time of client %d took more than 5 seconds, req. time = %d microsecs.");
            buf.Format(KErrorMsg, i, reqTime[i].Int64());
            ERR_PRINTF1(buf);
            error = ETrue;
            }
        for (TInt j = 0; j < i; j++)
            {
            _LIT(KReqMsg, "client %d request time: %d");

            buf.Format(KReqMsg, j, reqTime[j].Int64());
            INFO_PRINTF1(buf);
			TPosition posi;
			TPosition posj;
			position[i].GetPosition(posi);
			position[j].GetPosition(posj);
            posj.Time().FormatL(buf, KPosPhoneTimeLocaleFormat);
            INFO_PRINTF1(buf);

            buf.Format(KReqMsg, i, reqTime[i].Int64());
            INFO_PRINTF1(buf);
            posi.Time().FormatL(buf, KPosPhoneTimeLocaleFormat);
            INFO_PRINTF1(buf);

            _LIT(KSeparator, "********");
            INFO_PRINTF1(KSeparator);

            if (Abs(reqTime[i].Int64() - reqTime[j].Int64()) > timeDifferenceLimit.Int64())
                {
                _LIT(KMsg1, "Requests by client %d and %d wasn't served simultaniously: %d microsecs difference between reqTime.");
                TBuf<150> buf;
                buf.Format(KMsg1, i, j, Abs(reqTime[i].Int64() - reqTime[j].Int64()));
                ERR_PRINTF1(buf);
                error = ETrue;
                }
            if (Abs(posi.Time().Int64() - posj.Time().Int64()) > timeDifferenceLimit.Int64())
                {
                _LIT(KMsg2, "Requests by client %d and %d wasn't served simultaniously: %d microsecs difference between phoneTime.");
                TBuf<150> buf;
                buf.Format(KMsg2, i, j, Abs(posi.Time().Int64() - posj.Time().Int64()));
                ERR_PRINTF1(buf);
                error = ETrue;
                }
            }
        }

    CleanupStack::PopAndDestroy(); //clients
    ClosePositioner();

	Disconnect();

    if (error)
        User::Leave(KErrGeneral);
    }
