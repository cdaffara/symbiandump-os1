/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Mobility simulation components
* 
*
*/



/**
 @file MobilitySimulation.h
*/

#ifndef _MOBILITYSIMULATION_H_
#define _MOBILITYSIMULATION_H_

#include <e32base.h>
#include <e32property.h>
#include <in_sock.h>
#include <es_sock.h>

const TUid KBearerMobilityTestPropCat = TUid::Uid(0x10012345); // TODO replace with something meaningful or platsec ready

class CBearerAvailability
{
	public:
		// Constructor
		CBearerAvailability(TUint aBearer);

		enum TAvailabilityState {EAvailable, EUnavailable};
		TInt GetAvailability(TAvailabilityState& aAvailability);
		TInt MakeAvailable();
		TInt MakeUnavailable();
		TInt MakeDisconnected();


	private:
		TUint iBearerIndex;
		enum TBearerIndex {EMinBearerIndex = 1, EMaxBearerIndex = 3};
		enum TAvailabilityProp {EAvailabilityIAP1, EAvailabilityIAP2, EAvailabilityIAP3};

		TUint SetAvailability(TAvailabilityState aAvailability);

		TBool iPropertyDefined; // = false;
};

// Watches for changes in the simulated availability of a bearer set by a client application
class CBearerStateWatcher : CActive
{
	public:
		TInt WaitForAvailabilityChange();
		void RunL();
};


#endif

