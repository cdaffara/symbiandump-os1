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

#ifndef __CREMCONPSPCHOBSERVER_H__
#define __CREMCONPSPCHOBSERVER_H__

#include <e32base.h>

#include <e32property.h>
#include "mremconcontrol.h"

class MRemConPCHObserver;

class CRemConPSPCHObserver : public CActive
    {
    public:  
        
		static CRemConPSPCHObserver* NewL(); 
               		
        CRemConPSPCHObserver();                       
        	
        ~CRemConPSPCHObserver();                       
                
        void Start();
                        				
    public: 	
            
    protected:  
        void DoCancel();
	    void RunL();	    
	    TInt RunError( TInt aError );
	private:
      
      	RProperty iSubscribe;
      	
      	//MRemConPCHObserver& iPCHObserver;
      	
      	MRemConControl* iControl;
      	
      	TInt iEvent;
    };

    	  
    
#endif //__CREMCONPSPCHOBSERVER_H__
