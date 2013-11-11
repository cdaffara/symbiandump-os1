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

#ifndef __CBTASYSTUBOBSERVER_H__
#define __CBTASYSTUBOBSERVER_H__

#include <e32property.h>
#include "BTAsyStubService.h"
  	  
class CBTASYStubObserver : public CActive
    {
    public:  
        
		static CBTASYStubObserver* NewL(CBTASYStubService& aService); 
               		
        CBTASYStubObserver(CBTASYStubService& aService);                       
        	
        ~CBTASYStubObserver();                       
                
        void IssueRequest();
                        				
    public: 	
            
    protected:  
        void DoCancel();
	    void RunL();	    
	    TInt RunError( TInt aError );
	private:
      
      	RProperty iSubscribe;
      	CBTASYStubService* iService;
      	
    };
#endif //__CBTASYSTUBOBSERVER_H__
