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

#ifndef __CMDREQSWPCHANGE_H__
#define __CMDREQSWPCHANGE_H__

#include "ssmdeferrablecommand.h"
#include <ssm/ssmstatemanager.h>

class TSsmCommandParameters;
class RReadStream;

/**
Built-in command for requesting a system-wide property change
@internalComponent
@released
@see RSsmStateManager
@see TSsmSwp
*/
NONSHARABLE_CLASS (CCmdReqSwpChange) : public CSsmDeferrableCommand
	{
public: 
	static CCmdReqSwpChange* NewL(RReadStream& aReadStream);
	static CCmdReqSwpChange* NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	static CCmdReqSwpChange* NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo, const TUint16 aPriority);
#endif
	static CCmdReqSwpChange* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdReqSwpChange* NewLC(const CCmdReqSwpChange& aCmdReqSwpChange);
	~CCmdReqSwpChange();

	// from MSsmCommand
	TSsmCommandType Type() const;
	void ExternalizeL(RWriteStream& aWriteStream) const;
	void Execute(TRequestStatus& aStatus);
	void ExecuteCancel();
	void Release();
	inline TSsmSwp SsmSwpInfo() const {return iSwpInfo;}
	
protected:
	// from CActive
	void RunL();
	void DoCancel();

private:	
	CCmdReqSwpChange();
	CCmdReqSwpChange(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp aSwpInfo);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdReqSwpChange(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp aSwpInfo, const TUint16 aPriority);
#endif
	void InternalizeL(RReadStream& aReadStream);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL();
	void ConstructL(const CCmdReqSwpChange& aCmdReqSwpChange);
	static TInt MaxSupportedVersion();

private:
	RSsmStateManager iSsmStateManager;	// interface to State Management Server
	TSsmSwp iSwpInfo;					// property info
	};

#endif // __CMDREQSWPROPCHANGE_H__
