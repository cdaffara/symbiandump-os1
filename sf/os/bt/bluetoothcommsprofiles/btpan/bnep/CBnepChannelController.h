// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef __CBNEPCHANNELCONTROLLER_H
#define __CBNEPCHANNELCONTROLLER_H 

#include <e32base.h>
#include <e32def.h>
#include "MLinkManager.h"
#include "BnepTypes.h"
#include "RBnepNotUnderstoodResponseControl.h"
#include "CMultiAddrFilterTable.h"
#include "CNetTypeFilterTable.h"
#include "panroles.h"
#include "pancommon.h"

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include <elements/nm_node.h>
#include <comms-infras/ss_common.h>
#include <comms-infras/ss_nodeinterfaces.h>
#endif

class CBnepLink;
class RBnepSetupConnectionResponseControl;
class RBnepSetupConnectionRequestControl;
class CBnepBridge;
class RBnepFilterMultiAddrResponseControl;
class RBnepFilterMultiAddrSetRequestControl;
class RBnepFilterNetTypeResponseControl;
class RBnepFilterNetTypeSetRequestControl;
class RInternalSocket;


/**
Provides a controller to a BNEP channel to a single remote device.
@internalComponent
*/
NONSHARABLE_CLASS(CBnepChannelController) : public CBase,
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    public Messages::ASimpleNodeIdBase,
#endif
    public MBnepChannelControl, 
    public MLinkManager 
    {
public:
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    static CBnepChannelController* NewL (CBnepBridge& aBridge, RInternalSocket& aConnectedSocket, const Messages::TNodeId& aNotify, CPacketDriverOwner& aPktDrvOwner);
#else
    static CBnepChannelController* NewL (CBnepBridge& aBridge, RInternalSocket& aConnectedSocket, MPanConnectionNotify& aNotify);
#endif

    ~CBnepChannelController();

    void BnepRoleRequestFromLocalDevice (TBluetoothPanRole aRequestedLocalRole, TBluetoothPanRole aRequestedRemoteRole);
    void BnepRoleResponseFromLocalDevice (TBnepSetupConnectionResponseMessage aRoleResponseCode);
    void Close ();
    void Execute (RBnepControl* aControl);
    void RemoteDeviceDisconnect (TInt aError);
	void SetUplinkAccessAllowedForBnepLink(TBool aAllowed);

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
protected:
    // From ESock::MCFNode, via ESock::ACFNodeIdBase
	void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aCFMessage);
#endif
	
private:
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CBnepChannelController (const Messages::TNodeId& aNotify, CPacketDriverOwner& aPktDrvOwner);
#else
    CBnepChannelController (MPanConnectionNotify& aNotify);
#endif
    void ConstructL (CBnepBridge& aBridge, RInternalSocket& aConnectedSocket);

    void Handle (RBnepSetupConnectionRequestControl* aSetupRequest);
    void Handle (RBnepSetupConnectionResponseControl* aSetupResponse);
    void Handle (RBnepFilterNetTypeSetRequestControl* aFilterNetTypeSetRequest);
    void Handle (RBnepFilterNetTypeResponseControl* aFilterNetTypeResponse);
    void Handle (RBnepFilterMultiAddrSetRequestControl* aFilterMultiAddrSetRequest);
    void Handle (RBnepFilterMultiAddrResponseControl* aFilterMultiAddrResponse);
    void HandleUnknownCommandPacket (RBnepControl* aControl);

	void CreateAndQueueConnectionSetupRequestL(TBluetoothPanRole aRequestedLocalRole, TBluetoothPanRole aRequestedRemoteRole);
	void CreateAndQueueConnectionSetupResponseL(TBnepSetupConnectionResponseMessage aRoleResponseCode);
	void CreateAndQueueNotUnderstoodResponseL(TUint8 aControlType);
	void CreateAndQueueFilterNetTypeResponseL(TFilterNetTypeResponse aResponse);
	void CreateAndQueueFilterMultiAddrResponseL(TFilterMultiAddrResponse aResponse);

private: 
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    Messages::TNodeId iPanConnectionNotify;
	CPacketDriverOwner& iPktDrvOwner;
#else
    MPanConnectionNotify& iNotify;
#endif
	
	static TInt ProceedCb(TAny* aPtr);
	void Proceed();

private:
    CBnepLink* 			iLink;
	CAsyncCallBack*		iProceedCallBack;
    };

#endif

