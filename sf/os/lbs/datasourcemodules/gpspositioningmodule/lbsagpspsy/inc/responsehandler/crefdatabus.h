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
// An ative object wrapper to manage the reference data bus. This is the P&S bus
// that the network gateway publishes to.
// 
//

/**
 @file
 @InternalComponent
*/

#ifndef __LBSREFDATABUS_H__
#define __LBSREFDATABUS_H__

#include <e32base.h>
#include <lbspositioninfo.h>
#include "lbsnetinternalapi.h"
#include "lbsreffnpint.h"

class MDataBusObserver;



/**
Assumes something else has already called RLbsPositionUpdates::InitializeL()
*/
class CRefDataBus : public CActive
	{
public:
	static CRefDataBus* NewL(MDataBusObserver& aObserver);
	~CRefDataBus();
	void Subscribe();
	void UnSubscribe();
	TInt GetLastPositionInfo(TPositionInfo& aRefInfo, TTime& aActualTime);
	
	/**
	The panic codes for the reference data bus object.
	*/
	enum TRefDataBusPanic
		{
		/** 
		An instance of the MRefDataBusObserver has not been passed to the object.
		The panic category will be "RefDataBus".
		*/
		ERefDataBusObserverNotSet = 0
		};

private:
	CRefDataBus(MDataBusObserver& aObserver);
	void ConstructL();
	
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	MDataBusObserver& 		iObs;
	RLbsNetworkPositionUpdates	iRefDataBus;
	TPositionInfo iPositionInfo;
	};

#endif // __LBSREFDATABUS_H__
