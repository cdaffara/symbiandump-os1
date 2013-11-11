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
* Description:  Declaration of CCDSYController class
*
*/


#ifndef CDSYCONTROLLER_H
#define CDSYCONTROLLER_H

// INCLUDES
#include <e32base.h>



// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class MCDSYServiceLogic;
class CCDSYController;
class TCDSYMessage;
class CCDSYMessagesServiceLogic;



/**
*  CCDSYController
*  This class implements the CCDSYController, which routes the 
*  messages.
*/
NONSHARABLE_CLASS( CCDSYController ): public CBase
    {
    public:
        /**
        * Destructor.
        */
        ~CCDSYController();

        /**
        * Creates a new instance of CDSYController.
        * @return CCDSYController*                          Pointer to an instance of CCDSYController.
        */
        static CCDSYController* NewL();

        /**
        * Registers a service logic for a specific message.
        * @param const TUint32 aMessageGroup                Message group to which the service logic will be registered.
        * @param const TUint32 aMessageID                   Message ID to which the service logic will be registered.
        * @param MCDSYServiceLogic* aCDSYServiceLogic       Pointer to service logic which will be registered.
        * @return void 
        */
        IMPORT_C void RegisterServiceLogicL( const TUint32 aMessageGroup, const TUint32 aMessageID, MCDSYServiceLogic* aCDSYServiceLogic );

        /**
        * Registers a service logic for a specific message.
        * @param const TUint32 aMessageGroup                Message group to which the service logic will be registered.
        * @param const TUint32 aMessageID                   Message ID to which the service logic will be registered.
        * @param const TUint32 aTransactionID               Message's transaction ID to which the service logic will be registered.
        * @param MCDSYServiceLogic* aCDSYServiceLogic       Pointer to service logic which will be registered.
        * @return void 
        */
        IMPORT_C void RegisterServiceLogicL( const TUint32 aMessageGroup, const TUint32 aMessageID, const TUint32 aTransactionID, MCDSYServiceLogic* aCDSYServiceLogic );

        /**
        * Unregisters service logic for a specific message.
        * @param const TUint32 aMessageGroup                Message group from which the service logic will be unregistered.
        * @param const TUint32 aMessageID                   Message ID from which the service logic will be unregistered.
        * @param const MCDSYServiceLogic* aCDSYServiceLogic Pointer to service logic that will be unregistered.
        * @return void 
        */
        IMPORT_C void UnregisterServiceLogic( const TUint32 aMessageGroup, const TUint32 aMessageID, const MCDSYServiceLogic* aCDSYServiceLogic );

        /**
        * Unregisters service logic for a specific message.
        * @param const TUint32 aMessageGroup                Message group from which the service logic will be unregistered.
        * @param const TUint32 aMessageID                   Message ID from which the service logic will be unregistered.
        * @param const TUint32 aTransactionID               Message's transaction ID from which the service logic will be unregistered.
        * @param const MCDSYServiceLogic* aCDSYServiceLogic Pointer to service logic that will be unregistered.
        * @return void 
        */
        IMPORT_C void UnregisterServiceLogic( const TUint32 aMessageGroup, const TUint32 aMessageID, const TUint32 aTransactionID, const MCDSYServiceLogic* aCDSYServiceLogic );

        /**
        * Processes the incoming TCDSYMessage. The function routes the message to the service logics registered to the message.
        * @param TCDSYMessage& aCDSYMessage                 Message that will be routed.
        * @return void 
        */
        IMPORT_C void ProcessMessageL( TCDSYMessage& aCDSYMessage );

        /**
        * Expires the message.
        * @param TCDSYMessage& aCDSYMessage                 Message to be expired.
        * @return void 
        */
        IMPORT_C void ExpireMessageL( TCDSYMessage& aCDSYMessage );

        /**
        * Returns new transaction ID.
        * @return TUint8    New transaction ID.
        */
        IMPORT_C TUint8 GetNewTransactionID();

    private:
        /**
        * Constructor.
        */
        CCDSYController();

        TUint8 iCurrentTransactionID;                           // Holds the last transaction.
        RPointerArray<CCDSYMessagesServiceLogic> iServiceLogics;// Array for Message - ServiceLogic mappings.
    };



#endif //#ifndef CDSYCONTROLLER_H



//  End of File
