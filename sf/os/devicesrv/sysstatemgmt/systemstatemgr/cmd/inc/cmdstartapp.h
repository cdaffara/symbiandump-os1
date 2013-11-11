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

#ifndef __CMDSTARTAPP_H__
#define __CMDSTARTAPP_H__

#include "cmdstarterbase.h"

class RReadStream;
class TSsmCommandParameters;
class CSsmStartupProperties;

/**
Built-in command to start an application
@internalComponent
@released
*/
NONSHARABLE_CLASS(CCmdStartApp) : public CCmdStarterBase
	{
public: 
	static CCmdStartApp* NewL(RReadStream& aReadStream);
	static CCmdStartApp* NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	static CCmdStartApp* NewL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties *aAppInfo, const TUint16 aPriority);
#endif
	static CCmdStartApp* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdStartApp* NewLC(const CCmdStartApp& aCmdStartApp, CSsmCommandUtilProvider* aUtilProvider);
	~CCmdStartApp();
	
private:
	CCmdStartApp();
	CCmdStartApp(TCmdErrorSeverity aSeverity);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdStartApp(TCmdErrorSeverity aSeverity, const TUint16 aPriority);
#endif
	static TInt MaxSupportedVersion();
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(const CSsmStartupProperties *aAppInfo);
	void ConstructL(const CCmdStartApp& aCmdStartApp, CSsmCommandUtilProvider* aUtilProvider);
	};

#endif // __CMDSTARTAPP_H__
