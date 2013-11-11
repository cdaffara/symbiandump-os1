// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



// INCLUDE FILES
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>
#include <hwrm/hwrmfmtx.h>
#include "HWRMtrace.h"
#include "HWRMFmTxInternalCRKeys.h"
#include "HWRMFmTxFrequencyObserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMFmTxFrequencyObserver::CHWRMFmTxFrequencyObserver
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMFmTxFrequencyObserver::CHWRMFmTxFrequencyObserver()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::CHWRMFmTxFrequencyObserver()" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxFrequencyObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxFrequencyObserver::ConstructL(MHWRMFmTxObserver* aCallback)
    {    
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::ConstructL(0x%x)" ), aCallback );

    // To get access to cenrep
    iRepository = CRepository::NewL(KCRUidFmTxCenRes);

    // Register for central repository notifications
    iFreqNotify     = CCenRepNotifyHandler::NewL(*this, *iRepository, 
            CCenRepNotifyHandler::EIntKey, KFmTxCenResKeyFrequency);
    iMinFreqNotify  = CCenRepNotifyHandler::NewL(*this, *iRepository, 
            CCenRepNotifyHandler::EIntKey, KFmTxCenResKeyMinFrequency);
    iMaxFreqNotify  = CCenRepNotifyHandler::NewL(*this, *iRepository, 
            CCenRepNotifyHandler::EIntKey, KFmTxCenResKeyMaxFrequency);
    iStepSizeNotify = CCenRepNotifyHandler::NewL(*this, *iRepository, 
            CCenRepNotifyHandler::EIntKey, KFmTxCenResKeyStepSize);

    iFreqNotify->StartListeningL();
    iMinFreqNotify->StartListeningL();    
    iMaxFreqNotify->StartListeningL();
    iStepSizeNotify->StartListeningL();

    iCallback = aCallback;

    // Get previous/default FM Tx frequency
    TInt err = iRepository->Get(KFmTxCenResKeyFrequency, iFrequency);
    if ( err != KErrNone )
        {
        COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::ConstructL - Unable to read KFmTxCenResKeyFrequency from Cenrep" ) );
        User::Leave(err);
        }

    // Get FM Tx frequency range
    err = iRepository->Get(KFmTxCenResKeyMinFrequency, iMinFrequency);
    if ( err != KErrNone )
        {
        COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::ConstructL - Unable to read KFmTxCenResKeyMinFrequency from Cenrep" ) );
        User::Leave(err);
        }
    err = iRepository->Get(KFmTxCenResKeyMaxFrequency, iMaxFrequency);
    if ( err != KErrNone )
        {
        COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::ConstructL - Unable to read KFmTxCenResKeyMaxFrequency from Cenrep" ) );
        User::Leave(err);
        }      
    err = iRepository->Get(KFmTxCenResKeyStepSize, iStepSize);
    if ( err != KErrNone )
        {
        COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::ConstructL - Unable to read KFmTxCenResKeyStepSize from Cenrep" ) );
        User::Leave(err);
        }                 

    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::ConstructL - return, iFrequency = %d " ), iFrequency );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxFrequencyObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMFmTxFrequencyObserver* CHWRMFmTxFrequencyObserver::NewL(MHWRMFmTxObserver*  aCallback)
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::NewL(0x%x)" ), aCallback );

    CHWRMFmTxFrequencyObserver* self = new( ELeave ) CHWRMFmTxFrequencyObserver();
    
    CleanupStack::PushL( self );
    self->ConstructL(aCallback);
    CleanupStack::Pop( self );

    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::NewL - return 0x%x" ), self );
    
    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMFmTxFrequencyObserver::~CHWRMFmTxFrequencyObserver()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::~CHWRMFmTxFrequencyObserver()" ) );

    delete iFreqNotify;
    delete iMinFreqNotify;
    delete iMaxFreqNotify;
    delete iStepSizeNotify;
    delete iRepository;    

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::~CHWRMFmTxFrequencyObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxFrequencyObserver::HandleNotifyInt
// Callback to handle Central Repository notifications
// -----------------------------------------------------------------------------
//
void CHWRMFmTxFrequencyObserver::HandleNotifyInt(TUint32 aId, TInt aNewValue)
    {
    COMPONENT_TRACE3(_L("HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::HandleNotifyInt(0x%x, 0x%x)"), aId, aNewValue); 

    switch (aId)
        {    
        case KFmTxCenResKeyFrequency:
            {
            COMPONENT_TRACE1(_L("HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::HandleNotifyInt KFmTxCenResKeyFrequency") );
            iFrequency = aNewValue;

            if (iCallback)
                {
                // Inform user about change of the FM Tx frequency
                iCallback->FmTxFrequencyChanged(aNewValue);
                }
            }
            break;
            
        case KFmTxCenResKeyMinFrequency:
            {
            COMPONENT_TRACE1(_L("HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::HandleNotifyInt KFmTxCenResKeyMinFrequency") );
            iMinFrequency = aNewValue;
            }
            break;
            
        case KFmTxCenResKeyMaxFrequency:
            {
            COMPONENT_TRACE1(_L("HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::HandleNotifyInt KFmTxCenResKeyMaxFrequency") );
            iMaxFrequency = aNewValue;
            }
            break;
            
        case KFmTxCenResKeyStepSize:
            {
            COMPONENT_TRACE1(_L("HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::HandleNotifyInt KFmTxCenResKeyStepSize") );
            iStepSize = aNewValue;
            }
            break;   
            
        default:
            // do nothing
            break;                  
        }    

    COMPONENT_TRACE1(_L("HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::HandleNotifyInt - return") ); 
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxFrequencyObserver::HandleNotifyError
// Callback to handle Central Repository error notifications
// -----------------------------------------------------------------------------
//
#if  defined(COMPONENT_TRACE_FLAG) && defined(_DEBUG)
void CHWRMFmTxFrequencyObserver::HandleNotifyError(TUint32 aId, TInt error, CCenRepNotifyHandler* aHandler)

#else
void CHWRMFmTxFrequencyObserver::HandleNotifyError(TUint32 /*aId*/, TInt /*error*/, CCenRepNotifyHandler* aHandler)
#endif
    {
    COMPONENT_TRACE4(_L("HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::HandleNotifyError(0x%x, 0x%x, 0x%x)"), aId, error, aHandler); 
    
    // retry notification 
    TRAPD(err,aHandler->StartListeningL());
    if ( err != KErrNone )
        {
        COMPONENT_TRACE2(_L("HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::HandleNotifyError - error: %d "), err ); 
        }
        
    // Do nothing
        
    COMPONENT_TRACE1(_L("HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::HandleNotifyError - return") ); 
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxFrequencyObserver::Frequency
// Return FM Tx frequency
// -----------------------------------------------------------------------------
//
TInt CHWRMFmTxFrequencyObserver::Frequency() const
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::Frequency - return %d" ), iFrequency );
    
    return iFrequency;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxFrequencyObserver::MinFrequency
// Return the minimum FM Tx frequency
// -----------------------------------------------------------------------------
//
TInt CHWRMFmTxFrequencyObserver::MinFrequency() const
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::MinFrequency - return %d" ), iMinFrequency );
    
    return iMinFrequency;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxFrequencyObserver::MaxFrequency
// Return the maximum FM Tx frequency
// -----------------------------------------------------------------------------
//
TInt CHWRMFmTxFrequencyObserver::MaxFrequency() const
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::MaxFrequency - return %d" ), iMaxFrequency );
    
    return iMaxFrequency;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxFrequencyObserver::StepSize
// Return the FM Tx frequency step size
// -----------------------------------------------------------------------------
//
TInt CHWRMFmTxFrequencyObserver::StepSize() const
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::StepSize - return %d" ), iStepSize );    

    return iStepSize;
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxFrequencyObserver::FrequencyRangeUpdated
// Return the FM Tx frequency step size
// -----------------------------------------------------------------------------
//
TBool CHWRMFmTxFrequencyObserver::FrequencyRangeUpdated() const
    {
    TBool retVal = EFalse;
        
    if ( iMinFrequency != 0 &&
         iMaxFrequency != 0 &&
         iStepSize     != 0 )
        {
        retVal = ETrue;
        }
        
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxFrequencyObserver::FrequencyRangeUpdated - return %d" ), retVal );
    return retVal;
    }

//  End of File  
