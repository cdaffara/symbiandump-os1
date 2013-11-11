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



#ifndef SYSUTIL_H
#define SYSUTIL_H

#include <e32base.h>

class RFs;
class CDeviceTypeInformation;

/**
Constant conveying the size of the preallocated descriptor buffers used with 
the SysUtil version APIs.

@see SysUtil::GetSWVersion
@see SysUtil::GetLangSWVersion
@see SysUtil::GetLangVersion
@publishedAll
@released
*/
const TInt KSysUtilVersionTextLength = 64;


/**
SysUtil provides various system utility methods, as follows:
 - Functions for applications to retrieve SW and language package versions 
     strings for display purposes 
 - Functions to check whether there is free space on a disk drive before
     file creation or writing.
 - Functions to retrieve Device Type information (e.g. phone model) for display
     purposes.

Version, Attribute strings and free space thresholds are provisioned by 
the device creator into the ROM. For details on this see the
'SGL.TS0017.324 BAFL How-To FAQ Document' in the OS Developer Library.  

@publishedAll
@released
*/
class SysUtil
    {
public:
    IMPORT_C static TInt GetSWVersion( TDes& aValue );
    IMPORT_C static TInt GetLangSWVersion( TDes& aValue );
    IMPORT_C static TInt GetLangVersion( TDes& aValue );
    IMPORT_C static TInt GetPRInformation( TDes& aValue );
        
private:
    IMPORT_C static TBool FFSSpaceBelowCriticalLevel_OldL(
        RFs* aFs,
        TInt aBytesToWrite = 0 );
    IMPORT_C static TBool MMCSpaceBelowCriticalLevel_OldL(
        RFs* aFs,
        TInt aBytesToWrite = 0 );
    IMPORT_C static TBool DiskSpaceBelowCriticalLevel_OldL(
        RFs* aFs,
        TInt aBytesToWrite,
        TInt aDrive );
public:
    IMPORT_C static TInt GetMMCDriveLetter( RFs & aFs );
    IMPORT_C static TInt GetFFSDriveLetter( RFs & aFs );
    
public:
	IMPORT_C static CDeviceTypeInformation* GetDeviceTypeInfoL();
	
	IMPORT_C static TBool FFSSpaceBelowCriticalLevelL(
	           RFs* aFs,
	           TInt64 aBytesToWrite = 0 );
	IMPORT_C static TBool MMCSpaceBelowCriticalLevelL(
	           RFs* aFs,
	           TInt64 aBytesToWrite = 0 );
	IMPORT_C static TBool DiskSpaceBelowCriticalLevelL(
	           RFs* aFs,
	           TInt64 aBytesToWrite,
	           TInt aDrive );
	};

/**
This class is used to hold the device type information attributes and provides 
member functions to return the attribute values. These values are strings of 
UTF-16 characters and no format must be assumed or implied as it varies from 
one device manufacturer to the next. Please note that this information does not
identify a unique device but identifies the type of device.

An instance of this class cannot be created by user code. If device type 
information attributes are required then the user should use 
SysUtil::GetDeviceTypeInfoL which will return a pointer to an instance of this 
class. This instance will contain a full set of device type information 
attributes that have been provisioned by the device creator. For 
details of how these are provisioned see 'XXX xxx' document in the OS Developer 
Library.

For standard device type information attributes (attributes which are common 
to all device creators) named functions have been provided. These functions 
also offer the advantage of a default value being provided when an attribute 
has not been provisioned by the device creator. If a default value has been 
retrieved KDefaultValue will be returned.

Callers who do not care about whether a default value is retrieved or not can 
use the API as follows:

@code
	TPtrC16 modelNamePtrC;
	User::LeaveIfError( deviceTypeInfo->GetModelName(modelNamePtrC) );
@endcode

Where callers wish to avoid the default value it can be tested for as follows:

@code
	TPtrC16 modelNamePtrC;
	if (User::LeaveIfError( deviceTypeInfo->GetModelName(modelNamePtrC)) == CDeviceTypeInformation::KDefaultValue)
		{
		// We have a default attribute value, do something else
		...
		}
	else
		{
		// We have a device creator supplied attribute value.
		...
		}
@endcode

In addition to named functions, two additional generic functions are provided 
that can be used to retrieve any additional device type information attributes 
which may be provided by a device creator. These functions can also be used to 
retrieve the standard attributes; however, it is recommended that the named 
functions be used instead.

Any code which owns an instance of this class has the responsibility of 
destroying it. This may be achieved by calling delete on the pointer or using 
the CleanupStack.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CDeviceTypeInformation) : public CBase
	{
public: //publishedAll
	/** 
	 The maximum length of a device attribute string value.
	 */
	static const TInt KMaxAttributeLength = 64;
	
	/** 
	 This const is a value returned from calls to the named
	 CDeviceTypeInformation APIs. It indicates to the caller that the returned 
	 device type information attribute, stored in CDeviceTypeInformation, is a 
	 default value. This occurs when the device creator does not provision the 
	 attribute value.
	 */
	static const TInt KDefaultValue = 1;
public: // publishedAll
	IMPORT_C ~CDeviceTypeInformation();
	static CDeviceTypeInformation* NewL();
	IMPORT_C TInt GetManufacturerName( TPtrC16& aValue ) const;	
	IMPORT_C TInt GetModelName( TPtrC16& aValue ) const;
	IMPORT_C TInt GetModelCode( TPtrC16& aValue ) const;
	IMPORT_C TInt GetRevisionID( TPtrC16& aValue ) const;
	IMPORT_C TInt GetDefaultDeviceName( TPtrC16& aValue ) const;
	IMPORT_C TInt GetUIPlatformName( TPtrC16& aValue ) const;
	IMPORT_C TInt GetUIPlatformVersion( TPtrC16&  aValue ) const;
	IMPORT_C TInt GetUIPlatformVersion( TUint16& aMajor, TUint16& aMinor ) const;
	IMPORT_C TInt GetOSVersion( TPtrC16& aValue ) const;
	IMPORT_C TInt GetOSVersion( TUint16& aMajor, TUint16& aMinor ) const;

public: // publishedPartner
	IMPORT_C TInt GetAttribute( const TUid& aAttributeUid, TPtrC16& aValue ) const;
private:
	CDeviceTypeInformation();
	void ConstructL();
private:
	class TImpl;
	TImpl* iImpl;
	};

#endif // SYSUTIL_H
