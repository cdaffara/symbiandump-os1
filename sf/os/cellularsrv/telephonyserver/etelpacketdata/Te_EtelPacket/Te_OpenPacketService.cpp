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

CTestOpenPacketService::CTestOpenPacketService(RPacketService& aPacketService, RPhone& aPhone):iPacketService(aPacketService),iPhone(aPhone)
	{
	}

// Do Test step
enum TVerdict CTestOpenPacketService::doTestStepL()
	{
	TInt ret=iPacketService.Open(iPhone);
	
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
