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

/**
 @file
*/

#include "Te_SmsTestSteps.h"


CDeleteSARStores::CDeleteSARStores()
    {
    iPartOfMultiStepTestCase = ETrue;
    }


/**
	Deletes the SMS and WAP segmentation and reassembly stores
*/
TVerdict CDeleteSARStores::doTestStepL()
	{	
	
	INFO_PRINTF1(_L("Deleting stores"));

	// delete segmentation and reassembly store files before the test
	_LIT(KReassemblyStoreName,"C:\\Private\\101F7989\\sms\\smsreast.dat");
	_LIT(KSegmentationStoreName,"C:\\Private\\101F7989\\sms\\smssegst.dat");
	_LIT(KWapReassemblyStoreName,"C:\\Private\\101F7989\\sms\\wapreast.dat");

	//Delete ... wap reasembly store
	TInt ret = iFs.Delete(KWapReassemblyStoreName);
	INFO_PRINTF2(_L("Wap stores %d"),ret );
	TEST(ret==KErrNone || ret==KErrNotFound);

	//Reassembly Store
	ret = iFs.Delete(KReassemblyStoreName);
	INFO_PRINTF2(_L("SMS reasembly store %d"),ret );
	TEST(ret==KErrNone || ret==KErrNotFound);

	//Segmentation store	
	ret = iFs.Delete(KSegmentationStoreName);
	INFO_PRINTF2(_L("SMS segmentation store %d"),ret );
	TEST(ret==KErrNone || ret==KErrNotFound);
	
	return TestStepResult();
	}
	
	
