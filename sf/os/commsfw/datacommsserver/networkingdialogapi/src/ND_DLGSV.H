// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Dial Up Networking Dialog Server Header - Server Side
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __ND_DLGSV_H__
#define __ND_DLGSV_H__

#include <e32base.h>
#include <agentdialog.h>
#include <cfmsgs.h>
#include <cfshared.h>
#include <cfutil.h>
#include <comms-infras/commsdebugutility.h>
#include <metadatabase.h>
using namespace CommsDat;

/**
Reasons for panic
*/
enum TAgentDialogPanic
	{
	EClientBadRequest,
	EClientBadDescriptor,
	EClientBadRecordId,
	EClientPctNotOpen,
	EClientPctAlreadyOpen,
	EClientReadPctOutstandingOnClose,
	EClientDestroyPctNotfcnOutstandingOnClose,
	//
	ESvrMainSchedulerError,
	ESvrCreateServer,
	ESvrStartServer,
	ESvrDllFileNameNotFound,
	ESvrPctNotOpen,
	ESvrClosePctReturnedError,
	ESvrNoReadPctOutstanding,
	ESvrNoDestroyPctNotfcnOutstanding,
	ESvrNotNullOnClose,
	ESvrNoCommsServerProcess,
	ESvrCannotOpenServerProcess,
	ESvrCancelReturnedError,
	ESvrGeneralCallBackNoSession,
	ESvrGeneralCallBackError,
	ESvrUnloaderNull
	};

/** 
Dialog Server Major Version Number

Note that a version must be specifyed when creating a session with the server 
*/
const TUint KDialogServMajorVersionNumber=1;
/** 
Dialog Server Minor Version Number

Note that a version must be specifyed when creating a session with the server 
*/
const TUint KDialogServMinorVersionNumber=0;
/** 
Dialog Server Build Version Number 

Note that a version must be specifyed when creating a session with the server 
*/
const TUint KDialogServBuildVersionNumber=102;


/**
opcodes used in message passing between client and server
*/
enum TDialogServRqst
	{
	EGetIAP,
	EGetModemAndLocation,
	EWarnNewIAP,
	EGetAuthentication,
	EGetLogin,
	EGetReconnectReq,
	EOpenPct,
	EWritePct,
	EReadPct,
	EDestroyPctNotification,
	EClosePct,
	ECancelGetIAP,
	ECancelGetModemAndLocation,
	ECancelWarnIAP,
	ECancelLogin,
	ECancelAuthenticate,
	ECancelReconnect,
	ECancelReadPct,
	ECancelDestroyPctNotification,
	EWarnQoS,
	ECancelWarnQoS,
	EShutdownDialogServer,
	EGetAccessPoint,
	ECancelGetAccessPoint,
    EGetAccessPointAP
	};

class CNetDialDialogServerChannelHandler;
NONSHARABLE_CLASS(CNetDialDialogServer) : public CServer2
/**
CNetDialDialogServer

The server class; an active object.
Contains an instance of RServer; a handle to the kernel
server representation which is used to receive messages. 
*/
	{
public:
 	static CNetDialDialogServer* NewL(CommsFW::RCFChannel::TMsgQueues aRxQueues, CommsFW::RCFChannel::TMsgQueues aTxQueues);
 	virtual CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	void Shutdown();
	~CNetDialDialogServer();
private:
   	CNetDialDialogServer(TInt aPriority);
	void ConstructL(CommsFW::RCFChannel::TMsgQueues aRxQueues, CommsFW::RCFChannel::TMsgQueues aTxQueues);
private:
	CNetDialDialogServerChannelHandler* iChannel;
    __FLOG_DECLARATION_MEMBER;
   	};

NONSHARABLE_CLASS(CNetDialDialogServerChannelHandler) : public CommsFW::CCFModuleChannelHandler
/**
@internalComponent
*/
	{
	public:
		static CNetDialDialogServerChannelHandler* NewL(CommsFW::RCFChannel::TMsgQueues aRxQueues,
			CommsFW::RCFChannel::TMsgQueues aTxQueues, CNetDialDialogServer& aServer);
	private:
		CNetDialDialogServerChannelHandler(CNetDialDialogServer& aServer);
		virtual void CFMessageShutdown(const CommsFW::TCFShutdownMsg& aMessage);
	private:
		CNetDialDialogServer& iServer;
	};

//
// CNetDialDialogSession
class CIAPSelection;
class CAccessPointSelection;
class CModemAndLocationSelection;
class CIAPWarning;
class CLogin;
class CAuthenticate;
class CReconnect;
class CPct;
class CCommsDatabase;
class CQoSWarning;

NONSHARABLE_CLASS(CNetDialDialogSession) : public CSession2
/**
CNetDialDialogSession
*/
	{
public:
	static CNetDialDialogSession* NewL(CNetDialDialogServer* aServer);
	~CNetDialDialogSession();
// CSession

	virtual void ServiceL(const RMessage2& aMessage);
	void DispatchMessageL(const RMessage2& aMessage);
//
	void GetModemAndLocationL();
	void GetModemAndLocationCompleteL(TInt aStatus);
	void GetIAPL();
	void GetIAPCompleteL(TInt aStatus);
	void WarnIAPL();
	void WarnIAPCompleteL(TInt aStatus);
    void GetAccessPointL();
    void GetAccessPointAPL();
    void GetAccessPointCompleteL(TInt aStatus);
	void GetAuthenticationL();
	void CompleteAuthentication(TInt aStatus);
	void GetLoginL();
	void CompleteLoginL(TInt aStatus);
	void ReconnectReqL();
	void CompleteReconnectReq(TBool aBool);
//

	void CheckPctL(const RMessage2& aMessage);
	void OpenPctL();
	void WritePct();
	void ReadPctL();
	static TInt ReadPctCallBack(TAny* aContext);
	void ReadPctComplete(TInt aStatus);
	void DestroyPctNotificationL();
	static TInt DestroyPctCallBack(TAny* aContext);
	void DestroyPctComplete(TInt aStatus);
	void ClosePct();
	void WarnQoSL();
	void WarnQoSCompleteL(TInt aStatus);
	static TInt GeneralCallBackL(TAny* aSession);
	void Cancel(TDialogServRqst aRequest);
protected:
	void PanicClient(TInt aPanic) const;
private:
	CNetDialDialogSession(CNetDialDialogServer* aServer);
	void ConstructL();
	void CheckBusyL();
	void CancelEverything();
private:
	enum TSessionStates
		{
		EGetIAP,
		EGetModemAndLocation,
		EWarnIAP,
		ELogin,
		EAuthentication,
		EReconnectReq,
		EPct,
		EWarnQoSState,
		EGetAccessPointState
		} iState;

	enum TBufferSizes
		{
		EMaxWriteBufferSize=64,
		EMaxReadBufferSize=100
		};
private:
	TBool iBusy;
	TBool iPctOpen;
	TBool iReadPctOutstanding;
	TBool iDestroyPctNotfcnOutstanding;

	RMessage2 iMessage;
	RMessage2 iDestroyNotificationMessage;
	RMessage2 iReadPctMessage;

	CNetDialDialogServer* iNetDialDialogSvr;
	CMDBSession* iDb;

	CIAPSelection* iIAPSelection;
	CModemAndLocationSelection* iModemAndLocationSelection;
	CIAPWarning* iIAPWarning;
	CLogin* iLogin;
	CAuthenticate* iAuthenticate;
	CReconnect* iReconnect;
	CPct* iPct;
	CQoSWarning* iQoSWarning;

	TPckgBuf<TUint32> iIAP;
	TPckgBuf<TUint32> iModemId;
	TPckgBuf<TUint32> iLocationId;
	TPckgBuf<TIspConnectionNames> iConNames;
	TPckgBuf<TConnectionPrefs> iPrefs;
	TPckgBuf<TBool> iBool;
	TBuf<KCommsDbSvrDefaultTextFieldLength> iNewIAPName;
	TBuf<KCommsDbSvrMaxUserIdPassLength> iUsername;
	TBuf<KCommsDbSvrMaxUserIdPassLength> iPassword;
	HBufC* iBuffer;
	TPtr iBufPtr;
	TPtr iPctWriteBuffer;
	TBuf<EMaxReadBufferSize> iPctReadBuffer;
	TPckgBuf<TUint32> iAccessPoint;
	CAccessPointSelection* iAccessPointSelection;
	__FLOG_DECLARATION_MEMBER;
	};

/**
Global functions
*/
GLREF_C void PanicServer(TAgentDialogPanic aPanic);
IMPORT_C TInt ThreadFunction(TAny* aStarted);

#endif
