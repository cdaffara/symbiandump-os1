/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Provides the interface between SUPL Framework and POS
*				 Message plugin. 
*
*/



#include <badesca.h>

#include "epos_comasuplinforequestlist.h"


// ============================ MEMBER FUNCTIONS OF EPos_COMASuplInfoRequestList ===============================

//
// ---------------------------------------------------------
// Interface's constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplInfoRequestList::COMASuplInfoRequestList() 
	{
		// Reset the Array
		iInfoRequestList.ResetAndDestroy();
    }
    

//
// ---------------------------------------------------------
// Interface's destructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplInfoRequestList::~COMASuplInfoRequestList() 
    {
    	// Clear the array
    	iInfoRequestList.ResetAndDestroy();
    }


//
// ---------------------------------------------------------
// Interface's NewL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplInfoRequestList* COMASuplInfoRequestList::NewL()
    {
    COMASuplInfoRequestList* self = new( ELeave ) COMASuplInfoRequestList;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
//
// ---------------------------------------------------------
// Interface's Two phase construction method
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void COMASuplInfoRequestList::ConstructL()
    {
    }

//
// ---------------------------------------------------------
// Interface's Append Method
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt COMASuplInfoRequestList::Append(COMASuplInfoRequest *aInfoRequest)
    {
    	return iInfoRequestList.Append(aInfoRequest);
    }
    

// ---------------------------------------------------------
// Interface's ListCount
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt COMASuplInfoRequestList::GetCount()
    {
    	return iInfoRequestList.Count();
    }
    
// ---------------------------------------------------------
// Interface's GetElement
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplInfoRequest *COMASuplInfoRequestList::GetElement(TInt aIndex)
    {
    	if(aIndex >= 0 && aIndex < iInfoRequestList.Count())
    	{
    		return iInfoRequestList[aIndex];	
    	}
    	
    	return NULL;
    }
    

// ============================ MEMBER FUNCTIONS OF COMASuplInfoRequest ===============================
//
// ---------------------------------------------------------
// Interface's NewL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplInfoRequest* COMASuplInfoRequest::NewL()
    {
    COMASuplInfoRequest* self = new( ELeave ) COMASuplInfoRequest;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

//
// ---------------------------------------------------------
// Interface's NewL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplInfoRequest::COMASuplInfoRequest()
    {
    }
 
//
// ---------------------------------------------------------
// Interface's Two phase construction method
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void COMASuplInfoRequest::ConstructL()
    {
       	iReqStatus = KErrNone;
    }

//
// ---------------------------------------------------------
// Interface's Destructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplInfoRequest::~COMASuplInfoRequest()
    {
    }

//
// ---------------------------------------------------------
// Interface's Get Type method
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplInfoRequest::TOMASuplInfoType COMASuplInfoRequest::Type() const
    {
    	return iType;
    }


//
// ---------------------------------------------------------
// Interface's Get Status Method
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt COMASuplInfoRequest::Status() const
    {
    	return iReqStatus;
    }


//
// ---------------------------------------------------------
// Interface's SetType Method
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void COMASuplInfoRequest::SetType(TOMASuplInfoType aInfoType)
    {
    	iType = aInfoType;
    }

//
// ---------------------------------------------------------
// Interface's Two phase construction method
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void COMASuplInfoRequest::SetStatus(TInt aStatus)
    {
    	iReqStatus = aStatus;
    }

// -----------------------------------------------------------------------------
// COMASuplInfoRequest::CloneL() 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
EXPORT_C COMASuplInfoRequest* COMASuplInfoRequest::CloneL()const
	{
	COMASuplInfoRequest*	cloneInfoRequest = COMASuplInfoRequest::NewL();

	cloneInfoRequest->iReqStatus = iReqStatus;
	cloneInfoRequest->iType = iType;

	return (cloneInfoRequest);
	}
