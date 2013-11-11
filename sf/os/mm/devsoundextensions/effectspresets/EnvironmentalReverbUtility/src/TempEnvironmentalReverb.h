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
* Description:   This is the definition of the Environmental Reverb temp sub class.
*
*/



#ifndef CTEMPENVIRONMENTALREVERB_H
#define CTEMPENVIRONMENTALREVERB_H

// INCLUDES

#include <e32base.h>
#include <EnvironmentalReverbBase.h>
#include <mmfcontrollerframework.h>

/**
*  This is the EnvironmentalReverb effect proxy class responsible for handling framework messages.
*
*  @lib EnvironmentalReverbProxy.lib
*  @since 3.0
*/

class CTempEnvironmentalReverb : public CEnvironmentalReverb

	{
	public:		// Constructors and destructor

	    /**
        *
        * Factory function for creating the EnvironmentalReverb proxy object.
        * @since 3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	pointer to a EnvironmentalReverb proxy object
        */
		IMPORT_C static CTempEnvironmentalReverb* NewL(CEnvironmentalReverb* aEnvironmentalReverb);
		/**
        *
        * Destructor
        */
		virtual ~CTempEnvironmentalReverb();

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
		CTempEnvironmentalReverb();

		/**
		* Second phase constructor for this class.
        * @since	3.0
        * @return -
        */
		void ConstructL(CEnvironmentalReverb* aEnvironmentalReverb);

	private:
		CEnvironmentalReverb* iEnvironmentalReverb;
	};

#endif	// of CTEMPENVIRONMENTALREVERB_H

// End of File
