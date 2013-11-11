/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Audio Output Message Handler
*
*/


#ifndef CAUDIOOUTPUTMESSAGEHANDLER_H
#define CAUDIOOUTPUTMESSAGEHANDLER_H

//  INCLUDES
#include <mmf/common/mmfcontroller.h>
#include <MAudioOutputObserver.h>

// FORWARD DECLARATIONS
class CAudioOutput;
class CAudioOutputMessageHandlerAO;
// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*/
class CAudioOutputMessageHandler: public CMMFObject,
									public MAudioOutputObserver
	{
    public:  // Constructors and destructor
        IMPORT_C static CAudioOutputMessageHandler* NewL(TAny*                customInterface,
                                                         CMMFObjectContainer& aContainer);

		/**
		* Destructor.
		*/
		virtual ~CAudioOutputMessageHandler();

		IMPORT_C static TUid Uid();

		/**
		* ?member_description.
		* @param ?arg1 ?description
		* @return ?description
		*/
		virtual void HandleRequest(TMMFMessage& aMessage);        

	private:

		/**
		* C++ default constructor.
		*/
		CAudioOutputMessageHandler(CAudioOutput*        aAudioOutput,
		                           CMMFObjectContainer& aContainer);

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		void DoHandleRequestL(TMMFMessage& aMessage);

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		void DoDeleteL(TMMFMessage& aMessage);

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		void DoRegisterObserverL();

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		void DoSetAudioOutputL(TMMFMessage& aMessage);

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		void DoSetSecureOutputL(TMMFMessage& aMessage);

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		void DoUnregisterObserverL(TMMFMessage& aMessage);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		void DefaultAudioOutputChanged( CAudioOutput&                        aAudioOutput,
		                                CAudioOutput::TAudioOutputPreference aNewDefault );
		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aMessage 
        * @return void
        */
		void DoGetAudioOutputL(TMMFMessage& aMessage);

    private:    // Data
        CAudioOutput*        iAudioOutput;
        CMMFObjectContainer& iContainer;
		TMMFMessage*		 iMessage;
		TMMFMessage*		 iCallbackMessage;
		CAudioOutputMessageHandlerAO* iAudioOutputMessageHandlerAO;
		TInt iCount;


	};

#endif      // CAUDIOOUTPUTMESSAGEHANDLER_H
            
// End of File
