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
* Description:  Message handler for Ilbc encoder interface.
*
*/


#ifndef CILBCENCODERINTFCMSGHDLR_H
#define CILBCENCODERINTFCMSGHDLR_H

// INCLUDES
#include <mmfcontroller.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIlbcEncoderIntfc;

// CLASS DECLARATION

/**
*  Handle messages from interface proxy.
*  The class receives the messages sent by the custom interface proxy
*  and makes the appropriate call on its custom interface implementation.
*
*  @lib IlbcEncoderIntfcMsgHdlr.lib
*  @since S60 3.0
*/
class CIlbcEncoderIntfcMsgHdlr: public CMMFObject
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIlbcEncoderIntfcMsgHdlr* NewL(
                                           TAny* aIlbcEncoderIntfcImpl);
        
        /**
        * Destructor.
        */
		IMPORT_C virtual ~CIlbcEncoderIntfcMsgHdlr();

    public: // New functions
        
    public: // Functions from base classes
		IMPORT_C virtual void HandleRequest(TMMFMessage& aMessage);        
		
    protected:  // New functions
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
		CIlbcEncoderIntfcMsgHdlr(
		                  CIlbcEncoderIntfc* aIlbcEncoderIntfcImpl);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		void DoHandleRequestL(TMMFMessage& aMessage);

		void DoSetEncoderModeL(TMMFMessage& aMessage);
		void DoSetVadModeL(TMMFMessage& aMessage);
		void DoGetVadModeL(TMMFMessage& aMessage);

    public:     // Data
    protected:  // Data
    private:    // Data
        // pointer to the actual custom interface implementation
        CIlbcEncoderIntfc* iIlbcEncoderIntfcCI;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

	};

#endif      // CILBCENCODERINTFCMSGHDLR_H
            
// End of File
