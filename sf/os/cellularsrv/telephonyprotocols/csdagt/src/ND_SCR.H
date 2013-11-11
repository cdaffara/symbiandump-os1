/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Header for NetDial scripts.
* 
*
*/



/**
 @file Nd_scr.h
 @internalComponent
*/

#ifndef __ND_SCR_H__
#define __ND_SCR_H__

#include <cdblen.h>
#include <etel.h>
#include <comms-infras/dialogprocessor.h>

// Forward declarations

class CDialOutSMBase;
class CCommsDbNetDialAccess;
class CDialogProcessor;
class CScriptExecutor;

class MNetDialScriptObserver
/**
MNetDialScriptObserver class is used by CNetDialScript to complete the request of the calling state.

@internalComponent
*/
	{
public:
	virtual void ScriptFunctionComplete(TInt aError) =0;
	};


class CNetDialScript : public CActive, public MDialogProcessorObserver
/**
One of these per CDialOutSMBase object. Interfaces between state machine and the script 
execution and script execution and the dialog server. Uses a CDialOutSMBase to callback on 
completion of scanning and executing scripts, a CCommsDbAccess to access script and login 
information from the database, and a CDialogProcessor for PCT commands.  It owns a 
CScriptExecutor object for controlling the script.

@internalComponent
*/
	{
public:
	static CNetDialScript* NewL(CCommsDbNetDialAccess* aDb,CDialogProcessor* aDlgPrc,RCall::TCommPort& aCommPort,TCommRole aRole,TInt aLength);
	~CNetDialScript();

	// Script-related calls to script executor 
	void Scan(MNetDialScriptObserver& aObserver);
	TBool GetLoginParamsL(MNetDialScriptObserver& aObserver, TBool aIsReconnect);
	void ExecuteL(MNetDialScriptObserver& aObserver);
	void CloseScript();
	void CleanupScript();

	// Call from script executor on completion
	virtual void ScriptOperationComplete(TInt aError);
	
	// IO-related calls to script executor 
	void ConfigureCommPort(TRequestStatus& aStatus, const TCommConfig& aConfig);
	void CancelConfigureCommPort();
	void ReConfigureAndCancelCommPort(TRequestStatus& aStatus);
	void DropSignals(TRequestStatus& aStatus);
	void CreateChannel(TRequestStatus& aStatus);
	void CancelCreateChannel();
	void ShutdownChannel(TRequestStatus& aStatus);
	TInt GetExcessData(TDes8& aBuffer);
	void DropDTR(TRequestStatus* aStatusPtr);

	// Calls to dialog serve 
	TInt WritePct(TDes& aBuffer);
	void ReadPct(TDes& aBuffer);
	void ClosePct();
	void CancelDialogServer();	

	// others 
	inline TPtrC LoginName() const;
	inline TPtrC LoginPass() const;
	inline TBool UsePct() const;
	inline const TDesC& BcaStack() const;
	inline TInt IapId() const;

	// Derived from MDialogProcessorObserver 
	virtual void MDPOLoginComplete(TInt aError);
	virtual void MDPOReadPctComplete(TInt aError);
	virtual void MDPODestroyPctComplete(TInt aError);

protected:
	// Derived from CActive 
	virtual void RunL();
	virtual void DoCancel();

private:
	CNetDialScript(CCommsDbNetDialAccess* aDb,CDialogProcessor* aDlgPrc,TInt aLength);
	void ConstructL(RCall::TCommPort& aCommPort,TCommRole aRole);
	void GetAndSetScriptL();
	void SetLoginVarsL();
	void DoScanScriptL();
	void GetBcaStack();
	void GetIapIdL();
	// calls to the dialog server 
	TInt OpenPct();
	void DestroyPctNotification();
private:
	enum 
		{
		EIdle =0,
		EScanScript,
		EGetLogin,
		EExecute
		} iState;

	CCommsDbNetDialAccess* iDb;
	CDialogProcessor* iDlgPrc;
	CScriptExecutor* iScriptExecutor;
	MNetDialScriptObserver* iCurrentObserver;
	TBool iUsePct;
	TBool iPctOpen;
	TBuf<KCommsDbSvrMaxUserIdPassLength> iLoginName;
	TBuf<KCommsDbSvrMaxUserIdPassLength> iLoginPass;
	TBuf<KCommsDbSvrMaxFieldLength> iIPAddress;
	TBuf<KCommsDbSvrMaxFieldLength> iBcaStack;
	TInt iIapId;
	TBool iUseLoginDialog;
	TInt iLength;
	};

inline TPtrC CNetDialScript::LoginName() const 
/**
Return login username.
*/
	{
	return iLoginName;
	}

inline TPtrC CNetDialScript::LoginPass() const 
/**
Return login password.
*/
	{
	return iLoginPass;
	}

inline TBool CNetDialScript::UsePct() const
/**
Return PCT usage.
*/
	{
	return iUsePct;
	}

inline const TDesC& CNetDialScript::BcaStack() const
	{
	return iBcaStack;
	}

inline TInt CNetDialScript::IapId() const
	{
	return iIapId;
	}

#endif
