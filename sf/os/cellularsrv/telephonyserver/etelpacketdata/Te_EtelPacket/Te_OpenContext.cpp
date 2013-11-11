// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Symbian OS includes
// 
//

#include "Te_EtelPacketTestSteps.h"

#include <etelpckt.h>
#include <pcktcs.h>

#include "Te_EtelPacketTestStepBase.h"

#include "testdef.h"

#include "Te_etelpckt_defs.h"

CTestOpenContext::CTestOpenContext(RTelServer& aTelServer, RPacketService& aPacketService, RPacketContext& aContext, TName &aContextName)
	:iTelServer(aTelServer), iPacketService(aPacketService), iContext(aContext), iContextName(aContextName)
	{
	}

// Do Test step
enum TVerdict CTestOpenContext::doTestStepL()
	{
	TBool isNewContext;
	GetBoolFromConfig(ConfigSection(), _L("new"), isNewContext);
	TInt ret;
	
	//Check if a new context is needed or not
	if(isNewContext)
		{
		ret = iContext.OpenNewContext(iPacketService, iContextName);	
		}
	else
		{
		iTelServer.__DbgMarkHeap();
		RPacketContext tempContext;
		ret = tempContext.OpenExistingContext(iPacketService, iContextName);	
		tempContext.Close();
		iTelServer.__DbgMarkEnd(0);
		}

	if(ret!=KErrNone)
		{
		SetTestStepResult(EFail);		
		}
	else
		{
		SetTestStepResult(EPass);		
		}

	return TestStepResult();

	}
