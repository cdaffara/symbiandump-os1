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



#ifndef LOCATIONFWDOMAINPSKEYS_H_
#define LOCATIONFWDOMAINPSKEYS_H_

/** @file
 * This file defines P&S keys of Positioning Indicator API
 * @publishedPartner
 * @deprecated
 */

/** Defines value UID of Positioning Indicator P&S keys category.
 */
const TInt KPosIndicatorCategory = 0x101F7A79;

/** Defines UID of Positioning Indicator P&S keys category. */
const TUid KPosIndicatorCategoryUid = { KPosIndicatorCategory };

/**
 * Values to use for Integrated GPS HW Status key KPosIntGpsHwStatus.
 */
enum TPosIntGpsHwState
    {
    /** Integrated GPS HW is OFF. UI Indicator should be hidden. */
    EPosIntGpsHwOff    = 0,
    /** Integrated GPS HW is ON. UI indicator should be shown. */
    EPosIntGpsHwOn,
    /** Additional state of Integrated GPS HW. UI indicator should be in "animated" state. */
    EPosIntGpsHwIntermediate,
    };

/**
 * ID of the Integrated GPS HW Status key.
 * May contain values defined by TPosIntGpsHwIndicatorState .
 *
 * Default value (i.e. when this key is not found) is EPosIntGpsHwIndicatorOff.
 *
 * If it contains any other value than defined in TPosIntGpsHwState ,
 * such value shall be ignored and TPosIndicatorState::EPosIntGpsHwIndicatorOff
 * shall be assumed.
 */
const TInt KPosIntGpsHwStatus = 0x00000001;

#endif /*LOCATIONFWDOMAINPSKEYS_H_*/
