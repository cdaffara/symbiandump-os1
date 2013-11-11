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
// functional unit of the SAT.
//



/**
 @file The TEFUnit header file which tests the MultimediaMsg
*/

#ifndef CCSATMULTIMEDIAMSGFU_H
#define CCSATMULTIMEDIAMSGFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatMultimediaMsgFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyRetrieveMultimediaMsgPCmd0001L();
	void TestNotifySubmitMultimediaMsgPCmd0001L();
	void TestNotifyDisplayMultimediaMsgPCmd0001L();

	}; // class CCSatMultimediaMsgFU

#endif // CCSATMULTIMEDIAMSGFU_H

