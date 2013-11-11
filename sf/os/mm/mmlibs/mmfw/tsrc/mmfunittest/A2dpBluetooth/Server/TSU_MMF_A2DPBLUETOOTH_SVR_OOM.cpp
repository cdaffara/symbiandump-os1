// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TSU_MMF_A2DPBLUETOOTH_SVR_OOM.h"


CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0400::CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0400()
	{
	iTestStepName = _L("MM-MMF-A2DPBLUETOOTH-SVR-U-0400-HP");
	}
 

/**
OOM test not applicable to a2dp bluetooth headset server client
*/
TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0400::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
	return verdict;
	}
