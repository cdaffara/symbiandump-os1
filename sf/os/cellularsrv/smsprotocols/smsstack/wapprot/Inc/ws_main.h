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
// Declares CWapSmsProtocolFamily, CWapSmsProtocol,
// CWapSmsProviderWrite and CWapSmsProvider
// 
//

/**
 @file
 @internalAll
*/

#ifndef WS_MAIN_H__
#define WS_MAIN_H__

#include <f32file.h>
#include "es_wsms.h"
#include <es_sock.h>
#include <es_prot.h>
#include <es_mbufif.h>
#include "smsstacklog.h"
#include <e32cmn.h>

#include "nbbackuprestoreint.h"

/**
 *  @internalComponent
 */
const TUint KWapSmsMajorVersionNumber=7;
const TUint KWapSmsMinorVersionNumber=0;
const TUint KWapSmsBuildVersionNumber=0;


class CWapSapMessageSender;
class CWapProtocolObserver;
class CSmsProtocol;
class CSmsMessage;
class CWapSmsProviderWrite;
class CBackupAndRestore;


/**
 *  @internalComponent
 */
class CWapSmsProtocolFamily : public CProtocolFamilyBase
	{
private:
	CWapSmsProtocolFamily();
public:
	static CWapSmsProtocolFamily * NewL();
	TInt Install();
	TInt Remove();
	CProtocolBase * NewProtocolL(TUint aSockType,TUint aProtocol);
	TUint ProtocolList(TServerProtocolDesc *& aProtocolList);
	};

/**
 *  @internalComponent
 */
extern "C"
	{
	IMPORT_C CProtocolFamilyBase* InstallWapSms();	// Force export
	}

class CWapSmsProvider;
class CWapDatagram;
class CWapReassemblyStore;


/**
 *  @internalComponent
 */
class CWapSmsProtocol : public CProtocolBase,
public MNBBackupAndRestoreObserver
	{
public:
	virtual ~CWapSmsProtocol();
	static CWapSmsProtocol *NewL();

// Calls from the socket server which we implement
	CServProviderBase *NewSAPL(TUint aSocketType);
	void InitL(TDesC& aTag);
	void StartL(void);
	void Identify(TServerProtocolDesc *)const;

	virtual CHostResolvProvdBase *NewHostResolverL();
	virtual CServiceResolvProvdBase *NewServiceResolverL();
	virtual CNetDBProvdBase* NewNetDatabaseL();

	void BindL(CProtocolBase *aProtocol, TUint aId);
	virtual void BindToL(CProtocolBase *protocol);
	TInt Send(TDes8 &, TSockAddr *to,TSockAddr *from,CProtocolBase* aSourceProtocol);
	TInt GetOption(TUint aLevel,TUint aName,TDes8 &aOption,CProtocolBase* aSourceProtocol);
	TInt SetOption(TUint aLevel,TUint aName,const TDesC8 &aOption,CProtocolBase* aSourceProtocol);
	void Error(TInt aError,CProtocolBase* aSourceProtocol);
public:
	void ProcessSmsL(const CSmsMessage& aSmsMessage);
	CSmsProtocol* SmsProtocol();
	TInt AddrAlreadyUsedByWAP(const TWapAddr& aAddr, const CWapSmsProvider* aSap);
	TBool AllocateLocalAddress(TWapAddr& aAddr);
	TBool FindAndDeleteMsg(CWapDatagram& aDatagram);
	TInt CheckSarL(const TWapAddr& aAddr,CWapSmsProvider* aSap);
private:
	CWapSmsProtocol();
	void BindObserversToSmsL();
	void RemoveObserversFromSmsProtocol();
	void AddSAP(CWapSmsProvider* aSAP, TUint aSockType);
	CWapSmsProvider* LookupSAP(CWapDatagram* aMsg);

	virtual void HandleBackupOrRestoreStartingL();
	virtual void HandleBackupOrRestoreCompleteL();

private:

	RFs iFs;
	CWapReassemblyStore* iWapStore;
	TDblQue<CWapSmsProvider> iSAPList;
	CSmsProtocol* iSmsProtocol;
	CWapProtocolObserver* iObserver[3];
	TInt iNextSapPort;

	CBackupAndRestore* iBackupRestoreSession;
	};


/**
 *  @internalComponent
 */
class CWapSmsProviderWrite : public CActive
	{
public:

    static CWapSmsProviderWrite* NewL(CWapSmsProvider& aWapSmsProvider);
    ~CWapSmsProviderWrite();
    void Start(RMBufChain& aBufChain, TSockAddr& aAddr);

private:
    CWapSmsProviderWrite(CWapSmsProvider& aWapSmsProvider);
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
private:

    CWapSmsProvider& iWapSmsProvider;
    TWapAddr iWapAddr;

    HBufC8* iDes;
    CWapSapMessageSender* iMsgSender;
	};


/**
 *  @internalComponent
 */
class CWapSmsProvider : public CServProviderBase
	{

	friend class CWapSmsProviderWrite;

public:

	enum TWapSmsMessageType
		{
		ESmartMessage,
		EWapDatagram
		};

public:
	static CWapSmsProvider* NewL(CWapSmsProtocol* aProtocol);
	virtual ~CWapSmsProvider();

public:
	void Start();
public: // Not Implemented
	void RemName(TSockAddr& /*aAddr*/)const{BULLSEYE_OFF BULLSEYE_RESTORE}
	TInt SetRemName(TSockAddr& /*aAddr*/){BULLSEYE_OFF return KErrNotSupported; BULLSEYE_RESTORE}
	void ActiveOpen() {BULLSEYE_OFF BULLSEYE_RESTORE}
	void ActiveOpen(const TDesC8& /*aConnectionData*/){BULLSEYE_OFF BULLSEYE_RESTORE}
	TInt PassiveOpen(TUint /*aQueSize*/){BULLSEYE_OFF return KErrNotSupported; BULLSEYE_RESTORE}
	TInt PassiveOpen(TUint /*aQueSize*/,const TDesC8& /*aConnectionData*/){BULLSEYE_OFF return KErrNotSupported; BULLSEYE_RESTORE}
	void Shutdown(TCloseType /*option*/,const TDesC8& /*aDisconnectionData*/){BULLSEYE_OFF BULLSEYE_RESTORE}
	void AutoBind(){}
public:
	void LocalName(TSockAddr& aAddr) const;
	TInt SetLocalName(TSockAddr& aAddr);
	TInt GetOption(TUint level,TUint name,TDes8& aOption)const;
	void Ioctl(TUint level,TUint name,TDes8* aOption);
	void CancelIoctl(TUint aLevel,TUint aName);
	TInt SetOption(TUint level,TUint name,const TDesC8 &aOption);
 	TInt Write(RMBufChain& aBufChain,TUint options, TSockAddr* aAddr=NULL);
 	TInt GetData(RMBufChain& aBufChain, TUint aLength, TUint options,TSockAddr* aAddr=NULL);
	void Shutdown(TCloseType option);
	TBool IsNewStyleClient() {return iIsNewStyleClient;}
public:
	void Error(TInt aError, TUint aOperationMask);
	void AddToQueue(CWapDatagram* aMsg);
	void NewData(CWapDatagram* aMsg);
	void Deliver(TDes& aDesc);
	TBool MatchesLocalAddress(const TWapAddr& aAddr);
	TWapSmsStatusReportScheme GetStatusReportScheme();
	TWapSmsDataCodingScheme GetDataCodingScheme();

public:
	TWapSmsMessageType MessageType() { return iMessageType; }
	static TInt LinkOffset();

protected:
	CWapSmsProvider(CWapSmsProtocol* aProtocol);
	virtual TInt SecurityCheck(MProvdSecurityChecker* aSecurityChecker);
private:

	TSglQue<CWapDatagram> iRecvdMsgQueue;
	TWapSmsDataCodingScheme iDataCodingScheme;
	TWapSmsMessageType iMessageType;
	TDblQueLink iSAPLink;
	TWapAddr iLocalAddress;
	CWapSmsProtocol* iProtocol;
	TUint iName;							// The integer "name" of the current ioctl request
	TBool iSendPending;
	TBool iIoctlOutstanding;				// Bool value to indicate the outstanding ioctl
	TBool iIsNewStyleClient;						// Indicate if it is new client (BC)
	CWapSmsProviderWrite* iWapSmsProviderWrite;
	MProvdSecurityChecker* iSecurityChecker; 	//< ESOCK security checker object
	TWapSmsStatusReportScheme iStatusReportScheme;
	};


#endif // WS_MAIN_H__
