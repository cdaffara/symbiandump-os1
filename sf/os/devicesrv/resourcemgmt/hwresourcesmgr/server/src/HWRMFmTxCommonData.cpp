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
#include "HWRMtrace.h"
#include "HWRMFmTxInternalCRKeys.h"
#include "HWRMFmTxInternalPSKeys.h"
#include "HWRMFmTxAudioPolicyDomainPSKeys.h"
#include "HWRMFmTxCommonData.h"
#include "HWRMFmTxStateUtility.h"
#include "HWRMClientServer.h"
#include "HWRMConfiguration.h"
#include "HWRMFmTxClientServer.h"
#include "HWRMFmTxRdsTextConverter.h"
#include "HWRMFmTxCommands.h"

// CONSTANTS
const TUint KRdsSpaceChar = 0x20;
// For converting seconds to microseconds
const TInt KSecondsToMicros(1000000);

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::CHWRMFmTxCommonData
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMFmTxCommonData::CHWRMFmTxCommonData() 
    : iFrequencyRangeUpdated(EFalse)
    {
    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxCommonData::CHWRMFmTxCommonData()" ) ) );
    
    // Initialize RdsData
    iRdsData.iPty = KRdsPtyNone;
    iRdsData.iPtyn.Zero();
    iRdsData.iMs = ETrue;
    iRdsData.iLanguageId = KRdsLanguageUnknown;	
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxCommonData::ConstructL()
    {    
    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxCommonData::ConstructL()" ) ) );

    // Instantiate RDS text converter.  Must be done before InitCenrepL()
    iRdsTextConverter = CHWRMFmTxRdsTextConverter::NewL();

    InitCenrepL();

    // Setup status P&S values
    TInt err = RProperty::Define( KHWRMFmTxStatus, RProperty::EInt, KNoCapability, KHWRMSidOnlyPolicy);
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }

    User::LeaveIfError(iFmTxStatusProperty.Attach(KPSUidHWRMFmTx, KHWRMFmTxStatus));
    User::LeaveIfError(iFmTxStatusProperty.Set(EFmTxStateOff));
    
    // Setup Audio Policy P&S values
    TSecurityPolicy audioPolicy(iFmTxAudioRoutingSid);
    err = RProperty::Define( KHWRMFmTxAvailableFlag, 
                           RProperty::EInt, 
                           audioPolicy, 
                           KHWRMSidOnlyPolicy);
    
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }

    User::LeaveIfError(iFmTxIsAvailableProperty.Attach(KPSUidHWRMFmTx, KHWRMFmTxAvailableFlag));
    User::LeaveIfError(iFmTxIsAvailableProperty.Set(EFalse/*not available*/));      

    // Audio Policy will publish these P&S values
    TInt defineErr;

#ifdef INSECURE_AUDIO_POLICY_KEYS    
    // Allow test applications to publish audio routed flag
    defineErr = RProperty::Define( KHWRMFmTxAudioRoutedFlag, RProperty::EInt, KAlwaysPassPolicy, KAlwaysPassPolicy );
#else       
    defineErr = RProperty::Define( KHWRMFmTxAudioRoutedFlag, 
                                   RProperty::EInt, 
                                   KHWRMSidOnlyPolicy, 
                                   audioPolicy );
#endif // INSECURE_AUDIO_POLICY_KEYS    
    COMPONENT_TRACE2(  _L( "HWRM Server - CHWRMFmTxCommonData::ConstructL() KHWRMFmTxAudioRoutedFlag defineErr %d" ), defineErr );
    if( KErrNone != defineErr && 
        KErrAlreadyExists != defineErr )
        {
        User::Leave(defineErr);
        }
    User::LeaveIfError(iFmTxAudioRoutingProperty.Attach(KPSUidHWRMFmTx, KHWRMFmTxAudioRoutedFlag));
        
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxCommonData::ConstructL - return " ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMFmTxCommonData* CHWRMFmTxCommonData::NewL()
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxCommonData::NewL()" ) );

    CHWRMFmTxCommonData* self = new( ELeave ) CHWRMFmTxCommonData();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxCommonData::NewL - return 0x%x" ), self );
    
    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMFmTxCommonData::~CHWRMFmTxCommonData()
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxCommonData::~CHWRMFmTxCommonData()" ) );

    delete iRdsTextConverter;

    // clean up central repository related data
    CleanCenrep();

    // close properties
    iFmTxStatusProperty.Close();
    iFmTxIsAvailableProperty.Close();

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxCommonData::~CHWRMFmTxCommonData - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::ProcessIndicationL
// Handles status change indications from HWRM plugin
// -----------------------------------------------------------------------------
//
void CHWRMFmTxCommonData::ProcessIndicationL( const TUint32 aId, TDesC8& aData )
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxCommonData::ProcessIndicationL, aId = %d" ), aId );        

    switch ( aId )
        {
        case HWRMFmTxCommand::ETxStatusIndId:
            {                                       
            // Check data size
            if ( aData.Size() != sizeof(HWRMFmTxCommand::TFmTxStatusData) )
                {
                COMPONENT_TRACE3( _L( "HWRM Server - CHWRMFmTxCommonData::ProcessIndicationL data size error 0x%x, 0x%x" ), aData.Size(), sizeof(HWRMFmTxCommand::TFmTxStatusData) );
                return; // Ignore indication when data size is invalid
                }                   
                
            // Unpack the new status
            HWRMFmTxCommand::TStatusIndicationPackage statusPckg;
            statusPckg.Copy(aData);
            HWRMFmTxCommand::TFmTxStatusData status = statusPckg();

            TFmTxStateTransition stateTrans = EFmTxStateTransOff;
            if ( status.state == HWRMFmTxCommand::EFmTxHwStateOn )
                {
                stateTrans = EFmTxStateTransOn;
                    
                // Store the new frequency, when FM Tx is enabled
                UpdateFrequency(status.frequency);                  
                }
            else if ( status.state == HWRMFmTxCommand::EFmTxHwStateScanning )
                {
                stateTrans = EFmTxStateTransScanning;
                }

            // Store the new status
            UpdateStatus(stateTrans);           
            }
            break;
            
        default:
            // do nothing
            break;
        }
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::UpdateStatus
// Updates the common state data, after response/indication from the FM Tx plug-in
// -----------------------------------------------------------------------------
//
void CHWRMFmTxCommonData::UpdateStatus(TFmTxStateTransition aEvent)
    {
    COMPONENT_TRACE4(_L("HWRM Server - CHWRMFmTxCommonData::UpdateStatus(0x%x, 0x%x, 0x%x)"), aEvent, iFmTxPowerSaveFlag, iFmTxAudioRoutedFlag ); 

    TFmTxState newState(EFmTxStateUnknown);

    // Establish the newState
    switch ( aEvent )
        {
        case EFmTxStateTransError:
            {
            newState = EFmTxStateUnknown;
            }
            break;

        case EFmTxStateTransOff:
            {
            if ( IsPowerSaveOn() )
                {
                // The "power save" flag triggered a disable request                    
                newState = (iFmTxPowerSaveFlag == EHWRMFmTxPowerSaveInactivity) ? EFmTxStatePowerSaveInactivity : EFmTxStatePowerSaveAccessory;             
                }
            else
                {
                // FM Tx has been disabled automatically
                newState = EFmTxStateOff;
                }
            }
            break;

        case EFmTxStateTransRequestedOff:
            {
            // User has disabled FM Tx 
            newState = EFmTxStateOff;
            }
            break;

        case EFmTxStateTransOn:
            {
            if ( iFmTxAudioRoutedFlag != EFalse )
                {
                newState = EFmTxStateActive;
                }
            else
                {
                newState = EFmTxStateInactive;
                }
            }
            break;

        case EFmTxStateTransScanning:
            {
            newState = EFmTxStateScanning;
            }
            break;

        default:
            {
            newState = EFmTxStateUnknown;
            }
            break;
        }

    // Update if the state has changed
    if ( newState != iFmTxState)
        {
        // Notify Audio Policy first
        THWRMFmTxAvailableFlag nowAvailable = FmTxAvailableState(newState);
        THWRMFmTxAvailableFlag wasAvailable = FmTxAvailableState(iFmTxState);

        if ( nowAvailable != wasAvailable )
            {
            COMPONENT_TRACE2(_L("HWRM Server - CHWRMFmTxCommonData::UpdateStatus - Audio Policy notification: %d"), nowAvailable );
            TInt err = iFmTxIsAvailableProperty.Set(nowAvailable);      
            if ( err != KErrNone )
                {
                COMPONENT_TRACE2( _L("HWRM Server - CHWRMFmTxCommonData::UpdateStatus - Audio Policy notification failed: %d"), err );
                }
            
            // Update power save status if Fm Tx is available
            if( nowAvailable == EHWRMFmTxAvailableOn )
                {
                UpdatePowerSaveState( CHWRMFmTxCommonData::EHWRMFmTxPowerSaveOff );	
                }
            }

        // Update state
        COMPONENT_TRACE2( _L("HWRM Server - CHWRMFmTxCommonData::UpdateStatus - Setting state to: 0x%x"), newState ); 
        iFmTxState = newState;

        // Publish new state
        TInt err = iFmTxStatusProperty.Set(iFmTxState);
        if ( err != KErrNone )
            {
            COMPONENT_TRACE2( _L("HWRM Server - CHWRMFmTxCommonData::UpdateStatus - Property set failed: %d"), err );                     
            }
        }
   
    COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxCommonData::UpdateStatus - return")); 
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::FmTxAvailableState
// -----------------------------------------------------------------------------
//
THWRMFmTxAvailableFlag CHWRMFmTxCommonData::FmTxAvailableState( TFmTxState aState ) const
    {
    if( aState == EFmTxStatePowerSaveInactivity )
        {
        return EHWRMFmTxAvailableStandby;
        }
    else if( aState == EFmTxStateUnknown || aState == EFmTxStateOff ||  
             aState == EFmTxStatePowerSaveAccessory  ) 
        {
        return EHWRMFmTxAvailableOff;
        }
    else
        {
        return EHWRMFmTxAvailableOn;
        }
    }    

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::UpdateAudioStatus
// Updates the common state data, after Audio Policy flag has been set/cleared
// -----------------------------------------------------------------------------
//
void CHWRMFmTxCommonData::UpdateAudioStatus( TInt aAudioRoutedFlag )
    {           
    if ( aAudioRoutedFlag )
        {
        COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxCommonData::UpdateAudioStatus - Audio Activated") ) );
        
        // State transition from EFmTxStateInactive -> EFmTxStateActive
        if ( iFmTxState == EFmTxStateInactive )
            {
            iFmTxState = EFmTxStateActive;
                
            TInt err = iFmTxStatusProperty.Set(iFmTxState);
            if ( err != KErrNone )
                {
                COMPONENT_TRACE2( _L("HWRM Server - CHWRMFmTxCommonData::UpdateAudioStatus - Property set failed: %d"), err );                    
                }                   
            }
            
        // Cache audio routed flag  
        iFmTxAudioRoutedFlag = ETrue;               
        }
    else
        {
        COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxCommonData::UpdateAudioStatus - Audio Deactivated") );

        // State transition from EFmTxStateActive -> EFmTxStateInactive
        if ( iFmTxState == EFmTxStateActive )
            {
            iFmTxState = EFmTxStateInactive;
                
            TInt err = iFmTxStatusProperty.Set(iFmTxState);
            if ( err != KErrNone )
                {
                COMPONENT_TRACE2( _L("HWRM Server - CHWRMFmTxCommonData::UpdateAudioStatus - Property set failed: %d"), err );                    
                }                   
            }                           

        // Cache audio routed flag  
        iFmTxAudioRoutedFlag = EFalse;              
        }    
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::UpdateFrequency
// Updates the common frequency data, after response from the FM Tx plug-in
// -----------------------------------------------------------------------------
//
void CHWRMFmTxCommonData::UpdateFrequency(TInt aFrequency)
    {
    COMPONENT_TRACE3( _L("HWRM Server - CHWRMFmTxCommonData::UpdateFrequency(%d, %d)"), aFrequency, iFmTxFrequency );         

    if ( aFrequency != iFmTxFrequency )
        {
        iFmTxFrequency = aFrequency;
        iRepository->Set(KFmTxCenResKeyFrequency, aFrequency); // ignore errors
        }
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::UpdateRdsPty
// Update the common PTY data, after response from the FM Tx plug-in
// -----------------------------------------------------------------------------
//
void CHWRMFmTxCommonData::UpdateRdsPty(const TRdsProgrammeType aPty)
    {
    COMPONENT_TRACE3( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsPty %d, %d"), iRdsData.iPty, aPty );   
    
    if ( aPty != iRdsData.iPty)
        {
        COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsPty - Setting PTY") ); 
    
        // Publish new RDS settings
        TInt err = RProperty::Define(KHWRMFmTxRdsPty,  RProperty::EInt, KNoCapability, KHWRMSidOnlyPolicy);
        if ( err == KErrNone || err == KErrAlreadyExists )
            {
            err = RProperty::Set(KPSUidHWRMFmTx, KHWRMFmTxRdsPty, aPty);        
            if ( err != KErrNone )
                {
                COMPONENT_TRACE2( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsPty - Property set failed: %d"), err );                     
                }              
            }
        else
            {
            COMPONENT_TRACE2( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsPty - Property define failed: %d"), err );
            }
            
        // Update cached data 
        iRdsData.iPty = aPty;           
        }   
    }   

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::UpdateRdsPtyn
// Update the common PTYN data, after response from the FM Tx plug-in
// -----------------------------------------------------------------------------
//
void CHWRMFmTxCommonData::UpdateRdsPtyn(const TRdsProgrammeTypeName& aPtyn)
    {
    COMPONENT_TRACE3( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsPtyn %S, %S"), &iRdsData.iPtyn, &aPtyn );
    
    if ( aPtyn.Compare(iRdsData.iPtyn) )
        {
        COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsPtyn - Setting PTYN") ); 

        // Publish new RDS settings 
        TInt err = RProperty::Define(KHWRMFmTxRdsPtyn,  RProperty::EText, KNoCapability, KHWRMSidOnlyPolicy);
        if ( err == KErrNone || err == KErrAlreadyExists )
            {       
            err = RProperty::Set(KPSUidHWRMFmTx, KHWRMFmTxRdsPtyn, aPtyn);      
            if ( err != KErrNone )
                {
                COMPONENT_TRACE2( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsPtyn - Property set failed: %d"), err );                    
                }
            }
        else
            {
            COMPONENT_TRACE2( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsPtyn - Property define failed: %d"), err );
            }
            
        // Store converted 8bit version for convenience
        TRAP(err, iRdsTextConverter->ConvertFromUnicodeL(aPtyn, iRdsConvertedPtyn) );
        if (err != KErrNone)
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsPtyn - error converting PTYN") ); 
            // ignore error
            }

        // Update cached data 
        iRdsData.iPtyn.Copy(aPtyn);
        }
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::UpdateRdsMs
// Update the common MS data, after response from the FM Tx plug-in
// -----------------------------------------------------------------------------
//
void CHWRMFmTxCommonData::UpdateRdsMs(const TBool aMusic)
    {
    COMPONENT_TRACE3( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsMs %d, %d"), iRdsData.iMs, aMusic );

    if ( aMusic != iRdsData.iMs)
        {
        COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsMs - Setting MS") ); 

        // Publish new RDS settings 
        TInt err = RProperty::Define(KHWRMFmTxRdsMs, RProperty::EInt, KNoCapability, KHWRMSidOnlyPolicy);
        if ( err == KErrNone || err == KErrAlreadyExists )
            {       
            TInt value = (aMusic == EFalse) ? 0 : 1;        
            err = RProperty::Set(KPSUidHWRMFmTx, KHWRMFmTxRdsMs, value);        
            if ( err != KErrNone )
                {
                COMPONENT_TRACE2( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsMs - Property set failed: %d"), err );                  
                }
            }
        else 
            {
            COMPONENT_TRACE2( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsMs - Property define failed: %d"), err );
            }
            
        // Update cached data           
        iRdsData.iMs = aMusic;  
        }    
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::UpdateRdsLanguageId
// Update the common language ID data, after response from the FM Tx plug-in
// -----------------------------------------------------------------------------
//
void CHWRMFmTxCommonData::UpdateRdsLanguageId(const TRdsLanguageIdType aLanguageId)
    {
    COMPONENT_TRACE3( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsLanguageId %d, %d"), iRdsData.iLanguageId, aLanguageId );

    if ( aLanguageId != iRdsData.iLanguageId)
        {
        COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsLanguageId - Setting language ID") ); 

        // Publish new RDS settings
        TInt err = RProperty::Define(KHWRMFmTxRdsLangId,  RProperty::EInt, KNoCapability, KHWRMSidOnlyPolicy);
        if ( err == KErrNone || err == KErrAlreadyExists )
            {
            TInt err = RProperty::Set(KPSUidHWRMFmTx, KHWRMFmTxRdsLangId, aLanguageId);     
            if ( err != KErrNone )
                {
                COMPONENT_TRACE2( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsLanguageId - Property set failed: %d"), err );
                }
            }
        else
            {
            COMPONENT_TRACE2( _L("HWRM Server - CHWRMFmTxCommonData::UpdateRdsLanguageId - Property define failed: %d"), err );                   
            }
            
        iRdsData.iLanguageId = aLanguageId;         
        }
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::SetFrequencyRange
// Set the Central Repository data, frequency range and step size
// -----------------------------------------------------------------------------
//
void CHWRMFmTxCommonData::SetFrequencyRange(TInt aMinFrequency, TInt aMaxFrequency, TInt aStepSize)
    {
    COMPONENT_TRACE4( _L("HWRM Server - CHWRMFmTxCommonData::SetFrequencyRange(%d, %d, %d)"), aMinFrequency, aMaxFrequency, aStepSize );

    if ( iRepository->Set(KFmTxCenResKeyMinFrequency, aMinFrequency) == KErrNone &&
         iRepository->Set(KFmTxCenResKeyMaxFrequency, aMaxFrequency) == KErrNone &&
         iRepository->Set(KFmTxCenResKeyStepSize,     aStepSize)     == KErrNone )
        {
        iFrequencyRangeUpdated = ETrue;
        }
    else
        {
        COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxCommonData::SetFrequencyRange CenRep error") ) );
        // Not much we can do about this. Ignore error.
        }

    COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxCommonData::SetFrequencyRange - return")) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::CleanCenrep
// Cleans the central repository related functionality. 
// -----------------------------------------------------------------------------
//
void CHWRMFmTxCommonData::CleanCenrep()
    {
    COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxCommonData::CleanCenrep()") ) ); 
    
    delete iRepository;
    iRepository = NULL;        

    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxCommonData::CleanCenrep - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::InitCenrepL
// Starts the central repository related functionality. 
// -----------------------------------------------------------------------------
//
void CHWRMFmTxCommonData::InitCenrepL()
    {
    COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxCommonData::InitCenrepL()") ) ); 

    iRepository = CRepository::NewL(KCRUidFmTxCenRes);

    // Get the RDS replacement char.  Do this first, so iRdsTextConverter can be used
    TInt replacementChar;
    User::LeaveIfError(iRepository->Get(KFmTxCenResKeyRdsReplacementChar, replacementChar));
    iRdsTextConverter->SetUnsupportedCharReplacement( (TText8)replacementChar );

    // Get previously used FM Tx frequency
    User::LeaveIfError(iRepository->Get(KFmTxCenResKeyFrequency, iFmTxFrequency));

    // Get FM frequency range
    TInt minFreq, maxFreq, stepSize;
    User::LeaveIfError(iRepository->Get(KFmTxCenResKeyMinFrequency, minFreq));
    User::LeaveIfError(iRepository->Get(KFmTxCenResKeyMaxFrequency, maxFreq));
    User::LeaveIfError(iRepository->Get(KFmTxCenResKeyStepSize,     stepSize));
    
    // Flag whether frequency range has been updated    
    if ( minFreq!=0 && maxFreq!=0 && stepSize!=0 )
        {
        iFrequencyRangeUpdated = ETrue;
        }

    // Read PS string from CenRep
    TRdsProgrammeService ps;
    User::LeaveIfError(iRepository->Get(KFmTxCenResKeyRdsPs, ps));
    iRdsTextConverter->ConvertFromUnicodeL(ps, iRdsConvertedPs); // store in 8bit RDS format
    iRdsConvertedPs.AppendFill( KRdsSpaceChar, iRdsConvertedPs.MaxLength() - iRdsConvertedPs.Length() ); // pad with spaces

    // Get audio policy related configuration
    TInt value(0);    
    User::LeaveIfError(iRepository->Get(KFmTxCenResKeyAudioPolicySid, value));
    iFmTxAudioRoutingSid = TSecureId(value);	
    User::LeaveIfError(iRepository->Get(KFmTxCenResKeyPowerSaveTimerLength, value));
    iFmTxPowerSaveTimeout = value*KSecondsToMicros;

    COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxCommonData::InitCenrepL - return") ) ); 
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::UpdatePowerSaveState
// -----------------------------------------------------------------------------
//  
void CHWRMFmTxCommonData::UpdatePowerSaveState( TFmTxPowerSaveFlag aState )
    {
    COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxCommonData::UpdatePowerSaveState()")) );
    
    iFmTxPowerSaveFlag = aState;
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::IsPowerSaveOn
// -----------------------------------------------------------------------------
//  
TBool CHWRMFmTxCommonData::IsPowerSaveOn()
    {
    COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxCommonData::IsPowerSaveOn()")) );

    return (iFmTxPowerSaveFlag != EHWRMFmTxPowerSaveOff);
    }

// ----------------------------------------------------------------------------- 
// CHWRMFmTxCommonData::IsAccessoryPowerSaveOn 
// ----------------------------------------------------------------------------- 
//
TBool CHWRMFmTxCommonData::IsAccessoryPowerSaveOn()
    {
    COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxCommonData::IsAccessoryPowerSaveOn()")) );

    return (iFmTxPowerSaveFlag == EHWRMFmTxPowerSaveAccessory);
    } 

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::IsAudioRouted
// -----------------------------------------------------------------------------
//
TBool CHWRMFmTxCommonData::IsAudioRouted()
    {
    COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxCommonData::IsAudioRouted()")) );
    TBool audioRouted;

    if( iFmTxAudioRoutingProperty.Get( audioRouted ) == KErrNone)
        {
        iFmTxAudioRoutedFlag = audioRouted;
        }
        
    return iFmTxAudioRoutedFlag;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
