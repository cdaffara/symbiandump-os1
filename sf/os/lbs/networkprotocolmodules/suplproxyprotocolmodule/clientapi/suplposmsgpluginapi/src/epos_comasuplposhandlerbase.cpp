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



#include <ecom/ecom.h>
#include <badesca.h>

#include "epos_comasuplposition.h"
#include "epos_comasuplinforequestlist.h"
#include "epos_comasuplpospayload.h"
#include "epos_comasuplvelocity.h"
#include "epos_comasuplreqasstdata.h"

#include "epos_comasuplposhandlerbase.h"     // Our own base implementations for ECOM
#include "epos_comasuplpossessionbase.h"


//
// ---------------------------------------------------------
// Resets & destroys array if leave occurs
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void ResetAndDestroy(TAny* aArray)
    {
    	((RImplInfoPtrArray*)aArray)->ResetAndDestroy();
    }

// ============================ MEMBER FUNCTIONS ===============================

//
// ---------------------------------------------------------
// Interface's (abstract base class's) constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplPosHandlerBase::COMASuplPosHandlerBase()
    {
    }

//
// ---------------------------------------------------------
// Interface's (abstract base class's) destructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplPosHandlerBase::~COMASuplPosHandlerBase()
    {
    // Inform the ECOM framework that this specific instance of the 
    // interface has been destroyed.
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }
    
//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplPosHandlerBase
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  COMASuplPosHandlerBase* 
    COMASuplPosHandlerBase::NewL(const TUid &aImplementationUid)
    {

	//KPosHandlerInterfaceDefinitionUid is the UID of our custom ECOM interface.

	TAny* ptr = REComSession::CreateImplementationL(
								aImplementationUid,           
								_FOFF(COMASuplPosHandlerBase,iDtor_ID_Key)); 
		
	COMASuplPosHandlerBase* OMASuplPosHandler = REINTERPRET_CAST(COMASuplPosHandlerBase*, ptr);
	OMASuplPosHandler->SetImplementationUID(aImplementationUid);
	CleanupStack::PushL(OMASuplPosHandler);
	CleanupStack::Pop(OMASuplPosHandler);

	return OMASuplPosHandler;
	   
    }           



//
// -------------------------------------------------------------
// Determine the Implementation Version
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C TInt COMASuplPosHandlerBase::VersionL() const
	{

	RImplInfoPtrArray implInfoArray;
	CleanupStack::PushL(TCleanupItem(*ResetAndDestroy, &implInfoArray));
	REComSession::ListImplementationsL(KPosHandlerInterfaceDefinitionUid, implInfoArray);
		
	TInt version = KErrNotFound;
	TInt cnt = implInfoArray.Count();
		
	for (TInt index = 0; index < cnt; index++)
		{
		if (implInfoArray[index]->ImplementationUid() == iImplementationUID)
			{
				version = implInfoArray[index]->Version();
				break;
			}
		}

	CleanupStack::PopAndDestroy(&implInfoArray);

	return version;
	}

//
// -------------------------------------------------------------
// ImplementationUID
// Returns UID of implementation
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  TUid COMASuplPosHandlerBase::ImplementationUID() const
	{
		return iImplementationUID;
	}

// -------------------------------------------------------------
// Sets the Implementation UID
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
void COMASuplPosHandlerBase::SetImplementationUID(const TUid &aUID)
	{
		iImplementationUID = aUID;
	}
	
