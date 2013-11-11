// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __STATEADAPTATIONREF_H__
#define __STATEADAPTATIONREF_H__

#include <ssm/ssmadaptation.h>

/**

 CStateAdaptationRefEventHandler acts as a dummy event notifier in the absence of Cooperating system

  *********Need for the class*********
  
  On a device when NotifyCoopSysEvent is called, State Adaptation Plug-in would request for notification 
  from the Cooperating System for 'aEvent'.
  
  The reference implementation has to complete the notification request with KErrNotSupported.
  
  But this implementation will cause problems in the following scenario.
  
  *********Problem*********
  
  This API is used by Adaptation server to service the clients which request for notifications .
  This is done as follows.
  
  Adaptation server always tracks the events by calling NotifyCoopSysEvent during the whole life time 
  of the server.It first calls this API in the startup of the server and subsequently whenever an event 
  is notified(by completing the call NotifyCoopSysEvent) adaptation server resubmits the request 
  by calling NotifyCoopSysEvent .
  
  In this context if NotifyCoopSysEvent just completes the message immediately this results in a infinite 
  loop and starves other active objects with same priority.
  
  This makes testing of Adaptation servers notification functionality impossible on PlatSim/device.
  
  *********Solution*********

  To solve this problem NotifyCoopSysEvent will forward the notification request from adaptation server 
  to the test active object(CStateAdaptationRefEventHandler) which has lesser priority compared to 
  adaptation servers active object.
  In the RunL() method of this test active object the notification request is completed.  
  
  This will delay the completion of notification message till all the servers active objects 
  which have higher priority are run.
  
  @internalComponent
  
   
*/


class CStateRefAdaptationTimer : public CTimer
	{
public:
	CStateRefAdaptationTimer();
	~CStateRefAdaptationTimer();
	static CStateRefAdaptationTimer* NewL();
	void After(TTimeIntervalMicroSeconds32 aCancelDelay, TRequestStatus& aStatus);	
	void DoCancel();
private:
	virtual void RunL();

private:
	TRequestStatus* iReqStatus;
	};


// C Class
class CStateAdaptationRef : public CBase, public MStateAdaptation
	{
public:
	static CStateAdaptationRef* NewL();
	~CStateAdaptationRef();

	//from MStateAdaptation
	void Release();
	void RequestCoopSysStateChange(TSsmState aState, TRequestStatus& aStatus);
	void RequestCoopSysSelfTest(TRequestStatus& aStatus);
	void RequestCoopSysPerformRestartActions(TInt aReason, TRequestStatus& aStatus);
	void RequestCoopSysPerformShutdownActions(TInt aReason, TRequestStatus& aStatus);
	void RequestCoopSysPerformRfsActions(TSsmRfsType aRfsType, TRequestStatus& aStatus);
	void RequestCancel();

	void NotifyCoopSysEvent(TDes8& aEvent, TRequestStatus& aStatus);
	void NotifyCancel();
	
private:
	CStateAdaptationRef();
	void ConstructL();
	//Helper function to check for P&S Key
    TBool IsTestPsKeyDefined();

private:
    CStateRefAdaptationTimer* iTimer;
	MStateAdaptation* iSaaStateAdaptationDll;
	RLibrary iSaaStateAdaptationLib;
	};





#endif // __STATEADAPTATIONREF_H__
