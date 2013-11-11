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
* Description:     Header file for S60 display posting factory 
*
*/





#ifndef __POSTING_SURFACE_FACTORY_H__
#define __POSTING_SURFACE_FACTORY_H__

//- Include Files  ----------------------------------------------------------

#include <e32base.h>

//- Constants ---------------------------------------------------------------

enum TPostingDisplay
    {
    EMainDisplay      = 0x00000000,
    ESecondaryDisplay = 0x00000001,
    };
    
//- Forward Declarations ----------------------------------------------------

class CPostingSurface;

//- Class Definitions -------------------------------------------------------

/**  CPostingSurfaceFactory
*
*  CPostingSurfaceFactory implementation dll and library file must be named 
*  "PostingSurfaceFactory.dll" and "PostingSurfaceFactory.lib" respectively. 
*/
class CPostingSurfaceFactory : public CBase
{
    public:
        /**
        *  Object Destructor
        */
    virtual ~CPostingSurfaceFactory();
        /**
        *  Creates object for user and reserves posting for that
        *  If all success method returns pointer to the object
        *  @param aDisplay is TPostingDisplay Enum value indicating which display to work on 
        *  @return new PostingSurface If there isn't enough memory returns NULL 
        */
    virtual CPostingSurface* NewPostingSurfaceL( TInt aDisplay);
};

#endif //__POSTING_SURFACE_FACTORY_H__

// End of File
