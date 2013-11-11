// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CODECAPIRESOLVERCONSTS_H
#define CODECAPIRESOLVERCONSTS_H

/**
Opaque data tag for the media uid
*/
_LIT8(KMediaUid, "<i>");

/**
Opaque data tag for the input data format
*/
_LIT8(KInputPortFormat, "<s>");

/**
Opaque data tag for the output data format
*/
_LIT8(KOutputPortFormat, "<d>");

/**
Opaque data tag for the manufacturer name
*/
_LIT8(KManufacturer, "<m>");

/**
Opaque data tag for the picture size
*/
_LIT8(KMaxPictureSize, "<p>");

/**
Opaque data tag for the picture rate and size
*/
_LIT8(KListOfPictureRateAndSize, "<l>");

/**
Opaque data tag general format
*/
_LIT8(KTagMatch, "*<?>*");

/**
Opaque data tag for comma
*/
_LIT8(KComma, "*,*");

/**
Length of a tag from the opaque data
*/
const TUint KTagLength = 3;

#endif // CODECAPIRESOLVERCONSTS_H
