// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include <bluetooth/hci/completingeventqueryhelper.h>


/**
Return a newly constructed CHCICompletingEventQueryHelper for determining Completion Events
*/
CHCICompletingEventQueryHelper* CHCICompletingEventQueryHelper::NewL(TBool aExpectsToComplete)
	{
	CHCICompletingEventQueryHelper* self = new(ELeave) CHCICompletingEventQueryHelper(aExpectsToComplete);
	return(self);
	}


/**
C'tor - initialise iExpectCompletingEvent
*/
CHCICompletingEventQueryHelper::CHCICompletingEventQueryHelper(TBool aExpectsToComplete)
	: iExpectCompletingEvent(aExpectsToComplete)
	{
	}


CHCICompletingEventQueryHelper::~CHCICompletingEventQueryHelper()
	{
	}

	
/**
Virtual from MHCICompletingEventQuery interface, used by HCICmdQController
*/
TBool CHCICompletingEventQueryHelper::MhceqCompletingEventExpected()
	{
	return iExpectCompletingEvent;
	}

	
/**
Setter for iExpectCompletingEvent
*/
void CHCICompletingEventQueryHelper::SetExpectsCompletingEvent(TBool aExpectCompletingEvent)
	{
	iExpectCompletingEvent = aExpectCompletingEvent;
	}

