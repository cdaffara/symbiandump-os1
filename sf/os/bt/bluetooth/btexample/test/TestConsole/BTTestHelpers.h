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

#ifndef BTTESTHELPERS_H_
#define BTTESTHELPERS_H_

#include <e32std.h>
#include <e32cons.h>
#include <e32base.h>
#include <e32test.h>
#include <f32file.h>

#include <bt_sock.h>


// L2CAP DEBUG P&S
const TUint KUidBluetoothDebugPubSubKeyBase = 0x10272368; //	Range of 550 values registered


// L2CAP debug P&S key values.
const TUint KPropertyKeyBluetoothL2CapDebugBase = (KUidBluetoothDebugPubSubKeyBase + 0);

const TUint KPropertyKeyBluetoothBasePDUAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x00);
const TUint KPropertyKeyBluetoothBasePDUDel		= (KUidBluetoothDebugPubSubKeyBase + 0x01);
const TUint KPropertyKeyBluetoothBasePDUPeak	= (KUidBluetoothDebugPubSubKeyBase + 0x02);

const TUint KPropertyKeyBluetoothBFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x10);
const TUint KPropertyKeyBluetoothBFrameDel		= (KUidBluetoothDebugPubSubKeyBase + 0x11);
const TUint KPropertyKeyBluetoothBFramePeak		= (KUidBluetoothDebugPubSubKeyBase + 0x12);
const TUint KPropertyKeyBluetoothIFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x20);
const TUint KPropertyKeyBluetoothIFrameDel		= (KUidBluetoothDebugPubSubKeyBase + 0x21);
const TUint KPropertyKeyBluetoothIFramePeak		= (KUidBluetoothDebugPubSubKeyBase + 0x22);
const TUint KPropertyKeyBluetoothSFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x30);
const TUint KPropertyKeyBluetoothSFrameDel		= (KUidBluetoothDebugPubSubKeyBase + 0x31);
const TUint KPropertyKeyBluetoothSFramePeak		= (KUidBluetoothDebugPubSubKeyBase + 0x32);
const TUint KPropertyKeyBluetoothCFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x40);
const TUint KPropertyKeyBluetoothCFrameDel		= (KUidBluetoothDebugPubSubKeyBase + 0x41);
const TUint KPropertyKeyBluetoothCFramePeak		= (KUidBluetoothDebugPubSubKeyBase + 0x42);
const TUint KPropertyKeyBluetoothGFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x50);
const TUint KPropertyKeyBluetoothGFrameDel		= (KUidBluetoothDebugPubSubKeyBase + 0x51);
const TUint KPropertyKeyBluetoothGFramePeak		= (KUidBluetoothDebugPubSubKeyBase + 0x52);
const TUint KPropertyKeyBluetoothDataFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x60);
const TUint KPropertyKeyBluetoothDataFrameDel	= (KUidBluetoothDebugPubSubKeyBase + 0x61);
const TUint KPropertyKeyBluetoothDataFramePeak	= (KUidBluetoothDebugPubSubKeyBase + 0x62);
const TUint KPropertyKeyBluetoothFragFrameAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0x70);
const TUint KPropertyKeyBluetoothFragFrameDel	= (KUidBluetoothDebugPubSubKeyBase + 0x71);
const TUint KPropertyKeyBluetoothFragFramePeak	= (KUidBluetoothDebugPubSubKeyBase + 0x72);

const TUint KPropertyKeyBluetoothSDUAlloc		= (KUidBluetoothDebugPubSubKeyBase + 0x80);
const TUint KPropertyKeyBluetoothSDUDel			= (KUidBluetoothDebugPubSubKeyBase + 0x81);
const TUint KPropertyKeyBluetoothSDUPeak		= (KUidBluetoothDebugPubSubKeyBase + 0x82);

const TUint KPropertyKeyBluetoothMuxerAlloc		= (KUidBluetoothDebugPubSubKeyBase + 0x90);
const TUint KPropertyKeyBluetoothMuxerDel		= (KUidBluetoothDebugPubSubKeyBase + 0x91);
const TUint KPropertyKeyBluetoothMuxerPeak		= (KUidBluetoothDebugPubSubKeyBase + 0x92);

const TUint KPropertyKeyBluetoothLSAPAlloc		= (KUidBluetoothDebugPubSubKeyBase + 0xa0);
const TUint KPropertyKeyBluetoothLSAPDel		= (KUidBluetoothDebugPubSubKeyBase + 0xa1);
const TUint KPropertyKeyBluetoothLSAPPeak		= (KUidBluetoothDebugPubSubKeyBase + 0xa2);

const TUint KPropertyKeyBluetoothASAPAlloc		= (KUidBluetoothDebugPubSubKeyBase + 0xb0);
const TUint KPropertyKeyBluetoothASAPDel		= (KUidBluetoothDebugPubSubKeyBase + 0xb1);
const TUint KPropertyKeyBluetoothASAPPeak		= (KUidBluetoothDebugPubSubKeyBase + 0xb2);

const TUint KPropertyKeyBluetoothPSAPAlloc		= (KUidBluetoothDebugPubSubKeyBase + 0xc0);
const TUint KPropertyKeyBluetoothPSAPDel		= (KUidBluetoothDebugPubSubKeyBase + 0xc1);
const TUint KPropertyKeyBluetoothPSAPPeak		= (KUidBluetoothDebugPubSubKeyBase + 0xc2);

const TUint KPropertyKeyBluetoothCommandAlloc	= (KUidBluetoothDebugPubSubKeyBase + 0xd0);
const TUint KPropertyKeyBluetoothCommandDel		= (KUidBluetoothDebugPubSubKeyBase + 0xd1);
const TUint KPropertyKeyBluetoothCommandPeak	= (KUidBluetoothDebugPubSubKeyBase + 0xd2);

const TUint KPropertyKeyBluetoothL2DBGGetTimerGrpCID	= (KUidBluetoothDebugPubSubKeyBase + 0x100);
const TUint KPropertyKeyBluetoothL2DBGSetTimerGrpCID	= (KUidBluetoothDebugPubSubKeyBase + 0x101);

const TUint KPropertyKeyBluetoothL2DBGMinPDUGetTime		= (KUidBluetoothDebugPubSubKeyBase + 0x110);
const TUint KPropertyKeyBluetoothL2DBGMaxPDUGetTime		= (KUidBluetoothDebugPubSubKeyBase + 0x111);

const TUint KPropertyKeyBluetoothL2DBGMinPDUSentTime	= (KUidBluetoothDebugPubSubKeyBase + 0x112);
const TUint KPropertyKeyBluetoothL2DBGMaxPDUSentTime	= (KUidBluetoothDebugPubSubKeyBase + 0x113);

const TUint KPropertyKeyBluetoothL2DBGAvePDUGetTime		= (KUidBluetoothDebugPubSubKeyBase + 0x114);
const TUint KPropertyKeyBluetoothL2DBGAvePDUSentTime	= (KUidBluetoothDebugPubSubKeyBase + 0x115);


const TUint KPropertyKeyBluetoothQueuedSDUFlushedCounter		= (KUidBluetoothDebugPubSubKeyBase + 0x200);
const TUint KPropertyKeyBluetoothPartialSentSDUFlushedCounter	= (KUidBluetoothDebugPubSubKeyBase + 0x201);
const TUint KPropertyKeyBluetoothSentSDUFlushedCounter			= (KUidBluetoothDebugPubSubKeyBase + 0x202);
const TUint KPropertyKeyBluetoothFlushedPDUMuxerResend			= (KUidBluetoothDebugPubSubKeyBase + 0x203);
const TUint KPropertyKeyBluetoothFlushedPDUDataFlowResend		= (KUidBluetoothDebugPubSubKeyBase + 0x204);
const TUint KPropertyKeyBluetoothFlushedPDUIFrame				= (KUidBluetoothDebugPubSubKeyBase + 0x205);
const TUint KPropertyKeyBluetoothIncompleteErroredSDUReceived	= (KUidBluetoothDebugPubSubKeyBase +  0x206);



class CBTTestConnection;
class CBTTestConsole;


class TBTQueuedBasebandEventNotification : public TBTBasebandEventNotification
/**
	Extends via aggregation public TBTBasebandEventNotification class to make it queue-able
**/
	{
public:
	TBTQueuedBasebandEventNotification(const TBTBasebandEventNotification& aEvent)
		: TBTBasebandEventNotification(aEvent) {};

public:
	TSglQueLink iLink;
	};

class TConnProfile
	{
public:
	enum TConnRole
		{
		ENone,
		EMaster,
		ESlave,
		ENumRoles
		};

	enum TConnMode
		{
		EActive,
		ESniff,
		EPark,
		EHold,
		EExplicitActive,
		ENumModes
		};

	enum TConnState
		{
		EConnIdle,
		EConnListening,
		EConnListeningAccept,
		EConnAccepting,
		EConnConnecting,
		EConnConnected,
		EConnDisconnected,
		EConnServerDisconnected,
		EConnFailed,
		ENumStates
		};


	enum TDataState
		{
		EDataIdle,
		EDataDialog1,
		EDataDialog2,
		EDataFailed,
		EDataSend,
		EDataRecv,
		EDataRecvAny,
		EDataSendReceive,
		EDataSendOneShot,
		EDataRecvOneShot,
		ENumDataStates,
		ERecvContinuationData
		};

	TConnProfile();


	TInt		iPhyLinkIx;
	TConnState	iState;
	TConnRole	iRole;
	TUint		iPackets;
	TUint		iMaxSlots;
	//TBTLinkMode	iMode;
	TConnMode	iMode2;
	TUint		iModeAllowed;
	TBool		iSwitchAllowed;
	TBool		iEncryption;
	TDataState	iData;
	TPoint		iDataCursor;

	TInt		iAccepterIx;

	TBool		iIsSocketBased;

	TBTSockAddr	iRemoteBTAddr;
	TInt 		iErrorCode;

	TUint16		iSendingMTUSize;
	TUint16		iRecvMTUSize;

	TBool		iIsStreamProtocol;
	TBool		iIsSCO;
	TBool		iAutoSniffActive;

	static const TDesC& StateName(TConnState aState);
	static const TDesC& RoleName(TConnRole aState);
	static const TDesC& ModeName(TBTLinkMode aState);
	static const TDesC& ModeName(TConnProfile::TConnMode aState);
	static const TDesC& DataStateName(TDataState aState);
	};


class CBTTestConnectionNotifier : public CActive
	{
public:
	static CBTTestConnectionNotifier* NewL(CBTTestConnection & aParent);
	static CBTTestConnectionNotifier* NewLC(CBTTestConnection & aParent);
	CBTTestConnectionNotifier(CBTTestConnection & aParent);

	void CancelRequests();
	virtual ~CBTTestConnectionNotifier();

	void RequestNotifications(TUint32 aNotifications, RBTPhysicalLinkAdapter & aPhyAdapter);

private:
	void DoCancel();
	void RunL();

	CBTTestConnection& iParent;
	TBTBasebandEvent iEvent;
	TBTBasebandEventNotification iNotification;
	};


class CBTTestConnection;

// Thin handler class used to solver the issue of name clashes in BT API.
class TBTTestConnectionSyncLinkHandler : public MBluetoothSynchronousLinkNotifier
	{
public:
	TBTTestConnectionSyncLinkHandler(CBTTestConnection& aTestConnection) : iTestConnection(aTestConnection) {};

	// MBluetoothSynchronousLinkNotifier definitions
	inline void HandleSetupConnectionCompleteL(TInt aErr);
	inline void HandleDisconnectionCompleteL(TInt aErr);
	inline void HandleAcceptConnectionCompleteL(TInt aErr);
	inline void HandleSendCompleteL(TInt aErr);
	inline void HandleReceiveCompleteL(TInt aErr);

private:
	CBTTestConnection& iTestConnection;
	};



class CBTTestConnection : public CBase,
                          public MBluetoothSocketNotifier,
						  public MBluetoothPhysicalLinksNotifier,
						  public MBluetoothPhysicalLinkMetricsObserver
	{
public:
	enum TBTTestConnectionPanic
		{
		EBTConsoleNoSocketForSocketBasedConnection,
		EBTConsoleShutdownCalledOnNonCBluetoothSocket,
		EBTConsoleDisconnectCalledOnCBluetoothSocket,
		EBTConsoleInvalidLowPowerMode,
		EBTConsoleActivateActiveRequesterCalledOnCBluetoothSocket,
		};

	// METHODS
	static CBTTestConnection* NewL(CBTTestConsole* aConsole, RSocketServ& aSocketServer);
	static CBTTestConnection* NewLC(CBTTestConsole* aConsole, RSocketServ& aSocketServer);

	CBTTestConnection(CBTTestConsole* aConsole, RSocketServ& aSocketServer);
	virtual ~CBTTestConnection();

	TInt CreateListeningSocket(TUint16 aPort, TDesC16 const& aProtocolName, TBTServiceSecurity & aSecurity, TBool aUseDefaultConfig);
	TInt Accept(CBluetoothSocket& aListeningSocket);

	TInt ConnectToPeer(TBTDevAddr &aAddr, TUint16 aPort, TDesC16 const& aProtocolName, TBTServiceSecurity & aSecurity, TBool aUseDefaultConfig);
	TInt ConnectUsingBAP(TBTDevAddr &aAddr);

	TInt SetupPhysAdapter();

	void AcceptComplete(TInt result);
	void UpdateProfileAcceptComplete();
	void ConnectComplete(TInt result);
	void ConnectBAPComplete(TInt result);

	TInt ConfigureLink(TBool aUseIoctl);

	TInt SendEchoRequest(TInt aPayloadSize);

	void CancelAccept();
	void CancelConnect();

	void Close(RSocket& aSocket);
	TInt ShutdownSocket(RSocket::TShutdown aShutdownType);
	TInt DisconnectLink();
	TInt IncreaseRTXTimer(TUint aTime);

	void RequestNotification(TUint32 aNotification);
	void OutputEventNotification(TBTBasebandEventNotification & aNotification);
	void OutputOneShotEventNotification(TBTBasebandEventNotification & aNotification);
	void CancelBasebandNotifyRequest();

	TInt ChangeSupportedPacketTypes(TUint16 aPacketTypes);

	TInt RoleChangeAllowed(TBool aAllowed);
	TInt RequestRoleChange(TBTBasebandRole aRole);
	TInt RequestAuthentication();

	void StartACLData();
	void StopACLData();

	TBool SendComplete(TInt aIndex, TInt aStatus);
	TBool RecvComplete(TInt aIndex, TInt aStatus, TInt aDataErr);

	TInt RequestLPM(TConnProfile::TConnMode aLPM);
	TInt SupportLPM(TBTLinkMode aLPM);
	TInt PreventLPM(TBTLinkMode aLPM);
	TInt RemoveLPMRequest();

	TInt KillAllLinks(TInt aReason);
	TInt KillLink(TInt aReason);

	TInt ToggleAutoSniff();

	TInt PassiveSCO();
	TInt ActiveSCO(TBTDevAddr& aAddr);

	TInt PassiveESCO();
	TInt ActiveESCO(TBTDevAddr& aAddr);

	TInt SendMTUIoctl(TBool aIsIncomingMTU, TUint16 aNewValue);
	TInt SendPretendIncomingSduQFullIoctl(TBool aPretendIncomingSduQFull);
	TInt GetChannelMode(TL2CapChannelMode& aMode);
	TInt GetLocalPSM(TL2CAPPort& aPsm);

	TInt RecvOneShotData();
	//Send Data filled with 0123456789012... sequences
	TInt SendOneShotData(TUint16 aDataSize);

	//Receive part of data according buffer size aDataSize and check continuation in the mean time
	TInt RecvContinuationData(TUint16 aDataSize);

	TInt SetLocalModemStatus(TUint8 aModemStatus);
	TInt RequestRFCommConfChgInd();

	void StartPlmDisplayL();
	void StopPlmDisplayL();

	void StoreEvent(TBTBasebandEventNotification & aEvent);
	TBool PrintEvents(TInt aCurrentEvent, TInt aNumEvents);
	void ClearEventStore();
	TInt PrintBTPhyConnections();
	void StartLinkMonitorL();
	void StopLinkMonitor();
	void DoMonitor();

	CBluetoothSocket& GetBTSocket() const { return *iBTSocket; };
	TInt GetRemoteName(TNameEntry& aNameEntry);

	// CBluetoothSocket MBluetoothSocketNotifier definitions
	void HandleConnectCompleteL(TInt aErr);
	void HandleAcceptCompleteL(TInt aErr);
	void HandleShutdownCompleteL(TInt aErr);
	void HandleSendCompleteL(TInt aErr);
	void HandleReceiveCompleteL(TInt aErr);
	void HandleIoctlCompleteL(TInt aErr);
	void HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification);

	// MBluetoothPhysicalLinksNotifier definitions
	void HandleCreateConnectionCompleteL(TInt aErr);
	void HandleDisconnectCompleteL(TInt aErr);
	void HandleDisconnectAllCompleteL(TInt aErr);

	// MBluetoothSynchronousLinkNotifier definitions
	void HandleSyncSetupConnectionCompleteL(TInt aErr);
	void HandleSyncDisconnectionCompleteL(TInt aErr);
	void HandleSyncAcceptConnectionCompleteL(TInt aErr);
	void HandleSyncSendCompleteL(TInt aErr);
	void HandleSyncReceiveCompleteL(TInt aErr);

	// MBluetoothPhysicalLinkMetricsObserver definitions
	void MbplmoRssiChanged(TInt8 aRssi);
	void MbplmoLinkQualityChanged(TUint8 aLinkQuality);
	void MbplmoFailedContactCounterChanged(TUint16 aFailedContactCounter);
	void MbplmoTransmitPowerLevelChanged(TInt8 aTransmitPowerLevel);
	void MbplmoError(TInt aError);

	void ContinueDialog();
	void DoSend(TUint8 aVal);
	void DoRecv();
	TInt CheckRecvData();

	static TInt MonitorCB(TAny* aCBTTestConnection);

	void Panic(TBTTestConnectionPanic aPanic);

// ----------------------------------------------------------------------------
//							DEBUG METHODS
// ----------------------------------------------------------------------------
#ifdef _DEBUG
	TInt DataPlaneConfig();
	TInt IssueManualFlush();
#endif


protected:
private:

// DATA
public:
	TConnProfile iProfile;
	TUint iDataDialogCount;

protected:
	void ConstructL();
private:
	enum TCurrentIOCTL
		{
		EDontCare,
		ERFCOMMConfigInd,
		};

	TTime iTime;

	CBTTestConsole*				iParent;
	RSocketServ&				iSocketServer;

	CBTTestConnectionNotifier	iNotifier;

	CBluetoothSocket*			iBTSocket;
	CBluetoothPhysicalLinks*	iBTPhyLinks;
	CBluetoothSynchronousLink*  iBTSyncLink;
	TBTTestConnectionSyncLinkHandler iSyncLinkHandler;

	RBTPhysicalLinkAdapter		iPhyLinkAdapter;

	TBuf8<0xffff>				iRecvBuffer;
	TBuf8<0xffff>				iSendBuffer;
	TBuf8<0xffff>				iStreamRecvBuffer;

	TBuf8<672>					iEchoDataBuffer;
	TPckgBuf<TUint16>			i16bitIoctlBuffer;
	TPckgBuf<TBool>				iBoolIoctlBuffer;
	TSockXfrLength				iRecvBufferLength;
	TL2CapConfigPkg 			iConfigPkg;

	TSglQue<TBTQueuedBasebandEventNotification> iEventNotificationQueue;
	TCurrentIOCTL				iCurrentIOCTL;

	RBuf8 iCustomSendRecvBuffer;
	//Record the last digit from the former sequence, initial value is -1
	TInt iTailDigit;
	//Count the total number of bytes received for each UDP transfer
	TInt iTotalRecvByte;

	TL2CAPSockAddr				iPeerSocketAddr;
	TBool						iAutoSniff;
	CPeriodic*					iPeriodic;
	CBluetoothPhysicalLinkMetrics*		iMetrics;
	CConsoleBase*				iMetricsConsole;
	TUint						iLogTime;
	};


inline void TBTTestConnectionSyncLinkHandler::HandleSetupConnectionCompleteL(TInt aErr)
	{
	iTestConnection.HandleSyncSetupConnectionCompleteL(aErr);
	}

inline void TBTTestConnectionSyncLinkHandler::HandleDisconnectionCompleteL(TInt aErr)
	{
	iTestConnection.HandleSyncDisconnectionCompleteL(aErr);
	}

inline void TBTTestConnectionSyncLinkHandler::HandleAcceptConnectionCompleteL(TInt aErr)
	{
	iTestConnection.HandleSyncAcceptConnectionCompleteL(aErr);
	}

inline void TBTTestConnectionSyncLinkHandler::HandleSendCompleteL(TInt aErr)
	{
	iTestConnection.HandleSyncSendCompleteL(aErr);
	}

inline void TBTTestConnectionSyncLinkHandler::HandleReceiveCompleteL(TInt aErr)
	{
	iTestConnection.HandleSyncReceiveCompleteL(aErr);
	}


#endif
