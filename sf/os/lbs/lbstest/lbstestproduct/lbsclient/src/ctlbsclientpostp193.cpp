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
#include "ctlbsclientpostp193.h"
#include "ctlbsposclientholder.h"
#include <e32keys.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp193::CT_LbsClientPosTp193(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{
	_LIT(KTestName, "TP193 - Maximum number of Clients");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp193::~CT_LbsClientPosTp193()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp193::CloseTest
// Always called after the test, even if the test leaves
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp193::CloseTest()
    {
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp193::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp193::StartL()
    { 
	SetupPsyL(iUidTestPsy2);
	
	User::After(2000000);

	const TInt KNrOfClients = 25;
	CT_LbsPosClientHolder* clients = CT_LbsPosClientHolder::NewLC(KNrOfClients,iUidTestPsy2);	
    
    TTime starttime,stoptime;
    
    starttime.UniversalTime();
    
    clients->MakeRequests();
    
    stoptime.UniversalTime();
    
    // Substract total delay incurred due to TestPsy2...i.e 2sec
    TInt duration=stoptime.Int64()-starttime.Int64()-2000000;
    
    _LIT(KMessage,"25 simultaneous requests from 25 different sessions with a psy completed in %d microsec");
    TBuf<256> message;
    message.Format(KMessage,duration);
    INFO_PRINTF1(message);
    
    if(duration>3000000)
	    {
	    _LIT(KTime, "Time taken>3 sec");
	    INFO_PRINTF1(KTime);
	    }

	//check the result of the request
	//only KErrNoMemory or KErrNone is allowed.
	// CONSTANTS
    
   TPositionInfo position[KNrOfClients];

    TTimeIntervalMicroSeconds reqTime[KNrOfClients];
    TInt status;
    TBool error = EFalse;
    TBuf<150> buf;

	//some printing mostly used for information purpose.
    for (TInt i = 0; i < KNrOfClients; i++)
        {
        clients->GetResult(status, position[i], reqTime[i], i);
        if (status != 0)
            {
            _LIT(KError, "The position requested could not be retrieved, status = %d");
            buf.Format(KError, status);
            ERR_PRINTF1(buf);
            if (status != -4)
                {
                error = ETrue;
                }
            }
            _LIT(KReqMsg, "client %d request time: %d");

            buf.Format(KReqMsg, i, reqTime[i].Int64());
            INFO_PRINTF1(buf);
            _LIT(KSeparator, "********");
            INFO_PRINTF1(KSeparator);
        }

    if (error)
        {
        _LIT(KErrorCode, "The requests was not completed with KErrNoMemory or KErrNone");
        LogErrorAndLeaveL(KErrorCode);
        }
    CleanupStack::PopAndDestroy(clients); //clients
    }
