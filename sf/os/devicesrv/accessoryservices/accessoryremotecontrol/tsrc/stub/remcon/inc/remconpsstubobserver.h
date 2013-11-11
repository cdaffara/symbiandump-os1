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

#ifndef __CREMCONPSSTUBOBSERVER_H__
#define __CREMCONPSSTUBOBSERVER_H__

#include <e32base.h>

#include <e32property.h>

class MRemConStubObserver;

class CRemConPSstubObserver : public CActive
    {
    public:  
        
		static CRemConPSstubObserver* NewL(MRemConStubObserver& aObserver, TInt aTFControlKey); 
               		
        CRemConPSstubObserver(MRemConStubObserver& aObserver, TInt aTFControlKey);                       
        	
        ~CRemConPSstubObserver();                       
                
        void Start();
                        				
    public: 	
            
    protected:  
        void DoCancel();
	    void RunL();	    
	    TInt RunError( TInt aError );
	private:
      
      	RProperty iSubscribe;
      	
      	MRemConStubObserver& iObserver;
      	
      	TInt iTFControlKey;
      	
    };

    	  
    
#endif //__CREMCONPSSTUBOBSERVER_H__
