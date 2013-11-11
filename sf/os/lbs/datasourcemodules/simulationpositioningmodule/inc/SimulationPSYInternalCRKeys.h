/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef SIMULATIONPSYINTERNALCRKEYS_H
#define SIMULATIONPSYINTERNALCRKEYS_H

// Positioning Plugin Information API
#include <lbs/positioningpluginfwinternalcrkeys.h>

/** @file
 * 
 * @publishedPartner
 * @released
 */

/**
* Simulation PSY CenRep UID
*/
const TUid KCRUidSimulationPSY = { 0x101F7A81 };

/**
 *  Defines the file that the Simulation PSY uses for position its position data.
 */ 
const TUint32 KCRKeySimPSYSimulationFile    = 0x10000000;

/**
 *  The Central Repository key that is used to enable Common Data Set behaviour.
 *  0:  Independent Data Set (Default)
 *  1:  Common Data Set
 */
const TUint32 KCRKeySimPSYCommonDataSet		= 0x10000001;

#endif      // SIMULATIONPSYINTERNALCRKEYS_H
