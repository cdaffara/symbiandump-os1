/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef __CT_LBS_CLIENT_OBSERVER_H__ 
#define __CT_LBS_CLIENT_OBSERVER_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*/
class CT_LbsClientObserver : public CBase
    {
    public:  // New functions
        
        /**
        * Waits for EPos server to shut down. If it is
        * not running this method will return instantaneously.
        */
        static void WaitForEPosShutdown();
        
        /**
        * Pushes WaitForEPosShutdown to the cleanup stack. It is useful when
        * other cleanup items are dependent on EPos having released all its
        * resources
        */
        static void EPosShutdownWaitPushL();
        
        /**
        * Checks if EPos server is alive.
        */
        static TBool EPosAliveL();

        /**
        * Waits for privacy server to shut down. If it is
        * not running this method will return instantaneously.
        */
        static void WaitForPrivacyServerShutdown();
        
        /**
        * Pushes WaitForPrivacyServerShutdown to the cleanup stack. It is useful
        * when other cleanup items are dependent on EPos having released all its
        * resources
        */
        static void PrivacyServerShutdownWaitPushL();
        
        /**
        * Checks if privacy server is alive.
        */
        static TBool PrivacyServerAliveL();

    private: // New functions

        /**
        * Waits for a process to die. If the process is
        * not running this method will return instantaneously.
        * @param aProcessName The name of the process.
        */
        static void WaitForProcessDeath(
        /* IN  */       const TDesC& aProcessName
        );
        
        /**
        * Checks if a process is alive.
        * @param aProcessName The name of the process.
        */
        static TBool ProcessAliveL(
        /* IN  */       const TDesC& aProcessName
        );
        
    };

#endif      // __CT_LBS_CLIENT_OBSERVER_H__  
            
// End of File
