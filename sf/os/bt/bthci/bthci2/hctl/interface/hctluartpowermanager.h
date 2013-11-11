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
 @publishedPartner
 @released
*/

#ifndef HCTLUARTPOWERMANAGER_H
#define HCTLUARTPOWERMANAGER_H

#include <e32base.h>
#include <bluetooth/hci/hcitypes.h>
#include <bluetooth/hci/hctluartpowerobserver.h>

class CDelay;
class RBusDevComm;

// The timeout to detect if a device has changed to power off state
static const TInt KUartSignalChangeTimeout = 10000000; // 10s

_LIT(KHctlUartPowerManagerPanic, "Uart Power Man");
enum THctlUartPowerManagerPanic
	{
	EHCTLUartPowerOnWithCTSLow = 0,
	EHCTLUartPowerInvalidState,
	};

/**
This class implements monitoring of a UART device to detect when
it changes power state. It is intended to be used by any UART based
HCTL implementation that wishes to detect power changing from on to
off and vice versa.
*/
NONSHARABLE_CLASS(CHCTLUartPowerManager) : public CActive
    {
public:
	IMPORT_C static CHCTLUartPowerManager *NewL(MHCTLUartPowerObserver& aObserver, RBusDevComm& aPort);
	~CHCTLUartPowerManager();

	IMPORT_C void Start();

	static TInt PowerOffTimeout(TAny* aThis);
	
private:
    CHCTLUartPowerManager(MHCTLUartPowerObserver& aObserver, RBusDevComm& aPort);
	void ConstructL();

	virtual void DoCancel();
	virtual void RunL();

	void PowerChange();

private:
	MHCTLUartPowerObserver& iObserver;

	TBTPowerState iState;
	RBusDevComm& iPort;
	TUint iSignals;

	CDelay* iPowerOffTimer;
    };
   
#endif // HCTLUARTPOWERMANAGER_H

