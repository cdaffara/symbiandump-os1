/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DLL entry function
*
*/

#include <e32std.h>


#ifndef EKA2
GLDEF_C TInt E32Dll(  /*TDllReason aReason*/ )
    {
	LBSLOG(ELogP1, "E32Dll() Begin and End\n");
    return( KErrNone );
    }
#endif
