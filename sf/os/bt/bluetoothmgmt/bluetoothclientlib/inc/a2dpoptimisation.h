// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
*/

#ifndef A2DP_OPTIMISATION_H
#define A2DP_OPTIMISATION_H

#include <e32base.h>
#include <bttypes.h>

class RHCIServerSession;

_LIT(KA2dpOptimiserUtilsPanic, "A2dpOptimClient");

NONSHARABLE_CLASS(RA2dpOptimiser)
/**
API used for optimisation related to A2DP streaming
*/
	{
public:
	enum TA2dpOptimiserUtilsPanic
		{
		EApiUsedWhenHandleIsNotOpen = 0,
		EHandleIsAlreadyOpen = 1,
		};

	enum TA2dpOptimiserFunctionIds
		{
		EHCIA2dpOptimiseAcl = 0,
		EHCIA2dpRemoveAclOptimisation = 1,
		};

public:
	IMPORT_C RA2dpOptimiser();

	IMPORT_C TInt Open();
	IMPORT_C void Close();

	IMPORT_C void OptimiseAclForAudioStreaming(const TBTDevAddr& aDevAddr, TUint aPeakBandwidth, TUint aAccessLatency);
	IMPORT_C void RemoveAclOptimisation(const TBTDevAddr& aDevAddr);

private:
	// private copy c'tor to prevent shallow copy of iHCIServerSession
	RA2dpOptimiser(const RA2dpOptimiser&);
	
private:
	RHCIServerSession* iHCIServerSession;

	TUint32 iReserved1; // Padding for possible future "per-copy" state.
	};
	
#endif /* A2DP_OPTIMISATION_H */

