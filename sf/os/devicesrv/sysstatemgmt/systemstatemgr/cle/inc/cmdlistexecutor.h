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
// ssmcmdlistexecutor.h
// 
//

#ifndef __SSMCMDLISTEXECUTOR_H__
#define __SSMCMDLISTEXECUTOR_H__

#include <e32base.h>
#include <e32debug.h>

#include "cleconst.h"
#include <ssm/ssmcommandlist.h>
#include "cmdlistexecobs.h"
#include "ssmdebug.h"

NONSHARABLE_CLASS( CSsmCmdListExecutor ) : public CActive
	{

public:
	static CSsmCmdListExecutor* NewL( MCommandListExecutionObserver& aCmdListObserver, TInt aSessionIndex );
	static CSsmCmdListExecutor* NewLC( MCommandListExecutionObserver& aCmdListObserver, TInt aSessionIndex );
	~CSsmCmdListExecutor();
	
	void StartL( CSsmCommandList* aCmdList );
	
protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();

private:
	CSsmCmdListExecutor( MCommandListExecutionObserver& aCmdListObserver, TInt aSessionIndex );
	void ConstructL();
	void DoExecute();
private:
	CSsmCommandList* iCommandList; // We assume ownership.
	TInt iCommandIndex;
	TInt iCommandCount; 
	TBool iRunning;
	MCommandListExecutionObserver& iObserver;
	TInt iSessionIndex;
	RTimer iTimer;
	TBool iDoDelayToggle;
	TInt iInterCmdDelay;
	
	};



#endif // __SSMCMDLISTEXECUTOR_H__





