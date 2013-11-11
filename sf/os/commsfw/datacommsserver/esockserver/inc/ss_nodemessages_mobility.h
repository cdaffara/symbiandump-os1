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
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_NODEMESSAGES_MOBILITY_H
#define SYMBIAN_NODEMESSAGES_MOBILITY_H

namespace ESock
{

/**
The interface for a mobility provider, i.e.: a node that can swap access points and needs
to be instructed (by TCFMobilityControlClients via this interface) when to do so. 
TCFMobilityControlClient are the reverse facing role (nodes that want to instruct
TCFMobilityProviders to swap the access points).

Typical TCFMobilityProviders are: 
-CPRs (towards the application represented by ESock::AMobilityProtocolReq extension API) 
-MCPRs towards CPRs. 

@publishedPartner
@released
@see TCFMobilityControlClient
*/
class TCFMobilityProvider
	{
  private:
  enum
    {
    ECFStartMobility      = 1,
    ECFMobilityStarted    = 2,
    ECFMigrationRequested = 3,
    ECFMigrationComplete  = 4,
    ECFMigrationAccepted  = 5,
    ECFMigrationRejected  = 6
    };
protected:

public:
    enum { ERealmId = 0x10285F4A }; //UID allocated on 4/6/08 from KUidNodeMessageRealms

    /*==========================================================================
    [Name]:      TStartMobility
    [Semantics]: The client (TCFMobilityControlClient) requests the provider ('this' node)
                 to start mobility, i.e.: to prepare resources and start sending notifications 
                 (TCFMobilityControlClient::TMigrationNotification) about possible migrations
                 so that the client can decide upon them.
                 Following TStartMobility the client can perform a number of migrations, each
                 migration mandating a strict handshake to mark the beginning and end of that
                 migration. The handshake goes as follows:
                 TCFMobilityControlClient                                  TCFMobilityProvider
                                             <-TMigrationNotification
                                               TMigrationRequested->
                                             <-TMigrationComplete
                                               TMigrationAccepted->
                                               
                 At any time outside the handshakes, the client may instruct the provider
                 to stop mobility, i.e.: dealocate the resources and stop sending notifications.
                 The client can do that canceling (TEBase::TCancel) TStartMobility.
                 
    [Type]:      Request for TMobilityStarted
    [Fallible]:  Yes

    [Structure]:
    param        iValue - client id, used only when the request is originated by an application
                 when there is no iSender and it takes the value of TSubSessionUniqueId
    ===========================================================================*/
    typedef Messages::TMessageSigAnyNumberDefault<ECFStartMobility, TCFMobilityProvider::ERealmId> TStartMobility;
    typedef Messages::TMessageSigVoid<ECFMobilityStarted, TCFMobilityProvider::ERealmId> TMobilityStarted;

    /*==========================================================================
    [Name]:      TMigrationRequested
    [Semantics]: Request to perform the migration advertised by the most recent
                 TCFMobilityControlClient::TMigrationNotification. The provider
                 marks the completion of the migration by sending TMigrationComplete,
                 at which point the client gets to verify the new access point
                 and finally accept it (TMigrationAccepted) or reject it (TMigrationRejected)
                 closing the handshake.

    [Type]:      Request for TMigrationComplete
    [Fallible]:  Yes

    [Structure]:
    ===========================================================================*/
    typedef Messages::TMessageSigVoid<ECFMigrationRequested, TCFMobilityProvider::ERealmId> TMigrationRequested;
    typedef Messages::TMessageSigVoid<ECFMigrationComplete, TCFMobilityProvider::ERealmId> TMigrationComplete;

    /*==========================================================================
    [Name]:      TMigrationAccepted
    [Semantics]: Mobility handshake closure. The client accepts the new access point.

    [Type]:      Notification
    [Fallible]:  No

    [Structure]:
    ===========================================================================*/
    typedef Messages::TMessageSigVoid<ECFMigrationAccepted, TCFMobilityProvider::ERealmId> TMigrationAccepted;

    /*==========================================================================
    [Name]:      TMigrationRejected
    [Semantics]: TMigrationRejected will have a different meaning depending on the provider's state:
                 (1) Directly following TCFMobilityControlClient::TMigrationNotification, in which case the client
                     ignores the new access point advertised with TCFMobilityControlClient::TMigrationNotification.
                     The provider stays with the current access point. TMigrationRejected terminates the handshake.  
                 (2) Directly following TCFMobilityProvider::TMigrationComplete, in which case the client rejects
                     the access point the provider migrated to. The provider may at this point
                     offer another access point (TCFMobilityControlClient::TMigrationNotification),
                     which could be one of the previous active access points the client decided
                     to try to upgrade from. If the provider has no alternative the client should
                     observe the connection going down. TMigrationRejected terminates the handshake. 
                 (3) Outside of the mobility handshake. The client decides that, for whatever reasons
                     (e.g.: no desired routability), it is not happy with the current access point and wants to
                     force mobility. If the provider has an alternative access point to offer, the access point should
                     be offered with TCFMobilityControlClient::TMigrationNotification.

    [Type]:      Notification
    [Fallible]:  No

    [Structure]:
    ===========================================================================*/
    typedef Messages::TMessageSigNumber<ECFMigrationRejected, TCFMobilityProvider::ERealmId> TMigrationRejected;
	};



/**
Callback/reverse-facing interface for TCFMobilityProvider. Anybody invoking TCFMobilityProvider
must implement TCFMobilityControlClient.

@publishedPartner
@released
@see TCFMobilityProvider
*/
class TCFMobilityControlClient
    {
  private:
  enum
    {
    ECFMigrationNotification = 1
    };
protected:

public:
    enum { ERealmId = 0x10285F49 }; //UID allocated on 4/6/08 from KUidNodeMessageRealms
    /*==========================================================================
    [Name]:      TMigrationNotification
    [Semantics]: Notification about possible mobility migration sent by the TCFMobilityProvider.
                 The recipient _must_ respond to this notifaction by commencing the migration
                 handshake. The recipient must therefore respond with either:
                 TCFMobilityProvider::TMigrationRequested to pursue the migration
                 TCFMobilityProvider::TMigrationRejected to ignore the offer.

    [Type]:      Notification
    [Fallible]:  No

    [Structure]:
    param        iValue1 - old access point
                 iValue2 - new access point
                 iValue3 - is upgrade
    ===========================================================================*/
    typedef Messages::TMessageSigNumberNumberNumberNumber<ECFMigrationNotification, TCFMobilityControlClient::ERealmId> TMigrationNotification;
    };

} //namespace ESock

#endif //SYMBIAN_NODEMESSAGES_MOBILITY_H

