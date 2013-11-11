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

#include "epos_comasuplpospayload.h"


// ============================ MEMBER FUNCTIONS of COMASuplPosPayload===============================

//
// ---------------------------------------------------------
//  constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
COMASuplPosPayload::COMASuplPosPayload()
    {
    	SetType(COMASuplInfoRequest::EOMASuplPosPayload);
    }

//
// ---------------------------------------------------------
//  destructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplPosPayload::~COMASuplPosPayload()
    {
    delete iPayload;
    }
    
//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  COMASuplPosPayload* COMASuplPosPayload::NewL()
    {
    COMASuplPosPayload* self = new(ELeave) COMASuplPosPayload;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}           

//
// -------------------------------------------------------------
// Two Phase constructor
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
void COMASuplPosPayload::ConstructL()
    {
	}           
     
//
// -------------------------------------------------------------
// Retrieves the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  void COMASuplPosPayload::GetPosPayload(HBufC8*& aPayload,
									TOMASuplPosPayloadType& aPayloadType) const
    {
 	aPayload = iPayload;
 	aPayloadType = iPayloadType;
    }           
//
// -------------------------------------------------------------
// Sets the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//

EXPORT_C void COMASuplPosPayload::SetPosPayload(HBufC8* aPayload,
												TOMASuplPosPayloadType aPayloadType)
    {
    delete iPayload;
    iPayload = NULL;

	iPayload = aPayload;
	iPayloadType = aPayloadType;
    } 

// -----------------------------------------------------------------------------
// COMASuplPosPayload::CloneL() 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
EXPORT_C COMASuplInfoRequest* COMASuplPosPayload::CloneL()const
	{
	COMASuplPosPayload*	clonePayload = COMASuplPosPayload::NewL();
	CleanupStack::PushL(clonePayload);

	clonePayload->iPayloadType = iPayloadType;
	if(iPayload)
		{
			clonePayload->iPayload = HBufC8::NewL(iPayload->Length());
			*clonePayload->iPayload = *iPayload;
		}
	
	clonePayload->SetStatus(Status());
    clonePayload->SetType(Type());	
	CleanupStack::Pop(clonePayload);
	return (clonePayload);
	}

// -----------------------------------------------------------------------------
// COMASuplPosPayload::SetSuplPayloadType() 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
EXPORT_C void COMASuplPosPayload::SetSuplPayloadType(TOMASuplPosPayloadType aPayLoadType)
	{
		iPayloadType = aPayLoadType;
	}

// -----------------------------------------------------------------------------
// COMASuplPosPayload::SuplPayloadType() 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
EXPORT_C COMASuplPosPayload::TOMASuplPosPayloadType COMASuplPosPayload::SuplPayloadType() const
	{
		return iPayloadType;
	}

