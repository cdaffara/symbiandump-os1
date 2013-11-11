// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef RFCOMMSAP_H
#define RFCOMMSAP_H

#include <es_prot.h>
#include <bttypes.h>
#include <bt_sock.h>
#include "BtSap.h"
#include "rfcommtypes.h"
#include "secman.h"

class CRfcommMuxer;
class CRfcommProtocol;
class TRfcommState;
class CAsyncErrorKicker;

/**
   The socket service access point.
   This class forms the major interaction point with sockets.
**/
NONSHARABLE_CLASS(CRfcommSAP) : public CBluetoothSAP
	{
friend class CRfcommProtocol;
friend class CRfcommMuxer;

	// All states are friends
friend class TRfcommState;
friend class TRfcommStateError;
friend class TRfcommStateDefault;
friend class TRfcommStateClosed;
friend class TRfcommStateWaitForMux;
friend class TRfcommStateWaitForPNResp;
friend class TRfcommStateWaitForUA;
friend class TRfcommStateListening;
friend class TRfcommStateCloned;
friend class TRfcommStateWaitForSABM;
friend class TRfcommStateIncomingSecurityCheck;
friend class TRfcommStateOutgoingSecurityCheck;
friend class TRfcommStateWaitForStart;
friend class TRfcommStateCloseOnStart;
friend class TRfcommStateConnecting;
friend class TRfcommStateOpen;
friend class TRfcommStateDisconnect;
friend class TRfcommStateDisconnecting;
public:
	// Creation
	static CRfcommSAP* NewL(CRfcommProtocol& aProtocol);
	~CRfcommSAP();
	
	// CServProviderBase functions
	virtual void Start();
	virtual void LocalName(TSockAddr& aAddr) const;
	virtual TInt SetLocalName(TSockAddr& aAddr);
	virtual void RemName(TSockAddr& aAddr) const;
	virtual TInt SetRemName(TSockAddr& aAddr);
	virtual TInt GetOption(TUint aLevel,TUint aName,TDes8& aOption) const;
	virtual TInt SAPSetOption(TUint aLevel,TUint aName,const TDesC8 &aOption);
	virtual void Ioctl(TUint aLevel,TUint aName,TDes8* aOption);
	virtual void CancelIoctl(TUint aLevel,TUint aName);

#ifndef __WINS__
#ifndef __GCC32__
	//unhide the other overloads
	using CServProviderBase::Write;
	using CServProviderBase::GetData;
#endif
#endif

	virtual TUint Write(const TDesC8& aDesc,TUint aOptions, TSockAddr* aAddr=NULL);
	virtual void GetData(TDes8& aDesc,TUint aOptions,TSockAddr* aAddr=NULL);

	virtual void ActiveOpen();
	virtual void ActiveOpen(const TDesC8& aConnectionData);
	virtual TInt PassiveOpen(TUint aQueSize);
	virtual TInt PassiveOpen(TUint aQueSize,const TDesC8& aConnectionData);
	virtual void Shutdown(TCloseType aOption);
	virtual void Shutdown(TCloseType aOption,const TDesC8& aDisconnectionData);
	virtual void AutoBind();
	// End CServProviderBase functions
	
	// Notifications from the mux
	void LinkDown();
	void DISC();
	void UA();
	void DM();
	void RPN(const TRfcommRPNTransaction* aRPNTransactionPtr, CRfcommMuxer& aMuxer, TUint8 aDLCI);
	void RPNRsp(const TRfcommRPNTransaction& aRPNTransaction);
	void PN(TRfcommPortParams& aParams, CRfcommMuxer& aMuxer, TUint8 aDLCI);
	void PNResp(TRfcommPortParams& aParams);
	void MSC(TUint8 aSignals);
	void RLS(TUint8 aStatus);
	void Data(const TDesC8& aData);
	void CanSend();
	void IoctlComplete(TInt aErr, TUint aLevel, TUint aName, TDesC8* aBuf);

	void SABM(CRfcommMuxer& aMuxer, TUint8 aDLCI);

	// Notifications from the protocol
	void MuxUp();
	
	// Notifications from sent frames awaiting a response
	TBool HandleFrameResponseTimeout();

    // Other functions
	enum TErrorTypes
		{
		EErrorOperation,   // The operation in progress failed
		EErrorGeneral,      // This screws up all operations
		EErrorFatal         // Very bad ju-ju
		};
	void Error(TInt aErrorCode, TErrorTypes aType);
	TBool ServerChannelValid() const;
	TRfcommChannel ServerChannel() const;
	TUint8 DLCI() const;
	TUint16 MaximumMTU() const;
	TBool IsCloned();
	
	TBool CTS() const //< Are we clear to send?
			{return iClearToSend;}
	void CTS(TBool aState)
			{iClearToSend=aState;}
	TBool CTR() const  //< Are we clear to receive?
			{return iClearToRecv;}
	void CTR(TBool aState)
			{iClearToRecv=aState;}
	enum TSendBlockedBy
		{
		ESendOK = 0,
		EBlockedByMux = 0x01,
		EBlockedByRemote = 0x02,
		EBlockedByBoth = 0x03
		};
	
	TInt SendBlocked() const  //< Is send currently blocked
			{return iSendBlocked;}
	void SendBlocked(TInt aState)
			{iSendBlocked=aState;}

	TRfcommRemotePortParams LocalPortParams()
		{return iLocalPortParams;}
	void SetLocalPortParams(TRfcommRemotePortParams aPortParams)
		{iLocalPortParams = aPortParams;}

	TInt LowTideMark()
		{return iLowTideMark;};
	void SetLowTideMark(TInt aLowTideMark)
		{iLowTideMark = aLowTideMark;};
	TInt HighTideMark()
		{return iHighTideMark;};
	void SetHighTideMark(TInt aHighTideMark)
		{iHighTideMark = aHighTideMark;};
	TUint8 Signals()
		{return iSignals;};
	void SetSignals(TUint8 aSignals)
		{iSignals = aSignals;};

	CRfcommMuxer* Mux();

	CCirBuffer& DataBuffer();
	TInt UnusedBufferSpace() const;

	TUint16 NegotiatedMTU() const; //< Returns the negotiated MTU
	TUint16 OptimalMTUForSending() const; //< Returns the optimal MTU for sending
	TUint16 UsableMTU(TUint8 aCredit) const; //< Returns the actual maximum information field size available (which with CBFC will be one less than the negotiated MTU)

	TBool ListeningTo(const TBTSockAddr& aBTSockAddr) const;
	inline const CRfcommSAP& ListeningSAP() const {return *iParentSAP;};
//TRY_CBFC
	void DisallowCBFC();
	void AllowCBFC();
	TUint8 FreeCredit();
	TUint8 FreeCreditCalculation() const;
	TInt ProxyForRemoteCredit() const;
	void SetProxyForRemoteCredit(TInt aCredit);
	void ProxyForRemoteCreditDecrement();
	void ProxyForRemoteCreditAddCredit(TUint8 aCredit);
	TInt LocalCredit() const;
	void SetInitialLocalCredit(TInt aCredit);
	void LocalCreditDecrement();
	void LocalCreditAddCredit(TUint8 aCredit);
	TBool BoundTo(const TBTSockAddr& aBTSockAddr) const;
private:
	// Creation
	CRfcommSAP(CRfcommProtocol& aProt);
	void ConstructL();

	TInt PassiveAutoBind(TRfcommSockAddr& aAddress);

	//	Cloning support
	//	(NB. Only a SAP may clone itself)
	CRfcommSAP* CloneMe();

	//	Functions called on a Parent SAP by a cloned child
	void ChildConnected(CRfcommSAP& aRfcommSAP);
	void ChildConnectFailed(CRfcommSAP& aRfcommSAP);
	void ChildStarted(CRfcommSAP& aRfcommSAP);

	//	From MAccessRequestResponseHandler
	void AccessRequestComplete(TInt aResult);

	//	Functions called on cloned child SAPs by their parent
	void ParentClosed();

	//
	void StopListening();
	TBool IsValidServerChannel(TInt aChan) const;
	static TInt NotifyNewDataCallback(TAny* aSAP);

	// Data incoming
	CCirBuffer iDataBuffer;
	TInt       iHighTideMark;
	TInt       iLowTideMark;

	CAsyncCallBack*		iNotifyNewDataCallback;
	CAsyncErrorKicker*	iErrorKicker;			//	Used to asynchronously error a SAP

	TInt		iNewDataToNotify;

	TUint8     iDLCI;
	TUint8     iServerChannel;
	
	TBool      iClearToSend;   //< Set to true if we can send
	TBool      iClearToRecv;  //< True if we haven't switched off the remote
	TBool	   iReceivedMSC;  //< For a new connection, we should get an MSC before any user data
	TInt       iSendBlocked;  //< Reason sending by ESOCK is blocked, if at all
	//TRY_CBFC
	TInt	   iLocalCredit;	// in units of the MTU
	TInt	   iProxyForRemoteCredit;		// in units of the MTU - is it symmetric ?
	#ifdef _DEBUG
	TInt	   iLocalCreditsSupplied; //total credits received in incoming frames
	TInt	   iLocalCreditsUsed; //total credits used in sending data packets
	TInt	   iProxyForRemoteCreditsSupplied; //total credits sent in outcoming frames
	TInt	   iProxyForRemoteCreditsUsed; //total credits used in receiving data packets
	#endif
	TUint8     iSignals;  //< Current signals that we send for MSC cmd
	TUint8	   iRLSstatus;	//< Current status that we send for RLS cmd
	TUint8	   iRemoteModemStatus; //< Remote end's signals from last MSC received

	TBool iClosePending;
	// Links
	CRfcommProtocol& iProtocol;
	CRfcommMuxer*  iMux;
	TRfcommState*  iState;
	TDblQueLink    iLink;	//	Used by Muxer to keep track of the SAPs it is linked to
	TDblQueLink    iListeningLink;	//	Used by the protocol to keep track of listening SAPs
	TDblQueLink	   iBoundLink;		//	Used by the protocol to keep track of bound SAPs
	TUint16			iNegotiatedMTU;	//	Maximum data size for this SAP
	TUint16			iOptimalMTUForSending; // Optimal MTU for sending provided by L2CAP
	TUint16			iUserDefinedMTU;	//	User defined MTU (0=No restriction)
	//	Members used by listening SAP
	TInt			iMaxClonesWaitingForStart;
	TInt			iClonesWaitingForStart; // SAPs that were listening, have connected but haven't been Accept()ed - owned by ESOCK
	RPointerArray<CRfcommSAP> iClonedChildren; // SAPs that are listening but not connected - owned by parent SAP


	//	Member used by cloned SAP
	CRfcommSAP*		iParentSAP;

	TBool iCloned; // Indicates if this is a cloned SAP

	TRfcommRemotePortParams iLocalPortParams;	// Local Port Params for RPN commands/responses

	TUint8			iDisconnectMSCSignal;	// signal state that will cause a disconnect error

	// Used only when CBFC is employed.
	// Defaults to EFalse.
	// When EFalse, if the remote overflows us, we disconnect.
	// When ETrue, if the remote overflows us, we just drop the data and 
	// maintain the connection.
	// See option KRFCOMMForgiveCBFCOverflow. The default behaviour is for to 
	// be used by clients which require reliable transfer, e.g. OBEX. The 
	// 'forgiving' behaviour is to be used by clients which aren't worried 
	// about reliability at our level and would rather the connection stayed 
	// up. 
	TBool iForgiveCBFCOverflow;	
	};

#endif
