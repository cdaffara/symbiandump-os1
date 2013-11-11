// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the CSmspCommDbEvent class
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smspcdbTraces.h"
#endif

#include "smspcdb.h"
#include "smspmain.h"
#include "smsuset.h"
#include "SmsuTimer.h"


/**
 *  Private constructor used in the first phase of construction.
 *  
 *  @param aSmsSettings Reference to sms configuration settings
 *  @param aSmsMessaging Reference to sms message
 *  @param aPriority CActive priority
 */
CSmspCommDbEvent::CSmspCommDbEvent(const TSmsSettings& aSmsSettings, RMobileSmsMessaging& aSmsMessaging, TInt aPriority)
  : CSmsuActiveBase(aPriority),
	iSmsSettings(aSmsSettings),
	iSmsMessaging(aSmsMessaging)
	{
	} // CSmspCommDbEvent::CSmspCommDbEvent


CSmspCommDbEvent::~CSmspCommDbEvent()
	{
	iProperty.Close();
	} // CSmspCommDbEvent::~CSmspCommDbEvent


void CSmspCommDbEvent::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPCOMMDBEVENT_CONSTRUCTL_1, "CSmspCommDbEvent::ConstructL()");

	ConstructTimeoutL();
	} // CSmspCommDbEvent::ConstructL


/**
 *  Starts the state machine by requesting the property to notify us when
 *  the CommDB change event is fired.
 */
void CSmspCommDbEvent::NotifyOnEvent()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPCOMMDBEVENT_NOTIFYONEVENT_1, "CSmspCommDbEvent::NotifyOnEvent()");

	iProperty.Subscribe(iStatus);
	SetActive();
	} // CSmspCommDbEvent::NotifyOnEvent


/**
 *  Restart notification if there is an error other than KErrCancel
 *  
 *  @param aStatus a status value.
 */
void CSmspCommDbEvent::Complete(TInt aError)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPCOMMDBEVENT_COMPLETE_1, "CSmspCommDbEvent::Complete()");

	//
	// Call the base function to perform the actual complete...
	//
	CSmsuActiveBase::Complete(aError);

	//
	// If the request was not cancelled, then restart it...
	//
	if (aError != KErrCancel  &&  aError != KErrTimedOut)
		{
		NotifyOnEvent();
		}
	} // CSmspCommDbEvent::Complete
