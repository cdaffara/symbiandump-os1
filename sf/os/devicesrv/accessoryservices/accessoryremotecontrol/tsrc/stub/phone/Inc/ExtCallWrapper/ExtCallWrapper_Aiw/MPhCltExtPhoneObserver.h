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
* Description:  It defines the interface for the observers of 
*                the CPhCltExtPhoneBase.
*
*/


#ifndef MPHCLTEXTPHONEOBSERVER_H   
#define MPHCLTEXTPHONEOBSERVER_H   

// CLASS DECLARATION

/**
*  Defines the interface for the observers of the CPhCltExtPhoneBase.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class MPhCltExtPhoneObserver
    {          
    public:  // New functions   
        
        /**
        * It is called whenever client's dial request is completed.
        *
        * @param aStatus It is the status of the operation.
        *                <0 means Symbian OS error code.
        */       
        virtual void HandleDialL( const TInt aStatus ) = 0;
        
    };

#endif      // MPHCLTEXTPHONEOBSERVER_H   
            
// End of File
