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


// INCLUDE FILES
#include <TfAccessoryTestControlTypes.h>
#include "siftrace.h"
#include "AsyStubObserver.h" 

// ----------------------------------------------------------------------------------
// CASYStubObserver* CASYStubObserver::NewL()
// ----------------------------------------------------------------------------------	

CASYStubObserver* CASYStubObserver::NewL(CASYStubService& aService)
{
	CASYStubObserver* result = new (ELeave) CASYStubObserver(aService);
	CleanupStack::PushL(result);	
	CleanupStack::Pop();
	return result;	
}

// ----------------------------------------------------------------------------------
// CASYStubObserver::CASYStubObserver() 
// ----------------------------------------------------------------------------------	
CASYStubObserver::CASYStubObserver(CASYStubService& aService) 
: CActive( CActive::EPriorityStandard ),iService(&aService)
	{						
	TInt retval(KErrNone);
	retval = RProperty::Define(KTFAccessoryTestProperty, KTFAccessoryMethod, RProperty::EByteArray);    
    if(retval != KErrAlreadyExists)
    	User::LeaveIfError(retval);       
    
    COMPONENT_TRACE( ( _L( "ASYStub - CASYStubObserver::InitializeL() - Method attribute set to P&S" ) ) );
    retval =  iSubscribe.Attach(KTFAccessoryTestProperty, KTFAccessoryMethod);
    User::LeaveIfError(retval);       
    COMPONENT_TRACE( ( _L( "ASYStub - CASYStubObserver::InitializeL() - Attach to P&S" ) ) );
    
	CActiveScheduler::Add( this );		    		     	
	COMPONENT_TRACE( ( _L( "ASYStub - CASYStubObserver::CASYStubObserver - return" ) ) );
	}
//
// ----------------------------------------------------------------------------------
// CASYStubObserver::~CASYStubObserver()
// ----------------------------------------------------------------------------------	
CASYStubObserver::~CASYStubObserver()
	{				
    COMPONENT_TRACE( ( _L( "ASYStub - CTFTestControlObserver::~CASYStubObserver - succesful" ) ) );    
	}
//
// ----------------------------------------------------------------------------------
// CASYStubObserver::RunL()
// ----------------------------------------------------------------------------------
void CASYStubObserver::RunL()
    {                      
	COMPONENT_TRACE( ( _L( "ASYStub - CASYStubObserver::RunL() - enter" ) ) );
	TInt retval(KErrNone);	
    TPckgBuf< TTFAccessoryPublishAndSubscribe > buf;
	retval = iSubscribe.Get( KTFAccessoryTestProperty, KTFAccessoryMethod, buf );	
	User::LeaveIfError(retval);			
	
	
	iService->Service( buf().iTestCaseID,
	                   buf().iMethod,
	                   buf().iParam1,
	                   buf().iParam2, 
	                   buf().iTimeMs );	
	if(!IsActive())
		{
		iStatus = KRequestPending;
		iSubscribe.Subscribe(iStatus);		
		SetActive();		
		}	
	COMPONENT_TRACE( ( _L( "ASYStub - CASYStubObserver::RunL() - return" ) ) );		
    }        
//
// ----------------------------------------------------------------------------------
// CASYStubObserver::DoCancel()
// ----------------------------------------------------------------------------------
void CASYStubObserver::DoCancel()
    {
    }    

//
// ----------------------------------------------------------------------------------
// CASYStubObserver::DoCancel()
// ----------------------------------------------------------------------------------    
TInt CASYStubObserver::RunError( TInt aError )
{
	TInt errorCode(KErrNone);
	errorCode=aError;
	COMPONENT_TRACE( ( _L( "ASYStub - CASYStubObserver::RunError() - Error code %d" ), errorCode) );
	return errorCode;
}
//
// ----------------------------------------------------------------------------------
// CASYStubObserver::IssueRequest
// ----------------------------------------------------------------------------------  
void CASYStubObserver::IssueRequest() 			  
{
	if(!IsActive())
		{
		iStatus = KRequestPending;
		iSubscribe.Subscribe(iStatus);		
		SetActive();		
		}
}


