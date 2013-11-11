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
* This file contains the class declaration for the target modifier plugin API.
*
*/




/**
 @file
 @publishedPartner
 @released
*/



#ifndef HWRMTARGETMODIFIERPLUGIN_H
#define HWRMTARGETMODIFIERPLUGIN_H

#include <e32base.h>

/** ECOM Target modifier plugin interface UID. 
@publishedPartner
@released
*/
const TUid KHWRMTargetModifierPluginInterfaceUid = { 0x10281B99 };

/**
*  Plugin interface class to be implemented by customers.
*  Responsible for modifying targets
@publishedPartner
@released
*/
class CHWRMTargetModifierPlugin : public CBase
    {
    public:  // construction and destruction
    
        /**
        * Constructor method for instance.
        * Uses ECom to find an instance. 
        *
        * @param aMatch Determines the API that the loaded plugin implements.
        *               Same match value is used to find target modifier plugin 
        *                as is used to find corresponding HWRM plugin service.
        * @return New CHWRMTargetModifierPlugin instance.
        * @leave Standard Symbian leaves.
        */
        inline static CHWRMTargetModifierPlugin* NewL(const TDesC8& aMatch);

        /**
        * Destructor
        */
        inline virtual ~CHWRMTargetModifierPlugin();       


    public:
        /**
        * Method to modify target according to customer specific needs by removing
        * currently inactive (i.e. covered or otherwise hidden or just not active for
        * other reasons) targets from supplied mask. 
        * 
        * Modifying target must be restricted to removing unsupported elements from 
        * the supplied target. Adding new targets will result in problems with reservation
        * handling and is therefore not supported.
        *
        * @param aTarget            Target mask to be modified.
        * @return Modified target   If the returned target is empty, the related command will
        *                            complete with success but without any actual effect.
        *
        * @leave KErrNotSupported if supplied target mask contains currently illegal targets
        *             and cannot for some reason be modified so that modified mask would
        *             contain only legal (or no) targets. This might be desirable functionality
        *             with some product specific custom targets. Target masks containing only 
        *             system targets should never cause a leave.
        */
        virtual TInt ModifyTargetL(TInt aTarget) = 0;

        /**
        * Method to get full system target mask, including system targets that are
        * currently unavailable.
        * This is called only once during initialization per plugin.        
        *
        * @return All targets that are considered system targets.
        */
        virtual TInt GetFullSystemTarget() = 0;


   protected:
   
        /**
        * Destructor identifier to be used with ECom framework.
        */
        TUid iDestructorIDKey;
    };

#include <hwrm/hwrmtargetmodifierplugin.inl>

#endif      // HWRMTARGETMODIFIERPLUGIN_H   
            
// End of File
