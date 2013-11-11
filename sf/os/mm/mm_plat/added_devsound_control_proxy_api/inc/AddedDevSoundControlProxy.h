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
* Description:  Interface proxy for AddedDevSoundControl CI.
*
*/


#ifndef ADDEDDEVSOUNDCONTROLPROXY_H
#define ADDEDDEVSOUNDCONTROLPROXY_H

// INCLUDES
#include <mmfcontrollerframework.h>
#include <AddedDevSoundControlCI.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CCustomInterfaceUtility;
class MCustomCommand;

// CLASS DECLARATION

/**
 * Proxy for AddedDevSoundControl CI.
 * This proxy translates the interface API calls to messages and
 * sends them to the message handler.
 *
 * @lib AddedDevSoundControlProxy.lib
 * @since S60 3.2
 */
class CAddedDevSoundControlProxy : public MAddedDevSoundControl

    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CAddedDevSoundControlProxy* NewL(
                        TMMFMessageDestinationPckg aMessageHandler,
                        MCustomCommand& aCustomCommand,
                        CCustomInterfaceUtility* aCustomInterfaceUtility);

        /**
         * Destructor.
         */
        virtual ~CAddedDevSoundControlProxy();

    public: // New functions

    public: // Functions from base classes

        // From MAddedDevSoundControl

        /**
         * Handles client request to alter DevSound's behavior for Pause.
         *
         * @since S60 3.2
         * @param TBool - Toggles HW awarness
         * @return TInt - Function call status
         *
         */
        virtual TInt SetHwAwareness(TBool aHwAware);

        /**
         * Handles client request to pause the audio resources and flush.
         * the buffers.
         *
         * @since S60 3.2
         * @return TInt - Function call status
         */
        virtual TInt PauseAndFlush();


    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
         * C++ default constructor.
         */
        CAddedDevSoundControlProxy(
                TMMFMessageDestinationPckg aMessageHandler,
                MCustomCommand& aCustomCommand,
                CCustomInterfaceUtility* aCustomInterfaceUtility);

    public:     // Data
    protected:  // Data
    private:    // Data

        // Handle to object used for sending IPC commands
        MCustomCommand& iCustomCommand;

        // Message object destined for the message handler
        TMMFMessageDestinationPckg iMessageHandler;

        // Handle to utility object used in interface framework
        CCustomInterfaceUtility* iCustomInterfaceUtility;

    };

#endif      // ADDEDDEVSOUNDCONTROLPROXY_H

// End of File
