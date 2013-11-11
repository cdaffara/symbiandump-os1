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

#ifndef __SSMCOMMANDUTILPROVIDER__
#define __SSMCOMMANDUTILPROVIDER__

#include <e32base.h>
#include <ssm/ssmstartsafe.h>
#include <ssm/ssmloadsysmon.h>
#include <f32file.h>

/**
Provides CSsmStartSafe and RSysMonSession objects
@internalComponent
@released
*/
NONSHARABLE_CLASS (CSsmCommandUtilProvider) : public CBase
	{
public:
	IMPORT_C static CSsmCommandUtilProvider* NewL();
	IMPORT_C ~CSsmCommandUtilProvider();

	MSsmLoadSysMon& SysMonSessionL();
	RFs& RfsL();
	CSsmStartSafe& StartSafe();

private:
	CSsmCommandUtilProvider();
	void ConstructL();
	void LoadMonitorProxyLibL();

private:
	CSsmStartSafe* iStartSafe;
	MSsmLoadSysMon* iSysMonSession;
	RFs iFs;
	RLibrary iSysMonProxyLib;
	};

#endif //__SSMCOMMANDUTILPROVIDER__
