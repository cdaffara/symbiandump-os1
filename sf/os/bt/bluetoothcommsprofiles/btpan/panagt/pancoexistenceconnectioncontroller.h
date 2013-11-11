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
//

#ifndef PANCOEXISTENCECONNECTIONCONTROLLER_H
#define PANCOEXISTENCECONNECTIONCONTROLLER_H

/**
@file
@note Due to the limitation of DHCP server, BT PAN should not work concurrently with RNDIS. 
This class is used for mutual exclusion between PAN and RNDIS.
@internalComponent
*/

#include <e32std.h>
#include <e32base.h> 
#include <e32property.h>

class MPanCoexistenceConnectionObserver;

// This is a local definition of this shared enum of P&S key values.  These values should not be changed without changing
// the equivalent definition in the other IP bearer code.
enum TIPBearerCoexistenceStatus
	{
	ENoneIsActive, 
	ERndisIsActive,
	EBTPanIsActive
	};

// This is a local definition of this shared P&S id.  This value should not be changed without changing
// the equivalent definition in the other IP bearer code.
const TInt KIPBearerCoexistenceProperty = 0x10286a95;

class CPanCoexistenceConnectionController : public CActive
	{
public:
	static CPanCoexistenceConnectionController* NewL(MPanCoexistenceConnectionObserver& aObserver);
	~CPanCoexistenceConnectionController();

public:
	TInt TryToCreateNewPanConnection();
	void HandleAllPanConnectionsClosed();
	TIPBearerCoexistenceStatus ReadStatus();
	
private:
	CPanCoexistenceConnectionController(MPanCoexistenceConnectionObserver& aObserver);
	void ConstructL();

	TInt DefineProperty();
	void WriteStatus(TIPBearerCoexistenceStatus aStatus);
	void SubscribeToProperty();

private:
	void RunL();
	void DoCancel();
	
private:
	RProperty iProperty;
	RMutex iMutex;
	
	// These are the flags to set whether BT PAN connection is active and the status transition
	// is initiated by BT PAN
	TBool iActivePanConnectionExists;
	TBool iLocallyInitiatedTransition;
	
	MPanCoexistenceConnectionObserver& iPanCoexistenceConnectionObserver;
	};

class MPanCoexistenceConnectionObserver
	{
public:
	// This method is called ONLY when the value of the Co-existence status is changed due
	// to a different IP bearer (e.g., RNDIS)
	virtual void MpccoPanCoexistenceConnectionUpdated(TIPBearerCoexistenceStatus aStatus) = 0; 
	};

#endif /*PANCOEXISTENCECONNECTIONCONTROLLER_H*/


