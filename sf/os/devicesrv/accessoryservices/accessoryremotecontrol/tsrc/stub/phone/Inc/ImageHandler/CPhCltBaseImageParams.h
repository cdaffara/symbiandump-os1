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


#ifndef CPHCLTBASEPARAMS_H
#define CPHCLTBASEPARAMS_H

#include    <CPhCltImageParams.h>
#include    <e32std.h>
#include    <e32base.h>

/**
* Logo types
*
* @since Series60 3.0
*
* EPhCltLogoTypeOTA - OTA operator logo
* EPhCltLogoTypeProgrammable - programmable operator logo
*/
enum TPhCltExtOperatorLogoType
    {
    EPhCltLogoTypeOTA,
    EPhCltLogoTypeProgrammable
    };
       
 // Country code type definition.
typedef TInt TPhCltCountryCode;

// Network code type definition.
typedef TInt TPhCltNetworkCode;

// Constant to be used when deleting operator logo(s).
const TInt KPhCltDeleteOperatorLogo = 0;

struct TTelephonyOTALogoUpdate
    {
    /**
    * EPSTelephonyLogoChanged - Logo was added or replaced
    * EPSTelephonyLogoDeleted - Logo was deleted
    */
    enum TPSOperatorLogoUpdateEvent
        {
        EPSTelephonyLogoChanged,
        EPSTelephonyLogoDeleted,
        EPSTelephonyAllLogosDeleted
        };
    
    // Event type
    TPSOperatorLogoUpdateEvent iEvent;

    /**
    * Country code associated to logo, N/A if event
    * is EPSTelephonyAllLogosDeleted
    */ 
    TPhCltCountryCode iMCC;

    /**
    * Network code associated to logo, N/A if event
    * is EPSTelephonyAllLogosDeleted
    */ 
    TPhCltNetworkCode iMNC;

    typedef TPckg<TTelephonyOTALogoUpdate> TOTALogoUpdatePckg;
    };

// CLASS DECLARATION
/**
*  Parameter storage class.
*
*  @lib PhoneClient.lib
*  @since Series60 3.2
*/
NONSHARABLE_CLASS( CPhCltBaseImageParams ) 
: public CPhCltImageParams
    {
    public:
        /**
         * Returns class type.
         * @return type number;
         */
        virtual TPhCltImageType Type() const;

        /**
         * Adds an image.
         * @param aHandle handle to an image.
         */
        virtual void AddImageL( TInt aHandle );

        /**
        * Assignment operator
        * @param aParamClass parameter class.
        * @return 'deep' copied parameter class.
        */
        virtual CPhCltBaseImageParams& operator=( 
            const CPhCltBaseImageParams& aParamClass );

        /**
         * Gets an image.
         * @param aIndex index of the image to get.
         * @return image handle
         */
        virtual TInt GetImageL( const TInt aIndex ) const;

        /**
         * Gets image count.
         * @return image count
         */
        virtual TInt Count() const;

        /**
        * Get all the images
        * @return package buffered image struct
        */
        virtual TPckgBuf< TPhCltImageArray >& Images();

    protected:
        /**
         * Cleans class content.
         */
        void Clean();

    public: // Constructor and destructor.
        /**
         * Destructor.
         */
        virtual ~CPhCltBaseImageParams();

    protected:

        /**
         * Constructor.
         */
        CPhCltBaseImageParams( const TPhCltImageType aType  );

    public: // Data.
        TPckgBuf< TPhCltImageArray > iImages;
    };

// CLASS DECLARATION
/**
*  Parameter storage class.
*
*  @lib PhoneClient.lib
*  @since Series60 3.2
*/
NONSHARABLE_CLASS( CPhCltExtVTImageParams ) : public CPhCltBaseImageParams
    {
    public: // Constructor and destructor.
        /**
         * Destructor.
         */
        ~CPhCltExtVTImageParams();

        /**
         * Symbian 2 phase constructor.
         */
        static CPhCltExtVTImageParams* NewL( const TPhCltImageType aType );

    private:

        /**
         * Constructor.
         */
        CPhCltExtVTImageParams( const TPhCltImageType aType );
    };

// CLASS DECLARATION
/**
*  Parameter storage class.
*
*  @lib PhoneClient.lib
*  @since Series60 3.2
*/
NONSHARABLE_CLASS( CPhCltExtOperatorLogoParams ) 
: public CPhCltBaseImageParams
    {
    public: // Constructor and destructor.
        /**
         * Destructor.
         */
        ~CPhCltExtOperatorLogoParams();

        /**
         * Symbian 2 phase constructor.
         */
        static CPhCltExtOperatorLogoParams* NewL( );
        
    public: // New functions.
        
        /**
        * @see CPhCltImageParams::operator=.
        */
        CPhCltBaseImageParams& operator=( 
            const CPhCltBaseImageParams& aParamClass );
            
        /**
        * @see CPhCltImageParams::AddImageL.
        * NOTE: when deleting, set image to KPhCltDeleteOperatorLogo
        * @see CPhCltExtOperatorLogoParams::SetCodesL
        */
        void AddImageL( TInt aHandle );
        
        /**
        * Sets country and network code for operator logo.
        * @param aCountryCode country code
        * @param aNetworkCode network code
        * @param aLogoType OTA/programmable logo
        * NOTE: delete all: set codes to KPhCltDeleteOperatorLogo
        */        
        virtual void SetCodesL( 
            const TPhCltCountryCode aCountryCode, 
            const TPhCltNetworkCode aNetworkCode,
            const TPhCltExtOperatorLogoType aLogoType );
        
    private:

        /**
         * Constructor.
         */
        CPhCltExtOperatorLogoParams();
    };

#endif // CPHCLTBASEPARAMS_H

// End of file.
