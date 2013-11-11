// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/

#include <e32base.h>
#ifdef SYMBIAN_FEATURE_MANAGER
	#include <featdiscovery.h>
	#include <featureuids.h>
#endif
// LBS-specific
#include <lbs.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/lbsx3p.h>


#include "lbsdevloggermacros.h"
#include "lbsrootapi.h"
#include "privacyandlocationrequesthandler.h"
#include "x3phandler.h"
#include "ngmessageswitch.h"
#include "lbsnetworkrequesthandler.h"
#include "lbsnrhserver.h"
#include "agpsinterfacehandler.h"

CLbsNetworkRequestHandler::CLbsNetworkRequestHandler(CNrhServer* aNrhServer)
:	iNrhServer(aNrhServer)
    {
    }
CLbsNetworkRequestHandler::~CLbsNetworkRequestHandler()
    {
#if defined(_DEBUG)
    if(iOomListener)
    	{
    	iOomListener->Cancel();
    	delete iOomListener;
    	}
#endif

   	iNrhServer->SetPrivacyServerObserver(NULL);

	delete iX3pHandler;
	delete iPrivLocHandler;
	delete iNgMessageSwitch;
	iNetRegStatus.Close();
	delete iAdmin;
    }
    
CLbsNetworkRequestHandler* CLbsNetworkRequestHandler::NewL(CNrhServer* aNrhServer)
    {
    CLbsNetworkRequestHandler* self = new(ELeave) CLbsNetworkRequestHandler(aNrhServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return(self);
    }
    
void CLbsNetworkRequestHandler::ConstructL()
    {
	LBSLOG_BEGIN();
    // Create the LbsAdmin. 
    // It would be possible to do this every time a value
    // is needed, but that would give the potential for OOM
    // conditions, so do it once and pass the pointer to the
    // sub-components that need it.
    
    iAdmin = CLbsAdmin::NewL();
    
    // Connect to the internal network registration (roaming) status interface
    iNetRegStatus.OpenL();
    
    // Create the Network Gateway Message Switch
    iNgMessageSwitch = CNGMessageSwitch::NewL(*iAdmin);

    //Create the Privacy and Location Handler
    iPrivLocHandler = CPrivacyAndLocationHandler::NewL(*iNgMessageSwitch, 
    													*iAdmin,
    													iNetRegStatus);

    iNrhServer->SetPrivacyServerObserver(iPrivLocHandler->PrivacyHandler());

#if defined __WINSCW__ && defined SYMBIAN_CELLMO_CENTRIC
	TBool locationManagementSupported(EFalse);
#else
#ifdef SYMBIAN_FEATURE_MANAGER
	TBool locationManagementSupported = CFeatureDiscovery::IsFeatureSupportedL(NFeature::KLocationManagement);
#else
	TBool locationManagementSupported(ETrue);
#endif // SYMBIAN_FEATURE_MANAGER
#endif // __WINSCW__ && defined SYMBIAN_CELLMO_CENTRIC
	
	if(locationManagementSupported)
		{
		iX3pHandler = CX3pHandler::NewL(*iNgMessageSwitch, iPrivLocHandler->X3pStatusHandler(), *iAdmin);
		}

    // Let the server know about the X3P Handler
    iNrhServer->SetX3pServerObserver(iX3pHandler);
    
#if defined(_DEBUG)
	// For OOM testing. The listener will force an error on the next heap 
	// allocation when it is kicked by test code.
	iOomListener = CLbsOomListener::NewL();
	iOomListener->StartGettingRequests();
#endif 
    }

CLbsAdmin* CLbsNetworkRequestHandler::Admin() const
	{
	return iAdmin;
	}

RLbsNetworkRegistrationStatus& CLbsNetworkRequestHandler::NetworkRegistrationStatus()
	{
	return iNetRegStatus;
	}
