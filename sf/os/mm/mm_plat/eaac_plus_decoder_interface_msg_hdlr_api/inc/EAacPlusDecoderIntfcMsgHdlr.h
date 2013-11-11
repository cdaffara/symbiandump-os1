/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message handler for eAAC+ decoder configuration interface.
*
*/


#ifndef CEAACPLUSDECODERINTFCMSGHDLR_H
#define CEAACPLUSDECODERINTFCMSGHDLR_H

// INCLUDES
#include <mmfcontroller.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CEAacPlusDecoderIntfc;

// CLASS DECLARATION

/**
*  Handle messages received from the interface proxy.
*  The class receives the messages sent by the custom interface proxy
*  and makes the appropriate call on its custom interface implementation.
*
*  @lib EAacPlusDecoderIntfcMsgHdlr.lib
*  @since S60 3.0
*/
class CEAacPlusDecoderIntfcMsgHdlr: public CMMFObject
	{
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CEAacPlusDecoderIntfcMsgHdlr* NewL(
                        TAny* aAacDecoderConfigCI);

        /**
        * Destructor.
        */
		IMPORT_C virtual ~CEAacPlusDecoderIntfcMsgHdlr();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CEAacPlusDecoderIntfc
        */
		IMPORT_C virtual void HandleRequest(TMMFMessage& aMessage);

    protected:  // New functions
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
		CEAacPlusDecoderIntfcMsgHdlr(CEAacPlusDecoderIntfc*
		                             aAacDecoderConfigCI);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		void DoHandleRequestL(TMMFMessage& aMessage);

		void DoApplyConfigL(TMMFMessage& aMessage);


    public:     // Data
    protected:  // Data
    private:    // Data
    
        // pointer to the actual custom interface implementation
        CEAacPlusDecoderIntfc* iAacDecoderConfigCI;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

	};

#endif      // CEAACPLUSDECODERINTFCMSGHDLR_H

// End of File
