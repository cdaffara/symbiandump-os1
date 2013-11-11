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
//

/**
 @file
 @InternalComponent
*/

#include "cpositionerq.h"
#include "cnetresponsehandler.h"
#include "lbsdevloggermacros.h"
#include "psypanic.h"
#include "utilfunctions.h"
#include "crequesthandler.h"
#include "tpositionercall.h"

// -----------------------------------------------------------------------------
// CNETResponseHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNETResponseHandler* CNETResponseHandler::NewL(CPositionerQ* aPositionerQ, CRequestHandler* aRequestHandler)
    {
    CNETResponseHandler* self = new( ELeave ) CNETResponseHandler(aPositionerQ, aRequestHandler);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CNETResponseHandler::CNETResponseHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNETResponseHandler::CNETResponseHandler(CPositionerQ* aPositionerQ, CRequestHandler* aRequestHandler)
    {
    iPositionerQ = aPositionerQ;
    iRequestHandler = aRequestHandler;
    }

CNETResponseHandler::~CNETResponseHandler()
    {
    delete iNETDataBus;
    }

/**
* construct the data bus by module ID and set the data bus observer
*/
void CNETResponseHandler::ConstructL()
	{
	iNETDataBus = CNETDataBus::NewL(static_cast<MDataBusObserver*>(this), KLbsNetLocManagerUid);
	
	Subscribe();
	}
	
void CNETResponseHandler::Subscribe()
	{
	iNETDataBus->Subscribe();
	}
	
/**
Location update

@param aPosition Position info from the data bus
@param aStatus status of the bus
@param aError error code
@param aActualTime position update time
*/
void CNETResponseHandler::DataUpdate(TPositionInfo& aPosition, TInt aStatus, TInt aError, TTime aActualTime, TUint /* aAttributes */, TUint /*aMode*/)
	{
	LBSLOG(ELogP1, "CNETResponseHandler::DataUpdate Start ");

	Subscribe();
	
	if(aStatus==KErrNone)
		{
		if((KErrNone == aError))
			{
			LBSLOG(ELogP1, "CNETResponseHandler::DataUpdate Valid ");
			
			TLocationUpdate call(aPosition, aError, aActualTime, 0);
			iPositionerQ->PositionerIterator(call);
			iRequestHandler->NotifyRequestComplete();
			}
		else if (aError < 0)
			{
			LBSLOG2(ELogP1, "CNETResponseHandler::DataUpdate -  Error: %d", aError);
			
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
	LBSLOG(ELogP1, "CNETResponseHandler::DataUpdate End ");
	}

/**
GetLastPosition

@param aPos	container for the position being returned (with KErrNone return)	
@param aOldestValidTime	 the oldest valid time for a position being returned. If there is a valid position no older than this time, it should be returned.
@param aAllowPartial	whether partial updates should be considered
@return ETrue if the fix fulfills the requirements, 
*/
TBool CNETResponseHandler::GetLastPosition(TPositionInfoBase& aPos, TTime aOldestValidTime, TBool aAllowPartial)
	{
	TBool ret = EFalse;

	TTime actualTime;
	TPositionInfo posInfo;
 	TPosition pos;	 	
 	TInt err;
	
 	err = iNETDataBus->GetLastPositionInfo(posInfo, actualTime);
	if( err == KErrNone && actualTime >= aOldestValidTime)	// got a recent enough position
		{
		posInfo.GetPosition(pos);
		if(aAllowPartial || !Partial(pos))// complete enough
			{
			TRAPD(error, CopyPositionTypes(aPos, posInfo));
			if(!error)
				{
				ret = ETrue;
				}
			else
				{
				LBSLOG(ELogP1, "CNetResponseHandler::GetLastPosition() failed to copy position ");
				ret = EFalse;
				}			
			}
		}	
	return ret;
	}
