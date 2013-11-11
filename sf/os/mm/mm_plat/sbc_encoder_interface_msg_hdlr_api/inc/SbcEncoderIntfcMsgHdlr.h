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
* Description:  Message handler for SBC encoder interface.
*
*/


#ifndef CSBCENCODERINTFCMSGHDLR_H
#define CSBCENCODERINTFCMSGHDLR_H

// INCLUDES
#include <mmfcontroller.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CSbcEncoderIntfc;

// CLASS DECLARATION

/**
*  Handle messages from interface proxy.
*  The class receives the messages sent by the custom interface proxy
*  and makes the appropriate call on its custom interface implementation.
*
*  @lib SbcEncoderIntfcMsgHdlr.lib
*  @since S60 3.0
*/
class CSbcEncoderIntfcMsgHdlr: public CMMFObject
	{
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CSbcEncoderIntfcMsgHdlr* NewL(TAny* aSbcEncoderIntfcCI);

        /**
        * Destructor.
        */
		IMPORT_C virtual ~CSbcEncoderIntfcMsgHdlr();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CMMFObject
        * Handles the messages from the proxy.
        * Calls a subfunction which determines which custom interface to call.
        * A subfunction is used to contain multiple leaving functions for a
        * single trap.

        */
		IMPORT_C virtual void HandleRequest(TMMFMessage& aMessage);

    protected:  // New functions
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
		CSbcEncoderIntfcMsgHdlr(CSbcEncoderIntfc* aSbcEncoderIntfcCI);

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Handles the messages from the proxy.
        * Determines which custom interface to call.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoHandleRequestL(TMMFMessage& aMessage);

        /**
        * Handles request for the number of supported sampling frequecies.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoGetNumOfSupportedSamplingFrequenciesL(TMMFMessage& aMessage);

        /**
        * Handles request for returning of all supported sampling frequecies.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoGetSupportedSamplingFrequenciesL(TMMFMessage& aMessage);

        /**
        * Handles request for the number of supported channel modes.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoGetNumOfSupportedChannelModesL(TMMFMessage& aMessage);

        /**
        * Handles request for returning of all supported channel modes.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoGetSupportedChannelModesL(TMMFMessage& aMessage);

        /**
        * Handles request for the number of supported blocks.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoGetNumOfSupportedBlocksL(TMMFMessage& aMessage);

        /**
        * Handles request for returning of all supported blocks.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoGetSupportedBlocksL(TMMFMessage& aMessage);

        /**
        * Handles request for the number of supported subbands.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoGetNumOfSupportedNumOfSubbandsL(TMMFMessage& aMessage);

        /**
        * Handles request for returning of all supported subbands.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoGetSupportedNumOfSubbandsL(TMMFMessage& aMessage);

        /**
        * Handles request for the number of supported allocation methods.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoGetNumOfSupportedAllocationMethodsL(TMMFMessage& aMessage);

        /**
        * Handles request for returning of all supported allocation methods.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoGetSupportedAllocationMethodsL(TMMFMessage& aMessage);

        /**
        * Handles request for the supported bitpool range.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoGetSupportedBitpoolRangeL(TMMFMessage& aMessage);

        /**
        * Handles request for commiting configuration settings.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void DoApplyConfigL(TMMFMessage& aMessage);

        /**
        * Utility method converting a TUint-type array to a buffer stream.
        * @since S60 3.0
        * @param RArray<TUint>& - input array of TUint-type values.
        * @return void
        */
		void CreateBufFromUintArrayL(RArray<TUint>& aArray);

        /**
        * Sends data buffer to the client.
        * @since S60 3.0
        * @param TMMFMessage& - message from the proxy
        * @return void
        */
		void SendDataBufferToClientL(TMMFMessage& aMessage);


    public:     // Data
    protected:  // Data
    private:    // Data

        // Pointer to the actual custom interface implementation
        CSbcEncoderIntfc* iSbcEncoderIntfcCI;

        // Buffer for the returned array of supported sampling frequencies
		CBufFlat* iDataCopyBuffer;

	};

#endif      // CSBCENCODERINTFCMSGHDLR_H

// End of File
