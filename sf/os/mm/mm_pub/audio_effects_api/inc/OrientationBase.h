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
* Description:  This is the definition of the orientation effect baseclass.
*
*
*/


#ifndef CORIENTATION_H
#define CORIENTATION_H

// INCLUDES

#include <e32base.h>
#include <AudioEffectBase.h>
#include <OrientationData.h>
#include <MCustomInterface.h>

// CLASS DECLARATION

/**
*  This is the orientation effect base class.
*
*  @lib OrientationEffect.lib
*  @since 3.0
*/

class COrientation : public CAudioEffect
	{

	protected:	// Constructors and destructor

		/**
        *
        * Destructor
        */
		IMPORT_C virtual ~COrientation();

		/**
		* Private C++ constructor for this class.
        */
		IMPORT_C COrientation();


	public:		//New Functions

		/**
        * Get the orientation vectors for the position
        * @since 3.0
        * @param aHeading The heading (thousandths of radians)
        * @param aPitch The Pitch (thousandths of radians)
        * @param aRoll The Roll (thousandths of radians)
        * @return -
        */
		IMPORT_C void Orientation( TInt32& aHeading, TInt32& aPitch, TInt32& aRoll );

		/**
        * Gets the orientation of the position.
        * @since 3.0
        * @param aFrontX X value of Front vector
        * @param aFrontY Y value of Front vector
        * @param aFrontZ Z value of Front vector
        * @param aAboveX X value of Above vector
        * @param aAboveY Y value of Above vector
        * @param aAboveZ Z value of Above vector
        * @return -
        */
		IMPORT_C void OrientationVectors( TInt32& aFrontX, TInt32& aFrontY, TInt32& aFrontZ,
								   	      TInt32& aAboveX, TInt32& aAboveY, TInt32& aAboveZ );

		/**
        * Sets the Heading, Pitch, Roll values for the orientation of the source
        * @since 3.0
        * @param aHeading The heading (thousandths of radians)
        * @param aPitch The Pitch (thousandths of radians)
        * @param aRoll The Roll (thousandths of radians)
        * @return -
        */
		IMPORT_C void SetOrientationL( TInt32 aHeading, TInt32 aPitch, TInt32 aRoll );

		/**
        * Sets the Front and Above vectors for the orientation of the position.
        * @since 3.0
        * @param aFrontX X value of Front vector
        * @param aFrontY Y value of Front vector
        * @param aFrontZ Z value of Front vector
        * @param aAboveX X value of Above vector
        * @param aAboveY Y value of Above vector
        * @param aAboveZ Z value of Above vector
        * @return -
        */
		IMPORT_C void SetOrientationVectorsL( TInt32 aFrontX, TInt32 aFrontY, TInt32 aFrontZ,
								   	      TInt32 aAboveX, TInt32 aAboveY, TInt32 aAboveZ );

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

		// Orientation data structure
		TEfOrientation iOrientationData;
		// Data package sent to server
		TEfOrientationDataPckg iDataPckgTo;
		// Data package received from server
		TEfOrientationDataPckg iDataPckgFrom;
	};

#endif	// of CORIENTATION_H

// End of File
