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
* Description:  Subconnection class for session of ASYComms
*
*/


#ifndef CACCSRVSUBASYCOMMS_H
#define CACCSRVSUBASYCOMMS_H

// INCLUDES
#include "AccSrvProcessCommandTimer.h"
#include "AccSrvProcessCommandObserver.h"
#include "AsyProxyAPI.h"
#include "AccSrvSubBase.h"
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES
enum TProcessResponseType
    {
    EProcessResponseTBool,
    EProcessResponseTInt,
    EProcessResponseTDes,
    EProcessResponseObject
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccSrvMainSession;

// CLASS DECLARATION
/**
*  Subconnection class for session of ASYComms
*
*  @lib AccServer.exe
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvSubAsyComms ) : public CAccSrvSubBase, 
                                          public MAccSrvProcessCommandObserver
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since S60 3.1
        * @param aProxy Pointer to an ASYProxy
        * @param aConnectionController Pointer to a connection controller object
        * @param aServerModel Pointer to a server model object
        * @param aASYThreadID id of the ASY thread
        * @param aMainServiceUid Uid of the main service ASY
        * @return Pointer to a new subsession
        */
        static CAccSrvSubAsyComms* NewL(
                         CASYProxyAPI* aProxy,
                         CAccSrvConnectionController* aConnectionController,
                         CAccSrvServerModel* aServerModel,
                         TUint aASYThreadID,
                         TUid aMainServiceUid );

        /**
        * Destructor.
        */
        virtual ~CAccSrvSubAsyComms();

    public: // New functions

    public: // Functions from base classes
    
        /**
        * From CAccSrvSubBase
        * Handles received messages from client.
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void DispatchMessageL( const RMessage2& aMessage );

        /**
        * From CAccSrvSubBase Called to handle message completion handling.
        * @since S60 3.1
        * @param aMsgID Message id
        * @param aErrorCode Error Code of the operation.
        * @param aUniqueID Unique ID of the Generic ID in question.
        * @return ETrue if the message was consumpted by a observer.
        */
        TBool CompleteMessageL( TMsgID aMsgID,
                                TInt aErrorCode,
                                TInt aUniqueID );
                                               
        /**
        * From CAccSrvSubBase Called to handle message completion handling.
        * @since S60 3.1
        * @param aCmdn Process command id
        * @param aTrId Transaction id
        * @param aASYCommandParamRecord Data for NotifyProcessCommand() completion.
        * @param aPtrBuf Pointer to CBufFlat buffer where CAccPolObjectCon object
        *                is externalized to.
        * @return ETrue if the message was consumpted by a observer.
        */
        virtual TBool CompleteProcessCommandL(
                         TProcessCmdId aCmdn,
                         TInt aTrId,
                         TASYCommandParamRecord& aASYCommandParamRecord,
                         TPtr8* aPtrBuf=NULL );

    protected: // New functions
    
        /**
        * C++ default constructor.
        */
        CAccSrvSubAsyComms(
                CASYProxyAPI* aSession,
                CAccSrvConnectionController* aConnectionController,
                CAccSrvServerModel* aServerModel,
                TUint aASYThreadID,
                TUid aMainServiceUid );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected: // Functions from base classes
        
    private:

        /**
        * Handles NotifyProcessCommand() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void NotifyProcessCommand( const RMessage2& aMessage );

        /**
        * Handles CancelNotifyProcessCommand() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelNotifyProcessCommand( const RMessage2& aMessage );

        /**
        * Handles ProcessResponse() TBool, TInt and TDesC8 client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @param aResponseType Type of the response, TBool, TInt or TDesC8
        * @param aTimeOut Indicates has request timed out
        * @return void
        */
        void ProcessResponseL( const RMessage2& aMessage,
                               TProcessResponseType aResponseType,
                               TBool aTimeOut = EFalse );   

        TBool HandleCommandL( TProcessCmdId aCmdn,
                              TInt aTrId,
                              TASYCommandParamRecord& aASYCommandParamRecord,
                              TPtr8* aPtrBuf );
                              
        TInt HandleObjectBufferWriteL( TPtr8* aPtrBuf );

        virtual void CompleteProcessCommandInitL();
        virtual void ProcessCommandTimeOutCompleteL();

    public:     // Data

    protected:  // Data

    private:    // Data

        // Stores RAccessoryAsyComms::NotifyProcessCommand() request
        RMessage2 iMessage;

        //Transaction id for outstanding SetValueL() or GetValueL
        TInt iOutstandingTrId;

				//Timeout transaction id. 
        TInt iTimeOutTrId;
        
        //Process command ID for outstanding SetValueL() or GetValueL
        TProcessCmdId iOutstandingProcessCmdId;

        // Pointer to timer, that handles time out during
        // Init ProcessCommand
        CAccSrvProcessCommandTimer* iInitCommandTimer; // Owned
        
        // Pointer to timer, that handles time out during
        // ProcessCommands
        CAccSrvProcessCommandTimer* iProcessCommandTimer; // Owned

        //This sub-session is created by ASY from iASYThreadID thread
        TUint   iASYThreadID;
        TUid    iMainServiceUid;
        
        // CAccPolObjectBase object is externalized to this buffer.
        // This is done when ASY Proxy buffer size is not enough.
        CBufFlat* iObjectBuf; // Owned

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CACCSRVSUBASYCOMMS_H

// End of File
