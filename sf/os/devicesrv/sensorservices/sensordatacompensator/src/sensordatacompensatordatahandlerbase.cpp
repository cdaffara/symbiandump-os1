/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Base class for all compensators
*
*/


#include "sensordatacompensatordatahandlerbase.h"
#include "trace.h"


// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CSensorDataCompensatorDataHandlerBase::CSensorDataCompensatorDataHandlerBase
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorDataHandlerBase::CSensorDataCompensatorDataHandlerBase(
                                                        CSensorDataCompensatorPlugin& aPlugin )
    : iPlugin( aPlugin )
    {
    FUNC_LOG;
    }
