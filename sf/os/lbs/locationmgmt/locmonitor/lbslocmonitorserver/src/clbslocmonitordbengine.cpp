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



#include "clbslocmonitordbengine.h"
#include "lbslocmonitordbenginedefs.h"
#include "lbsdevloggermacros.h"

const TInt KMaskInBit28 = 0x10000000;

CLbsLocMonitorDbEngine* CLbsLocMonitorDbEngine::NewL()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::NewL");
	CLbsLocMonitorDbEngine* self = new(ELeave)CLbsLocMonitorDbEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


void CLbsLocMonitorDbEngine::ConstructL()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::ConstructL");
	InitDbL();
	iDbTimer = CLbsLocMonitorDbTimer::NewL(*this);
	}		


void CLbsLocMonitorDbEngine::InitDbL()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::InitDbL");
	iCount = 0;
	iIsLastValid = EFalse;
	TInt error = iDatabase.Open(KSecureLocMonDB);
	if(KErrNotFound == error)
		{		
		RSqlSecurityPolicy securityPolicy;
		User::LeaveIfError(securityPolicy.Create(TSecurityPolicy(TSecurityPolicy::EAlwaysPass)));
		CleanupClosePushL(securityPolicy);
		User::LeaveIfError(securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy, TSecurityPolicy(TSecurityPolicy::EAlwaysPass)));
		#ifdef LBS_LOCMONITORDB_TEST
		User::LeaveIfError(iDatabase.Create(KSecureLocMonDB));
		#else
		User::LeaveIfError(iDatabase.Create(KSecureLocMonDB, securityPolicy));
		#endif
		CleanupStack::PopAndDestroy(&securityPolicy);
		iDBInitialised = ETrue;
		User::LeaveIfError(iDatabase.Exec(KCreateTable));
		User::LeaveIfError(iDatabase.Exec(KCreateIndex4));
		User::LeaveIfError(iDatabase.Exec(KCreateIndex3));
		User::LeaveIfError(iDatabase.Exec(KCreateLastPosTable));
		}
	else
		{
		User::LeaveIfError(error);
		iDBInitialised = ETrue;
		}
	User::LeaveIfError(iDatabase.Exec(KCreateTempTable));
	User::LeaveIfError(iDatabase.Exec(KCreateTempIndex4));
	User::LeaveIfError(iDatabase.Exec(KCreateLastPosTempTable));
	}


CLbsLocMonitorDbEngine::CLbsLocMonitorDbEngine():
CActive(EPriorityStandard),
iDBInitialised(EFalse),
iSaveLastPos(EFalse)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::CLbsLocMonitorDbEngine");
	CActiveScheduler::Add(this);
	}


CLbsLocMonitorDbEngine::~CLbsLocMonitorDbEngine()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::~CLbsLocMonitorDbEngine");
	Cancel();
	
	delete iDbTimer;

	if(iDBInitialised)
		{
		iSqlSaveStatement.Close();
		if(iIsLastValid)
			{
			Insert(ETrue);
			}

		Flush(ETrue);
		
		iDatabase.Close();
		}
	}


TInt CLbsLocMonitorDbEngine::SavePosition(TUint aMcc, TUint aMnc, TUint aLac, TUint aCid, TBool aIs3gMode, const TPosition& aPosition, TBool aUserPosition, TRequestStatus& aStatus)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::SavePosition");

	iSaveLastPos = aUserPosition;

	if(aMcc > KMaxTInt || aMnc > KMaxTInt || aLac > KMaxTInt || aCid > KMaxTInt)
		{
		return KErrArgument;
		}		

    iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;
	
	// If the db flush timer is not running, start it
	if(!iDbTimer->IsRunning())
	    {
        iDbTimer->StartTimer(KFlushPeriod);
	    }
	
	// If the cache does not contain a cell, this cell goes in the cache
	if (!iIsLastValid)
		{
		iLastMcc = aMcc;
		iLastMnc = aMnc;
		iLastLac = aLac;
		iLastCid = aCid;
		iLastModeIs3g = aIs3gMode;
		iLastTime.UniversalTime();
		iLastPosition = aPosition;
		if(iSaveLastPos)
		    {
            iLastKnownPosition = aPosition;
		    }
		iIsLastValid = ETrue;
		User::RequestComplete(iClientStatus, KErrNone);
		return KErrNone;
		} 
	// If this cell is the same as the the cache, update the cache's position and timestamp
	
	else if(CacheMatchLevel(aMcc, aMnc, aLac, aCid, aIs3gMode).CellIdMatch())
		{
		iLastPosition = aPosition;
	    if(iSaveLastPos)
            {
            iLastKnownPosition = aPosition;
            }
		iLastTime.UniversalTime();
		iLastModeIs3g = aIs3gMode;
		User::RequestComplete(iClientStatus, KErrNone);
		return KErrNone;
		}
	// If this cell is different from the cache, send the cache to db server then put this cell in cache
	else
		{
		TInt result = Insert(EFalse);
		if(result >= KErrNone)
			{
			iLastMcc = aMcc;
			iLastMnc = aMnc;
			iLastLac = aLac;
			iLastCid = aCid;
			iLastTime.UniversalTime();
			iLastPosition = aPosition;
			iLastModeIs3g = aIs3gMode;
			iIsLastValid = ETrue;
			}
		return result;
		}
	}


TInt CLbsLocMonitorDbEngine::Insert(TBool aShutdown)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::Insert");
	TInt error = KErrNone;
	TPckg<TPosition> positionDes( iLastPosition );
	error = iSqlSaveStatement.Prepare(iDatabase, KUpsertRow);
	if(error == KErrNone)
		{
		TInt indexMcc = iSqlSaveStatement.ParameterIndex(KMcc);
		TInt indexMnc = iSqlSaveStatement.ParameterIndex(KMnc);
		TInt indexLac = iSqlSaveStatement.ParameterIndex(KLac);
		TInt indexCid = iSqlSaveStatement.ParameterIndex(KCid);
		TInt indexStamp = iSqlSaveStatement.ParameterIndex(KStamp);
		TInt indexData = iSqlSaveStatement.ParameterIndex(KData);
		

		iSqlSaveStatement.BindInt(indexMcc, iLastMcc);
		iSqlSaveStatement.BindInt(indexMnc, iLastMnc);
		iSqlSaveStatement.BindInt(indexLac, iLastLac);
		// Set bit 28 if 3g to distinguish from 2g equivalent...
		TInt effectiveCid = iLastCid;
		if (iLastModeIs3g)
			{
			effectiveCid |= KMaskInBit28;
			}
		iSqlSaveStatement.BindInt(indexCid, effectiveCid);
		iSqlSaveStatement.BindInt64(indexStamp, iLastTime.Int64());
		iSqlSaveStatement.BindBinary(indexData, positionDes);
		
		// Shutdown inserts are synchronous, regular inserts asynchronous
		if(aShutdown)
			{
			iSqlSaveStatement.Exec();
			iSqlSaveStatement.Close();
			if(iSaveLastPos)
			    {
                error = iSqlSaveStatement.Prepare(iDatabase, KUpsertLastPosRow);
                if(KErrNone == error)
                    {
                    TPckg<TPosition> positionDes(iLastKnownPosition);
                    TInt indexStamp = iSqlSaveStatement.ParameterIndex(KStamp);
                    TInt indexData = iSqlSaveStatement.ParameterIndex(KData);
                    iSqlSaveStatement.BindInt64(indexStamp, iLastTime.Int64());
                    iSqlSaveStatement.BindBinary(indexData, positionDes);
                    
                    iSqlSaveStatement.Exec();
                    iSqlSaveStatement.Close();
                    }
			    }
			}
		else
			{
			// Statement is closed in RunL, once asynchrnous insert has taken place
			iStatus = KRequestPending;
			iSqlSaveStatement.Exec(iStatus);
			SetActive();
			}
		}
	return error;
	}


TInt CLbsLocMonitorDbEngine::GetPosition(TUint aMcc, TUint aMnc, TUint aLac, TUint aCid, TBool aIs3gMode, TPosition& aPosition, TPositionAreaExtendedInfo& aMatchingAreaInfo, TRequestStatus& aStatus)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::GetPosition");
	if(aMcc > KMaxTInt || aMnc > KMaxTInt || aLac > KMaxTInt || aCid > KMaxTInt)
		{
		return KErrArgument;
		}
		
	TInt result = KErrNone;
	// Clear result
	aMatchingAreaInfo.SetMobileCountryCodeMatch(EFalse);
	aMatchingAreaInfo.SetMobileNetworkCodeMatch(EFalse);
	aMatchingAreaInfo.SetLocationAreaCodeMatch(EFalse);
	aMatchingAreaInfo.SetCellIdMatch(EFalse);
	aMatchingAreaInfo.SetArea(TPositionAreaExtendedInfo::EAreaUnknown);

	TUint effectiveCid = aCid;
	if (aIs3gMode)
		{
		effectiveCid |= KMaskInBit28;
		}

	TPositionAreaExtendedInfo cacheMatch = CacheMatchLevel(aMcc, aMnc, aLac, aCid, aIs3gMode);
	if(cacheMatch.CellIdMatch() || Select(aPosition, TPtrC(KSelectTempRow4), aMcc, aMnc, aLac, effectiveCid) || Select(aPosition, TPtrC(KSelectRow4), aMcc, aMnc, aLac, effectiveCid))
		{		
		if(cacheMatch.CellIdMatch())
			{
			aPosition = iLastPosition;
			}
		aMatchingAreaInfo.SetMobileCountryCodeMatch(ETrue);
		aMatchingAreaInfo.SetMobileNetworkCodeMatch(ETrue);
		aMatchingAreaInfo.SetLocationAreaCodeMatch(ETrue);
		aMatchingAreaInfo.SetCellIdMatch(ETrue);
		aMatchingAreaInfo.SetArea(TPositionAreaExtendedInfo::EAreaCity);
		}
	else 
		{	
		TBool matchLac = cacheMatch.LocationAreaCodeMatch();
		if(matchLac)
			{
			aPosition = iLastPosition;
			}
		else
			{
			TBuf<30>gsmOrWcdmaCells;
			if (aIs3gMode)
				{
				gsmOrWcdmaCells = KSelectWcdmaCells;
				}
			else
				{
				gsmOrWcdmaCells = KSelectGsmCells;
				}
				
			TBuf<256> selectStatement;
			selectStatement.Format(KSelectTempRow3, &gsmOrWcdmaCells);

			if (Select(aPosition, TPtrC(selectStatement), aMcc, aMnc, aLac))
				{
				matchLac = ETrue;
				}
			else
				{
				selectStatement.Format(KSelectRow3,&gsmOrWcdmaCells);
				if (Select(aPosition, TPtrC(selectStatement), aMcc, aMnc, aLac))
					{
					matchLac = ETrue;
					}
				}
			}
				
		if (matchLac)
			{
			aMatchingAreaInfo.SetMobileCountryCodeMatch(ETrue);
			aMatchingAreaInfo.SetMobileNetworkCodeMatch(ETrue);
			aMatchingAreaInfo.SetLocationAreaCodeMatch(ETrue);
			aMatchingAreaInfo.SetArea(TPositionAreaExtendedInfo::EAreaRegion);
			}
		else
			{
			if(cacheMatch.MobileNetworkCodeMatch() || Select(aPosition, TPtrC(KSelectTempRow2), aMcc, aMnc) || Select(aPosition, TPtrC(KSelectRow2), aMcc, aMnc))
				{
				if(cacheMatch.MobileNetworkCodeMatch())
					{
					aPosition = iLastPosition;
					}
				aMatchingAreaInfo.SetMobileCountryCodeMatch(ETrue);
				aMatchingAreaInfo.SetMobileNetworkCodeMatch(ETrue);
				aMatchingAreaInfo.SetArea(TPositionAreaExtendedInfo::EAreaCountry);
				}
			else if(cacheMatch.MobileCountryCodeMatch() || Select(aPosition, TPtrC(KSelectTempRow1), aMcc) || Select(aPosition, TPtrC(KSelectRow1), aMcc))
				{
				if(cacheMatch.MobileCountryCodeMatch())
					{
					aPosition = iLastPosition;
					}
				aMatchingAreaInfo.SetMobileCountryCodeMatch(ETrue);
				aMatchingAreaInfo.SetArea(TPositionAreaExtendedInfo::EAreaCountry);
				}
			else if(iIsLastValid || Select(aPosition, TPtrC(KSelectTempRowLatest)) || Select(aPosition, TPtrC(KSelectRowLatest)) || iIsLastValid)
				{
				if(iIsLastValid)
					{
					aPosition = iLastPosition;
					}
				}
			else
				{
				result = KErrNotFound;
				}

			} // end else no match on LAC

		} // end else no match on cell id

	TRequestStatus* status = &aStatus;
	*status = KRequestPending;
	User::RequestComplete(status, result);
	return result;
	}


TInt CLbsLocMonitorDbEngine::GetPosition(TPosition& aPosition, TRequestStatus& aStatus)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::GetPosition");
	TInt result = KErrUnknown;
	// Before going to db server, check if we can get cell position from cache
	if(iIsLastValid)
		{
		aPosition = iLastPosition;
		result = KErrNone;
		}
	else if(Select(aPosition, TPtrC(KSelectTempRowLatest)) || Select(aPosition, TPtrC(KSelectRowLatest)))
		{
		result = KErrNone;
		}
	TRequestStatus* status = &aStatus;
	*status = KRequestPending;
	User::RequestComplete(status, result);
	return result;
	}

TInt CLbsLocMonitorDbEngine::ClearDatabase()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::ClearDatabase");	
	iIsLastValid = EFalse;
	iDatabase.Exec(KClear);
	iDatabase.Exec(KLastPosClear);
	iDatabase.Close();
	TInt del = iDatabase.Delete(KSecureLocMonDB);
	TRAPD(init, InitDbL());
    return (KErrNone != del) ? del: init;
	}


TBool CLbsLocMonitorDbEngine::Select(TPosition& aPosition, TPtrC aQuery, TInt aMcc, TInt aMnc, TInt aLac, TInt aCid)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::Select");
	TBool found = EFalse;
	TPckg<TPosition> positionDes( aPosition );
	RSqlStatement sqlStatement;
	sqlStatement.Prepare(iDatabase, aQuery);	
	if(aMcc > KErrNotFound)
		{
		TInt indexMcc = sqlStatement.ParameterIndex(KMcc);
		sqlStatement.BindInt(indexMcc, aMcc);
		}
	if(aMnc > KErrNotFound)
		{
		TInt indexMnc = sqlStatement.ParameterIndex(KMnc);
		sqlStatement.BindInt(indexMnc, aMnc);
		}
	if(aLac > KErrNotFound)
		{
		TInt indexLac = sqlStatement.ParameterIndex(KLac);
		sqlStatement.BindInt(indexLac, aLac);
		}
	if(aCid > KErrNotFound)
		{
		TInt indexCid = sqlStatement.ParameterIndex(KCid);
		sqlStatement.BindInt(indexCid, aCid);
		}
		
	TInt columnIndex = sqlStatement.ColumnIndex(KDataColumn);
	if(sqlStatement.Next() == KSqlAtRow)
		{
		sqlStatement.ColumnBinary(columnIndex, positionDes);
		found = ETrue;
		}
	sqlStatement.Close();
	return found;
	}


void CLbsLocMonitorDbEngine::CheckFlush()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::CheckFlush");
	iCount++;
	if(iCount > KTempFlushSize)
		{
		Flush(EFalse);		
		}
	}


void CLbsLocMonitorDbEngine::Flush(TBool aShutdown)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::Flush");
	// Only do this if an insert is not currently in progress
	if(!IsActive())
		{	
		iCount = 0;
		// Move all data from temp table to permanent table
		iDatabase.Exec(KBegin);
		iDatabase.Exec(KCopy);	
		iDatabase.Exec(KClear);
		iDatabase.Exec(KLastPosCopy);
		iDatabase.Exec(KLastPosClear);
		if((!aShutdown) && (DbSize() > KMaxDbSize))
			{
			// Delete 2000 oldest records
			iDatabase.Exec(KPrune);
			}
		iDatabase.Exec(KCommit);
		}
	}


void CLbsLocMonitorDbEngine::FlushTimerCallback()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::FlushTimerCallback");
	Flush(EFalse);
	}


// Returns number of rows in db
TInt CLbsLocMonitorDbEngine::DbSize()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::DbSize");
	TInt result = KErrNotFound;
	RSqlStatement sqlStatement;
	sqlStatement.Prepare(iDatabase, KCount);
	TInt columnIndex = sqlStatement.ColumnIndex(KCountColumn);
	if(sqlStatement.Next() == KSqlAtRow)
		{
		result = sqlStatement.ColumnInt(columnIndex);
		}
	sqlStatement.Close();
	return result;
	}


TPositionAreaExtendedInfo CLbsLocMonitorDbEngine::CacheMatchLevel(TInt aMcc, TInt aMnc, TInt aLac, TInt aCid, TBool aIs3gMode)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::CacheMatchLevel");
	TPositionAreaExtendedInfo areaInfo;

	if(iIsLastValid)
		{
		if((aMcc == iLastMcc))
			{
			if(aMnc == iLastMnc)
				{
				if(aLac == iLastLac && aIs3gMode == iLastModeIs3g) 
					{
					if(aCid == iLastCid)
						{
						areaInfo.SetCellIdMatch(ETrue);
						}
					areaInfo.SetLocationAreaCodeMatch(ETrue);
					}
				areaInfo.SetMobileNetworkCodeMatch(ETrue);
				}
			areaInfo.SetMobileCountryCodeMatch(ETrue);
			}
		}

	return areaInfo;
	}



void CLbsLocMonitorDbEngine::RunL()
	{
    // Now then
    // we set a member variable when asked to save a pos we have seen ourselves
    // we also want to save this position into the last pos table
    
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::RunL");
	
	if(!iSaveLastPos)
	    {
        // fully done, either we had no last pos to save, or we just did that
        User::RequestComplete(iClientStatus, KErrNone);
        iSqlSaveStatement.Close();
        CheckFlush();
	    }
	else
	    {
        iSaveLastPos = EFalse;
        // first close the statement, we can now reuse it
        iSqlSaveStatement.Close();
        
        TInt error = iSqlSaveStatement.Prepare(iDatabase, KUpsertLastPosRow);

        if(KErrNone == error)
            {
            TPckg<TPosition> positionDes(iLastKnownPosition);
            TInt indexStamp = iSqlSaveStatement.ParameterIndex(KStamp);
            TInt indexData = iSqlSaveStatement.ParameterIndex(KData);
            iSqlSaveStatement.BindInt64(indexStamp, iLastTime.Int64());
            iSqlSaveStatement.BindBinary(indexData, positionDes);
            
            // Statement is closed in RunL, once asynchrnous insert has taken place
            iStatus = KRequestPending;
            iSqlSaveStatement.Exec(iStatus);
            SetActive();      
            iLastKnownPosition = iLastPosition;
            }

	    }

	}


void CLbsLocMonitorDbEngine::DoCancel()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorDbEngine::DoCancel");
	*iClientStatus = KRequestPending;
	User::RequestComplete(iClientStatus, KErrCancel);
	}		
