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


#include <e32base.h>
#include <e32debug.h>
#include <e32cmn.h>
#include <lbsposition.h>
#include "rlbslocmonitorareapositioner.h"


EXPORT_C RLbsLocMonitorAreaPositioner::RLbsLocMonitorAreaPositioner()
  : RSecureSubSessionBase(), 
    iPtrHolder(NULL)
	{
	}
	
EXPORT_C void RLbsLocMonitorAreaPositioner::ConstructL()
	{
	iPtrHolder = CLbsLocMonitorPtrHolder::NewL(CLbsLocMonitorPtrHolder::ELastParam, 0);
	}

EXPORT_C void RLbsLocMonitorAreaPositioner::Destruct()
	{
	delete iPtrHolder;
	iPtrHolder = NULL;
	}

EXPORT_C void RLbsLocMonitorAreaPositioner::OpenL(RLbsLocMonitorSession& aLocMonSession)
	{
	__ASSERT_ALWAYS(aLocMonSession.Handle() != 0, User::Invariant()); // trying to open twice
	TInt err(KErrNone);
	ConstructL();
	
	// Call Server Framework 'Open'
	TLbsLocMonitorAreaPositionerData emptyData;
	TLbsLocMonitorSubSessionType type(ELocMonitorSubSessionTypeAreaPositioner);
	err = RSecureSubSessionBase::Open<TLbsLocMonitorSubSessionType, TLbsLocMonitorAreaPositionerData>(aLocMonSession, type, emptyData);
	if (err != KErrNone)
		{
		Destruct();
		}
	User::LeaveIfError(err);
	}

EXPORT_C void RLbsLocMonitorAreaPositioner::Close()
	{
	// Call Server Framework 'Close'
	RSecureSubSessionBase::Close();
	Destruct();
	}

/**
Method called by the Location Server to send the Last Known Position to the Location Monitor
for storing together with Local Area information if possible.

@param aPosInfo the position object to be sent to the server (only the TPositionInfo portion 
		will be sent)
@return KErrNone if position is succesfully sent to server
		KErrArgument if aPosInfo is not a TPositionInfo class or derived class.
		Any of the error codes returned by RSessionBase::SendReceive
*/
EXPORT_C TInt RLbsLocMonitorAreaPositioner::SetLastKnownPosition(const TPositionInfoBase& aPosInfo) const
	{
	__ASSERT_ALWAYS(SubSessionHandle(), User::Invariant());
	__ASSERT_ALWAYS(iPtrHolder != NULL, User::Invariant());
	// Verify aPosInfo is of a type that contains a position
	const TPositionInfoBase& infoBase = static_cast<const TPositionInfoBase&>(aPosInfo);
    TUint32 classType = infoBase.PositionClassType();
    if (!(classType & EPositionInfoClass))
        {
        return KErrArgument;
        }

	// Get the TPosition in aPosInfo and send to the Location Monitor
	const TPositionInfo& posInfo = static_cast<const TPositionInfo&>(aPosInfo);
	TPosition position;
	posInfo.GetPosition(position);

	TPckgC<TPosition> posPckg(position);
	return SendReceive(ESetLastKnownPosition, TIpcArgs(&posPckg));
	}


/* Retrieve the position last stored in the database */
EXPORT_C void RLbsLocMonitorAreaPositioner::GetLastKnownPosition(TPositionInfoBase& aPosInfo,
                                	 TRequestStatus& aStatus) const
	{
	__ASSERT_ALWAYS(SubSessionHandle(), User::Invariant());
	__ASSERT_ALWAYS(iPtrHolder != NULL, User::Invariant());
	
	// Ensure aPosInfo output parameter is at TPositionInfo or derived class
	const TPositionInfoBase& infoBase = static_cast<const TPositionInfoBase&>(aPosInfo);
    TUint32 classType = infoBase.PositionClassType();
    if (!(classType & EPositionInfoClass))
        {
        TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrArgument);
		return;
        }

	iPtrHolder->Ptr(CLbsLocMonitorPtrHolder::EPosForGetLastKnownPosition).Set(
		reinterpret_cast<TUint8*>(&aPosInfo),
		aPosInfo.PositionClassSize(),
		aPosInfo.PositionClassSize());

	SendReceive(EGetLastKnownPosition,
				TIpcArgs(&iPtrHolder->Ptr(CLbsLocMonitorPtrHolder::EPosForGetLastKnownPosition)),
						aStatus);
	}                              

EXPORT_C TInt RLbsLocMonitorAreaPositioner::CancelGetLastKnownPosition() const
	{
	__ASSERT_ALWAYS(SubSessionHandle(), User::Invariant());

	return SendReceive(ECancelGetLastKnownPosition);
	}


/**
Returns in aPosInfo a position which had been previously stored 
in association with "context information" that is bound to a 
geographical area (e.g. Global Cell Id).

Current context information will be sensed by the server and used
to search for a position in the database. If found, the position 
will be an approximation of the current position of the handset.
If no position is found in the database for the current context
information, the position last stored in the database is returned.

The method returns in aAreaInfo a rough indication of how accurate the
returned position is based on an estimation of the size of the
geographical area covered by the sensed context information.
*/
EXPORT_C void RLbsLocMonitorAreaPositioner::GetLastKnownPositionArea(TPositionInfoBase& aPosInfo,
				      				 TPositionAreaInfoBase& aAreaInfo,
				      				 TPosAreaReqParams& aParameters,
                                	 TRequestStatus& aStatus) const
	{
	__ASSERT_ALWAYS(SubSessionHandle(), User::Invariant());
	__ASSERT_ALWAYS(iPtrHolder != NULL, User::Invariant());

	// Ensure aPosInfo output parameter is at TPositionInfo or derived class
	const TPositionInfoBase& infoBase = static_cast<const TPositionInfoBase&>(aPosInfo);
    TUint32 classType = infoBase.PositionClassType();
    if (!(classType & EPositionInfoClass))
        {
        TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrArgument);
		return;
        }
	 
	iPtrHolder->Ptr(CLbsLocMonitorPtrHolder::EPosForGetLastKnownPositionArea).Set(
		reinterpret_cast<TUint8*>(&aPosInfo),
		aPosInfo.PositionClassSize(),
		aPosInfo.PositionClassSize());

	iPtrHolder->Ptr(CLbsLocMonitorPtrHolder::EAreaForGetLastKnownPositionArea).Set(
		reinterpret_cast<TUint8*>(&aAreaInfo),
		aAreaInfo.PositionClassSize(),
		aAreaInfo.PositionClassSize());

	iPtrHolder->Ptr(CLbsLocMonitorPtrHolder::EParamsForGetLastKnownPositionArea).Set(
		reinterpret_cast<TUint8*>(&aParameters),
		sizeof(TPosAreaReqParams),
		sizeof(TPosAreaReqParams));
		  
	SendReceive(EGetLastKnownPositionArea,
	 	 		TIpcArgs(&iPtrHolder->Ptr(CLbsLocMonitorPtrHolder::EPosForGetLastKnownPositionArea),
	 	 				&iPtrHolder->Ptr(CLbsLocMonitorPtrHolder::EAreaForGetLastKnownPositionArea),
	 	 				&iPtrHolder->Ptr(CLbsLocMonitorPtrHolder::EParamsForGetLastKnownPositionArea)),
	 	 				aStatus);
	}

EXPORT_C TInt RLbsLocMonitorAreaPositioner::CancelGetLastKnownPositionArea() const
	{
	__ASSERT_ALWAYS(SubSessionHandle(), User::Invariant());
	return SendReceive(ECancelGetLastKnownPositionArea);
	}

