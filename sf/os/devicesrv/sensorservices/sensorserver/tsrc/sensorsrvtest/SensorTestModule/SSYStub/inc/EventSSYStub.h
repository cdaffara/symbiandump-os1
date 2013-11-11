/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef __CEVENTSSYSTUB_H_
#define __CEVENTSSYSTUB_H_

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <ssycontrol.h>

#include "EventChannel.h"
#include <ssypropertyprovider.h>




// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  SSY sensor stub
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CEventSsyStub : public CSsyControl,
                                public MSsyPropertyProvider
    {
    public:  // Constructors and destructor

       static CEventSsyStub* NewL( MSsyCallback* aSsyCallback );
        /**
        * Destructor.
        */
        virtual ~CEventSsyStub();

    public: // From CSsyControl

    	/**
        * Request from sensor server to open a sensor channel to client side.
        *
        * @since S60 5.0
        * @param aChannelID Channel that is requested to be opened
        */
		void OpenChannelL( TSensrvChannelId aChannelID);

    	/**
        * Request to close a sensor channel
        *
        * @since S60 5.0
        * @param aChannelID Channel that is reqeusted to be closed
        * @param aRequestStatus Request must be completed when a channel is closed
        */
		void CloseChannelL( TSensrvChannelId aChannelID );

    public: // From MSsyPropertyProvider

        /**
        * Check if property value affects other sensor channels already open.
        * If the new property value is legal but affects somehow other channels'
        * properties, SSY must return list of the affected channels so that the
        * sensor server can check if the client is allowed to set this property.
        * If the SSY value is not legal SSY must leave with KErrArgument-value.
        *
        * @since S60 5.0
        * @param[in] aChannelId Channel Id to be linked to this operation
        * @param[in] aProperty Property to be checked.
        * @param[out] aAffectedChannels Return list of the channel which will be
        *             affected if the property value will be set.
        * @leave KErrArgument If the property value is illegal.
        */
        void CheckPropertyDependenciesL
                                  ( const TSensrvChannelId aChannelId,
                                    const TSensrvProperty& aProperty,
                                    RSensrvChannelList& aAffectedChannels );

        /**
        * Set property for the channel. Before the sensor server sets the property
        * value, it is checked with CheckPropertyDependenciesL()-function.
        * This means a property value should always be valid for the SSY.
        *
        * @since S60 5.0
        * @param[in] aChannelId Channel Id to be linked to this set property
        *            operation
        * @param[in] aProperty Rereference to a property object to be set
        */
        void SetPropertyL(  const TSensrvChannelId aChannelId,
                                    const TSensrvProperty& aProperty );

        /**
        * Get channel property value. The property parameter contains channel id and
        * item index. SSY fills values and attributes to the property object.
        *
        * @since S60 5.0
        * @param[in] aChannelId Channel Id to be linked to this get property operation
        * @param[in, out] aProperty Reference to a property object to be filled with
        * property values and attributes.
        */
        void GetPropertyL( const TSensrvChannelId aChannelId,
                                   TSensrvProperty& aProperty );

        /**
        * Get all channel properties. Returns all properties which are related to this
        * channel.
        *
        * @since S60 5.0
        * @param[in] aChannelId Channel Id to be linked to this get all properties
        *            operation
        * @param[out] aChannelPropertyList List of the all properties of the channel.
        */
        void GetAllPropertiesL( const TSensrvChannelId aChannelId,
                                        RSensrvPropertyList& aChannelPropertyList );

    	/**
    	* Returns a pointer to a specified interface extension - to allow future extension
    	* of this class without breaking binary compatibility
        *
        * @since S60 5.0
    	* @param aInterfaceUid Identifier of the interface to be retrieved
    	* @param aInterface A reference to a pointer that retrieves the specified interface.
    	*/
    	void GetPropertyProviderInterfaceL( TUid aInterfaceUid,
    	                                            TAny*& aInterface );


        /**
		* Callback function for CPeriodic
		*
		*/
    	static TInt PropertyPeriodicTick( TAny* aObject );




	protected:
	    CEventSsyStub( MSsyCallback* aSsyCallback );

	    void ConstructL();

	    void PropertyChannged( const TSensrvChannelId aChannelId, TSensrvProperty& aProperty );
	    TInt DoPropertyPeriodicTick();


    protected:    // Data

         MSsyCallback* iSsyCallback;

         RPointerArray<CEventChannel> iChannelArray;

         TSensrvProperty iPropDataRate;
         TSensrvProperty iPropMeasureRange;
         TSensrvProperty iPropSensorDescription;
         TSensrvProperty iPropAvailability;

         CPeriodic*          iPeriodicForProperty;


   	public:     // Friend classes

    };

#endif // __CEVENTSSYSTUB_H_
