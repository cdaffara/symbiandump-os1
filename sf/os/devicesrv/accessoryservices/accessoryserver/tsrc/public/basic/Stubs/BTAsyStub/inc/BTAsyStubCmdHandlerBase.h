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


#ifndef __CBTASYSTUBCMDHANDLERBASE_H_
#define __CBTASYSTUBCMDHANDLERBASE_H_

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "AsyCmdTypes.h"
#include <AsyCommandHandlerBase.h>

#ifdef __WINS__
static 	TInt iDistributor = 0;
#endif

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Process request command from AccessoryServer
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CBTASYStubCmdHandlerBase : public CASYCommandHandlerBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aCmdService callback to ASY Command Handler.
		* @return The created object.
        */
        static CBTASYStubCmdHandlerBase* NewL();
        
        /**
        * Destructor.
        */
        ~CBTASYStubCmdHandlerBase();

    public: // From base class
        
		/**
        * Processes request command from server 
        * @since Series ?XX ?SeriesXX_version
        * @param aCommand
        * @param aTrId
        * @param aData
        * @return void
        */
        void ProcessCommandL( const TProcessCmdId aCommand, const TASYCmdParams& aCmdParams );

    private: //functions
             
		/**
        * C++ default constructor.
        * @param aParams Initialisation parameters.
        */
   		CBTASYStubCmdHandlerBase();

    private:    // Data
    
    	//TBool iAudioOutConnector;
    	//TInt  iCase_55_counter;
    	
    };

#endif // CBTASYSTUBCMDHANDLERBASE_H_
