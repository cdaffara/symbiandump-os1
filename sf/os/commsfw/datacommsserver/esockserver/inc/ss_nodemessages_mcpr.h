// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_NODEMESSAGES_MCPR_H
#define SYMBIAN_NODEMESSAGES_MCPR_H

#include <comms-infras/ss_nodemessages_mobility.h>
#include <comms-infras/ss_nodemessages_availability.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_serviceprovider.h>

namespace ESock
{


/**
Message interface for meta connection providers
Nodes wishing to implement this interface must implment all message protocols in the interface.

@publishedPartner
@released
*/
class TCFMcpr : public TCFServiceProvider, public TCFMobilityProvider, public TCFAvailabilityProvider, public TCFControlProvider
	{
private:
  	enum
 	{
	ECFReConnect                        = 1,
	ECFReConnectComplete                = 2,
	ECFProviderStatusChangeRegistration = 3,
	ECFProviderStatusChange             = 4
	};

public:
    enum { ERealmId = 0x10285F48 }; //UID allocated on 4/6/08 from KUidNodeMessageRealms

	/**
    Sent to a peer other than the service provider to register for TProviderStatusChange.

    @li Type:      Request for TProviderStatusChange notification.
    @li Fallible:  Yes
	*/
	typedef Messages::TMessageSigVoid<ECFProviderStatusChangeRegistration, TCFMcpr::ERealmId> TProviderStatusChangeRegistration;

	/**
	Enumeration of possible values for TProviderStatusChange. 
	*/
	enum TProviderStatus
	{
	EStopped = 0,
	EStarted = 1,
	};

    /**
	Equivalent to TStatusChange but currently also used as a notification to peers other
	than control clients who requested it via TProviderStatusChangeRegistration.

	The value can be anything from TCFMCpr::TProviderStatus or a custom defined value so
	long as it is understood by the MCPR and the control client recieving it.
	
    @li Type:      Notification
    @li Fallible:  No

    @param        iValue  new state
	*/
	typedef Messages::TMessageSigNumber<ECFProviderStatusChange, TCFMcpr::ERealmId> TProviderStatusChange;

    /**
	Sent to an MCPR to request reconnection of its layer.
	Reconnection combines layer rebinding together with stopping
	and starting of the new layer (which can be either make-before-brake
	or brake-before-make, however implemented by this activity).

    @li Type:      Request
    @li Fallible:  Yes

    @param        iNodeId1  from (service provider provided)
    @param        iNodeId1  to (service provider provided)
	*/
	typedef Messages::TMessageSigNodeIdNodeId<ECFReConnect, TCFMcpr::ERealmId> TReConnect;

    /**
	Response to TReConnect signifying that the reconnect has finished.

    @li Type:      Response
    @li Fallible:  No
	*/
	typedef Messages::TMessageSigVoid<ECFReConnectComplete, TCFMcpr::ERealmId> TReConnectComplete;
	};

} //namespace esock

#endif
//SYMBIAN_NODEMESSAGES_MCPR_H

