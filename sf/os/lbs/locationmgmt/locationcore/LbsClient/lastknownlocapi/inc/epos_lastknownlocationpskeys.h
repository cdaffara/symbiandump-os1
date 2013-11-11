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



#ifndef EPOS_LASTKNOWNLOCATIONPSKEYS_H
#define EPOS_LASTKNOWNLOCATIONPSKEYS_H

/** @file
 * This file defines P&S keys of Last Known Location API
 * @publishedPartner
 * @released
 */

/** Defines value UID of Location P&S keys category.
 */
const TUid KPosLastKnownLocationCategory = { 0x10009BFA };

/**
 * ID of the Last Known Location key.
 * Contains binary values, a buffer with packaged TPositionInfo.
 *
 * If it contains a buffer, which length does not match TPositionInfo,
 * such value shall be ignored.
 *
 * Client must have ReadDeviceData capability in order to read this key.
 */
const TInt KPosLastKnownLocation = 0x00000001;

#endif /*EPOS_LASTKNOWNLOCATIONPSKEYS_H*/
