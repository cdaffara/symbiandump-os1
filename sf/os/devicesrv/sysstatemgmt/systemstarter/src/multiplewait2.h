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

#if !defined(__MULTIPLE_WAIT2_H__)
#define __MULTIPLE_WAIT2_H__

#include <e32base.h>
#include "StartupStateInfo.h"

class CAppStarter;
class CStartupProperties;
class CStartupUtilProvider;

/** A startup command object responsible for checking that all deferred 
wait commands have rendezvoused.  
@internalComponent
@deprecated
*/
class CMultipleWait : public CBase, public MStartupCommand
	{
public:
	static CMultipleWait* NewL(CommandList& aDeferredList, CStartupUtilProvider& aProvider);
	static CMultipleWait* NewLC(CommandList& aDeferredList, CStartupUtilProvider& aProvider);
	~CMultipleWait();
	void Execute(TRequestStatus& aStatus); 	// from MStartupCommand	
	void Release();

 	void SetTimeout(TInt aTimeout);
	void SetFailOnError(TBool aFail);	
	TInt Timeout() const;
	TBool FailOnError() const;
	
private:
	CMultipleWait(CStartupUtilProvider& aProvider);
	void ConstructL(CommandList& aDeferredList);

	void CheckCommandsAndIgnore(TRequestStatus& aTimerStatus);
	void CheckCommandsForRestart(TRequestStatus& aTimerStatus);
	void RegisterMonitoringL(CAppStarter& aStartCommand);
	void RestartSystem(const CStartupProperties& aStartupProperties);
	
private:
	CStartupUtilProvider& iProvider;
 	CommandList iDeferredList;   	
 	TInt iTimeout;
	TBool iFailOnError;
	};

#endif	// __MULTIPLE_WAIT2_H__
