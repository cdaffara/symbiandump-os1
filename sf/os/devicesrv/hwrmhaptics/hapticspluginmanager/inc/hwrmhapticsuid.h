/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics uid header file.
*                This class is designed to parse the associated text file
*                and determine allowed UIDs for requesting license key.
*
*/


#ifndef C_HWRMHAPTICSUID_H
#define C_HWRMHAPTICSUID_H

#include <e32base.h>

class RFs;


/**
 *  This class is contains information is license auto set allowed 
 *  for one specific UID (VID or SID)
 *
 *  @since S60 5.1
 */
class TUidItem
    {
public:
    /**
     * C++ constructor.
     */
    TUidItem( const TDesC8& aUidType, TUid aUid, TBool aAllowed );

public:

    /**
     * Uid type, "VID" / "SID"
     */
    TBuf8<3> iUidType;

    /**
     * Secure id / Vendor id.
     */
    TUid iUid;

    /**
     * Is license auto set allowed.
     */
    TBool iAllowed;    
    };


/**
 *  This class is intended to parse the associated UID text file and 
 *  provide an API to check is license auto set allowed VID/SID
 *  combination specified.
 *
 *  @since S60 5.1
 */
class CHWRMHapticsUid : public CBase
    {
public:
    
    /**
     * Two-phased constructor.
     *
     * @param aFilename  Filename of UID file, which should not
     *                   include the path as the file is stored in 
     *                   the private directory.
     */
    static CHWRMHapticsUid* NewL( const TDesC& aFilename );

    /**
     * Two-phased constructor.
     * Leaves instance on the cleanup stack.
     * 
     * @param aFilename  Filename of UID file, which should not
     *                   include the path as the file is stored in 
     *                   the private directory.
     */
    static CHWRMHapticsUid* NewLC( const TDesC& aFilename );
        
    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsUid();

public:
    
    /**
     * Checks if automated license setting is allowed for VID/SID combination
     * found from aMessage.
     *       
     * @param aMessage Message which VID and SID are checked against
     * the values retrieved from file.
     * @return ETrue or EFalse depending is automated license setting allowed.
     */
    TBool LicenseAutoSetAllowed( const RMessage2& aMessage );
    
private:

    /**
     * C++ constructor.
     */
    CHWRMHapticsUid();

    /**
     * By default Symbian 2nd phase constructor is private.
     *
     * @param aFilename  Filename of UID file, which should not
     *                   include the path as the file is stored in 
     *                   the private directory.
     */
    void ConstructL( const TDesC& aFilename );
    
    /**
     * Method constructs the array of UIDs from the 
     * given stream (usually an RFileReadStream)
     * 
     * @param aFs File server session
     * @param aFilename Filename to read
     */
    void ParseUidFileL( RFs& aFs, const TDesC& aFilename );

    /**
     * Helper method to convert and validate UID from a descriptor.
     * 
     * @param aUidDes The Uid to be converted
     * @param aUid Reference that will contain the UID
     * @return KErrNone if successful, KErrCorrupt if invalid
     */
    TInt ConvertId( const TDesC8& aUidDes, TUid& aUid ) const;

private:

    /**
     * Array of UIDs from the UID file.
     */
    RArray<TUidItem> iUidArray;
    
    /**
     * Index pointer to iUidArray.
     */
    TInt         iNextUidIndex;
    };

#endif // C_HWRMHAPTICSUID_H

// End of File
