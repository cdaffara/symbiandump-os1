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
// ctestagpsupdatesobserver.h
// GPS Integration Module Observer for AGPS Manager UNIT testing
// 
//

#ifndef __CTESTAGPSUPDATESOBSERVER_H__
#define __CTESTAGPSUPDATESOBSERVER_H__

#include <e32cmn.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include "lbsinternalinterface.h"

#include "ctestobserverbase.h"

class CTestUpdatesListener;

// Class used to let CTestAgpsUpdatesObserver know of the 
// location updates
class MTestUpdateListenerObserver
	{
public:	
	// This method can be mofied to pass back to the observer
	// the actual location update and the other bits of info
	// that come with it. No current UNIT test using this framework
	// looks into the values of the update.
	virtual void UpdateAvailable() = 0;
	};

/** AGPS Updates Observer class definition.

This class is used by UNIT-test code to capture position
updates published by the AGPS Manager on the Location Updates Bus after
the test has sent location update requests to the AGPS Manager.
*/
class CTestAgpsUpdatesObserver : public CTestObserverBase, public MTestUpdateListenerObserver
	{

public:

	static CTestAgpsUpdatesObserver* NewL();

	~CTestAgpsUpdatesObserver();


	// Methods called by the test to find out 
	// if a LocationUpdate has been published
	// (the test MUST call it to reduce the number of
	// events stored in the observer)
	TBool IsLocationUpdateAvailable();

	// From MTestUpdateListenerObserver
	// (this method could be expanded to receive
	// and store the actual location update so that
	// its details are available to the test. No test
	// currently needs this functionality)
	void UpdateAvailable();
	
	
	// Getters for the location update previously captured and stored
	// ========== MAY NOT BE NEEDED, COMMENT OUT FOR THE TIME BEING ============
	/*
	const TLbsLocUpdate& Update();	
	*/

private:

	TBool CheckCallbackObserved();

	CTestAgpsUpdatesObserver();
	
	void ConstructL();
	
private:

	// Storage for the Location Update captured from the Location
	// Updates bus and the rest of bits of info that come with it...
	// Not currently needed by any test.
	
	// TPositionSatelliteInfo iPosInfoUpdate;
	// TBool iConflictControlFlag;
	// TBool iTargetTime;
	// TBool iActualTime;
	// TUint aAttributes;
	// TUint aGpsMode;
	
	CTestUpdatesListener* iUpdatesListener;
	};


// This active object is only required because the "send"
// operation in class CTestAgpsToNgObserver
// requires a TRequestStatus.
class CTestUpdatesListener : public CActive
{
public:

	static CTestUpdatesListener* NewL(MTestUpdateListenerObserver& aUpdateObserver);
	~CTestUpdatesListener();
	
private:
	CTestUpdatesListener(MTestUpdateListenerObserver& aUpdateObserver);
	void ConstructL();
	// From CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	
private:

	MTestUpdateListenerObserver& iUpdateObserver;

	// Channel to listen for AGPS Manager updates
	RLbsPositionUpdates iPosUpdates;
};
#endif // __CTESTAGPSUPDATESOBSERVER_H__

