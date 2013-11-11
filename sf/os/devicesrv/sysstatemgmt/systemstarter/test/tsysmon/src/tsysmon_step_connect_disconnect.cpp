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

#include "tsysmon_step_connect_disconnect.h"
#include "tsysmon_server_defs.h"

CConnectDisconnectStep::~CConnectDisconnectStep()
	{
	}

CConnectDisconnectStep::CConnectDisconnectStep()
	{
	SetTestStepName(KConnectDisconnectStep);
	}

TVerdict CConnectDisconnectStep::doTestStepL()
	{
	RSysMonSession sysmon;	
	TRAPD(err,sysmon.OpenL());
	TEST( err == KErrNone );
	sysmon.Close();
	return TestStepResult();
	}

