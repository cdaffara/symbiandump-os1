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
* Implementation of SsmActivityCmdFactory class.
*
*/

#include "ssmactivitycmdfactory.h"
#include "ssmactivitycmd.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// SsmActivityCmdFactory::SsmActivityCmdNewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmCustomCommand* SsmActivityCmdFactory::SsmActivityCmdNewL()
    {
    FUNC_LOG;

    return CSsmActivityCmd::NewL();
    }
