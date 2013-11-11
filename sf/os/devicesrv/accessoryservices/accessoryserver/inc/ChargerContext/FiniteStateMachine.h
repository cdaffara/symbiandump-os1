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
* Description:  Interfaces of Finite State Machine (.
*
*/



#ifndef M_FINITESTATEMACHINE_H
#define M_FINITESTATEMACHINE_H


#include <e32def.h>
#include <e32cmn.h> 
#include <e32des8.h>


typedef TInt TFSMState;

class MFSMState;


/**
 *  Finite State Machine's body interface. 
 *
 *  This callback interface has to be implemented by FSM body
 *  implementation class.
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib AccServer.exe
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( MFSMBody )
    {
public:
    virtual ~MFSMBody() {} // Destructor needed for ownership transfer

    /**
     * Function is called by host object to indicate that FSM can proceed to 
     * initial state and make necessary initialization actions.
     * 
     * @since S60 5.1
     * @param none. 
     * @return void.
     */
    virtual void Start() = 0;

    /**
     * Function is called by  current state to indicate that FSM has to change
     * its state. Pervious state has performed necessary exit actions if any.
     *
     * @since S60 5.1
     * @param aNextState Next state where to transit. 
     * @return TBool True is transition valid. False if nextstate value invalid.
     */
    virtual TBool Transit( TFSMState aNextState ) = 0;

    /**
     * Get the id of current state.
     *
     * @since S60 5.1
     * @param none. 
     * @return TFSMState Current state identification.
     */
    virtual TFSMState CurrentStateID() = 0;

    /**
     * Function is called whenever an event that might have impact 
     * on the state machine occurs.
     *
     * @since S60 5.1
     * @param none. 
     * @return none.
     */
    virtual void Input() = 0;

    /**
     * Get the pointer of current state object.
     *
     * @since S60 5.1
     * @param none. 
     * @return MFSMState* Current state's interface pointer.
     */
    virtual  MFSMState* CurrentState() = 0;

    /**
     * Get the name of current state.
     *
     * @since S60 5.1
     * @param none. 
     * @return TPtrC Name of the current state.
     */
    virtual TPtrC CurrentStateName() = 0;

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
 *  @lib AccServer.exe
 *  @since S60 5.1
 */
class MFSMState
    {
public:
 
    virtual ~MFSMState() {} // Destructor needed for ownership transfer
 
    /**
     * Function is called when entering the state. State can perform
     * necessary entry actions if any.
     *
     * @since S60 5.1
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
     * @since S60 5.1
     * @param none. 
     * @return none.
     */
    virtual void Input() = 0;
    /**
     * Function returns state's identification.
     *
     * @since S60 5.1
     * @param none. 
     * @return TFSMState State's identification.
     */
    virtual TFSMState Id() = 0;

    /**
     * Function returns state's name.
     *
     * @since S60 5.1
     * @param none. 
     * @return TPtrC Name of the state.
     */
    virtual TPtrC Name() = 0;
    
    };


#endif // M_FINITESTATEMACHINE_H
