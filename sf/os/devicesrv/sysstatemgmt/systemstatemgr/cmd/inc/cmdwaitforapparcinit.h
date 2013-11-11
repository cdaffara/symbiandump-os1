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

#ifndef __CMDWAITFORAPPARCINIT_H__
#define __CMDWAITFORAPPARCINIT_H__

#include "ssmcommandbase.h"

class RReadStream;
class TSsmCommandParameters;

/**
Built-in command to wait for apparc init. This command waits for applist population, 
and 'apsexe.exe' should be started before running this command.

@internalComponent
@released
*/
NONSHARABLE_CLASS(CCmdWaitForApparcInit) : public CSsmCommandBase 
	{
public: 
	static CCmdWaitForApparcInit* NewL(RReadStream& aReadStream);
	static CCmdWaitForApparcInit* NewL(TCmdErrorSeverity aSeverity);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	static CCmdWaitForApparcInit* NewL(TCmdErrorSeverity aSeverity, const TUint16 aPriority);
#endif
	static CCmdWaitForApparcInit* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdWaitForApparcInit* NewLC(const CCmdWaitForApparcInit& aCmdWaitForApparcInit, CSsmCommandUtilProvider* aUtilProvider);
	~CCmdWaitForApparcInit();

	// from MSsmCommand
	void Execute(TRequestStatus& aStatus);
	void Release();
	void ExecuteCancel();
	TSsmCommandType Type() const;
	void ExternalizeL(RWriteStream& aWriteStream) const;

protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdWaitForApparcInit(TCmdErrorSeverity aSeverity, const TUint16 aPriority);
#endif
	CCmdWaitForApparcInit(TCmdErrorSeverity aSeverity);
	CCmdWaitForApparcInit();
	void InternalizeL(RReadStream& aReadStream);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL(const CCmdWaitForApparcInit& aCmdWaitForApparcInit, CSsmCommandUtilProvider* aUtilProvider);
	static TInt MaxSupportedVersion();
	};

#endif // __CMDWAITFORAPPARCINIT_H__
