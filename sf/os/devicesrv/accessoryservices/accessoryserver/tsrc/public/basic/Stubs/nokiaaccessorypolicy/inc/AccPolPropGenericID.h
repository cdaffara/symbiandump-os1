/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  These definitions define the proprietary Generic ID features.
*
*/



#ifndef ACCPOLPROPGENERICID_H
#define ACCPOLPROPGENERICID_H

#include <AccPolGenericid.h>

/**
*  Proprietary Generic ID bit mask definitions.
*  These definitions define the proprietary Generic ID features, e.g.
*   -Device type bit mask definitions
*   -Physical connection bit mask definitions
*   -Application protocol bit mask definitions
*   -Subblock definitions
* 
*  Proprietary NAME-VALUE PAIRS are defined in 
*  AccPolProprietaryNameValuePairs.h-file.
*/

// Device type bit mask definitions

// Physical connection bit mask definitions
const TUint32 KPCNokiaPopPort           = 0x10000; // 00000 00010 00000 00000 00000
const TUint32 KPCNokiaAV                = 0x20000; // 00000 00100 00000 00000 00000

// Application protocol bit mask definitions

// Nokia Audio capability group                                                                   
const TUint64 KCGNokiaAudio 	        = 0x100000000;	// 00000 00100 00000 00000 00000 00000 00000 00000
 
// Nokia Carkit capability group 
const TUint64 KCGNokiaCarkit 	        = 0x200000000;	// 00000 01000 00000 00000 00000 00000 00000 00000

#endif      // ACCPOLPROPGENERICID  
            
// End of File
