/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM backup storage class.
*
*/


#ifndef PSMBACKUPSTORAGE_H
#define PSMBACKUPSTORAGE_H

#include <e32base.h>
#include "psmstorage.h"

/**
 *  PSM Backup storage class
 *
 *  Resposible for backin up previous PSM configurations and 
 *  also responsible to get correct configurations when returning back
 *  to normal mode. 
 *
 *  @since S60 5.0
 */
class CPsmBackupStorage : public CPsmStorage
    {
public:

    /**
     * Creates a new PSM storage.
     *
     * @return A pointer to the created object.
     */
    static CPsmBackupStorage* NewL( TPsmsrvMode iMode, RFs& aFile );
    static CPsmBackupStorage* NewLC( TPsmsrvMode iMode, RFs& aFile );

    /**
     * Destructor.
     */
    virtual ~CPsmBackupStorage();

    /**
	 * Starts parsing config file. This function blocks until file is parsed
     */
	void InitStorageL( const TUint32 aStorageUid );

    /**
	 * Closes storage file.
     */
	void CloseStorageL();

    /**
     * Backsup configuration to backup storage
     */
    void BackupConfigurationL( const RConfigInfoArray& aConfigArray );

    /**
     * Lists set items of a specific passive configuration
     */
    void ListPassiveConfigSetL( RConfigInfoArray& aPsmConfigArray );

    /**
     * Lists set items of a specific passive configuration
     */
    void ListCurrentSettingSetL( RConfigInfoArray& aPsmConfigArray );

    /**
     * Updates backup storage with new values
     */
    void UpdateCurrentSettingSetL( const RConfigInfoArray& aPsmConfigArray );

private:

    /**
     * C++ constructor.
     */
    CPsmBackupStorage( TPsmsrvMode iMode, RFs& aFile );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Initializes root elements when storage file has been parsed
     */
    void InitRootElementsL();

    /**
     * Helper function to get correct set item.
     *
     * @param aConfigSet reference to configuration set XML element where to find set item
     * @param aKey Identifier of the set element to find
     * @return XML element if set is found, otherwise leaves with KErrNotFound
     */	 
    TXmlEngElement FindSetItemL( const TXmlEngElement& aConfigSet, const TInt aKey );    

    /**
     * Checks does backup folder exist and creates if not
     */
    void CheckBackupLocation();

    /**
     * Generates the location of the backup file in descriptor. 
     * contains <drive letter>+<private path>+<file name>+<extension>
     */
    void BackupFileLocation( TDes& aLocation );

    /**
     * Backsup configuration to backup storage
     */
    void UpdateBackupL( const RConfigInfoArray& aConfigArray, const TInt aMode );

private:    // Data

    /**
     * Indicates is there changes in XML file
     */
    TBool iChangesMade;
    
    };

#endif // PSMBACKUPSTORAGE_H 
