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
//

/**
 @file
 @internalComponent
*/

#include "hctluartbase.h"
#include "btbusdevcomm.h"

#include <bluetooth/logger.h>
#include <bluetooth/hci/hciutil.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCTL_BASE);
#endif

EXPORT_C CHCTLUartBase::CHCTLUartBase()
	{
	LOG_FUNC
	}

EXPORT_C CHCTLUartBase::~CHCTLUartBase()
	{
	LOG_FUNC
	if (iPort)
		{
		iPort->Close();
		delete iPort;
		}
	}

/**
	Common method used by UART derivative HCTLs to check hardware is connected
*/
EXPORT_C TBool CHCTLUartBase::CheckHardwareConnected()
	{
	LOG_FUNC
	TBool hwPresent = ETrue;

	LOG1(_L8("\tsignals = 0x%02x"), iPort->Signals());

	// If there are currently no signals assume the HW is not connected.
	if(!iPort->Signals())
		{
		hwPresent = EFalse;

		TRequestStatus lineStatus;
		TRequestStatus timeStatus;

		TUint testSignals;
		RTimer timer;

		TInt err = timer.CreateLocal();
		if(err == KErrNone)
			{
			iPort->NotifySignalChange(lineStatus, testSignals);
			timer.After(timeStatus, 5000000); // 5s
			
			User::WaitForRequest(lineStatus, timeStatus); // so we don't wait forever
			
			if (timeStatus.Int() == KErrNone)
				{
				// The guard timer has expired.
				iPort->NotifySignalChangeCancel();
				User::WaitForRequest(lineStatus);	// consume the request
				}
			else if ( lineStatus.Int() == KErrNone)
				{
				// The hardware is there.  Cancel the timer.
				timer.Cancel();
				User::WaitForRequest(timeStatus);	// consume the request
				hwPresent = ETrue;
				}
			}
		timer.Close();
		}

	LOG1(_L8("\thwPresent = %d"), hwPresent);
	return hwPresent;
	}

/**
Common method used by UART derivative HCTLs to open the available physical 
UART connections for bluetooth hardware
*/
void CHCTLUartBase::OpenPortL(TUint aPortNo)
	{
	LOG_FUNC
	LOG1(_L8("\taPortNo = %d"), aPortNo);
	
	TInt ret = iPort->Open(aPortNo);
	
	if(ret != KErrNone && ret != KErrAlreadyExists)
		{
		LEAVEIFERRORL(ret);
		}
	}

// For debug emulator environments, we provide a mechanism to cycle through possible
// port numbers until we find a port to use (it maybe be connected to something other
// than Bluetooth hardware, but that has to be identified at a higher level).  This meant
// users don't have to fiddle with esk or ini files, especially for the case where they're
// running back to back on one PC. 
// However, this slows down emulator start-up, so it is configured by ini file which does
// mean potentially fiddling with ini files anyway ... c'est la vie.
#if(defined(__WINS__) && defined(_DEBUG))
#define SYMBIAN_INTERNAL_RETRY_BLUETOOTH_PORT_OPEN
#pragma message("If configured, Bluetooth HCTL will scan UARTs to find a connected port")
#endif

#ifndef SYMBIAN_INTERNAL_RETRY_BLUETOOTH_PORT_OPEN

void CHCTLUartBase::ScanAndOpenPortL(TUint /*aStartingPortNo*/)
	{
	LOG_FUNC
	LEAVEL(KErrNotSupported);
	}

#else

/**
	Common method used by UART derivative HCTLs to scan the available physical UART connections
	for bluetooth hardware
*/
void CHCTLUartBase::ScanAndOpenPortL(TUint aStartingPortNo)
	{
	LOG_FUNC
	LOG1(_L8("\aStartingPortNo = %d"), aStartingPortNo);
	TUint portNo = aStartingPortNo;
	
	TInt ret = KErrNone;
	
	// This is a limit set by windows.  RBusDevComm will panic if aPortNo of 32 or higher
	// is used.  
	const TInt KMaxPortNumber = 32;
	
	while(portNo < KMaxPortNumber)
		{
		LOG2(_L8("\tTRYING: port %d (PC COM Port %d)"), portNo, portNo + 1);
		ret = iPort->Open(portNo);
		
		if(ret == KErrNone || ret == KErrAlreadyExists)
			{
			if(CheckHardwareConnected())
				{
				LOG2(_L8("\tUSING: port %d (PC COM Port %d)"), portNo, portNo + 1);
				// Exit the loop, a port has been opened.
				break;
				}
			else 
				{
				iPort->Close();	// close it - there was no HW there...
				}
			}
		portNo++;  // try the next port
		}
	
	if(ret != KErrNone && ret != KErrAlreadyExists)
		{
		// A port could not be found.
		LEAVEIFERRORL(KErrHardwareNotAvailable);
		}
	}
	
#endif // SYMBIAN_INTERNAL_RETRY_BLUETOOTH_PORT_OPEN

//Implementation of pure virtuals from CHCTLBase
EXPORT_C TAny* CHCTLUartBase::Interface(TUid aUid)
	{
	TAny* ret = NULL;
	switch(aUid.iUid)
		{
		case KHCTLInterfaceUid:
			ret = reinterpret_cast<TAny*>(static_cast<MHCTLInterface*>(this));
			break;

		default:
			break;
		};

	return ret;
	}

EXPORT_C void CHCTLUartBase::BaseConstructL(const TDesC& aIniFileName)
	{
	LOG_FUNC
	
	_LIT(KSection, "hctl");
	_LIT(KVarPort, "port");
	_LIT(KVarBaudRate, "baud_rate");
	_LIT(KVarPwrCtrlMode, "pwr_ctrl_mode");
	#ifdef SYMBIAN_INTERNAL_RETRY_BLUETOOTH_PORT_OPEN
	_LIT(KVarScanPorts, "scan_ports");
	#endif // SYMBIAN_INTERNAL_RETRY_BLUETOOTH_PORT_OPEN

	// Create HCI Utility library
	CHciUtil* hciUtil = CHciUtil::NewL(aIniFileName);
	CleanupStack::PushL(hciUtil);
	
	hciUtil->OpenIniFileL();

	TUint portNumber = hciUtil->GetValueFromFileL(KSection, KVarPort);
	TUint baudRate = hciUtil->GetValueFromFileL(KSection, KVarBaudRate);
	
	// we don't want to leave if the param is not present. just consider
	// the default value as true 
	TRAPD(err, iPowerCtrlMode = static_cast<TPowerControlDetectionMode>(hciUtil->GetValueFromFileL(KSection, KVarPwrCtrlMode)));
	if (err != KErrNone)
		{
		iPowerCtrlMode = EPwrCtrlCTSTimedLow;	// keeps back compatibility
		}
	
#ifdef SYMBIAN_INTERNAL_RETRY_BLUETOOTH_PORT_OPEN
	TBool scanPorts = EFalse;
	TRAP(err, scanPorts = static_cast<TBool>(hciUtil->GetValueFromFileL(KSection, KVarScanPorts)));
	if(err != KErrNone)
		{
		scanPorts = EFalse; // default to off because of stack boot time impact
		}
#endif // SYMBIAN_INTERNAL_RETRY_BLUETOOTH_PORT_OPEN
	
	// Remove the HCI UTIL instance. 
	CleanupStack::PopAndDestroy();
	
	// Allocate the RBtBusDevComm port object
	iPort = new (ELeave) RBtBusDevComm;
	
#ifdef SYMBIAN_INTERNAL_RETRY_BLUETOOTH_PORT_OPEN
	if(scanPorts)
		{
		ScanAndOpenPortL(portNumber);
		}
	else
#endif // SYMBIAN_INTERNAL_RETRY_BLUETOOTH_PORT_OPEN
		{
		OpenPortL(portNumber);
		}
	
	SetPortBaudRateL(baudRate);

	// Perform any HCTL specific initialisation. 
	PortOpenedL();
	}

EXPORT_C void CHCTLUartBase::SetPortBaudRateL(TUint32 aBaudRate)
	{	
	LOG_FUNC

	TCommConfig configPkg;
	iPort->Config(configPkg);
	TCommConfigV01& config = configPkg(); //Get reference to TCommConfig iConfig

	switch(aBaudRate)
		{
		case 57600:
			config.iRate = EBps57600;
			break;

		case 115200:
			config.iRate = EBps115200;
			break;

		case 230400:
			config.iRate = EBps230400;
			break;

		case 460800:
			config.iRate = EBps460800;
			break;

		case 576000:
			config.iRate = EBps576000;
			break;

		case 750000:
			config.iRate = EBpsSpecial;
			config.iSpecialRate=0; // 750000;
			break;

		case 1152000:
			config.iRate = EBps1152000;
			break;

		case 921600:
			config.iRate = EBps921600;
			break;

		case 1843200:
			config.iRate = EBpsSpecial;
			config.iSpecialRate=1; // 1843200;
			break;

		case 3686400:
		case 3692300:
		case 3600000:
			config.iRate = EBpsSpecial;
			config.iSpecialRate=2; // 3686400;
			break;

		case 3000000:
			config.iRate = EBpsSpecial;
			config.iSpecialRate = 3;
			break;

		default:
			// Use default rate in case of error
			config.iRate = EBps115200;
			break;
		};
	
	// Update the port configuration.
	LEAVEIFERRORL(iPort->SetConfig(configPkg));
	}

// Accessor method for the port
EXPORT_C RBusDevComm& CHCTLUartBase::Port()
	{
	// No check for a NULL iPort is needed because BaseConstructL will Leave if we
	// fail to allocate the port object
	return *iPort;
	}

EXPORT_C CHCTLUartBase::TPowerControlDetectionMode CHCTLUartBase::PowerCtrlMode() const
	{
	return iPowerCtrlMode;
	}
