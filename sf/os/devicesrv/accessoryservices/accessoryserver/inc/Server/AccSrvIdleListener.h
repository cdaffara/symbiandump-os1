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
* Description:  Idle state listener
*
*/



#ifndef ACCSRVIDLELISTENER_H
#define ACCSRVIDLELISTENER_H

// INCLUDES
#include "AsyProxyAPI.h"
#include "AccSrvSubBase.h"
#include <e32base.h>
#include <e32property.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccSrvServerModel;

// CLASS DECLARATION

/**
*  Idle state listener implementation.
*
*  AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvIdleListener ) : public CActive
    {
    public: // Constructors and destructor

        /**
        * Destructor.
        */
        ~CAccSrvIdleListener();

        /**
        * Two-phased constructor.
        */
        static CAccSrvIdleListener* NewL( CAccSrvServerModel* aServerModel);

    public: // New functions

    public: // Functions from base classes

    protected: // New functions

        /**
        * C++ default constructor.
        */
        CAccSrvIdleListener( CAccSrvServerModel* aServerModel );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();
        
    	CAccSrvIdleListener();        
    protected:  // Functions from base classes

		//from CActive
		void DoCancel();

		void RunL();	


    public:     // Friend classes

    protected:  // Friend classes
    
    private:
    

    private:    
    	//not owned
    	CAccSrvServerModel* iServerModel;

   		//owned 	
    	RProperty iProperty;	
    };

#endif // CACCSRVSUBCONNECTIONBASE_H

// End of File
