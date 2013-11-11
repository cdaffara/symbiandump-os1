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
//

/**
 @file
 @InternalComponent
*/

#include "cagpsrequestbus.h"
#include "lbsdevloggermacros.h"

/**
Create a request bus, this should be called from the request queue

@return The pointer to the request bus
*/
CAgpsRequestBus* CAgpsRequestBus::NewL(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel)
	{
	CAgpsRequestBus* self = new (ELeave) CAgpsRequestBus(aChannel);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
/**
constructor,

@param aChannel Channel identifier for the RProperty key
*/
CAgpsRequestBus::CAgpsRequestBus(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel) : iChannel(aChannel)
	{
	}

/**
Close the RProperty.
*/
CAgpsRequestBus::~CAgpsRequestBus()
	{
	iAGPSRequestBus.Close();
	}
	
/**
Open the RProperty object wrappered by the request bus.
Request bus is implemented as Publish & Subscribe mechanism.
*/
void CAgpsRequestBus::ConstructL()
	{
	iAGPSRequestBus.OpenL(iChannel);
	}

/**
Publish the position request onto the request bus.

@param aRequest The TLbsPositionUpdateRequestBase reference to the postion update request
*/
void CAgpsRequestBus::PublishRequestL(const TLbsPositionUpdateRequestBase& aRequest)
	{
	LBSLOG(ELogP1, "LocServer - CAgpsRequestBus::PublishRequestL");
	User::LeaveIfError(iAGPSRequestBus.SetPositionUpdateRequest(aRequest));
	}
