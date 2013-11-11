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
* Description:  Message handler for AddedDevSoundControl CI.
*
*/


#ifndef CADDEDDEVSOUNDCONTROLMSGHDLR_H
#define CADDEDDEVSOUNDCONTROLMSGHDLR_H

// INCLUDES
#include <mmfcontroller.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MAddedDevSoundControl;

// CLASS DECLARATION

/**
 *  Handle messages received from the interface proxy.
 *  The class receives the messages sent by the custom interface proxy
 *  and makes the appropriate call on its custom interface implementation.
 *
 *  @lib AddedDevSoundControlMsgHdlr.lib
 *  @since S60 3.2
 */
class CAddedDevSoundControlMsgHdlr: public CMMFObject
	{
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CAddedDevSoundControlMsgHdlr* NewL(
                        TAny* aAddedDSControlCI);

        /**
         * Destructor.
         */
		virtual ~CAddedDevSoundControlMsgHdlr();

    public: // New functions

    public: // Functions from base classes

        /**
         * From CAddedDevSoundControl
         */
		virtual void HandleRequest(TMMFMessage& aMessage);

    protected:  // New functions
    protected:  // Functions from base classes

    private:

        /**
         * C++ default constructor.
         */
		CAddedDevSoundControlMsgHdlr(MAddedDevSoundControl*
		                             aAddedDSControlCI);

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * Determines which custom interface to call.
         *
         * @since S60 3.2
         * @param TMMFMessage& - client/server message container.
         * @return void
         *
         */
		void DoHandleRequestL(TMMFMessage& aMessage);

        /**
         * Handles EAddedDSControlSetHwAwareness message from the proxy and 
         * calls custom interface method.
         *
         * @since S60 3.2
         * @param TMMFMessage& - client/server message container.
         * @return void
         *
         */
		void DoSetHwAwarenessL(TMMFMessage& aMessage);

        /**
         * Handles EAddedDSControlPauseAndFlush message from the proxy and 
         * calls custom interface method.
         *
         * @since S60 3.2
         * @param TMMFMessage& - client/server message container.
         * @return void
         *
         */
        void DoPauseAndFlushL(TMMFMessage& aMessage);
        

    public:     // Data
    protected:  // Data
    private:    // Data

        // Pointer to the actual custom interface implementation
        MAddedDevSoundControl* iAddedDSControlCI;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

	};

#endif      // CADDEDDEVSOUNDCONTROLMSGHDLR_H

// End of File
