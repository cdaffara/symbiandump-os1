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
* Description:  This is the definition of the audio effects base class.
*
*
*/


#ifndef CAUDIOEFFECT_H
#define CAUDIOEFFECT_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATION
class MAudioEffectObserver;

// CLASS DECLARATION

/**
*  This is the base class for audio effects.
*
*  @lib AudioEffectBase.lib
*  @since 3.0
*/

class CAudioEffect : public CBase
	{

	public: // Constructors and destructor

		/**
        *	Destructor
        */
		IMPORT_C virtual ~CAudioEffect();

    public: // New functions

		/**
        * Apply effect settings
        * @since 3.0
        */
		virtual void ApplyL() = 0;

		/**
        * Disable the effect
        * @since 3.0
        */
		IMPORT_C virtual void DisableL();

		/**
        * Enable the effect
        * @since 3.0
        */
		IMPORT_C virtual void EnableL();

		/**
        * Enforce the effect.
        * @since 3.0
        * @param aEnforced Indicate the effect is to be enforced or not. ETrue = Enforced.
        */
		IMPORT_C virtual void EnforceL( TBool aEnforced );

		/**
        * Check if this effect object currently has update rights.
        * A client can lose update rights in some hardware platforms where there are a limited
        * number of instances of an effect that can exist at the same time. When an effect instance
        * has lost update rights the user can still change settings, but any calls to Apply the
        * settings will be deferred until update rights are regained.
        * @since 3.0
        * @return ETrue if this object currently has rights to update the settings of this effect,
        *         EFalse otherwise.
        */
		IMPORT_C virtual TBool HaveUpdateRights() const;

		/**
        * Check if the effect is enabled
        * @since 3.0
        * @return ETrue if the effect is enabled, EFalse if the effect is disabled.
        */
		IMPORT_C virtual TBool IsEnabled() const;

		/**
        * Check if the effect is enforced.
        * @since 3.0
        * @return ETrue if the effect is enforced, EFalse if the effect isn ot enforced.
        */
		IMPORT_C virtual TBool IsEnforced() const;

		/**
        * Adds the specified observer to the list of observers to be notified when
        * the effect object changes state.
        * @since 3.0
        * @param aObserver Object to be added to notifier list.
        */
		IMPORT_C void RegisterObserverL( MAudioEffectObserver& aObserver );

		/*
        * Get the unique identifier of the audio effect
        * @since 3.0
        * @return Unique identifier of the audio effect object.
        */
		virtual TUid Uid() const = 0 ;

		/**
        * Removes the specified observer from the list of observers.
        * @since 3.0
        * @param aObserver object to be removed.
        */
		IMPORT_C void UnRegisterObserver( MAudioEffectObserver& aObserver );

	protected:

		/**
		* Private C++ constructor for this class.
        * @since 3.0
        * @param aEffectObserver	reference to event observer object
        * @return -
        */
		IMPORT_C CAudioEffect();

		/**
		* Internal function to package data into a descriptor.
        * @since 3.0
        * @return A descriptor containing the effect data.
        */
		virtual const TDesC8& DoEffectData() = 0 ;

		/**
		* Internal function to unpack effect data
        * @since 3.0
        * @param aEffectDataBuffer Descriptor containing packed effect data
        */
        virtual void SetEffectData( const TDesC8& aEffectDataBuffer ) = 0;

	protected: // Data

		// Flag to indicate whether the effect is enabled or not
		TBool iEnabled;
		// Flag to indicate wheter the effect is enforced
		TBool iEnforced;
		// Flag to indicate wheter the effect current has update rights
		TBool iHaveUpdateRights;
		// Pointer to Observers
		RPointerArray<MAudioEffectObserver> iObservers;
	};

#endif	// of CAUDIOEFFECT_H

// End of File
