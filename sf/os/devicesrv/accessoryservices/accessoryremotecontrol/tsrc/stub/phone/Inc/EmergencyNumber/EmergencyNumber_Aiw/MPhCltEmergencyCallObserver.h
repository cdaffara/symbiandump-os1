/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/


#ifndef MPHCLTEMERGENCYCALLOBSERVER_H   
#define MPHCLTEMERGENCYCALLOBSERVER_H   

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  Defines the interface for the observers of the CPhCltEmergencyCall.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class MPhCltEmergencyCallObserver
    {          
    public:  // New functions   
        
        /**
        * It is called whenever client's dial request is completed.
        *
        * @param aStatus error code of the success of the operation.
        */       
        virtual void HandleEmergencyDialL( const TInt aStatus ) = 0;
        
    };

#endif      // MPHCLTEMERGENCYCALLOBSERVER_H   
            
// End of File
