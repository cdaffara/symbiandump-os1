/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Audio Output Custom Interface Handler class  
*
*/

#include <e32base.h>
#include "AudioOutputCI.h"
#include <mmfcontrollerframework.h>

class CAudioOutputCustomInterfaceHandler : public CActive
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CAudioOutputCustomInterfaceHandler* NewL();

        /**
        * Request to generate event
        * @aEvent The event to generate
        * @aError The error to report with the event
        * @since 3.0
        */
        void HandleRequest(TMMFMessage& aMessage);

        virtual ~CAudioOutputCustomInterfaceHandler();

    private:

        /**
        * C++ default constructor.
        */
        CAudioOutputCustomInterfaceHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        void RunL();

        void DoCancel();

        /**
        * Used to complete a request
        */
        void Ready(const TInt aStatus);

    private:

		TMMFMessage* iMessage;
    };

