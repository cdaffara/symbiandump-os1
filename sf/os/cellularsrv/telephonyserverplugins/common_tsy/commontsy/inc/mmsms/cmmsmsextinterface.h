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



#ifndef CMMSMSEXTINTERFACE_H
#define CMMSMSEXTINTERFACE_H

//  INCLUDES
#include <e32def.h>
#include <et_tsy.h>
#include <etelmm.h>

//  FORWARD DECLARATIONS
class TDataPackage;
class CMmSmsTsy;
class CMmPhoneTsy;
struct TSmsMsg;
class CListReadAllAttempt;

// CLASS DECLARATION

/**
*  CMmSmsExtInterface is used to provide an extension interface for system 
*  specific phone object extensions.
*/
NONSHARABLE_CLASS( CMmSmsExtInterface ) : public CBase
	{
    public:

        /**
        * Destructor
        */
        virtual ~CMmSmsExtInterface();
        
        /**
         * Default implementation of CompleteReceiveMessage method
         *          
         * 
         * @param aSmsMsg Pointer to the sms message
         * @param aReceiveMessageParamsPtr Pointer to the receive message 
         *        params
         * @param aReceiveMessagePduPtr Pointer to the receive message pdu
         * @return KErrNotSupported
         */
        virtual TInt CompleteReceiveMessage( TSmsMsg* aSmsMsg, 
            TAny* aReceiveMessageParamsPtr,
            TDes8* aReceiveMessagePduPtr ) = 0;

        /**
         * Default implementation of MessageStoreInfo method
         *          
         * 
         * @param aGetMessageStoreInfoPtr Pointer to get message store info
         * @param aTotalEntries Total entries
         * @param aUsedEntries Used entries
         * @return KErrNotSupported
         */
        virtual TInt MessageStoreInfo( TAny* aGetMessageStoreInfoPtr,
            TInt aTotalEntries,
            TInt aUsedEntries );

        /**
         * Default implementation of StoreInfo method
         *          
         * 
         * @param aGetMessageStoreInfoPtr Pointer to store info
         * @param aTotalEntries Total entries
         * @param aUsedEntries Used entries
         * @return KErrNotSupported
         */
        virtual TInt StoreInfo( TAny* aGetMessageStoreInfoPtr,
            TInt aTotalEntries,
            TInt aUsedEntries );

        /**
         * Default implementation of CompleteReadAllSmsPhase1L method.
         *          
         * 
         * @param aSmsListArray SMS message list storage array
         * @param aReadAllId Read all SMS messages Id
         * @param aSmsReadAllArray Read all SMS messages array
         * @param aReadAllBufSizePtr Read all SMS messages pointer
         * @return KErrNotSupported
         */
        virtual TInt CompleteReadAllSmsPhase1L( 
            CArrayPtrFlat<TSmsMsg>* aSmsListArray,
            RMobilePhone::TClientId aReadAllId,
            CArrayPtrFlat<CListReadAllAttempt>* aSmsReadAllArray,
            TInt* aReadAllBufSizePtr );

        /**
         * Default implementation of GetSupportedEntry method
         *          
         * 
         * @return KErrNotSupported
         */
        virtual TInt GetSupportedEntry();

    protected:
        
        /**
         * C++ default constructor.
         */
        CMmSmsExtInterface();

	};

#endif // CMMSMSEXTINTERFACE_H

//  End of File
