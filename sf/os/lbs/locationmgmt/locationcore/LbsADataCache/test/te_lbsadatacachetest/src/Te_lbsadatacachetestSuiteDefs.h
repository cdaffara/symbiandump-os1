/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#if (!defined __TE_LBSADATACACHETEST_SUITEDEFS_H__)
#define __TE_LBSADATACACHETEST_SUITEDEFS_H__

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_lbsadatacachetestSuiteString,"TheString");
_LIT(KTe_lbsadatacachetestSuiteInt,"TheInt");
_LIT(KTe_lbsadatacachetestSuiteBool,"TheBool");

// For test step panics
_LIT(KTe_lbsadatacachetestSuitePanic,"Te_lbsadatacachetestSuite");

// Test server UID
const TUint KTe_lbsadatacachetestSuiteUidValue = 0x102869D4;
const TUid KTe_lbsadatacachetestSuiteUid = { KTe_lbsadatacachetestSuiteUidValue };

#endif
