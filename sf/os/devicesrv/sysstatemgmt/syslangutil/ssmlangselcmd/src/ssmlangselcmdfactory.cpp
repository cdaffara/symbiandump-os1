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
* Implementation of SsmLangSelCmdFactory class.
*
*/

#include "ssmlangselcmdfactory.h"
#include "ssmlangselcmd.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// SsmLangSelCmdFactory::SsmLangSelCmdNewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmCustomCommand* SsmLangSelCmdFactory::SsmLangSelCmdNewL()
    {
    FUNC_LOG;

    return CSsmLangSelCmd::NewL();
    }
