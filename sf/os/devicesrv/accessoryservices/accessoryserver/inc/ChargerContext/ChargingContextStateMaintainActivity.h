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
* Description:  Charging Context Controller's "Maintain Activity" state.
*
*/



#ifndef C_CHARGINGCONTEXTSTATEMAINTAINACTIVITY_H
#define C_CHARGINGCONTEXTSTATEMAINTAINACTIVITY_H


#include "FiniteStateMachine.h"
#include "AccSrvChargingContextController.h"


class CAccSrvChargingContextController;



/**
 * "Maintain activity" state implementation for Charging Context 
 * Controller.
 * In this state User::ResetInactivityTime() is called periodically
 * in order to maintain circumtances - i.e keep backlight on, 
 * screensaver off, keylock off and autolock off.
 * In case charges is disconnected state machine retreats back to 
 * "wait for chargere" state.
 * In case display orientation changes to other value than "up",
 * state machine returns back to state "charging".  
 * 
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib AccServer.exe
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CChargingContextStateMaintainActivity ): public CBase, public MFSMState
    {


public:

    
    /**
     * Two-phased constructor.
     * @param aFSMBody Body of the Finite State Machine.
     * @param aChargingContextController Host class object that owns the FSM.
     */
    static CChargingContextStateMaintainActivity* NewL(
            MFSMBody* aFSMBody, 
            CAccSrvChargingContextController* aChargingContextController );
    

    /**
    * Destructor.
    */
    virtual ~CChargingContextStateMaintainActivity();

    
// from base class MFSMState


    /**
     * From MFSMState.
     * Function is called when entering the state. State can perform
     * necessary entry actions if any.
     *
     * @since S60 5.1
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
     * @since S60 5.1
     * @param none. 
     * @return none.
     */
    virtual void Input();

    /**
     * From MFSMState.
     * Function returns state's identification.
     *
     * @since S60 5.1
     * @param none. 
     * @return TFSMState State's identification.
     */
    virtual TFSMState Id();

    /**
     * From MFSMState.
     * Function returns state's name.
     *
     * @since S60 5.1
     * @param none. 
     * @return TPtrC Name of the state.
     */
    virtual TPtrC Name();

    
protected:


private:


    /**
     * C++ default constructor.
     */
    CChargingContextStateMaintainActivity();

    /**
     * By default Symbian OS constructor is private.
     * @param aFSMBody Body of the Finite State Machine.
     * @param aChargingContextController Host class object that owns the FSM.
     */
    void ConstructL(
            MFSMBody* aFSMBody, 
            CAccSrvChargingContextController* aChargingContextController );

private: // data

    /**
     * Host class object that owns the FSM.
     * Not own.
     */
    CAccSrvChargingContextController* iChargingContextController;

    /**
     * FSM body.
     * Not own.
     */
    MFSMBody* iFSMBody;

    };

#endif // C_CHARGINGCONTEXTSTATEMAINTAINACTIVITY_H
