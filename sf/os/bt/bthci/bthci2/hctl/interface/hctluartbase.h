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
 @publishedPartner
 @released
*/

#ifndef HCTLUARTBASE_H
#define HCTLUARTBASE_H

#include <bluetooth/hci/hctlbase.h>
#include <bluetooth/hci/hctlinterface.h>
#include <bluetooth/hardresetinitiator.h>
#include <d32comm.h>

class MHCTLChannelObserver;
class MHCTLDataObserver;
class MHCTLEventObserver;
class MControllerStateObserver;
class RBtBusDevComm;

/**
An implementation of a HCTL Plugin, for use as a base class
for UART based HCTL implementations.

This class directly implements the interfaces MHCTLInterface. It 
"exports" through the CHCTLBase::GetInterface function.


This class is intended to be derived from.
*/
class CHCTLUartBase : public CHCTLBase,
					  public MHCTLInterface,
					  public MHardResetInitiator
    {
public:
	enum TPowerControlDetectionMode
		{
		EPwrCtrlModeNone 	= 0x00,
		EPwrCtrlCTSTimedLow = 0x01,
		};

public:
	IMPORT_C ~CHCTLUartBase(); 

protected:
	IMPORT_C CHCTLUartBase();
	IMPORT_C virtual TBool CheckHardwareConnected();
	IMPORT_C void BaseConstructL(const TDesC& aIniFileName);
	
protected:
	IMPORT_C TAny* Interface(TUid aUid);
	IMPORT_C void SetPortBaudRateL(TUint32 aBaudRate);
	/**
	Accessor method for the port
	*/
	IMPORT_C RBusDevComm& Port();
	
	// Getter for iPowerCtrlMode
	IMPORT_C CHCTLUartBase::TPowerControlDetectionMode PowerCtrlMode() const;

private:
	void OpenPortL(TUint aPortNo);
	void ScanAndOpenPortL(TUint aStartingPortNo);

	virtual void PortOpenedL() = 0;

protected: 
	// unowned
	MHCTLChannelObserver* iChannelObserver;
	MHCTLDataObserver* iDataObserver;
	MHCTLEventObserver* iEventObserver;
	MControllerStateObserver* iControllerStateObserver;

private:
	// owned
	RBtBusDevComm* 				iPort;
	TPowerControlDetectionMode	iPowerCtrlMode;
	TUint32						iPadding;
    };

#endif // HCTLUARTBASE_H
