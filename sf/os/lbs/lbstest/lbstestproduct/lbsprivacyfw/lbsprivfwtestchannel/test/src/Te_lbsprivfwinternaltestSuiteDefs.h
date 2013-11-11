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
 @file Te_lbsnetinternaltestSuiteDefs.h
*/
#if (!defined __TE_LBSNETINTERNALTEST_SUITEDEFS_H__)
#define __TE_LBSNETINTERNALTEST_SUITEDEFS_H__

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_lbsnetinternaltestSuiteString,"TheString");
_LIT(KTe_lbsnetinternaltestSuiteInt,"TheInt");
_LIT(KTe_lbsnetinternaltestSuiteBool,"TheBool");

// For test step panics
_LIT(KTe_lbsprivfwinternaltestSuitePanic,"Te_lbsnetinternaltestSuite");

// Test server UID
const TUint KTe_lbsnetinternaltestSuiteUidValue = 0x1028224C;
const TUid KTe_lbsnetinternaltestSuiteUid = { KTe_lbsnetinternaltestSuiteUidValue };

#endif
