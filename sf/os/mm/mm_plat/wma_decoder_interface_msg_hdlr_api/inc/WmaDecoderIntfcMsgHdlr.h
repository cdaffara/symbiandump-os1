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
* Description:  Message handler for WMA decoder configuration interface.
*
*/


#ifndef CWMADECODERINTFCMSGHDLR_H
#define CWMADECODERINTFCMSGHDLR_H

// INCLUDES
#include <mmfcontroller.h>
#include <WmaDecoderIntfc.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Handle messages received from the interface proxy.
*  The class receives the messages sent by the custom interface proxy
*  and makes the appropriate call on its custom interface implementation.
*
*  @lib WmaDecoderIntfcMsgHdlr.lib
*  @since S60 3.2
*/
class CWmaDecoderIntfcMsgHdlr: public CMMFObject
	{
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CWmaDecoderIntfcMsgHdlr* NewL(
                        TAny* aWmaDecoderConfigCI);

        /**
        * Destructor.
        */
		IMPORT_C virtual ~CWmaDecoderIntfcMsgHdlr();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CWmaDecoderIntfc
        */
		IMPORT_C virtual void HandleRequest(TMMFMessage& aMessage);

    protected:  // New functions
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
		CWmaDecoderIntfcMsgHdlr(CWmaDecoderIntfc* aWmaDecoderConfigCI);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Handles the messages from the proxy.
        * Determines which custom interface to call.
        * @since S60 3.2
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoHandleRequestL(TMMFMessage& aMessage);

        /**
        * Handles request for the number of supported formats methods.
        * @since S60 3.2
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
    	void DoGetSupportedFormatsL(TMMFMessage& aMessage);

        /**
        * Handles request for the supported tools methods.
        * @since S60 3.2
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
    	void DoGetSupportedToolsL(TMMFMessage& aMessage);

        /**
        * Handles request for the number of supported max channels methods.
        * @since S60 3.2
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
    	void DoGetSupportedMaxChannelsL(TMMFMessage& aMessage);

        /**
        * Handles request for the number of supported max bit rate methods.
        * @since S60 3.2
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
    	void DoGetSupportedMaxBitrateL(TMMFMessage& aMessage);

        /**
        * Handles request for the number of supported max sample methods.
        * @since S60 3.2
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
     	void DoGetSupportedMaxSampleRateL(TMMFMessage& aMessage);

        /**
        * Handles request for the controllable tools methods.
        * @since S60 3.2
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
    	void DoGetControllableToolsL(TMMFMessage& aMessage);

        /**
        * Handles request for get all configure parameters.
        * @since S60 3.2
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoGetAllL(TMMFMessage& aMessage);
		
        /**
        * Handles request for apply configuration methods.
        * @since S60 3.2
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoApplyConfigL(TMMFMessage& aMessage);

        /**
        * Handles request for number of supported formats.
        * @since S60 3.2
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
    	void DoGetNumOfSupportedFormatsL(TMMFMessage& aMessage);

        /**
        * Handles request for number of supported tools.
        * @since S60 3.2
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
    	void DoGetNumOfSupportedToolsL(TMMFMessage& aMessage);
 
         /**
        * Handles request for number of controllable tools.
        * @since S60 3.2
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
    	void DoGetNumOfControllableToolsL(TMMFMessage& aMessage);

        /**
        * Utility method converting a TUint-type array to a buffer stream.
        * @since S60 3.2
        * @param RArray<TUint>& - input array of TUint-type values.
        * @return void
        */
		void CreateBufFromUintArrayL(RArray<TUint>& aArray);

    public:     // Data
    protected:  // Data
    private:    // Data
    
        // pointer to the actual custom interface implementation
        CWmaDecoderIntfc* iWmaDecoderIntfcCI;

        // Buffer for the returned array of supported sampling frequencies
		CBufFlat* iDataCopyBuffer;

        RArray<CWmaDecoderIntfc::TFormat> iSupportedFormats;
        RArray<CWmaDecoderIntfc::TTool> iSupportedTools;
        RArray<CWmaDecoderIntfc::TTool> iControllableTools;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

	};

#endif      // CWMADECODERINTFCMSGHDLR_H

// End of File
