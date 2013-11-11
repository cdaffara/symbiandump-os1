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
* Description: M class definitions for Finite State Machine's (FSM's)
* body, fsm (subautomaton) and state. 
*
*/

#ifndef M_MULTIFINITESTATEMACHINE_H
#define M_MULTIFINITESTATEMACHINE_H


#include <e32def.h>
#include <e32cmn.h> 
#include <e32des8.h>


/**
 * Data type for FSM's states. Each implemented FSM has an enumeration
 * for state identifiers and enum value is assigned to a variable of 
 * this type so that FSM framework can handle value transparently.  
 */
typedef TInt TFSMState;

/**
 * Data type for FSM's identifications. Each implemented FSM body has an enumeration
 * for included FSMs and enum value is assigned to a variable of this type so that
 * FSM framework can handle value transparently.  
 */
typedef TInt TFSMId;


/**
 * Data type for Interface identifications. Every interface used by a FSM Body has to have
 * an unique identifier i.e enumeration constant of which value is assigned to a variable
 * of this type so that FSM framework can handle value transparently.  
 */
typedef TInt TFSMInterfaceId;

/**
 * Data type for Interface's event identifications. Every interface has to have unique
 * identifiers for its events i.e enumeration constant of which value is assigned to a variable
 * of this type so that FSM framework can handle value transparently.  
 */
typedef TInt TFSMEventId;

/**
 * Data type for FSM's main- and substate. Each implemented FSM has an enumeration
 * for main- an substate identifiers and enum values are assigned to a variable of 
 * this type so that FSM framework can handle values transparently.  
 */
class TFSMMainAndSubState
    {
public:
    inline TFSMMainAndSubState(): iMainState(0),iSubstate(0){};
    TFSMState iMainState;
    TFSMState iSubstate;
    };

// FORWARD DECLARATIONS
class MFSMState;
class MFSMForBody;


// CLASS DECLARATION

/**
 *  Finite State Machine's body interface for its owner. Body can include 1 to n 
 *  state machines.
 *
 *  This callback interface has to be implemented by FSM body
 *  implementation class.
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( MFSMBody )
    {
public:
    virtual ~MFSMBody() {} // Destructor needed for ownership transfer

    /**
     * Function is called by host object to indicate that FSM can proceed to 
     * initial state and make necessary initialization actions.
     * 
     * @since TB 9.2
     * @param none. 
     * @return void.
     */
    virtual void Start() = 0;


    /**
     * Get the id of current state.
     *
     * @since TB 9.2
     * @param aFSMId FSM identification. 
     * @return TFSMState Current state identification.
     */
    virtual TFSMState CurrentStateID( TFSMId aFSMId = 0 ) = 0;

    /**
     * Function is called whenever an event that might have impact 
     * on the state machine occurs.
     *
     * @since TB 9.2
     * @param aFSMId FSM identification.
     * @param aInterfaceId Interface identification.
     * @param aEvent Event identification.
     * @return none.
     */
    virtual void Input
        (
        TFSMId aFSMId = 0,
        TFSMInterfaceId aInterfaceId = 0,
        TFSMEventId aEvent = 0      
        ) = 0;

    /**
     * Get the pointer of current state object.
     *
     * @since TB 9.2
     * @param aFSMId FSM identification. 
     * @return MFSMState* Current state's interface pointer.
     */
    virtual  MFSMState* CurrentState( TFSMId aFSMId = 0 ) = 0;

    /**
     * Get the name of current state.
     *
     * @since TB 9.2
     * @param aFSMId FSM identification. 
     * @return TPtrC Name of the current state.
     */
    virtual TPtrC CurrentStateName( TFSMId aFSMId = 0 ) = 0;

    /**
     * Get pointer of a given FSM.
     *
     * @since TB 9.2
     * @param aFSMId FSM identification. 
     * @return MFSMForBody Pointer of the identified FSM.
     */
    virtual MFSMForBody* GetFSM( TFSMId aFSMId = 0 ) = 0;
    
    /**
     * Get the ids of current main and substate id.
     *
     * @since TB 9.2
     * @param aFSMId FSM identification. 
     * @return TFSMMainAndSubState Current main- and substate ids.
     *         Substate id is zero if current state havent any substates.
     */
    virtual TFSMMainAndSubState CurrentMainAndSubstateIDs( TFSMId aFSMId = 0 ) = 0;
    };

/**
 *  FSM's interface for its (owning) Body. 
 *
 *  This callback interface has to be implemented by FSM
 *  implementation class.
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none
 *  @since TB 9.2
 */
class MFSMForBody
    {
public:
    virtual ~MFSMForBody() {} // Destructor needed for ownership transfer

    /**
     * Function is called by host object to indicate that FSM can proceed to 
     * initial state and make necessary initialization actions.
     * 
     * @since TB 9.2
     * @param none. 
     * @return void.
     */
    virtual void Start() = 0;


    /**
     * Get the FSM's Id.
     *
     * @since TB 9.2
     * @param none. 
     * @return TFSMId FSM identification.
     */
    virtual TFSMId FSMID() = 0;

    /**
     * Get the id of current state.
     *
     * @since TB 9.2
     * @param none. 
     * @return TFSMState Current state identification.
     */
    virtual TFSMState CurrentStateID() = 0;

    /**
     * Function is called whenever an event that might have impact 
     * on the state machine occurs.
     *
     * @since TB 9.2
     * @param aInterfaceId Interface identification.
     * @param aEvent Event identification.
     * @return none.
     */
    virtual void Input
        (
        TFSMInterfaceId aInterfaceId = 0,
        TFSMEventId aEvent = 0      
        ) = 0;

    /**
     * Get the pointer of current state object.
     *
     * @since TB 9.2
     * @param none. 
     * @return MFSMState* Current state's interface pointer.
     */
    virtual  MFSMState* CurrentState() = 0;

    /**
     * Get the name of current state.
     *
     * @since TB 9.2
     * @param none. 
     * @return TPtrC Name of the current state.
     */
    virtual TPtrC CurrentStateName() = 0;

    /**
     * Get the ids of current main and substate id.
     *
     * @since TB 9.2
     * @param none. 
     * @return TFSMMainAndSubState Current main- and substate ids.
     *         Substate id is zero if current state havent any substates.
     */
    virtual TFSMMainAndSubState CurrentMainAndSubstateIDs() = 0;

    };

/**
 *  FSM's interface for its State. 
 *
 *  This callback interface has to be implemented by FSM
 *  implementation class.
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none
 *  @since TB 9.2
 */
class MFSMForState
    {
public:
 
    virtual ~MFSMForState() {} // Destructor needed for ownership transfer
    
    /**
     * Function is called by  current state to indicate that FSM has to change
     * its state. Pervious state has performed necessary exit actions if any.
     *
     * @since TB 9.2
     * @param aNextState Next state where to transit. 
     * @return TBool True is transition valid. False if nextstate value invalid.
     */
    virtual TBool Transit( TFSMState aNextState ) = 0;
    
    };

/**
 *  FSM's State interface. 
 *
 *  This callback interface has to be implemented by FSM state
 *  implementation class.
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none
 *  @since TB 9.2
 */
class MFSMState
    {
public:
 
    virtual ~MFSMState() {} // Destructor needed for ownership transfer
 
    /**
     * Function is called when entering the state. State can perform
     * necessary entry actions if any.
     *
     * @since TB 9.2
     * @param none. 
     * @return void.
     */
    virtual void Enter() = 0;

    /**
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
            TFSMInterfaceId aInterfaceId = 0,
            TFSMEventId aEvent = 0 
            ) = 0;
    /**
     * Function returns state's identification.
     *
     * @since TB 9.2
     * @param none. 
     * @return TFSMState State's identification.
     */
    virtual TFSMState Id() = 0;

    /**
     * Function returns state's name.
     *
     * @since TB 9.2
     * @param none. 
     * @return TPtrC Name of the state.
     */
    virtual TPtrC Name() = 0;
    
    /**
     * Function returns substate's identification.
     *
     * @since TB 9.2
     * @param none. 
     * @return TFSMState Substate's identification.
     */
    virtual TFSMState SubId() = 0;

    };


#endif // M_MULTIFINITESTATEMACHINE_H
