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

#ifndef __CLESRV_H__
#define __CLESRV_H__

#include <e32base.h>
#include <s32mem.h>

#include "cmdlistexecobs.h"
#include "cmdlistexecutor.h"


_LIT( KCleSrvName, "!CleSrv" );


class CSsmCommandList;
class CSsmCmdListExecutor;



/**
@internalComponent
@released
*/
struct SCleSessionInfo
	{
	CSsmCmdListExecutor* iExecutor;
	RMessagePtr2 iMessagePtr;
	TBool iInUse;
	};


	
/**
 The Commamd-list executor server
 
@internalComponent
@released
*/
NONSHARABLE_CLASS( CCleServer ) : public CPolicyServer, public MCommandListExecutionObserver
	{

public:
	IMPORT_C static CCleServer* NewL(const TDesC& aServerName);
	IMPORT_C static CCleServer* NewLC(const TDesC& aServerName);
	IMPORT_C static TInt StartCleSrv(const TDesC& aServerName);
	~CCleServer();
	
	void ExecuteCommandListL( CSsmCommandList* aCommandList, const RMessage2& aMessage, const TInt aSessionIndex );
	void ExecuteCommandListCancel( TInt aSessionIndex );
	void RegisterSessionL( TInt& aSessionIndex );	
	void DeregisterSession( const TInt& aSessionIndex );
	TInt CloseNeverUnloadLibrariesL();
	void CleanHandlesFileL();

	// From MCommandListExecutionObserver
	void ListExecutionComplete( TInt aComplete, TInt aSessionIndex, TCmdErrorSeverity aSeverity);
	// From CServer2
	IMPORT_C CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;

private:	
	void ConstructL(const TDesC& aServerName);	
	CCleServer();
	static TInt CleSrvThreadFn( TAny* aAny );
private:
	TVersion iVersion;	
	RArray<SCleSessionInfo> iSessionInfoArray;
	TInt iSessionCount;
	};


#endif // __CLESRV_H__
