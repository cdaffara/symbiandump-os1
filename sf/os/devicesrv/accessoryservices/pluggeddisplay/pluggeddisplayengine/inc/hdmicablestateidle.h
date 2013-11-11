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
* Description:  HDMI Cable Status FSM's "Idle" state.
*
*/


#ifndef C_HDMICABLESTATEIDLE_H
#define C_HDMICABLESTATEIDLE_H


#include "multifinitestatemachine.h"


class CTVOutConfigForHDMI;
class CEDIDHandler;



/**
 * "Idle" state implementation for HDMI Cable Status FSM.
 * HDMI Cable is not connected in this state.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( CHDMICableStateIdle ): public CBase, public MFSMState
    {


public:

    
    /**
     * Two-phased constructor.
     * @param aFSMForState Finite State Machine interface for State.
     * @param aTVOutConfigForHDMI TV Out Configurer.
     * @param aEDIDHandler EDID Handler object.
     */
    static CHDMICableStateIdle* NewL(
            MFSMForState& aFSMForState, 
            CTVOutConfigForHDMI& aTVOutConfigForHDMI,
            CEDIDHandler& aEDIDHandler );
    

    /**
    * Destructor.
    */
    virtual ~CHDMICableStateIdle();

    
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

    
protected:


private:


    /**
     * C++ default constructor.
     * @param aFSMForState Finite State Machine interface for State.
     * @param aTVOutConfigForHDMI TV Out Configurer.
     * @param aEDIDHandler EDID Handler object.
     */
    CHDMICableStateIdle(
            MFSMForState& aFSMForState, 
            CTVOutConfigForHDMI& aTVOutConfigForHDMI,
            CEDIDHandler& aEDIDHandler );

    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();

private: // data

    enum TSubState
      {
      ESubStateUndefined = 0
      };

    /**
     * FSM.
     * Not own.
     */
    MFSMForState& iFSMForState;
    
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


    // Substate
    TSubState iSubState;

    };

#endif // C_HDMICABLESTATEIDLE_H
