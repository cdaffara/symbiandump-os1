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
* Description:  CDSY message structures.
*
*/


#ifndef CDSYMESSAGESTRUCTURES_H
#define CDSYMESSAGESTRUCTURES_H


// INCLUDES
#include <PSVariables.h>
#include <DosSvrServices.h>



// CONSTANTS
// MACROS
// DATA TYPES


// Generic message data structures
typedef struct
    {
    TInt                    iValue;
    } TCDSYMsgDataValue;

typedef struct
    {
    TInt                    iValue;
    TInt                    iReturn;
    } TCDSYMsgDataReturnValue;

typedef struct
    {
    TInt                    iReturn;
    } TCDSYMsgDataReturn;

//Extension
class MDosExtensionBaseDSY;

typedef struct 
    {
    MDosExtensionBaseDSY*   iDosExtensionBaseDSY;
    } TCDSYMsgDataExtensionInitialization;


// Events
typedef struct
    {
    TInt                    iParameter;
    } TCDSYMsgDataEvent;

typedef struct
    {
    TInt                    iEvent;
    TAny*                   iParameter;
    TInt                    iParameterSize;
    } TCDSYMsgDataGeneralEvent;


// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

#endif // CDSYMESSAGESTRUCTURES_H

//  End of File
