/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Central place for nice debug-type macros & functions
*
*/


#ifndef DEBUG_H
#define DEBUG_H

//INCLUDES
#include <f32file.h>
#define FTRACE(a) {a;}

/**
* Declare the FPrint function
* @since Series 60 2.7
* @return none
*/
inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
{
    VA_LIST list;
    VA_START(list,aFmt);
    TInt tmpInt = VA_ARG(list, TInt);
    TInt tmpInt2 = VA_ARG(list, TInt);
    TInt tmpInt3 = VA_ARG(list, TInt);
    VA_END(list);
    #ifdef _DEBUG
    RDebug::Print(aFmt, tmpInt, tmpInt2, tmpInt3);
    #endif
}
#endif // DEBUG_H

// End of File
