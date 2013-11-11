// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



#include    <lbscommon.h>
#include    "epos_modulesinternal.h"
#include    <lbs/epos_cposmoduleupdate.h>

// =========================== MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModuleUpdate::CPosModuleUpdate()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModuleUpdate::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPosModuleUpdate* CPosModuleUpdate::NewLC()
    {
    CPosModuleUpdate* self = new (ELeave) CPosModuleUpdate;

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModuleUpdate::~CPosModuleUpdate()
    {
    }

/**
 * Sets new module availability value.
 *
 * Availability indicates if the positioning module is available for
 * use via Location Acquisition API.
 *
 * The change is committed by calling CPosModules::UpdateModuleL() .
 * 
 * @param aIsAvailable ETrue if the module should be available, EFalse
 *   otherwise.
 */
EXPORT_C void CPosModuleUpdate::SetUpdateAvailability( 
    TBool aIsAvailable)
    {
    iIsAvailableUpdate = aIsAvailable;
    iUpdateMask |= KPosModUpdateAvailability;
    }

/**
* Sets new cost attribute value.
*
* Cost is just an indication of the charge involved when using a
* certain positioning module to retrieve a position. The
* cost indication may be inaccurate and can be updated by a client.
* 
* The change is committed by calling CPosModules::UpdateModuleL() .
* 
* @param aCost The new cost indicator value.
*/
EXPORT_C void CPosModuleUpdate::SetUpdateCost( 
    TPositionQuality::TCostIndicator aCost)
    {
    __ASSERT_ALWAYS((aCost >= TPositionQuality::ECostUnknown) &&
        (aCost <= TPositionQuality::ECostCharge),
        Panic(EPosLocSetPanicParameterOutOfRange));
    iCostUpdate = aCost;
    iUpdateMask |= KPosModUpdateCost;
    }

/**
 * Sets new visibility attribute value.
 *
 * Visibility defines if this module should be listed by
 * CPosModules::ModuleIdListL() and CPosModules::ModuleIdListLC() .
 * It also affects RPositionServer::GetNumModules() and 
 * RPositionServer::GetModuleByIndex().
 * 
 * The change is committed by calling CPosModules::UpdateModuleL() .
 * 
 * @param aIsVisible The new visibility value. ETrue - visible, otherwise not.
 *
 */
EXPORT_C void CPosModuleUpdate::SetUpdateVisibility( 
    TBool aVisibility)
    {
    iIsVisibleUpdate = aVisibility;
    iUpdateMask |= KPosModUpdateVisibility;
    }

/** Returns the update bitmap.
* The bitmap contains information of which attributes will be updated.
* Individual attributes can be masked out by applying the constants
* which start with KPosModUpdate .
* 
* @return The update bitmap.
* @internalTechnology
* @released
*/
TUint CPosModuleUpdate::UpdateMap() const
    {
    return iUpdateMask;
    }

/** 
* Returns the update value for the module status attribute.
*
* @return The update value for the module status attribute.
* @internalTechnology
* released
*/
TBool CPosModuleUpdate::AvailabilityUpdate() const
    {
    return iIsAvailableUpdate;
    }

/**
* Returns the update value for the cost attribute.
* @return The update value for the cost attribute.
* @internalTechnology
* @released
*/
TPositionQuality::TCostIndicator CPosModuleUpdate::CostUpdate() const
    {
    return iCostUpdate;
    }

/**
* Returns the update value for the visibility attribute.
*
* @return The update value for the visibility attribute.
* @internalTechnology
* @released
*/

TBool CPosModuleUpdate::VisibilityUpdate() const
    {
    return iIsVisibleUpdate;
    }
