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
 @file Te_lbsnetgatewaySuiteDefs.h
*/
#if (!defined __TE_LBSNETGATEWAY_SUITEDEFS_H__)
#define __TE_LBSNETGATEWAY_SUITEDEFS_H__

#include "lbsprocessuiddefs.h"

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_lbsnetgatewaySuiteString,"TheString");
_LIT(KTe_lbsnetgatewaySuiteInt,"TheInt");
_LIT(KTe_lbsnetgatewaySuiteBool,"TheBool");

// For test step panics
_LIT(KTe_lbsnetgatewaySuitePanic,"Te_lbsnetgatewaySuite");

// Test server Uid
const TUint KTe_lbsnetgatewaySuiteUidValue = 0x1028224E;
const TUid KTe_lbsnetgatewaySuiteUid = { KTe_lbsnetgatewaySuiteUidValue };

#endif
