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
// Bearer availability and mobility simulator classes
// 
//

/**
 @file MobilitySimulation.cpp
*/

#include <e32base.h>
#include <e32property.h>
#include <in_sock.h>
#include <es_sock.h>

#include "MobilitySimulation.h"


#ifdef __1060_TESTCODE__


TInt CBearerAvailability::GetAvailability(TAvailabilityState& aAvailability)
{
	return RProperty::Get(KBMobilityTestPropCat, iBearerIndex, (TInt&)aAvailability);
}

TUint CBearerAvailability::SetAvailability(TAvailabilityState aAvailability)
{
	// Set the property associated with the bearer who's availability we are changing
	TUint err = RProperty::Set(KBMobilityTestPropCat, iBearerIndex, aAvailability);

	// Should have occurred
	return err;
}


TInt CBearerAvailability::MakeAvailable()
{
	return SetAvailability(EAvailable);
}

TInt CBearerAvailability::MakeUnavailable()
{
	return SetAvailability(EUnavailable);
}

TInt CBearerAvailability::Disconnect()
{
	return SetAvailability(EDisconnected);
}

CBearerAvailability::CBearerAvailability(TUint aBearer)
{
	if ((aBearer >= EMinBearerIndex) && (aBearer <= EMaxBearerIndex))
	{
		iBearerIndex = aBearer;

		TInt err = RProperty::Define(KBMobilityTestPropCat, iBearerIndex, RProperty::EInt);

		if (err == KErrNone)
			iPropertyDefined = true;
	}
}

// Class for watching for changes to availability

TInt CBearerStateWatcher::WaitForBearerStateChange()
{
}

#endif


