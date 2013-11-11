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
* Description:  Declaration of CCDSYMessagesServiceLogic class
*
*/


#ifndef CDSYMESSAGESSERVICELOGIC_H
#define CDSYMESSAGESSERVICELOGIC_H

// INCLUDES
#include <e32base.h>



// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class MCDSYServiceLogic;



/**
*  CCDSYMessagesServiceLogic
*  This class implements the CCDSYMessagesServiceLogic, which stores the message and service logic binding.
*/
NONSHARABLE_CLASS( CCDSYMessagesServiceLogic ): public CBase
    {
    public:
        /**
        * Destructor.
        */
        ~CCDSYMessagesServiceLogic();

        /**
        * Creates a new instance of CCDSYMessagesServiceLogic.
        * @param const TUint32 aMessageGroup            Message group to which the service logic is registered.
        * @param const TUint32 aMessageID               Message ID to which the service logic is registered.
        * @param MCDSYServiceLogic* aCDSYServiceLogic   Pointer to service logic.
        * @return static CCDSYMessagesServiceLogic*     Pointer to an instance of CCDSYMessagesServiceLogic.
        */
        static CCDSYMessagesServiceLogic* NewL( const TUint32 aMessageGroup, const TUint32 aMessageID, MCDSYServiceLogic* aCDSYServiceLogic );

        /**
        * Creates a new instance of CCDSYMessagesServiceLogic.
        * @param const TUint32 aMessageGroup            Message group to which the service logic is registered.
        * @param const TUint32 aMessageID               Message ID to which the service logic is registered.
        * @param const TUint32 aMessageTransactionID    Message's transaction ID to which the service logic is registered.
        * @param MCDSYServiceLogic* aCDSYServiceLogic   Pointer to service logic.
        * @return static CCDSYMessagesServiceLogic*     Pointer to an instance of CCDSYMessagesServiceLogic.
        */
        static CCDSYMessagesServiceLogic* NewL( const TUint32 aMessageGroup, const TUint32 aMessageID, const TUint32 aMessageTransactionID, MCDSYServiceLogic* aCDSYServiceLogic );

        /**
        * Returns the message group to which the service logic is registered.
        * @return inline TUint32                        Message group to which the service logic is registered.
        */
        inline TUint32 MessageGroup() const
            {
            return iMessageGroup;
            }

        /**
        * Returns the message ID to which the service logic is registered.
        * @return TUint32                               Message ID to which the service logic is registered.
        */
        inline TUint32 MessageID() const
            {
            return iMessageID;
            }

        /**
        * Returns the service logic related to the message.
        * @return inline MCDSYServiceLogic*             Pointer to service logic related to the message.
        */
        inline MCDSYServiceLogic* CDSYServiceLogic()
            {
            return iCDSYServiceLogic;
            }

        /**
        * Returns if the service logic is dynamically registered.
        * @return TBool                                 Registration status.
        */
        inline TBool DynamicRegistration() const
            {
            return iDynamicRegistration;
            }

        /**
        * Returns the tranasaction ID to which the service logic is registered.
        * @return TUint32                               Transaction ID.
        */
        inline TUint32 MessageTransactionID() const
            {
            return iMessageTransactionID;
            }

    private:
        /**
        * Constructor.
        * @param const TUint32 aMessageGroup            Message group to which the service logic is registered.
        * @param const TUint32 aMessageID               Message ID to which the service logic is registered.
        * @param const TUint32 aMessageTransactionID    Message's transaction ID to which the service logic is registered.
        * @param const TBool aDynamicRegistration       Dynamic registration.
        * @param MCDSYServiceLogic* aCDSYServiceLogic   Pointer to service logic.
        */
        CCDSYMessagesServiceLogic( const TUint32 aMessageGroup, const TUint32 aMessageID, const TUint32 aMessageTransactionID, const TBool aDynamicRegistration, MCDSYServiceLogic* aCDSYServiceLogic );

    private:
        TUint32 iMessageGroup;
        TUint32 iMessageID;
        TUint32 iMessageTransactionID;
        TBool iDynamicRegistration;
        MCDSYServiceLogic* iCDSYServiceLogic;
    };



#endif // CDSYMESSAGESSERVICELOGIC_H



//  End of File
