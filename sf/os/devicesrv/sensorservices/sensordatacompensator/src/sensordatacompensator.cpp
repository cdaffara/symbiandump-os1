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
* Description:  Sensor data compensator class implementation
*
*/


#include <sensordatacompensationtypes.h>
#include "sensordatacompensatorimpl.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CSensorDataCompensator* CSensorDataCompensator::NewL(
    TSensrvChannelDataTypeId aDataType, TSensorCompensationType aType )
    {
    CSensorDataCompensator* impl = 
        CSensorDataCompensatorImpl::NewL( aDataType, aType );
    return impl;
    }

