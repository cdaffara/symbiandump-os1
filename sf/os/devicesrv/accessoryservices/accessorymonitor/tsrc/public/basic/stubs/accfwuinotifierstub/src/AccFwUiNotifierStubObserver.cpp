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


#include "AccFWUINotifierStubObserver.h"
#include "AccFwUiDialogNotifierStub.h"

#include "TfAccessoryTestControlTypes.h"

  	  
CAccFWUINotifierStubObserver* CAccFWUINotifierStubObserver::NewL( CAccFwUiDialogNotifierStub* /*aService*/ )
{
/*	CAccFWUINotifierStubObserver * self = new (ELeave) CAccFWUINotifierStubObserver( aService );
	CleanupStack::PushL( self );
	
	TInt err = self->iSubscribe.Attach( KTFAccessoryTestProperty, KTFAccessoryNotifierMethod );
    
	CActiveScheduler::Add( self );
	
	
	self->iStatus = KRequestPending;
	self->iSubscribe.Subscribe( self->iStatus );		
	self->SetActive();	
	
	
	CleanupStack::Pop( self );
	
	return self;			    		     	*/
        return NULL;
}
               		
CAccFWUINotifierStubObserver::CAccFWUINotifierStubObserver( CAccFwUiDialogNotifierStub* aService )
	: CActive( EPriorityNormal ), iService( aService )
{	
}
        	
CAccFWUINotifierStubObserver::~CAccFWUINotifierStubObserver()
{
	
}
                
void CAccFWUINotifierStubObserver::DoCancel()
{
	
}


void CAccFWUINotifierStubObserver::RunL()
{
/*	TInt retval(KErrNone);	
    TPckgBuf< TTFAccessoryPublishAndSubscribe > buf;
	retval = iSubscribe.Get( KTFAccessoryTestProperty, KTFAccessoryNotifierMethod, buf );	
	User::LeaveIfError( retval );			
	RArray<TInt> some;
	some.Append( buf().iParam1 );
	TBool booli( EFalse );
	booli = 1 & buf().iParam2;
	
	iService->SetObserverSelection( some, boolibuf().iParam1, buf().iParam2);
	
	if( !IsActive() )
		{
		iStatus = KRequestPending;
		iSubscribe.Subscribe( iStatus );		
		SetActive();		
		}	 */
}

TInt CAccFWUINotifierStubObserver::RunError( TInt /*aError*/ )
{
	return KErrNone;	
}



