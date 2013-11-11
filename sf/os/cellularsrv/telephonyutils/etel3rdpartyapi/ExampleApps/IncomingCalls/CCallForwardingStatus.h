// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CCALLFORWARDINGSTATUS_H__
#define __CCALLFORWARDINGSTATUS_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves the status of the network call forwarding supplementary service.
*/
class CCallForwardingStatus : public CISVAPIAsync
	{

// Methods
public:
	static CCallForwardingStatus* NewL(MExecAsync* aController);
	~CCallForwardingStatus();

	void DoStartRequestL();

private:
	CCallForwardingStatus(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores current call forwarding status when call status is checked for 
	CTelephony::ECallForwardingNoReply.
	*/
	CTelephony::TCallForwardingSupplServicesV1 iCallForwardingStatus1V1;
	/**
	Stores current call forwarding status when call status is checked for 
	CTelephony::ECallForwardingUnconditional.
	*/
	CTelephony::TCallForwardingSupplServicesV1 iCallForwardingStatus2V1;

private:
	/**
	Package descriptor for iCallForwardingStatus1V1.
	*/
	CTelephony::TCallForwardingSupplServicesV1Pckg iCallForwardingStatus1V1Pckg;
	/**
	Package descriptor for iCallForwardingStatus2V1.
	*/
	CTelephony::TCallForwardingSupplServicesV1Pckg iCallForwardingStatus2V1Pckg;
	/**
	When set to EFalse, DoStartRequestL() will check if the call forwarding 
	status is CTelephony::ECallForwardingUnconditional. When set to ETrue, 
	DoStartRequestL() will check if the call forwarding status is 
	CTelephony::ECallForwardingNoReply
	*/
	TBool iSecondCondition;
	};

#endif // __CCALLFORWARDINGSTATUS_H__
