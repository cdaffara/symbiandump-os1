// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HWRMPOWERDOMAINCRKEYS_H
#define HWRMPOWERDOMAINCRKEYS_H

#include <e32cmn.h>

/**
* Power Settings repository UID.
*
* @publishedPartner
* @released
*/
const TUid KCRUidPowerSettings = {0x10200C8D};


/**
* Value for the Base Time Interval.
*
* Value is in milliseconds
*
* @publishedPartner
* @released
*/
const TUint32 KPowerBaseTimeInterval            = 0x00000001;

/**
* Maximum Reporting Period.
* 
* Value is in milliseconds
* 
* @publishedPartner
* @released
*/
const TUint32 KPowerMaxReportingPeriod          = 0x00000002;

#endif  // HWRMPOWERDOMAINCRKEYS_H
            
// End of File
