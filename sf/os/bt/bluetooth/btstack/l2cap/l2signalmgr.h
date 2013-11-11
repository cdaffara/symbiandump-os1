// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the L2CAP Mux
// 
//

#ifndef L2SIGNALMGR_H
#define L2SIGNALMGR_H

#include <e32base.h>	// CBase
#include <es_prot.h>	// MSocketNotify
#include <bt_sock.h>

#include "notification.h"
#include "L2CapPDU.h"

class CL2CAPMuxController;
class CL2CapSAPSignalHandler;
class CL2CapLinkSignalHandler;

class CL2CapBasicDataController;
class MEchoResponseHandler;

class TBTDevAddr;

static const TUint KRemoteDeviceProcessRequestDuration = 1; // Time in seconds
NONSHARABLE_CLASS(CL2CAPMux) : 	public CBase,
                  				private MSocketNotify,
                  				public MPhysicalLinkObserver,
                  				public MPduOwner
	{
public:
	static CL2CAPMux* NewL(CL2CAPMuxController& aMuxController, const TBTDevAddr& aAddr);
	~CL2CAPMux();

	// Some notifications passed up from the linkmgr
	void IoctlComplete(TInt aErr, TUint level,TUint name,TDesC8* aBuf);
	void L2CapEntityConfigUpdated();

	// From MSocketNotify
	void NewData(TUint aCount);
	void CanSend();
	void ConnectComplete();
	void ConnectComplete(const TDesC8& aConnectData);
    void ConnectComplete(CServProviderBase& aSSP);
	void ConnectComplete(CServProviderBase& aSSP,const TDesC8& aConnectData);
	void CanClose(TDelete aDelete = EDelete);
	void CanClose(const TDesC8& aDisconnectData, TDelete aDelete = EDelete);
	void Error(TInt aError,TUint aOperationMask = EErrorAllOperations);
	void Disconnect();
	void Disconnect(TDesC8& aDisconnectData);

	void IoctlComplete(TDesC8 *aBuf);
	void NoBearer(const TDesC8& aConnectionInfo);
	void Bearer(const TDesC8& aConnectionInfo);

	// MPduOwner
	virtual void HandlePduSendComplete(HL2CapPDU& aPdu);
	virtual void HandlePduSendError(HL2CapPDU& aPdu);

	// Signal packet timer interface

	// Getters & Setters
	const TBTDevAddr& RemoteBTAddr() const;
	TUint8 NextSigId();
	CL2CAPMuxController& MuxController() const;

	TInt SetOption(TUint level,TUint name,const TDesC8 &aOption);
	TInt GetOption(TUint level,TUint name,TDes8& aOption) const;

	void RegisterSAPSignalHandler(CL2CapSAPSignalHandler& aSAPSigHandler);
	void DetachFromMux(CL2CapSAPSignalHandler& aSAPSigHandler);

	void CompleteACLConnect(CServProviderBase* aSAP);

	void RegisterDataPDUHandler(CL2CapBasicDataController& aPDUDataHandler);
	void DataChannelRemoved(CL2CapBasicDataController* aDataController);
	void MuxerPriorityUpdate(TInt aPriority);
	TInt GetSumMuxerChannelPriorities();
	void ChannelPriorityUpdated(CL2CapBasicDataController& aDataController);

	void PDUAvailable();

	void ProcessFlushTimerExpiry();
	TUint16 SigMTU() const;

	void ErrorAllSignalHandlers(TInt aError);

	TInt SendEchoRequest(const TDes8* aData, CL2CapSAPSignalHandler& aEchoResponseHandler);
	void EchoResponseReceived(const TDesC8* aData, MEchoResponseHandler& aEchoResponseHandler);

	TInt GetFreeCID(TL2CAPPort& aReturnPort);
	TInt GetACLMTU() const;

	CL2CapSAPSignalHandler* GetSignalHandlerWithRemoteCID(TL2CAPPort aRemoteCID);
	
	//From MPhysicalLinkObserver
	void PhysicalLinkChange(const TBTBasebandEventNotification& aEvent, CPhysicalLink& aPhysicalLink);
	TPhysicalLinkObserverQLink& ObserverQLink() {return iQlink;};
	
	TUint8 AdjustRTXTimerForSniffMode(TUint8 aBaseRTXTimerDuration) const;
	
private:
	CL2CAPMux(CL2CAPMuxController&, const TBTDevAddr& aAddr);
	void ConstructL();

	void PacketComplete();
	TBool HardwareFlushAllowed();

	TBool Idle();
	TInt MuxIdled();
	void CancelIdle();
	void StartIdleTimer();
	static TInt IdleTimerExpired(TAny *aMuxer);
	static TInt AsyncCallBackForIdleMux(TAny *aMuxer);
	static TInt AsyncCallBackForTryToSend(TAny *aMuxer);
	void NewL2CAPData(RMBufChain& aData, TUint8 aFlag);

	void PriorityAddDataController(CL2CapBasicDataController& aDataController);
	void TryToSend();
	
public:
	TDblQueLink iMuxControllerLink;

private:
	// Link info
	enum TLinkState
		{
		ELinkNone = 0,
		ELinkPending,
		EConnected,
		ELinkInbound,
		};


	CL2CAPMuxController& iMuxController; //< Our Mux Controller
	TLinkState		 iLinkState;	//< The state of the baseband link
	TBTDevAddr       iRemote;		//< The device we're talking to

	TDeltaTimerEntry iIdleTimerEntry;  //< Disconnection idle timer
	TBool			 iIdleTimerActive; //< Is Idle timer running.

	CAsyncCallBack*	 iAsyncCallBackForTryToSend;//< Async. stuff: callback SendCallBack to send (medium priority) or, finally, TerminateCallBack to kill (high priority).
	CAsyncCallBack*	 iAsyncCallBackForIdleMux;//< For deleting idle muxers without a delay

	// the bound SAP
	CServProviderBase*	iBoundSAP;

	//link level stuff
	TInt iACLMTU;
	TInt iACLMRU;
	TUint8 iSigId;

	TDblQue<CL2CapBasicDataController> iDataHandlers;
	TDblQue<CL2CapSAPSignalHandler> iSapSignalHandlers;
	CL2CapLinkSignalHandler* iBoundLinkSignalHandler;
		
	TDblQue<HL2CapPDU> iPDUsWaitingResendQ;
	TDblQue<HL2CapPDU> iPDUsWaitingSend;

	HFragmentedPDUSender* iFragmentSender;
	RMBufChain iIncomingPDU;
	
	TInt iTryToSendPriority;
	TBool iUpdateTryToSendPriority;
	
	TL2CAPPort iNextCID;
	TUint16 iNumberOfDataChannels;
	TBasebandTime iSniffInterval;

	TPhysicalLinkObserverQLink iQlink;
	};

inline const TBTDevAddr& CL2CAPMux::RemoteBTAddr() const
	{
	return iRemote;
	}

inline CL2CAPMuxController& CL2CAPMux::MuxController() const
	{
	return iMuxController;
	}
	
#endif
