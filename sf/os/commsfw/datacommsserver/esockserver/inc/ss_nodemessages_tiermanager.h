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

#ifndef SYMBIAN_NODEMESSAGES_TIERMANAGER_H
#define SYMBIAN_NODEMESSAGES_TIERMANAGER_H

#include <comms-infras/ss_nodemessages_selector.h>

namespace ESock
{
/**
Message interface for tier status providers
Nodes wishing to implement this interface must implment all message protocols in the interface.

@publishedPartner
@released
*/
class TCFTierStatusProvider
	{
private:
  	enum
		{
		ECFTierStatusQuery              = 1,
		ECFTierStatus        			= 2,
		ECFTierNotificationRegistration	= 3,
		ECFTierNotification				= 4,
		};
	
public:
    enum { ERealmId = 0x10285F51 }; //UID allocated on 4/6/08 from KUidNodeMessageRealms

	/**
	Sent to a TierManager of interest by anyone (e.g. CConnectionServer)
	who wants to enquire (one off) for the status of that tier.

	Example (query):
	"tell me which connections are started w.r.t. this tier"

    @li Type:      Request for TTierStatus response.
    @li Fallible:  Yes

    @param iBundle (of type CParameterBundleBase) containing query.
				   See CConnectionServParameterBundle header for structure and examples.
				   Ownership of the bundle (i.e. responsibility to delete) passes
				   to the recipient of this message (even in case of error).
				   The core tier implementation handles this.
	@param iMessage (of type RMessage2) containing the platsec info of the
    			    originator of the request - verified by the tier as appropriate.
    			    Obviously if the tier decides the permissions are insufficient
    			    a KErrPermissionDenied will be returned in a TError message.
	*/
	typedef TMessageSigBundleRMessage2<ECFTierStatusQuery, TCFTierStatusProvider::ERealmId> TTierStatusQuery;

    /**
	Response from TierManager to a querying node (one off).

	Example (response to above query):
	"Access points 2, 3 and 6 are started"

    @li Type:      Response to TTierStatusQuery
    @li Fallible:  No

    @param iBundle (of type CParameterBundleBase) containing results.
				   See CConnectionServParameterBundle header for structure and examples.
				   Ownership of the bundle (i.e. responsibility to delete) passes
				   to the recipient of this message.
				   The core CConnectionServer implementation handles this but if
				   it is received by another node inside the mesh making an
				   internal query, that node will have to remember to delete the
				   received bundle once it's finished using it.
	*/
	typedef TMessageSigBundle<ECFTierStatus, TCFTierStatusProvider::ERealmId> TTierStatus;

    /**
	Sent to a TierManager of interest by anyone (e.g. CConnectionServer)
	who wants to register for notifications of changes in
	status of a tier.

	Example (registration):
	"Tell me when access point 6 changes in started status"

    @li Type:      Request for 1 - n TTierNotification notifications.
    @li Fallible:  Yes - generally early on when setting up the change monitoring.

    @param iBundle (of type CParameterBundleBase) containing query.
	               See CConnectionServParameterBundle header for structure and examples.
				   Ownership of the bundle (i.e. responsibility to delete) passes
				   to the recipient of this message (even in case of error).
				   The core tier implementation handles this.
	@param iMessage (of type RMessage2) containing the platsec info of the
    			    originator of the request - verified by the tier as appropriate.
					Obviously if the tier decides the permissions are insufficient
					a KErrPermissionDenied will be returned in a TError message.
	*/
	typedef TMessageSigBundleRMessage2<ECFTierNotificationRegistration, TCFTierStatusProvider::ERealmId> TTierNotificationRegistration;

    /**
	Notification of changes from TierManager to a querying node.

	Example (responses to above registration):
	                 (empty bundle) - signifies the monitoring is active and
										all events from now on will be caught
						.. some time passes, then ..
					"Access point 6 is now started"
						.. some time passes, then ..
					"Access point 6 is now stopped"
						.. some time passes, then ..
					"Access point 6 is now started"
						.. etc. etc. ..

	@li Type:      Response to TTierNotificationRegistration
    @li Fallible:  No

    @param iBundle (of type CParameterBundleBase) containing results.
				   See CConnectionServParameterBundle header for structure and examples.
				   Ownership of the bundle (i.e. responsibility to delete) passes
				   to the recipient of this message.
				   The core CConnectionServer implementation handles this but if
				   it is received by another node inside the mesh making an
				   internal query, that node will have to remember to delete the
				   received bundle once it's finished using it.
	*/
	typedef TMessageSigBundle<ECFTierNotification, TCFTierStatusProvider::ERealmId> TTierNotification;
	};

/**
Message interface for data collectors
Nodes wishing to implement this interface must implment all message protocols in the interface.

@publishedPartner
@released
*/
class TCFDataCollector
	{
private:
  	enum
		{
		ECFTierGetDataCollectors		= 1,
		ECFTierDataCollectors			= 2
		};
	
public:
    enum { ERealmId = 0x10285F43 }; //UID allocated on 4/6/08 from KUidNodeMessageRealms
    
	/**
	Sent by the TierNotificationActivity core engine in the tier
	manager to the same tier manager. This is so that it triggers
	a "one-shot" GetDataCollectorsActivity, which can be customised
	for the technology without having to rewrite the core
	TierNotificationActivity.. GetDataCollectorsActivity
	creates the appropriate MDataCollector instances for
	the current query (which would include collectors specific
	to the technology, started when some custom query ParameterSet is
	present in the bundle)

    @li Type:	Request for TTierDataCollectors.
    @li Fallible:  Yes
	*/
	typedef Messages::TMessageSigVoid<ECFTierGetDataCollectors, TCFDataCollector::ERealmId> TTierGetDataCollectors;

    /**
	Returned by GetDataCollectorsActivity to the core
	TierNotificationActivity (see above)
	
    @li Type:		 Response to the TTierGetDataCollectors request
    @li Fallible:  No

	@param iPtr	 (of type TAny*, implicitly cast from RPointerArray<MDataCollector>*)
				 The list of MDataCollector instances created for the query.
				 Ownership of the array and its members passed with this message
				 back to the core engine, who will handle cleanup.
    ===========================================================================*/
	typedef Messages::TMessageSigAny<ECFTierDataCollectors, TCFDataCollector::ERealmId> TTierDataCollectors;
	};

/**
Message interface for tier managers
Doesn't specify any new message interfaces itself, but pulls in tier status provider and selector

@publishedPartner
@released
*/
class TCFTierManager : public TCFTierStatusProvider, public TCFSelector
	{
public:
    enum { ERealmId = 0x10285F52 }; //UID allocated on 4/6/08 from KUidNodeMessageRealms
	};

} //namespace esock

#endif
//SYMBIAN_NODEMESSAGES_TIERMANAGER_H

