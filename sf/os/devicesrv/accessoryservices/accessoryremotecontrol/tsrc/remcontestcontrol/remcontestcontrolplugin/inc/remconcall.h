/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/

#ifndef CREMCONCALL_H
#define CREMCONCALL_H

//  INCLUDES
#include <RemConInterfaceSelector.h>
#include <RemConCallHandlingTargetObserver.h>
#include <RemConCallHandlingTarget.h>
#include <RemConExtensionApi.h>

#include "mremconcontrol.h"
#include "mremconobserver.h"
#include "remconactive.h"
#include "remcontypes.h"
#include "remconcontrol.h"

class MRemConObserver;

class CRemConCall : public MRemConObserver, public MRemConCallHandlingTargetObserver
								
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRemConCall* NewL(CRemConInterfaceSelector* aTargetInterfaceSelector);
        
        /**
        * Destructor.
        */
        virtual ~CRemConCall();
		
    public: // New functions
        
        //void RequestResponse(TXXXTestClientFunction aSync);
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

	
	   void CompleteResponse( const TInt aError );
		void GetCommandSourceInfo(TRemConExtCmdSource& aSource);	   
	            
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

	/**
	* Answer an incoming phone call.
	*/
	void AnswerCall();

	/**
	* End an incoming/ongoing phone call.
	*/
	void EndCall();
	
	/**
	* End an incoming/ongoing phone call.
	*/
	void AnswerEndCall();
	
	/**
	* Make a voice call.
	*/
	void VoiceDial( const TBool aActivate );
	
	/**
	* Redial last phone call.
	*/
	void LastNumberRedial();
	
	/**
	* Dial a phone call.
	*/
	void DialCall( const TDesC8& aTelNumber );
	
	/**
	* Make multiparty call.
	*/
	void MultipartyCalling( const TDesC8& aData );
	
	/**
	* Generates DTMF signal.
	*/
	void GenerateDTMF( const TChar aChar );
	
	/**
	* Make a speed dial call.
	*/
	void SpeedDial( const TInt aIndex );

    private:

        /**
        * C++ default constructor.
        */
        CRemConCall();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CRemConInterfaceSelector* aTargetInterfaceSelector);

    public:     
    
    protected:  
        
    private:
        CRemConActive* iActive;
		CRemConCallHandlingTarget* iCall;
		TInt iError;
		TInt iEvent;
		TInt iParameter;
		
    public: 
        
    protected:
   
    private:  

    };

#endif      //CREMCONCALL_H
            
// End of File
