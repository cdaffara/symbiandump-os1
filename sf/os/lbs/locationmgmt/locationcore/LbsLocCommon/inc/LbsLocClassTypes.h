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

#ifndef __LBSLOCCLASSTYPE_H__
#define __LBSLOCCLASSTYPE_H__

#include <e32def.h>

/**  Transmit position to third party options class types
@publishedPartner
@released
 */
enum _TTransmitPositionOptionsClassType
	{
	/** ETransmitPositionOptionsUnknownClass */
	ETransmitPositionOptionsUnknownClass		= 0,
	/** ETransmitPositionOptionsClass */
	ETransmitPositionOptionsClass				= 0x01,
	/** ETransmitPositionOptionsFirstCustomClass */
	ETransmitPositionOptionsFirstCustomClass	= 0x01000000
	};

/** TTransmitPositionOptionsClassType typedef  
@publishedPartner
@released
 */
typedef TUint32 TTransmitPositionOptionsClassType;

/**  External location request information class types.
@publishedPartner
@released
 */
enum _TExternalRequestInfoClassType
	{
	/** EExternalRequestInfoUnknownClass */
	EExternalRequestInfoUnknownClass		= 0,
	/** EExternalRequestInfoClass */
	EExternalRequestInfoClass				= 0x01,
	/** EExternalRequestInfoClass2 */
	EExternalRequestInfoClass2				= 0x02,
	/** EExternalRequestInfoFirstCustomClass */
	EExternalRequestInfoFirstCustomClass	= 0x01000000
	};
	
/** TExternalRequestClassInfoType typedef  
@publishedPartner
@released
 */
typedef TUint32 TExternalRequestClassInfoType;

#endif // __LBSLOCCLASSTYPE_H__
