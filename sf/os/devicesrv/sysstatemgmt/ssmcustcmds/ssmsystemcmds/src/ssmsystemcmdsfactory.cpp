/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of SsmSystemCmdsFactory class.
*
*/

#include "ssmsystemcmdsfactory.h"
#include "ssminitstartupreason.h"
#include "ssminitpskeys.h"
#include "ssminitcriticallevels.h"
#include "ssmwaitpskey.h"
#include "ssmsashutdown.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// SsmSystemCmdsFactory::InitPsKeysNewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmCustomCommand* SsmSystemCmdsFactory::InitPsKeysNewL()
    {
    FUNC_LOG;

    return CSsmInitPsKeys::NewL();
    }

// ---------------------------------------------------------------------------
// SsmSystemCmdsFactory::InitStartupReasonNewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmCustomCommand* SsmSystemCmdsFactory::InitStartupReasonNewL()
    {
    FUNC_LOG;

    return CSsmInitStartupReason::NewL();
    }
    
// ---------------------------------------------------------------------------
// SsmSystemCmdsFactory::SsmSystemCmdsNewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmCustomCommand* SsmSystemCmdsFactory::InitCriticalLevelsNewL()
    {
    FUNC_LOG;

    return CSsmInitCriticalLevels::NewL();
    }

// ---------------------------------------------------------------------------
// SsmSystemCmdsFactory::WaitForPsKeyExactNewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmCustomCommand* SsmSystemCmdsFactory::WaitForPsKeyExactNewL()
    {
    FUNC_LOG;
    
    return CSsmWaitPsKey::NewL( CSsmWaitPsKey::ESsmWaitPsExactValue );
    }
// ---------------------------------------------------------------------------
// SsmSystemCmdsFactory::WaitForPsKeyRangeNewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmCustomCommand* SsmSystemCmdsFactory::WaitForPsKeyRangeNewL()
    {
    FUNC_LOG;
    
    return CSsmWaitPsKey::NewL( CSsmWaitPsKey::ESsmWaitPsRange );
    }
// ---------------------------------------------------------------------------
// SsmSystemCmdsFactory::SaShutdownNewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmCustomCommand* SsmSystemCmdsFactory::SaShutdownNewL()
    {
    FUNC_LOG;
    
    return CSsmSaShutdown::NewL();
    }
