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
 @publishedPartner
 @released
*/
#ifndef MMTSY_NAMES_H
#define MMTSY_NAMES_H

//  INCLUDES
#include <e32std.h>


// CONSTANTS

//Name definitions. 
    // These names must be used by clients when they try to open different 
    // modules, lines or systems from the MMTSY. If a client tries to open a 
    // line, the line has also to be supported (see above, Supported lines
    // and modules).
     
     /**
     * MMTSY module name. This name must be used by client when is loads the 
     * phone module using RTelServer's LoadPhoneModule method. If any other 
     * name is used by the client than the one defined below, the TSY will 
     * not be loaded.
     */     
    _LIT( KMmTsyModuleName, "PhoneTsy"); 

    /**
     * MMTSY currently supports only one phone module. The client must use the 
     * name defined below when opening a phone module. Inside the TSY this  
     * value is used in CMmPhoneFactoryTsy::NewPhoneL.
     */     
    _LIT( KMmTsyPhoneName, "DefaultPhone");

    /**
     * The literal below defines the name of the primary voice line. The 
     * client  application may choose to open this line to create voice calls.
     */      
    _LIT( KMmTsyVoice1LineName, "Voice1");

    /**
     * The literal below defines the name of the secondary voice line. The  
     * client application may choose to open this line to create voice calls.
     */      
    _LIT( KMmTsyVoice2LineName, "Voice2");

    /**
     * MMTSY currently supports only one data line. The client application  
     * must open this line if it want's to create data calls.
     */     
    _LIT( KMmTsyDataLineName, "Data");

    /**
     * The client application must open a Fax line using this name.
     */     
    _LIT( KMmTsyFaxLineName, "Fax");

#endif //  MMTSY_NAMES_H
            
// End of File
