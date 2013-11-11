/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CLBSLOCMONITORNETWORKINFOFINDER_H
#define CLBSLOCMONITORNETWORKINFOFINDER_H

#include <etelmm.h>
#include "clbslocmonitorareainfofinder.h"
#include "LbsLogger.h"


/**
Class used to keep track of the current global cell id

@internalComponent
@released
*/
class CLbsLocMonitorNetworkInfoFinder: public CLbsLocMonitorAreaInfoFinder
{

public:
	/** CLbsLocMonitorNetworkInfoFinder states.
	*/
	enum TLbsLocMonitorNetworkInfoFinderState
		{
		/** Not valid */
		EStateNull = 0,
		/** Waiting for Comms RootServer to be up and running */
		EStateInitializing = 1,
		/** Getting network info from ETEL with GetCurrentNetwork */
		EStateGettingNetInfo = 2,
		EStateNetInfoChangeMon = 3
		};

public:

	~CLbsLocMonitorNetworkInfoFinder();
	static CLbsLocMonitorNetworkInfoFinder* NewL();
	
	void StartGettingNetworkInfoL();

	
	virtual TInt RunError(TInt aError);
	virtual void RunL();
	virtual void DoCancel();

private:

	CLbsLocMonitorNetworkInfoFinder();
	void ConstructL();
	void GetNetworkInfoL();
	void InitialisePhoneL();
	TBool ValidateNetInfo(TLbsLocMonitorAreaInfoGci& areaInfo);
	void MonitorNetworkChange();
	void GetCommDbTSYnameL(TDes& aTsyName);


private:

	RTelServer iTelServer;
	RProperty iConfigurationProperty;

	/** iNetPhone monitors changes to Network Info */
	RMobilePhone iNetPhone;
	RMobilePhone::TMobilePhoneNetworkInfoV1 iNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg iNetworkInfoPckg;
	RMobilePhone::TMobilePhoneLocationAreaV1 iLocArea;
	
	TLbsLocMonitorNetworkInfoFinderState iState;
};

#endif //CLBSLOCMONITORNETWORKINFOFINDER_H
