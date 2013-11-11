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

#include <AccessoryServer.h>
#include <AccessoryControl.h>
#include <AccPolGenericID.h>
#include <AccPolGenericIDarray.h>
#include <AccConfigFileParser.h>
#include <TfAccessoryTestControlTypes.h>
#include <AccessorySettings.h>

#ifndef __CASYSTUBSERVICE_H__
#define __CASYSTUBSERVICE_H__

struct TTFCapability
    {
    TUint32				   iCapability;
    TInt64                 iValue;
    TInt                   iTimeMs;//0 means that command is server synchronously
    };

		
class CASYStubService: public CBase
    {
public:
	
	CASYStubService();
    void ConstructL( void );    	 
       
    void Service( TInt aTestCaseID, TTFASYReference aMethod, TInt64 aParam1, TUint32& aParam2, TInt aTimeMs );
   
    TInt GetTestCaseID();
     
    TTFCapability GetAndRemoveCapability();
     
private: //methods

    ~CASYStubService( void );    
        
private: //data
    
    
    RAccessoryServer  iAccessoryServer;   	
    RAccessoryControl iAccessoryControl;
    TAccPolGenericID  iGenericId;
    TAccPolGIDHeader  iGenericIDHeader;  
    
    // From test driver!
    TInt iTestCaseID;
    // From test driver!
    RArray<TTFCapability> iCapabilityArray;
    };

#endif //__CASYSTUBSERVICE_H__
