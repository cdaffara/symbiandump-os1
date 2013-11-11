
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
* Description:  Declaration of TCDSYMessage class
*
*/


#ifndef CDSYMESSAGE_H
#define CDSYMESSAGE_H


// INCLUDES
#include <e32base.h>



// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS



/**
*  TCDSYMessage
*  This class implements the TCDSYMessage, which is a generic DSY message class.
*/
NONSHARABLE_CLASS( TCDSYMessage )
    {
    public:
        /**
        * Constructor.
        * @param aGroup         Message group.
        * @param aID            Message ID.
        * @param aData          Pointer to data.
        * @param aTransactionID Transaction ID
        */
        IMPORT_C TCDSYMessage( const TUint32 aGroup, const TUint32 aID, const TDesC8* aData, const TUint32 aTransactionID );

        /**
        * Constructor.
        * @param aGroup         Message group.
        * @param aID            Message ID.
        * @param aData          Pointer to data.
        */
        IMPORT_C TCDSYMessage( const TUint32 aGroup, const TUint32 aID, const TDesC8* aData );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~TCDSYMessage();

        /**
        * Returns message group.
        * @return               Message group.
        */
        inline TUint32 Group() const
            {
            return iGroup;
            }

        /**
        * Returns message ID.
        * @return               Message ID.
        */
        inline TUint32 ID() const
            {
            return iID;
            }

        /**
        * Returns messages transaction ID.
        * @return              Messages transaction ID.
        */
        inline TUint32 TransactionID() const
            {
            return iTransactionID;
            }

        /**
        * Returns messages data.
        * @return              Message data.
        */
        inline const TDesC8* Data() const
            {
            return iData;
            }

        /**
        * Return messages return data.
        * @return              Message's return data.
        */
        inline const TDesC8* ReturnData() const
            {
            return iReturnData;
            }

        /**
        * Sets the messages return data.
        * @param aReturnData    Message's return data.
        * @return void
        */
        IMPORT_C void SetReturnDataL( const TDesC8* aReturnData );

        /**
        * Sets the transaction ID.
        * @param aTransactionID Transaction ID.
        * @return void
        */
        inline void SetTransactionID( TUint32 aTransactionID )
            {
            iTransactionID = aTransactionID;
            }

    private:
        TUint32 iGroup;
        TUint32 iID;
        const TDesC8* iData;
        TUint32 iTransactionID;
        HBufC8* iReturnData;
    };



#endif //#ifndef CDSYMESSAGE_H



//  End of File
