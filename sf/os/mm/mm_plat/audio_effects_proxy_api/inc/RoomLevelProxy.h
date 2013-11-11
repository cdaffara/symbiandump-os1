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
* Description:  This is the definition of the Room Level proxy class.
*
*
*/


#ifndef CROOMLEVELPROXY_H
#define CROOMLEVELPROXY_H

// INCLUDES
#include <e32base.h>
#include <RoomLevelBase.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <CustomCommandUtility.h>
#include "RoomLevelMessageTypes.h"
#include <MRoomLevelObserver.h>
#include <MAudioEffectObserver.h>

// FORWARD DECLARATION
class CRoomLevelEventObserver;
//class CEnvironmentalReverbProxy;

// CLASS DECLARATION

/**
*  Interface class to be implemented by objects that receives callbacks from Room Level Event Observer.
*
*  @since 3.0
*/
class MRoomLevelCallback
    {
    public:

        /**
        * Invoked by the RoomLevel Event Observer when the RoomLevel object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the RoomLevel data
        */
        virtual void RoomLevelEvent( const TDesC8& aDataPckg ) = 0;

    };

/**
*  This is the RoomLevel effect proxy class responsible for handling framework messages.
*
*  @lib RoomLevelProxy.lib
*  @since 3.0
*/

class CRoomLevelProxy : public CRoomLevel,
					    public MRoomLevelCallback
	{
	public:		// Constructors and destructor

	    /**
        *
        * Factory function for creating the RoomLevel proxy object.
        * @since 3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	pointer to a RoomLevel proxy object
        */
		IMPORT_C static CRoomLevelProxy* NewL( TMMFMessageDestinationPckg aMessageHandler,
                     						        	 MCustomCommand& aCustomCommand,
                      						        	 CCustomInterfaceUtility* aCustomInterfaceUtility );
		/**
        *
        * Destructor
        */
		virtual ~CRoomLevelProxy();

	public: // functions from base class

		/**
		* From CAudioEffect
        * Apply effect settings
        * @since 3.0
        */
		IMPORT_C virtual void ApplyL();


	public: // functions from MRoomLevelCallback

		/**
		* From MRoomLevelCallback
        * Changes to RoomLevel data has occured
        * @since 3.0
        * @param aBuffer Buffer containing serialized RoomLevel data
        */
		void RoomLevelEvent( const TDesC8& aDataPckg );


		/**
		* Set reverb unique ID
        *
        * @since 3.0
        * @param aReverb A reference to the client reverb object
        */
		virtual TInt AttachReverb(CEnvironmentalReverb& aReverb);

		/**
		* Delete related message handler and custom interface
        *
        * @since 3.0
        * @param aReverb A reference to the client reverb object
        */
		virtual TInt DettachReverb(CEnvironmentalReverb& aReverb);



	private:

		/**
		* Private C++ constructor for this class.
        * @since	3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	-
        */
		CRoomLevelProxy(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
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
	 	// RoomLevel Event Observer
		CRoomLevelEventObserver* iRoomLevelEventObserver;
		// Pointer to the custom interface utility
    	CCustomInterfaceUtility* iCustomInterfaceUtility;

	};

#endif	// of CROOMLEVELPROXY_H

// End of File
