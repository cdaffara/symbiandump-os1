/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef CMMSMSGSMEXT_H
#define CMMSMSGSMEXT_H

//  INCLUDES
#include <et_phone.h>
#include "cmmsmsextinterface.h"

//  FORWARD DECLARATIONS
class CMmSmsTsy;

// CLASS DECLARATION

/**
*  CMmSmsGsmExt contains mode-dependent phone functionality for GSM phone.
*  Extension request handles are stored in MM Phone object.
*/
NONSHARABLE_CLASS( CMmSmsGsmExt ) : public CMmSmsExtInterface
    {
    public:
        /**
         * Two-phased constructor.
         */
        static CMmSmsGsmExt* NewL();

        /**
         * Destructor
         */
        virtual ~CMmSmsGsmExt();

        /**
         * Takes reception of incoming unstored SMS
         *         
         * 
         * @param aSmsMsg Pointer to the sms message
         * @param iReceiveMessageParamsPtr Pointer to the Receive Message 
         *        params
         * @param iReceiveMessagePduPtr Pointer to the Receive Message Pdu
         * @return Error value
         */
        virtual TInt CompleteReceiveMessage( TSmsMsg* aSmsMsg, 
            TAny* iReceiveMessageParamsPtr,
            TDes8* iReceiveMessagePduPtr );

        /**
         * Set store info attributes values
         *         
         * 
         * @param iGetMessageStoreInfoPtr Pointer to get message store info
         * @param aTotalEntries Total entries
         * @param aUsedEntries Used entries
         * @return KErrNone
         */
        virtual TInt MessageStoreInfo( TAny* iGetMessageStoreInfoPtr,
            TInt aTotalEntries,
            TInt aUsedEntries );

        /**
         * Set attributes values to the sms store info
         *         
         * 
         * @param iGetInfoInfoPtr Pointer to store info
         * @param aTotalEntries Total entries
         * @param aUsedEntries Used entries
         * @return KErrNone
         */
        virtual TInt StoreInfo( TAny* iGetInfoInfoPtr, 
            TInt aTotalEntries,
            TInt aUsedEntries );

        /**
         * Read the list, store its content and then return size of this  
         * buffer to client. This methos runs until TSY's internal SMS storage 
         * is empty.
         *         
         * 
         * @param iSmsListArray SMS message list storage array
         * @param iReadAllId Read all SMS messages Id
         * @param iSmsReadAllArray Read all SMS messages array
         * @param iReadAllBufSizePtr Read all SMS messages pointer
         * @return KErrNone
         */
        virtual TInt CompleteReadAllSmsPhase1L( CArrayPtrFlat<TSmsMsg>* iSmsListArray,
            RMobilePhone::TClientId iReadAllId,
            CArrayPtrFlat<CListReadAllAttempt>* iSmsReadAllArray,
            TInt* iReadAllBufSizePtr );

        /**
         * Return supported entry.
         *         
         * 
         * @return Entry Value
         */
        virtual TInt GetSupportedEntry();

    protected:
        
        /**
         * C++ default constructor.
         */
        CMmSmsGsmExt();

    };

#endif // CMMSMSGSMEXT_H

// End of File
