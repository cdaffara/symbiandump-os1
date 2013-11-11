// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file defines the major data type definitions for the LBS Privacy Request API.
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/

#include <e32std.h>

#ifndef LBSPRVREQUESTCLASSTYPES_H_
#define LBSPRVREQUESTCLASSTYPES_H_

/** Type for values defined in _TLbsPrivacyRequestClassTypes */
typedef TUint32 TLbsPrivacyRequestClassTypes;

/** Class types for TLbsPrivacyRequest and its derived classes 
Note that the _TLbsPrivacyRequestClassType enum may be extended in the future by adding
more enumerated values. To maintain compatibility any unrecognized values 
must be handled as ELbsPrivacyRequestUnknownClass.
*/
enum _TLbsPrivacyRequestClassType
 {
 /** Not used */
 ELbsPrivacyRequestUnknownClass = 0x00,
 /** The only value of the enumeration that should be used */
 ELbsPrivacyRequestClass        = 0x01,
 /** Not used */
 ELbsPrivacyRequestLastClass   = 0xFFFFFFFF
 };

#endif // LBSPRVREQUESTCLASSTYPES_H_

