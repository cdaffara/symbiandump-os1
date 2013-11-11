/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file te_agpsmanagerstepbase.h
*/

#if (!defined __TE_AGPSMANAGERSTEPBASE__)
#define __TE_AGPSMANAGERSTEPBASE__
#include <test/testexecutestepbase.h>
#include "lbsnetinternalapi.h"
#include "ctestgpsmodule.h"
#include "ctestgpsmoduleobserver.h"
#include "ctestagpsrequestsender.h"
#include "ctestagpstongobserver.h"
#include "ctestagpspositionsobserver.h"
#include "lbsmanagermainlogic.h"

const TUid KManagerTestSuiteUid = {0x102869CB};

/****************************************************************************
* Base Test Step class for te_agpsmangersuite test steps
* 
****************************************************************************/
class CTe_AgpsManagerStepBase : public CTestStep
	{
public:
	// Define properties keys needed for testing
	enum TTestPropertyKeys{ETestCapabilitiesKey};
public:
	virtual ~CTe_AgpsManagerStepBase();
	CTe_AgpsManagerStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	
protected:
	
	//-------------- Senders -----------------------------
	// To send requests to the AGPS Manager
	// as if they came from the Location Server
	CTestAgpsRequestSender* iLsToManagerRequestInjector;
	// To send requests to the AGPS Manager as if
	// they came from the Location Server
	CTestAgpsRequestSender* iNrhToManagerRequestInjector;
	// To send events to the AGPS Manager as if
	// they came from the GPS Module
	CTestGpsModule* iGpsToManagerMessageInjector;


	//------------- Observers-----------------------------
	// To observe messages sent by the AGPS Manager
	// to the GPS Module (through the CTestGspModule above)
	CTestGpsModuleObserver* iGpsModuleObserver;
	// To observe messages sent from the AGPS Manager to
	// the Network Gateway. The test should also use
	// this class for sending messages to the AGPS Manager
	// as if they came from the NRH (this is so because the
	// comms channel is bidirectional)
	CTestAgpsToNgObserver* iNetworkGatewayChannel;
	// To observe location updates published by the 
	// AGPS Manager for the benefit of LS and NRH
	CTestAgpsUpdatesObserver* iLocationUpdatesObserver;
	// To observe satellite measurements
	// published by the Manager for the NRH...not
	// currently required by any test
	// CTestAgpsUpdatesObserver* iMeasurementUpdates
	
	// Used to let the Manager know the hw capabilities
	// from the test step
	RProperty iCapabilitiesProperty;
	
private:

	CActiveScheduler* iActiveScheduler;
	
	// The system under test
	CManagerMainLogic* iAgpsManager;
	
	};

#endif
