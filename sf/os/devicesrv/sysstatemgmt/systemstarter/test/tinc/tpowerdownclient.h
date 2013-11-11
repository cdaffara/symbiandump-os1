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

#ifndef __TPOWERDOWNCLIENT_H
#define __TPOWERDOWNCLIENT_H

#include <savenotf.h>

/**
This is a test class with the same name of the friend class 'CPowerdownClient' of CSaveNotifier.
It helps to access the exported private method CSaveNotifier::ServerPowerState() to send a request
to query the power state of the shutdown server.
*/
class CPowerdownClient: public CBase
	{
public:
    static CPowerdownClient* NewL();
    ~CPowerdownClient();
	TInt QueryServerPowerState(CSaveNotifier* aNotifier, TBool& aPowerOff);
private:
    CPowerdownClient();
	};

#endif // __TPOWERDOWNCLIENT_H
