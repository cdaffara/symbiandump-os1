/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Custom Interface proxy for RA8 decoder.
*
*/


#ifndef RA8CUSTOMINTERFACEPROXY_H
#define RA8CUSTOMINTERFACEPROXY_H

// INCLUDES
#include <Ra8CustomInterface.h>
#include <mmfcontrollerframework.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CCustomInterfaceUtility;
class MCustomCommand;
class CMMFDevSound;

// CLASS DECLARATION

/**
*  Proxy for Ra8 decoder interface.
*  This proxy translates the interface API calls
*  to messages and sends them to the message handler.
*
*  @lib Ra8CustomInterfaceProxy.lib
*  @since Series 60 3.0
*/
class CRa8CustomInterfaceProxy : public CRa8CustomInterface
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CRa8CustomInterfaceProxy* NewL(
                            TMMFMessageDestinationPckg aMessageHandler,
                            MCustomCommand& aCustomCommand,
                            CCustomInterfaceUtility* aCustomInterfaceUtility);
		
        /**
        * Destructor.
        */
		IMPORT_C virtual ~CRa8CustomInterfaceProxy();

    public: // New functions
        
    public: // Functions from base classes

        /**
        * From CRa8CustomInterface
        */
        IMPORT_C CRa8CustomInterfaceProxy* NewL(CMMFDevSound& aDevSound);
		IMPORT_C virtual TInt FrameNumber();
		IMPORT_C virtual TInt SetInitString(TDesC8& aInitString);

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
		CRa8CustomInterfaceProxy(
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
		MCustomCommand&             iCustomCommand;
		// message object destined for the message handler
		TMMFMessageDestinationPckg  iMessageHandler;
		// handle to utility object used in interface framework
		CCustomInterfaceUtility*    iCustomInterfaceUtility;
         
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

	};

#endif      // RA8CUSTOMINTERFACEPROXY_H
            
// End of File
