/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "rlbslocmonitordb.h"
#include "clbslocmonitordbengine.h"
#include "lbsdevloggermacros.h"


/** Default constructor 
@released*/
RLbsLocMonitorDb::RLbsLocMonitorDb():
	iDbEngine(NULL)
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::RLbsLocMonitorDb()");
	}


/** Default destructor */
RLbsLocMonitorDb::~RLbsLocMonitorDb()
	{
	LBSLOG(ELogP1, "->S RLbsLocMonitorDb::~RLbsLocMonitorDb() \n");
	__ASSERT_DEBUG(iDbEngine == NULL, User::Panic(_L("RLbsLocMonitorDb "), KErrInUse));
	}


void RLbsLocMonitorDb::OpenL()
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::Open()");
	iDbEngine = CLbsLocMonitorDbEngine::NewL();
	}


void RLbsLocMonitorDb::Close()
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::Close()");
	if(iDbEngine)
		{
		delete iDbEngine;
		iDbEngine = NULL;
		}
	}


TInt RLbsLocMonitorDb::SavePosition(const TPosition& aPosition, const RPointerArray<TLbsLocMonitorAreaInfoBase>& aAreaInfo, TBool aUserPosition, TRequestStatus& aStatus)
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::SavePosition()");
	if(iDbEngine)
		{
		const TLbsLocMonitorAreaInfoGci* areaGci = static_cast<const TLbsLocMonitorAreaInfoGci*>(aAreaInfo[0]);

		// Only try to save valid cells and ignore any that are invalid.
		if (areaGci->iValidity)
			return iDbEngine->SavePosition(areaGci->iMcc, areaGci->iMnc, areaGci->iLac, areaGci->iCid, areaGci->iIs3gNetworkMode, aPosition, aUserPosition, aStatus);
		else
			return KErrNone;
		}
	return KErrNotFound;
	}


TInt RLbsLocMonitorDb::CancelSavePosition()
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::CancelSavePosition()");
	return KErrNotFound;
	}


TInt RLbsLocMonitorDb::GetPosition(TPosition& aPosition, const RPointerArray<TLbsLocMonitorAreaInfoBase>& aAreaInfo, TPositionAreaExtendedInfo& aMatchingAreaInfo, TRequestStatus& aStatus)
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::GetPosition()");
	if(iDbEngine)
		{
		const TLbsLocMonitorAreaInfoGci* areaGci = static_cast<const TLbsLocMonitorAreaInfoGci*>(aAreaInfo[0]);
		return iDbEngine->GetPosition(areaGci->iMcc, areaGci->iMnc, areaGci->iLac, areaGci->iCid, areaGci->iIs3gNetworkMode,
			                          aPosition, aMatchingAreaInfo, aStatus);
		}
	return KErrNotFound;
	}


TInt RLbsLocMonitorDb::CancelGetPosition()
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::CancelGetPosition()");
	return KErrNotFound;
	}


TInt RLbsLocMonitorDb::GetLastStoredPosition(TPosition& aPosition, TRequestStatus& aStatus)
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::GetLastStoredPosition()");
	if(iDbEngine)
		{
		return iDbEngine->GetPosition(aPosition, aStatus);
		}
	return KErrNotFound;
	}


TInt RLbsLocMonitorDb::CancelGetLastStoredPosition()
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::CancelGetLastStoredPosition()");
	return KErrNotFound;
	}


TInt RLbsLocMonitorDb::ClearDatabase()
	{
	LBSLOG(ELogP1, "RLbsLocMonitorDb::ClearDatabase()");
	if(iDbEngine)
		{
		return iDbEngine->ClearDatabase();
		}
	return KErrNotFound;
	}
