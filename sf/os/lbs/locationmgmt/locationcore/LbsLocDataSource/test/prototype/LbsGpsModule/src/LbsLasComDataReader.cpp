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

#include "LbsLasDataSource.h"
#include "LbsLasComDataReader.h"
#include "LbsLasCommon.h"

// KCommPortName should be the serial port receiving the NMEA data. This can be a bluetooth port.
// On the windows emulator, Symbian OS port numbers are one less than their windows counterpart.
// For example Symbian OS port "COMM::5" connects through to Windows port "COM6"
//
_LIT(KCommPortName,   "COMM::5");
_LIT(KCommModuleName, "ECUART");



//************************************************************************************************************
//   CLasComDataReader
//************************************************************************************************************
CLasComDataReader::CLasComDataReader(MLasDataSourceObserver* aObserver)
	:
	CActive(CActive::EPriorityStandard),
	iConnectState(EStateUnknown),
	iObserver(aObserver),
	iMonitoring(EFalse)
	{
	CActiveScheduler::Add(this);
	}
	

//------------------------------------------------------------------------------------------------------------
CLasComDataReader* CLasComDataReader::NewL(MLasDataSourceObserver* aObserver)
	{
	CLasComDataReader* self = new(ELeave) CLasComDataReader(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


//------------------------------------------------------------------------------------------------------------
void CLasComDataReader::ConstructL()
	{
	User::LoadPhysicalDevice(_L("ECDRV"));
	User::LoadPhysicalDevice(_L("EUART1"));
	User::LoadLogicalDevice(_L("ECOMM"));
	}

//------------------------------------------------------------------------------------------------------------
CLasComDataReader::~CLasComDataReader()
   {
	CloseDataSource();

	User::FreePhysicalDevice(_L("ECDRV"));
	User::FreePhysicalDevice(_L("EUART1"));
	User::FreeLogicalDevice(_L("ECOMM"));
   }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLasComDataReader::DoCancel()
	{
	iCommPort.Cancel();
	}

void CLasComDataReader::RunL()
{
	LASLOG("CLasComDataReader::RunL - Enter");
	
   
	LASLOGDES(iData);
	if (iMonitoring)
		{
		iObserver->HandleData(iData, iStatus.Int());
		}

	if (iStatus == KErrNone)
		{
		LASLOG("CLasComDataReader::RunL - Got Data Ok");
		iCommPort.ReadOneOrMore(iStatus, iData);
		SetActive();
		}
	else
		{
		LASLOG("CLasComDataReader::RunL - Error reading data");	
		iConnectState = EStateError;
		}
   }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TInt CLasComDataReader::ConfigPort(RComm& aCommPort)
	{
	// Set port config to 4800,7,N,1

	TCommConfig configBuf;
	TCommConfigV01& config = configBuf();

	config.iRate=EBps4800;
	config.iDataBits=EData7;
	config.iStopBits=EStop1;
	config.iParity=EParityNone;
	config.iHandshake=KConfigObeyXoff|KConfigSendXoff;
	config.iParityError=KConfigParityErrorFail;
	config.iTerminatorCount=0;
	config.iXonChar=0x11; // XON
	config.iXonChar=0x13; // XOFF
	config.iSpecialRate=0;
	config.iParityErrorChar=0;

	TInt err = aCommPort.SetConfig(configBuf);
	
	return err;
	}


TInt CLasComDataReader::OpenDataSource()
	{
 	LASLOG("OpenDataSource - Enter");

	switch (iConnectState)
		{
		case EStateUnknown:
			{
			break;
			}
		case EStateOnLine:
		case EStateError:
			{
			CloseDataSource();
			}
		}

	TInt err = iCommServ.Connect();
	if (err)
		{
		LASLOG("OpenDataSourceL - Unable to connect to serial server");
		iConnectState = EStateError;
		return err;
		}

	err = iCommServ.LoadCommModule(KCommModuleName);
	if (err)
		{
		LASLOG2("OpenDataSourceL - Unable to load comm module %S", &KCommModuleName);
		iConnectState = EStateError;
		return err;
		}

	TInt numPorts;
	
	err = iCommServ.NumPorts(numPorts);
	if (err)
		{
		LASLOG("OpenDataSourceL - Unable to retrieve serial port count");
		iConnectState = EStateError;
		return err;
		}
		
	LASLOG2("OpenDataSourceL - Connected to serial server (NumPorts: %d)", numPorts);

	err = iCommPort.Open(iCommServ, KCommPortName, ECommShared);
	if (err)
		{
		LASLOG2("OpenDataSourceL - Unable to open comm port %S", &KCommPortName);
		iConnectState = EStateError;
		return err;
		}

	err = ConfigPort(iCommPort);
	if (err)
		{
		LASLOG2("OpenDataSourceL - Unable to configure comm port %S", &KCommPortName);
		iConnectState = EStateError;
		return err;
		}

	iConnectState = EStateOnLine;
	
	LASLOG("OpenDataSourceL - Openned OK");

	iCommPort.ReadOneOrMore(iStatus, iData);
	SetActive();
	
	return KErrNone;
	}

void CLasComDataReader::CloseDataSource()
	{
	Cancel();

	iCommPort.Close();
	iCommServ.Close();

	iConnectState = EStateUnknown;
	}


void CLasComDataReader::MonitorDataSource()
	{
	iMonitoring = ETrue;
	}
	
	
// Allows the caller to signal they are no longer interested in receiving data.
// Call does not actual stop the reading from device - data will still be
// consumed from the bluetooth device. However, client will no longer receive
// a callback.
//
void CLasComDataReader::CancelMonitor()
	{
	iMonitoring = EFalse;
	}

