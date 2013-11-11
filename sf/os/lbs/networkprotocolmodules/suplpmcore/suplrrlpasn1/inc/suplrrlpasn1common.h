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
//

/**
 @file
 @internalTechnology
 
*/
#ifndef SUPL_RRLP_ASN1_COMMON
#define SUPL_RRLP_ASN1_COMMON

/**
Constants used in the encoding of position values.

@see CSuplMessageBase::PopulatePosition()
*/
const TReal KLbsLatitudeConst  = 93206.7555555556;   // 2^23 / 90
const TReal KLbsLongitudeConst = 46603.3777777778;   // 2^24 / 360
const TInt  KLbsMaxAltitude    = 32767;          // 2^15 - 1

/**
Constants used in velocity conversion
1 meter per second = 3.6 kilometers per hour

@see CSuplMessageBase::PopulatePosition()
*/
const TReal KLbsMpsKmphConstant = 3.6;

/**
Constants used in encoding of uncertainty data

@see CSuplStart::Uncertainty()
@see CSuplStart::UncertaintyAltitude()
*/
const TReal KLbsLogOnePointOne = 0.095310179804324860043952123280765;
const TReal KLbsLogOnePointZeroTwoFive = 0.02469261259037150101430767543669;
const TInt  KLbsMaxUncert = 127;

/**
Number of milliseconds in a second

@see CSuplStart::SetQoP
*/
const TInt KLbsMilliSeconds = 1000000;


#endif // SUPL_RRLP_ASN1_COMMON
