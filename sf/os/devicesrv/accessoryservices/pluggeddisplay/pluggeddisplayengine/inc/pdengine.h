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
 * Description:
 * CPDEngine class declaration.
 *
 */

#ifndef PDENGINE_H
#define PDENGINE_H

// SYSTEM INCLUDE FILES
#include <e32base.h>
#include <AsyCommandHandlerBase.h>
#include "pdefsm.h"
#include "pdeconstants.h"

// USER INCLUDE FILES

// FORWARD DECLARATIONS
class MPDAsyCmdHandler;
class RAccessoryServer;

// DATA TYPES

// CLASS DECLARATION

/**
 * CPDEngine class provides the main functionality in the plugged dislay.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *  
 * @lib pluggeddisplayengine.lib
 * @since S60 TB9.2
 */
NONSHARABLE_CLASS( CPDEngine ): public CBase, public MFSMBody
    {
public:

    /**
     * Symbian two phased constructors.
     *
     * @since S60 TB9.2
     * @param aAccessoryServer Accessory Server.
     * @return CPDEngine
     */
    IMPORT_C static CPDEngine* NewL( RAccessoryServer& aAccessoryServer );
    IMPORT_C static CPDEngine* NewLC( RAccessoryServer& aAccessoryServer );

    /**
     * C++ destructor.
     */
    virtual ~CPDEngine();

public:

    /**
     * Process a command received from the Accessory FW.
     * Process command handled will be replied through the MPDAsyCmdHandler
     * interface when the command is fully handled.
     * 
     * @since TB9.2
     * @param aCommand Command to be processed.
     * @param aCmdParams Command parameters.
     * @param aAsyCmdHandler Call back interface to process command response.
     * @return None.
     */
    IMPORT_C void ProcessCommandL( const TProcessCmdId aCommand,
        const TASYCmdParams& aCmdParams,
        MPDAsyCmdHandler& aAsyCmdHandler );


    /**
     * Get Unique ID of a cable accessory if available.
     *
     * @since TB 9.2
     * @param aFSMId FSM identification.
     * @param aAccPolGenericID Object where to copy actual values.
     * @return Error code. KErrNotFound if don't exist.
     */
    virtual TInt GetAccPolGenericID( TFSMId aFSMId, TAccPolGenericID& aAccPolGenericID );

// from base class MFSMBody
    /**
     * From MFSMBody.
     * Get the id of current state.
     *
     * @since TB 9.2
     * @param aFSMId FSM identification.
     * @return TFSMState Current state identification.
     */
    virtual TFSMState CurrentStateID( TFSMId aFSMId );

    /**
     * From MFSMBody.
     * Function is called whenever an event that might have impact 
     * on the state machine occurs.
     *
     * @since TB 9.2
     * @param aFSMId FSM identification.
     * @param aInterfaceId Interface identification.
     * @param aEvent Event identification.
     * @return none.
     */
    virtual void Input( 
            TFSMId aFSMId,
            TFSMInterfaceId aInterfaceId,
            TFSMEventId aEvent );

    /**
     * From MFSMBody.
     * Get the pointer of current state object.
     *
     * @since TB 9.2
     * @param aFSMId FSM identification.
     * @return MFSMState* Current state's interface pointer.
     */
    virtual MFSMState* CurrentState( TFSMId aFSMId );

    /**
     * From MFSMBody.
     * Get pointer of a given FSM.
     *
     * @since TB 9.2
     * @param aFSMId FSM identification. 
     * @return MFSMForBody Pointer of the identified FSM.
     */
    virtual MFSMForBody* GetFSM( TFSMId aFSMId = 0 );

    /**
     * From MFSMBody.
     * Get the ids of current main and substate id.
     *
     * @since TB 9.2
     * @param aFSMId FSM identification. 
     * @return TFSMMainAndSubState Current main- and substate ids.
     *         Substate id is zero if current state haven't any substates.
     */
    virtual TFSMMainAndSubState CurrentMainAndSubstateIDs( TFSMId aFSMId = 0 );
  
    /**
     * From MFSMBody.
     * Get the name of current state.
     *
     * @since TB 9.2
     * @param aFSMId FSM identification.
     * @return TPtrC Name of the current state.
     */
    virtual TPtrC CurrentStateName( TFSMId aFSMId );


private:

    // from base class MFSMBody

    /**
      * From MFSMBody.
     * Function is called by host object to indicate that FSM can proceed to 
     * initial state and make necessary initialization actions.
     * 
     *  @since TB 9.2
     * @param none. 
     * @return void.
     */
    virtual void Start();

    CPDEngine();
    void ConstructL( RAccessoryServer& aAccessoryServer );

private: // Data
    
    /**
     * An array for FSM pointers.
     * Own
     */
    MPdeFSM* iFSMPtr[ EPDEFSMIdNbrOfUsedIds ];
    
    };

#endif // PDENGINE_H
