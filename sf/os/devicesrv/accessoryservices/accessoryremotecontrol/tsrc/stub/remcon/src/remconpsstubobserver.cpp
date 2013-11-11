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
#include "RemConPSstubObserver.h" 
#include "RemConStub.h"
#include <accremconpstypes.h>
#include "siftrace.h"

// ----------------------------------------------------------------------------------
// CRemConPSstubObserver* CRemConPSstubObserver::NewL()
// ----------------------------------------------------------------------------------	

CRemConPSstubObserver* CRemConPSstubObserver::NewL(MRemConStubObserver& aObserver, TInt aTFControlKey)
{
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::NewL() - Enter" ) ) );
	CRemConPSstubObserver* result = new (ELeave) CRemConPSstubObserver(aObserver, aTFControlKey);
	CleanupStack::PushL(result);	
	CleanupStack::Pop();
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::NewL() - Return" ) ) );
	
	return result;	
}

// ----------------------------------------------------------------------------------
// CRemConPSstubObserver::CRemConPSstubObserver() 
// ----------------------------------------------------------------------------------	
CRemConPSstubObserver::CRemConPSstubObserver(MRemConStubObserver& aObserver, TInt aTFControlKey) 
: CActive( CActive::EPriorityStandard ), iObserver( aObserver )
	{						
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::CRemConPSstubObserver() - Enter" ) ) );
	TInt retval(KErrNone);
	
    retval =  iSubscribe.Attach(KTFRemConTestProperty, aTFControlKey);
    User::LeaveIfError(retval);       
	CActiveScheduler::Add( this );		    	
	iTFControlKey = aTFControlKey;
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::CRemConPSstubObserver() - Return" ) ) );
	}
//
// ----------------------------------------------------------------------------------
// CRemConPSstubObserver::~CRemConPSstubObserver()
// ----------------------------------------------------------------------------------	
CRemConPSstubObserver::~CRemConPSstubObserver()
	{				
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::~CRemConPSstubObserver() - Enter" ) ) );
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::~CRemConPSstubObserver() - Return" ) ) );
	}
//
// ----------------------------------------------------------------------------------
// CRemConPSstubObserver::RunL()
// ----------------------------------------------------------------------------------
void CRemConPSstubObserver::RunL()
    {                 
    
    COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::RunL() - Enter" ) ) );     
	TInt retval(KErrNone);	
	
	TRemConPckgStubBuf keyDataBuf;
	retval = iSubscribe.Get( KTFRemConTestProperty, iTFControlKey, keyDataBuf );	
	User::LeaveIfError(retval);			
    
    iObserver.TestCaseCommandReceived(iTFControlKey, keyDataBuf);
    
	if(!IsActive())
		{
		iStatus = KRequestPending;
		iSubscribe.Subscribe(iStatus);		
		SetActive();		
		}		
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::RunL() - Return" ) ) );		
    }        
//
// ----------------------------------------------------------------------------------
// CRemConPSstubObserver::DoCancel()
// ----------------------------------------------------------------------------------
void CRemConPSstubObserver::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::DoCancel() - Enter" ) ) );     
    
    if(IsActive())
    	{
    	iSubscribe.Cancel();	
    	}
    
    COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::DoCancel() - Return" ) ) );     
    }    

//
// ----------------------------------------------------------------------------------
// CASYReferenceObserver::DoCancel()
// ----------------------------------------------------------------------------------    
TInt CRemConPSstubObserver::RunError( TInt /*aError*/ )
{
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::RunError() - Enter" ) ) );     
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::RunError() - Return" ) ) );     
	return KErrNone;
}
//
// ----------------------------------------------------------------------------------
// CASYReferenceObserver::Start
// ----------------------------------------------------------------------------------  
void CRemConPSstubObserver::Start() 			  
{
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::Start() - Enter" ) ) );     
	if(!IsActive())
		{
		iSubscribe.Subscribe(iStatus);		
		SetActive();		
		}
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConPSstubObserver::Start() - Return") ) );     
}


