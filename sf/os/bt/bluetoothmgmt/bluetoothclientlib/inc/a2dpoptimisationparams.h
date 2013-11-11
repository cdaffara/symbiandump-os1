// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef A2DP_OPTIMISATION_PARAMS_H
#define A2DP_OPTIMISATION_PARAMS_H

#include <e32base.h>
#include <bttypes.h>

NONSHARABLE_CLASS(TA2dpOptimisationParams)
	{
public:
	IMPORT_C TA2dpOptimisationParams();
	IMPORT_C TA2dpOptimisationParams(const TBTDevAddr& aBDAddr, TUint aPeakBandwidth, TUint aAccessLatency);

public:
	IMPORT_C TBTDevAddr RemoteDeviceAddress() const;
	IMPORT_C TUint PeakBandwidth() const;
	IMPORT_C TUint AccessLatency() const;

private:
	// address of the remote device to apply optimisation
	TBTDevAddr iBDAddr;
	
	// Peak Bandwidth in octets per second
	TUint iPeakBandwidth;
	
	// Access Latency in microseconds
	TUint iAccessLatency;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32	iPadding1;
	TUint32 iPadding2;
	};
	
#endif	//A2DP_OPTIMISATION_PARAMS_H
