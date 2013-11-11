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
// A wrapper around the location reqeust bus. This bus is used by the
// location server to issue requests to the Network Location manager
// 
//

/**
 @file
 @InternalComponent
*/

#ifndef __NETREQUESTBUS_H__
#define __NETREQUESTBUS_H__

#include <e32base.h>
#include "LbsInternalInterface.h"

/**
Wrapper around the RLbsPositionUpdateRequests
Assume something else (root) has already called RLbsPositionUpdateRequests::InitializeL()
therefore just call RLbsPositionUpdateRequests::Open
*/
class CNETRequestBus
	{
public:
	static CNETRequestBus* NewL(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel);
	CNETRequestBus(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel);
	~CNETRequestBus();
	void ConstructL();
	void PublishRequestL(const TLbsPositionUpdateRequestBase& aRequest);
	void ReadRequestL(TLbsPositionUpdateRequestBase& aRequest);
protected:
	/** from the Internal API, the request bus, a RProperty wrapper */
	RLbsPositionUpdateRequests	iNETRequestBus;
	/** the channel indentifier of the request property */
	const RLbsPositionUpdateRequests::TChannelIdentifer& 	iChannel;
	};

#endif // __NETREQUESTBUS_H__
