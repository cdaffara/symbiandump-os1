// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains definition of CTestStepIPC which is the base class 
// for all the IPC Test Step classes  
// 
//

// EPOC includes
#include <e32base.h>
#include <es_sock.h>
#include <test/es_dummy.h>

// Test system includes

#include "TestStepIPC.h"

// constructor
CTestStepIPC::CTestStepIPC() 
	{
	iProt = _L("IPC Stream");
	}

// destructor
CTestStepIPC::~CTestStepIPC()
	{

	}

enum TVerdict CTestStepIPC::doTestStepL()
	{
	
	TRAPD(trapRet,enum TVerdict stepRet = InternalDoTestStepL();\
		if (stepRet !=EPass)\
			{\
			SetTestStepResult(stepRet);\
			}\
			)
			
			if (trapRet != KErrNone)
				{
				SetTestStepResult(EFail);
				return EFail;
				}
			return EPass;
	};

TInt CTestStepIPC::OptimalConnect(RSocketServ& aSrv)
	{
    TSessionPref pref;
    pref.iAddrFamily = KDummyAddrFamily;
    pref.iProtocol = KDummyOne;
	return aSrv.Connect(pref);
	}	
	


