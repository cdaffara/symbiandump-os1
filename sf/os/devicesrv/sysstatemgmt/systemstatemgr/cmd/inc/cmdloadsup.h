// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CMDLOADSUP_H__
#define __CMDLOADSUP_H__

#include <ssm/ssmsupinfo.h>
#include <ssm/ssmsuscli.h>
#include "ssmdeferrablecommand.h"

class TSsmCommandParameters;
class RReadStream;

/**
Built-in command for loading the SSMA Utility Plugin
@internalComponent
@released
@see RSsmSusCli
@see TSsmSupInfo
*/
NONSHARABLE_CLASS (CCmdLoadSup) : public CSsmDeferrableCommand
	{
public: 
	static CCmdLoadSup* NewL(RReadStream& aReadStream);
	static CCmdLoadSup* NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	static CCmdLoadSup* NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo, const TUint16 aPriority);
#endif
	static CCmdLoadSup* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdLoadSup* NewLC(const CCmdLoadSup& aCmdLoadSup);
	~CCmdLoadSup();

	// from MSsmCommand
	TSsmCommandType Type() const;
	void ExternalizeL(RWriteStream& aWriteStream) const;
	void Execute(TRequestStatus& aStatus);
	void ExecuteCancel();
	void Release();
	inline TInt16 Retries() const {return iRetries;}
	inline TSsmSupInfo SsmSupInfo() const {return iSupInfo;}
	
protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:	
	CCmdLoadSup();
	CCmdLoadSup(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdLoadSup(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TUint16 aPriority);
#endif
	void InternalizeL(RReadStream& aReadStream);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL(const TSsmSupInfo& aInfo);
	void ConstructL(const CCmdLoadSup& aCmdLoadSup);
	void ValidateL();
	static TInt MaxSupportedVersion();
	TInt ConnectAndLoadSup();

private:
	RSsmSusCli iSusCli;					// interface to SSMA Utility Server
	TSsmSupInfo iSupInfo;				// plugin info
 	TPckgC<TSsmSupInfo>* iSupInfoBuf;	// descriptor wrapper for plugin info
 	TInt16 iRetries;					// number of retries
	TInt16 iAttempts;					// count of unsuccessful attempts
	TBool iConnected;					// whether connected to service provider or not
	};

#endif // __CMDLOADSUP_H__
