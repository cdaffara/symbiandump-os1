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

#ifndef __CMDSTARTERBASE_H__
#define __CMDSTARTERBASE_H__

#include "ssmdeferrablecommand.h"

class TSsmCommandParameters;
class CSsmStartupProperties;

/**
Base class for start app / process commands
@internalComponent
@released
*/
NONSHARABLE_CLASS(CCmdStarterBase) : public CSsmDeferrableCommand
	{
public:
	~CCmdStarterBase();

	// from MSsmCommand
	void Execute(TRequestStatus& aStatus);
	void Release();
	void ExecuteCancel();
	TSsmCommandType Type() const;
	void ExternalizeL(RWriteStream& aWriteStream) const;
	inline CSsmStartupProperties* AppInfo()const {return iAppInfo;}
	
protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	// construct this object with app info
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL(const CSsmStartupProperties *aAppInfo);
	// construct this object with app info without validate only used by CLE
	void ConstructL(const CSsmStartupProperties *aAppInfo, TBool aDummy);
	
	CCmdStarterBase(TSsmCommandType aCommandType);
	CCmdStarterBase(TSsmCommandType aCommandType, TCmdErrorSeverity aSeverity);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdStarterBase(TSsmCommandType aCommandType, TCmdErrorSeverity aSeverity, const TUint16 aPriority);
#endif
	void InternalizeL(RReadStream& aReadStream);
	void ValidateL();

private:
	// command type can be ESsmCmdStartProcess / ESsmCmdStartApp
	TSsmCommandType iCommandType;
	// Startup properties to start an application / process
	CSsmStartupProperties* iAppInfo;
	// The index integer returned by StartL() of Startsafe. It is preserved and passed in the case of a call to StartCancel()
	TInt iCallerIndex;
	// Handle to the created process
	RProcess iProcess;
	};

#endif // __CMDSTARTERBASE_H__
