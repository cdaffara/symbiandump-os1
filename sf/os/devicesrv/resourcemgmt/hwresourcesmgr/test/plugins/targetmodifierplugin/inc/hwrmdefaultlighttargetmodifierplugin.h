/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __HWRMDDEFAULTLIGHTTARGETMODIFIERPLUGIN_H_
#define __HWRMDDEFAULTLIGHTTARGETMODIFIERPLUGIN_H_

#include <hwrmtargetmodifierplugin.h>

/**
*  Default Target Modifier Plugin.
*  Responsible for modifying targets.
*
*/
class CHWRMDefaultLightTargetModifierPlugin : public CHWRMTargetModifierPlugin
    {
    public:

        static CHWRMDefaultLightTargetModifierPlugin* NewL();
        
        virtual ~CHWRMDefaultLightTargetModifierPlugin();

        /**
        * Method to modify target according to customer specific needs.
        *
        * @see CHWRMTargetModifierPlugin::ModifyTargetL 
        */
        virtual TInt ModifyTargetL( TInt aTarget );
        
        /**
        * Method to get full system target.
        *
        * @see CHWRMTargetModifierPlugin::GetFullSystemTarget 
        */
        virtual TInt GetFullSystemTarget();

    private: // Construction

        CHWRMDefaultLightTargetModifierPlugin();

        TBool IsFeatureSupported(const TUid aFeatureUid);
        
        void ConstructL();
        
    private: 
        TBool                  iCoverDisplaySupported;    // ETrue if cover display is supported by platform
        TBool                  iGripSupported;            // ETrue if grip is supported by platform
    };

#endif // __HWRMDDEFAULTLIGHTTARGETMODIFIERPLUGIN_H_
