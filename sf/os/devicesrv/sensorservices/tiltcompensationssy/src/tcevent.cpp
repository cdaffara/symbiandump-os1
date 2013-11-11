/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TTCEvent class implementation.
*
*/


#include "tcevent.h"
#include "trace.h"

TTCEvent::TTCEvent( TTCEventId aId ):
    iId( aId )
    {
    FUNC_LOG;
    }

// METHODS

// ----------------------------------------------------------------------------------
// TTCEvent::Id
// ----------------------------------------------------------------------------------
//
TTCEventId TTCEvent::Id() const
    {
    FUNC_LOG;
    
    return iId;
    }

// End of File
