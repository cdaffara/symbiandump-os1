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
#include "ctlbssimulationpsypostp256.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================
  
// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsSimulationPSYPosTp256::CT_LbsSimulationPSYPosTp256(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{
	_LIT(KTestName, "TP256 - Measure TimeToFix for SimPSY (ESLI-65KCDT)");
	SetTestStepName(KTestName);
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsSimulationPSYPosTp256::~CT_LbsSimulationPSYPosTp256()
	{
	}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp256::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp256::StartL()
    {
	// Setup test environment
    ConnectL();
    SetupPsyL(iUidSimulationPsy);

	TestTimeToFixL();
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp256::CloseTest()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp256::CloseTest()
	{
    iPositioner.Close();
	iPosServer.Close();
	}


// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp256::TestTimeToFixL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp256::TestTimeToFixL()
    {
    _LIT(KFunctionName, "We are inside TestTimeToFixL()");
	INFO_PRINTF1(KFunctionName);

#ifdef __WINS__
    _LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationTTF.sps");
#else
    _LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationTTF.sps");
#endif

	_LIT(KService, "service");

	const TInt KAcceptedDiff = 200000; // Accepted difference is 200000 us
	// Do testing five times
	const TInt KNrOfFixes=5;
    _LIT(KError1, "Error when requesting position");
    _LIT(KStart, "Testing TestTimeToFixL()");
    _LIT(KEnd, "TestTimeToFixL() passed");
    INFO_PRINTF1(KStart);

	// Update registry to point to correct simulation data file
    SetSimDataFileL(KSimulationSource);
    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));

    TTime requestStart;
    // Request a pos. fix
    TPositionInfo posInfo = TPositionInfo();

	for (TInt i=0; i<KNrOfFixes; i++)
		{
		requestStart.UniversalTime();
		TInt err = PerformSyncRequest(KService, &posInfo);
		if (err != KErrNone)
			{
			LogErrorAndLeaveL(KError1);
			}
		
		// Measure power-up time
		TTime requestFinished;
		requestFinished.UniversalTime();
		TTimeIntervalMicroSeconds requestTime = 
			requestFinished.MicroSecondsFrom(requestStart);

		TBuf<150> buf;
		_LIT(KDebug, "Request time = %d us");
			buf.Format(KDebug, requestTime.Int64());

			INFO_PRINTF1(buf);

		TPosition tmp;
		posInfo.GetPosition(tmp);
		TTime reqTime = tmp.Time();

		TTimeIntervalMicroSeconds requestTime2 = 
			reqTime.MicroSecondsFrom(requestStart);

		_LIT(KDebug2, "Request time2 = %d us");
		buf.Format(KDebug2, requestTime2.Int64());

		INFO_PRINTF1(buf);

		// Verify that the timestamp of the positionInfo object is set when 
		// the position is returned and not when the position request is made
		// Accept a minor difference between the two
		if (Abs(requestTime.Int64()-requestTime2.Int64())>KAcceptedDiff )
			{
			_LIT(KErrorAndLeave, "Timestamp not correctly set");
			LogErrorAndLeaveL(KErrorAndLeave);
			}
		}

    ClosePositioner();
    INFO_PRINTF1(KEnd);
    }

//  End of File
