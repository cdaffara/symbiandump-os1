/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Header UID.H
*
*/


#ifndef __UID_H__
#define __UID_H__

#include "GDR.H"
/**
@file
@publishedAll 
*/
const int32 KFnttranVersion = 41; /**<WARNING: Constant for internal use ONLY.  Compatibility is not guaranteed in future releases.*/
const int32 KPdrtranVersion = 41; /**<WARNING: Constant for internal use ONLY.  Compatibility is not guaranteed in future releases.*/
const uid KStoreWriteOnceLayoutUid = 268435511; /**<WARNING: Constant for internal use ONLY.  Compatibility is not guaranteed in future releases.*/
const uid KFontStoreFileUid = 268435513; /**<WARNING: Constant for internal use ONLY.  Compatibility is not guaranteed in future releases.*/
const uid KPdrStoreFileUid = 268435514; /**<WARNING: Constant for internal use ONLY.  Compatibility is not guaranteed in future releases.*/
const uid KNullUid = 0; /**<WARNING: Constant for internal use ONLY.  Compatibility is not guaranteed in future releases.*/
const uint32 KFontStoreFileChecksum = 0x47393853; /**<WARNING: Constant for internal use ONLY.  Compatibility is not guaranteed in future releases.*/
const uint32 KPdrStoreFileChecksum = 0x4739A38F; /**<WARNING: Constant for internal use ONLY.  Compatibility is not guaranteed in future releases.*/

#endif
