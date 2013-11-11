/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "ASYStubMainServiceBase.h"

#include "AsyStubObserver.h"
#include "siftrace.h"

#ifndef EKA2
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }
#endif

//
// ----------------------------------------------------------------------------------
// CASYStubMainServiceBase::CASYStubMainServiceBase()
// ----------------------------------------------------------------------------------
//  
CASYStubMainServiceBase::CASYStubMainServiceBase()
	{

	}

//
// ----------------------------------------------------------------------------------
// CASYStubMainServiceBase::NewL()
// ----------------------------------------------------------------------------------
CASYStubMainServiceBase* CASYStubMainServiceBase::NewL()
	{
	return new ( ELeave ) CASYStubMainServiceBase();
	}

//
// ----------------------------------------------------------------------------------
// CASYStubMainServiceBase::~CASYStubMainServiceBase()
// ----------------------------------------------------------------------------------
CASYStubMainServiceBase::~CASYStubMainServiceBase()
	{
	}

//
// ----------------------------------------------------------------------------------
// CASYStubMainServiceBase::StartL()
// ----------------------------------------------------------------------------------
TInt CASYStubMainServiceBase::StartL()
{
	iService = new ( ELeave ) CASYStubService(); 
    CleanupStack::PushL( iService );
    iService->ConstructL(); 
    CleanupStack::Pop(); 
    CASYStubObserver* subscribeObserver = CASYStubObserver::NewL(*iService);
	subscribeObserver->IssueRequest();
	this->Signal();
	return KErrNone;
}

//
// ----------------------------------------------------------------------------------
// CASYStubMainServiceBase::AsyStubMainServicePointerReceivedL()
// ----------------------------------------------------------------------------------
void CASYStubMainServiceBase::AsyStubMainServicePointerReceivedL()
{
	COMPONENT_TRACE( ( _L( "ASYStub - CASYStubMainServiceBase::AsyStubMainServicePointerReceivedL() - MAINSERVICEBASE POINTER RECEIVED!" ) ) );
}

//
// ----------------------------------------------------------------------------------
// CASYStubMainServiceBase::GetTestCaseID()
// ----------------------------------------------------------------------------------
TInt CASYStubMainServiceBase::GetTestCaseID()
{
	COMPONENT_TRACE( ( _L( "ASYStub - CASYStubMainServiceBase::GetTestCaseID()" ) ) );
	return iService->GetTestCaseID();
}

//
// ----------------------------------------------------------------------------------
// CASYStubMainServiceBase::GetAndRemoveCapability()
// ----------------------------------------------------------------------------------
TTFCapability CASYStubMainServiceBase::GetAndRemoveCapability()
{
	COMPONENT_TRACE( ( _L( "ASYStub - CASYStubMainServiceBase::GetAndRemoveCapability()" ) ) );
	return iService->GetAndRemoveCapability();
}


 


