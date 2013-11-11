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

#ifndef SYMBIAN_NODEMESSAGES_DATACLIENT_H
#define SYMBIAN_NODEMESSAGES_DATACLIENT_H

#include <elements/nm_signatures.h>
#include <elements/nm_messages_child.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_nodemessages_peer.h>

namespace ESock
{

/**
Message interface for data clients
Nodes wishing to implement this interface must implment all message protocols in the interface.

@publishedPartner
@released
*/
class TCFDataClient : public Messages::TEChild
	{
private:
  	enum
 	{
	ECFStart                        = 1,
	ECFStarted                      = 2,
	ECFStop				            = 3,
	ECFStopped                      = 4,
	ECFBindTo		                = 5,
	ECFBindToComplete		        = 6,
 	ECFProvisionConfig				= 7,
    ECFCommitBindTo                 = 8,
	};


public:
	
    enum { ERealmId = 0x10285F42 }; //UID allocated on 4/6/08 from KUidNodeMessageRealms


	/**
	Request to start the layer. The recipient should perform the 'start' routine
	pertaining to the technology/abstraction it represents. The recipient would usually
	send TStart to its dataclients in the course of implementing this request.

    @li Type: Request for TStarted.
    @li Fallible:  Yes.
	*/
    typedef Messages::TMessageSigVoid<ECFStart,   TCFDataClient::ERealmId> TStart;
    typedef Messages::TMessageSigVoid<ECFStarted, TCFDataClient::ERealmId> TStarted;

    /**
	Sent to a dataclient to request permanent stop of the layer.

    @li Type:      Request for TStopped.
    @li Fallible:  No.

    @param iValue  stop code (reason).
    ===========================================================================*/
    typedef Messages::TMessageSigNumber<ECFStop,    TCFDataClient::ERealmId> TStop;
    typedef Messages::TMessageSigNumber<ECFStopped, TCFDataClient::ERealmId> TStopped;

	/**
    Recipient should accept the node provided as the new service provider. 
	More precisely; the recipient should bind itself and the remainder of the layer it 
	represents (its dataclients) to the layer supplied by the sender of TBindTo.
	TBindTo may be sent when processing TControlClient::TNoBearer or for other reasons
	(e.g.: building the layer or rebinding the layer during mobility scenarios). 
	The sender of TBindTo should supply a startable layer, i.e.: as far as the sender
	is concerned the layer has a chance to start. 
	For instance; if an SCPR receives TBindTo, then the sender CPR has started
	the connection, which means the SCPR's attempt to start its new service
	provider has a chance of succeeding.                 
    
    @li Type:      Request for TBindToComplete
	@li Fallible:  Yes.

    @param iNodeId the node the dataclient should bind to.
    @param iValue  the indication on whether TBindTo is a part of a transaction:
                     *(ETrue) TBindTo should prepare for migration, but not commit until the sender
				      sends TCommitBindTo. Note that TCommitBindTo must not fail and it 
				      completes the handshake.
                     *(EFalse)(default) TBindTo should autocommit.
				      One example where the sender of TBindTo might be running a transaction is
				      where it has many dataclients it simultaneously binds. If some succeed
				      and some not, the operation must be rolled back for those who succeed.
				      TBindTo sender may roll back a transaction by cancelling the reuqest (TCancel).
	*/
    typedef Messages::TMessageSigNodeIdNumberDefault<ECFBindTo,  TCFDataClient::ERealmId> TBindTo;
    typedef Messages::TMessageSigVoid<ECFBindToComplete, TCFDataClient::ERealmId> TBindToComplete;
    typedef Messages::TMessageSigVoid<ECFCommitBindTo, TCFDataClient::ERealmId> TCommitBindTo;

	/**
	The message provisions all the necessary information to configure the recipient.
	This is typically sent by a control provider to its freshly created dataclient.
	The message should contain all the data structures required by the recipient
	to function. For instance, an Ethernet flow should be provision with the static
	IP address info (if applicable).

    @li Type:  Notification

	IMPORTANT: There is no response to TProvisionConfig. If the recipient finds out
	that the configuration received is in any way inadequate, it cannot flag this
	fact until it is requested to do something (e.g.: TCFDataClient::Start) it can't
	(due to inadequate configuration).

    
    @param iContainer A copy of an RMetaExtensionContainerC instance, which can be
            searched for type identified Meta::SMetaData objects.  
	*/
    typedef TMessageSigMetaExtContainer<ECFProvisionConfig, TCFDataClient::ERealmId> TProvisionConfig;
	};

template<TInt id, TInt32 realm>
class TCFMessageSigDataClientStatus : public Messages::TSigNumber, public Messages::TSignatureBase::TTypeToMessageId<id, realm>
/**
Message signature for message that can carry inform a control provider whether a data client is started or stopped
*/
    {
public:
	enum TDataClientStatus {
		EStopped = 0,
		EStarted = 1
	};

	TCFMessageSigDataClientStatus(TDataClientStatus aStatus)
	:	TSigNumber(Messages::TNodeSignal::TMessageId(id, realm), aStatus)
		{}
    };

/**
Message interface for control providers
Nodes wishing to implement this interface must implment all message protocols in the interface.

@publishedPartner
@released
*/
class TCFControlProvider : public TCFPeer
	{
  private:
  	enum
       	{
      	ECFNoBearer            = 1,
      	ECFBearer              = 2,
      	ECFDataClientGoneDown  = 3,
		ECFDataClientStatusChange = 4,
      	ECFIdle                = 5,
      	ECFActive              = 6
      	};
protected:

public:
    enum { ERealmId = 0x10285F40 }; //UID allocated on 4/6/08 from KUidNodeMessageRealms

	/**
	Request for a bearer. Dataclient flags the absence of its service provider
    and herby requests one from its control provider.

    @li Type:      Request for TBindTo
    @li Fallible:  Yes

	@param iFamilyBundle parameters which may assist the finding/creation of the bearer
	*/
    typedef TMessageSigFamilyBundleDefaultToNull<ECFNoBearer, TCFControlProvider::ERealmId> TNoBearer;
    typedef Messages::TMessageSigVoid<ECFBearer, TCFControlProvider::ERealmId> TBearer;

    /**
	Notification about the dataclient gone down. This is an equivallent of
	of an unsolicited TCFDataClient::TStopped (i.e.: the dataclient is notionally stopped).
	
    @li Type:      Notification

	@param       iValue1  reason for going down.
	@param       iValue2  undefined in the general scope, but particular dataclients can convey
                           the information meaningful to their control providers.
	*/
    typedef Messages::TMessageSigNumberNumberDefault<ECFDataClientGoneDown, TCFControlProvider::ERealmId> TDataClientGoneDown;

	/**
	Indicate to a control provider that the started state of its data client has changed.

	@li Type:      Notification

	@param       iStatus EStarted or EStopped
	*/
	typedef TCFMessageSigDataClientStatus<ECFDataClientStatusChange, TCFControlProvider::ERealmId> TDataClientStatusChange;

    /**
	Sent to the control provider to indicate idleness or (in other words) to
	inform the control provider that the sender can be shot at will. Sender
	doesn't have any control clients when sending TIdle and if it
	has clients, they're idle themselves.

    @li Type:      Notification
	*/
    typedef Messages::TMessageSigVoid<ECFIdle, TCFControlProvider::ERealmId> TIdle;

    /**
	Opposite to TIdle. Usually sent when gaining a first control client.

    @li Type:      Notification
	*/
    typedef Messages::TMessageSigVoid<ECFActive, TCFControlProvider::ERealmId> TActive;
	};

} //namespace esock

#endif
//SYMBIAN_NODEMESSAGES_DATACLIENT_H

