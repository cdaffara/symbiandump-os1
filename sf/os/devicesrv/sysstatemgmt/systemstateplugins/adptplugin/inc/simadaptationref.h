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

#ifndef __SIMADAPTATIONREF_H__
#define __SIMADAPTATIONREF_H__

#include <ssm/ssmadaptation.h>

/**

 CSimAdaptationRefEventHandler acts as a dummy event notifier in the absence of Cooperating system

  *********Need for the class*********
  
  On a device when NotifySimEvent is called, Sim Adaptation Plug-in would request for notification 
  from the Cooperating System for 'aEvent'.
  
  The reference implementation has to complete the notification request with KErrNotSupported.
  
  But this implementation will cause problems in the following scenario.
  
  *********Problem*********
  
  This API is used by Adaptation server to service the clients which request for notifications .
  This is done as follows.
  
  Adaptation server always tracks the events by calling NotifySimEvent during the whole life time 
  of the server.It first calls this API in the startup of the server and subsequently whenever an event 
  is notified(by completing the call NotifySimEvent) adaptation server resubmits the request 
  by calling NotifySimEvent .
  
  In this context if NotifySimEvent just completes the message immediately this results in a infinite 
  loop and starves other active objects with same priority.
  
  This makes testing of Adaptation servers notification functionality impossible on PlatSim/device.
  
  *********Solution*********

  To solve this problem NotifySimEvent will forward the notification request from adaptation server 
  to the test active object(CSimAdaptationRefEventHandler) which has lesser priority compared to 
  adaptation servers active object.
  In the RunL() method of this test active object the notification request is completed.  
  
  This will delay the completion of notification message till all the servers active objects 
  which have higher priority are run.
  
  @internalComponent
  
   
*/


class CSimRefAdaptationTimer : public CTimer
	{
public:
	CSimRefAdaptationTimer();
	~CSimRefAdaptationTimer();
	static CSimRefAdaptationTimer* NewL();
	void After(TTimeIntervalMicroSeconds32 aCancelDelay, TRequestStatus& aStatus);	
	void DoCancel();
private:
	virtual void RunL();

private:
	TRequestStatus* iReqStatus;
	};


// C Class
class CSimAdaptationRef : public CBase, public MSimAdaptation
	{
public:
	static CSimAdaptationRef* NewL();
	~CSimAdaptationRef();

	//from MSimAdaptation
	void Release();
	void GetSimOwned(TDes8& aOwnedPckg, TRequestStatus& aStatus);
	void GetCancel();

	void NotifySimEvent(TDes8& aTypePckg, TRequestStatus& aStatus);
	void NotifyCancel();

private:
	CSimAdaptationRef();
	void ConstructL();
	//Helper function to check for P&S Key
    TBool IsTestPsKeyDefined();
	
private:
	CSimRefAdaptationTimer* iTimer;
	MSimAdaptation* iSaaSimAdaptationDll;
	RLibrary iSaaSimAdaptationLib;
	};






#endif // __SIMADAPTATIONREF_H__
