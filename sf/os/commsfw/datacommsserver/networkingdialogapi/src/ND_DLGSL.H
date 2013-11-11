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
//

/**
 @file
 @internalComponent
*/

#ifndef __ND_DLGSL_H
#define	__ND_DLGSL_H

#include <e32cons.h>
#include <agentdialog.h>
#include <networking/nd_std.h>
#include "agentnotifierdefs.h"
#include <comms-infras/commsdebugutility.h>

#include <metadatabase.h>
#include <commsdattypesv1_1.h>
using namespace CommsDat;
class TConnectionPrefs;

class CNetDialDialogSession;

__FLOG_STMT(_LIT8(KDlgSvrLogSubsys, "agentdialog");)
__FLOG_STMT(_LIT8(KDlgSvrLogComponent, "server");)
//__FLOG_STMT(_LIT8(KDlgCliLogComponent, "client");)

NONSHARABLE_CLASS(CDialogBase) : public CActive
/**
@internalComponent
*/
	{
public:
	virtual ~CDialogBase();
protected:
	CDialogBase(CNetDialDialogSession* aSession, TInt aPriority);
	virtual void ConstructL();
    void StartNotifierAndGetResponse();
    void CancelNotifier();

	// CActive
	virtual void DoCancel();

private:
	enum TBufferSize
		{
		EGenericBufferSize=80
		};
protected:
	CNetDialDialogSession* iSession;
	RNotifier* iNotifier;
	CAsyncCallBack* iCallBack;
	TInt iNotifyResult;
	TBool iCancelFlag;
	TAgentDialogInput& iAgentDialogInput;
	TInt& iAgentDialogOutput;
	TAgentDialogInputBuf iAgentDialogInputBuf;
	TAgentDialogOutputBuf iAgentDialogOutputBuf;
	
	TUid iUidAgentDialogNotifier;
	__FLOG_DECLARATION_MEMBER;
	};

NONSHARABLE_CLASS(CModemAndLocationSelection) : public CDialogBase
/**
@internalComponent
*/
	{
public:
	static CModemAndLocationSelection* NewL(CMDBSession* aDb,CNetDialDialogSession* aSession, TInt aPriority=CActive::EPriorityStandard);
	virtual ~CModemAndLocationSelection();
	void GetModemAndLocationL(TUint32& aModemId,TUint32& aLocationId,TCallBack aCallBack);
	TInt Status();

private:
	virtual void RunL();
	// CDialogBase
	virtual void ConstructL();
	//
	CModemAndLocationSelection(CMDBSession* aDb,CNetDialDialogSession* aSession, TInt aPriority);
	void InitRecordSetL(TMDBElementId aRecordId);
	void StartModemSelectL();
	void StartLocationSelectL();
	void DisplayNoRecordsL();
	void SelectL(const TDesC& aTable, TInt aError);
	void ReselectL();
private:
	enum {
		EEnterModem,
		EEnterLocation,
		ENoRecordFound
		} iState;
	CMDBSession* iDb;
	CMDBRecordSetBase* iTable;
	TInt iRecordIndex;
	TInt iResultStatus;
	TUint32* iModemId;
	TUint32* iLocationId;
	};

NONSHARABLE_CLASS(CIAPSelection) : public CDialogBase
/**
@internalComponent
*/
	{
public:
	static CIAPSelection* NewL(CMDBSession* aDb,CNetDialDialogSession* aSession, TInt aPriority=CActive::EPriorityStandard);		
	~CIAPSelection();
	void GetIAPL(TUint32& aIAPId, const TConnectionPrefs& aPrefs, TInt aError, TCallBack aCallBack);
	TInt Status();

private:
	virtual void RunL();
	// CDialogBase
	virtual void ConstructL();
	//
	CIAPSelection(CMDBSession* aDb,CNetDialDialogSession* aSession, TInt aPriority);
	void InitRecordSetL();
	void SelectL(TInt aError);
	void ReselectL();
private:
	CMDBSession* iDb;
	CMDBRecordSetBase* iTable;	
	TInt iRecordIndex;
	
	TBool iRecordsPresent;
	TInt iResultStatus;
	TUint32* iIAPId;
	};

NONSHARABLE_CLASS(CIAPWarning) : public CDialogBase
/**
@internalComponent
*/
	{
public:
	static CIAPWarning* NewL(CNetDialDialogSession* aSession, TInt aPriority = CActive::EPriorityStandard);
	~CIAPWarning();
	void WarnIAPL(const TConnectionPrefs& aPrefs, TInt aLastError, const TDesC& aNewIapName, TBool& aResponse, TCallBack aCallBack);
	TInt Status();

private:
	virtual void RunL();
	// CDialogBase
	virtual void ConstructL();
	//
	CIAPWarning(CNetDialDialogSession* aSession, TInt aPriority);
private:
	TInt iResultStatus;
	TBool* iResponse;
	};

NONSHARABLE_CLASS(CAccessPointSelection) : public CDialogBase
/**
@internalComponent
*/
	{
public:
	static CAccessPointSelection* NewL(CMDBSession* aDb,CNetDialDialogSession* aSession, TInt aPriority=CActive::EPriorityStandard);		
	~CAccessPointSelection();
	void GetAccessPointL(TUint32& aAPId, TInt aTierId, TCallBack aCallBack);
	TInt Status();

private:
	virtual void RunL();
	// CDialogBase
	virtual void ConstructL();
	//
	CAccessPointSelection(CMDBSession* aDb,CNetDialDialogSession* aSession, TInt aPriority);
	void InitRecordSetL(TUint32 aDefaultSnap, TInt aTierId);
	void ReselectL();
private:
	CMDBSession* iDb;
	CMDBRecordSetBase* iTable;	
	TInt iRecordIndex;
	
	TBool iRecordsPresent;
	TInt iResultStatus;
	TUint32* iAPId;
	};

NONSHARABLE_CLASS(CLogin) : public CDialogBase
/**
@internalComponent
*/
	{
public:
	static CLogin* NewL(CNetDialDialogSession* aSession, TInt aPriority =CActive::EPriorityStandard);
	~CLogin();
	void GetUserPassL(TDes& aUsername, TDes& aPassword,TCallBack aCallBack);
	TInt Status();

private:
	virtual void RunL();
	//
	CLogin(CNetDialDialogSession* aSession, TInt aPriority =CActive::EPriorityStandard);
	void StartGetUsername();
	void StartGetPassword();	
private:
	enum {
		EEnterName,
		EEnterPass
		} iState;
	
	TInt iResultStatus;
	TDes* iUsername;
	TDes* iPassword;
	};

NONSHARABLE_CLASS(CAuthenticate) : public CDialogBase
/**
@internalComponent
*/
	{
public:
	static CAuthenticate* NewL(CNetDialDialogSession* aSession, TInt aPriority =CActive::EPriorityStandard);
	~CAuthenticate();
	void GetUserPassL(TDes& aUsername, TDes& aPassword,TCallBack aCallBack);
	TInt Status();

private:
	virtual void RunL();
	//
	CAuthenticate(CNetDialDialogSession* aSession, TInt aPriority =CActive::EPriorityStandard);
	void StartGetUsername();
	void StartGetPassword();
private:
	enum {
		EEnterName,
		EEnterPass
		} iState;

	TInt iResultStatus;
	TDes* iUsername;
	TDes* iPassword;
	};

NONSHARABLE_CLASS(CReconnect) : public CDialogBase
/**
@internalComponent
*/
	{
public:
	static CReconnect* NewL(CNetDialDialogSession* aSession, TInt aPriority =CActive::EPriorityStandard);
	~CReconnect();
	void ReconnectL(TCallBack aCallBack);
	TBool Status();
	// CActive
	virtual void DoCancel();
private:
	virtual void RunL();
	//
	CReconnect(CNetDialDialogSession* aSession, TInt aPriority =CActive::EPriorityStandard);
private:
	TBool iResultStatus;
	};

class CPctTimer;
NONSHARABLE_CLASS(CPct) : public CDialogBase
/**
@internalComponent
*/
	{
public:
	static CPct* NewL(CNetDialDialogSession* aSession, TInt aPriority =CActive::EPriorityStandard);
	~CPct();
	void WritePct(TDes& aBuffer);
	void ReadPctL(TDes& aBuffer,TCallBack& aCallBack);
	void ClosePct();
	void DestroyPctNotificationL(TCallBack& aCallBack);
	void PctTimerComplete(TInt aStatus);
	void CancelEverything();
	TInt Status();
	void DoNotificationCancel();
	void DoReadCancel();
	void DoTimerCancel();
	// CActive
	virtual void DoCancel();
private:
	virtual void RunL();
	// CDialogBase
	virtual void ConstructL();
	//
	CPct(CNetDialDialogSession* aSession, TInt aPriority =CActive::EPriorityStandard);
	void StartRead();
	void DoWrite();
private:
	enum {
		ENone,
		EWrite,
		ERead
		} iState;

	TInt iResultStatus;
	TBuf<KRxBufferSize> iWriteBuffer;
	TInt iStartBuffer;
	TDes* iReadBuffer;
	CPctTimer* iTimer;
	CAsyncCallBack iDestroyCallBack;
	TBool iDestroyCallBackOutstanding;
	};


NONSHARABLE_CLASS(CPctTimer) : public CTimer
/**
CPctTimer 

@internalComponent
*/
	{
public:
	static CPctTimer* NewL(CPct* aNotifier);
	~CPctTimer();
	void Start();
private:
	CPctTimer(CPct* aNotifier);
	// CTimer
	virtual void RunL();
private:	
	CPct* iNotifier;
	};

NONSHARABLE_CLASS(CQoSWarning) : public CDialogBase
/**
@internalComponent
*/
	{
public:
	static CQoSWarning* NewL(CNetDialDialogSession* aSession, TInt aPriority = CActive::EPriorityStandard);
	~CQoSWarning();
	void WarnQoSL(TBool& aResponse, TCallBack aCallBack);
	TInt Status();

private:
	virtual void RunL();
	// CDialogBase
	virtual void ConstructL();
	//
	CQoSWarning(CNetDialDialogSession* aSession, TInt aPriority);
private:
	TInt iResultStatus;
	TBool* iResponse;
	};

#endif
