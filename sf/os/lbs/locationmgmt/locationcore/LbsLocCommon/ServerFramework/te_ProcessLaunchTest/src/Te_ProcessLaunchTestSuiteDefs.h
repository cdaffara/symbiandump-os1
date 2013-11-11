/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* This file define all the common values thoughout your test project
* 
*
*/



/**
 @file Te_ProcessLaunchTestSuiteDefs.h
 @internalTechnology
*/
#if (!defined __TE_PROCESSLAUNCHTEST_SUITEDEFS_H__)
#define __TE_PROCESSLAUNCHTEST_SUITEDEFS_H__

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_ProcessLaunchTestSuiteString,"TheString");
_LIT(KTe_ProcessLaunchTestSuiteInt,"TheInt");
_LIT(KTe_ProcessLaunchTestSuiteBool,"TheBool");

// For test step panics
_LIT(KTe_ProcessLaunchTestSuitePanic,"Te_ProcessLaunchTestSuite");


_LIT(KServerName,"Te_ProcessLaunchTestSuite");

const TInt KProcessLaunchTestUidValue = 0x10282244;
const TUid KProcessLaunchTestUid = {KProcessLaunchTestUidValue};
const TInt KServerFrameworkTestSuicideKey = 100;

#endif
