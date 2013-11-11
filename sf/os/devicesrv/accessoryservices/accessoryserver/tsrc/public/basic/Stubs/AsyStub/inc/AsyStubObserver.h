/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __CASYSTUBOBSERVER_H__
#define __CASYSTUBOBSERVER_H__

#include <e32property.h>
#include "AsyStubService.h"
  	  
class CASYStubObserver : public CActive
    {
    public:  
        
		static CASYStubObserver* NewL(CASYStubService& aService); 
               		
        CASYStubObserver(CASYStubService& aService);                       
        	
        ~CASYStubObserver();                       
                
        void IssueRequest();
                        				
    public: 	
            
    protected:  
        void DoCancel();
	    void RunL();	    
	    TInt RunError( TInt aError );
	private:
      
      	RProperty iSubscribe;
      	CASYStubService* iService;
      	
    };
#endif //__CASYSTUBOBSERVER_H__
