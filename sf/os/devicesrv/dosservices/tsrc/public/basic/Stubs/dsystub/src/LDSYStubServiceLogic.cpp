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
* Description:  Implementation of CLDSYStubServiceLogic class
*
*/




// INCLUDE FILES 
#include <CDSYController.h>
#include <CDSYMessage.h>
#include <CDSYMessageStructures.h>
#include <CDSYMessageIDs.h>

#include <psvariables.h>

#include "LDSYStubServiceLogic.h"
#include "LDSYTrace.h"


const TInt KLDSYDummyEvent= 666;



CDSYTimer* CDSYTimer::NewLC( CLDSYStubServiceLogic* aCLDSYStubServiceLogic )
	{
	CDSYTimer* self = new (ELeave) CDSYTimer( aCLDSYStubServiceLogic );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

CDSYTimer::~CDSYTimer() // destruct - virtual, so no export
	{
	Cancel();
	iTimer.Close();
	}


CDSYTimer::CDSYTimer( CLDSYStubServiceLogic* aCLDSYStubServiceLogic) : CActive( CActive::EPriorityStandard )
	{
	iCLDSYStubServiceLogic = aCLDSYStubServiceLogic;
	TInt ret( KErrNone );
	ret = iTimer.CreateLocal();
	CActiveScheduler::Add( this );
	}

void CDSYTimer::ConstructL() // second-phase constructor
	{
    }

void CDSYTimer::StartEventTimer( )
	{
	if( !IsActive() )
		{		
		iTimer.After( iStatus, 500 );		
		SetActive();
		CActiveScheduler::Start();
		}
	}

void CDSYTimer::RunL()
	{
	iCLDSYStubServiceLogic->SendEventL();
	CActiveScheduler::Stop();
	}
	
void CDSYTimer::DoCancel()
	{
	
	    
	}
		
// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// CLDSYStubServiceLogic::CLDSYStubServiceLogic
// -----------------------------------------------------------------------------
CLDSYStubServiceLogic::CLDSYStubServiceLogic( CCDSYController& aCDSYController )
    : TLDSYServiceLogic( aCDSYController )
    {
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYStubServiceLogic::CLDSYStubServiceLogic(0x%x)" ), &aCDSYController ) );
    iValue = 0;
    iEventTimer = CDSYTimer::NewLC( this );    
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYStubServiceLogic::CLDSYStubServiceLogic - return void" ) ) );
    }



// -----------------------------------------------------------------------------
// CLDSYStubServiceLogic::~CLDSYStubServiceLogic
// -----------------------------------------------------------------------------
CLDSYStubServiceLogic::~CLDSYStubServiceLogic()
    {
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYStubServiceLogic::~CLDSYStubServiceLogic()" ) ) );    
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgInitializeExtensionCommand, this );
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYStubServiceLogic::~CLDSYStubServiceLogic - return void" ) ) );
    }



// -----------------------------------------------------------------------------
// CLDSYStubServiceLogic::ConstructL
// -----------------------------------------------------------------------------
void CLDSYStubServiceLogic::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYStubServiceLogic::ConstructL()" ) ) );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgInitializeExtensionCommand, this );
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYStubServiceLogic::ConstructL - return void" ) ) );
    }



// -----------------------------------------------------------------------------
// CLDSYStubServiceLogic::NewL
// -----------------------------------------------------------------------------
CLDSYStubServiceLogic* CLDSYStubServiceLogic::NewL( CCDSYController& aCDSYController )
    {
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYStubServiceLogic::NewL(0x%x)" ), &aCDSYController ) );

    CLDSYStubServiceLogic* self = new ( ELeave ) CLDSYStubServiceLogic( aCDSYController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "LDSY - CLDSYStubServiceLogic::NewL - return 0x%x" ), self ) );
    return self;
    }



// -----------------------------------------------------------------------------
// CLDSYStubServiceLogic::ProcessCDSYMessageL
// -----------------------------------------------------------------------------
void CLDSYStubServiceLogic::ProcessCDSYMessageL( TCDSYMessage& aCDSYMessage )
    {
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYStubServiceLogic::ProcessCDSYMessageL(0x%x)" ), &aCDSYMessage ) );
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYNotSupportedServiceLogic::ProcessCDSYMessageL(0x%x)" ), &aCDSYMessage ) );

    TUint32 returnID( 0 );
    TBool isGetter = EFalse;

    switch ( aCDSYMessage.ID() )
        {
        case KCDSYMsgInitializeExtensionCommand:
        {
        TCDSYMsgDataExtensionInitialization returnVal;
    
        returnVal.iDosExtensionBaseDSY = this;
        TPtrC8 ReturnPtr( ( TUint8* )&returnVal, sizeof ( TCDSYMsgDataExtensionInitialization ) );
        TCDSYMessage CDSYMsgDataExtensionInitializationReturn( KCDSYMsgGroup, KCDSYMsgInitializeExtensionReturn, &ReturnPtr, aCDSYMessage.TransactionID() );

        iCDSYController.ProcessMessageL( CDSYMsgDataExtensionInitializationReturn );    
        }
            
        break;

        default:

            TRACE_ASSERT_ALWAYS;

        break;    
        }

    }
    
// -----------------------------------------------------------------------------
// CLDSYExtensionMapper::CallFunctionAndCompleteL
// -----------------------------------------------------------------------------
void CLDSYStubServiceLogic::CallFunctionAndCompleteL( TInt /*aFunc*/, TAny* /*aParameter*/, TInt /*aParLength*/, const RMessage2& aMessage )
    {    
    aMessage.Complete( KErrNone );        
    
    iEventTimer->StartEventTimer();
        
    }    
    

void CLDSYStubServiceLogic::SendEventL()
    {    
    iValue = 5;
	TPckgBuf<TCDSYMsgDataGeneralEvent> package;
    package().iEvent = KLDSYDummyEvent;
    package().iParameter = &iValue;
    package().iParameterSize = sizeof( TInt );
        
    TCDSYMessage dsyEvent( KCDSYMsgGroup, KCDSYEventGeneral, &package, iCDSYController.GetNewTransactionID() );
    iCDSYController.ProcessMessageL( dsyEvent );
    }
    

// -----------------------------------------------------------------------------
// CLDSYExtensionMapper::CallFunctionL
// -----------------------------------------------------------------------------
TInt CLDSYStubServiceLogic::CallFunctionL( TInt aFunc, TAny* aParameter, TInt aParLength, TBool& aParameterModified )
    {
    return KErrNone;
    }    
    
    
    
