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
#include <e32svr.h>
#include <centralrepository.h>
#include <hwrm/hwrmfmtx.h>
#include "HWRMFmTxImpl.h"
#include "HWRMFmTxClientServer.h"
#include "HWRMtrace.h"
#include "HWRMFmTxStatusObserver.h"
#include "HWRMFmTxFrequencyObserver.h"
#include "HWRMFmTxInternalPSKeys.h"
#include "hwrmuipluginhandler.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::CHWRMFmTxImpl
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMFmTxImpl::CHWRMFmTxImpl() :
    iClearFrequencyPckg(NULL, 0, 0),
    iClearFrequencyCountPckg(NULL, 0, 0)
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::CHWRMFmTxImpl()" ) );
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::CHWRMFmTxImpl - return" ));
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::ConstructL(MHWRMFmTxObserver* aCallback)
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::ConstructL()" ) );
    
    // NULL is acceptable value for aCallback, just means that 
    // client is not interested in callbacks.
    iCallback = aCallback;

    // Connect the session and order notifications
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::ConstructL()" ));

    User::LeaveIfError(iClient.Connect(HWRMResourceTypeFmTx));

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::ConstructL - Ordering notifications" ) );
    
    iStatusObserver    = CHWRMFmTxStatusObserver::NewL(iCallback);
    iFrequencyObserver = CHWRMFmTxFrequencyObserver::NewL(iCallback);
    
    TInt err(KErrNone);    
	TRAP(err, iUiPluginHandler = CHwrmUiPluginHandler::NewL( ));
	if ( err != KErrNone )
    	{    	
    	COMPONENT_TRACE2(_L( "HWRM FM Tx Client - CHWRMFmTxImpl::ConstructL() - No Cone Plugin Handler" ), err );
    	iUiPluginHandler = NULL;
    	}
	if(iUiPluginHandler)
		{
			iUiPluginHandler->AddForegroundObserverL(this);    	
		}

	COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::ConstructL - return" ) );    
    }


// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMFmTxImpl::~CHWRMFmTxImpl()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::~CHWRMFmTxImpl()" ) );

    // close HWRM client, also deletes status observers
    Close();

    delete iUiPluginHandler;
    
    // iCallback is not owned so it is not cleaned, just set to NULL 
    iCallback = NULL;
    
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::~CHWRMFmTxImpl - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::Close
// Closes session.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::Close()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::Close()" ));
        
    // Inform session that we are cleaning up now.
    // Ignore errors, just trace them
    TInt err(KErrNone);
    if (iClient.Handle())
        {
        err =iClient.ExecuteOperation(EHWRMFmTxCleanup, TIpcArgs());
        }
    
    if ( err != KErrNone )
        {
        COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::Close - Error in cleanup: %d" ), err );
        }
    
    // Abort status/frequency notifications
    if ( iStatusObserver )
        {
        COMPONENT_TRACE1( _L( "HWRM FM Tx - CHWRMFmTxImpl::Close - canceling FM Tx Status notifications" ) );
        iStatusObserver->Cancel();
        delete iStatusObserver;
        iStatusObserver = NULL;
        }

    delete iFrequencyObserver;
    iFrequencyObserver = NULL;

    iClient.Close();
    
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::Close - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::ReserveL
// Handles client request to reserve FM Tx
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::ReserveL(TBool aForceNoCCoeEnv)
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::ReserveL(0x%x)" ), aForceNoCCoeEnv);
    
    if ( !aForceNoCCoeEnv )
        {        
        if ( iUiPluginHandler )
            {
            // Check that we are on foreground (i.e. we have keyboard focus.)
            // Note: There can be windows on top of our application that have explicitly disabled keyboard focus,
            // but then again the foreground/background observer will not kick in in those cases either.
            // -> We can use focus to determine foreground status here.
            if ( iUiPluginHandler->InForeground() == EFalse )        
                {
                COMPONENT_TRACE1( ( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::ReserveL - Application does not have focus, Reserve denied." ) ) );
                User::Leave(KErrNotReady);
                }
            }
        else
            {
            COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMFmTxImpl::::ReserveL - No iUiPluginHandler available" ) );
            User::Leave(KErrBadHandle);
            }
        }
    
    // Do the actual reservation
    DoReserveL(aForceNoCCoeEnv);
    
    COMPONENT_TRACE1(_L( "HWRM FM Tx Client - CHWRMFmTxImpl::ReserveL - return" ) );          
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::Release
// Handles client request to release FM Tx
// -----------------------------------------------------------------------------
//  
void CHWRMFmTxImpl::Release()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::Release()" ) );

    TInt err = DoRelease();

    if ( err != KErrNone )
        {
        COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::Release - execution failed: %d" ), err );
        }
    
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::Release - return" ) );  
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::Status
// Returns current FM Tx state
// -----------------------------------------------------------------------------
//
TFmTxState CHWRMFmTxImpl::Status() const
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::Status" ) );
    
    TFmTxState state = EFmTxStateUnknown;

    if ( iStatusObserver )
        {
        state = iStatusObserver->Status();
        }

    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::Status - return 0x%x" ), state );    
    
    return state;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::EnableL
// Handles client request to switch FM Tx ON
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::EnableL(TInt aFrequency)
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::EnableL(%d)"), aFrequency );

    // Defaults to last used frequency
    if ( !aFrequency )
        {
        aFrequency = iFrequencyObserver->Frequency();
        COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::EnableL default freq = %d"), aFrequency );
        }

    // Range check on aFrequency
    if ( !FrequencyRangeCheck(aFrequency) )
        {
        COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::EnableL Out of range error") );
        User::Leave(KErrArgument);
        }
        
	// round up to the closest frequency
    aFrequency = RoundFreqToNearestStep(aFrequency);
    
    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxOn, TIpcArgs(aFrequency)));
            
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::EnableL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::DisableL
// Handles client request to switch FM Tx OFF
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::DisableL()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::DisableL" ) );
        
    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxOff, TIpcArgs()));
            
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::DisableL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::Frequency
// Returns current Tx frequency
// -----------------------------------------------------------------------------
//
TInt CHWRMFmTxImpl::Frequency() const
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::Frequency" ) );
    
    TInt frequency = 0;

    if ( iFrequencyObserver )
        {
        frequency = iFrequencyObserver->Frequency();
        }
               
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::Frequency - return %d" ), frequency );
    
    return frequency;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::GetFrequencyRangeL
// Handles client query of FM Tx frequency range, and channel spacing (step size)
// This may already be cached, otherwise it's queried from the server-side
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::GetFrequencyRangeL(TFmTxFrequencyRange& aFrequencyRange) const
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetFrequencyRange" ) );

    if ( iFrequencyObserver )
        {
        if ( iFrequencyObserver->FrequencyRangeUpdated() )
            {
            // Return frequency range from CenRep
            aFrequencyRange.iMinFrequency = iFrequencyObserver->MinFrequency();
            aFrequencyRange.iMaxFrequency = iFrequencyObserver->MaxFrequency();
            aFrequencyRange.iStepSize     = iFrequencyObserver->StepSize();             
            }
        else
            {
            // Request frequency range update from server 
            TPckg<TFmTxFrequencyRange> freqRangePckg(aFrequencyRange);
            User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxGetFreqRange, TIpcArgs(&freqRangePckg)));
            }
        }

    COMPONENT_TRACE4( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetFrequencyRange - return %d-%d, %d" ), aFrequencyRange.iMinFrequency, aFrequencyRange.iMaxFrequency, aFrequencyRange.iStepSize );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::SetFrequencyL
// Handles client request to set the Tx frequency
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::SetFrequencyL(TInt aFrequency)
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetFrequencyL(%d)"), aFrequency );

    // Range check on aFrequency
    if ( !FrequencyRangeCheck(aFrequency) )
        {
        COMPONENT_TRACE1( ( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetFrequencyL Out of range error") ) );
        User::Leave(KErrArgument);
        }       

    // round up to the closest frequency
	aFrequency = RoundFreqToNearestStep(aFrequency);
        
    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxSetFreq, TIpcArgs(aFrequency)));
            
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetFrequencyL - return" ) );     
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::GetNextClearFrequency
// Handles client request, to scan for available clear frequencies
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::GetNextClearFrequencyL(TInt& aClearFrequency)
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetNextClearFrequencyL") );

    // Call server 
    TPckg<TInt> clearFreqPckg(aClearFrequency);    
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxGetNextClearFreq, TIpcArgs(&clearFreqPckg)));
    
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetNextClearFrequency - return %d" ), aClearFrequency );   
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::GetNextClearFrequency
// Handles client request, to scan for available clear frequencies
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::GetNextClearFrequencyL(TClearFrequencyArray& aClearFrequencyArray, TUint& aCount)
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetNextClearFrequencyL, aCount %d"), aCount );

    // Range check on aCount
    if ( aCount == 0 ||
         aCount > KClearFrequencyArrayMax )
        {
        COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetNextClearFrequencyL Out of range error") );
        User::Leave(KErrArgument);
        }

    // Call server     
    TPckg<TClearFrequencyArray> clearFreqArrayPckg(aClearFrequencyArray);    
    TPckg<TUint> countPckg(aCount);
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxGetNextClearFreqArray, TIpcArgs(&clearFreqArrayPckg, &countPckg)));
        
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetNextClearFrequency - return" ) );   
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::GetNextClearFrequency
// Handles client request, to scan for available clear frequencies
// Asynchronous version
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::GetNextClearFrequency(TRequestStatus& aStatus, TInt& aClearFrequency)
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetNextClearFrequency async") );

    // Call server
    iClearFrequencyPckg.Set( TPckg<TInt>(aClearFrequency) );
    iClient.ExecuteAsyncOperation(EHWRMFmTxGetNextClearFreq, TIpcArgs(&iClearFrequencyPckg), aStatus);
 
    COMPONENT_TRACE3( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetNextClearFrequency - return %d, %d" ), aStatus.Int(), aClearFrequency );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::GetNextClearFrequencyL
// Handles client request, to scan for available clear frequencies
// Asynchronous version
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::GetNextClearFrequencyL(TRequestStatus& aStatus, TClearFrequencyArray& aClearFrequencyArray, TUint& aCount)
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetNextClearFrequencyL async, aCount %d"), aCount );

    // Range check on aCount
    if ( aCount == 0 ||
         aCount > KClearFrequencyArrayMax )
        {
        COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetNextClearFrequency Out of range error") );
        User::Leave(KErrArgument);
        }

    // Call server
	iClearFrequencyPckg.Set( TPckg<TClearFrequencyArray>(aClearFrequencyArray) );
	iClearFrequencyCountPckg.Set( TPckg<TUint>(aCount) );
    iClient.ExecuteAsyncOperation(EHWRMFmTxGetNextClearFreqArray, TIpcArgs(&iClearFrequencyPckg, &iClearFrequencyCountPckg), aStatus);
    
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetNextClearFrequency - return" ) );     
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::CancelGetNextClearFrequency
// Cancels previous GetNextClearFrequency request
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::CancelGetNextClearFrequency()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::CancelGetNextClearFrequency" ) );

    TInt error = iClient.ExecuteOperation(EHWRMFmTxCancelGetNextClearFreq, TIpcArgs());
    
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::CancelGetNextClearFrequency - return %d" ), error );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::SetNextClearFrequency
// Handles client request, to set the Tx frequency to a new available channel
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::SetNextClearFrequencyL(TInt& aClearFrequency)
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetNextClearFrequencyL") );

    // Call server 
    TPckg<TInt> clearFreqPckg(aClearFrequency);     
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxSetNextClearFreq, TIpcArgs(&clearFreqPckg)));

    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetNextClearFrequencyL - return %d" ), aClearFrequency );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::SetNextClearFrequency
// Handles client request, to set the Tx frequency to a new available channel
// Asynchronous version
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::SetNextClearFrequency(TRequestStatus& aStatus, TInt& aClearFrequency)
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetNextClearFrequency async") );

    // Call server     
    iClearFrequencyPckg.Set( TPckg<TInt>(aClearFrequency) );
    iClient.ExecuteAsyncOperation(EHWRMFmTxSetNextClearFreq, TIpcArgs(&iClearFrequencyPckg), aStatus);
    
    COMPONENT_TRACE3( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetNextClearFrequency - return %d, %d" ), aStatus.Int(), aClearFrequency );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::CancelSetNextClearFrequency
// Cancels previous SetNextClearFrequency request
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::CancelSetNextClearFrequency()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::CancelSetNextClearFrequency" ) );

    TInt error = iClient.ExecuteOperation(EHWRMFmTxCancelSetNextClearFreq, TIpcArgs());
    
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::CancelSetNextClearFrequency - return &d" ), error );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::IsAutoFrequencyEnabled
// Returns whether the automatic frequency mode is ON
// -----------------------------------------------------------------------------
//
TBool CHWRMFmTxImpl::IsAutoFrequencyEnabled()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::IsAutoFrequencyEnabled" ) );

    // Not supported yet
    TBool autoFreqOn = EFalse;

    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::IsAutoFrequencyEnabled - return %d" ), autoFreqOn );

    return autoFreqOn;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::EnableAutoFrequency
// Handles client request to turn automatic frequency mode ON
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::EnableAutoFrequencyL()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::EnableAutoFrequencyL") );

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxAutoFreqOn, TIpcArgs()));

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::EnableAutoFrequencyL - return" ) );      
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::DisableAutoFrequency
// Handles client request to turn automatic frequency mode OFF
// -----------------------------------------------------------------------------
//
void  CHWRMFmTxImpl::DisableAutoFrequencyL()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::DisableAutoFrequencyL") );

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxAutoFreqOff, TIpcArgs()));

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::DisableAutoFrequencyL - return" ) );         
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::GetRdsData
// Returns current RDS data
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::GetRdsData(TFmTxRdsData& aData)
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetRdsData") );

    // RDS properties are only defined after 1st SetRds request 
    if ( RProperty::Get(KPSUidHWRMFmTx, KHWRMFmTxRdsPty, aData.iPty) != KErrNone )
        {
        COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetRdsData - PTY Get error" ) );
        aData.iPty = KRdsPtyNone; // This is ok, use default value
        }
        
    if ( RProperty::Get(KPSUidHWRMFmTx, KHWRMFmTxRdsPtyn, aData.iPtyn) != KErrNone )
        {
        COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetRdsData - PTYN Get error" ) );                        
        aData.iPtyn.Zero(); // This is ok, use default value
        }       

    if ( RProperty::Get(KPSUidHWRMFmTx, KHWRMFmTxRdsMs, aData.iMs) != KErrNone )
        {
        COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetRdsData - MS Get error" ) );                      
        aData.iMs = ETrue; // default is Music
        }  

    if ( RProperty::Get(KPSUidHWRMFmTx, KHWRMFmTxRdsLangId, aData.iLanguageId) != KErrNone )
        {
        COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetRdsData - Language ID Get error" ) );                     
        aData.iLanguageId = KRdsLanguageUnknown; // This is ok, use default value
        }

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::GetRdsData - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::SetRdsPTY
// Handles client request to set the Programme TYpe
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::SetRdsPtyL(TRdsProgrammeType aPty)
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRdsPtyL(%d)"), aPty );

    if( aPty < KRdsPtyNone || aPty > KRdsPtyAlarm )
        {
        User::Leave(KErrArgument);
        }

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxSetRdsPty, TIpcArgs(aPty)));
            
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRdsPtyL - return" ) );        
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::SetRdsPTYN
// Handles client request to set the Programme TYpe Name
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::SetRdsPtynL(TRdsProgrammeTypeName& aPtyn)
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRdsPtynL(%S)"), &aPtyn );

    if( aPtyn.Length() > KMaxRdsPtynLength )
        {
        User::Leave(KErrArgument);
        }

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxSetRdsPtyn, TIpcArgs(&aPtyn)));
            
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRdsPtynL - return" ) );           
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::SetRdsMS
// Handles client request to set the Music/Speech flag
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::SetRdsMsL(TBool aMusic)
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRdsMsL(%d)"), aMusic );

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxSetRdsMs, TIpcArgs(aMusic)));
            
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRdsMsL - return" ) );         
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::SetRdsLanguageId
// Handles client request to set the language ID
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::SetRdsLanguageIdL(TRdsLanguageIdType aLanguageId)
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRdsLanguageIdL(0x%x)"), aLanguageId );

    if( aLanguageId < KRdsLanguageUnknown || aLanguageId > KRdsLanguageAmharic )
        {
        User::Leave(KErrArgument);
        }

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxSetRdsLanguageId, TIpcArgs(aLanguageId)));
            
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRdsLanguageIdL - return" ) );         
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::SetRadiotext
// Handles client request to set the RT data
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::SetRadiotextL(TDes& aText)
    {
    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRadiotextL(%S)"), &aText );

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxSetRadiotext, TIpcArgs(&aText)));
            
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRadiotextL - return" ) );                 
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::SetRadiotextPlusL
// Handles client request to set the RT+ data
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::SetRadiotextPlusL(TDes& aText, TRtPlusContentType aContentType)
    {
    COMPONENT_TRACE3( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRadiotextPlusL(%S, ct %d)"), &aText, aContentType );

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxSetRadiotextPlus1, TIpcArgs(&aText, aContentType)));
            
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRadiotextPlusL - return" ) );         
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::SetRadiotextPlusL
// Handles client request to set the RT+ data
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::SetRadiotextPlusL(TDes& aText, TRtPlusTag aTag)
    {
    COMPONENT_TRACE3( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRadiotextPlusL(%S, tag %d)"), &aText, aTag.iContentType );

    THWRMRtPlusTagCmdPackage pckg(aTag);

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxSetRadiotextPlus2, TIpcArgs(&aText, &pckg)));
            
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRadiotextPlusL - return" ) ); 
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::SetRadiotextPlusL
// Handles client request to set the RT+ data
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::SetRadiotextPlusL(TDes& aText, TRtPlusTag aTag1, TRtPlusTag aTag2)
    {
    COMPONENT_TRACE4( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRadiotextPlusL(%S, tag %d, tag2 %d)"), &aText, aTag1.iContentType, aTag2.iContentType );

    THWRMRtPlusTagCmdPackage pckg(aTag1);
    THWRMRtPlusTagCmdPackage pckg2(aTag2);

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxSetRadiotextPlus3, TIpcArgs(&aText, &pckg, &pckg2)));
            
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::SetRadiotextPlusL - return" ) );         
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::ClearRdsData
// Clears any previously set RDS data
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::ClearRdsData()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::ClearRdsData") );

    // Call server 
    TInt err = iClient.ExecuteOperation(EHWRMFmTxClearRdsData, TIpcArgs());
    
    if ( err != KErrNone )
        {
        COMPONENT_TRACE2(  _L( "HWRM FM Tx Client - CHWRMFmTxImpl::ClearRdsData - execution failed: %d" ), err );
        }

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::ClearRdsData - return" ) );                  
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::ClearRadiotextData
// Clears any previously set RT data
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::ClearRadiotextData()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::ClearRadiotextData") );

    // Call server 
    TInt err = iClient.ExecuteOperation(EHWRMFmTxClearRtData, TIpcArgs());
    
    if ( err != KErrNone )
        {
        COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::ClearRadiotextData - execution failed: %d" ), err );
        }

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::ClearRadiotextData - return" ) );                    
    }

//------------------------------------------------------------------------------
// CHWRMFmTxImpl::HandleGainingForeground     
// Inherited From MCoeForegroundObserver - called when app comes to foreground
//------------------------------------------------------------------------------
//
void CHWRMFmTxImpl::HandleGainingForeground()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::HandleGainingForeground()" ) );

    TRAPD(err, DoReserveL(EFalse));
        
    if ( err != KErrNone )
        {
        COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::HandleGainingForeground - Error reserving: %d " ), err );
        }

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::HandleGainingForeground - return " ) );
    }

//------------------------------------------------------------------------------
// CHWRMFmTxImpl::HandleLosingForeground     
// Inherited From MCoeForegroundObserver - called when app goes to background 
//------------------------------------------------------------------------------
void CHWRMFmTxImpl::HandleLosingForeground()
    {
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::HandleLosingForeground()" ) );

    TInt err = DoRelease();

    if ( err != KErrNone )
        {
        COMPONENT_TRACE2( _L( "HWRM FmTxClient - CHWRMFmTxImpl::HandleLosingForeground - Error releasing: %d " ), err );
        }

    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::HandleLosingForeground - return " ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::FrequencyRangeCheck
// Helper function for enabling or setting Tx frequency
// -----------------------------------------------------------------------------
//
TBool CHWRMFmTxImpl::FrequencyRangeCheck(TInt aFrequency)   
    {
    
    COMPONENT_TRACE1( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::FrequencyRangeCheck()" ) );
    
    TBool retVal = ETrue;
    
    if ( iFrequencyObserver->FrequencyRangeUpdated() )
        {
        // Check against frequency range in CenRep
        if ( aFrequency < iFrequencyObserver->MinFrequency() || 
             aFrequency > iFrequencyObserver->MaxFrequency() )
            {
            retVal = EFalse;
            }
        }
    else
        {
        // Query frequency range from server    
        TPckgBuf<TFmTxFrequencyRange> freqRangePckg;
        TInt err = iClient.ExecuteOperation(EHWRMFmTxGetFreqRange, TIpcArgs(&freqRangePckg));
        TFmTxFrequencyRange freqRange = freqRangePckg();    
        COMPONENT_TRACE4( _L("HWRM FM Tx Client - CHWRMFmTxImpl::FrequencyRangeCheck() got from server %d-%d, %d"), freqRange.iMinFrequency, freqRange.iMaxFrequency, freqRange.iStepSize );

        // Check against frequency range from server        
        if ( err != KErrNone ||
             aFrequency < freqRange.iMinFrequency || 
             aFrequency > freqRange.iMaxFrequency )
            {
            retVal = EFalse;        
            }
        }

    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTxImpl::FrequencyRangeCheck() return %d" ), retVal );
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::DoReserveL
// Helper function for reserving FM Tx resource
// -----------------------------------------------------------------------------
//
void CHWRMFmTxImpl::DoReserveL(TBool aForceNoCCoeEnv)
    {
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMFmTxReserve, TIpcArgs(aForceNoCCoeEnv)));
    }   

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::DoRelease
// Helper function for releasing FM Tx resource
// -----------------------------------------------------------------------------
//
TInt CHWRMFmTxImpl::DoRelease()
    {
    return iClient.ExecuteOperation(EHWRMFmTxRelease, TIpcArgs());
    }   

// -----------------------------------------------------------------------------
// CHWRMFmTxImpl::RoundFreqToNearestStep
// Round up the frequency to the nearest valid frequency based on the 
// frequency range and the step size
// -----------------------------------------------------------------------------
//
TInt CHWRMFmTxImpl::RoundFreqToNearestStep(TInt aFrequency)
    {
    TInt stepSize = iFrequencyObserver->StepSize();
    TInt frequency;
    
    frequency = aFrequency - iFrequencyObserver->MinFrequency();
    
	TInt remainder(0);
    if ( stepSize )
	  {
	  remainder = frequency % stepSize;
	  }
    
    if ( remainder == 0)
    	{
    	frequency = aFrequency;
    	}
    else
    	{
    	if (remainder < stepSize / 2)
    		{
    		frequency = aFrequency - remainder;
    		}
    	else
    		{
    		frequency = aFrequency - remainder + stepSize;
    		}
    	}
    return frequency;
    }   


//  End of File  
