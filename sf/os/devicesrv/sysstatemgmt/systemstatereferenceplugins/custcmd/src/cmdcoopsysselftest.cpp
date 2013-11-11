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
 @internalComponent
 @released
*/

#include "cmdcoopsysselftest.h"

CCustomCmdCoopSysSelfTest* CCustomCmdCoopSysSelfTest::NewL()
	{
	CCustomCmdCoopSysSelfTest* self = new (ELeave) CCustomCmdCoopSysSelfTest();
	return self;
	}

CCustomCmdCoopSysSelfTest::CCustomCmdCoopSysSelfTest()
	{
	}

CCustomCmdCoopSysSelfTest::~CCustomCmdCoopSysSelfTest()
	{
	}

TInt CCustomCmdCoopSysSelfTest::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	//Connect RSsmStateAdaptation
	return iSsmStateAdaptation.Connect();
	}

void CCustomCmdCoopSysSelfTest::Close()
	{
	//Cancel the request if there is any
	iSsmStateAdaptation.RequestCancel();

	//Close RSsmStateAdaptation
	iSsmStateAdaptation.Close();
	}

void CCustomCmdCoopSysSelfTest::Release()
	{
	delete this;
	}

void CCustomCmdCoopSysSelfTest::Execute(const TDesC8& /*aParams*/, TRequestStatus& aStatus)
	{
	//Request coop system to perform self test
	iSsmStateAdaptation.RequestCoopSysSelfTest(aStatus);
	}

void CCustomCmdCoopSysSelfTest::ExecuteCancel()
	{
	//Cancel the request if there is any
	iSsmStateAdaptation.RequestCancel();
	}
