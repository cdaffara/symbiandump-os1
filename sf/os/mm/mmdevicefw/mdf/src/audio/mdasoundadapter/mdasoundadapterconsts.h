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
 
#ifndef MDASOUNDADAPTERCONSTVALUES_H
#define MDASOUNDADAPTERCONSTVALUES_H

#include <e32std.h>

/**
@SYMPatchable
@publishedPartner
 
Patchable constant for licencee to set force the adapter to output stereo data.
The constant can be changed at ROM build time using patchdata OBY keyword.
 */
IMPORT_C extern const TInt KSoundAdapterForceStereo;

#endif // MDASOUNDADAPTERCONSTVALUES_H
