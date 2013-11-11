/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Parameter class definitions.
*
*/


#ifndef CPHCLTIMAGEPARAMS_H
#define CPHCLTIMAGEPARAMS_H

#include    <e32std.h>
#include    <e32base.h>


// Number of images in max.
const TInt KPhCltImagesArrayLen = 10; 


/**
* Enumerates image type.
* 
* @since 3.0.
* 
* EPhCltVersionNotSet - not set version nbr.
* EPhCltVersionVTStill - Video telephony still image.
* EPhCltVersionOperatorLogo - Operator logo.
* EPhCltTypeVTDefault - Video telephony default image when
*                       VT still image is not in use.
*/
enum TPhCltImageType
    {
    EPhCltTypeNotSet         = 0,
    EPhCltTypeVTStill        = 1,
    EPhCltTypeOperatorLogo   = 2,
    EPhCltTypeVTDefault      = 3
    };
    
    
/**
* Image handler uses TPhCltImageArray.
* Identification is done according to the version number, and handled
* accordingly.
*/
struct TPhCltImageArray
    {
    TInt iImages[KPhCltImagesArrayLen];
    TInt iImageCount; 
    TPhCltImageType iType;
    };


// CLASS DECLARATION
/**
*  Parameter storage class.
*
*  @lib PhoneClient.lib
*  @since Series60 3.2
*/
class CPhCltImageParams : public CBase
    {
    public:
        /**
         * Returns class type.
         * @return type number;
         */
        IMPORT_C virtual TPhCltImageType Type() const = 0;

        /**
         * Adds an image.
         * @param aHandle handle to an image.
         */
        IMPORT_C virtual void AddImageL( TInt aHandle ) = 0;

        /**
         * Gets an image.
         * @param aIndex index of the image to get.
         * @return image handle
         */
        IMPORT_C virtual TInt GetImageL( const TInt aIndex ) const = 0;

        /**
         * Gets image count.
         * @return image count
         */
        IMPORT_C virtual TInt Count() const = 0;

        /**
        * Get all the images
        * @return package buffered image struct
        */
        IMPORT_C virtual TPckgBuf< TPhCltImageArray >& Images() = 0;
    };

#endif // CPHCLTIMAGEPARAMS_H