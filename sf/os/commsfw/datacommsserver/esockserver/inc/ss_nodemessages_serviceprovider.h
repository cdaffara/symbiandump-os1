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

#ifndef SYMBIAN_NODEMESSAGES_SERVICEPROVIDER_H
#define SYMBIAN_NODEMESSAGES_SERVICEPROVIDER_H

#include <comms-infras/ss_nodemessages.h>
#include <elements/nm_messages_peer.h>
#include <comms-infras/ss_nodemessages_peer.h>
#include <comms-infras/ss_nodemessages_parameters.h>

namespace ESock
{
class TCFServiceProvider : public ESock::TCFPeer
	{
  private:
  	enum
 	{
	ECFStart                        = 1,
	ECFStop							= 2,
	ECFStarted                      = 3,
	ECFStopped               		= 4,
	ECFCommsBinderRequest			= 5,
	ECFCommsBinderResponse			= 6,
    ECFCommsBindToComplete          = 7,
	};
protected:

public:
    enum { ERealmId = 0x10285F4F }; //UID allocated on 4/6/08 from KUidNodeMessageRealms 
    /**
    A request for a dataclient. The recipient of TCommsBinderRequest has to respond with 
    TCommsBinderResponse carrying one of its data clients. Once the requestor (control client) 
    consumes the data client, the consumption is acknowledged back to the servicing node with 
    TCFServiceProvider::TBindToComplete.            
    
    @li Type:      Request for TCommsBinderResponse
    @li Fallible:  Yes

    @param iValue  The semantics of this parameter depends on the recipient's plane:
                          CPR : - dataclient type, i.e.: EAttachToDefault/ECreateNew/EWaitForIncoming
                          SCPR: - protocol Id - could be left blank when not needed
                          MCPR: - attach flags (normal/monitor) 0 for start only
    @param iFamilyBundle parameters to assist the finding/creation of dataclient
    */
    typedef TMessageSigNumberDefaultFamilyBundleDefaultToNull<ECFCommsBinderRequest, TCFServiceProvider::ERealmId> TCommsBinderRequest;
    
    /**
    @li Type:       Request for TCFServiceProvider::TBindToComplete
                    Response to TCFServiceProvider::TCommsBinderRequest
    @li Fallible:   Yes
    @param iNodeId  The requested binder
    
    */
    typedef Messages::TMessageSigNodeId<ECFCommsBinderResponse, TCFServiceProvider::ERealmId> TCommsBinderResponse;   
    
    /**
    Response to TCommsBinderResponse. 
    @li Type:       Response to TCommsBinderResponse
    @param iValue   Result of data client consumption
    */
    typedef Messages::TMessageSigNumberDefault<ECFCommsBindToComplete, TCFServiceProvider::ERealmId> TBindToComplete;

    /**
    Request to start the stack. The recipient should start the stack below it and start its own layer.
    @li Type:      Request for TStarted.
    @li Fallible:  Yes
    */
    typedef Messages::TMessageSigVoid<ECFStart,   TCFServiceProvider::ERealmId> TStart;
    typedef Messages::TMessageSigVoid<ECFStarted, TCFServiceProvider::ERealmId> TStarted;

    /**
    Request to stop the stack. The recipient should stop its own layer and subsequently stop the stack below it.

    @li Type:      Request for TStopped.
    @li Fallible:  No

    @param: iValue - stop code (reason).
    */
    typedef Messages::TMessageSigNumber<ECFStop,    TCFServiceProvider::ERealmId> TStop;
    typedef Messages::TMessageSigNumber<ECFStopped, TCFServiceProvider::ERealmId> TStopped;
	};

class TCFControlClient : public TCFPeer
	{
private:
  	enum
		{
		ECFGoneDown          			= 1,
		ECFGoneUp                       = 2
		};

public:
    enum { ERealmId = 0x10285F3F }; //UID allocated on 4/6/08 from KUidNodeMessageRealms

    /**
    Notification about the stack below gone down. This is an equivallent of of an 
    unsolicited TStopped (i.e.: the stack below is notionally stopped).

    @li Type:      Notification

    @param       iValue1 - reason for going down.
                 iValue2 - Access Point id as provisioned with CAccessPointConfig::ProviderInfo()::APId()
    */
    typedef Messages::TMessageSigNumberDefaultNumberDefault<ECFGoneDown, TCFControlClient::ERealmId> TGoneDown;
    
    /**
    Notification about the stack below gone up. This is an equivallent of of an unsolicited 
    TStarted  (i.e.: the stack below is notionally started).

    @li Type:      Notification

    */
	typedef Messages::TMessageSigVoid<ECFGoneUp, TCFControlClient::ERealmId> TGoneUp;
	};

} //namespace ESock

#endif
//SYMBIAN_NODEMESSAGES_SERVICEPROVIDER_H

