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
#include "BTAsyStubObserver.h" 

// ----------------------------------------------------------------------------------
// CBTASYStubObserver* CBTASYStubObserver::NewL()
// ----------------------------------------------------------------------------------	

CBTASYStubObserver* CBTASYStubObserver::NewL(CBTASYStubService& aService)
{
	CBTASYStubObserver* result = new (ELeave) CBTASYStubObserver(aService);
	CleanupStack::PushL(result);	
	CleanupStack::Pop();
	return result;	
}

// ----------------------------------------------------------------------------------
// CBTASYStubObserver::CBTASYStubObserver() 
// ----------------------------------------------------------------------------------	
CBTASYStubObserver::CBTASYStubObserver(CBTASYStubService& aService) 
: CActive( CActive::EPriorityStandard ),iService(&aService)
	{	
	TInt retval(KErrNone);
						
    retval = RProperty::Define(KTFAccessoryTestProperty, KTFAccessoryMethod, RProperty::EByteArray);    
    if(retval != KErrAlreadyExists)
    	User::LeaveIfError(retval);       						
    
    retval =  iSubscribe.Attach(KTFAccessoryTestProperty, KTFAccessoryMethod);
    User::LeaveIfError(retval);    
       
    COMPONENT_TRACE( ( _L( "BTASYSTUB - CBTASYStubObserver::InitializeL() - Attach to P&S" ) ) );
	CActiveScheduler::Add( this );		    		     	
	COMPONENT_TRACE( ( _L( "BTASYSTUB - CBTASYStubObserver::CBTASYStubObserver - return" ) ) );
	}
//
// ----------------------------------------------------------------------------------
// CBTASYStubObserver::~CBTASYStubObserver()
// ----------------------------------------------------------------------------------	
CBTASYStubObserver::~CBTASYStubObserver()
	{				
    COMPONENT_TRACE( ( _L( "ASYStub - CTFTestControlObserver::~CBTASYStubObserver - succesful" ) ) );    
	}
//
// ----------------------------------------------------------------------------------
// CBTASYStubObserver::RunL()
// ----------------------------------------------------------------------------------
void CBTASYStubObserver::RunL()
    {                      
	COMPONENT_TRACE( ( _L( "BTASYSTUB - CBTASYStubObserver::RunL() - enter" ) ) );
	TInt retval(KErrNone);	
    TPckgBuf< TTFAccessoryPublishAndSubscribe > buf;
	retval = iSubscribe.Get( KTFAccessoryTestProperty, KTFAccessoryMethod, buf );	
	User::LeaveIfError(retval);			
	iService->Service ( buf().iMethod,
	                    buf().iParam1,
	                    buf().iTestCaseID,
	                    buf().iGenericID,
	                    buf().iParam2 );	
	if(!IsActive())
		{
		iStatus = KRequestPending;
		iSubscribe.Subscribe(iStatus);		
		SetActive();		
		}	
	COMPONENT_TRACE( ( _L( "BTASYSTUB - CBTASYStubObserver::RunL() - return" ) ) );		
    }        
//
// ----------------------------------------------------------------------------------
// CBTASYStubObserver::DoCancel()
// ----------------------------------------------------------------------------------
void CBTASYStubObserver::DoCancel()
    {
    }    

//
// ----------------------------------------------------------------------------------
// CBTASYStubObserver::DoCancel()
// ----------------------------------------------------------------------------------    
TInt CBTASYStubObserver::RunError( TInt aError )
{
	TInt errorCode(KErrNone);
	errorCode=aError;
	COMPONENT_TRACE( ( _L( "BTASYSTUB - CBTASYStubObserver::RunError() - Error code %d" ), errorCode) );
	return errorCode;
}
//
// ----------------------------------------------------------------------------------
// CBTASYStubObserver::IssueRequest
// ----------------------------------------------------------------------------------  
void CBTASYStubObserver::IssueRequest() 			  
{
	if(!IsActive())
		{
		iStatus = KRequestPending;
		iSubscribe.Subscribe(iStatus);		
		SetActive();		
		}
}


