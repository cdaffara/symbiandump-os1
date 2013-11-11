// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalTechnology 
*/

#ifndef HWRMFEATUREMGR_H
#define HWRMFEATUREMGR_H


#include <e32def.h>
#include <e32cmn.h> 


class HWRMFeatureMgr
    {
    public:
        
    /**
         Fetches information whether a certain feature is supported.
        
         @param  aFeatureUid feature id.
         @return feature support status.    
    */
    
    static TBool IsFeatureSupported(const TUid aFeatureUid);
    
    private:

    /**
     C++ default constructor.
     Prohibits instantiation of this class.
    */
    HWRMFeatureMgr();
    };

#endif
