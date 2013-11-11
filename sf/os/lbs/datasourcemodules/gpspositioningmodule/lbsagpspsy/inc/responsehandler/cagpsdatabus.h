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
// locationdatabus.h
// An ative object wrapper to managed the location data bus. This is the P&S bus
// that the location managers (e.g. the AGPS manager and network manager)
// publish onto. The location server and others subscribe to this bus.
// 
//

/**
 @file
 @InternalComponent
*/

#ifndef __LBSLOCATIONDATABUS_H__
#define __LBSLOCATIONDATABUS_H__

#include <e32base.h>
#include "LbsInternalInterface.h"
#include <lbssatellite.h>

class MDataBusObserver;

/**
The panic codes for the location data bus object.
*/
enum TLocDataBusPanic
	{
	/** 
	An instance of the MLocDataBusObserver has not been passed to the object.
	The panic category will be "LocDataBus". The panic value is zero.
	*/
	ELocDataBusObserverNotSet = 0
	};


/**
Sanity wrapper around the RLbsPositionUpdate
Assume something else has already called RLbsPositionUpdates::InitializeL()!
Don't do this in the ConstructL, just call RLbsPositionUpdates::OpenL
*/
class CAgpsDataBus : public CActive
	{
public:
	static CAgpsDataBus* NewL(MDataBusObserver& aObserver, const TUid& aModuleId);
	~CAgpsDataBus();
	void Subscribe();
	void UnSubscribe();
	TInt GetLastPositionInfo(TPositionSatelliteInfo& satelliteInfo, TTime& actualTime, TUint& aAttributes, TUint& aMode);	

private:
	CAgpsDataBus(MDataBusObserver& aObserver, const TUid& aModuleId);
	void ConstructL();
	
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	TUid					iModuleId;
	MDataBusObserver&		iObs;
	RLbsPositionUpdates		iAGPSDataBus; // RProperty wrapper	
	};

#endif // __LBSLOCATIONDATABUS__
