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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif
void winscw_workaround()
{
    fopen(0,0);
    fread(0,0,0,0);
    iscntrl(0);
    realloc(0,0);
    calloc(0,0);
}
#ifdef __cplusplus

}
#endif
