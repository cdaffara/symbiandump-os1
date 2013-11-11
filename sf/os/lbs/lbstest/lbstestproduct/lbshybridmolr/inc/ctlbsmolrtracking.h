// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains the header file for Location Data Source Tests
// 
//

#ifndef __CT_LBS_MOLR_TRACKING_H__
#define __CT_LBS_MOLR_TRACKING_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/lbsadmin.h>

// LBS test includes.
#include "ctlbshybridmolrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/test/posserverwatch.h>
#include "ctlbstestactivemanager.h"
#include "ctlbsnetprotocol.h"
#include "lbs/lbspositioningstatus.h"

// Literals used
_LIT(KLbsMolrTracking, "LbsMolrTracking");

class CT_LbsMolrTracking :	public	CT_LbsHybridMOLRStep, public MTestFlowObserver
	{
private:
	class CT_ClientData : public CBase
		{
	public:
		CT_ClientData(TInt aSessionIdDelay, TInt aSessionStepDelay, TBool aCancelRequest,TInt aSessionIdCancel, TInt aSessionStepCancel, TInt aTrackingLoops, TPositionUpdateOptions aUpdateOptions, TInt aPsyUid, TInt aExpectedError)
			: iSessionIdDelay(aSessionIdDelay), iSessionStepDelay(aSessionStepDelay), 
			 	iCancelRequest(aCancelRequest), iSessionIdCancel(aSessionIdCancel), iSessionStepCancel(aSessionStepCancel), iPsyUid(aPsyUid), iExpectedError(aExpectedError),
				iInitialStep(ETrue), iNumberOfNPUDs(aTrackingLoops), iPositionsReceived(0), 
				iUpdateOptions(aUpdateOptions),iRequestCancelled(EFalse)
			{}
		~CT_ClientData()
			{
			delete iWatcher;
			iWatcher = NULL;
			iPositioner.Close();
			iServer.Close();
			}
	public:
		// After what session should this client start
		TInt iSessionIdDelay;
		// After which step in the session should this client start
		TInt iSessionStepDelay;
		// whether an NPUD should be cancelled
		TBool iCancelRequest;
		// After what session should this client's last NPUD be cancelled
		TInt iSessionIdCancel;
		// After which step in the session should this client's last NPUD be cancelled
		TInt iSessionStepCancel;		
		// Specifies if the initial NPUD has been sent out or not
		TBool iInitialStep;		
		RPositionServer iServer;
		RPositioner iPositioner;
		// The number of NPUDs it needs to send out
		TInt iNumberOfNPUDs;
		// The number of positions received so far
		TInt iPositionsReceived;
		CPosServerWatcher* iWatcher;
		
		TTime iTimeOfInitialPosition;
		TPositionUpdateOptions iUpdateOptions;
		// whether the clients last NPUD was cancelled 
		TBool iRequestCancelled;
		// The time when the last calculated position was received (for MaxAge checking)
		TTime iTimeOfLastPosition;
		// The time when the previous position was received (for calculating the right window for variant2 behaviour)
		TTime iTimeOfPreviousPosition;
		// Uid of the positioner to be used
		TInt iPsyUid;
		TBool iAutonomousPsy;	// whether the client has opened a session to the (autonomous) gps psy
		
		TInt  iExpectedError;	// error code that the client expects when it issues an NPUD
		};
public:
	~CT_LbsMolrTracking();

	static CT_LbsMolrTracking* New(CT_LbsHybridMOLRServer& aParent);
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

	// MTestFlowObserver
	void OnReadyL(TInt aObjectId);
	void OnTimedOutL(TInt aObjectId);
	void OnGetLastKnownPositionL(TInt aObjectId, TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdateL(TInt aObjectId, TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void StopTest();
	void OnSignalNetworkStep(TInt aSessionId, TInt aSessionStep);
	
protected:
	CT_LbsMolrTracking(CT_LbsHybridMOLRServer& aParent);
	void ConstructL();

private:
	void DoClientStepL(TInt aIndex);
	void EnablePsy(TInt aPsyUid);
private:
	
	/** Used so the test checks the right request is sent to the network */
	CLbsAdmin::TGpsMode iAdminGpsMode;

	/** Used so the network proxy knows what to expect*/
	TBool iGPSModeNotSupported;
	
	CT_LbsNetProtocol* iNetworkProtocol;
	RPointerArray<CT_LbsTestActiveManager> iClientTestManagers;

	/** The information of the clients of LBS */
	RPointerArray<CT_ClientData> iClients;
	
	// used for "one off" tests
	// =1 - LastKnownPosition test
	// =2 - MaxAge test
	// =3 - EarlyComplete test 
	TInt iSpecialTestMode;
	};

#endif //__CT_LBS_MOLR_TRACKING_H__

