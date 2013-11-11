/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_manSuiteDefs.h
*/
#if (!defined __TE_MAN_SUITEDEFS_H__)
#define __TE_MAN_SUITEDEFS_H__

#include "lbsagpsngmsgs.h"
#include "lbscommoninternaldatatypes.h"

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_manSuiteString,"TheString");
_LIT(KTe_manSuiteInt,"TheInt");
_LIT(KTe_manSuiteBool,"TheBool");

// For test step panics
_LIT(KTe_manSuitePanic,"Te_manSuite");

const TLbsNetSessionIdInt KDummySessionId;
const TLbsNetSessionCompleteAgpsMsg KSessionCompleteKErrNone(KDummySessionId, KErrNone);
#endif
