/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef LBSREFFNPINT_H
#define LBSREFFNPINT_H

#include <e32std.h>
#include <e32property.h>
#include "lbscommoninternaldatatypes.h"
#include <lbspositioninfo.h>
#include <lbssatellite.h>
#include <lbs/lbsextendedsatellite.h>
/**
@file
@internalTechnology
@released
*/

/**
Class used to publish network reference location updates to interested 
components. Currently this includes the Network Location Manager and
the Network Request Handler.

@see TPositionInfoBase
*/
NONSHARABLE_CLASS(RLbsNetworkPositionUpdates)
	{
public:
	/** The types of position that can be supplied from the network.
	*/
	enum TLbsNetworkPositionUpdateId
		{
		ENetworkPositionUnknown = 0,
		ENetworkPositionReference,
		ENetworkPositionFinal
		};

public:
	IMPORT_C static void InitializeL();
	IMPORT_C static void ShutDownL();

	IMPORT_C RLbsNetworkPositionUpdates();

	IMPORT_C void OpenL(TLbsNetworkPositionUpdateId aPositionId);
	IMPORT_C void Close();
	
	IMPORT_C TInt GetPositionInfo(TLbsNetSessionIdInt& aSessionId, TPositionInfoBase& aPosInfo, TTime& aTargetTime, TTime& aActualTime);
	IMPORT_C TInt SetPositionInfo(const TLbsNetSessionIdInt& aSessionId, const TInt& aError, const TPositionInfoBase& aPositionInfo, const TTime& aActualTime);

	IMPORT_C void NotifyNetworkLocationUpdate(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyNetworkLocationUpdate();

private:
	TBool IsValidPositionInfoBaseType(TUint32 aClassType, TUint aClassSize);

private:
	RLbsNetworkPositionUpdates(const RLbsNetworkPositionUpdates&);
	RLbsNetworkPositionUpdates& operator=(const RLbsNetworkPositionUpdates&);
	
private:
	static void GetCategoryUidL(TUint32 aKey, TUid& aCategoryUid);
	static void SetCategoryUidL(TUint32 aKey, TUid aCategoryUid);

private:
	RProperty iNetworkPositionProperty;
	};

#endif // LBSREFFNPINT_H
