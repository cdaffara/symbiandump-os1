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
// responsehandler.cpp
// 
//

/**
 @file
 @InternalComponent
*/

#include "cpositionerq.h"
#include "cagpsresponsehandler.h"
#include "lbsdevloggermacros.h"
#include "psypanic.h"
#include "utilfunctions.h"
#include "cpositionerq.h"
#include "crequesthandler.h"
#include "tpositionercall.h"

/*
* CAgpsResponseHandler::NewL
* Two-phased constructor.
*/
CAgpsResponseHandler* CAgpsResponseHandler::NewL(CPositionerQ* aPositionerQ, CRequestHandler* aRequestHandler)
    {
    CAgpsResponseHandler* self = new( ELeave ) CAgpsResponseHandler(aPositionerQ, aRequestHandler);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

/*
* CAgpsResponseHandler::CAgpsResponseHandler
* C++ default constructor can NOT contain any code, that
* might leave.
*/
CAgpsResponseHandler::CAgpsResponseHandler(CPositionerQ* aPositionerQ, CRequestHandler* aRequestHandler)
    {
    iPositionerQ = aPositionerQ;
    iRequestHandler = aRequestHandler;
    }

CAgpsResponseHandler::~CAgpsResponseHandler()
    {
    delete iAGPSDataBus;
    delete iFinalNetDataBus;
	delete iRefDataBus;
	}

/**
* construct the data bus by module ID and set the data bus observer
*/
void CAgpsResponseHandler::ConstructL()
	{
	iAGPSDataBus = CAgpsDataBus::NewL(*this, KLbsGpsLocManagerUid);
	iFinalNetDataBus = CFinalNetDataBus::NewL(*this);
	iRefDataBus = CRefDataBus::NewL(*this);
	
   	Subscribe();
	}
	
void CAgpsResponseHandler::Subscribe()
	{
	iAGPSDataBus->Subscribe();
	iFinalNetDataBus->Subscribe();
	iRefDataBus->Subscribe();
	}
	
/**
Location update

@param aPosition Position info from the data bus
@param aStatus status of the bus
@param aError error code
@param aActualTime position update time
*/
void CAgpsResponseHandler::DataUpdate(TPositionInfo& aPosition, TInt aStatus, TInt aError, TTime aActualTime, TUint aAttributes, TUint aMode)
	{
	LBSLOG(ELogP1, "CAgpsResponseHandler::DataUpdateL Start ");

	Subscribe();
	
	if(aStatus==KErrNone)
		{
		TModeUpdate mode(aMode);
		iPositionerQ->PositionerIterator(mode);
		
		// if this is valid data then we need to tell the positioner
		if((KErrNone == aError))
			{
			LBSLOG(ELogP1, "CAgpsResponseHandler::DataUpdateL Valid ");
			
			TLocationUpdate call(aPosition, aError, aActualTime, aMode);
			iPositionerQ->PositionerIterator(call);
			
			iRequestHandler->NotifyRequestComplete();
			}
		else if (KPositionCalculationFutile == aError)
			{
			LBSLOG(ELogP1, "CAgpsResponseHandler::DataUpdateL Futile update ");
			if(!(aAttributes & RLbsPositionUpdates::ESelfLocateSessionInProgress))//nb: futile updates ignored during session as manager will re-publish on session complete
				{
				// If there's a valid FNP we should use that, otherwise use this futile update
				TPositionInfo FnpPosInfo;
				TTime actualTime;
				TInt err = iFinalNetDataBus->GetLastPositionInfo(FnpPosInfo, actualTime);
				if(err == KErrNone && actualTime > aActualTime)
					{
					LBSLOG(ELogP1, "CAgpsResponseHandler::DataUpdateL using FNP");
					TLocationUpdate call(FnpPosInfo, aError, aActualTime, aMode);
					iPositionerQ->PositionerIterator(call);
					iRequestHandler->NotifyRequestComplete();
					}
				else
					{
					TLocationUpdate call(aPosition, aError, aActualTime, aMode);
					iPositionerQ->PositionerIterator(call);
					iRequestHandler->NotifyRequestComplete();
					}
				}
			else
				{
				LBSLOG(ELogP1, "CAgpsResponseHandler::DataUpdateL Ignoring Futile update during self locate session");
				}
			}
		else if (aError < 0)
			{
			LBSLOG2(ELogP1, "CAgpsResponseHandler::DataUpdateL -  Error: %d", aError);
			
			TUpdateFailed call(aError);
			iPositionerQ->PositionerIterator(call);
			iRequestHandler->NotifyRequestComplete();
			}
		else
			{
			// 'Futile' is the only +ve error code we currently have, so this shouldn't happen
			__ASSERT_DEBUG(EFalse, User::Panic(KAdaptationPanicCategory, EPanicUnknownUpdateError));
			}
		}
	LBSLOG(ELogP1, "CAgpsResponseHandler::DataUpdateL End ");
	}

/**
GetLastPosition

Retrieves the most recent position that's less than the supplied max age if there is one, 
including partial positions if partial allowed

@param aPos	container for the position being returned (with KErrNone return)	
@param aOldestValidTime	 the oldest valid time for a position being returned. If there is a valid position no older than this time, it should be returned.
@param aAllowPartial	whether partial updates should be considered
@return ETrue if the fix fulfills the requirements, 
*/
TBool CAgpsResponseHandler::GetLastPosition(TPositionInfoBase& aPos, TTime aOldestValidTime, TBool aAllowPartial)
	{
	TBool ret = EFalse;
	TTime actualTime, mostRecent;
	TPositionSatelliteInfo satPosInfo;
	TPositionInfo refPosInfo, FnpPosInfo;
 	TPosition pos;
 	TPositionChoice posChoice = ENone; 	 	
 	TInt err;
 	TUint attributes;
 	TUint mode = 0;
 	
 	err = iAGPSDataBus->GetLastPositionInfo(satPosInfo, mostRecent, attributes, mode);
	if( err == KErrNone && mostRecent >= aOldestValidTime)	// got a recent enough gps position
		{
		satPosInfo.GetPosition(pos);
		if(aAllowPartial || !Partial(pos))// complete enough
			{
			posChoice = EAGPSPos;
			}
		}
	err = iRefDataBus->GetLastPositionInfo(refPosInfo, actualTime);
	if( err == KErrNone && actualTime >= aOldestValidTime) // got a recent enough ref position
		{
		refPosInfo.GetPosition(pos);
		if(aAllowPartial || !Partial(pos))// complete enough
			{
			if(posChoice == ENone || actualTime > mostRecent)
				{
				posChoice = ERefPos;
				}
			}		
		}
	err = iFinalNetDataBus->GetLastPositionInfo(FnpPosInfo, actualTime);
	if( err == KErrNone && actualTime >= aOldestValidTime)  // got a recent enough fnp position
		{
		FnpPosInfo.GetPosition(pos);
		if(aAllowPartial || !Partial(pos))// complete enough
			{
			if(posChoice == ENone || actualTime > mostRecent )
				{
				posChoice = EFNPPos;
				}
			}		
		}

	TPositionInfo* chosenPosInfo(NULL);
	
	switch(posChoice)
		{
		case ENone:
			break;	
		case EAGPSPos:
			{
			chosenPosInfo = &satPosInfo;
			break;
			}
		case ERefPos:
			{
			chosenPosInfo = &refPosInfo;
			break;
			}
			
		case EFNPPos:
			{
			chosenPosInfo = &FnpPosInfo;
			break;
			}
		default:
			__ASSERT_DEBUG(EFalse, User::Invariant());
		}
		
	if(chosenPosInfo != NULL)
		{
		TRAPD(error, CopyPositionTypes(aPos, *chosenPosInfo));
		if(!error)
			{
			ret = ETrue;
			}
		else
			{
			LBSLOG(ELogP1, "CAgpsResponseHandler::GetLastPosition() failed to copy position ");
			ret = EFalse;
			}
		}
	
	return ret;
	}

