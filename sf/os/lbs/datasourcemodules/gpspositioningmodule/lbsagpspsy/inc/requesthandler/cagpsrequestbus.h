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
// location server to issue requests to the AGPS manager
// 
//

/**
 @file
 @InternalComponent
*/

#ifndef __AGPSREQUESTBUS_H__
#define __AGPSREQUESTBUS_H__

#include <e32base.h>
#include "LbsInternalInterface.h"

/**
Wrapper around the RLbsPositionUpdateRequests
Assume something else has already called RLbsPositionUpdateRequests::InitializeL()
therefore just call RLbsPositionUpdateRequests::Open
*/
class CAgpsRequestBus
	{
public:
	static CAgpsRequestBus* NewL(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel);
	CAgpsRequestBus(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel);
	~CAgpsRequestBus();
	void ConstructL();
	void PublishRequestL(const TLbsPositionUpdateRequestBase& aRequest);

protected:
	/** from the Internal API, the request bus, a RProperty wrapper */
	RLbsPositionUpdateRequests	iAGPSRequestBus;
	/** the channel indentifier of the request property */
	const RLbsPositionUpdateRequests::TChannelIdentifer& 	iChannel;
	};

#endif // __AGPSREQUESTBUS_H__
