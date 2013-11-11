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

#ifndef __CMDAMASTARTER_H__
#define __CMDAMASTARTER_H__

#include "ssmdeferrablecommand.h"

class RReadStream;
class TSsmCommandParameters;
class CSsmLoadAmaStart;

/*
Provides mechanism for starting AMAs from a DSC
@internalComponent
@released
*/
NONSHARABLE_CLASS(CCmdAmaStarter) : public CSsmDeferrableCommand
	{
public:
	static CCmdAmaStarter* NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TUid aDscId);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	static CCmdAmaStarter* NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TUid aDscId, const TUint16 aPriority);
#endif
	static CCmdAmaStarter* NewL(RReadStream& aReadStream);
	static CCmdAmaStarter* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdAmaStarter* NewLC(const CCmdAmaStarter& aCmdAmaStarter);

	~CCmdAmaStarter();

	// from MSsmCommand
	void Execute(TRequestStatus& aStatus);
	void Release();		
	void ExecuteCancel();
	TSsmCommandType Type() const;
	void ExternalizeL(RWriteStream& aWriteStream) const;
	inline TUid DscId() const {return iDscId;}

protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdAmaStarter(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUid& aDscId, const TUint16 aPriority);
#endif
	CCmdAmaStarter(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUid& aDscId);
	CCmdAmaStarter();
	void ConstructL(const CCmdAmaStarter& aCmdAmaStarter);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL();
	void InternalizeL(RReadStream& aReadStream);
	void ValidateL();//only used at the time of commandlist creation by SSM
	static TInt MaxSupportedVersion();
	void LoadAmaStartLibraryL();
	
private:
	TUid iDscId;
	CSsmLoadAmaStart* iAmaStart;
	RLibrary iAmaStartLib;
	};

#endif // __CMDAMASTARTER_H__
