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
//

#include "ssmpropertymonitor.h"
#include "cmnpanic.h"
#include "ssmdebug.h"

/**

 */
CSsmPropertyMonitor* CSsmPropertyMonitor::NewL(CSsmSystemWideProperty& aObserver, TUint aSwpKey, TUid aProcessSid)
	{
	CSsmPropertyMonitor* self = new (ELeave) CSsmPropertyMonitor(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aSwpKey, aProcessSid);
	CleanupStack::Pop(self);
	return self;
	} //lint !e1746 Suppress parameter 'aProcessSid' could be made const reference

/**

 */
CSsmPropertyMonitor::CSsmPropertyMonitor(CSsmSystemWideProperty& aObserver) : 
	CActive(CActive::EPriorityStandard),
	iObserver(aObserver),
	iSwpLocal(TSsmSwp(0,0))
	{
	CActiveScheduler::Add(this);
	}

/**

 */
CSsmPropertyMonitor::~CSsmPropertyMonitor()
	{
	Cancel();
	iSwpRemote.Close();
	}

/**

 */
void CSsmPropertyMonitor::ConstructL(TUint aSwpKey, TUid aProcessSid)
	{
	SSMLOGLEAVEIFERROR(iSwpRemote.Connect(aSwpKey, aProcessSid));
	
	//Setup change subscription
	iStatus = KRequestPending;
	iSwpRemote.Subscribe(iStatus);
	SetActive();
	
	//Read current value and store locally
	TInt value = 0;
	if(KErrNone == iSwpRemote.GetValue(value))
		{
		iDefined = ETrue;
		}
	iSwpLocal.Set(aSwpKey, value);
	} //lint !e1746 Suppress parameter 'aProcessSid' could be made const reference

/**

 */
void CSsmPropertyMonitor::DoCancel()
	{
	iSwpRemote.SubscribeCancel();
	}

/**

 */
void CSsmPropertyMonitor::RunL()
	{
	SSMLOGLEAVEIFERROR(iStatus.Int());

	//Re-issue a subscription request before retrieving the current value
	iStatus = KRequestPending;
	iSwpRemote.Subscribe(iStatus);
	SetActive();
	
	//Read current value and store locally
	TInt newValue = 0;
	SSMLOGLEAVEIFERROR(iSwpRemote.GetValue(newValue));
	iSwpLocal.Set(iSwpLocal.Key(), newValue);
	
	//Notify our clients
	iObserver.NotifySubscribers(iSwpLocal);			
	}

/**
@panic ECmnPropMonError1 if an error occurs in RunL
 */
TInt CSsmPropertyMonitor::RunError(TInt aError)
	{
	//All possible RProperty errors; KErrPermissionDenied, KErrNotFound and KErrArgument
	//should occur already in ConstructL and never in RunL.
	
	DEBUGPRINT2A("CSsmPropertyMonitor::RunError: %d", aError);
	User::Panic(KPanicSsmCmn, ECmnPropMonError1);  //A panic here is easier to track than a panic in CActive
	return aError; //lint !e527 Unreachable
	}

/**

 */
TInt CSsmPropertyMonitor::GetValue(TSsmSwp& aSwp) const
	{
	if(iDefined)
		{
		aSwp.Set(iSwpLocal.Key(), iSwpLocal.Value());
		return KErrNone;
		}
	
	//The property was not yet defined when this monitor was created
	TInt value = 0;
	const TInt err = iSwpRemote.GetValue(value);
	if(KErrNone == err)
		{
		const_cast<TSsmSwp&>(iSwpLocal).Set(iSwpLocal.Key(), value);
		aSwp.Set(iSwpLocal.Key(), value);
		}
	return err;
	}
