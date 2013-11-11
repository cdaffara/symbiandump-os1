// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "EPos_CPosControllerBase.h"
#include "EPos_CPosSimulationPositioner.h"
#include <e32debug.h>

//consts

const TInt KDefaultReadInterval(1000000); //1 second

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosControllerBase::CPosControllerBase(TBool aTimeRelativePlayback)
: CActive(EPriorityStandard), iTimeRelativePlayback(aTimeRelativePlayback),
iTimeBetweenReads(KDefaultReadInterval)
    {
    }

// Destructor
CPosControllerBase::~CPosControllerBase()
    {
    CancelAllNotifyPositionUpdate();
    iSimulationPositioners.Close();
    }

void CPosControllerBase::CompleteRequest(const TInt aIndex, TInt aErrorCode)
	{
	if (aIndex > KErrNotFound && aIndex < iSimulationPositioners.Count())
	    {
	    TRequestStatus& status = iSimulationPositioners[aIndex].iReqStatus;
		iSimulationPositioners[aIndex].iSimPos.CompleteRequest(status, aErrorCode);
		iSimulationPositioners.Remove(aIndex);
	    }
	else
		{
		User::Panic(_L("iSimulationPositioners corrupted"), KErrCorrupt);
		}
	}

// ----------------------------------------------------------------------------
// CPosNmeaController::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosControllerBase::CancelNotifyPositionUpdate(const CPosSimulationPositioner& aSimPos)
    {
    TInt index = FindPositioner(aSimPos);
    if (index > KErrNotFound)
    	{
		TRequestStatus& rs = iSimulationPositioners[index].iReqStatus;
		CompleteRequest(index, KErrCancel);
    	}
    }

void CPosControllerBase::CancelAllNotifyPositionUpdate()
    {
    while ( iSimulationPositioners.Count() > 0 )
    	{
    	//complete always first of iSimulationPositioner
    	//because CompleteRequest deletes completed positioner
    	CompleteRequest(0, KErrCancel);
    	}
//    for(TInt index=0; index < iSimulationPositioners.Count(); index++)
//    	{
//		CompleteRequest(index, KErrCancel);
//		index--;
//    	}
    }

void CPosControllerBase::NotifyPositionUpdateBase(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus, CPosSimulationPositioner& aSimPos)
	{
    aStatus = KRequestPending;
    CPosSimulationPositioner::ClearAndSetDefaultPositionData(aPosInfo);
    TPositionerReference ref(aSimPos,aStatus, static_cast<TPositionInfo&>(aPosInfo));
    TInt err = iSimulationPositioners.Append(ref);
    if (err != KErrNone)
    	{
    	//not using CopleteRequest(const CPosSimulationPositioner&, TRequestStatus&, TInt)
    	//because adding positioner to array was unsuccessful then we can't remove it 
    	//from it. It could cause PANIC
    	aSimPos.CompleteRequest(aStatus, err);
    	}
	}

void CPosControllerBase::CompleteRequestBase(TInt aErrorCode)
	{
	TPosition pos;
	iPosition.GetPosition(pos);
	TInt i = 0;
	while (i < iSimulationPositioners.Count())
		{
		if (aErrorCode != KPositionPartialUpdate
				|| iSimulationPositioners[i].iSimPos.PartialUpdateAllowed())
			{
			iSimulationPositioners[i].iPosition.SetPosition(pos);
			CompleteRequest(i, aErrorCode);
			//complete request removes current position from iSimulationPositioners
			//then wee need to not modify iterator
			}
		else
			{
			i++;
			}
		}
//	for(TInt i=0; i < iSimulationPositioners.Count(); i++)
//		{
//		if(aErrorCode!=KPositionPartialUpdate || 
//				iSimulationPositioners[i].iSimPos.PartialUpdateAllowed() )
//			{
//			iSimulationPositioners[i].iPosition.SetPosition(pos);
//			CompleteRequest(i, aErrorCode);
//			//complete request removes current position from iSimulationPositioners
//			//then wee need to modify iterator
//			i--;
//			}
//		}

	CPosSimulationPositioner::ClearAndSetDefaultPositionData(iPosition);
	}

TInt CPosControllerBase::FindPositioner(const CPosSimulationPositioner& aSimPos)
	{
	TInt ret = KErrNotFound;
	for(TInt i = 0; i < iSimulationPositioners.Count(); i++)
		{
		if( &iSimulationPositioners[i].iSimPos == &aSimPos)
			{
			ret = i;
			break;
			}
		}
	return ret;
	}

//  End of File
