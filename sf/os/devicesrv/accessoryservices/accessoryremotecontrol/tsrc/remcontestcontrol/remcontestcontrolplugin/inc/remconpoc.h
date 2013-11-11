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

#ifndef CREMCONPOC_H
#define CREMCONPOC_H

//  INCLUDES
#include <RemConInterfaceSelector.h>
#include <RemConPocTargetObserver.h>
#include <RemConExtensionApi.h>

#include "mremconcontrol.h"
#include "mremconobserver.h"
#include "remconactive.h"
#include "remcontypes.h"
#include "remconcontrol.h"

class MRemConObserver;

class CRemConPoc : public MRemConObserver, public MRemConPocTargetObserver
								
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRemConPoc* NewL(CRemConInterfaceSelector* aTargetInterfaceSelector);
        
        /**
        * Destructor.
        */
        virtual ~CRemConPoc();


		void APOCButtonDownResponse();
		
		void APOCButtonUpResponse();
		
		void GetCommandSourceInfo(TRemConExtCmdSource& aSource);
		
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

	 	/* POC button*/
		void APOCButtonDown();
		void APOCButtonUp();
       /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CRemConPoc();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CRemConInterfaceSelector* aTargetInterfaceSelector);

    public:     
    
    protected:  
        
    private:
        
        CRemConActive* iActive;
    	CRemConPocTarget* iPoc;
		//MRemConControl* iControl;
		TInt iError;
		TInt iEvent;
		TInt iParameter;
		
    public: 
        
    protected:
   
    private:  

    };

#endif      //CREMCONPOC_H
            
// End of File
