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
 @internalComponent
*/

#ifndef HCTLBCSPCONTROLLERMANAGER_H
#define HCTLBCSPCONTROLLERMANAGER_H

#include <e32base.h>
#include <bluetooth/hci/hctluartbase.h>
#include <bluetooth/hci/hctlpowerinterface.h>
#include <bluetooth/hci/hctluartpowerobserver.h>

class MControllerStateObserver;
class CHCTLBcspControllerReset;
class CHCTLBcspControllerPowerOn;
class RBusDevComm;
class CHCTLBcsp;
class CHCTLUartPowerManager;

/**
This is the class that implements a controller manager.
This is a class which provides a framework for managing the
physical aspects of the host controller. 
These include power control\observation and resetting.

There are a number of alternative ways to manage controller power 
and reset.

Reset.
1) Apply a controller specific synchronous command that uses to
a GPIO (General Purpose) line to tell the controller to reset.
2) Apply a controller specific synchronous command that uses to
a GPIO (General Purpose) line to tell the controller to power off.
Then wait a sufficient length of time to ensure the controller
has truly powered down. Then use another possibly asynchronous 
GPIO line command to power up.
3) Use the HCI Reset Command.
4) Use a Vendor (Manufacturer) specific command to perform a full
controller reset.
Note: 3) and 4) are unlikely to work if the controller has truly hung.
However they will be useful testing if 1) and 2) are not available.
THIS class provides an implementation for 4).

Power Control.
1) Apply a controller specific command that uses to
a GPIO (General Purpose) line to tell the controller to power down or up.
It is possible that the power up command will be asynchronous.
2) Use a Vendor (Manufacturer) specific command to perform the power
control functions.
3) Use an alternative to actually powering the controller up or down, 
For example merely switching page scan off on receipt of a power off
request, and switching page scan back on (if appropriate) on receipt of
power up request. (For the best way to do this - see the porting guide.)
Note: 2) is unlikely to work if the controller has truly hung.
However it will be useful testing if 1) is not available.
THIS class provides an implementation for 2). 

Power observation
1) Use a GPIO type power checking facility.
2) Use a vendor specific power checking command.
3) Cache last known power state, assuming the controller
starts powered up.
In this implementation we use 1),
*/
NONSHARABLE_CLASS(CHCTLBcspControllerManager) : public CBase, public MHCTLPowerInterface, public MHCTLUartPowerObserver
    {
public:
	typedef TBuf8<28> TBCCMDBuffer;
	enum TControllerManagerState 
		{
		EIdle,
		EWaiting,
		EResetBCSP,
		EResetHardware
		};

	enum TControllerManagerTask 
		{
		ENoTask,
		EPowerDown,
		EPowerUp,
		EControllerReset
		};

    static CHCTLBcspControllerManager* NewL(CHCTLBcsp& aHCTLBcsp, RBusDevComm& aPort,
    		CHCTLUartBase::TPowerControlDetectionMode aPwrCtrlMode);
    ~CHCTLBcspControllerManager();
	void SetObserver(MControllerStateObserver& aObserver);

	void Start();
	void HardReset();
	
	TBool BcspLinkEstablished();
	TBool ExpectedControllerReset();
	TBool PowerOffRequested();
	void ProcessBcCmdEvent(const TDesC8& aEvent);


private:
	void MhupoPowerChange(TInt aError);

	TInt MhpiGetPower(TBTPowerState& aState);
	TInt MhpiSetPower(const TBTPowerState aState);

    CHCTLBcspControllerManager(CHCTLBcsp& aHCTLBcsp, RBusDevComm& aPort,
    		CHCTLUartBase::TPowerControlDetectionMode aPwrCtrlMode);
	void ConstructL();

	TInt DoSetPower(TBTPowerState aState);
	static TInt HandlePowerDown(TAny* aThis);
	void DoHandlePowerDown();
	static TInt HandleReadyToResetController(TAny* aThis);
	void DoHandleReadyToResetController();
	void McroControllerResetComplete();
	void McpooPowerOnComplete();
	TBool DoBcspLinkEstablishedForPowerUp();
	TBool DoBcspLinkEstablishedForControllerReset();


private: 
	// Unowned
    MControllerStateObserver* iObserver;
    RBusDevComm& iPort;

	// Owned
	TBCCMDBuffer iColdResetCommand;
	TBCCMDBuffer iColdHaltCommand;
	CHCTLBcsp& iHCTLBcsp;
	TControllerManagerState iControllerManagerState;
	TBTPowerState iLastPowerRequest;
	TBTPowerState iCurrentPowerState;
	TBool iIsMissedPowerRequest;
	TControllerManagerTask iCurrentTask;
	CHCTLBcspControllerPowerOn* iControllerPowerOn;
	CHCTLBcspControllerReset* iControllerReset;
	CAsyncCallBack* iPowerDownCallback;
	CAsyncCallBack* iReadyToResetControllerCallback;
	
	CHCTLUartPowerManager* iUartPowerManager;
	CHCTLUartBase::TPowerControlDetectionMode iPwrCtrlMode;
    };

#endif // HCTLBCSPCONTROLLERMANAGER_H
