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


#ifndef __TEMERGENCYCALLRF_ADAPTATIONPLUGIN_STEP_H__
#define __TEMERGENCYCALLRF_ADAPTATIONPLUGIN_STEP_H__


#include "emergencycallrfadaptationref.h"
#include "tadaptation_step_base.h"
#include <ssm/ssmstate.h>
#include <ssm/ssmstates.hrh>

#include <ssm/ssmadaptationcli.h>

_LIT(KTCTestEmergencyCallRfAdaptationPlugin, "CTestEmergencyCallRfAdaptationPlugin");


class CTestEmergencyCallRfAdaptationPlugin : public CAdaptationTestBase
	{
public:
	CTestEmergencyCallRfAdaptationPlugin();
	~CTestEmergencyCallRfAdaptationPlugin();

	//from CAdaptationTestBase
	TVerdict doTestStepL();

private:


	void TestActivateRfForEmergencyCall();
	void TestDeactivateRfForEmergencyCall();
	void TestCancel();

private:
	TRequestStatus iStatus;
	RSsmEmergencyCallRfAdaptation iSsmEmergencyCallRfAdaptation;
	};

#endif	// __TEMERGENCYCALLRF_ADAPTATIONPLUGIN_STEP_H__
