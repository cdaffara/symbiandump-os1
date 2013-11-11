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
* Description:  Message handler for G711 decoder interface.
*
*/


#ifndef CG711DECODERINTFCMSGHDLR_H
#define CG711DECODERINTFCMSGHDLR_H

// INCLUDES
#include <mmfcontroller.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CG711DecoderIntfc;

// CLASS DECLARATION

/**
*  Handle messages from interface proxy.
*  The class receives the messages sent by the custom interface proxy
*  and makes the appropriate call on its custom interface implementation.
*
*  @lib G711DecoderIntfcMsgHdlr.lib
*  @since S60 3.0
*/
class CG711DecoderIntfcMsgHdlr: public CMMFObject
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CG711DecoderIntfcMsgHdlr* NewL(
                                           TAny* aG711DecoderIntfcImpl);
        
        /**
        * Destructor.
        */
		IMPORT_C virtual ~CG711DecoderIntfcMsgHdlr();

    public: // New functions
        
    public: // Functions from base classes
		IMPORT_C virtual void HandleRequest(TMMFMessage& aMessage);        
		
    protected:  // New functions
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
		CG711DecoderIntfcMsgHdlr(
		                  CG711DecoderIntfc* aG711DecoderIntfcImpl);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		void DoHandleRequestL(TMMFMessage& aMessage);

		void DoSetDecoderModeL(TMMFMessage& aMessage);
		void DoSetCngL(TMMFMessage& aMessage);
		void DoGetCngL(TMMFMessage& aMessage);
		void DoSetPlcL(TMMFMessage& aMessage);

    public:     // Data
    protected:  // Data
    private:    // Data
        // pointer to the actual custom interface implementation
        CG711DecoderIntfc* iG711DecoderIntfcCI;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

	};

#endif      // CG711DECODERINTFCMSGHDLR_H
            
// End of File
