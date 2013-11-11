// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// SpudNetworkSideServer.cpp
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to
// be started in the process of the client. The client initialises the server
// by calling the one and only ordinal.
// 
//

/**
 @file
 @internalComponent
*/

#include "te_spudNetworkSideServer.h"
#include "te_spudNetworkSideSteps.h"
#ifdef SYMBIAN_NETWORKING_UMTSR5
#include "te_spudNetworkSideR5Steps.h"
#endif

using namespace te_spudNetworkSide;
_LIT(KServerName,"TE_SpudNetworkSide");




// EKA2 much simpler
// just an E32Main and a MainL()

/**
 * Much simpler, uses the new Rendezvous() call to sync with the client.
 *
 * @internalComponent
 *
 * @leave When new(ELeave) leaves 
 */
LOCAL_C void MainL()
	{
	// leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched = NULL;
	sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CSpudNetworkSideTestServer* server = NULL;
	// create the CTestServer derived server
	TRAPD(err,server = CSpudNetworkSideTestServer::NewL());
	if (!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// only a DLL on emulator for typhoon and earlier

/**
 * Main entry point.
 *
 * @internalComponent
 *
 * @return Standard Epoc error codes.
 */
GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier

/**
 * Called inside the MainL() function to create and start the CTestServer derived server.
 *
 * @internalComponent
 *
 * @return Instance of the test server.
 */
CSpudNetworkSideTestServer* CSpudNetworkSideTestServer::NewL()
	{
	CSpudNetworkSideTestServer * server = new (ELeave) CSpudNetworkSideTestServer();
	CleanupStack::PushL(server);
	
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}
	




CTestStep* CSpudNetworkSideTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* step = NULL;
	
		if(aStepName == KCESockCheck)
			{
			step = new CESockCheck;
			}
		else if(aStepName == KCSpudPrimary)
			{
			step = new CSpudPrimary;
			}
		else if(aStepName == KCSpudPrimaryEvent)
			{
			step = new CSpudPrimaryEvent;
			}
        else if(aStepName == KCSpudPrimaryContextEventProgress)
            {
            step = new CSpudPrimaryContextEventProgress;
            }
		else if(aStepName == KCSpudDeletePrimary)
			{
			step = new CSpudDeletePrimary;
			}
		else if(aStepName == KCSpudSecondary)
			{
			step = new CSpudSecondary;
			}
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		else if(aStepName == KCSpudSecondaryEvent)
			{
			step = new CSpudSecondaryEvent;
			}
		else if(aStepName == KCSpudTwoSecondaryEvents)
			{
			step = new CSpudTwoSecondaryEvents;
			}
		else if(aStepName == KCSpudSecondaryEvent2)
			{
			step = new CSpudSecondaryEvent2;
			}		
		else if(aStepName == KCSpudSecondaryChangeQoS)
			{
			step = new CSpudSecondaryChangeQoS;
			}
#endif
		else if(aStepName == KCSpudSuspendContext)
			{
			step = new CSpudSuspendContext;
			}
		else if(aStepName == KCESockCheck)
			{
			step = new CESockCheck;
			}
		else if(aStepName == KCSpudPrimaryInterfaceStop)
			{
			step = new CSpudPrimaryInterfaceStop;
			}
		else if(aStepName == KCSpudPrimaryInterfaceEventStop)
			{
			step = new CSpudPrimaryInterfaceEventStop;
			}
		else if(aStepName == KCSpudSecondaryInterfaceStop)
			{
			step = new CSpudSecondaryInterfaceStop;
			}
		else if(aStepName == KCSpudSecondaryInterfaceEventStop)
			{
			step = new CSpudSecondaryInterfaceEventStop;
			}
		else if(aStepName == KCSpudPrimaryDeletionInterfaceStop)
			{
			step = new CSpudPrimaryDeletionInterfaceStop;
			}
		else if(aStepName == KCSpudPrimayStartingStop)
			{
			step = new CSpudPrimayStartingStop;
			}
		else if(aStepName == KCSpudSecondaryStartingStop)
			{
			step = new CSpudSecondaryStartingStop;
			}
		else if(aStepName == KCSpudSecondaryLowerNifDownStop)
			{
			step = new CSpudSecondaryLowerNifDownStop;
			}
		else if(aStepName == KCSpudPrimaryEventOnLowerNifStarting)
			{
			step = new CSpudPrimaryEventOnLowerNifStarting;
			}
		else if(aStepName == KCSpudPppPrimary)
			{
			step = new CSpudPppPrimary;
			}
		else if(aStepName == KCSpudPppSecondarySend)
			{
			step = new CSpudPppSecondarySend;
			}
		else if(aStepName == KCSpudMultiPrimary)
			{
			step = new CSpudMultiPrimary;
			}
#ifdef SYMBIAN_NETWORKING_UMTSR5
		else if(aStepName == KCSpudQosR5Secondary)
			{
			step = new CSpudQosR5Secondary;
			}
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		else if(aStepName == KCSpudSecondaryQosR5Event)
			{
			step = new CSpudSecondaryQosR5Event;
			}
		else if(aStepName == KCSpudSecondaryChangeR5QoS)
			{
			step = new CSpudSecondaryChangeR5QoS;
			}
#endif
		else if(aStepName == KCSpudDeletePrimaryR5Qos)
			{
			step = new CSpudDeletePrimaryR5Qos;
			}	
#endif
      else if(aStepName == KCIoctlAddressRetrieve)
            {
            step = new CIoctlAddressRetrieve;
            }
      else if (aStepName == KCRawIpMinMaxMMU)
          {
          step = new CRawIpMinMaxMMU;
          }
	return step;	
	}











