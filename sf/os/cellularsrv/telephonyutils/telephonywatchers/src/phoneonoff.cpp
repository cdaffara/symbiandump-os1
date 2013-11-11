// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "phoneonoff.h"
#include <sacls.h>


//
// ------> MPhoneOnOffObserver (source)
//

EXPORT_C void MPhoneOnOffObserver::PhoneIsOff()
	{
	}

EXPORT_C  void MPhoneOnOffObserver::PhoneIsOn()
	{
	}


//
// ------> CPhoneOnOff (source)
//

CPhoneOnOff::CPhoneOnOff(MPhoneOnOffObserver& aPropertyPhoneWatcher)
:	CActive(CActive::EPriorityHigh), iPropertyPhoneWatcher(aPropertyPhoneWatcher)
	{
	CActiveScheduler::Add(this);
	}

CPhoneOnOff::~CPhoneOnOff()
	{
	Cancel();
	iPhonePowerProperty.Close();
	}

void CPhoneOnOff::ConstructL()
	{
	// Attach to Phone Power property
	User::LeaveIfError(iPhonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid)); 
	IssueRequestL();
	}

void CPhoneOnOff::IssueRequestL()
	{
	iPhonePowerProperty.Subscribe(iStatus);
		
	SetActive();
	iStatus = KRequestPending;
	}

void CPhoneOnOff::RunL()
	{
	if	(iStatus.Int() == KErrNone)
		{		
		IssueRequestL(); // reissue subscribe before checking property
				
		TInt phoneStatus;
		User::LeaveIfError(iPhonePowerProperty.Get(phoneStatus));		
		
		// Notify faxwatcher of phone off
		if	(phoneStatus == ESAPhoneOff)
			iPropertyPhoneWatcher.PhoneIsOff();
		else
			iPropertyPhoneWatcher.PhoneIsOn();		
		}
	}

void CPhoneOnOff::DoCancel()
	{	
	iPhonePowerProperty.Cancel();
	}
