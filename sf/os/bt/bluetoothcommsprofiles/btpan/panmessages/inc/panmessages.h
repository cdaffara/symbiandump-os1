// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Custom messages between BNEP and PAN
// 
//

/**
 @file
 @internalComponent
*/

#ifndef PANMESSAGES_H
#define PANMESSAGES_H

#include <e32base.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/eintsock.h>

#include "panroles.h"
#include "pancommon.h"

const TInt KPanMessagesImplementationUid = 0x1028376D;


// PAN Custom Message Signatures
DECLARE_MESSAGE_SIG_1(SigInternalSocket, RInternalSocket*, Socket)
DECLARE_MESSAGE_SIG_1(SigBool, TBool, Value)
DECLARE_MESSAGE_SIG_1(SigSetupConnectionResponse, TBnepSetupConnectionResponseMessage, SetupResponse)
DECLARE_MESSAGE_SIG_2(SigUuidUuid, TUUID, RequestedLocalRole, TUUID, RequestedRemoteRole)
DECLARE_MESSAGE_SIG_2(SigPanRolePanRole, TBluetoothPanRole, RequestedLocalRole, TBluetoothPanRole, RequestedRemoteRole)

/*
@internalComponent 
*/
NONSHARABLE_CLASS(TPanMessage)
	{
private:
    enum
	    {
        // 0 is always TCFNodeSignalBase::TMessageId::KCFNullMessageId
    	ECreateChannelController			    = 1,
    	ESetRetryConnect                        = 2,
    	ERoleRequestFromRemoteDevice            = 3,
    	ERoleResponseFromRemoteDevice           = 4,
    	ERemoteDeviceDisconnect                 = 5,
    	ERoleRequestFromLocalDevice             = 6,
    	ERoleResponseFromLocalDevice            = 7,
    	ECloseChannelController                 = 8,
    	ESetUplinkAccessAllowedForBnepLink      = 9,
    	EChannelControllerCreated               = 10,
    	ESetRetryConnectComplete                = 11,
    	EChannelControllerClosed                = 12
    	};

public:
    /** Register the messages with the comms-transport */
	IMPORT_C static void RegisterL();

    /** De-register the messages with the comms-transport */
	IMPORT_C static void DeRegister();

public:
    enum { ERealmId = 0x1028376C }; // Same as DLL Uid
    
    /**
    Activities that help identify what a TLastMessageAck is actually
    ACKing.
    */
    enum TPanActivity
        {
        EActivityNull                       = MeshMachine::KActivityNull,
        EActivityCreateChannelControllerForIncoming,    // 1
        EActivityCreateChannelControllerForOutgoing,    // 2
        };
    
    // MBnepConnectionManager replacement messages   
    typedef TMessageSigInternalSocket<ECreateChannelController, ERealmId> TCreateChannelController;
    typedef Messages::TMessageSigVoid<ESetRetryConnect, ERealmId> TSetRetryConnect;


    // MPanConnectionNotify replacement messages (Bnep -> Pan)
    typedef TMessageSigUuidUuid<ERoleRequestFromRemoteDevice, ERealmId> TRoleRequestFromRemoteDevice;
    typedef TMessageSigSetupConnectionResponse<ERoleResponseFromRemoteDevice, ERealmId> TRoleResponseFromRemoteDevice;
    typedef Messages::TMessageSigNumber<ERemoteDeviceDisconnect, ERealmId> TRemoteDeviceDisconnect;


    // MBnepChannelControl replacement messages (Pan -> Bnep)
    typedef TMessageSigPanRolePanRole<ERoleRequestFromLocalDevice, ERealmId> TRoleRequestFromLocalDevice;
    typedef TMessageSigSetupConnectionResponse<ERoleResponseFromLocalDevice, ERealmId> TRoleResponseFromLocalDevice;
    typedef Messages::TMessageSigVoid<ECloseChannelController, ERealmId> TCloseChannelController;
    typedef TMessageSigBool<ESetUplinkAccessAllowedForBnepLink, ERealmId> TSetUplinkAccessAllowedForBnepLink;
    
    // Messages not corresponding to legacy interfaces
    typedef Messages::TMessageSigNumber<EChannelControllerCreated, ERealmId> TChannelControllerCreated;
    typedef Messages::TMessageSigNumber<ESetRetryConnectComplete, ERealmId> TSetRetryConnectComplete;
	};
	
#endif
// PANMESSAGES_H
