// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef FBSDEFS_H
#define FBSDEFS_H

/**
@publishedAll
@released
WARNING: Constant is  for internal use ONLY.  Compatibility is not guaranteed in future releases.
*/
const TUid KMultiBitmapRomImageUid = { 268435521 };

/**
@publishedAll
@released
*/
IMPORT_C TInt FbsStartup();

/** 
Required to ensure BC between NGage and 7.0S platforms.  
Function is exported at ordinal corresponding to where NGage platform
has extended this library and must not be moved.
@publishedAll
@released
*/
IMPORT_C void DummyReserved1();


#endif // FBSDEFS_H
