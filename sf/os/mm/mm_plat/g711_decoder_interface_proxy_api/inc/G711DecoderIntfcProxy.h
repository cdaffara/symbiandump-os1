/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface proxy for G711 decoder.
*
*/


#ifndef G711DECODERINTFCPROXY_H
#define G711DECODERINTFCPROXY_H

// INCLUDES
#include <G711DecoderIntfc.h>
#include <mmf/common/mmfcontrollerframework.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CCustomInterfaceUtility;
class MCustomCommand;

// CLASS DECLARATION

/**
*  Proxy for G711 decoder interface.
*  This proxy translates the interface API calls
*  to messages and sends them to the message handler.
*
*  @lib G711DecoderIntfcProxy.lib
*  @since S60 3.0
*/
class CG711DecoderIntfcProxy: public CG711DecoderIntfc
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CG711DecoderIntfcProxy* NewL(
		                    TMMFMessageDestinationPckg aMessageHandler,
                            MCustomCommand& aCustomCommand,
                            CCustomInterfaceUtility* aCustomInterfaceUtility);
		
        /**
        * Destructor.
        */
		IMPORT_C virtual ~CG711DecoderIntfcProxy();

    public: // New functions
        
    public: // Functions from base classes

        /**
        * From CG711DecoderIntfc
        */
		IMPORT_C virtual TInt SetDecoderMode(TDecodeMode aDecodeMode);

        /**
        * From CG711DecoderIntfc
        */
		IMPORT_C virtual TInt SetCng(TBool aCng);

        /**
        * From CG711DecoderIntfc
        */
		IMPORT_C virtual TInt GetCng(TBool& aCng);

        /**
        * From CG711DecoderIntfc
        */
		IMPORT_C virtual TInt SetPlc(TBool aPlc);

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
		CG711DecoderIntfcProxy(
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

#endif      // G711DECODERINTFCPROXY_H
            
// End of File
