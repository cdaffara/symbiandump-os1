// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HCTLBCSP_H
#define HCTLBCSP_H

#include <bluetooth/hci/hctluartbase.h>
#include <bluetooth/hciframelogger.h>

class CHCTLBcspReceiver;
class CLinkEstablishment;
class CHCTLBcspSequencer;
class CHCTLBcspFrameQueue;
class CRxHctlBcspFrame;
class CHCTLBcspControllerManager;

/**BCSP HCI Framing constants for command frames */
static const TInt KHCTLCommandHeaderSize =0;
static const TInt KHCTLCommandTrailerSize=0;

/**BCSP HCI Framing constants for ACL data frames */
static const TInt KHCTLAclDataHeaderSize =0;
static const TInt KHCTLAclDataTrailerSize=0;

/**BCSP HCI Framing constants for Synchronous data frames */
static const TInt KHCTLSynchronousDataHeaderSize =0;
static const TInt KHCTLSynchronousDataTrailerSize=0;

_LIT(KIniFileName, "hctl_bcsp");

/**
	This is the class that implements the BCSP specific HCTL.	
*/
NONSHARABLE_CLASS(CHCTLBcsp) : public CHCTLUartBase
	{
public:	
	static CHCTLBcsp* NewL();

	void DoConfigL();
	void CanSend(TBool aCanSend);	// BCSP opens/closes all channels together

	void QueueReadForNextFrame();
	TInt HandleRx(const TDesC8& aReceivedFrame); // Handles slip decoding and BCSPFrame Decoding
	TInt PacketRouter();
	
	void TxAckMsg();
	void TxLinkMsg(const TDesC8 &aData);

	void FlogRx();

	TBool CheckIsAckPacket() const;
	void UnChoke();
	void Choke();
	void HandlePeerReset();
	void Reset();
	TInt WriteBcCmd(const TDesC8& aData); //For controller manager to send BCCMDs

	TBool Muzzled();
	void ResetMuzzled();	
	~CHCTLBcsp();
	
private:
	CHCTLBcsp();
	void ConstructL();

	TAny* Interface(TUid aUid);

	void ProcessACLData();
	void ProcessEventData();
	void ProcessBcCmdEventData();
	void ProcessSynchronousData();

	void DoWriteCommandL(const TDesC8& aData);
	void DoWriteACLDataL(const TDesC8& aData);
	void DoWriteBcCmdL(const TDesC8& aData); //For controller manager to send BCCMDs

	// From CHCTLUartBase
	void PortOpenedL();

	// From MHCTLInterface
	TInt MhiWriteCommand(const TDesC8& aData);
	TInt MhiWriteAclData(const TDesC8& aData);
	TInt MhiWriteSynchronousData(const TDesC8& aData);
	void MhiGetAclDataTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const;
	void MhiGetSynchronousDataTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const;
	void MhiGetCommandTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const;
	void MhiSetChannelObserver(MHCTLChannelObserver& aChannelObserver);
	void MhiSetDataObserver(MHCTLDataObserver& aDataObserver);
	void MhiSetEventObserver(MHCTLEventObserver& aEventObserver);
	void MhiSetControllerStateObserver(MControllerStateObserver& aControllerStateObserver);
	void MhiSetQdpPluginInterfaceFinder(MQdpPluginInterfaceFinder& aQdpPluginInterfaceFinder);

	// From MHardResetInitiator 
	void MhriStartHardReset();

private:
	TBool iMuzzled;
	TBool iCanSend;
	//When BCSP is choked, it means we hasn't established a link with the controller yet. 
	//Only UART link management packets are allowed to pass through. 
	TBool iChoked;
	CHCTLBcspReceiver* iReceiver;
	CHCTLBcspSequencer* iSequencer;
	CLinkEstablishment* iLinkEstablishment;
	MControllerStateObserver* iControllerStateObserver;
	MQdpPluginInterfaceFinder* iQdpPluginInterfaceFinder;

	CHCTLBcspFrameQueue* iFrameQueue;

	CRxHctlBcspFrame* iReceivedFrame;

	TInt iCommUnit;
	TUint8 iAck;

	CHCTLBcspControllerManager* iControllerMan;
	
	DECLARE_HCI_LOGGER
	};
#endif // HCTLBCSP_H
