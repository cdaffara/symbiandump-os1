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
* Name        : CSatDataPackage.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Extension class for CMmDataPackage (defines SAT data package)
* Version     : draft
*
*/



#ifndef CSATDATAPACKAGE_H
#define CSATDATAPACKAGE_H

//  INCLUDES
#include <ctsy/pluginapi/cmmdatapackage.h>

// DESCRIPTION
//
//  
//
NONSHARABLE_CLASS ( CSatDataPackage ) : public CMmDataPackage
    {
public:
    CSatDataPackage( void )
        { 
        iType = CSatDataPackage::KSatType;
        } //default constructor
	};

#endif //CSatDataPackage_H

//  End of file
