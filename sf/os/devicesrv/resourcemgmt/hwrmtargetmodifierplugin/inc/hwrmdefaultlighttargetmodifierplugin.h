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
* Description:  Hardware Resource Manager Default Light Target Modifier Plugin 
*                header file
*
*/


#ifndef __HWRMDDEFAULTLIGHTTARGETMODIFIERPLUGIN_H_
#define __HWRMDDEFAULTLIGHTTARGETMODIFIERPLUGIN_H_

#include <hwrmtargetmodifierplugin.h>

/**
*  S60 default Target Modifier Plugin.
*  Responsible for modifying targets.
*
*  @lib HWRMDEFAULTLIGHTTARGETMODIFIERPLUGIN.DLL
*  @since S60 v3.2
*/
class CHWRMDefaultLightTargetModifierPlugin : public CHWRMTargetModifierPlugin
    {
    public:

        static CHWRMDefaultLightTargetModifierPlugin* NewL();
        
        virtual ~CHWRMDefaultLightTargetModifierPlugin();

        /**
        * Method to modify target according to customer specific needs.
        *
        * @since S60 v3.2
        * @see CHWRMTargetModifierPlugin::ModifyTargetL 
        */
        virtual TInt ModifyTargetL( TInt aTarget );
        
        /**
        * Method to get full system target.
        *
        * @since S60 v3.2
        * @see CHWRMTargetModifierPlugin::GetFullSystemTarget 
        */
        virtual TInt GetFullSystemTarget();

    private: // Construction

        CHWRMDefaultLightTargetModifierPlugin();
        
        void ConstructL();
        
    private: 
        TBool                  iCoverDisplaySupported;    // ETrue if cover display is supported by platform
        TBool                  iGripSupported;            // ETrue if grip is supported by platform
    };

#endif // __HWRMDDEFAULTLIGHTTARGETMODIFIERPLUGIN_H_