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
 
#ifndef DEVSOUNDADAPTATIONINFOCONSTVALUES_H
#define DEVSOUNDADAPTATIONINFOCONSTVALUES_H

#include <e32std.h>

/**
@SYMPatchable
@publishedPartner
 
Patchable constant for licencee to set devsound adaptor MaxGain
Licencees should ensure that the devsound adaptor MaxGain is equal to the system adaptor MaxGain.
The constant can be changed at ROM build time using patchdata OBY keyword.
 */
IMPORT_C extern const TInt KDevsoundMaxGain;

/**
@SYMPatchable
@publishedPartner
 
Patchable constant for licencee to set devsound adaptor MaxVolume
Licencees should ensure that the devsound adaptor MaxVolume is equal to the system adaptor MaxVolume.
The constant can be changed at ROM build time using patchdata OBY keyword.
 */
IMPORT_C extern const TInt KDevsoundMaxVolume;

#endif // DEVSOUNDADAPTATIONINFOCONSTVALUES_H
