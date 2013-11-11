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
* Description:  Message handler for speech encoder configuration interface.
*
*/


#ifndef CSPEECHENCODERCONFIGMSGHDLR_H
#define CSPEECHENCODERCONFIGMSGHDLR_H

// INCLUDES
#include <mmfcontroller.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CSpeechEncoderConfig;

// CLASS DECLARATION

/**
*  Handle messages from interface proxy.
*  The class receives the messages sent by the custom interface proxy
*  and makes the appropriate call on its custom interface implementation.
*
*  @lib SpeechEncoderConfigMsgHdlr.lib
*  @since S60 3.0
*/
class CSpeechEncoderConfigMsgHdlr: public CMMFObject
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CSpeechEncoderConfigMsgHdlr* NewL(
                                                TAny* aSpeechEncoderConfigCI);
        
        /**
        * Destructor.
        */
		IMPORT_C virtual ~CSpeechEncoderConfigMsgHdlr();

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
		CSpeechEncoderConfigMsgHdlr(
		                        CSpeechEncoderConfig* aSpeechEncoderConfigCI);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		void DoHandleRequestL(TMMFMessage& aMessage);

		void DoGetNumOfSupportedBitratesL(TMMFMessage& aMessage);
		void DoGetSupportedBitratesL(TMMFMessage& aMessage);
		void DoSetBitrateL(TMMFMessage& aMessage);
		void DoGetBitrateL(TMMFMessage& aMessage);
		void DoSetVadModeL(TMMFMessage& aMessage);
		void DoGetVadModeL(TMMFMessage& aMessage);
		void CreateBufFromUintArrayL(RArray<TUint>& aSupportedBitrates);

    public:     // Data
    protected:  // Data
    private:    // Data
        // pointer to the actual custom interface implementation
        CSpeechEncoderConfig* iSpeechEncoderConfigCI;
        // buffer for the returned array of supported bitrates
		CBufFlat*         iDataCopyBuffer;
		RArray<TUint>     iSupportedBitrates;


    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

	};

#endif      // CSPEECHENCODERCONFIGMSGHDLR_H
            
// End of File
