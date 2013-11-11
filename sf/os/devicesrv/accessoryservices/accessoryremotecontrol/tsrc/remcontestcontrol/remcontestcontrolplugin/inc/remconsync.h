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

#ifndef CREMCONSYNC_H
#define CREMCONSYNC_H

//  INCLUDES
#include <RemConInterfaceSelector.h>
#include <RemConSynchronizationTargetObserver.h>
#include <RemConSynchronizationTarget.h>
#include <RemConExtensionApi.h>

#include "mremconcontrol.h"
#include "mremconobserver.h"
#include "remconactive.h"
#include "remcontypes.h"
#include "remconcontrol.h"

class MRemConObserver;

class CRemConSync : public MRemConObserver, public MRemConSynchronizationTargetObserver
								
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRemConSync* NewL(CRemConInterfaceSelector* aTargetInterfaceSelector);
        
        /**
        * Destructor.
        */
        virtual ~CRemConSync();


		void SynchronizeResponse();
		
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

	 	/* Sync button*/
		void Synchronize();

    private:

        /**
        * C++ default constructor.
        */
        CRemConSync();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CRemConInterfaceSelector* aTargetInterfaceSelector);

    public:     
    
    protected:  
        
    private:
        CRemConActive* iActive;
		CRemConSynchronizationTarget* iSync;
		TInt iError;
		TInt iEvent;
		TInt iParameter;
		
    public: 
        
    protected:
   
    private:  

    };

#endif      //CREMCONSYNC_H
            
// End of File
