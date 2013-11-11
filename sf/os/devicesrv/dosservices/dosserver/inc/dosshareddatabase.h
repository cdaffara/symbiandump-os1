/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*    This is the definition for CDosharedDataBase class.
*
*/



#ifndef __DOSSHAREDDATABASE_H__
#define __DOSSHAREDDATABASE_H__

// INCLUDES
#include "dosservice.h"
//#include <e32base.h>
#include <s32file.h>


// CONSTANTS
//const TInt KSDReserveFileMaxSize = 64*1024;


// FORWARD DECLARATIONS
class CSharedDataFileSystemNotifier;

/**
* Server side object for RDosAudio class in the client side and the plug-in
* side Audio service derives from this,
*/
class CDosSharedDataBase : public CDosService
{
public:

	IMPORT_C static CDosSharedDataBase* NewL(CSharedDataFileSystemNotifier* fileSystemNotifier);
	IMPORT_C static CDosSharedDataBase* NewLC(CSharedDataFileSystemNotifier* fileSystemNotifier);
	
	virtual ~CDosSharedDataBase();

private:
	/**
	* Processes the message sent by the client.
	* @param aMessage Client-server message.
	* @return Symbian error code.
	*/

	IMPORT_C TInt ExecuteMessageL(const RMessage2& aMessage);

    void ConstructL(CSharedDataFileSystemNotifier* fileSystemNotifier);

protected:

	/**
	* Provide default implementation for plug-in methods incase plug-in
	* module doesn't provide the actual implementation.
	*/

    void RequestFreeDiskSpace( TInt* aRequest );

    void RequestFreeDiskSpaceCancel( TInt* aRequest );
        
    /**
    * Adjusts the size of the reserve file as close to the maximum size
    * as possible so that the current minimum free disk space
    * request is still fulfilled.
    *
    * @param aRequiredFreeDiskSpace current minimum requirement for the
    *                               amount of free disk space
    */
    void SetReserveFileSize( const TInt aRequiredFreeDiskSpace );
    
    


private:
    // free disk space notifier
    //CSharedDataFileSystemNotifier* iFileSystemNotifier;

    // highest outstanding free disk space request
    //TInt* iCurrentFreeDiskSpaceRequest;
        
    //TInt iFreeDiskSpaceRequest;
    const RMessage2* iMessage;  // Kernel owns
    CSharedDataFileSystemNotifier* iFileSystemNotifier;
    RFs iFs;
    TInt iFreeDiskSpaceRequest;

//	inline CSharedDataServer* Server();

    // friend classes
    friend class CSharedDataFileSystemNotifier;
};

#endif //__DOSSHAREDDATABASE_H__
