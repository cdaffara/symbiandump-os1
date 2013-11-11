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
* Description:  HDMI Cable Status FSM's "Connected" state.
*
*/


#ifndef C_COMPOSITECABLESTATECONNECTED_H
#define C_COMPOSITECABLESTATECONNECTED_H


#include "multifinitestatemachine.h"
#include "centralrepositorywatch.h" 
#include "tvoutconfigforcomposite.h"
#include "compositecablestatusfsm.h"




/**
 * "Connected" state implementation for Composite Cable Status FSM.
 * Composite Cable is connected in this state.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( CCompositeCableStateConnected ): public CBase, 
    public MFSMState, public MCentralRepositoryObserver
    {


public:

    
    /**
     * Two-phased constructor.
     * @param aFSMForState Finite State Machine interface for State.
     * @param aTVOutConfigForHDMI TV Out Configurer.
     */
    static CCompositeCableStateConnected* NewL(
            MFSMForState& aFSMForState, 
            CTVOutConfigForComposite& aTVOutConfigForComposite,
            CCompositeCableStatusFSM& aCompositeCableStatusFSM);
    

    /**
    * Destructor.
    */
    virtual ~CCompositeCableStateConnected();

    
// from base class MFSMState


    /**
     * From MFSMState.
     * Function is called when entering the state. State can perform
     * necessary entry actions if any.
     *
     * @since TB 9.2
     * @param none. 
     * @return aFSMBody Interface pointer of a FSM body.
     */
    virtual void Enter();

    /**
     * From MFSMState.
     * Function is called by FSM Body whenever an event that might have 
     * impact on the state has been fired. Implementation has to found
     * out what event has occurred and decide whether it enforces
     * transition to another state.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    virtual void Input(
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );

    /**
     * From MFSMState.
     * Function returns state's identification.
     *
     * @since TB 9.2
     * @param none. 
     * @return TFSMState State's identification.
     */
    virtual TFSMState Id();

    /**
     * From MFSMState.
     * Function returns state's name.
     *
     * @since TB 9.2
     * @param none. 
     * @return TPtrC Name of the state.
     */
    virtual TPtrC Name();

    /**
     * Function returns substate's identification.
     *
     * @since TB 9.2
     * @param none. 
     * @return TFSMState Substate's identification.
     */
    virtual TFSMState SubId();

    
protected:

// from base class MCentralRepositoryObserver

    
    /**
     * Function is called by watcher every time when key value has changed
     * in CR.
     *
     * @since TB 9.2
     * @param aRepositoryId Changed repository. 
     * @param aRepositoryId Changed key. 
     * @return none
     */
    virtual void CentRepKeyChanged(
            TUid    aRepositoryId,
            TUint32 aId );

    /**
     * Function is called by property watcher when reading the integer property's
     * current value fails.
     *
     * @since TB 9.2
     * @param aRepositoryId Changed repository. 
     * @param aRepositoryId Changed key. 
     * @param aError Error code:
     *      KErrAbort if in a transaction that has previously failed, 
     *      KErrPermissionDenied if caller fails capability check, 
     *      KErrNotFound if the setting does not exist, 
     *      KErrArgument if the setting exists but is not an integer, plus other system-wide error codes. 
     * . 
     * @return none
     */
    virtual void CentRepGetKeyValueFailed( 
            TUid    aRepositoryId,
            TUint32 aId,
            TInt aError );


private:

    /**
     * Input function for substate Enabling.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateEnablingInput( 
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );

    /**
     * Input function for substate Enabled.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateEnabledInput( 
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );

    /**
     * Input function for substate Disabling.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateDisablingInput( 
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );

    /**
     * Input function for substate Setting.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateSettingInput( 
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );
    
    /**
     * Input function for substate Start Listen 
     * Config Changes.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateStartListenConfigChangesInput( 
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );
    
    /**
     * Input function for substate Disabling for resetting
     * TV Out Configuration settings. 
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateResettingDisableInput(
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );    

    /**
     * Input function for substate Resetting.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateResettingInput( 
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );

    /**
     * C++ default constructor.
     * @param aFSMForState Finite State Machine interface for State.
     * @param aTVOutConfigForHDMI TV Out Configurer.
     */
    CCompositeCableStateConnected(
            MFSMForState& aFSMForState, 
            CTVOutConfigForComposite& aTVOutConfigForComposite,
            CCompositeCableStatusFSM& aCompositeCableStatusFSM);

    /**
     * Resetting the available config in tvout config
     * 
     * @since TB 9.2
     * @return none.
     */
    void ClearAvailableTvOutConfig();
    
    /**
     * Transit to Idle state
     * 
     * @since TB 9.2
     * @return none.
     */
    void TransitStateIdle();
    
    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();

private: // data

    enum TSubState
      {
      ESubStateEnabling,
      ESubStateEnabled,
      ESubStateDisabling,
      ESubStateSetting,
      ESubStateStartListenSettingsChanges,
      ESubStateResettingDisable,
      ESubStateResetting
      };

    /**
     * FSM.
     * Not own.
     */
    MFSMForState& iFSMForState;

    /**
     * Central Repository Watchers.
     * Own
     */
    CCentralRepositoryWatch* iCRWatchForAspectRatio;
    CCentralRepositoryWatch* iCRWatchForFlickerFilter;
    CCentralRepositoryWatch* iCRWatchForTvSystem;
	CCentralRepositoryWatch* iCRWatchForVOverScan;

    /**
     * TV Out Configurer.
     * Not own
     */
    CTVOutConfigForComposite&   iTVOutConfigForComposite;
    
    /**
     * Composite Cable Status FSM.
     * Not own.
     */    
    CCompositeCableStatusFSM& iCompositeCableStatusFSM;
    
    /**
     * TV Out Configurer for listening setting changes.
     * Not own
     */
    CTVOutConfigForComposite* iTVOutConfigForSettingChanges;
    
    // Substate
    TSubState iSubState;
    
    // Flag
    TBool iCableDisconnectedWhileEnabling;
    
    // Flag
    TBool iCableConnectedWhileDisabling;
    
    // Flag
    TBool iCableDisconnectedWhileSetting;

    // Flag
    TBool iCableDisconnectedWhileResetting;
    
    // Flag
    TBool iCableDisconnectedWhileListening;
    
    // Flag
    TBool iCableDisconnectedWhileResettingDisable;
    
    // Flag
    TBool iResettingInput;
    
    // Flag
    TBool iClearConfig;
    
   };

#endif // C_COMPOSITECABLESTATECONNECTED_H
