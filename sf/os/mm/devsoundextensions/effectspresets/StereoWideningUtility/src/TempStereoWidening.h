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
* Description:   This is the definition of the Audio StereoWidening proxy class.
*
*/



#ifndef CTEMPSTEREOWIDENING_H
#define CTEMPSTEREOWIDENING_H

// INCLUDES

#include <e32base.h>
#include <StereoWideningBase.h>
#include <mmfcontrollerframework.h>

/**
*  This is the StereoWidening effect proxy class responsible for handling framework messages.
*
*  @lib StereoWideningProxy.lib
*  @since 3.0
*/

class CTempStereoWidening : public CStereoWidening

	{
	public:		// Constructors and destructor

	    /**
        *
        * Factory function for creating the StereoWidening proxy object.
        * @since 3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	pointer to a StereoWidening proxy object
        */
		IMPORT_C static CTempStereoWidening* NewL(TInt aLevel, CStereoWidening* aStereoWidening);
		/**
        *
        * Destructor
        */
		virtual ~CTempStereoWidening();

	public: // functions from base class

		/**
		* From CAudioEffect
        * Apply effect settings
        * @since 3.0
        */
		IMPORT_C virtual void ApplyL();
		
        /**
		* From CAudioEffect
        * check enable flag
        * @since 3.0
        */
	    IMPORT_C virtual TBool IsEnabled() const;
	
		/**
        * Disable the effect
        * @since 3.0
        */
		IMPORT_C virtual void DisableL();
	
		/**
        * Check if the effect is enforced.
        * @since 3.0
        * @return ETrue if the effect is enforced, EFalse if the effect isn ot enforced.
        */
		IMPORT_C virtual TBool IsEnforced() const;

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

	private:

		/**
		* Private C++ constructor for this class.
        * @since	3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	-
        */
		CTempStereoWidening();

		/**
		* Second phase constructor for this class.
        * @since	3.0
        * @return -
        */
		void ConstructL(TInt aLevel,CStereoWidening* aStereoWidening);

	private:
		CStereoWidening* iStereoWidening;
	};

#endif	// of CTEMPSTEREOWIDENING_H

// End of File
