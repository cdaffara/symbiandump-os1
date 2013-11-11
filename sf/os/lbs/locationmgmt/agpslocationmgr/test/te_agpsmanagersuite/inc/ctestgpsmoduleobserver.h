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
// GPS Integration Module Observer for AGPS Manager UNIT testing
// 
//

#ifndef __CTESTGPSMODULEOBSERVER_H__
#define __CTESTGPSMODULEOBSERVER_H__

#include <e32cmn.h>
#include <ecom/ecom.h>
#include <lbs/lbslocdatasourcegpsbase.h>

#include "ctestobserverbase.h"
#include "lbscommoninternaldatatypes.h"

class MTestGpsModuleObserver
{
public:
	virtual void RequestLocationUpdate(const TTime& aTargetTime, const TLbsLocRequestQuality& aQuality) = 0;
  	virtual void CancelLocationRequest() = 0;
	virtual void AdvisePowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode) = 0;
	virtual void SetGpsOptions(const TLbsGpsOptions& aGpsOptions) = 0;
	virtual void AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aDataMask) = 0;
};

/** GPS Module observer class definition.

This class is used to capture and store calls made from the
AGPS Manager to the GPS Module. The callback methods are all
defined by the MTestGpsModuleObserver interface.
*/
class CTestGpsModuleObserver : public CTestObserverBase , public MTestGpsModuleObserver
	{

public:

	/*
	One flag for each method called by the AGPS Manager
	on the interface with the GPS Module 
	*/
	enum TObservedGpsModuleCalls
	{
		ELocationUpdateReqCall 	= 0x0001,
		ECancelLocationCall		= 0x0002,
		EAdvisePowerModeCall	= 0x0004,
		ESetOptionsCall			= 0x0008,
		EAssistanceDataCall		= 0x0010
	};

public:

	static CTestGpsModuleObserver* NewL();

	~CTestGpsModuleObserver();


	// Derived from MTestGpsModuleObserver. These are the callbacks that are called by the 
	// test GPS module object when it receives AGPS Manager requests
	//
  	void RequestLocationUpdate(const TTime& aTargetTime, const TLbsLocRequestQuality& aQuality);
  	void CancelLocationRequest();
	void AdvisePowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode);
	void SetGpsOptions(const TLbsGpsOptions& aGpsOptions);
	void AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aDataMask);
	
	
	// Methods called by the test to find out what callbacks have occurred
	// (calling this methods decreases the internal count of callbacks received
	// and clears the flag corresponding to this callback)
	TBool WasLocationUpdateRequested(TTime& aTargetTime, TLbsLocRequestQualityInt& aQuality);
	TBool WasLocationUpdateCancelled();
	TBool WasPowerModeAdvised(CLbsLocationSourceGpsBase::TPowerMode& aPowerMode);
	TBool WereGpsOptionsSet(TLbsGpsOptions& aGpsOptions);
	TBool WereAssistanceDataReported(TInt& aError, TLbsAsistanceDataGroup& aDataMask);
	
	// Getters for the values observed in calls from AGPS Manager to GPS module
	// ========== MAY NOT BE NEEDED, COMMENT OUT FOR THE TIME BEING ============
	/*
	const TTime& TargetTime();
	const TLbsLocRequestQuality& RequestQuality();
	
	const TPowerMode& PowerMode();
	const TLbsGpsOptions& GpsOptions();
	
	TInt AssistanceDataEventError();
	TLbsAsistanceDataGroup AssistanceDataMask();	
	*/

private:

	CTestGpsModuleObserver();
	
	void ConstructL();
	
	// Check that a callback has happened and decrease callback count
	TBool CheckCallbackObserved(const TObservedGpsModuleCalls aCall);

private:

	// Each bit in iObservedGpsEvents represents a call made by the AGPS Manager
	// to the GPS Module. Calls are represented by TObservedGpsModuleCalls. 
	TUint iObservedGpsEvents;

	// Values passed by the Manager to the GPS module when
	// it calls RequestLocationUpdate
	TTime iRequestedTargetTime;
	TLbsLocRequestQualityInt iRequestedQuality;

	// Value passed by the Manager to the GPS module when
	// it calls AdvisePowerMode
	CLbsLocationSourceGpsBase::TPowerMode iAdvisedPowerMode;

	// Value passed by the Manager to the GPS module when
	// it calls SetGpsOptions
	TLbsGpsOptions* iRequestedGpsOptions;

	// Value passed by the Manager to the GPS module when
	// it calls AssistanceDataEvent
	TInt iAssistanceDataEventError;
	TLbsAsistanceDataGroup iAssistanceDataMask;		
	};


#endif // __CTESTGPSMODULEOBSERVER_H__

