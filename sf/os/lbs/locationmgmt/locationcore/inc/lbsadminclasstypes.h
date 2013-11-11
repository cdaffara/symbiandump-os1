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
// Location Based Services Administration API Advanced Settings
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef LBS_ADMIN_CLASS_TYPES_H
#define LBS_ADMIN_CLASS_TYPES_H

#include <e32def.h>

/** TLbsAdminInfoClassType typedef.
@publishedPartner
@released
*/
typedef TUint32 TLbsAdminInfoClassType;

/**  Settings class types.
Every TLbsAdminInfoBase derived class has a unique class type.
The class types are not intended to be bitwise ORed.

Note that the enumeration may be extended in the future by adding more values. 
To maintain compatibility any unrecognized values must be handled as 
ELbsAdminInfoBaseUnknownClass.

@publishedPartner
@released
*/
enum _TLbsAdminInfoClassType
	{
	ELbsAdminInfoBaseUnknownClass					= 0x00,
	ELbsAdminProtocolModulesInfoClass				= 0x01,
	ELbsAdminExtLocServicesInfoClass				= 0x02,
	ELbsAdminProtocolModuleLoadingInfoClass			= 0x03
	};

#endif LBS_ADMIN_CLASS_TYPES_H
