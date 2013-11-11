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
* Description:  Pure virtual command handler base class for ASY
*
*/


#ifndef CASYCOMMANDHANDLERBASE_H
#define CASYCOMMANDHANDLERBASE_H

//  INCLUDES
#include <AsyCmdTypes.h>
#include <e32std.h>
#include <e32base.h>
#include <ecom/ecom.h>
#include <AccessoryTypes.h>
#include <AsyMainServiceBase.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccPolObjectCon;

// CLASS DECLARATION
class CASYProxyCommandObserver;

/**
*  Pure virtual base class for ASYs, from where ASY are meant to be inherited.
*  ASYProxy creates the instances.
*
*  @lib ASYProxy.lib
*  @since S60 3.1
*/
class CASYCommandHandlerBase : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        // Calls RECOM AsyImplementation
        static CASYCommandHandlerBase* NewL(
               CASYMainServiceBase* aAsyMainServiceBase,
               CASYProxyCommandObserver* aASYProxyCommandObserver,
               TUid iImplementationUidAsyHandler );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CASYCommandHandlerBase();

    public: // New functions

        /**
        * Processes request command from server. 
        * @since S60 3.1
        * @param aCommand Command to process.
        * @param aCmdParams Params of command to process.
        * @return void
        */
        IMPORT_C virtual void ProcessCommandL( const TProcessCmdId aCommand,
                                               const TASYCmdParams& aCmdParams ) = 0;
                                                                                                                               
    public: // Functions from base classes

    protected:  // New functions

        /**
        * ASY calls this after handling ProcessCommandL.
        * Method is not meant to be override.
        * @since S60 3.1
        * @param aCmdResponse Response value
        * @param aErrCode Error code for command processing
        * @return void
        */
        IMPORT_C void ProcessResponseL( const TAccValueTypeTBool aCmdResponse,
                                        const TInt aErrCode = KErrNone );

        /**
        * ASY calls this after handling ProcessCommandL.
        * Method is not meant to be override.
        * @since S60 3.1
        * @param aCmdResponse Response value
        * @param aErrCode Error code for command processing
        * @return void
        */
        IMPORT_C void ProcessResponseL( const TAccValueTypeTInt aCmdResponse,
                                        const TInt aErrCode = KErrNone );

        /**
        * ASY calls this after handling ProcessCommandL.
        * Method is not meant to be override.
        * @since S60 3.1
        * @param aCmdResponse Response value
        * @param aErrCode Error code for command processing
        * @return void
        */
        IMPORT_C void ProcessResponseL( const TDesC8& aCmdResponse,
                                        const TInt aErrCode = KErrNone );
                                                                                                                                
        /**
        * ASY calls this after handling a "ProcessCommandL" with params:
        * ECmdGetObjectValue,
        * ECmdSetObjectValue
        *
        * Method is not meant to be override.        
        * @param aObjectCon Contains an object(s) which is/are externalized 
        * to ASY proxy buffer.
        * @param aErrCode Error code for command processing
        * @return void
        */
        IMPORT_C void ProcessResponseL( const CAccPolObjectCon& aObjectCon,
                                        const TInt aErrCode = KErrNone );
                                                                                                                                                                        
        /**
        * ASY calls this to fetch a data to the object container.
        *                
        * Method is not meant to be override.        
        * @param aObjectCon reference to the object container where 
        * to internalize a data from ASY proxy buffer.
        * @return void
        */
        IMPORT_C void ObjectConDataFromProxyL( CAccPolObjectCon& aObjectCon ) const;

        /**
        * ASYCommandHandler calls this to get a pointer to
        *  a CASYMainServiceBase object
        * @since S60 3.1
        * @param void
        * @return Pointer to a CASYMainServiceBase object
        */
        IMPORT_C CASYMainServiceBase* ASYMainServiceBase();

        /**
        * C++ default constructor.
        */
        IMPORT_C CASYCommandHandlerBase();

    protected:  // Functions from base classes

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(
             CASYMainServiceBase* aAsyMainServiceBase,
             CASYProxyCommandObserver* aASYProxyCommandObserver );

    public:     // Data

    protected:  // Data

    private:    // Data

        // Pointer to a ASYMainService
        CASYMainServiceBase* iAsyMainServiceBase; // Not owned

        // Pointer of an Observer class,
        //  which one passes process responses to the client
        CASYProxyCommandObserver* iASYProxyCommandObserver; // Not owned

        // Identifier of this instance
        TUid iDtor_ID_Key;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CASYCOMMANDHANDLERBASE_H

// End of File
