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
//

#include "te_mmnotifyuepositioningreset.h"
#include <e32test.h>
#include "TE_EtelMMTestStepBase.h"
#include "testdef.h"


/**
 * Client makes its own location request to the network and it waits for the notification of incoming A-GPS assistance data from the TSY. 
 * Location information is computed using the A-GPS assistance data received from the network 
 * and sends the location information report to the network/TSY
 * On completion of the MOLR request incoming location estimate and velocity estimate is sent to the client
 */

enum TVerdict CTestNotifyUePositioningReset::doTestStepL()
	{
	TRequestStatus status;
	RMobileLocationServices::TUePosTechnology posTech;
	iLocationSvc.NotifyResetUePositioningInformation(status,posTech);
	
	User::WaitForRequest(status);
	TEST(status == KErrNone);
	TEST(posTech == DMMTSY_PHONE_LCS_UE_POS_TECH);
	return TestStepResult();
	}
