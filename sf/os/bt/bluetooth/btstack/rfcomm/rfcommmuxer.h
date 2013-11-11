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
//

#ifndef RFCOMMMUXER_H
#define RFCOMMMUXER_H

#include <es_prot.h>
#include <bttypes.h>
#include "rfcommsap.h"
#include "rfcommframe.h"
#include "rfcommflow.h"

class CRfcommSignalFrame;
class CRfcommMuxChannel;
class CRfcommFlowStrategyFactory;
class CMuxChannelStateFactory;
class TRfcommFlowStrategy;
/**
   Acts as the TS7.10 multiplexer.
   
   There is one of these per remote device, as only one RFCOMM
   multiplexer session can be run between two devices.  This class
   acts as an internal socket binding to a lower level SAP.  Thus we
   derive from MSocketNotify so that the SAP can directly notify us of events.

**/
NONSHARABLE_CLASS(CRfcommMuxer) : public CBase, public MSocketNotify
	{
friend class CRfcommProtocol;
friend class CRfcommMuxChannel;

public:
	// Create/destroy

	// Two flavours of factory - one takes an L2CAP connection which should 
	// be used by the Mux channel associated with this muxer. The other 
	// takes a CProtocolBase which it can use to create its own connection
	static CRfcommMuxer* NewL(CRfcommProtocol& aProt, CProtocolBase& aL2CAP,
							  CMuxChannelStateFactory& aFactory);

	static CRfcommMuxer* NewL(CRfcommProtocol& aProt, CServProviderBase* aSAP,
							  CMuxChannelStateFactory& aFactory);
	
	~CRfcommMuxer();
	// Interface for the Protocol to request services
	void Bind(TBTDevAddr& aAddr);
	void AddSAP(CRfcommSAP& aSAP);
	void DetachSAP(CRfcommSAP& aSAP);

	void GetInboundServerChannelsInUse(TFixedArray<TBool, KMaxRfcommServerChannel>& aChannelInUse);
	
	// Interface for the SAPs to request actions
	TUint8 MakeOutboundDLCI(TUint8 aServerChannel);
	TUint8 MakeInboundDLCI(TUint8 aServerChannel);
	TUint8 MakeServerChannel(TUint8 aDLCI);

	TInt SendSABM(CRfcommSAP& aSAP);
	TInt SendMSC(CRfcommSAP& aSAP, TUint8 aFlags);
	TInt SendMSCRsp(CRfcommSAP& aSAP, TUint8 aFlags);
	TInt SendRLS(CRfcommSAP& aSAP, TUint8 aStatus);
	TInt SendRLSRsp(CRfcommSAP& aSAP, TUint8 aStatus);
	TInt SendRPN(CRfcommSAP& aSAP, TBool aCommand, TUint8 aLength, 
				 const TRfcommRPNTransaction& aRPNTransaction);
	TInt SendUA(CRfcommSAP& aSAP);
	TInt SendUA(TUint8 aDLCI);
	TInt SendPN(CRfcommSAP& aSAP, const TRfcommPortParams& params);
	TInt SendPNResponse(CRfcommSAP& aSAP, const TRfcommPortParams& params);
	void SendDISC(CRfcommSAP& aSAP);
	void SendDISC(TUint8 aDLCI);
	void SendDM(TUint8 aDLCI);
	TInt SendFCon();
	TInt SendFCoff();
	void Donate(CRfcommSAP& aSAP, TUint8 aCredit); //TRY_CBFC
	TInt Write(CRfcommSAP& aSAP, TUint8 aCredit, const TDesC8& aData);
	void SetCanHandleData(CRfcommSAP& aSAP, TBool aCanReceive);
	TInt GetMaxDataSize() const;
	TInt Ioctl(TUint aLevel, TUint aName, TDes8* aOption);
	void CancelIoctl(TUint aLevel, TUint aName);
	TInt SetOption(TUint aLevel, TUint aName, const TDesC8 &aOption);
	TInt GetOption(TUint aLevel, TUint aName, TDes8 &aOption);
	
	// From MSocketNotify
	void NewData(TUint aCount);
	void CanSend();
	void ConnectComplete();
	void ConnectComplete(const TDesC8& aConnectData);
	void ConnectComplete(CServProviderBase& aSSP);
	void ConnectComplete(CServProviderBase& aSSP,const TDesC8& aConnectData);
	void CanClose(TDelete aDelete=EDelete);
	void CanClose(const TDesC8& aDisconnectData,TDelete aDelete=EDelete);
	void Error(TInt anError,TUint anOperationMask=EErrorAllOperations);
	void Disconnect(void);
	void Disconnect(TDesC8& aDisconnectData);
	void IoctlComplete(TDesC8 *aBuf);

	void NoBearer(const TDesC8& /*aConnectionInf*/) {};
	void Bearer(const TDesC8& /*aConnectionInf*/) {};

	// Other functions
	void PropagateIoctlCompletion(TInt aError, TUint aIoctlLevel, TUint aIoctlName, TDesC8* aBuf);
	void FrameResponseTimeout(CRfcommFrame* aFrame);
	
	void ClearResponseQueue(CRfcommSAP& aSAP);
	void ClearOutboundQueue(CRfcommSAP& aSAP);
	
	void MuxChannelUp();
	void MuxChannelDown();	// the MuxChannel has gone down - so notify SAPs
	void MuxChannelError(TBool aFatal, TInt anError);
	void MuxChannelClosed();	// the MuxChannel has gone down - no need for the muxer
	void CloseSAPs();
	const TBTDevAddr& RemoteBTAddr() const;
	CRfcommSAP* FindSAP(const TUint8 aDLCI);
//TRY_CBFC
	void DisallowCBFC();
	void AllowCBFC();
	TRfcommFlowStrategy* FlowStrategy();
	CRfcommFlowStrategyFactory::TFlowStrategies FlowType();
	TBool SetFlowType(CRfcommFlowStrategyFactory::TFlowStrategies aFlowType);



private:

	enum TInitiationDirection
		{
		KInitiationDirectionIncoming,	//	0
		KInitiationDirectionOutgoing	//	1
		};
		
	CRfcommMuxer(CRfcommProtocol& aProtocol, TInitiationDirection); 
	void ConstructL(CMuxChannelStateFactory& aFactory, CProtocolBase& aL2CAP);
	void ConstructL(CMuxChannelStateFactory& aFactory, CServProviderBase* aSAP);
	void CommonConstructL();

	// Processing incoming frames
	void ProcessFrame();
	void ParseCtrlMessageL();
	void ParsePNL(TInt aOffset, TInt aLen, TUint8& aDLCI, TRfcommPortParams& aParams);
	void ParseRPN(TInt aOffset, TInt aLen, TUint8& aDLCI, 
				  TRfcommRPNTransaction* aRPNTransactionPtr=NULL);
	void ProcessDataFrame(TUint8 aDLCI, TBool aPoll);
	TInt GetTypeFieldL(TInt& aType, TInt aPos);
	TInt GetLengthFieldL(TInt& aLen, TInt aPos);
	void HandleDISC(TUint8 aDLCI);
	void HandleSABM(TUint8 aDLCI);
	void HandleUA(TUint8 aDLCI);
	void HandleDM(TUint8 aDLCI);
	void HandlePN(TBool aCommand, TUint8 aDLCI, TRfcommPortParams& aParams);
	void HandleRPN(const TBool& aCommand, const TUint8& aDLCI, 
				   const TRfcommRPNTransaction* aRPNTransactionPtr=NULL);
	void HandleTest(TBool aCommand, TInt aOffset, TInt aLen);
	void HandleFCon(TBool aCommand);
	void HandleFCoff(TBool aCommand);
	void HandleMSC(TBool aCommand, TUint8 aDLCI, TUint8 aSignals);
	void HandleRLS(TBool aCommand, TUint8 aDLCI, TUint8 aStatus);
	
	// Packet utility functions
	TBool CheckFCS(TUint8 aFCS, TUint8 aCtrl);
	TUint8 DecodeDLCI(TUint8 aAddr);
	void DecodeLengthAndCredit(TBool aCBFC);
	TUint8 BuildAddr(TUint8 aDLCI, TBool aCommand);
	void EnqueFrame(CRfcommFrame* aFrm);
	void TryToSend();
	TInt CtrlFrameResponse(TUint8 aDLCI);
	void MuxCtrlResponse(TUint8 aType);
	void MuxCtrlResponseDM(TUint8 aDLCI);
	
	// Flow control
	void SetSendBlocked(CRfcommSAP& aSAP, TBool aState);
	void SignalSAPsCanSend();
	TBool ClearToSend() const
			{return iCanSend;}
	void SetCanSend(TBool aState)   //< True if OK to send
			{iCanSend=aState;}
	TBool L2CAPBlocked() const
			{return iL2CAPSendBlocked;}
	void L2CAPBlocked(TBool aState)
			{iL2CAPSendBlocked=aState;}

	// Constructing packets
	CRfcommMuxCtrlFrame* NewSignalFrame(TInt aCommandLength, TBool aCommand, CRfcommSAP* aSAP =NULL);
	CRfcommCtrlFrame* NewFrame(CRfcommSAP* aSAP =NULL);
	CRfcommDataFrame* NewDataFrame(TUint8 aDLCI, TInt aLen, TUint8 aCredit, CRfcommSAP* aSAP=NULL);

	// Sending packets
	TInt TransmitSABM(TUint8 aDLCI, CRfcommSAP* aSAP =NULL);
	TInt TransmitUA(TUint8 aDLCI, CRfcommSAP* aSAP =NULL);
	TInt TransmitDISC(TUint8 aDLCI, CRfcommSAP* aSAP =NULL);
	TInt TransmitDM(TUint8 aDLCI, TBool aPFBit, CRfcommSAP* aSAP =NULL);	
	TInt TransmitPN(TUint8 aDLCI, TBool aCommand, const TRfcommPortParams& aParams,
					CRfcommSAP* aSAP =NULL);
	TInt TransmitRPN(TUint8 aDLCI, TBool aCommand, TUint8 aLen,
					 const TRfcommRPNTransaction& aRPNTransaction, CRfcommSAP* aSAP = NULL);
	TInt TransmitNSC(TBool aCommand, TUint8 aType);
	TInt TransmitFCon(TBool aCommand, CRfcommSAP* aSAP =NULL);
	TInt TransmitFCoff(TBool aCommand, CRfcommSAP* aSAP =NULL);
	TInt TransmitMSC(TUint8 aDLCI, TBool aCommand, TUint8 aSignals,
					  CRfcommSAP* aSAP =NULL);
	TInt TransmitTest(TBool aCommand, const TDesC8& aData, CRfcommSAP* aSAP =NULL);
	TInt TransmitRLS(TBool aCommand, TUint8 aDLCI, TUint8 aStatus,
					 CRfcommSAP* aSAP =NULL);

	
	// Callbacks & timers
	static TInt IdleTimerExpired(TAny* aMux);
	static TInt TryToSendCallbackStatic(TAny* aMux);
	void TryToSendCallback();
	void CheckForIdle(TBool aClosing = EFalse);
	void QueIdleTimer();
	void DequeIdleTimer();

	// Utility
	void DeleteQueuedFrames();
	CRfcommSAP* FindConnectedOrListeningSAP(const TUint8 aDLCI);
	TUint8 MakeDLCI(TUint8 aServerChannel, TUint8 aDirectionBit);

	// Reception of data from L2CAP
	TBool CanProcessNewData() const;
	void SetNoFreeSpace(TInt aDLCI, TBool aNoSpace);

	// Debug
#ifdef __FLOG_ACTIVE
	void ExplainOutgoingFrame(CRfcommFrame* aFrm, CRfcommMuxer* aMux );
	void LogMuxCommand(CRfcommSAP* aSAP, CRfcommMuxer* aMux, TUint8 aCommand);
#endif
	
	// Data items
	TDblQue<CRfcommSAP> iSAPs;  //< SAPs using this Mux
	TDblQue<CRfcommFrame> iOutboundQ; //< Frames to send
	TDblQue<CRfcommFrame> iResponseQ; //< Frames waiting for a response
	TInt iOutboundQLength;
	CRfcommProtocol& iProtocol;
	CServProviderBase* iBoundSAP;
	TInt iL2CAPMTU;
	TBTDevAddr  iRemoteAddr;
	const TInt iDirection; //< 1 if we are initiating, else 0

	TBool iClosing;  //< True if we are closing down our channel
	TDeltaTimerEntry  iIdleTimerEntry; //< Disconnection idle timer
	TBool  iIdleTimerQueued;
	TInt iMuxIdleTimeout;

	CAsyncCallBack* iSendCallback;
	HBufC8*  iNextPacket;   // Next packet coming up from L2CAP

	TInt iCurrentHeaderLength; //< Length of the current frame's header
	TInt iCurrentDataLength;  //< Length of the current frame's data

	// Ioctls
	TUint  iIoctlLevel;
	TUint  iIoctlName;
	
	// Flow control
	TBool  iCanSend;  //< False if remote has asked us not to send (FCoff)
	TDblQue<CRfcommSAP> iBlockedSAPs;  //< SAPs that are blocked
	// Control over reading from L2CAP
	TInt     iPacketsWaiting;   //< Number of packets L2CAP has for us
	TUint32  iSAPNoFreeSpaceMask[2];  //< Mask showing which SAP have no space left
	TInt	 iDataFramesSent;
	// Mux channel
	CRfcommMuxChannel* iMuxChannel;
	TBool  iL2CAPSendBlocked;  // Is L2CAP ok to send data?
	//TRY_CBFC
	TBool iCBFCDisallowed; // ensure flow strategy set to Mux type when object is instantiated
	TBool iTriedCBFC;	// have we tried to set CBFC ?
	TInt iCurrentCredit; // a temporary store for the received frame credit
	TInt iInitialTxCredit;	// used as default credit for new SAPs
	TRfcommFlowStrategy* iFlowStrategy;
	
	// Go on a Q
	TDblQueLink iLink;
	};


#endif
