// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined (__APPSTARTER2_H__)
#define __APPSTARTER2_H__

#include <e32base.h>
#include "StartupCommand.h"
#include "startuputilprovider.h"

/** A startup command object responsible for starting an application
@internalComponent
@deprecated
*/
class CAppStarter : public CBase, public MStartupCommand
	{
public:
	static CAppStarter* NewL(CStartupProperties* aAppInfo, CStartupUtilProvider& aProvider);
	static CAppStarter* NewLC(CStartupProperties* aAppInfo, CStartupUtilProvider& aProvider);
	~CAppStarter();
	
	// from MStartupCommand
	void Execute(TRequestStatus& aCallerStatus);
	void Release();
	
	CStartupProperties& AppInfo();
	RProcess& Process(); 
	TRequestStatus& CommandStatus();

private:
	CAppStarter(CStartupProperties* aAppInfo, CStartupUtilProvider& aProvider);

private:	
	CStartupUtilProvider& iProvider;
	CStartupProperties *iAppInfo;
	
	RProcess iProcess;
	TRequestStatus iCommandStatus;
	};

#endif // __APPSTARTER2_H__
