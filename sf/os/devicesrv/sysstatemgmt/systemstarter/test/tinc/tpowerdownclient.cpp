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
 @test
 @internalComponent - Internal Symbian test code
*/

#include "tpowerdownclient.h"

CPowerdownClient* CPowerdownClient::NewL()
    {
    CPowerdownClient* self = new (ELeave) CPowerdownClient;
    return self;
    }

/**
Constructor
*/
CPowerdownClient::CPowerdownClient()
    {
    }
    
/**
Destructor
*/
CPowerdownClient::~CPowerdownClient()
    {
    }
    
// With SYSLIBS_TEST macro defined, the method 'CSaveNotifier::ServerPowerState()' sends a request to query
// the power state of the shutdown server. The syslibs component 'pwrcli' has to be built properly with 
// this test macro defined inside the system HRH file under the folder 'epoc32\include\variant\' before
// running this test for ARMV5. Otherwise, it will cause the H4 board to crash.
#ifdef SYSLIBS_TEST
TInt CPowerdownClient::QueryServerPowerState(CSaveNotifier* aNotifier, TBool& aPowerOff)
	{

	return aNotifier->ServerPowerState(aPowerOff);
	
	}
#else
TInt CPowerdownClient::QueryServerPowerState(CSaveNotifier*, TBool&)
	{

	return 0;
	
	}
#endif


