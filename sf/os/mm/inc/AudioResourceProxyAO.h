/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the class handles Message Handling between Audio Resrouce
*                and Adaption layer
*
*
*/

#ifndef CAUDIORESOURCEPROXYAO_H
#define CAUDIORESOURCEPROXYAO_H

//  INCLUDES
#include <e32base.h>
#include <mmfcontrollerframework.h>
#include <MCustomCommand.h>

// FORWARD DECLARATIONS
class CCustomCommandUtility;
class CAudioResourceUtilityProxy;
//class CAudioOutput;

// CLASS DECLARATION

/**
*  This is the Audio Resource proxy Active object class responsible for handling callback messages.
*
*  @lib AudioResourceProxy.lib
*  @since 3.0
*/
class CAudioResourceProxyAO : public CActive
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CAudioResourceProxyAO* NewL(CAudioResourceUtility *aResourceProxy,
        								MAudioResourceUtilityObserver& aObserver,
        								MCustomCommand* aUtility);

        virtual ~CAudioResourceProxyAO();

		/**
        * Send Asyncronous Message
        * @since 3.0
        */
		void SendAsyncMessage(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt aFunction );

		void SetRegisterFlag(TBool aFlag);

    private:

        /**
        * C++ default constructor.
        */
        CAudioResourceProxyAO(CAudioResourceUtility *aResourceProxy,
        					MAudioResourceUtilityObserver& aObserver,
        					MCustomCommand* aUtility);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        void RunL();

        void DoCancel();

	private:
		CAudioResourceUtility*				iAudioResourceProxy;
		MAudioResourceUtilityObserver&		iObserver;
		MCustomCommand*				iCustomCommandUtility;
		const TMMFMessageDestinationPckg*				iDestination;
		TInt											iFunction;
		TBool											iRegistered;

    };

#endif 		// CAUDIORESOURCEPROXYAO_H