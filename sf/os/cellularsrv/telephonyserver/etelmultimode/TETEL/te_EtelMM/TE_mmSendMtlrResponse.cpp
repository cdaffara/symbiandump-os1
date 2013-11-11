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
//

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmSendMtlrResponse.h"
#include "testdef.h"

enum TVerdict CTestSendMtlrResponse::doTestStepL()
	{
	INFO_PRINTF1(_L("Successful MT-LR response"));

	TRequestStatus reqStatus;
	TInt mtlrResponse;
	GetIntFromConfig(ConfigSection(), _L("mtlrResponse"), mtlrResponse);

	if(mtlrResponse == 1)
		iMtlrResponse=DMMTSY_MTLR_RESPONSE1;    
	
	if(mtlrResponse == 2)
		iMtlrResponse=DMMTSY_MTLR_RESPONSE2;    
	
    iLocationSvc.SendMtlrResponse(reqStatus,iMtlrResponse);
    User::WaitForRequest(reqStatus);
    TEST(reqStatus == KErrNone);
 	INFO_PRINTF2(_L("Test %d - RMobileLocationServices::SendMtlrResponse passed"), iTestCount++);
	
	return TestStepResult();
	}
	
