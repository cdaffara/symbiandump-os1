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
#include "ctlbssimulationpsypostp221.h"

// CONSTANTS
#ifdef __WINS__
  // Nmea raw data files
  _LIT(KMagellanGPS315, "z:\\system\\test\\testdata\\MagellanGPS315.nme");
  //Ini files to corresponding nmea raw data file
#else
   // Nmea raw data files
  _LIT(KMagellanGPS315, "c:\\system\\test\\testdata\\MagellanGPS315.nme");
  //Ini files to corresponding nmea raw data file
#endif


// ================= MEMBER FUNCTIONS =======================
  
// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsSimulationPSYPosTp221::CT_LbsSimulationPSYPosTp221(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{
	_LIT(KTestName, "TP221 - Reset of incoming request");
	SetTestStepName(KTestName);
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsSimulationPSYPosTp221::~CT_LbsSimulationPSYPosTp221()
	{
	}
    
// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp221::CloseTest()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp221::CloseTest()
	{
	ClosePositioner();
	Disconnect();
    }
    
// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp221::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp221::StartL()
    {
	TBool errorDuringExecution = FALSE;  // True if an error occurs
	
	//Connect to Epos
	ConnectL();
	//Set up PSY
    SetupPsyL(iUidSimulationPsy);
    _LIT(KServiceName, "SAAA");
	// Use correct nmea file
	SetSimDataFileL(KMagellanGPS315);

	//Open positioner
    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));

	//The position object. Don't request any fields!
	HPositionGenericInfo* positioninfo = HPositionGenericInfo::NewLC();
	
	//Setting up the position object:
	//Set update type
	positioninfo->SetUpdateType(EPositionUpdateUnknown);
	//Set "old data" that should be erased by Psy when making a new position request
	TInt8 numsats = 7; //just magic numbers...
	TReal32 speed = 32;
	User::LeaveIfError(positioninfo->SetValue(EPositionFieldSatelliteNumUsed, numsats));
	User::LeaveIfError(positioninfo->SetValue(EPositionFieldHorizontalSpeed, speed));
	
	//Make a position request!
	User::LeaveIfError(PerformSyncRequest(KServiceName, positioninfo));

	//Checking returned position object
	if (positioninfo->UpdateType() != EPositionUpdateGeneral)
		{
		_LIT( KWrongUpdateType, "Wrong Update Type! EPositionUpdateGeneral expected." );
		ERR_PRINTF1(KWrongUpdateType);
		errorDuringExecution = TRUE;
		}

	TInt err1 = positioninfo->GetValue(EPositionFieldSatelliteNumUsed, numsats);
	TInt err2 = positioninfo->GetValue(EPositionFieldHorizontalSpeed, speed);

	if (err1 != KErrNotFound || err2 != KErrNotFound)
		{
		_LIT( KValueReturned, "HPositionGenericInfo::GetValue() returned value!" );
		ERR_PRINTF1(KValueReturned);
		errorDuringExecution = TRUE;
		}

	//Close Positioner
	ClosePositioner();

	//Disconnect from Epos
	Disconnect();

	//Cleaning
	CleanupStack::PopAndDestroy(positioninfo);
	//Checking if any errors occured during execution
	if (errorDuringExecution)
		{
		_LIT(KErrorAndLeave, "Error(s) occured during execution, closing test!");
		LogErrorAndLeaveL(KErrorAndLeave);
		}
    }


