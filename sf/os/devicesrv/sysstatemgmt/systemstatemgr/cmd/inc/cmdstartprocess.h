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

#ifndef __CMDSTARTPROCESS_H__
#define __CMDSTARTPROCESS_H__

#include "cmdstarterbase.h"

class RReadStream;
class CSsmStartupProperties;
class TSsmCommandParameters;

/**
Built-in command to start a process
@internalComponent
@released
*/
NONSHARABLE_CLASS(CCmdStartProcess) : public CCmdStarterBase
	{
public:
	static CCmdStartProcess* NewL(RReadStream& aReadStream);
	static CCmdStartProcess* NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	static CCmdStartProcess* NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo, const TUint16 aPriority);
#endif
	static CCmdStartProcess* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdStartProcess* NewLC(const CCmdStartProcess& aCmdStartProcess, CSsmCommandUtilProvider* aUtilProvider);
	~CCmdStartProcess();

private:
	CCmdStartProcess();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdStartProcess(TCmdErrorSeverity aSeverity, const TUint16 aPriority);
#endif
	CCmdStartProcess(TCmdErrorSeverity aSeverity);
	static TInt MaxSupportedVersion();
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(const CSsmStartupProperties *aAppInfo);
	void ConstructL(const CCmdStartProcess& aCmdStartProcess, CSsmCommandUtilProvider* aUtilProvider);
	};

#endif // __CMDSTARTPROCESS_H__
