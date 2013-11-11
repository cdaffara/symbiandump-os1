/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Finite State Machine class for Charging Context Controller.
*
*/



#ifndef C_CHARGINGCONTEXTFSMBODY_H
#define C_CHARGINGCONTEXTFSMBODY_H



#include "FiniteStateMachine.h"
#include "AccSrvChargingContextController.h"


class CAccSrvChargingContextController;

/**  Constants for states of Charging Context FSM.  */
enum TChargerContextState
    {
    EChargerContextStateUndefined = 0,
    EChargerContextStateWaitForCharger,
    EChargerContextStateCharging,
    EChargerContextStateMaintainActivity,
    // Add new constants before this row.
    // Following constant is not a state.
    // It just indicates maximum index value.
    EChargerContextStateMaxValue
    };

/**
 *  Finite State Machine class for Charging Context Controller.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib AccServer.exe
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CChargingContextFSMBody ): public CBase, public MFSMBody
    {

public:

    /**
     * Two-phased constructor.
     * @param aChargingContextController Host class object that owns the FSM.
     */
    static CChargingContextFSMBody* NewL(
        CAccSrvChargingContextController* aChargingContextController );
    

    /**
    * Destructor.
    */
    virtual ~CChargingContextFSMBody();


// from base class MFSMBody


    /**
      * From MFSMBody.
     * Function is called by host object to indicate that FSM can proceed to 
     * initial state and make necessary initialization actions.
     * 
     * @since S60 5.1
     * @param none. 
     * @return void.
     */
    virtual void Start();

    /**
     * From MFSMBody.
     * Function is called by  current state to indicate that FSM has to change
     * its state. Pervious state has performed necessary exit actions if any.
     *
     * @since S60 5.1
     * @param aNextState Next state where to transit. 
     * @return TBool True is transition valid. False if nextstate value invalid.
     */
    virtual TBool Transit( TFSMState aNextState );

    /**
     * From MFSMBody.
     * Get the id of current state.
     *
     * @since S60 5.1
     * @param none. 
     * @return TFSMState Current state identification.
     */
    virtual TFSMState CurrentStateID();

    /**
     * From MFSMBody.
     * Function is called whenever an event that might have impact 
     * on the state machine occurs.
     *
     * @since S60 5.1
     * @param none. 
     * @return none.
     */
    virtual void Input();

    /**
     * From MFSMBody.
     * Get the pointer of current state object.
     *
     * @since S60 5.1
     * @param none. 
     * @return MFSMState* Current state's interface pointer.
     */
    virtual MFSMState* CurrentState();

    /**
     * From MFSMBody.
     * Get the name of current state.
     *
     * @since S60 5.1
     * @param none. 
     * @return TPtrC Name of the current state.
     */
    virtual TPtrC CurrentStateName();


protected:


private:

    /**
     * C++ default constructor.
     */
    CChargingContextFSMBody();

    /**
     * By default Symbian OS constructor is private.
     * @param aChargingContextController Host class object that owns the FSM.
     */
    void ConstructL(
        CAccSrvChargingContextController* aChargingContextController );


private: // data

    /**
     * An array for state object pointers.
     * Own
     */
     // TFixedArray< > ...
    MFSMState* iStateArray[ EChargerContextStateMaxValue ];
    
    /**
     * Identification of the current state. Used to point the 
     * corresponding state object in the above array. 
     * 0 < a valid state id =< EChargerContextStateMaxValue
     */
    TFSMState iCurrentStateId;

    /**
     * Host class object that owns the FSM.
     * Not own.
     */
    CAccSrvChargingContextController* iChargingContextController;

    };


#endif // C_CHARGINGCONTEXTFSMBODY_H
