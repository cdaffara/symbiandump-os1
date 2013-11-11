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
* Description:  Interface proxy for Ilbc decoder.
*
*/


#ifndef ILBCDECODERINTFCPROXY_H
#define ILBCDECODERINTFCPROXY_H

// INCLUDES
#include <IlbcDecoderIntfc.h>
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
*  Proxy for Ilbc decoder interface.
*  This proxy translates the interface API calls
*  to messages and sends them to the message handler.
*
*  @lib IlbcDecoderIntfcProxy.lib
*  @since S60 3.0
*/
class CIlbcDecoderIntfcProxy: public CIlbcDecoderIntfc
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CIlbcDecoderIntfcProxy* NewL(
		                    TMMFMessageDestinationPckg aMessageHandler,
                            MCustomCommand& aCustomCommand,
                            CCustomInterfaceUtility* aCustomInterfaceUtility);
		
        /**
        * Destructor.
        */
		IMPORT_C virtual ~CIlbcDecoderIntfcProxy();

    public: // New functions
        
    public: // Functions from base classes

        /**
        * From CIlbcDecoderIntfc
        */
		IMPORT_C virtual TInt SetDecoderMode(TDecodeMode aDecodeMode);

        /**
        * From CIlbcDecoderIntfc
        */
		IMPORT_C virtual TInt SetCng(TBool aCng);

        /**
        * From CIlbcDecoderIntfc
        */
		IMPORT_C virtual TInt GetCng(TBool& aCng);


    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
		CIlbcDecoderIntfcProxy(
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

#endif      // ILBCDECODERINTFCPROXY_H
            
// End of File
