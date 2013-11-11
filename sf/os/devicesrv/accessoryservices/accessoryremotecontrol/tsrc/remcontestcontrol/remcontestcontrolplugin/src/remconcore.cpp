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


// INCLUDE FILES
#include "remconcontrol.h"
#include "remconactive.h"
#include "siftrace.h"
#include "remconcore.h"
#include <remconaddress.h>

// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
CRemConCore::CRemConCore()
    {
     iEvent     = ETFRemConEvent_None;
     iParameter = KErrNone;	
    }

// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
void CRemConCore::ConstructL( CRemConInterfaceSelector* aTargetInterfaceSelector )
    {
    iCore = CRemConCoreApiTarget::NewL( *aTargetInterfaceSelector, *this );
    
    iCoreController = CRemConCoreApiController::NewL( *aTargetInterfaceSelector, *this );

	iTargetInterfaceSelector =	aTargetInterfaceSelector;
	
    iActive = CRemConActive::NewL( *this );
	}

// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
CRemConCore* CRemConCore::NewL( CRemConInterfaceSelector* aTargetInterfaceSelector )
    {
    CRemConCore* self = new( ELeave ) CRemConCore();
    CleanupStack::PushL( self );
    self->ConstructL( aTargetInterfaceSelector );
    CleanupStack::Pop();
    return self;
    }
 
// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
CRemConCore::~CRemConCore()
    {
    delete iActive;
    }


// -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------
void CRemConCore::CompleteResponse( const TInt aError )
    {
		if( aError != KErrNone )
	    {
    	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL -  CRemConCore::CompleteResponse( %d)" ), aError) );
    	}
    	else
    	{
    	
    	MRemConControl* aControl = GetRemConControl();
    	aControl->CompleteKeyEventResponse( iEvent, iParameter, aError );	
    	}
	 
	 iEvent     = ETFRemConEvent_None;
     iParameter = KErrNone;	
	}
// -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------	
void CRemConCore::MrccatoCommand(TRemConCoreApiOperationId aOperationId, 
			TRemConCoreApiButtonAction aButtonAct)
{
    
	iParameter = (TInt)aButtonAct;
	
	switch ( aOperationId )
        {       	
        case ERemConCoreApiVolumeUp:
            {
            iEvent = ETFRemConEvent_VolumeUp;
            break;
            }
        case ERemConCoreApiVolumeDown:
        	{
			iEvent = ETFRemConEvent_VolumeDown;
        	break;
        	}        
        case ERemConCoreApiStop:
        	{
			iEvent = ETFRemConEvent_CodeStop;
        	break;
        	}
        case ERemConCoreApiFastForward:
        	{
			iEvent = ETFRemConEvent_CodeForward;
        	break;
        	}
        case ERemConCoreApiRewind:
        	{
			iEvent = ETFRemConEvent_CodeBack;
        	break;
        	}
		case ERemConCoreApiForward:
        	{
			iEvent = ETFRemConEvent_CodeForward;
        	break;
        	}
		
		case ERemConCoreApiBackward:
        	{
			iEvent = ETFRemConEvent_CodeBack;
        	break;
        	}
        	
        case ERemConCoreApiPausePlayFunction:
        	{
			iEvent = ETFRemConEvent_CoreApiPlaybackSpeedX1; //event not used in real implementation
        	break;
        	}
        	
        case ERemConCoreApiMute:
        	{
        	iEvent = ETFRemConEvent_Mute;
        	break;
        	} 

     	default:
     	    iEvent = ETFRemConEvent_None;
            break;      
        }
    MRemConControl* aControl = GetRemConControl();
	aControl->NotifyKeyEvent(iEvent, aButtonAct);
	
	iEvent     = ETFRemConEvent_None;
    iParameter = KErrNone;	

}

// -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------		
void CRemConCore::MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed, 
			TRemConCoreApiButtonAction aButtonAct)
{
	
	//TTFRemConkeyEvent aKeyCode;
	iParameter = (TInt)aButtonAct;
	
	switch ( aSpeed )
        {       	
        case ERemConCoreApiPlaybackSpeedX1:
            {
            iEvent = ETFRemConEvent_CoreApiPlaybackSpeedX1;
            break;
            }

     	default:
     	    iEvent = ETFRemConEvent_None;
            break;      
        }
	MRemConControl* aControl = GetRemConControl();
	aControl->NotifyKeyEvent(iEvent, aButtonAct);
	
	iEvent     = ETFRemConEvent_None;
    iParameter = KErrNone;	
	
}

// -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------
void CRemConCore::MrccatoTuneFunction(TBool /*aTwoPart*/, 
			TUint /*aMajorChannel*/, 
			TUint /*aMinorChannel*/,
			TRemConCoreApiButtonAction /*aButtonAct*/)
{
}

// -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------
void CRemConCore::MrccatoSelectDiskFunction(TUint /*aDisk*/,
			TRemConCoreApiButtonAction /*aButtonAct*/)
{
}

// -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------		
void CRemConCore::MrccatoSelectAvInputFunction(TUint8 /*aAvInputSignalNumber*/,
			TRemConCoreApiButtonAction /*aButtonAct*/)
{
}

// -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------
void CRemConCore::MrccatoSelectAudioInputFunction(TUint8 /*aAudioInputSignalNumber*/,
			TRemConCoreApiButtonAction /*aButtonAct*/)
{
}

// -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------
void CRemConCore::Response(TRemConCoreApiOperationId /*aOperationId*/, TInt /*aError*/)
{
	
}

// -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------
void CRemConCore::Play()
{
		TUint numRemotes;		
		
		iActive->iStatus = KRequestPending;
 		iCoreController->Play(iActive->iStatus, numRemotes, ERemConCoreApiButtonPress);
 		iActive->SetActive();
}
 
 // -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------

void CRemConCore::Stop()
{		
		TUint numRemotes;
		TRemConAddress* aConnection;
		TSglQue<TRemConAddress> aConnections;		
		const TInt KBearerUid = 0x10205083;	
					
		iTargetInterfaceSelector->GetConnections(aConnections);
		
		aConnection = aConnections.First();					

		if(aConnection->IsNull() == EFalse)
		{			
			//
			// Should check also last element...
			//
			while (aConnection->BearerUid() == TUid::Uid(KBearerUid))
	        {
	        aConnection = (TRemConAddress*)aConnection->iLink.iNext;        
	        };
	        
	 		if(aConnection->IsNull() == EFalse)
	 			{
	 			iTargetInterfaceSelector->GoConnectionOrientedL(*aConnection);	
	 			}
            
            iActive->iStatus = KRequestPending;					
			iCoreController->Stop(iActive->iStatus, numRemotes, ERemConCoreApiButtonPress);		
			iActive->SetActive();	
		}
}

// -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------
void CRemConCore::Cancel()
{
		TUint numRemotes;		
 		iCoreController->Play(iActive->iStatus, numRemotes, ERemConCoreApiButtonPress); 		
 		iCoreController->Cancel();
 		iActive->SetActive();
}
		  
