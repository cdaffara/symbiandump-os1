/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
*    This file contains the neccessary clases to access to the Dos Server
*    Services. Client side API.  
*
*/


#ifndef __DOSCLISHAREDDATA_H__
#define __DOSCLISHAREDDATA_H__

// INCLUDES
#include <e32base.h>
#include <DosSvrServices.h>

/**
* Subsession that provides functionality to make a DOS side self test.
*/
class RDosSharedData : public RDosSubSession
{
public:

	/**
	* Opens the subsession.
	* @param aServer A connected session with the Server.
	* @return Symbian error code.
	*/
	IMPORT_C TInt Open(RDosServer& aServer);

    IMPORT_C void RequestFreeDiskSpace( const TInt aAmount ) const;
    IMPORT_C void RequestFreeDiskSpaceCancel() const;
    IMPORT_C void Close();
    
    //TInt iFreeDiskSpaceRequest;
    
private:    // Data
    const RMessage2* iMessage;  // Kernel owns
    void RDosSharedData::SetFreeDiskSpaceRequest( const TInt aAmount);
};


#endif // __DOSCLISHAREDDATA_H__
