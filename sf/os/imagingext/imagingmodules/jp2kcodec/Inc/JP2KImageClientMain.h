/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  All used panics are defined in this header file.
*
*/


#ifndef __JP2IMAGECLIENTMAIN_H__
#define __JP2IMAGECLIENTMAIN_H__

//  INCLUDES
#include <e32std.h>

// CONSTANTS

// MACROS

// DATA TYPES

// To keep analogy between ICL panics and JPEG 2000 plug-in
// panics, we use same enumerations and same panics if needed.
enum TIclPanic
    {
    EFrameNumberOutOfRange = 14, //lint !e769 Referenced in ASSER ALWAYS macro
    EHeaderProcessingNotComplete = 17, //lint !e769 Referenced in ASSER ALWAYS macro
    ECommentNumberOutOfRange = 18, //lint !e769 Referenced in ASSER ALWAYS macro
    EInvalidState = 28 //lint !e769 Referenced in ASSER ALWAYS macro
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// Global panic function
// -----------------------------------------------------------------------------
//
GLDEF_C void Panic( TIclPanic aError );

#endif // __JP2IMAGECLIENTMAIN_H__
