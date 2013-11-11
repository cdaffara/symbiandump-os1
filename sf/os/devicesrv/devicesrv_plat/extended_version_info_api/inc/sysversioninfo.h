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
* Description:  Extended Version Info API provides extended version 
*                information i.e. firmware, operator etc version information
*
*/



#ifndef SYS_VERSION_INFO_H
#define SYS_VERSION_INFO_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class RFs;

// CONSTANTS
/**
* Helper constant to allocate buffers for SysVersionInfo::GetVersionInfo.
*/
const TInt KSysVersionInfoTextLength = 256;

/**
* Helper constants to allocate buffers for SysVersionInfo::TProductVersion items.
*/
const TInt KSysVersionInfoManufacturerLength = 50;
const TInt KSysVersionInfoModelLength = 50;
const TInt KSysVersionInfoProductLength = 50;
const TInt KSysVersionInfoRevisionLength = 50;

// CLASS DECLARATION
/**
* Extended Version Info API provides extended version information 
* i.e. firmware, operator etc version information.
*
* Usage:
*  
* Get the version information:
* @code
* // iFs contains open file server session
*
* TBuf< KSysVersionInfoTextLength > info;
* User::LeaveIfError( SysVersionInfo::GetVersionInfo(
*   SysVersionInfo::EFWVersion, info, iFs ) );
*
* // Now info contains firmware version information
*
* SysVersionInfo::TSymbianOSVersion osVersion;
* User::LeaveIfError( SysVersionInfo::GetVersionInfo( osVersion, iFs ) );
*
*  // Now osVersion contains the Symbian OS version information
*
* SysVersionInfo::TProductVersion productVersion;
* User::LeaveIfError( SysVersionInfo::GetVersionInfo( productVersion, iFs ) );
*
*  // Now productVersion contains the product version information
*
* @endcode
*
* @lib sysversioninfo.lib
* @since S60 3.2
*/
class SysVersionInfo
    {
public:

    /**
    * Enumeration TVersionInfoType defines values to be used for
    * the aType parameter in GetVersionInfo method.
    * @since S60 3.2
    */
    enum TVersionInfoType
        {
        /** To get the firmware version that is composed of the version info 
        * of multiple images (core + variants images).
        */
        EFWVersion = 0,

        /** To get the customer software version. This identifies 
        * the user friendly version of e.g. operator/country/regional variant 
        * image.
        */
        EOPVersion,

        /** To get the model version.
        */
        EModelVersion,
        
        /**
        * To get the product code. 
        **/
        EProductCode

        };

    /**
    * Class TVersionBase is a base class for version information data.
    * @since S60 3.2
    */
    class TVersionBase;

    /**
    * This method gets the requested version information as string.
    * The maximum length for the string is KSysVersionInfoTextLength if 
    * it has not been explicitly specified by the requested TVersionInfoType 
    * definition.
    *
    * KErrTooBig is returned if the requested version info does not 
    * fit into given descriptor. The part that fits is copied to 
    * the given descriptor.
    *
    * @since S60 3.2
    * @param aType A version information type specified by TVersionInfoType.
    * @param aValue Stores version information.
    * @return System wide error code.
    *
    * @see TVersionInfoType
    * @see KSysVersionInfoTextLength
    */
    IMPORT_C static TInt GetVersionInfo(
        const TVersionInfoType aType, TDes& aValue );

    /**
    * This method gets the requested version information as string.
    * The maximum length for the string is KSysVersionInfoTextLength if 
    * it has not been explicitly specified by the requested TVersionInfoType 
    * definition. The method avoids overhead of creating new file 
    * server session.
    *
    * KErrTooBig is returned if the requested version info does not 
    * fit into given descriptor. The part that fits is copied to 
    * the given descriptor.
    *
    * @since S60 3.2
    * @param aType A version information type specified by TVersionInfoType.
    * @param aValue Stores version information.
    * @param aFs Open file server session.
    * @return System wide error code.
    *
    * @see TVersionInfoType
    * @see KSysVersionInfoTextLength
    */
    IMPORT_C static TInt GetVersionInfo(
        const TVersionInfoType aType, TDes& aValue, RFs& aFs );

    /**
     * This method gets the requested version information.
     *
     * @since S60 3.2
     * @param aVersion On return, contains the version information.
     * @return System wide error code.
     *
     * @see TVersionBase
     */
    IMPORT_C static TInt GetVersionInfo( TVersionBase& aVersion  );

    /**
     * This method gets the requested version information and avoids 
     * overhead of creating new file server session.
     *
     * @since S60 3.2
     * @param aVersion On return, contains the version information.
     * @param aFs Open file server session.
     * @return System wide error code.
     *
     * @see TVersionBase
     */
    IMPORT_C static TInt GetVersionInfo( TVersionBase& aVersion, RFs& aFs );

    /**
    * Class TVersionBase is a base class for version information data.
    * @since S60 3.2
    */
    class TVersionBase
        {
    friend class SysVersionInfo;

    protected:
        /**
        * Constructor for subclasses
        * @since S60 3.2
        * @param aType Sets type of version info specified by TVersionType
        */
        inline TVersionBase( TInt aType );

    private:
        /**
        * C++ default constructor.
        */
        TVersionBase();

    private:
        /**  Contains type of version info specified by TVersionType */
        TInt iType;
        };

   /**
    * Class TSymbianOSVersion stores the Symbian OS version.
    * The class is used as parameter in GetVersionInfo() methods.
    * @since S60 3.2
    */
    class TSymbianOSVersion : public TVersionBase
        {
    public:
        /**
         * C++ default constructor.
        */
        inline TSymbianOSVersion();

    public:
        /**  Contains the major Symbian OS version. For example 9 if Symbian OS 9.3 */
        TUint16 iMajorVersion;

        /**  Contains the minor Symbian OS version. For example 3 if Symbian OS 9.3 */
        TUint16 iMinorVersion;

        };

    /**
    * Class TProductVersion stores the product information.
    * The class is used as parameter in GetVersionInfo() methods.
    * @since S60 3.2
    */
    class TProductVersion : public TVersionBase
        {
    public:
        /**
         * C++ default constructor.
        */
        inline TProductVersion();

    public:
        /**  Contains the manufacturer name. For example "Nokia". */
        TBuf< KSysVersionInfoManufacturerLength > iManufacturer;

        /**  Contains the phone model sales name. For example "N01". */
        TBuf< KSysVersionInfoModelLength > iModel;

        /**  Contains the product code name. For example "RM-1". */
        TBuf< KSysVersionInfoProductLength > iProduct;

        /**  Contains the product revision. For example "01". */
        TBuf< KSysVersionInfoRevisionLength > iRevision;

        };

private:

    /**
     * C++ default constructor.
     */
    SysVersionInfo();

    /** Used internally to detect type of version info */
    enum TVersionType
        {
        ESymbianOSVersion = 0,
        EProductVersion
        };

    };

#include "sysversioninfo.inl"

#endif // SYS_VERSION_INFO_H

// End of File
