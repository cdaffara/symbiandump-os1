/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TCT_LBS_CLIENT_MODULES_OBSERVER_H__ 
#define __TCT_LBS_CLIENT_MODULES_OBSERVER_H__

//  INCLUDES
#include "epos_mposmodulesobserver.h"

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*/
class TCT_LbsClientModulesObserver :public MPosModulesObserver
    {
    public:  // Functions from base classes

        /**
        * From MPosModulesDbObserver
        */
        inline void HandleSettingsChangeL(
        /* IN  */       TPosModulesEvent /*aEvent*/
        ){CActiveScheduler::Stop();};


    };

#endif      // __TCT_LBS_CLIENT_MODULES_OBSERVER_H__

// End of File
