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

#include "cnetrequestbus.h"
#include "lbsdevloggermacros.h"

/**
Create a request bus, this should be called from the request queue

@return The pointer to the request bus
*/
CNETRequestBus* CNETRequestBus::NewL(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel)
	{
	CNETRequestBus* self = new (ELeave) CNETRequestBus(aChannel);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
/**
constructor,

@param aChannel Channel identifier for the RProperty key
*/
CNETRequestBus::CNETRequestBus(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel) : iChannel(aChannel)
	{
	}

/**
Close the RProperty.
*/
CNETRequestBus::~CNETRequestBus()
	{
	iNETRequestBus.Close();
	}
	
/**
Open the RProperty object wrappered by the request bus.
Request bus is implemented as Publish & Subscribe mechanism.
*/
void CNETRequestBus::ConstructL()
	{
	iNETRequestBus.OpenL(iChannel);
	}

/**
Publish the position request onto the request bus.

@param aRequest The TLbsPositionUpdateRequestBase reference to the postion update request
*/
void CNETRequestBus::PublishRequestL(const TLbsPositionUpdateRequestBase& aRequest)
	{
	LBSLOG(ELogP1, "LocServer - CNETRequestBus::PublishRequestL");
	User::LeaveIfError(iNETRequestBus.SetPositionUpdateRequest(aRequest));
	}
	
/**
Read the request from the request bus. This function should be called by request bus observers subscribed to the bus.

@param aRequest A reference to the returned position update request 
*/
void CNETRequestBus::ReadRequestL(TLbsPositionUpdateRequestBase& aRequest)
	{
	User::LeaveIfError(iNETRequestBus.GetPositionUpdateRequest(aRequest));
	}
	
