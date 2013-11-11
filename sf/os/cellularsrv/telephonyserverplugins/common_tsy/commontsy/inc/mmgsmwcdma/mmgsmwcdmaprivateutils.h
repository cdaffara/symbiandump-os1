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



#ifndef MMGSMWCDMAPRIVATEUTILS_H
#define MMGSMWCDMAPRIVATEUTILS_H

//  INCLUDES
#include <etelmm.h>
#include <ctsy/serviceapi/mmgsmwcdmautils.h>

/**
* Map linename to multimode api mobile service
*     
* @param TName aLinename: linename
* @param RMobilePhone::TMobileService* aMobileService: mobileservice
* @return status value
*/
TInt MapLinenameToMmMobileService( const TDesC& aLinename,
    RMobilePhone::TMobileService* aMobileService );


#endif // MMGSMWCDMAPRIVATEUTILS_H

// End of File
