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
* Script Executor Header.
* 
*
*/



/**
 @file Sscrexec.h
 @internalComponent
*/

#ifndef __SSCREXEC_H__
#define __SSCREXEC_H__

#include <d32comm.h>
#include <etel.h>
#include <comms-infras/dialogprocessor.h>
#include "ND_STD.H"
#include "SCOMMAND.H"

const TInt KMaxReadBufferLength=100;
const TInt KNumCommands=9;	///< number of derived command classes in SCommand.h

struct SErrorVariable
	{
	const TText* iString;
	TInt iValue;
	};

// Forward declarations

class CScriptIO;
class CScriptCharacterConverter;
class CNetDialScript;
class CGotoCommand;
class CWaitCommand;
class CLoopCommand;
class CReadPCTCommand;
class CScriptCommandBase;
class CScriptReader;
class CScriptVarMan;
class CScriptLabelMan;
class CScriptCharacterConverter;


class CScriptExecutor : public CActive
/**
One per CNetDialScript object.  Has one of all script commands and label and variable managers.
Controls execution of the scripts.

@internalComponent
*/
	{
public:
	static CScriptExecutor* NewL(CNetDialScript* aScript,const TDesC& aCommChannel, TInt aScriptLength);
	CScriptExecutor(CNetDialScript* aScript);
	~CScriptExecutor();
	//
	void SetLoginParams(const TDesC& aLoginName,const TDesC& aLoginPass);
	void SetScript(const TDesC& aScript);
	void ScanScriptL();
	void RunScript();
	
	static TInt StartScript(TAny* aPtr);
	void ProcessScript();
	void CompletedWrite(TInt aStatus);
	void CompletedReadL(TInt aStatus,TInt aIndex =0);
	void Disconnect();
	void CreateChannel(TRequestStatus& aStatus);
	void CancelCreateChannel();
	void ShutdownChannel(TRequestStatus& aStatus);
	void ReConfigureAndCancelCommPort(TRequestStatus& aStatus);
	void DropSignals(TRequestStatus& aStatus);
	void CloseScript();
	void Close();
	inline TBool IsScriptSet() const;
	//
	TInt WritePct(const TDesC8& aBuffer);
	void ReadPct();
	void ReadPctComplete(TInt aStatus);
	void DestroyPctNotificationReceived(TInt aStatus);
	void PctCancelAndClose();
	void SetReadFromPctPending(TBool aPending);
	inline TBool ReadFound() const;
	inline TBool ReadPctPending() const;
	inline TBool RequestUsePct() const;
	//
	void SetVariableL(const TDesC& aName, const TDesC& aContent);
	void ConfigureCommPort(TRequestStatus& aStatus, const TCommConfig& aConfiguration);
	void CancelConfigureCommPort();
	void DropDTR(TRequestStatus* aStatusPtr);

	inline const TDesC& BcaStack() const;
	inline TInt IapId() const;
	TInt GetExcessData(TDes8& aBuffer);

	void SetRetransmittedLoginTimeout(TReal& aRetransmitLoginTimeOut);
protected:
	// derived from CActive
	void RunL();
	void DoCancel();
private:
	void ConstructL(const TDesC& aCommChannel, TInt aScriptLength);
	void ProcessNextLinesL();
	void SetReadPctBuffer();
	void CompletedScript(TInt aStatus);
	void CleanupLastCommand();
	void ConvertScriptError(TInt& aError) const;
private:
	CScriptIO* iScriptIO;
	CGotoCommand* iGotoCommand;
	CWaitCommand* iWaitCommand;
	CLoopCommand* iLoopCommand;
	CReadPCTCommand* iReadCommand;
	CScriptCommandBase* iCommands[KNumCommands];
	CScriptCommandBase* iLastCommand;
	CScriptReader* iScriptReader;
	CScriptVarMan* iVarMan;
	CScriptLabelMan* iLabelMan;
	CScriptCharacterConverter* iCharConv;
	CNetDialScript* iScript;
	TBool iInitializationFlag;
	enum {
		EFirstWaitCommand,
		ESendingCR,
		ESecondWaitCommand,
		EDeactivated
		} iServerRetransmitLoginPromptStatus;
	CLabelSearchArray* iDummySearchArray;
	TReal iRetransmitLoginTimeOut;
	enum {
		EIdle,
		ERunning
		} iScriptStatus;
	TBool iReadFound;
	TInt iReadReq;
	TBool iReadFromPctPending;
	TBuf<KMaxReadBufferLength> iReadBuffer;		///< This is a straight ASCII or UNICODE buffer - no fancy character sets!
	TBuf8<KTxBufferSize> iDataToWrite;			///< This is an 8 bit buffer with characters converted appropriately
//	Need for test dialog server only
	TBuf<KCommsDbSvrMaxUserIdPassLength> iLoginName;
	TBuf<KCommsDbSvrMaxUserIdPassLength> iLoginPass;
	};

#include "SSCREXEC.INL"

#endif
