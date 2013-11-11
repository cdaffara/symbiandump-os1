// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Object to monitor the ETel network registration status and 
// broadcast it to the other components of the LBS sub-system.
// 
//

/**
 @file
 @internalComponent
 @released
*/
#ifndef NETREGSTATUSMONITOR_H_
#define NETREGSTATUSMONITOR_H_

#include <etelmm.h>
#include <commsdattypesv1_1.h>
#include "lbsnetregstatusint.h"

/** Observer class used internally inside the NG.

Inside the network gateway, this class is used instead of reading the 
RProperty for the network registration status, just because it's quicker
than having to wait for the notification that the property has changed.
*/
class MNetworkRegistrationStatusObserver
	{
public:
	virtual void OnNetworkRegistrationStatusChange(
			RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus aStatus) = 0;
	};

/** Monitors the ETel network registration status and broadcasts it to other LBS components.

Using the RMobilePhone interface, this class monitors the network registration
status of the 'default' phone on the device. Any changes in state are
broadcast on a RProperty that other interested components can subscribe to.
*/
class CNetworkRegistrationStatusMonitor : public CActive
	{
public:
	static CNetworkRegistrationStatusMonitor* NewL(MNetworkRegistrationStatusObserver& aObserver);
	~CNetworkRegistrationStatusMonitor();
	
	void MonitorNetworkRegistrationStatus();
	
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus GetCurrentNetworkRegistrationStatus();
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus GetLastGoodNetworkRegistrationStatus();

protected:
	CNetworkRegistrationStatusMonitor();
	CNetworkRegistrationStatusMonitor(MNetworkRegistrationStatusObserver& aObserver);
	void ConstructL();

	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	void GetCommDbTSYnameL(TDes& aTsyName);
	void InitialisePhoneL();
	void SetupPhoneL();
	
#ifdef ENABLE_LBS_DEV_LOGGER
	const TDesC& NetRegStatusToDesC(RMobilePhone::TMobilePhoneRegistrationStatus iRegStatus);
	const TDesC& LbsNetRegStatusToDesC(RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus iRegStatus);
#endif
	
private:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	RMobilePhone::TMobilePhoneRegistrationStatus iPhoneNetRegStatus;
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus iCurrentNetRegStatus;
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus iLastGoodNetRegStatus;
	RLbsNetworkRegistrationStatus iLbsNetRegStatus;
	RProperty iConfigurationProperty;
	TBool iInitialised;
	MNetworkRegistrationStatusObserver& iObserver;
	};

#endif // NETREGSTATUSMONITOR_H_
