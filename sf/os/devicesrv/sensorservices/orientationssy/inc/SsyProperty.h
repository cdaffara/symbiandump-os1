/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Property class of Orientation SSY
*
*/


#ifndef __SSYPROPERTY_H__
#define __SSYPROPERTY_H__

// INCLUDES
#include <sensrvproperty.h>

// FORWARD DECLARATIONS
class TSSyChannelProperties;

// CLASS DECLARATION

/**
 *  CSSYProperty class
 *
 *  Property class for orientation ssy.
 *
 *  @lib orientationssy.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CSSYProperty ) : public CBase
    {
    public:

        /**
         * C++ constructor.
         *
         * @param aSensorNumber
         * @param aProperties Properties of this SSY
         * @param aNumberOfProperties Number of properties this SSY has
         */
        CSSYProperty( const TInt aSensorNumber, 
                      const TSSyChannelProperties* aProperties, 
                      const TInt aNumberOfProperties );

        /**
         * Destructor
         */
        virtual ~CSSYProperty();

        /**
         * Registers a channel with ChannelId to the properties.
         * 
         * @param aChannelID Channel identifier to register
         * @return KErrAlreadyExists if channel already exists
         */
        TInt RegisterChannel( const TSensrvChannelId aChannelID );

        /**
         * Get property.
         *
         * @param aProperty Contains property information to get
         * @return KErrNone if property get is successfull
         */
        TInt GetProperty( TSensrvProperty& aProperty );

        /**
         * Set property and return affectedchannels.
         * 
         * @param aProperty Property to set
         * @param aAffectedChannels List of channels this property affects
         * @return KErrNone if property is set successfully
         */
        TInt SetProperty( const TSensrvProperty& aProperty, 
                          RSensrvChannelList& aAffectedChannels );

        /**
         * Finds property's index.
         * 
         * @param aProperty Contains property to find
         * @return index of the found property
         */
        TInt FindPropertyIndex( const TSensrvProperty& aProperty );

        /**
         * Get all properties.
         *
         * @param aChannelPropertyList Contains all properties
         * @return KErrGeneral if property type is not known
         */
        TInt GetAllProperties( RSensrvPropertyList& aChannelPropertyList );

        /**
         * Get affected channels.
         *
         * @param aAffectedChannels Affected channels for properties
         */
        void GetAffectedChannels( RSensrvChannelList& aAffectedChannels );

        /**
         * Is data rate updated.
         *
         * @return Boolean indicating is data rate already got
         */
        TBool DataRateUpdated();

    private:

        /** 
         * Array of channel property configurations. 
         */
        RArray<TSSyChannelProperties> iChannelProperties;

        /** 
         * Array of channels registered to property. 
         */
        RSensrvChannelList           iRegisteredChannels;

        /**
         * Indicates is data rate updated
         */
        TBool iDataRateUpdate;
    };


#endif // __SSYPROPERTY_H__

// End of File
