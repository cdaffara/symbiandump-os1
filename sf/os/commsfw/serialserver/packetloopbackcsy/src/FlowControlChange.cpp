// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of class CFlowControlChange
// 
//

/**
 @file
*/

#include <e32property.h>

#include "LoopbackConfig.h"
#include "FlowControlChange.h"

CFlowControlChange* CFlowControlChange::NewL(TUint aUnit, MFlowControlChangeCallBack* aCallback)
	{
	CFlowControlChange* self = new (ELeave) CFlowControlChange(aUnit, aCallback);
	return self;
	}

CFlowControlChange::CFlowControlChange(TUint aUnit, MFlowControlChangeCallBack* aCallback)
	: CActive(EPriorityStandard), iCallback(aCallback), iUnit(aUnit), iFlowControlIsOn(EFalse)
	{
	CActiveScheduler::Add(this);

	TBool flowControlIsOn = EFalse;
	iProperty.Attach(KUidPSCsyFlowControlCategory, iUnit);
	iProperty.Subscribe(iStatus);
	TInt ret = RProperty::Get(KUidPSCsyFlowControlCategory, iUnit, flowControlIsOn);
	if (ret == KErrNone)
		iFlowControlIsOn = flowControlIsOn;
	SetActive();
	}
	
CFlowControlChange::~CFlowControlChange()
	{
	Cancel();
	iProperty.Close();
	}

/** 
 @return ETrue if flow control is currently on, EFalse if it is not
 */
TBool CFlowControlChange::FlowControlIsOn() const
	{
	return iFlowControlIsOn;
	}

/**
 Stops listening for changes to flow control
 */
void CFlowControlChange::DoCancel()
	{
	iProperty.Cancel();
	}

/**
 Notifies the callback of a change in flow control, and resubscribes to obtain future notifications of property changes
 */
void CFlowControlChange::RunL()
	{
	User::LeaveIfError(KErrNone);	// To prevent LeaveScan warning

	// Resubscribe before retrieving the value so we don't miss any changes
	TInt status = iStatus.Int();
	iProperty.Subscribe(iStatus);
	SetActive();
	
	TBool flowControlIsOn = EFalse;
	if(status == KErrNone)
		{
		RProperty::Get(KUidPSCsyFlowControlCategory, iUnit, flowControlIsOn);
		}
	if (flowControlIsOn != iFlowControlIsOn)
		{
		iFlowControlIsOn = flowControlIsOn;
		if (iFlowControlIsOn)
			iCallback->StartFlowControl();
		else
			iCallback->StopFlowControl();
		}
	}

