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
* Description: Audio Output Proxy header
*
*/


#ifndef CAUDIOOUTPUTPROXY_H
#define CAUDIOOUTPUTPROXY_H

//  INCLUDES
#include "AudioOutput.h"
#include <mmf/common/mmfcontrollerframework.h>
#include <MCustomCommand.h>

// FORWARD DECLARATIONS
class CCustomCommandUtility;
class CCustomInterfaceUtility;
class CAudioOutputProxyAO;
// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*/
class CAudioOutputProxy: public CAudioOutput
	{
	friend class CAudioOutput;
    public:  // Constructors and destructor

		IMPORT_C static CAudioOutputProxy* NewL(TMMFMessageDestinationPckg aMessageHandler, 
												MCustomCommand& aCustomCommand, 
												CCustomInterfaceUtility* aCustomInterfaceUtility);
		
		/**
		* Destructor.
		*/
		virtual ~CAudioOutputProxy();

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		IMPORT_C static const TUid Uid();
		
		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		virtual TAudioOutputPreference AudioOutput();

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		virtual TAudioOutputPreference DefaultAudioOutput();

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		virtual void RegisterObserverL( MAudioOutputObserver& aObserver );

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		virtual TBool SecureOutput();

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		virtual void SetAudioOutputL( TAudioOutputPreference aAudioOutput = ENoPreference );

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		virtual void SetSecureOutputL( TBool aSecureOutput = EFalse );

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		virtual void UnregisterObserver( MAudioOutputObserver& aObserver );

	protected:
		/**
		* C++ default constructor.
		*/
		CAudioOutputProxy(TMMFMessageDestinationPckg aMessageHandler,
								MCustomCommand& aCustomCommand, 
								CCustomInterfaceUtility* aCustomInterfaceUtility);

	private:
		/**
		* EPOC constructor.
		*/
		void ConstructL();

    private:    // Data
		MCustomCommand*                	      iCustomCommand;
		TAudioOutputPreference                iDefaultOutput;
		TAudioOutputPreference                iOutput;
		TBool                                 iSecureOutput;
		MAudioOutputObserver*		  		  iObserver;
		TMMFMessageDestinationPckg            iMessageHandler;
		CAudioOutputProxyAO*				  iAsyncSender;
		TBool								  iRegistered;
		CCustomInterfaceUtility* 			  iCustomInterfaceUtility;

	};

#endif      // CAUDIOINPUTPROXY_H
            
// End of File
