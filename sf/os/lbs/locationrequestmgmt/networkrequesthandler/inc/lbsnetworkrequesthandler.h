/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @internalComponent
 @released
*/

#ifndef _NETWORKREQUESTHANDLER_H 
#define _NETWORKREQUESTHANDLER_H

class CTestNetworkGateway;
class CLbsAdmin;
class CNGMessageSwitch;
class CPrivacyAndLocationHandler;
class CNrhServer;
class CX3pHandler;
class MNrhServerObserver;

#include "lbsrootapi.h"
#include "lbsnetregstatusint.h"

#if defined(_DEBUG)
#include "LbsOomListener.h"
#endif

class CLbsNetworkRequestHandler : public CBase
    {
public:
    static CLbsNetworkRequestHandler* NewL(CNrhServer* aNrhServer);
    ~CLbsNetworkRequestHandler();
    
public:
	CLbsAdmin* Admin() const;
	RLbsNetworkRegistrationStatus& NetworkRegistrationStatus();

protected:
    CLbsNetworkRequestHandler(CNrhServer* aNrhServer);
    void ConstructL();    
    
private:
	CLbsAdmin*                  iAdmin;
	CNGMessageSwitch*           iNgMessageSwitch;
	CPrivacyAndLocationHandler* iPrivLocHandler;
	// iNrhServer is used to register relevant subcomponent classes for
	// server callbacks 
	CNrhServer*					iNrhServer;
	CX3pHandler*				iX3pHandler;
	RLbsNetworkRegistrationStatus iNetRegStatus;
	
#if defined(_DEBUG)
	CLbsOomListener*			iOomListener;
#endif	
    };
    
#endif // _NETWORKREQUESTHANDLER_H    
