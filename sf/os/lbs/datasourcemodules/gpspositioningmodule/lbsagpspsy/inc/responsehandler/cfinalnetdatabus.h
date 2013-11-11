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
// netfinaldatabus.h
// An ative object wrapper to manage the network final data bus. This is the P&S bus
// that the location managers (e.g. the LocServer manager and network manager)
// publish onto. The location server and others subscribe to this bus.
// 
//

/**
 @file
 @InternalComponent
*/

#ifndef __NETFINALDATABUS_H__
#define __NETFINALDATABUS_H__

#include <e32base.h>
#include "lbsnetinternalapi.h"
#include "mdatabusobserver.h"
#include "lbsreffnpint.h"

class CFinalNetDataBus : public CActive
	{
public:
	static CFinalNetDataBus* NewL(MDataBusObserver& aObserver);
	void ConstructL();
	~CFinalNetDataBus();
	void Subscribe();
	void UnSubscribe();
	TInt GetLastPositionInfo(TPositionInfo& aFNPInfo, TTime& aActualTime);
	
private:
	CFinalNetDataBus(MDataBusObserver& aObserver);

	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
	MDataBusObserver& iObs;
	RLbsNetworkPositionUpdates iFinalNetDataBus; // RProperty wrapper	
	TPositionInfo	iPositionInfo; // position info data from the network
	};

#endif // __NETFINALDATABUS_H__
