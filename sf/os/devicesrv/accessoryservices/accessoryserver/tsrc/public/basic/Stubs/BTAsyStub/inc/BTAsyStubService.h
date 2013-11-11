/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <AccessoryServer.h>
#include <AccessoryControl.h>
#include <AccPolGenericID.h>
#include <AccPolGenericIDarray.h>
#include <AccConfigFileParser.h>
#include <TfAccessoryTestControlTypes.h>
#include <AccessorySettings.h>
#include <AccessoryBTControl.h>

#ifndef __CBTASYSTUBSERVICE_H__
#define __CBTASYSTUBSERVICE_H__
		
class CBTASYStubService
    {
public:
	
	CBTASYStubService();
    void ConstructL( void );    	 
       
    void Service( TTFASYReference aMethod, 
                  TInt64 aParam, 
                  TInt aTestCaseID,
                  TAccPolGenericID aGenericID,
                  TUint32& aParam2 );
   
     TInt GetTestCaseID();
private: //methods

    ~CBTASYStubService( void );    
        
private: //data
    
    TInt iTestCaseID;
    RAccessoryServer 							iAccessoryServer;   	
        
    TAccPolGenericID							iGenericId;
    };

#endif //__CBTASYStubService_H__
