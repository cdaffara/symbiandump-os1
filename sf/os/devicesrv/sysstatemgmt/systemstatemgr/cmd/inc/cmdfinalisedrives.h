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

#ifndef __CMDFINALISEDRIVES_H__
#define __CMDFINALISEDRIVES_H__

#include "ssmcommandbase.h"

class TSsmCommandParameters;
class RReadStream;

/**
Built-in command for finalising the drives
@internalComponent
@released
@see RFs
*/
NONSHARABLE_CLASS (CCmdFinaliseDrives) : public CSsmCommandBase
	{
public:
	static CCmdFinaliseDrives* NewL(RReadStream& aReadStream);
	static CCmdFinaliseDrives* NewL(TCmdErrorSeverity aSeverity);
	static CCmdFinaliseDrives* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdFinaliseDrives* NewLC(const CCmdFinaliseDrives& aCmdFinaliseDrives);
	~CCmdFinaliseDrives();

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
	CCmdFinaliseDrives();
	CCmdFinaliseDrives(TCmdErrorSeverity aSeverity);
	void InternalizeL(RReadStream& aReadStream);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL(const CCmdFinaliseDrives& aCmdFinaliseDrives);
	static TInt MaxSupportedVersion();

private:
    RFs iFs;				// handle to filesystem
	};

#endif // __CMDFINALISEDRIVES_H__
