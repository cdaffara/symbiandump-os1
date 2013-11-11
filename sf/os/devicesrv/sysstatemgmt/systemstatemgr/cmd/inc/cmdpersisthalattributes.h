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

#ifndef __CMDPERSISTHALATTRIBUTES_H__
#define __CMDPERSISTHALATTRIBUTES_H__

#include "ssmcommandbase.h"

class TSsmCommandParameters;
class RReadStream;

/**
Built-in command for persisting the HAL attributes
@internalComponent
@released
@see BaflUtils
*/
NONSHARABLE_CLASS (CCmdPersistHalAttributes) : public CSsmCommandBase
	{
public: 
	static CCmdPersistHalAttributes* NewL(RReadStream& aReadStream);
	static CCmdPersistHalAttributes* NewL(TCmdErrorSeverity aSeverity);
	static CCmdPersistHalAttributes* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdPersistHalAttributes* NewLC(const CCmdPersistHalAttributes& aCmdPersistHalAttributes);
	~CCmdPersistHalAttributes();

	// from MSsmCommand
	TSsmCommandType Type() const;
	void ExternalizeL(RWriteStream& aWriteStream) const;
	void Execute(TRequestStatus& aStatus);
	void ExecuteCancel();
	void Release();
	
protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:	
	CCmdPersistHalAttributes();
	CCmdPersistHalAttributes(TCmdErrorSeverity aSeverity);
	void InternalizeL(RReadStream& aReadStream);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL(const CCmdPersistHalAttributes& aCmdPersistHalAttributes);
	static TInt MaxSupportedVersion();
	};

#endif // __CMDPERSISTHALATTRIBUTES_H__
