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
// System
// 
//

/**
 @file
 @internalTechnology
 
*/

#include <e32base.h>
#include <e32math.h>

// Component
#include "netpmutils.h"

/**
Constants used in encoding of uncertainty data

@see CSuplStart::Uncertainty()
@see CSuplStart::UncertaintyAltitude()
*/
const TReal KLbsLogOnePointOne = 0.095310179804324860043952123280765;
const TReal KLbsLogOnePointZeroTwoFive = 0.02469261259037150101430767543669;
const TInt  KLbsMaxUncert = 127;

/**
 * TODO: update asn1 code to use this version and delete the CSuplMessageBase version?
Uncertainty()

Converts a minumum accuracy value in meters to an uncertainty value K as 
described in 3GPP 23.032 (Universal Geographical Area Description) section 6.2.

r = C((1+x)^K - 1)

where r = distance in meters
      C = 10
      x = 0.1
      K = uncertainty value
      
hence K = ln(r/C + 1) / ln(1.1)

@param aDistance - distance measurement in meters
@return uncertainty value K
*/
EXPORT_C TInt NetPmUtils::Uncertainty(const TReal32& aDistance)
	{
	TReal uncert;
	Math::Ln(uncert,  (aDistance/10) + 1 );
	uncert /= KLbsLogOnePointOne;
	if (uncert>KLbsMaxUncert)
		{
		uncert = KLbsMaxUncert;
		}

	// round to nearest whole number
	TReal uncertRounded;
	Math::Round(uncertRounded, uncert, 0);
	
	return (TInt)uncertRounded;
	}


/**
UncertaintyAltitude()

Converts a minumum accuracy value in meters to an uncertainty altitude value K as 
described in 3GPP 23.032 (Universal Geographical Area Description) section 6.4.

r = C((1+x)^K - 1)

where r = distance in meters
      C = 45
      x = 0.1
      K = uncertainty value
      
hence K = ln(r/C + 1) / ln(1.1)

@param aDistance - altitude accuracy in meters
@return uncertainty altitude value K
*/
EXPORT_C TInt NetPmUtils::UncertaintyAltitude(const TReal32& aDistance)
	{
	
	TReal uncert;
	Math::Ln(uncert,  (aDistance/45) + 1 );
	uncert /= KLbsLogOnePointZeroTwoFive;
	if (uncert>KLbsMaxUncert)
		{
		uncert = KLbsMaxUncert;
		}
	// round to nearest whole number
	TReal uncertRounded;
	Math::Round(uncertRounded, uncert, 0);
	
	return (TInt)uncertRounded;
	}

/**
EncodeAltitude()

Converts an value for altiutude to an 15 bit binary coded number N

@param aAltitude - altitude in meters
@return uncertainty altitude value K
*/
EXPORT_C TInt NetPmUtils::EncodeAltitude(const TReal32& aAltitude)
	{
	TInt altEncoded = (TInt)aAltitude;
	if (altEncoded>KLbsMaxAltitude)
		{
		altEncoded = KLbsMaxAltitude;
		}

	return altEncoded;
	}
