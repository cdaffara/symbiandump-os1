/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Proxy class for handling client requests
*
*/



#ifndef TELEPHONYAUDIOROUTINGPROXY_H
#define TELEPHONYAUDIOROUTINGPROXY_H

//  INCLUDES
#include <e32base.h>
#include  <e32std.h>
#include "TelephonyAudioRouting.h"
#include "TelephonyAudioRoutingSession.h"


// CLASS DECLARATION

/**
*  Defines client side API for telephony audio routing requests.
*  
* @lib TelephonyAudioRouting.lib
* since Series 60 3.1
*/
class CTelephonyAudioRoutingProxy: public CTelephonyAudioRouting
	{
	
    public:  // Constructors and destructor

		IMPORT_C static CTelephonyAudioRoutingProxy* NewL(MTelephonyAudioRoutingObserver& aObserver);
		
		/**
		* Destructor.
		*/
		virtual ~CTelephonyAudioRoutingProxy();

		/**
		* Provide array of available outputs
		* since Series 60 3.1
		* @param none
		* @return const TArray<TAudioOutput>: array of available outputs
		*/
		virtual const TArray<TAudioOutput>& AvailableOutputs();
		
		/**
		* Provide current audio output
		* since Series 60 3.1
		* @param none
		* @return TAudioOutput: audio output
		*/
		virtual TAudioOutput Output();

	
		/**
		* Request to set the current output
		* since Series 60 3.1
		* @param aOutput: desired output value
		* @return void
		*/
		virtual void SetOutputL( TAudioOutput aOutput = ENotActive);
		
		/**
		* Provide previous output value
		* since Series 60 3.1
		* @param none
		* @return TAudioOutput: previous output value.
		*/
		virtual TAudioOutput PreviousOutput();
		
	
		/**
		* Request to set the ShowNote flag       
       	* @since Series 60 3.1
        * @param TBool aMode Show Note mode. ETrue or EFalse.
        * @return KErrNone if succesful, otherwise system-wide error code.
        */
        virtual TInt SetShowNote( TBool aMode );
        
       	/**
        * Returns the the Show Note mode for the last succesfully completed
        * SetOutputL() request. 
       	* @since Series 60 3.1
        * @param TBool aMode. On return of this function will contain
        *        Show Note mode. EFalse or ETrue.
        * @return KErrNone if succesful, otherwise system-wide error code.
        */
        virtual TInt GetShowNote( TBool& aMode );
               
       				
	private:

		/**
		* C++ default constructor.
		*/
		CTelephonyAudioRoutingProxy(MTelephonyAudioRoutingObserver& aObserver);
		
		/**
		* EPOC constructor.
		*/
		void ConstructL();
		TBool ValidateOutputRequest(TAudioOutput aOutput);
		TBool ShowNoteMode();				
		
    private:    // Data

		MTelephonyAudioRoutingObserver&		iObserver;
		RTelephonyAudioRoutingSession 		iSession;
		TBool								iShowNote;
		TArray<CTelephonyAudioRouting::TAudioOutput> iAvailableOutputs;

			
	};

#endif      // TELEPHONYAUDIOROUTINGPROXY_H

           
// End of File
