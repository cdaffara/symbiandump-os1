/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Mediator logging services
*
*/

#ifndef MEDIATORDEBUG_H
#define MEDIATORDEBUG_H

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include <flogger.h>
#include "MediatorServiceDefs.h"
#include "MediatorCommon.h"
#include "MediatorDebug.hrh"

// CONSTANTS
const TInt KMediatorDebugBufferLength = 512;

// FORWARD DECLARATIONS
class CMediatorServer;
class CMediatorServerSession;

using namespace MediatorService;

// CLASS DECLARATION

/**
*  Mediator logging services
*  
*
*  @lib MediatorServer.exe
*  @since S60 3.1
*/
class RMediatorDebug
    {
    public:  
        
        /**
        * Mediator logging service initialization
        * @since S60 3.1
        * @param aMediator mediator server
        * @return void
        */
        inline static void Initialize(CMediatorServer* aMediator);
        
        /**
        * Mediator logging service uninitialization
        * @since S60 3.1
        * @return void
        */
        inline static void Uninitialize();
        
        /**
        * Prints debug information of the operation 
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @return void
        */
        inline static void Log(const RMessage2& aMessage, CMediatorServerSession& aSession);
        
        /**
        * Prints event list
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @param aEventList event list
        * @return void
        */
        inline static void LogData(const RMessage2& aMessage, CMediatorServerSession& aSession, const MediatorService::REventList& aEventList);
        
        /**
        * Prints a single event
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @param aEvent event
        * @return void
        */
        inline static void LogData(const RMessage2& aMessage, CMediatorServerSession& aSession, const MediatorService::TEvent& aEvent);
        
        /**
        * Prints command list
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @param aCommandList command list
        * @return void
        */
        inline static void LogData(const RMessage2& aMessage, CMediatorServerSession& aSession, const MediatorService::RCommandList& aCommandList);
        
        /**
        * Prints a single command
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @param aCommand command
        * @return void
        */
        inline static void LogData(const RMessage2& aMessage, CMediatorServerSession& aSession, const MediatorService::TCommand& aCommand);
        
        /**
        * Prints a list of UIDs, depending on context either list of domains or categories
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @param aCategoryList category list
        * @return void
        */
        inline static void LogData(const RMessage2& aMessage, CMediatorServerSession& aSession, const MediatorService::RCategoryList& aCategoryList);
        
        /**
        * Prints raw parameter data
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @param aData data
        * @return void
        */
        inline static void LogData(const RMessage2& aMessage, CMediatorServerSession& aSession, const TDesC8& aData);
        
        /**
        * Prints status report for mediator
        * @since S60 3.1
        * @return void
        */
        inline static void LogStatus();                
        
        /**
        * Prints the error information of the failed operation
        * @since S60 3.1
        * @param aMessage message
        * @param aError error identifier
        * @return void
        */
        inline static void LogError(const RMessage2& aMessage, TInt aError);
        
        /**
        * Prints implementation information of the plugin
        * @since S60 3.1
        * @param aImpl plugin implementation
        * @return void
        */
        inline static void LogPlugin(const CImplementationInformation* aImpl);
                        
        /**
        * C++ default destructor.
        */
        ~RMediatorDebug();
         
    private:

        /**
        * C++ default constructor.
        */
        RMediatorDebug();
        
        /**
        * Two-phased constructor.
        */
        static RMediatorDebug* NewL(CMediatorServer* aMediator);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CMediatorServer* aMediator);
        
        /**
        * Checks whether the log for the message should be printed.
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @return TBool ETrue if log should be printed, otherwise EFalse.
        */
        TBool ShouldPrintLogL(const RMessage2& aMessage, CMediatorServerSession& aSession);
        
        /**
        * Prints debug information of the operation 
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @return void
        */
        void PrintOperationL(const RMessage2& aMessage, CMediatorServerSession& aSession);
        
        /**
        * Prints the error information of the failed operation
        * @since S60 3.1
        * @param aMessage message
        * @param aError error identifier
        * @return void
        */
        void PrintErrorL(const RMessage2& aMessage, TInt aError);
        
        /**
        * Prints status report for mediator
        * @since S60 3.1
        * @return void
        */                    
        void PrintServerStatusL();
        
        /**
        * Prints raw parameter data
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @param aData data
        * @return void
        */
        void PrintRawL(const RMessage2& aMessage, CMediatorServerSession& aSession, const TDesC8& aData);
        
        /**
        * Prints a single event
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @param aEvent event
        * @return void
        */
        void PrintEventL(const RMessage2& aMessage, CMediatorServerSession& aSession, const MediatorService::TEvent& aEvent, TInt aEventNmbr);
        
        /**
        * Prints event list
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @param aEventList event list
        * @return void
        */
        void PrintEventListL(const RMessage2& aMessage, CMediatorServerSession& aSession, const MediatorService::REventList& aList);
        
        /**
        * Prints a single command
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @param aCommand command
        * @return void
        */
        void PrintCommandL(const RMessage2& aMessage, CMediatorServerSession& aSession, const MediatorService::TCommand& aCmd, TInt aCmdNmbr);
        
        /**
        * Prints command list
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @param aCommandList command list
        * @return void
        */
        void PrintCommandListL(const RMessage2& aMessage, CMediatorServerSession& aSession, const MediatorService::RCommandList& aList);
        
        /**
        * Prints a list of UIDs, depending on context either list of domains or categories
        * @since S60 3.1
        * @param aMessage message
        * @param aSession session
        * @param aCategoryList category list
        * @return void
        */
        void PrintCategoryListL(const RMessage2& aMessage, CMediatorServerSession& aSession, const MediatorService::RCategoryList& aList);
        
        /**
        * Prints implementation information of the plugin
        * @since S60 3.1
        * @param aImpl plugin implementation
        * @return void
        */
        void PrintPluginL(const CImplementationInformation* aImpl);
        
        /**
        * Prints the data stored in internal write buffer.
        * @since S60 3.1
        * @param None
        * @return void
        */
        void PrintL();
        
        /**
        * Prints the text passed as parameter. Depending on configuration,
        * the print target can console and/or file.
        * @since S60 3.1
        * @param aText  text to be printed
        * @return void
        */
        void Print(const TDesC& aText);
        
        /**
        * Prints the current configuration of logging service.
        * 
        * @since S60 3.1
        * @param None.
        * @return None.
        */
        void RMediatorDebug::PrintConfigurationL();
        
        /**
        * Handles internal error (usually leave due to buffer overflow),
        * if one occurs
        * @since S60 3.1
        * @param aError error code
        * @return void
        */
        void HandleInternalError(TInt aError);
        
        /**
        * Returns whether operation has a category.
        * @since S60 3.1
        * @param aMessage operation message
        * @return ETrue if category exists for this operation.
        */
        TBool HasCategory(const RMessage2& aMessage);
        
        /**
        * Returns whether operation has a domain.
        * @since S60 3.1
        * @param aMessage operation message
        * @return ETrue if domain exists for this operation.
        */
        TBool HasDomain(const RMessage2& aMessage);   
        
        /**
        * Returns string presentation of an operation.
        * @since S60 3.1
        * @param aOperation operation code
        * @return TPtrC string descriptor containing operation's name.
        */
        TPtrC OperationString(TInt aOperation);
        
        /**
        * Returns string presentation of an error code.
        * @since S60 3.1
        * @param aError error code
        * @return TPtrC string descriptor containing error's name.
        */
        TPtrC ErrorString(TInt aError);
        
        /**
        * Returns abbreviated (2-3 letters) string presentation of a capability.
        * @since S60 3.1
        * @param aCapability capability
        * @return TPtrC string descriptor containing abbreviated capability name.
        */
        TPtrC CapabilityString( TCapability aCapability );
        
        /**
        * Appends list of capability names specifiec in capability set
        * into internal print buffer.
        * @since S60 3.1
        * @param aCaps capability set
        * @return None.
        */
        void AppendCapabilitiesL( const TCapabilitySet& aCaps );
        
        /**
        * Handles initialization error
        * @since S60 3.1
        */
        static void HandleInitError( TInt aError );
        
        /**
        *  Overflow handler for buffer overlow checking.
        *  
        *
        *  @lib MediatorServer.exe
        *  @since S60 3.1
        */
        class TDebugOverflow : public TDesOverflow
            {
            /**
            * From TDesOverflow.
            * Handles overflow situation by resetting the buffer and
            * raising a leave.
            * @since S60 3.1
            * @param aBufferThatOverflowed buffer that caused overflow
            * @return TPtrC string descriptor containing error's name.
            */
            void Overflow(TDes& aBufferThatOverflowed)
                {
                aBufferThatOverflowed.Zero(); // reset buffer for new usage
                
                /*
                This non-leaving method can leave because implementing this oveflow
                check functionality would lead to more bloated and error-prone code.
                This method will be only called from functions that can leave and the leaves
                are trapped in the highest abstraction level of logging services.
                */
                User::Leave(KErrOverflow);
                }
            };
                
    private:    // Data
        
        /**
        * mediator server
        * Not own.
        */
        CMediatorServer* iMediator;
        
        /**
        * filtering options for domains
        */
        EDebugLogMode iDomainOptions;
        
        /**
        * filtered domains
        */
        RArray<TUid> iDomains;
        
        /**
        * filtering options for categories
        */
        EDebugLogMode iCategoryOptions;
        
        /**
        * filtered categories
        */
        RArray<TUid> iCategories;
        
        /**
        * general logging options
        */
        TInt iOptions;
        
        /**
        * print buffer
        */
        TBuf<KMediatorDebugBufferLength> iBuf;
        
        /**
        * overflow handler for internal buffers
        */
        TDebugOverflow iOf;
                
        /**
        * The only instance of this class
        * Own.
        */
        static RMediatorDebug* iInstance;
        
        /**
        * File logger.
        */
        RFileLogger iFileLogger;
         
    };

#include "MediatorDebug.inl"

#endif      // MEDIADOR_DEBUG_H
           
//  End of File