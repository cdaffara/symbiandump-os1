/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#include "BTASYStubMainServiceBase.h"

#include "BTAsyStubObserver.h"
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
CBTASYStubMainServiceBase::CBTASYStubMainServiceBase()
	{

	}

//
// ----------------------------------------------------------------------------------
// CBTASYStubMainServiceBase::NewL()
// ----------------------------------------------------------------------------------
CBTASYStubMainServiceBase* CBTASYStubMainServiceBase::NewL()
	{
	return new ( ELeave ) CBTASYStubMainServiceBase();
	}

//
// ----------------------------------------------------------------------------------
// CBTASYStubMainServiceBase::~CBTASYStubMainServiceBase()
// ----------------------------------------------------------------------------------
CBTASYStubMainServiceBase::~CBTASYStubMainServiceBase()
	{
	}

//
// ----------------------------------------------------------------------------------
// CBTASYStubMainServiceBase::StartL()
// ----------------------------------------------------------------------------------
TInt CBTASYStubMainServiceBase::StartL()
{
	iService = new ( ELeave ) CBTASYStubService(); 
    CleanupStack::PushL( iService );
    iService->ConstructL(); 
    CleanupStack::Pop(); 
    CBTASYStubObserver* subscribeObserver = CBTASYStubObserver::NewL(*iService);
	subscribeObserver->IssueRequest();
	
	this->Signal();
	return KErrNone;
}

//
// ----------------------------------------------------------------------------------
// CASYStubMainServiceBase::AsyStubMainServicePointerReceivedL()
// ----------------------------------------------------------------------------------
TInt CBTASYStubMainServiceBase::GetTestCaseID()
{
	COMPONENT_TRACE( ( _L( "BTASYSTUB - CBTASYStubMainServiceBase::GetTestCaseID()" ) ) );
	return iService->GetTestCaseID();
}

