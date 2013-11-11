// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ssmclayer.h"
#include "ssmdebug.h"
#include <e32cmn.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmstateawaresession.h>
#include <ssm/ssmdomaindefs.h>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmadaptationcli.h>

#include <ssm/starterclient.h>
#include <ssm/startupdomainpskeys.h>
#include <ssm/ssmuiproviderdll.h>

#include <ssm/startupadaptationcommands.h>
#include <ssm/clayerswp.hrh>
#include <ssm/ssmsystemwideproperty.h>
#include <ssm/ssmstates.hrh>
#include <ssm/ssmpatchableconstants.h>

// The following assert ensures that it's ok to just copy the handle from the 
// RSsmStateManager class as there are no other data members if RSsmStateManage
// is the same size as RSessionBase.
//
// This assert will fail if extra data members are added to RSsmStateManager
// This ensures that the code in RStarterSession is correct.

__ASSERT_COMPILE(sizeof(RSessionBase) == sizeof(RSsmStateManager)); //lint -e752 Suppress local declarator not referenced 
	

EXPORT_C TInt RStarterSession::Connect()
	{
	RSsmStateManager session;
	TInt err = session.Connect();
	if (err == KErrNone)
		{
		// Connection successful, copy handle from session to this handle
		SetHandle(session.Handle());
		}
	// session is not closed before returning as this now owns the handle 
    return err;
    }
	
/*
     * Helper Function that reads KRFStatusPropertyKey SWP using RSsmSystemWideProperty and returns.
     * @return KErrNone on Success
     * Returns an error code from RSsmSystemWideProperty API - Connect & GetValue. 
     * On resulting error return, aRFSwpStatus contains ESsmRfOff
     * 
     * @internalComponent
*/	
static TInt GetRFSwpStatus ( TInt& aRFSwpStatus )
	{
	TUint rfStatusPropertyKey = CSsmUiSpecific::RFStatusPropertyKey ();
	
	RSsmSystemWideProperty swp;
	TInt err = swp.Connect ( rfStatusPropertyKey );
	DEBUGPRINT2A ( "RF SWP connect error = %d ",err );
	TInt rfStatus ( ESsmRfOff );
	if ( err == KErrNone )
		{
		err = swp.GetValue ( rfStatus );
		DEBUGPRINT3A ( "RF SWP status = %d ( err %d )", rfStatus, err );
		}
		
	aRFSwpStatus = rfStatus;
	swp.Close ();
	return err;
	}

/**
 * This adjustment specifies how to convert start-up sub-states
 * to the states defined in StartupAdaptation::TGlobalState
 * 
 * It is calculated using the same method specified in CSaaStateAdaptation::MapToStartupAdaptationState(),
 * see that method for further details.
 * 
 * @internalComponent
 */
const TInt KStartupSubStateAdjustment = static_cast<TInt>(StartupAdaptation::ESWStateStartingUiServices) 
                                    - static_cast<TInt>(ESsmStartupSubStateNonCritical);

/**
 * Helper Function that is used for performing transition to startup substates .
 * @return KErrNone on Success and returns an error if RequestStateTransition fails or transition is not supported 
 * 
 * @internalComponent
*/
static TInt TransitionToStartupSubState(TSsmState &aCurrentState, RSsmStateManager& aSession)
    {    
    TInt err = KErrNone;
    TInt reason = KSsmCLayerNullReason;
    TUint16 mainstate = aCurrentState.MainState();
    TUint16 substate = 0; 

    DEBUGPRINT2A( "RStarterSession::SetState, mainstate = %d", mainstate );                
    if( mainstate == ESsmStartup)
        {
        switch(aCurrentState.SubState())
            {
            case StartupAdaptation::ESWStateCharging - KStartupSubStateAdjustment:
                {
                substate = StartupAdaptation::ESWStateChargingToNormal - KStartupSubStateAdjustment;
                TSsmStateTransition stateinfo(mainstate, substate, reason);
                // Make request for state transition
                err = aSession.RequestStateTransition(stateinfo);
                break;
                }
            case StartupAdaptation::ESWStateAlarm - KStartupSubStateAdjustment:
                {
                substate = StartupAdaptation::ESWStateAlarmToNormal - KStartupSubStateAdjustment;
                TSsmStateTransition stateinfo(mainstate, substate, reason);
                // Make request for state transition
                err = aSession.RequestStateTransition(stateinfo);
                break;
                }
            default:
                {
                err = KErrArgument;
                }
            }
        }
    else
       {
       err = KErrArgument;
       }                    
    return err;    
    }

/**
 * Helper Function that is used for obtaining the currentstate.
 * @return KErrNone on Success and returns an error if RSsmStateAwareSession fails to connect to domainId KSM2UiServicesDomain3
 * @internalComponent
*/
static TInt GetCurrentState( TSsmState &aCurrentState )
    {    
    RSsmStateAwareSession sas;
    const TInt err = sas.Connect(KSM2UiServicesDomain3);
    if (KErrNone == err)
        {
        aCurrentState = sas.State();
        sas.Close();       
        }
    return err;
    }

EXPORT_C TInt RStarterSession::SetState( const TGlobalState aState )
	{
	RSsmStateManager session;
	// Set session to use the handle stored in this
	session.SetHandle(Handle());

	DEBUGPRINT2A( "RStarterSession::SetState( %d )", aState );
    TInt err = KErrNone;
    //Initialising main state with an invalid main state (i.e ESsmMainSystemStateMax)
    TUint16 mainstate = ESsmMainSystemStateMax;
    TUint16 substate = 0;
    TSsmState currentState;
    TUint rfStatusPropertyKey = CSsmUiSpecific::RFStatusPropertyKey();
    switch (aState)
		{
		case ENormal:
		    {
		    //Switch depending on the patchable constant KSsmGracefulOffline is enabled
            if (IsSsmGracefulOffline())
                {
                err = GetCurrentState(currentState);
                if ( err == KErrNone )
                    {
                    TInt globalSystemState;
                    err =RProperty::Get(KPSUidStartup, KPSGlobalSystemState, globalSystemState);
                    if( err == KErrNone )
                        {
                        mainstate = currentState.MainState();
                        //Allow state transition to normal RF ON if global state is not ESwStateNormalRfOn
                        if ((mainstate == ESsmNormal) && (globalSystemState != ESwStateNormalRfOn))
                            {
                            substate = ESsmNormalRfOnSubState;
                            TInt reason = KSsmCLayerNullReason;
                            TSsmStateTransition stateinfo(ESsmNormal,substate, reason);
                            err = session.RequestStateTransition(stateinfo);
                            }
                        else
                            {
                            //Transition to startup substate
                            err = TransitionToStartupSubState(currentState, session);
                            }
                        }
                    }
                }
            else
                {
                TInt rfSwpValue;
                err = GetRFSwpStatus ( rfSwpValue );
                // Change value of RF Swp (to ESsmRfOn) only if it is not ESsmRfOn.
                if (err == KErrNone && rfSwpValue != ESsmRfOn)
                    {
                    TSsmSwp swpRFStatus ( rfStatusPropertyKey, ESsmRfOn );
                    err = session.RequestSwpChange ( swpRFStatus );
                    }
                else
                    {
                    err = GetCurrentState(currentState);
                    if(err == KErrNone)
                        {
                        //Transition to startup substate
                        err = TransitionToStartupSubState(currentState, session);
                        }
                    }
                }
            break;
            }
		case EAlarm:
			{
			// Charging to alarm
			mainstate = ESsmStartup;
			substate = StartupAdaptation::ESWStateChargingToAlarm - KStartupSubStateAdjustment;
			TInt reason = KSsmCLayerNullReason;
			TSsmStateTransition stateinfo(mainstate, substate, reason);
			// Make request and return success.
			err = session.RequestStateTransition(stateinfo);
			break;
			}
            case ECharging:
			{
			// Alarm to charging
			mainstate = ESsmStartup;
			substate = StartupAdaptation::ESWStateAlarmToCharging - KStartupSubStateAdjustment;
			TInt reason = KSsmCLayerNullReason;
			TSsmStateTransition stateinfo(mainstate, substate, reason);
			// Make request and return success.
			err = session.RequestStateTransition(stateinfo);
			break;
			}
		case EOffline:
			{
			if (IsSsmGracefulOffline())
                {
                TInt globalSystemState;
                err =RProperty::Get(KPSUidStartup, KPSGlobalSystemState, globalSystemState); 
                if( err == KErrNone )
                    {
                    //Allow state transition if global state is not ESwStateNormalRfOff
                    if (globalSystemState != ESwStateNormalRfOff)
                        {
                        const TInt reason = KSsmCLayerNullReason;
                        substate = ESsmNormalRfOffSubState;
                        TSsmStateTransition stateinfo(ESsmNormal, substate, reason);
                        // Make request for substate transition to offline
                        err = session.RequestStateTransition(stateinfo);
                        }
                    }                  
                }
            else
                {
                TInt rfSwpValue;
                err = GetRFSwpStatus ( rfSwpValue );
                // Change value of RF SwP (to ESsmRfOff) only if it is not ESsmRfOff.
                if ( err == KErrNone && rfSwpValue != ESsmRfOff )
                    {
                    TSsmSwp swpRFStatus ( rfStatusPropertyKey, ESsmRfOff );
                    err = session.RequestSwpChange ( swpRFStatus );
                    }
                }
            
            break;
			}
		case EBTSap:
			{
			TInt rfSwpValue;
			err = GetRFSwpStatus ( rfSwpValue );
			// Change value of RF Swp (to ESsmBtSap) only if it is notESsmBtSap.
			if ( err == KErrNone &&	rfSwpValue != ESsmBtSap )
				{
				TSsmSwp swpRFStatus ( rfStatusPropertyKey, ESsmBtSap );
				err = session.RequestSwpChange ( swpRFStatus );
				}
			break;
			}
		default:
			{
			err = KErrArgument;
			}
		}

	// Session is not closed before returning as this owns the handle
	return err;
	}

EXPORT_C void RStarterSession::Reset( const TResetReason aReason )
	{
	RSsmStateManager session;
    // Set session to use the handle stored in this
    session.SetHandle(Handle());
    
    // Package up the state transition
    TUint16 mainstate = ESsmShutdown;
    TUint16 substate = KSsmAnySubState;
    TInt reason = aReason;
    TSsmStateTransition stateinfo(mainstate, substate, reason);
    
    // Make request and ignore return value
    session.RequestStateTransition(stateinfo);
    // Session is not closed before returning as this owns the handle
    }

EXPORT_C void RStarterSession::Shutdown()
	{
	RSsmStateManager session;
    // Set session to use the handle stored in this
    session.SetHandle(Handle());
    
    // Package up the state transition
    TUint16 mainstate = ESsmShutdown;
    TUint16 substate = KSsmAnySubState;
    TInt reason = KSsmCLayerPowerOffReason; // A null reason on shutdown will lead to a poweroff
    TSsmStateTransition stateinfo(mainstate, substate, reason);
    
    // Make request and ignore return value
    session.RequestStateTransition(stateinfo);
    // Session is not closed before returning as this owns the handle
    }

EXPORT_C TInt RStarterSession::ResetNetwork()
	{
	TInt requestResult;
	RSsmStateManager session;
	// Set session to use the handle stored in this
	session.SetHandle(Handle());
	
	//Perform state transition instead of SwP transition if KSsmGracefulOffline is enabled
	if (IsSsmGracefulOffline())
		{
		DEBUGPRINT1A( "KSsmGracefulOffline is enabled perform state transition" );
		TSsmStateTransition stateinfo(ESsmNormal, ESsmNormalRfOffSubState, KSsmCLayerNullReason);
		// Make request for substate transition to offline
		requestResult = session.RequestStateTransition(stateinfo);
		DEBUGPRINT2A( "State transition to ESsmNormalRfOffSubState returned : %d", requestResult);
		if (KErrNone == requestResult)
			{
			TSsmStateTransition stateinfo(ESsmNormal, ESsmNormalRfOnSubState, KSsmCLayerNullReason);
			// Make request for substate transition to online
			requestResult = session.RequestStateTransition(stateinfo);
			DEBUGPRINT2A( "State transition to ESsmNormalRfOnSubState returned : %d", requestResult);
			}
		}
	else
		{
		// Make request and return success.
		TUint rfStatusPropertyKey = CSsmUiSpecific::RFStatusPropertyKey();
		TSsmSwp swpRFStatus(rfStatusPropertyKey, ESsmRfOff);

		RSsmSystemWideProperty swp;
		swp.Connect(rfStatusPropertyKey);

		TRequestStatus status;
		//Subscribe for SwP changes
		swp.Subscribe(status);

		requestResult = session.RequestSwpChange(swpRFStatus);
		DEBUGPRINT2A( "RF SwP transition to ESsmRfOff returned : %d", requestResult);
		if (KErrNone == requestResult)
			{
			// Wait for SwP Change
			User::WaitForRequest(status);
			swpRFStatus.Set(rfStatusPropertyKey, ESsmRfOn);
			requestResult = session.RequestSwpChange(swpRFStatus);
			DEBUGPRINT2A( "RF SwP transition to ESsmRfOn returned : %d", requestResult);
			}
		swp.Close();
		}
	// Session is not closed before returning as this owns the handle
	DEBUGPRINT2A( "ResetNetwork() is returning requestResult as: %d", requestResult);
	return requestResult;
	}

EXPORT_C TBool RStarterSession::IsRTCTimeValid()
	{
    TBool isRTCValid = EFalse;

	//Ignoring return code for compatability reasons
    RProperty::Get(CSsmUiSpecific::ValidateRTCPropertyCategory(), 
                    CSsmUiSpecific::ValidateRTCPropertyKey(), isRTCValid);
    return isRTCValid;
    }

EXPORT_C TInt RStarterSession::ActivateRfForEmergencyCall()
	{
    RSsmEmergencyCallRfAdaptation ecradaptation;
    TInt err = ecradaptation.Connect();
    if (err != KErrNone)
    	{
    	return err;
    	}
    TRequestStatus status;
    // Request activation
    ecradaptation.ActivateRfForEmergencyCall(status);
    // Wait for completion
    User::WaitForRequest(status);
    ecradaptation.Close();
    // Return the completion value of status
    return status.Int();
    }

EXPORT_C TInt RStarterSession::DeactivateRfAfterEmergencyCall()
	{
	RSsmEmergencyCallRfAdaptation ecradaptation;
    TInt err = ecradaptation.Connect();
    if (err != KErrNone)
    	{
    	return err;
    	}
    TRequestStatus status;
    // Request activation
    ecradaptation.DeactivateRfForEmergencyCall(status);
    // Wait for completion
    User::WaitForRequest(status);
    ecradaptation.Close();
    // Return the completion value of status
    return status.Int();
    }

EXPORT_C TInt RStarterSession::EndSplashScreen()
	{
	RProperty splashscreen;
	TInt err = splashscreen.Attach(KPSUidStartup, KPSSplashShutdown, EOwnerThread);
	if (err != KErrNone)
		{
		return err;
		}
	
	err = splashscreen.Set(ESplashShutdown);
	
	splashscreen.Close();
	return err;
    }
