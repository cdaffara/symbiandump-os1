/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Definition of DosServer controller constants
*
*/


#ifndef __TFDOSTYPES_H__
#define __TFDOSTYPES_H__

#include <e32base.h>

const TInt KLDSYDummyEvent= 666;

enum TTFDosFunction
    {
    ETFDosFunction_None                             = 0,    
    ETFDosSharedData_Open                           = 80,
    ETFDosSharedData_Close                          = 81,
    ETFDosSharedData_FillDisk                       = 82,
    ETFDosSharedData_ReleaseDisk                    = 83,
    ETFDosSharedData_CheckDiskSpace                 = 84,
    ETFDosSharedData_RequestFreeDiskSpace           = 85,
    ETFDosSharedData_RequestFreeDiskSpaceCancel     = 86,
    ETFDosExtension_Open					        = 87,
    ETFDosExtension_CallFunction                    = 88,
    ETFDosExtension_CallFunctionAndComplete         = 89,
    ETFDosExtension_Close                           = 99,         
    ETFDosEvent_Synchronized                        = 100,
    ETFRFs_Connect		                            = 116,
    ETFRFs_Close		                            = 117,    
    ETFDosExtension_GeneralEvent                    = 118,
    ETFDosExtension_GeneralEventArray               = 119
    };


enum TTFDosEvent
    {
    ETFDosEvent_None                                = 0,      
    ETFDOSDummyEvent                                = 666,
    };


enum TTFDosFlags
    {
    ETFDosFlags_None                                = 0x00,
    ETFDosFlags_IgnoreParameters                    = 0x01,
    ETFDosFlags_IgnoreEventParameters               = 0x02,
    ETFDosFlags_IgnoreResult                        = 0x04,
    ETFDosFlags_IgnoreUnexpectedEvents              = 0x08,
    ETFDosFlags_StoreParameters                     = 0x10,
    ETFDosFlags_UseParameters                       = 0x20,
    ETFDosFlags_WrongCapabilitySet                  = 0x30
    };

    
struct TTFDosServerControlTestCaseState
    {
    TTFDosFunction          iDosFunction;
    TInt                    iArg1;
    TInt                    iArg2;
    TInt                    iExpectedResult;
    TTFDosEvent             iCompletionEvent;
    TInt                    iStateFlags;
    TInt					iArg3;
    };    

const TInt KTFErrDosServiceMissing             = -4001;
const TInt KTFErrDosNoSyncEnd                  = -4002;
const TInt KTFErrDosUnexpectedEvent            = -5000;
const TInt KTFErrDosUnexpectedEventParameter   = -6000;
const TInt KTFErrDosUnexpectedArg1             = -7000;
const TInt KTFErrDosUnexpectedArg2             = -8000;
const TInt KTFErrDosUnexpectedResult           = -9000;

#endif
