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

#ifndef CREMCONCORE_H
#define CREMCONCORE_H

//  INCLUDES
#include <RemConInterfaceSelector.h>
#include <RemConCoreApiTargetObserver.h>
#include <RemConCoreApiTarget.h>
#include <RemConCoreApiController.h>
#include <RemConCoreApiControllerObserver.h>

#include "mremconcontrol.h"
#include "mremconobserver.h"
#include "remconactive.h"
#include "remcontypes.h"
#include "remconcontrol.h"

class MRemConObserver;

class CRemConCore : public MRemConObserver, public MRemConCoreApiTargetObserver, public MRemConCoreApiControllerObserver
								
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRemConCore* NewL(CRemConInterfaceSelector* aTargetInterfaceSelector);
        
        /**
        * Destructor.
        */
        virtual ~CRemConCore();

		void Play();
		
		void Stop();
		
		void Cancel();
		
    public: // New functions
        
        //void RequestResponse(TXXXTestClientFunction aPoc);
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

	
	   void CompleteResponse( const TInt aError );
	            
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

	 	
    private:

        /**
        * C++ default constructor.
        */
        CRemConCore();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CRemConInterfaceSelector* aTargetInterfaceSelector);

    public:     
    
    protected:  
        
    private:

        CRemConActive* iActive;
    	CRemConCoreApiTarget* iCore;
		CRemConCoreApiController* iCoreController;
		CRemConInterfaceSelector* iTargetInterfaceSelector;
		
		TInt iEvent;
		TInt iParameter;
		
    public: 
        
    protected:
   
    private:  
    
    /**
		A command has been received. 
		@param aOperationId The operation ID of the command.
		@param aButtonAct The button action associated with the command.
		*/
		void MrccatoCommand(TRemConCoreApiOperationId aOperationId, 
			TRemConCoreApiButtonAction aButtonAct);
		
		/** 
		A 'play' command has been received. 
		@param aSpeed The playback speed.
		@param aButtonAct The button action associated with the command.
		*/
		void MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed, 
			TRemConCoreApiButtonAction aButtonAct);

		/**
		A 'tune function' command has been received.
		@param aTwoPart If EFalse, only aMajorChannel is to be used. Otherwise, 
		both aMajorChannel and aMinorChannel are to be used.
		@param aMajorChannel The major channel number.
		@param aMinorChannel The minor channel number.
		@param aButtonAct The button action associated with the command.
		*/
		void MrccatoTuneFunction(TBool aTwoPart, 
			TUint aMajorChannel, 
			TUint aMinorChannel,
			TRemConCoreApiButtonAction aButtonAct);

		/** 
		A 'select disk function' has been received.
		@param aDisk The disk.
		@param aButtonAct The button action associated with the command.
		*/
		void MrccatoSelectDiskFunction(TUint aDisk,
			TRemConCoreApiButtonAction aButtonAct);
		
		/** 
		A 'select AV input function' has been received.
		@param aAvInputSignalNumber The AV input.
		@param aButtonAct The button action associated with the command.
		*/
		void MrccatoSelectAvInputFunction(TUint8 aAvInputSignalNumber,
			TRemConCoreApiButtonAction aButtonAct);

		/** 
		A 'select audio input function' has been received.
		@param aAudioInputSignalNumber The audio input.
		@param aButtonAct The button action associated with the command.
		*/
		void MrccatoSelectAudioInputFunction(TUint8 aAudioInputSignalNumber,
			TRemConCoreApiButtonAction aButtonAct);
		

		void Response(TRemConCoreApiOperationId aOperationId, TInt aError);
       /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();


    };

#endif      //CREMCONCORE_H
            
// End of File
