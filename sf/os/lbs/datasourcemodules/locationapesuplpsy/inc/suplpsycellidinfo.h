/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of TCellIdInfo.
*
*/

#ifndef SUPLPSYCELLIDINFO_H
#define SUPLPSYCELLIDINFO_H

// INCLUDES


/**
*  structure to store the Cell Id information	
*  @lib nlacidsuplpsy.lib
*  @since S60 v3.1
*/

struct TCellIdInfo
    {
    TInt iMNC;
    TInt iMCC;
    TInt iCid;
    TInt iLac; 
    };

#endif      // SUPLPSYCELLIDINFO_H
            
// End of File
