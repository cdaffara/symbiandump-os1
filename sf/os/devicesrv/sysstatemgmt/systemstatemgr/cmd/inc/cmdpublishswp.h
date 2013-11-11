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

#ifndef __CMDPUBLISHSWP_H__
#define __CMDPUBLISHSWP_H__

#include "ssmdeferrablecommand.h"
#include <ssm/ssmswp.h>

class TSsmCommandParameters;
class RReadStream;

/**
Built-in command for publishing a system-wide property
@internalComponent
@released
@see TSsmSwp
*/
NONSHARABLE_CLASS (CCmdPublishSwp) : public CSsmDeferrableCommand
	{
public: 
	static CCmdPublishSwp* NewL(RReadStream& aReadStream);
	static CCmdPublishSwp* NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwp);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	static CCmdPublishSwp* NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwp, const TUint16 aPriority);
#endif
	static CCmdPublishSwp* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdPublishSwp* NewLC(const CCmdPublishSwp& aCmdPublishSwp);
	~CCmdPublishSwp();

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
	TInt RunError(TInt aError);

private:	
	CCmdPublishSwp();
	CCmdPublishSwp(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdPublishSwp(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo, const TUint16 aPriority);
#endif

	void InternalizeL(RReadStream& aReadStream);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(const CCmdPublishSwp& aCmdPublishSwp);
	void ConstructL();
	static TInt MaxSupportedVersion();

private:
	TSsmSwp iSwpInfo;			// property info
	};

#endif // __CMDPUBLISHSWP_H__
