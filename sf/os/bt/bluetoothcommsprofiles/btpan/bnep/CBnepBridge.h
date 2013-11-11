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

#ifndef __CBNEPBRIDGE_H
#define __CBNEPBRIDGE_H 

#include <e32base.h>
#include "MPanDevice.h"
#include "MPanDeviceOwner.h"
#include "BnepPacketNotifier.h"
#include <networking/pktdrv.h>
#include <networking/pktdrv_internal.h>


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include <elements/nm_node.h>
#include <comms-infras/ss_common.h>
#endif

class MPanConnectionNotify;
class MBnepChannelControl;
class RBnepFrame;
class TBTDevAddr;
class RInternalSocket;


const TUint KBnepLocalDeviceArrayId = 0;

const TUint KBnepPacketNotifyInterval=2000000;
const TUint KBnepPacketNotifySleepTicks=10;

/**
   Acts as a GN, routing packets between interfaces
   This occurs even if the local role is PANU, although in this case there are
   only two devices in its lists.
   @note The local device is treated differently - it is joined to the bridge
   at startup and has a special (known) place in the bridging array
*/
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
NONSHARABLE_CLASS(CBnepBridge) : public CBase, public Messages::ASimpleNodeIdBase, public MPanDeviceOwner
#else
NONSHARABLE_CLASS(CBnepBridge) : public CBase, public MPanDeviceOwner
#endif
    {
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
private:
	NONSHARABLE_CLASS(CPanLinkControl) : public CBase, public MPanLinkControl
		{
	public:
		static CPanLinkControl* NewL();
		~CPanLinkControl();
	private:
		void ConstructL();
		CPanLinkControl();

	private: 
		// MPanLinkControl methods
		virtual TAny* MplcbGetInterface(const TUid & aInterface);
		virtual TInt MplcRequestSniff(const TBTDevAddr &);
		virtual TInt MplcRequestActive(const TBTDevAddr &);

	private:	
		RSocketServ iSockServ;
		};
#endif

public:
    ~CBnepBridge();
    TInt AttachDevice (MPanDevice& aDevice);
    const TBTDevAddr& DefaultAddress () const;
    void LinkGoingDown (MPanDevice& aDevice);
	void RemoteDeviceReady ();

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    inline void SetPacketDriverOwner(CPacketDriverOwner& aPktDrvOwner);
    void AttemptNewBnepConnection(const Messages::TNodeId& aSender, const Messages::TSignatureBase& aMessage, TUint16 aActivity);
#else
    MBnepChannelControl& NewBnepConnectionL (RInternalSocket& aConnectedSocket, MPanConnectionNotify& aNotify);
#endif

    static CBnepBridge* NewL (MPanDevice& aLocalDevice);
    TInt Process (RBnepFrame& aFrame, const TBTDevAddr& aSourceIfAddr);
    void StopLinks ();
   	virtual void SetRetryConnect();
   	void DataCountUpdated();

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
protected:
    // From ESock::MCFNode, via ESock::ACFNodeIdBase
	void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aCFMessage);
#endif

private:
    CBnepBridge (MPanDevice& aLocalDevice);
	void ConstructL();
    void RemoveDevice (MPanDevice& aDevice);
    TInt GetVacantSlot (TUint& aSlotIndex);
	static TInt DataCounterCallBack(TAny*);
	void DataCounterNotify();

private: 
    TUint8 iLinkCount;
    TFixedArray< MPanDevice* , KMaxPanConnections  > iLinks;
    RLibrary iLib;
	CPeriodic* iDataCounterTimer;
	MBnepPacketNotifier* iBnepPacketNotifier;
	RArray<TBnepBytesTransferred> iDataTransferred;
	TUint iIdleCount;
	TBool iRetryConnect;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    // Points to the owner of the packet driver lib and is passed to
    // CBnepChannelControllers when we create them - DON'T DELETE IT
    CPacketDriverOwner* iPktDrvOwner;
	// MPanLinkControl implementation
	CPanLinkControl* iPanLinkControl;
#endif
    };

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
void CBnepBridge::SetPacketDriverOwner(CPacketDriverOwner& aPktDrvOwner)
    {
    iPktDrvOwner = &aPktDrvOwner;
    }

#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

#endif
// __CBNEPBRIDGE_H

