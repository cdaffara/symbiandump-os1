/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMCMN_H
#define HWRMCMN_H

// INCLUDES
//#include <e32std.h>

/**
* Data structure used to handle light status information.
*/ 
struct THWRMStatusInfo
    {
    // Light target.
    TInt iTarget;
    
    // Status of the target.
    TInt iStatus;
    };
    
/**
* Defines THWRMStatusInfo array. 
*/
typedef RArray<THWRMStatusInfo> RLightStatusArray;

#endif      // HWRMCMN_H

// End of File

