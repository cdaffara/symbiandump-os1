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

#ifndef __CMDSETPANDSKEY_H__
#define __CMDSETPANDSKEY_H__

#include "ssmcommandbase.h"

class RReadStream;
class TSsmCommandParameters;

/**
Built-in command to set a publish and subscribe key.
@internalComponent
@released
*/
NONSHARABLE_CLASS (CCmdSetPAndSKey) : public CSsmCommandBase
	{
public:
	static CCmdSetPAndSKey* NewL(TCmdErrorSeverity aSeverity, const TUid& aCategory, TUint aKey, TInt aValue);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	static CCmdSetPAndSKey* NewL(TCmdErrorSeverity aSeverity, const TUid& aCategory, TUint aKey, TInt aValue, const TUint16 aPriority);
#endif
	static CCmdSetPAndSKey* NewL(RReadStream& aReadStream);
	static CCmdSetPAndSKey* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdSetPAndSKey* NewLC(const CCmdSetPAndSKey& aCmdSetPAndSKey);
	
	~CCmdSetPAndSKey();

	// from MSsmCommand
	void Execute(TRequestStatus& aStatus);
	void Release();		
	void ExecuteCancel();
	TSsmCommandType Type() const;
	void ExternalizeL(RWriteStream& aWriteStream) const;
	inline TUid Category() const { return iCategory;}
	inline TUint Key() const { return iKey;}
	inline TInt Value() const { return iValue;}

protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	CCmdSetPAndSKey();
	CCmdSetPAndSKey(TCmdErrorSeverity aSeverity, const TUid& aCategory, TUint aKey, TInt aValue);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdSetPAndSKey(TCmdErrorSeverity aSeverity, const TUid& aCategory, TUint aKey, TInt aValue, const TUint16 aPriority);
#endif
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL(const CCmdSetPAndSKey& aCmdSetPAndSKey);
	void InternalizeL(RReadStream& aReadStream);
	static TInt MaxSupportedVersion();

private:
	TUid iCategory;
	TUint iKey;
	TInt iValue;
	};

#endif // __CMDSETPANDSKEY_H__
