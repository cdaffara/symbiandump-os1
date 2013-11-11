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
// An active object wrapper to manage the network location data bus. This is the P&S bus
// that the network location manager publishes onto. 
// The location server and others subscribe to this bus.
// 
//

/**
 @file
 @InternalComponent
*/

#ifndef __LBSNETDATABUS_H__
#define __LBSNETDATABUS_H__

#include <e32base.h>
#include "LbsInternalInterface.h"
#include "mdatabusobserver.h"

/**
The panic codes for the location data bus object.
*/
enum TNetDataBusPanic
	{
	/** 
	An instance of the MLocDataBusObserver has not been passed to the object.
	The panic category will be "NetDataBus". The panic value is zero.
	*/
	ENetDataBusObserverNotSet = 0
	};


/**
Sanity wrapper around the RLbsPositionUpdate
Assume something else (root) has already called RLbsPositionUpdates::InitializeL()!
Don't do this in the ConstructL, just call RLbsPositionUpdates::OpenL
*/
class CNETDataBus : public CActive
	{
public:
	static CNETDataBus* NewL(MDataBusObserver* aObserver, const TUid& aModuleId);
	void ConstructL();
	~CNETDataBus();
	void Subscribe();
	void UnSubscribe();
	TInt GetLastPositionInfo(TPositionInfo& aPosInfo, TTime& aActualTime);
	
private:
	CNETDataBus(MDataBusObserver* aObserver, const TUid& aModuleId);
	
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	MDataBusObserver*		iObs;
	RLbsPositionUpdates		iNETDataBus; 	// RProperty wrapper
	TUid					iModuleId;		// module id (of the netloc manager) which identifies the bus 
	TPositionInfo			iPositionInfo;  // position info data from Network
	};

#endif // __LBSNETDATABUS_H__
