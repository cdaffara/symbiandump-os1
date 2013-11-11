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
* Description:  Composite Cable Status FSM's "Idle" state.
*
*/


#ifndef C_COMPOSITECABLESTATEIDLE_H
#define C_COMPOSITECABLESTATEIDLE_H


#include "multifinitestatemachine.h"



/**
 * "Idle" state implementation for Composite Cable Status FSM.
 * Composite Cable is not connected in this state.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( CCompositeCableStateIdle ): public CBase, public MFSMState
    {


public:

    
    /**
     * Two-phased constructor.
     * @param aPDEngine FSM Body object that owns the FSM.
     */
    static CCompositeCableStateIdle* NewL(
            MFSMForState& aFSMForState );
    

    /**
    * Destructor.
    */
    virtual ~CCompositeCableStateIdle();

    
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
            TFSMEventId aEvent);

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


private:


    /**
     * C++ default constructor.
     * @param aPDEngine FSM Body object that owns the FSM.
     */
    CCompositeCableStateIdle(
            MFSMForState& aFSMForState);

    /**
     * By default Symbian OS constructor is private.
     * @param aPDEngine FSM Body object that owns the FSM.
     */
    void ConstructL( );

private: // data

    /**
     * FSM.
     * Not own.
     */
    MFSMForState& iFSMForState;

    };

#endif // C_COMPOSITECABLESTATEIDLE_H
