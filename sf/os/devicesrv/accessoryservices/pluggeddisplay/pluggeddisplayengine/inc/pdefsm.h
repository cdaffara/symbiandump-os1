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
* Description: Finite State Machine's body interface for Plugged Display Engine.
*
*/

#ifndef M_PDEFSM_H
#define M_PDEFSM_H


//#include <e32def.h>
//#include <e32cmn.h> 
#include <e32def.h>
#include "multifinitestatemachine.h"
#include <AsyCmdTypes.h>
#include <AsyCommandHandlerBase.h>



// FORWARD DECLARATIONS
class MPDAsyCmdHandler;


// CLASS DECLARATION

/**
 *  Finite State Machine's body interface for Plugged Display Engine.
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
NONSHARABLE_CLASS( MPdeFSM ) : public MFSMForBody
    {
public:
    virtual ~MPdeFSM() {} // Destructor needed for ownership transfer


    /**
     * Process a command received from the Accessory FW.
     * Process command handled will be replied through the MPDAsyCmdHandler
     * interface when the command is fully handled.
     * 
     * @since TB9.2
     * @param aCommand Command to be processed.
     * @param aCmdParams Command parameters.
     * @param aAsyCmdHandler Call back interface to process command response.
     * @return TBool ETrue if command belonged to called FSM.
     */
    virtual TBool ProcessCommandL( 
            const TProcessCmdId aCommand,
            const TASYCmdParams& aCmdParams,
            MPDAsyCmdHandler& aAsyCmdHandler 
            ) = 0;

    /**
     * Get the Generic ID of connected accessory.
     * 
     * @since TB9.2
     * @param aAccPolGenericID Object where to copy actual values.
     * @return Error code. KErrNotFound if don't exist.
     */
    virtual TInt GetAccPolGenericID( TAccPolGenericID& aAccPolGenericID ) = 0;

    };


#endif // M_PDEFSM_H
