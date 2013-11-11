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
* Description:  Handler for NotifyProcessCommands and responses
*
*/


#ifndef CASYPROXYCOMMANDOBSERVER_H
#define CASYPROXYCOMMANDOBSERVER_H

// INCLUDES
#include "AccessoryServer.h"
#include <AsyCommandHandlerBase.h>
#include <e32base.h>

// CONSTANTS
const TInt KDeleteAllRecords = 0;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CASYProxyCommsService;
class CASYProxyTRRecord;
class CAccPolObjectCon;

// CLASS DECLARATION

/**
*  Observer class for processing current response
*  One observer object for every command is needed
*
*  @lib ASYProxy.dll
*  @since S60 3.1
*/
class CASYProxyCommandObserver : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CASYProxyCommandObserver* NewL( CASYProxyCommsService *iAsyProxyCommsSrvc );

        /**
        * Two-phased constructor.
        */
        static CASYProxyCommandObserver* NewL();

        /**
        * Destructor.
        */
        virtual ~CASYProxyCommandObserver();

    public: // New functions

        /**
        * Issues asynchronously request for notifying process command to client
        * @since S60 3.1
        * @param void
        * @return void
        */
        void IssueRequestL();

        /**
        * Sets the pointer of iAsyProxyTRRecord.
        * @since S60 3.1
        * @param aAsyProxyTRRecord Pointer to the current transaction record
        * @return void
        */
        void InitializeRef( CASYProxyTRRecord* aAsyProxyTRRecord );

       /**
        * Process responses.
        * @since S60 3.1
        * @param aCmdResponse Response value
        * @param aErrCode Error code of the response
        * @return void
        */
        void DoProcessResponseL( const TAccValueTypeTBool aCmdResponse, const TInt aErrCode );

        /**
        * Process responses.
        * @since S60 3.1
        * @param aCmdResponse Response value
        * @param aErrCode Error code of the response
        * @return void
        */
        void DoProcessResponseL( const TAccValueTypeTInt aCmdResponse, const TInt aErrCode );

        /**
        * Process responses.
        * @since S60 3.1
        * @param aCmdResponse Response value
        * @param aErrCode Error code of the response
        * @return void
        */
        void DoProcessResponseL( const TDesC8& aCmdResponse, const TInt aErrCode );

        /**
        * Process responses.
        * @param aObject Response value
        * @param aErrCode Error code of the response
        * @return void
        */
        void DoProcessResponseL( const CAccPolObjectCon& aObjectCon, const TInt aErrCode );         
                
        /**
        * Gets an object container data from proxy.
        * @param aObjectCon reference to objeccontainer. 
        * @return void
        */
        void DoObjectContainerFromProxyL( CAccPolObjectCon& aObjectCon ) const;
        
        /**
        * Marks for final destruction, that avoids creation of new observer.
        * @since S60 3.1
        * @param void
        * @return void
        */
        void MarkForFinalDestruction ();
        
    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

        /**
        * From CActive Implements cancellation of an outstanding request.
        */
        void DoCancel();

        /**
        * From CActive Handles an active object's request completion event.
        */
        void RunL();

        /**
        * Runs if the RunL leaves 
        */
        TInt RunError( TInt aError );

    private:

        /**
        * C++ default constructor.
        */
        CASYProxyCommandObserver();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CASYProxyCommsService *iAsyProxyCommsSrvc );
        void ConstructL();

        void AllowResponseL();

        void DoDestroy( TInt aTrId );

    public:     // Data

    protected:  // Data

    private:    // Data

        // Pointer to an object of responsible to handle responses
        CASYProxyCommsService* iAsyProxyCommsSrvc; // Not owned

        // Pointer to an object to keep up current command data of process
        CASYProxyTRRecord* iAsyProxyTRRecord; // Not owned

        // For checking that only one response is accepted per command.
        TInt iAllowResponse;

        //CAccPolObjectBase object is externalized to this buffer.
        CBufFlat* iObjectBuf; // Owned
        
        // Pointer to iObjectBuf
        TPtr8     iObjectBufPtr;
        
        // If ProxyCommandObserver is in Destruction, Avoid Creating a new ProxyCommandObserver.
        TBool   iInDestructionPhase;
        
    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
};

#endif // CASYPROXYCOMMANDOBSERVER_H

// End of File
