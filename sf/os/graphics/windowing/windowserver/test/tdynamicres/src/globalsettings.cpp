// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test class that allows a scriopt to set global values 
// 
//

#include "globalsettings.h"
#include <test/extendtef.h>

TGlobalSettings	settings_instance={0,0};
const TGlobalSettings& TGlobalSettings::Instance()
	{
	return settings_instance;
	}

// Create a suite of all the tests
CTestSuite* CGlobalSettings::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE_OPT(CGlobalSettings,NULL);

		ADD_TEST_STEP_PARAM_RANGE(SetScreenNoL,0,9);
		ADD_TEST_STEP_PARAM_BOOL(ExpectDisconnectedScreenL);

	END_SUITE;	
	}

void CGlobalSettings::SetScreenNoL(TInt aScreenNo)
	{
	RWsSession session;
	ASSERT_EQUALS(session.Connect(), KErrNone);
	TInt maxScreens=session.NumberOfScreens();
	
	if (maxScreens>aScreenNo)
		{
		INFO_PRINTF2(_L("New screen number for all output selected: %i"),aScreenNo);
			{
			CWsScreenDevice screen(session);
			screen.Construct(aScreenNo);
			screen.SetCurrentRotations(0,CFbsBitGc::TGraphicsOrientation(0));
			screen.SetScreenMode(0);
			screen.SetAppScreenMode(0);
			}
		settings_instance.iScreen=aScreenNo;
		session.Flush();
		session.Close();
		}
	else
		{
		INFO_PRINTF1(_L("**************************"));
		INFO_PRINTF1(_L(""));
		INFO_PRINTF3(_L("Can't set screen output to %i. Only got %i screens"),aScreenNo,maxScreens);
		INFO_PRINTF2(_L("Screen number for all output remains at: %i"),settings_instance.iScreen);

		INFO_PRINTF1(_L(""));
		INFO_PRINTF1(_L("**************************"));
		session.Flush();
		session.Close();
		User::Leave(KErrTEFUnitFail);
		}
	}

void CGlobalSettings::ExpectDisconnectedScreenL(TBool aValue)
	{
	settings_instance.iDisconnected=aValue;
	}
