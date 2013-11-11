/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  AudioOutput Proxy Active Object
*
*/

#ifndef CAUDIOOUTPUTPROXYAO_H
#define CAUDIOOUTPUTPROXYAO_H

//  INCLUDES
#include <e32base.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <MCustomCommand.h>
// FORWARD DECLARATIONS
class CCustomCommandUtility;
class CAudioOutputProxy;
//class CAudioOutput;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*/
class CAudioOutputProxyAO : public CActive
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CAudioOutputProxyAO* NewL(CAudioOutput *aOutputProxy,MAudioOutputObserver& aObserver,MCustomCommand* aUtility);

        virtual ~CAudioOutputProxyAO();

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aDestination Handle of the message handler
        * @param aFunction Message handler function
        * @return void
        */
		void SendAsyncMessage(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt aFunction );

		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aFlag EFalse=unregistered, ETrue=registered
        * @return void
        */
		void SetRegisterFlag(TBool aFlag);
		
		/**
        * ?member_description.
        * @since Series 60 3.0
        * @param aObserver Observer Instance
        * @return void
        */
		void SetObserver(MAudioOutputObserver& aObserver);


    private:

        /**
        * C++ default constructor.
        */
        CAudioOutputProxyAO(CAudioOutput *aOutputProxy,MAudioOutputObserver& aObserver,MCustomCommand* aUtility);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		// Functions from base classes

        /**
        * From CActive ?member_description
        */
        void RunL();

        /**
        * From CActive ?member_description
        */
        void DoCancel();

	private:
		CAudioOutput*									iAudioOutputProxy;
		MCustomCommand*							iCustomCommandUtility;
		TPckgBuf<CAudioOutput::TAudioOutputPreference>	iCallbackData;
		MAudioOutputObserver*							iObserver;
		const TMMFMessageDestinationPckg*				iDestination;
		TInt											iFunction;
		TBool											iRegistered;


    };

#endif 		// CAUDIOOUTPUTPROXYAO_H