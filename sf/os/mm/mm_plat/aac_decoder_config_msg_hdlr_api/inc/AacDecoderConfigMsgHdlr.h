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
* Description:  Message handler for AAC decoder configuration interface.
*
*/


#ifndef CAACDECODERCONFIGMSGHDLR_H
#define CAACDECODERCONFIGMSGHDLR_H

// INCLUDES
#include <mmfcontroller.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAacDecoderConfig;

// CLASS DECLARATION

/**
*  Handle messages from interface proxy.
*  The class receives the messages sent by the custom interface proxy
*  and makes the appropriate call on its custom interface implementation.
*
*  @lib AacDecoderConfigMsgHdlr.lib
*  @since S60 3.0
*/
class CAacDecoderConfigMsgHdlr: public CMMFObject
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CAacDecoderConfigMsgHdlr* NewL(TAny* aAacDecoderConfigCI);
        
        /**
        * Destructor.
        */
		IMPORT_C virtual ~CAacDecoderConfigMsgHdlr();

    public: // New functions
        
    public: // Functions from base classes

        /**
        * From CAacDecoderConfig
        */
		IMPORT_C virtual void HandleRequest(TMMFMessage& aMessage);        
		
    protected:  // New functions
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
		CAacDecoderConfigMsgHdlr(CAacDecoderConfig* aAacDecoderConfigCI);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		void DoHandleRequestL(TMMFMessage& aMessage);

		void DoSetAudioConfigL(TMMFMessage& aMessage);

    public:     // Data
    protected:  // Data
    private:    // Data
        // pointer to the actual custom interface implementation
        CAacDecoderConfig* iAacDecoderConfigCI;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

	};

#endif      // CAACDECODERCONFIGMSGHDLR_H
            
// End of File
