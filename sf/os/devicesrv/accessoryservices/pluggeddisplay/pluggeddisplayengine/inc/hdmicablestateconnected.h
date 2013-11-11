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


#ifndef C_HDMICABLESTATECONNECTED_H
#define C_HDMICABLESTATECONNECTED_H


#include "multifinitestatemachine.h"
#include "centralrepositorywatch.h" 


class CEDIDHandler;
class CAccessoryControlIf;
class CTVOutConfigForHDMI;
class CHDMICableStatusFSM;


/**
 * "Connected" state implementation for HDMI Cable Status FSM.
 * HDMI Cable is connected in this state.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( CHDMICableStateConnected ): public CBase, public MFSMState, public MCentralRepositoryObserver
    {


public:

    
    /**
     * Two-phased constructor.
     * @param aTVOutConfigForHDMI TV Out Configurer for cable connection listening.
     * @param aEDIDHandler EDID Handler object.
     * @param aAccessoryControlIf Accessory Control object.
     * @param aHDMICableStatusFSM HDMI Cable Status FSM.
     */
    static CHDMICableStateConnected* NewL( 
            CTVOutConfigForHDMI& aTVOutConfigForHDMI,
            CEDIDHandler& aEDIDHandler,
            CAccessoryControlIf& aAccessoryControlIf,
            CHDMICableStatusFSM&  aHDMICableStatusFSM );
    

    /**
    * Destructor.
    */
    virtual ~CHDMICableStateConnected();

    
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
     * @param aInterfaceId Interface identification.
     * @param aEvent Event identification.
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

    // Substate constants
    enum TSubState
      {
      // Prefer not to change the values, since they are used
      // for example in test modules
      ESubStateUndefined                    = 0x0,
      ESubStateFechEdidData                 = 0x10,
      ESubStateWaitForSettingsChanged       = 0x11,
      ESubStateEnable                       = 0x12,
      ESubStateConnectAccessory             = 0x13,
      ESubStateStartListenSettingsChanges   = 0x14,
      
      ESubStateConnected                    = 0x21,
      
      ESubStateIdlingDisable                = 0x30,
      ESubStateIdlingDisconnectAccessory    = 0x31,
      
      ESubStateResettingEnable              = 0x40,
      ESubStateResettingDisable             = 0x41,
     
      ESubStateRejectingDisable             = 0x50,
      ESubStateRejectingDisconnectAccessory = 0x51
      };    
    
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
     * Start listen to Copy Protection Status changes.
     *
     * @since TB 9.2
     * @param none. 
     * @return void.
     */
    void ListenCopyProtectionStatusChanges();

    /**
     * Input function for substate Fetch Edid Data.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateFetchEdidDataInput( 
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
     * Input function for substate Enabling.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateEnableInput(
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );

    /**
     * Input function for substate Enabling for resetting
     * TV Out Configuration settings.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateResettingEnableInput(
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );

    /**
     * Input function for substate Disabling before
     * going to Idle state.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateIdlingDisableInput(
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
     * Input function for substate Disabling for rejecting
     * the connected cable. 
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateRejectingDisableInput(
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );

    /**
     * Input function for substate Connect Accessory.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateConnectAccessoryInput(
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );
    
    /**
     * Input function for substate Disconnect Accessory before
     * going to Idle state.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateIdlingDisconnectAccessoryInput(
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );

    /**
     * Input function for substate Disconnect Accessory before
     * going to Rejected state.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateRejectingDisconnectAccessoryInput(
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );

    /**
     * Input function for substate Connected.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateConnectedInput(
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );

    /**
     * Input function for substate Wait For Setting Changes.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface Id. 
     * @param aEvent Interface event Id.
     * @return none.
     */
    void SubStateWaitForSettingsChangedInput(
            TFSMInterfaceId aInterfaceId ,
            TFSMEventId aEvent );

	void ClearAvailableTvOutConfig();

    /**
     * C++ default constructor.
     * @param aTVOutConfigForHDMI TV Out Configurer for cable connection listening.
     * @param aEDIDHandler EDID Handler object.
     * @param aAccessoryControlIf Accessory Control object.
     * @param aHDMICableStatusFSM HDMI Cable Status FSM..
     */
    CHDMICableStateConnected(
            CTVOutConfigForHDMI& aTVOutConfigForHDMI,
            CEDIDHandler& aEDIDHandler,
            CAccessoryControlIf& aAccessoryControlIf,
            CHDMICableStatusFSM&  aHDMICableStatusFSM );

    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();

private: // data


    struct SCopyProtectListenFailCounter
        {
        TUint8 iCount;
        enum
            {
            ECounterMaxValue = 5
            };
        };

    struct SCableStatusListenFailCounter
        {
        TUint8 iCounter;
        enum 
            {
            EMaxCounterValue = 5
            };
        };


    /**
     * TV Out Configurer.
     * Not own.
     */
    CTVOutConfigForHDMI& iTVOutConfigForHDMI;

    /**
     * EDID data Handler.
     * Not own.
     */
    CEDIDHandler& iEDIDHandler;

    /**
     * Accessory Control Interface.
     * Not own
     */
    CAccessoryControlIf& iAccessoryControlIf;
    
    /**
     * TV Out Configurer for Copy Protection listening and setting .
     * Not own.
     */
    CTVOutConfigForHDMI* iTVOutConfigForCopyProtect;

    /**
     * TV Out Configurer for listening setting changes.
     * Not own
     */
    CTVOutConfigForHDMI* iTVOutConfigForSettingChanges;
        
    /**
     * HDMI Cable Status FSM.
     * Not own.
     */
    CHDMICableStatusFSM&  iHDMICableStatusFSM;
        
    // Counter for Copy Protection status listen failures.
    SCopyProtectListenFailCounter iCopyProtectListenFailCounter;
    
    // Counter for Cable status listen failures.
    SCableStatusListenFailCounter iCableStatusListenFailCounter;
        
    // Substate
    TSubState iSubState;

	//CR Observer for ovescan percentage
    //CCentralRepositoryWatch* iCRWatchForHOverScan;
    CCentralRepositoryWatch* iCRWatchForVOverScan;

    // Current overscan value
	TInt iHOverscanValue;
	TInt iVOverscanValue;
	
    };

#endif // C_HDMICABLESTATECONNECTED_H
