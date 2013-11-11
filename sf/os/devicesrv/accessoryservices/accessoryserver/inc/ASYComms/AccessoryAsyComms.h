/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef RACCESSORYASYCOMMS_H
#define RACCESSORYASYCOMMS_H

// INCLUDES

#include "AccClientServerMessages.h"
#include "AccSrvProxy.h"
#include <AccessoryTypes.h>
#include <AccessorySubsessionBase.h>
#include <AccessoryServer.h>
#include <AsyCmdTypes.h>
#include <e32std.h>
#include <accpolobjectbase.h>

// CONSTANTS

// MACROS

// DATA TYPES
typedef TPckg<TProcessCmdId> TAccSrvProcessCmdIdPckg;

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  AsyComms sub-session represents AsyComms connection
*  Provides methods for NotifyProcessCommands and ProcessResponses
*
*  @lib ASYComms.dll
*  @since S60 3.1
*/
class RAccessoryAsyComms : public RAccessorySubsessionBase
    {
    public: // Constructors and destructor

        /**
        * C++ default constructor.
        */
        IMPORT_C RAccessoryAsyComms();

    public: // New functions
        
        /**
        * Creates a new sub-session within an existing session.
        * @since S60 3.1
        * @param aSession The session to which this sub-session will belong.
        * @param aMainServiceUid Implementation uid of main service ASY
        * @return KErrNone if successful, system-wide error code if failed.
        */
        IMPORT_C TInt CreateSubSession( RAccessoryServer& aSession,
                                        const TUid aMainServiceUid );

        /**
        * Closes the RAccessoryAsyComms sub-session.
        * This function must be called for all RAccessoryAsyComms
        * sub-sessions which have been created 
        * @since S60 3.1
        * @return KErrNone if successful, system-wide error code if failed.
        * @see CreateSubSession()
        */
        IMPORT_C TInt CloseSubSession();
        
        /**
        * Process command request.
        * Actual request takes place when this asynchronous request completes
        * @since S60 3.1
        * @param aStatus Indicates the completion status of a request.
        *                KErrNone if successful, system-wide error code if failed.
        *                Specifically:
        *                KErrAlreadyExists, if request allready exist
        *                KErrCancel, if request is cancelled
        * @param aCmdn   On successful request completion, contains the updated
        *                ProcessCommand ID
        * @param aTrId   On successful request completion, contains the updated
                         Transaction ID
        * @param aASYCommandParamRecord On successful request completion contains 
        *                the updated parameters.
        * @return void
        * @see The AsyCmdTypes.h, where TASYCmdParams is declared.
        */
        IMPORT_C void NotifyProcessCommand(
                        TRequestStatus& aStatus,
                        TProcessCmdId& aCmdn,
                        TInt& aTrId,
                        TASYCmdParams& aASYCommandParamRecord );

        /**
        * Process command request.
        * Actual request takes place when this asynchronous request completes
        *
        * @since S60 3.1
        * @param aStatus Indicates the completion status of a request.
        *                KErrNone if successful, system-wide error code if failed.
        *                Specifically:
        *                KErrAlreadyExists, if request allready exist
        *                KErrCancel, if request is cancelled
        * @param aPtr    Pointer to buffer where CAccPolObjectBase type of object
        *                is written to in externalized format.
        *                ProcessCommand ID, Transaction ID and ASYCommandParamRecord
        *                parameters.
        * @return void
        * @see The AsyCmdTypes.h, where TASYCmdParams is declared.
        */
        IMPORT_C void NotifyProcessCommand( TRequestStatus& aStatus,
                                            TProxyCommandPckgBuf& aParams,
                                            TPtr8& aPtr );
                                            
        /**
        * Cancel NotifyProcessCommand().
        * @since S60 3.1
        * @param void
        * @return void
        */
        IMPORT_C void CancelNotifyProcessCommand();

        /**
        * Handles responses get from ASYproxy for previously
        * made NotifyProcessCommands() -call.
        * ASYProxy calls this function if TProcessCmdId was
        * ECmdProcessCommandInit or ECmdGetValueBool
        * @since S60 3.1
        * @param aTrid Transaction id
        * @param aCmdResponse Response data
        * @param aErrCode KErrNone if succesful, system-wide error code if failed.
        * @return void
        */
        IMPORT_C void ProcessResponse( const TInt aTrid,
                                       const TAccValueTypeTBool aCmdResponse,
                                       const TInt aErrCode );

        /**
        * Handles responses get from ASYproxy for previously
        * made NotifyProcessCommands() -call.
        * ASYProxy calls this function if TProcessCmdId was
        * ECmdGetValueTInt
        * @since S60 3.1
        * @param aTrid Transaction id
        * @param aCmdResponse Response data
        * @param aErrCode KErrNone if succesful, system-wide error code if failed.
        * @return void
        */
        IMPORT_C void ProcessResponse( const TInt aTrid,
                                       const TAccValueTypeTInt aCmdResponse,
                                       const TInt aErrCode );

        /**
        * Handles responses get from ASYproxy for previously
        * made NotifyProcessCommands() -call.
        * ASYProxy calls this function if TProcessCmdId was
        * ECmdGetValueTDes8 or ECmdGetSupportedBTProfiles
        * @since S60 3.1
        * @param aTrid Transaction id
        * @param aCmdResponse Response data
        * @param aErrCode KErrNone if succesful, system-wide error code if failed.
        * @return void
        */
        IMPORT_C void ProcessResponse( const TInt aTrid,
                                       const TDesC8& aCmdResponse,
                                       const TInt aErrCode );

        /**
        * Handles responses get from ASYproxy for previously
        * made NotifyProcessCommands() -call.
        * ASYProxy calls this function if TProcessCmdId was
        * ECmdGetObjectValue        
        * @param aTrid Transaction id
        * @param aPtr Response data
        * @param aErrCode KErrNone if succesful, system-wide error code if failed.
        * @return void
        */
        IMPORT_C void ProcessResponse( const TInt aTrid,
                                       TPtr8* aPtr,
                                       const TInt aErrCode );
                                                                       
        /**
        * Read the CAccPolObjectBase buffer.
        *        
        * @param aPtr Pointer to buffer where CAccPolObjectBase type of object
        *             is written to in externalized format.
        * @return void
        */                           
        IMPORT_C void GetBuffer( TPtr8 aPtr );

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * Externalize aValue to aBuf.
        */
        void ExternalizeL( CBufFlat*& aBuf, CAccPolObjectBase& aValue );


        /**
        * Definition for pure virtual function defined in base class.
        * Declaration is empty.
        */
        IMPORT_C TInt CreateSubSession( RAccessoryServer& aSession );

    public:     // Data

    protected:  // Data

    private:    // Data

        // Points to memory allocated by the client.
        // Used to update ProcessCommand ID from server process
        TAccSrvProcessCmdIdPckg iProcessCmdIdPckg;

        // Points to memory allocated by the client.
        // Used to update Transaction ID from server process
        TPckg<TInt>             iTransactionIDPckg;
        
    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // RACCESSORYASYCOMMS_H

// End of File
