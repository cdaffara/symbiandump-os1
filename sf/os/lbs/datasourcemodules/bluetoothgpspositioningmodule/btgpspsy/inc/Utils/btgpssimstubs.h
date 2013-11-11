// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// LBS Bluetooth GPS PSY: Simulation stubs
//

#ifndef BTGPSSIMSTUBS_H
#define BTGPSSIMSTUBS_H

#ifdef	LBS_BTGPSPSY_SIM

#include <e32base.h>
#include "btgpsdiscovery.h"
#include "btgpssim.h"


void AddSimulatedPairedDevicesL(RBTDeviceArray* aDeviceArray);
TInt GetSimSockAddr(TSockAddr& aSockAddr, TUint& aProtocol);


/* SDP Agent simulator 

@see CBTGPSDiscovery

@internalComponent
*/
class CSdpAgentSim : public CActive
	{
public:
	static CSdpAgentSim* NewL(MSdpAgentNotifier& aObserver);
	~CSdpAgentSim();

	TInt GetChannelStart(const TBTDevAddr& aAddr, TInt& aChannel);
	
private:
	CSdpAgentSim(MSdpAgentNotifier& aObserver);
	void ConstructL();

	void RunL();
	void DoCancel();
	
private: 
	MSdpAgentNotifier& iObserver;
	RTimer iTimer;
	};


#endif // LBS_BTGPSPSY_SIM
#endif // BTGPSSIMSTUBS_H
