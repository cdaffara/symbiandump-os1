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

#ifndef __CMDPOWEROFF_H__
#define __CMDPOWEROFF_H__

#include <e32power.h>
#include "ssmcommandbase.h"

#ifdef TEST_SSM_GRACEFUL_SHUTDOWN	// for testing purpose
#include <e32property.h>
#endif
	
class TSsmCommandParameters;
class RReadStream;

/**
Built-in command for powering down the device
@internalComponent
@released
@see TPowerState
*/
NONSHARABLE_CLASS (CCmdPowerOff) : public CSsmCommandBase
	{
public: 
	static CCmdPowerOff* NewL(RReadStream& aReadStream);
	static CCmdPowerOff* NewL(TCmdErrorSeverity aSeverity, TPowerState aPowerEvent);
	static CCmdPowerOff* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdPowerOff* NewLC(const CCmdPowerOff& aCmdPowerOff);
	~CCmdPowerOff();

	// from MSsmCommand
	TSsmCommandType Type() const;
	void ExternalizeL(RWriteStream& aWriteStream) const;
	void Execute(TRequestStatus& aStatus);
	void ExecuteCancel();
	void Release();
	//returns the current power state
	inline TPowerState PowerStateInfo()const {return iState;}
	
protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:	
	CCmdPowerOff();
	CCmdPowerOff(TCmdErrorSeverity aSeverity, TPowerState aState);
	void InternalizeL(RReadStream& aReadStream);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL(const CCmdPowerOff& aCmdPowerOff);
	void ConstructL();
	void ValidateL();
	static TInt MaxSupportedVersion();

private:
	TPowerState iState;		// system-wide power state
public:
#ifdef TEST_SSM_GRACEFUL_SHUTDOWN	// for testing purpose
	TUint iTestKey;
	TUid iTestCategory;
#endif
	};

#endif // __CMDPOWEROFF_H__
