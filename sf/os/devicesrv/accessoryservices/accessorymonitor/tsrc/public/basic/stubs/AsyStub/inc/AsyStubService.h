/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Used as an reference implementation
*
*/


#include <AccessoryServer.h>
#include <AccessoryControl.h>
#include <AccPolGenericID.h>
#include <AccPolGenericIDarray.h>
#include <AccConfigFileParser.h>
#include <AccessorySettings.h>
#include "AccMonConnetionConsts.h"
#include "AccMonitorCapMapper.h"

#ifndef CASYSTUBSERVICE_H
#define CASYSTUBSERVICE_H

struct TTFCapability
    {
    TUint32 iCapability;
    TInt64 iValue;
    TInt iTimeMs;//0 means that command is server synchronously
    };

// Forward declaration
class CAccMonitoringTestModule;
class CAccMonitorCapMapper;

/**
*  This class is used to emulate accessory connections.
*/
class CASYStubService
    {
public:
  
    CASYStubService();
    void ConstructL( void );       
       
    void Service( TTFASYReference aMethod, TInt64 aParam1, RArray<TUint32> aCapabilityArray );
   
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

#endif //CASYSTUBSERVICE_H
