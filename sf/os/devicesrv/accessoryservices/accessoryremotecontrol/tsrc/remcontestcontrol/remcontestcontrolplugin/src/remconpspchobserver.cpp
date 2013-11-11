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
#include "remconpspchobserver.h" 
#include "accremconpstypes.h"
#include "siftrace.h"

// ----------------------------------------------------------------------------------
// CRemConPSPCHObserver* CRemConPSPChObserver::NewL()
// ----------------------------------------------------------------------------------	

CRemConPSPCHObserver* CRemConPSPCHObserver::NewL()
{
	
	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::NewL() - Enter" ) ) );
	CRemConPSPCHObserver* result = new (ELeave) CRemConPSPCHObserver();
	CleanupStack::PushL(result);	
	CleanupStack::Pop();
	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::NewL() - Return" ) ) );
	
	return result;	
}

// ----------------------------------------------------------------------------------
// CRemConPSPCHObserver::CRemConPSPCHObserver() 
// ----------------------------------------------------------------------------------	
CRemConPSPCHObserver::CRemConPSPCHObserver() 
: CActive( CActive::EPriorityStandard )
	{						
	
	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::CRemConPSPCHObserver() - Enter" ) ) );
	TInt retval(KErrNone);
	
	_LIT_SECURITY_POLICY_PASS(KReadPolicy);
    _LIT_SECURITY_POLICY_PASS(KWritePolicy);
    
	retval = RProperty::Define(KTFPhoneCmdHandlerProperty, KTFPhoneCmdHandlerKey, RProperty::EByteArray, KReadPolicy, KWritePolicy);    
	
	if(retval != KErrAlreadyExists)
    	User::LeaveIfError(retval);           
	
    retval =  iSubscribe.Attach(KTFPhoneCmdHandlerProperty, KTFPhoneCmdHandlerKey);
    User::LeaveIfError(retval);       
	CActiveScheduler::Add( this );	
	
	iControl = GetRemConControl();
		    		     	
	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::CRemConPSPCHObserver() - Return" ) ) );
	
	}
//
// ----------------------------------------------------------------------------------
// CRemConPSPCHObserver::~CRemConPSPCHObserver()
// ----------------------------------------------------------------------------------	
CRemConPSPCHObserver::~CRemConPSPCHObserver()
	{				
	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::~CRemConPSPCHObserver() - Enter" ) ) );
	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::~CRemConPSPCHObserver() - Return" ) ) );
	}
//
// ----------------------------------------------------------------------------------
// CRemConPSPCHObserver::RunL()
// ----------------------------------------------------------------------------------
void CRemConPSPCHObserver::RunL()
    {                 
    
    COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::RunL() - Enter" ) ) );     
	TInt retval(KErrNone);	
	
	TPhoneCmdHandlerKeyEventBuf keyDataBuf;
	retval = iSubscribe.Get( KTFPhoneCmdHandlerProperty, KTFPhoneCmdHandlerKey, keyDataBuf );	
	User::LeaveIfError(retval);			
    
    	switch ( keyDataBuf().iOperationId  )
		{
		case ERemConExtAnswerCall:
			{
			iEvent = ETFRemConEvent_AnswerCall;
			break;
			}
		
		case ERemConExtEndCall:
			{
			iEvent = ETFRemConEvent_EndCall;
			break;
			}
			
		case ERemConExtAnswerEnd:
			{
			iEvent = ETFRemConEvent_AnswerEnd;
			break;
			}
			
		case ERemConExtVoiceDial:
			{
			iEvent = ETFRemConEvent_VoiceDial;
			break;
			}
			
		case ERemConExtLastNumberRedial:
			{
			iEvent = ETFRemConEvent_LastNumberRedial;
			break;
			}
					
		case ERemConExtDialCall:
			{
			iEvent = ETFRemConEvent_DialCall;
			break;
			}
			
		case ERemConExt3WaysCalling:
			{
			iEvent = ETFRemConEvent_MultiPartyCalling;
			break;
			}
			
		case ERemConExtGenerateDTMF:
			{
			iEvent = ETFRemConEvent_GenerateDTMF;
			break;	
			}
			
		case ERemConExtSpeedDial:
			{
			iEvent = ETFRemConEvent_SpeedDial;
			break;
			}
			
		default:
			iEvent = ETFRemConEvent_None;
			break;
		}
	
	
    iControl->NotifyKeyEvent(iEvent, 0);
     
	if(!IsActive())
		{
		iStatus = KRequestPending;
		iSubscribe.Subscribe(iStatus);		
		SetActive();		
		}		
	
	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::RunL() - Return" ) ) );		
    }        
//
// ----------------------------------------------------------------------------------
// CRemConPSPCHObserver::DoCancel()
// ----------------------------------------------------------------------------------
void CRemConPSPCHObserver::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::DoCancel() - Enter" ) ) );     
    
    if(IsActive())
    	{
    	iSubscribe.Cancel();	
    	}
    
    COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::DoCancel() - Return" ) ) );     
    }    

//
// ----------------------------------------------------------------------------------
// CASYReferenceObserver::DoCancel()
// ----------------------------------------------------------------------------------    
TInt CRemConPSPCHObserver::RunError( TInt /*aError*/ )
{
	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::RunError() - Enter" ) ) );     
	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::RunError() - Return" ) ) );     
	return KErrNone;
}
//
// ----------------------------------------------------------------------------------
// CASYReferenceObserver::Start
// ----------------------------------------------------------------------------------  
void CRemConPSPCHObserver::Start() 			  
{
	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::Start() - Enter" ) ) );     
	if(!IsActive())
		{
		iSubscribe.Subscribe(iStatus);		
		SetActive();		
		}
	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConPSPCHObserver::Start() - Return") ) );     
}


