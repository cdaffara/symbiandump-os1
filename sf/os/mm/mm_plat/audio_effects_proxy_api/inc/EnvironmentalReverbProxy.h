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
* Description:  This is the definition of the Environmental Reverb proxy class.
*
*
*/


#ifndef CENVIRONMENTALREVERBPROXY_H
#define CENVIRONMENTALREVERBPROXY_H

// INCLUDES

#include <e32base.h>
#include "EnvironmentalReverbMessageTypes.h"
#include <EnvironmentalReverbBase.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <CustomCommandUtility.h>
#include <MEnvironmentalReverbObserver.h>
#include <MAudioEffectObserver.h>

// FORWARD DECLARATION
class CEnvironmentalReverbEventObserver;
class CRoomLevelProxy;

// CLASS DECLARATION

/**
*  Interface class to be implemented by objects that receives callbacks from Environmental Reverb Event Observer.
*
*  @since 3.0
*/
class MEnvironmentalReverbCallback
    {
    public:

        /**
        * Invoked by the EnvironmentalReverb Event Observer when the EnvironmentalReverb object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the EnvironmentalReverb data
        */
        virtual void EnvironmentalReverbEvent( const TDesC8& aDataPckg ) = 0;

    };

/**
*  This is the EnvironmentalReverb effect proxy class responsible for handling framework messages.
*
*  @lib EnvironmentalReverbProxy.lib
*  @since 3.0
*/

class CEnvironmentalReverbProxy : public CEnvironmentalReverb,
					         	  public MEnvironmentalReverbCallback
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
		IMPORT_C static CEnvironmentalReverbProxy* NewL( TMMFMessageDestinationPckg aMessageHandler,
                     						        	 MCustomCommand& aCustomCommand,
                      						        	 CCustomInterfaceUtility* aCustomInterfaceUtility );
		/**
        *
        * Destructor
        */
		virtual ~CEnvironmentalReverbProxy();

	public: // functions from base class

		/**
		* From CAudioEffect
        * Apply effect settings
        * @since 3.0
        */
		IMPORT_C virtual void ApplyL();

		/**
        * Gets unique ID used by Adapation to identify stream specific Reverb
        * @since 3.1
        * @return ID
        */
		virtual TUint32 GetUniqueId() const;

		/**
        * Add the reference to Room Level object to array
        * @since 3.1
        * @return error/no error
        */
		virtual TInt RoomLevelAttached(CRoomLevelProxy& aProxy);


		/**
        * Remove the reference to Room Level object from array
        * @since 3.1
        * @return error/no error
        */

		virtual TInt RoomLevelDetached(CRoomLevelProxy& aProxy);


	public: // functions from MEnvironmentalReverbCallback

		/**
		* From MEnvironmentalReverbCallback
        * Changes to EnvironmentalReverb data has occured
        * @since 3.0
        * @param aBuffer Buffer containing serialized EnvironmentalReverb data
        */
		void EnvironmentalReverbEvent( const TDesC8& aDataPckg );


	private:
		/**
		* Private C++ constructor for this class.
        * @since	3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	-
        */
		CEnvironmentalReverbProxy(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
					         	  CCustomInterfaceUtility* aCustomInterfaceUtility);

		/**
		* Second phase constructor for this class.
        * @since	3.0
        * @return -
        */
		void ConstructL();

		/**
		* Start the active observer.
        * @since	3.0
		* @param -
        * @return -
        */
        void StartObserver();

	private:

		// Pointer to custom command utility
		MCustomCommand* iCustomCommand;
		// Message handler handle
		TMMFMessageDestinationPckg iMessageHandler;
	 	// EnvironmentalReverb Event Observer
		CEnvironmentalReverbEventObserver* iEnvironmentalReverbEventObserver;
		// Pointer to the custom interface utility
    	CCustomInterfaceUtility* iCustomInterfaceUtility;

        RPointerArray<CRoomLevelProxy> iCRoomLevelProxyList;

	};

#endif	// of CENVIRONMENTALREVERBPROXY_H

// End of File
