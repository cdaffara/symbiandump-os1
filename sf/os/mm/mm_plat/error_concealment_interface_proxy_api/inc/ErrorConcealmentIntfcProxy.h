/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface proxy for error concealment.
*
*/


#ifndef ERRORCONCEALMENTINTFCPROXY_H
#define ERRORCONCEALMENTINTFCPROXY_H

// INCLUDES
#include <ErrorConcealmentIntfc.h>
#include <mmfcontrollerframework.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CCustomInterfaceUtility;
class MCustomCommand;

// CLASS DECLARATION

/**
*  Proxy for error concealment interface.
*  This proxy translates the interface API calls
*  to messages and sends them to the message handler.
*
*  @lib ErrorConcealmentIntfcProxy.lib
*  @since S60 3.0
*/
class CErrorConcealmentIntfcProxy: public CErrorConcealmentIntfc
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CErrorConcealmentIntfcProxy* NewL(
		                    TMMFMessageDestinationPckg aMessageHandler,
                            MCustomCommand& aCustomCommand,
                            CCustomInterfaceUtility* aCustomInterfaceUtility);
		
        /**
        * Destructor.
        */
		IMPORT_C virtual ~CErrorConcealmentIntfcProxy();

    public: // New functions
        
    public: // Functions from base classes

        /**
        * From CErrorConcealmentIntfc
        */
		IMPORT_C virtual TInt ConcealErrorForNextBuffer();

        /**
        * From CErrorConcealmentIntfc
        */
		IMPORT_C virtual TInt SetFrameMode(TBool aFrameMode);

        /**
        * From CErrorConcealmentIntfc
        */
		IMPORT_C virtual TInt FrameModeRqrdForEC(TBool& aFrameModeRqrd);
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
		CErrorConcealmentIntfcProxy(
		                    TMMFMessageDestinationPckg aMessageHandler, 
                            MCustomCommand& aCustomCommand,
                            CCustomInterfaceUtility* aCustomInterfaceUtility);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();

    public:     // Data
    
    protected:  // Data

    private:    // Data
        // handle to object used for sending IPC commands
		MCustomCommand&                			iCustomCommand;
		// message object destined for the message handler
		TMMFMessageDestinationPckg            	iMessageHandler;
		// handle to utility object used in interface framework
		CCustomInterfaceUtility* 				iCustomInterfaceUtility;
         
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

	};

#endif      // ERRORCONCEALMENTINTFCPROXY_H
            
// End of File
