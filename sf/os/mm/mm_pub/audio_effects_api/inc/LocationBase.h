/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the definition of the base class for location effect.
*
*
*/


#ifndef CLOCATION_H
#define CLOCATION_H

// INCLUDES

#include <e32base.h>
#include <AudioEffectBase.h>
#include <LocationData.h>

// CLASS DECLARATION

/**
*  This is the base class for location effect settings.
*
*  @lib LocationEffect.lib
*  @since 3.0
*/

class CLocation : public CAudioEffect
	{

	protected:	// Constructors and destructor

		/**
        *
        * Destructor
        */
		IMPORT_C virtual ~CLocation();

		/**
		* Private C++ constructor for this class.
        */
		IMPORT_C CLocation();

	public: // New Functions

		/**
        * Gets the cartesian coordinates for the location of the position.
        * @since 3.0
        * @param aX The x-coordinate of the position (in millimeters)
        * @param aY The y-coordinate of the position (in millimeters)
        * @param aZ The z-coordinate of the position (in millimeters)
        * @return -
        */
		IMPORT_C void LocationCartesian( TInt32& aX, TInt32& aY, TInt32& aZ );

		/**
        * Gets the spherical coordinates for the location of the position.
        * @since 3.0
        * @param aAzimuth The Azimuth of the position (thousandths of radians)
        * @param aElevation The elevation of the position (thousandths of radians)
        * @param aRadius The radius of the position (thousandths of radians)
        * @return -
        */
		IMPORT_C void LocationSpherical( TInt32& aAzimuth, TInt32& aElevation, TInt32& aRadius );

		/**
        * Sets the cartesian coordinates for the location of the position.
        * @since 3.0
        * @param aX The x-coordinate of the position
        * @param aY The y-coordinate of the position
        * @param aZ The z-coordinate of the position
        * @return -
        */
		IMPORT_C void SetLocationCartesianL( TInt32& aX, TInt32& aY, TInt32& aZ );

		/**
        * Sets the spherical coordinates for the location of the position.
        * @since 3.0
        * @param aAzimuth The Azimuth of the position (thousandths of radians)
        * @param aElevation The elevation of the position (thousandths of radians)
        * @param aRadius The radius of the position (thousandths of radians)
        * @return -
        */
		IMPORT_C void SetLocationSphericalL( TInt32& aAzimuth, TInt32& aElevation, TInt32& aRadius );

    protected:  // Functions from base classes

		/**
		* From CAudioEffect
		* Create a package of the effect data
        * @since 3.0
        * @return A descriptor containing the effect data.
        */
		IMPORT_C const TDesC8& DoEffectData();

		/**
		* From CAudioEffect
		* Internal function to unpack effect data
        * @since 3.0
        * @param aEffectDataBuffer Descriptor containing packed effect data
        * @return -
        */
        IMPORT_C void SetEffectData( const TDesC8& aEffectDataBuffer );

	protected:

		// Location data structure
		TEfLocation iLocationData;
		// Data package sent to server
		TEfLocationDataPckg iDataPckgTo;
		// Data package received from server
		TEfLocationDataPckg iDataPckgFrom;
	};

#endif	// of CLOCATION_H

// End of File
