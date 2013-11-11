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


#ifndef __CSENSRVTESTOBSERVER_H__
#define __CSENSRVTESTOBSERVER_H__

#include <e32property.h>
#include <sensrvproperty.h>
#include <sensrvchannelinfo.h>

class CSensrvChannel;
  	  
class CSensrvTestObserver : public CActive
    {
    public:  
        
		static CSensrvTestObserver* NewL(TInt aProcessID); 
        static CSensrvTestObserver* NewLC(TInt aProcessID);    
                   		
        CSensrvTestObserver(TInt aProcessID);                       
        	
        ~CSensrvTestObserver();                       
                
        void IssueRequest();
                        				
    public: 	
            
    protected:  
        void DoCancel();
	    void RunL();	    
	    TInt RunError( TInt aError );
	    
	private:
	
	    void DoSetPropertyL( TInt aExpectedResult, TSensrvProperty& aProperty );
	    void DoOpenChannelL( TInt aExpectedResult );
	    void DoCloseChannelL( TInt aExpectedResult );
	    void DoFindChannelL();
	    TInt CheckFindResult( TSensrvChannelInfo& aSearchConditions, RSensrvChannelInfoList aChannelInfoList );

	
	private:
      
      	RProperty       iProperty;   
      	TInt            iMyProcessID;   
      	CSensrvChannel* iSensorChannel;//own	
      	
    };
#endif //__CSENSRVTESTOBSERVER_H__
