/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef __CMDCREATESWP_H__
#define __CMDCREATESWP_H__

#include <ssm/ssmswp.h>

#include "ssmcommandbase.h"

class RSsmStateManager;
class TSsmCommandParameters;
class RReadStream;

/**
Built-in command for creating a system-wide property
@internalComponent
@released
@see RSsmStateManager
@see TSsmSwp
*/
NONSHARABLE_CLASS (CCmdCreateSwp) : public CSsmCommandBase
	{
public: 
	static CCmdCreateSwp* NewL(RReadStream& aReadStream);
	static CCmdCreateSwp* NewL(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwpInfo, const TDesC& aFilename);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	static CCmdCreateSwp* NewL(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwpInfo, const TDesC& aFilename, const TUint16 aPriority);
#endif
	static CCmdCreateSwp* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdCreateSwp* NewLC(const CCmdCreateSwp& aCmdCreateSwp);
	~CCmdCreateSwp();

	// from MSsmCommand
	TSsmCommandType Type() const;
	void ExternalizeL(RWriteStream& aWriteStream) const;
	void Execute(TRequestStatus& aStatus);
	void ExecuteCancel();
	void Release();
	inline TSsmSwp SsmSwpInfo() const {return iSwpInfo;}
	inline const TDesC& FileName() const {return iFileName;}
	
protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:	
	CCmdCreateSwp();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdCreateSwp(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwpInfo, const TUint16 aPriority);
#endif
	CCmdCreateSwp(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwpInfo);
	void InternalizeL(RReadStream& aReadStream);
	void ConstructL(const TDesC& aFileName);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL(const CCmdCreateSwp& aCmdCreateSwp);
	void ValidateL();
	static TInt MaxSupportedVersion();

private:
	RSsmStateManager iSsm;		// interface to State Management Server
	TSsmSwp iSwpInfo;			// property info
	RBuf iFileName;			// policy file
	};

#endif // __CMDCREATESWP_H__
