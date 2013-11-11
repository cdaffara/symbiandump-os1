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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code   
*/

#ifndef __TLOADSYSMON_H__
#define __TLOADSYSMON_H__

#include <e32std.h>
#include <ssm/ssmstartupproperties.h>
#include <ssm/ssmloadsysmon.h>
#include <startupproperties.h>

class CTLoadSysMon : public CBase
	{
public:
	static CTLoadSysMon* NewLC();
	~CTLoadSysMon();
	void TestForSysMonFuncUsingSsmStartupPropL(const CSsmStartupProperties& aSsmStartupPropForSelfMonitor, const CSsmStartupProperties& aSsmStartupPropForOtherProcMonitor);
	void TestForSysMonFuncUsingStartupPropL(const CStartupProperties& aStartupPropForSelfMonitor, const CStartupProperties& aStartupPropForOtherProcMonitor);

private:
	void ResumeL(RProcess& aProcess);
	CTLoadSysMon();
	void ConstructL();

private:
	RLibrary iMonitorProxyLib;
	MSsmLoadSysMon* iSysMonCli;
	};

#endif

