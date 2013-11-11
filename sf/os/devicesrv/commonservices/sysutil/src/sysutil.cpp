// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



// INCLUDE FILES
#include "sysutil.h"
#include <f32file.h>
#include <bsul/ccacheddriveinfo.h>
#include "sysutildomaincrkeys.h"	// for disc space
#include "sysutilinternalpskeys.h"	// for disc space
#include "sysutildebug.h"			// debug helper
#include "sysutilpatchdata.h"		// patchable data defining the MMC drive integer identifer
#include <baflpan.h>
#include <e32property.h>
#include <e32capability.h>
#include <bsul/inifile.h>
#include <bautils.h>
#include "sysutilsetup.h"
#include "sysutilplugindef.h"

// File names of the files which store version and device type information.
_LIT( KSWVersionFileName,			"sw.txt" );
_LIT( KLangSWVersionFileName,		"langsw.txt" );
_LIT( KLangVersionFileName,			"lang.txt" );
_LIT( KDeviceAttributesFileName,	"deviceattributes.ini" );
_LIT( KPRInformationFileName,       "purpose.txt" );

// UID of sysutilsetup.exe.
const TUid KSysUtilSetupUid = {0x10285B3B};

// Publish and Subscribe keys.
const TUid KTestFilePathsUid = {0x10285B40};
const TUid KSWVersionUid = {0x10285B3C};
const TUid KLangSWVersionUid = {0x10285B3D};
const TUid KLangVersionUid = {0x10285B3E};
const TUid KDeviceTypeInfoUid = {0x10285B3F};
const TUid KPRInformationUid = {0x20027BC4};

// Device Type Information attribute UIDs.
const TUid KManufacturerNameUid = {0x10286358};
const TUid KModelNameUid = {0x10286359};
const TUid KModelCodeUid = {0x1028635A};
const TUid KRevisionIDUid = {0x1028635B};
const TUid KDefaultDeviceNameUid = {0x1028635C};
const TUid KUIPlatformNameUid = {0x1028635D};
const TUid KUIPlatformVersionMajorUid = {0x1028635E};
const TUid KUIPlatformVersionMinorUid = {0x1028635F};
const TUid KUIPlatformVersionUid = {0x10286360};
const TUid KOSVersionMajorUid = {0x10286361};
const TUid KOSVersionMinorUid = {0x10286362};
const TUid KOSVersionUid = {0x10286363};

// This is the maximum number of Device Type Information attributes that can be 
// stored in the Device Type Information Publish and Subscribe property.
const TUint16 KAttributeLimit = 450;

// Default values that will be used if no plugin is provided, the 
// deviceattributes.ini file is missing or if a device attribute is not 
// specified by a licensee.
_LIT16( KDefaultManufacturer, "Symbian" );
_LIT16( KDefaultModelName, "Symbian" );
_LIT16( KDefaultModelCode, "Symbian" );
_LIT16( KDefaultRevisionID, "00.00" );
_LIT16( KDefaultDeviceName, "Symbian" );
_LIT16( KDefaultUIPlatform, "Symbian" );
_LIT16( KDefaultUIPlatformVersionMajor, "0" );
_LIT16( KDefaultUIPlatformVersionMinor, "0" );
_LIT16( KDefaultUIPlatformVersion, "0.0" );
_LIT16( KDefaultOSVersionMajor, "0" );
_LIT16( KDefaultOSVersionMinor, "0" );
_LIT16( KDefaultOSVersion, "0.0" );


// Security policies for the Publish and Subscribe properties.
_LIT_SECURITY_POLICY_C1( KSecurityPolicyNone, ECapability_None );
_LIT_SECURITY_POLICY_S1( KSecurityPolicyWriteDeviceData, 0x10285B3B, ECapabilityWriteDeviceData );

_LIT( KNewLinePattern, "\\n" );
_LIT( KNewline, "\n" );


/**
Dummy sysutil implementation object - never instantiated.
@internalComponent
*/
class CDeviceTypeInformation::TImpl
	{
	TUint32 iDummy;
	};

/**
Stores a copy of the device type information attributes. For details of the 
memory layout see SGL.TS0017.201 BAFL Component Design Document.doc.
@internalComponent 
*/
typedef struct _SDeviceAttributes
	{
	TUint16 Length() const;
	TUint16 Count() const;
	TInt32 Uid( const TInt aAttributeIndex ) const;
	TInt16 Error( const TInt aAttributeIndex ) const;
	TInt AttributeLength( const TInt aAttributeIndex ) const;
	TUint16* AttributePtr( const TInt aAttributeIndex ) const;
	TUint16* iDeviceAttributes;
	} SDeviceAttributes;
	
/**
@return TInt16 The number of elements in the memory block.
*/
inline TUint16 SDeviceAttributes::Length() const
	{
	return iDeviceAttributes[0];
	}

/**
@return TInt16 The number of device type information attributes.
*/
inline TUint16 SDeviceAttributes::Count() const
	{
	return iDeviceAttributes[1];
	}

/**
Returns the UID which matches the attribute with the given index.

@param 	aAttribIndex	The index of the attribute in iDeviceAttributes.
@return TInt32 			The UID of the attribute with the given index.
*/
inline TInt32 SDeviceAttributes::Uid( const TInt aAttribIndex ) const
	{
	return (iDeviceAttributes[2 + (2 * aAttribIndex)] << 16) + iDeviceAttributes[2 + (2 * aAttribIndex) + 1];
	}

/**
Returns an error code associated with the device type information attribute with 
the given index. This error code is generated when the attribute is retrieved 
from a plugin/deviceattributes.ini. It indicates if a value was successfully 
read, no value was provided or if an error occured.

@param 	aAttribIndex	The index of the attribute in iDeviceAttributes.
@return TInt16 			The error of the attribute with the given index.
*/
inline TInt16 SDeviceAttributes::Error( const TInt aAttribIndex) const
	{
	return static_cast<TInt16>(iDeviceAttributes[2 + (3 * Count()) + 1 + aAttribIndex]);
	}

/**
Returns the length of the attribute with the given index.

@param 	aAttribIndex	The index of the attribute in iDeviceAttributes.
@return TInt			The length of the attribute with the given index.
*/
inline TInt SDeviceAttributes::AttributeLength( const TInt aAttribIndex ) const
	{
	return iDeviceAttributes[2 + (2 * Count()) + aAttribIndex + 1] - iDeviceAttributes[2 + (2 * Count()) + aAttribIndex];
	}

/**
Returns a pointer to the attribute value in iDeviceAttributes with the given 
index.

@param 	aAttribIndex	The index of the attribute in iDeviceAttributes.
@return TUint16*		A pointer to the attribute value with the given index.
*/
inline TUint16* SDeviceAttributes::AttributePtr( const TInt aAttribIndex ) const
	{
	return iDeviceAttributes + iDeviceAttributes[2 + (2 * Count()) + aAttribIndex];
	}

// ======== LOCAL FUNCTIONS ======== 

/**
Returns the total length of all the device type information attribute values stored 
in aValues.

@param	aKeys	An array which contains a list of device type information attribute
				UIDs. The UIDs corrospond to the values in aValues.
@param	aValues	An array which contains a list of device type information attribute
				values. The values corrospond to the UIDs in aKeys.
@return TInt	The total length of all the device type information attribute values 
				stored in aValues.
@internalComponent
*/
static TInt AttributesTotalLength( RArray<TInt32>&, CDesC16ArraySeg& aValues )
	{
	TUint attributesTotalLength = 0;
	TInt numAttributes = aValues.Count();
	for( TInt attributeIndex = 0; attributeIndex < numAttributes; attributeIndex++ )
		{
		TInt attributeLength = aValues[attributeIndex].Length();
		if( attributeLength < CDeviceTypeInformation::KMaxAttributeLength )
			{
			attributesTotalLength += attributeLength;
			}
		else
			{
			attributesTotalLength += CDeviceTypeInformation::KMaxAttributeLength;
			}
		}
	
	return attributesTotalLength;
	}

/**
Returns a pointer to some memory which contains the device type information attribute 
UIDs and values (these are contained in aKeys and aValues) in a compact format. The 
layout of the memory can be found in the Design Document 
(SGL.TS0017.201 BAFL Component Design Document.doc). 

@param	aKeys		An array which contains a list of device type information 
					attribute UIDs. The UIDs corrospond to the values in aValues.
@param	aValues		An array which contains a list of device type information 
					attribute values. The values corrospond to the UIDs in aKeys.
@leave	-			One of the system-wide error codes.
@return	TUint16*	A pointer to the memory which contains the device type 
					information attribute UIDs and values in a compact format.
@internalComponent
*/
static TUint16* CreateDeviceTypeInfoPSDataL( RArray<TInt32>& aKeys, CDesC16ArraySeg& aValues )
	{
	// See SGL.TS0017.201 BAFL Component Design Document.doc for more information on the
	// memory requirements and layout.
	TInt memoryElements = 3 + (4 * aKeys.Count()) + AttributesTotalLength( aKeys, aValues );
	
	// Allocate memory to store the device type information attribute keys and values
	TUint16* deviceAttributes = static_cast<TUint16*>( User::AllocL( sizeof(TUint16) * memoryElements ) );
	CleanupStack::PushL( deviceAttributes );
	
	// Store the attributes length and count
	deviceAttributes[0] = memoryElements;
	deviceAttributes[1] = aKeys.Count();
	
	// Store the attribute UIDs, offsets of the attributes in the memory, error codes and attribute values.
	if( deviceAttributes[1] > 0 )
		{
		// Set the first offset to point to the start of the attributes
		deviceAttributes[2 + (2 * deviceAttributes[1])] = 2 + 1 + (4 * deviceAttributes[1]);
		
		for( TInt attributeNumber = 0; attributeNumber < deviceAttributes[1]; attributeNumber++ )
			{
			// Store the UID as two TUint16s
			deviceAttributes[2 + (2 * attributeNumber)] = static_cast<TUint16>(aKeys[attributeNumber] >> 16); 
			deviceAttributes[2 + (2 * attributeNumber) + 1] = static_cast<TUint16>(aKeys[attributeNumber]); 
			
			if( KUIPlatformVersionMajorUid.iUid == aKeys[attributeNumber] || KUIPlatformVersionMinorUid.iUid == aKeys[attributeNumber] 
				|| KOSVersionMajorUid.iUid == aKeys[attributeNumber] || KOSVersionMinorUid.iUid == aKeys[attributeNumber] )
				{
				// If the values were present in the INI/plugin (as we have the attribute keys) and the values
				// are KNullDesC16 then the values were corrupt.
				if( aValues[attributeNumber].Compare( KNullDesC16 ) == 0 )
					{
					deviceAttributes[2 + (2 * deviceAttributes[1]) + 1 + attributeNumber] = deviceAttributes[2 + (2 * deviceAttributes[1]) + 1 + attributeNumber - 1];
					deviceAttributes[2 + (3 * deviceAttributes[1]) + 1 + attributeNumber] = static_cast<TUint16>(KErrCorrupt);
					continue;
					}
				}
			
			// If the attribute value is too long we need to truncate it and set the error as KErrOverflow.
			if( aValues[attributeNumber].Length() > CDeviceTypeInformation::KMaxAttributeLength )
				{
				deviceAttributes[2 + (2 * deviceAttributes[1]) + 1 + attributeNumber] = deviceAttributes[2 + (2 * deviceAttributes[1]) + attributeNumber] + CDeviceTypeInformation::KMaxAttributeLength;
				deviceAttributes[2 + (3 * deviceAttributes[1]) + 1 + attributeNumber] = static_cast<TUint16>(KErrOverflow);
				Mem::Copy( deviceAttributes + deviceAttributes[2 + (2 * deviceAttributes[1]) + attributeNumber], aValues[attributeNumber].Ptr(), CDeviceTypeInformation::KMaxAttributeLength * 2 );
				}
			else
				{
				deviceAttributes[2 + (2 * deviceAttributes[1]) + 1 + attributeNumber] = deviceAttributes[2 + (2 * deviceAttributes[1]) + attributeNumber] + aValues[attributeNumber].Length();
				deviceAttributes[2 + (3 * deviceAttributes[1]) + 1 + attributeNumber] = KErrNone;
				Mem::Copy( deviceAttributes + deviceAttributes[2 + (2 * deviceAttributes[1]) + attributeNumber], aValues[attributeNumber].Ptr(), aValues[attributeNumber].Size() );
				}
			}
		}
	CleanupStack::Pop( 1 );
	
	return deviceAttributes;
	}

/**
Helper function which deletes the Version and Device Type Information Publish and 
Subscribe properties. This is meant for testing purposes only and requires the 
KSysUtilTestModeEnabled constant to be patched to ETrue.

@leave	-			One of the system-wide error codes.
@internalComponent
*/
EXPORT_C void DeletePSPropertiesL()
	{
	if( KSysUtilTestModeEnabled )
		{
		TInt err = RProperty::Delete(KSWVersionUid.iUid);
		if(err != KErrNotFound)
			{
			User::LeaveIfError(err);
			}
		
		err = RProperty::Delete(KLangSWVersionUid.iUid);
		if(err != KErrNotFound)
			{
			User::LeaveIfError(err);
			}
		
		err = RProperty::Delete(KLangVersionUid.iUid);
		if(err != KErrNotFound)
			{
			User::LeaveIfError(err);
			}
		
		err = RProperty::Delete(KDeviceTypeInfoUid.iUid);
		if(err != KErrNotFound)
			{
			User::LeaveIfError(err);
			}
		
		err = RProperty::Delete(KPRInformationUid.iUid);
        if(err != KErrNotFound)
            {
            User::LeaveIfError(err);
            }
		}
	}

/**
Finds and returns the device type information attribute number in aAttr. This 
number is used as an index in a populated instance of SDeviceAttributes.

@param aAttr A reference to SDeviceAttributes which contains a copy of the device type information attributes.
@param aKey The UID of the device type information attribute which needs to be found.
@return The attribute index number (position) in aAttr.
@return KErrNotFound aKey is not present in aAttr.
@internalComponent
*/
static TInt FindAttributeNumber( const SDeviceAttributes& aAttr, const TUid& aKey )
	{
	TInt bottom = 0;
	TInt top = aAttr.Count() - 1;
	
	while( bottom <= top )
		{
		TUint middle = bottom + ( top - bottom ) / 2;
		if( static_cast<TUint32>(aKey.iUid) < aAttr.Uid( middle ) )
			{
			top = middle - 1;
			}
		else if( static_cast<TUint32>(aKey.iUid) > aAttr.Uid( middle ) )
			{
			bottom = middle + 1;
			}
		else
			{
			return middle;
			}
		}
	return KErrNotFound;
	}

/**
Find a value from the patchable data. If that fails try Publish&Subscribe,
or if that fails, from Central Repository.

@internalComponent
@param aPCKey identifies the value to get from P&S.
@param aCRKey identifies the value to get from Centrep
@param aPatchableData patchable data set at compile time, may have been patched via rom patching
@return Disk level threshold
*/
static TInt FindValueL(
    const TUint aPCKey,
    const TUint aCRKey,
    const TInt aPatchableData )
    {
    TInt val( 0 );
    
    if (aPatchableData != -1)
    	{
    	__SYSUTIL_TRACE1("SysUtil: CL treshold value found from patchable data %d",aPatchableData);	
    	return aPatchableData;
    	}
	
	// Patchable data not set, try to get it from P & S Keys.
	TInt errorCode = RProperty::Get( KPSUidDiskLevel, aPCKey, val );
    if ( errorCode != KErrNone )
        {
        __SYSUTIL_TRACE1("SysUtil: CL treshold value not found from PS: %d, trying CR",errorCode);
        
        CRepository* repository = CRepository::NewLC(KCRUidDiskLevel);
        User::LeaveIfError(repository->Get(aCRKey,val));
        CleanupStack::PopAndDestroy( repository );
        }	
    	
	return val;
    }

/**
Find out the critical level treashold (in bytes) for disk drives of type
aMediaType. Different media types may have different tresholds. If adding
a non-default treshold for some media type, implementation should go here.

@internalComponent
@param aMediaType identifies the media type of the drive
@return Disk level threshold
*/
static TInt64 FindCriticalLevelTresholdL( const TMediaType aMediaType )
    {
    if ( aMediaType == EMediaRam ) // RAM drives have different critical level than others
        {
        return (TInt64) FindValueL(KRamDiskCriticalThreshold, KRamDiskCriticalLevel, KSysUtilRamDiskCriticalThreshold  );
        }
    else // Some other media type
        {
        return (TInt64) FindValueL(KOtherDiskCriticalThreshold, KDiskCriticalThreshold, KSysUtilOtherDiskCriticalThreshold );
        }
    }

/**
Retrieves a string containing the location of sysutilplugin.dll. The location  
which is retrieved is dependent upon two things. The first is a patchable constant 
KSysUtilTestModeEnabled. This must be set to ETrue in order to use the test location instead 
of the standard path. This is set to EFalse by default for hardware builds.

The second is a Publish and Subscribe property, which has a UID of
0x10285B40. If KSysUtilTestModeEnabled is set to ETrue then this property will be defined 
and set to ETrue. This means that the test location is being used instead of the 
standard location. If the client wishes to switch to the standard location they 
can set this property to EFalse.

It may be the case that the standard location needs to be used before the property 
is defined. If this is needed then the client can define and set the property. 
Alternatively, a call can be made to SysUtil::GetSWVersion, 
SysUtil::GetLangSWVersion, SysUtil::GetLangVersion or SysUtil::GetDeviceTypeInfoL. 
It can then call RProperty::Set with a value of EFalse.

@param aLocation	On return contains the location of sysutilplugin.dll.
@leave	-			One of the system-wide error codes.
@internalComponent
*/
static void GetDllLocationL( TDes& aLocation )
	{
	_LIT( KDeviceAttributeDll,	"Z:\\sys\\bin\\sysutilplugin.dll" );

	if( KSysUtilTestModeEnabled )
		{
		TBool testPaths = EFalse;
		TInt err = RProperty::Get( KUidSystemCategory, KTestFilePathsUid.iUid, testPaths );
		if( KErrNone != err )
            testPaths = EFalse;
		
		if( testPaths )
			{
			TDriveUnit systemDrive( static_cast<TUint>(RFs::GetSystemDrive()) );
			TParse path;
			path.Set( systemDrive.Name(), &KDeviceAttributeDll, NULL );
			
			aLocation = path.FullName();
			__SYSUTIL_TRACE1("DLL Location: %S", &aLocation);
			return;
			}
		}
	
	__SYSUTIL_TRACE1("DLL Location: %S", &KDeviceAttributeDll);
	aLocation = KDeviceAttributeDll;
	}

/**
Retrieves a string containing the path to deviceattributes.ini. The path which 
is retrieved is dependent upon two things. The first is a patchable constant 
KSysUtilTestModeEnabled. This must be set to ETrue in order to use the test path instead 
of the standard path. This is set to EFalse by default for hardware builds.

The second is a Publish and Subscribe property, which has a UID of
0x10285B40. If KSysUtilTestModeEnabled is set to ETrue then this property will be defined 
and set to ETrue. This means that the test path is being used instead of the 
standard path. If the client wishes to switch to the standard path they can set 
this property to EFalse.

It may be the case that the standard path needs to be used before the property 
is defined. If this is needed then the client can define and set the property. 
Alternatively, a call can be made to SysUtil::GetSWVersion, 
SysUtil::GetLangSWVersion, SysUtil::GetLangVersion SysUtil::GetDeviceTypeInfoL or SysUtil::GetPRInformation. 
It can then call RProperty::Set with a value of EFalse.

@param 	aPath	On return contains the path to deviceattributes.ini.
@return	-		One of the system-wide error codes.
@internalComponent
*/
static TInt GetFilePath( TDes& aPath )
	{
	_LIT( KDebugPath, "Z:\\versions\\" );
	_LIT( KPath, "Z:\\resource\\versions\\" );

	if( KSysUtilTestModeEnabled )
		{
		TBool testPaths = EFalse;
		
		TInt err = RProperty::Get( KUidSystemCategory, KTestFilePathsUid.iUid, testPaths );
		if( KErrNone != err )
            testPaths = EFalse;
	
		if( testPaths )
			{
			TDriveUnit systemDrive( static_cast<TUint>(RFs::GetSystemDrive()) );
			TParse path;
			path.Set( systemDrive.Name(), &KDebugPath, NULL );
			
			aPath = path.DriveAndPath();
			__SYSUTIL_TRACE1("File Location: %S", &aPath);
			return KErrNone;
			}
		}
	
	__SYSUTIL_TRACE1("File Location: %S", &KPath);
	aPath = KPath;
	return KErrNone;
	}

/**
Fetch text from specified files. The algorithm first reads the file and puts
the string into a buffer. Then it searches for these "\n" sequences in
the buffer. To search for "\n" in a string the search string is specified as "\\n"
with the extra slash in front to mean that we are really searching for "\n" and not
the newline character - 0x000A. Once the "\n" is found in the buffer it is replaced
with the newline character - 0x000A. So since this input string is unicode, it is
actually searching for the byte sequence of 0x005C, 0x006E. These 4 bytes are
replaced by the two byte sequence of 0x000A.

@internalComponent
@param aFilename File name to fetch string from.
@param aValue On return, contains the requested version string. If the buffer is
insufficient the descriptor is filled to its maximum length. If a buffer is provided
that is longer than 64 characters, and the file content is larger than 64 characters,
the returned buffer is truncated at 64 characters (see error codes below).
@param aRemoveNewLines Replaces new line patterns if set to ETrue
@return KErrNone on success.
	    KErrTooBig if the maximum length of the descriptor is insufficient
to hold the file content, or has been truncated (see above). In both cases new-line processing
is done on the buffers if needed. If the file is empty KErrEof is returned, and the length of the
buffer is set to zero. If none of these cases apply then one of the Symbian error codes is
returned if reading the version string fails.
*/
static TInt GetTextFromFile(
    const TDesC& aFilename,
    TDes& aValue,
    TBool aRemoveNewLines )
    {
    RFs fs;
    TInt err;
    err = fs.Connect();
    if (err != KErrNone)
        return err;

    RFile file;
    err = file.Open( fs, aFilename,
                     EFileRead | EFileStreamText | EFileShareReadersOnly );
    if (err != KErrNone)
        {
        fs.Close();
        return err;
        }

    TBuf8<2> characters;
    err =  file.Read(characters);
    
    if (err == KErrNone || err == KErrTooBig)
    	{
	    // This means that we have an ANSI file (without the header bytes)
	    if( characters.Length() == 0 || characters.Length() == 1 )
	    	{
		    file.Close();
		    fs.Close();
			return KErrCorrupt;
	    	}
	    else 
	    	{
			TUint8 firstByte = characters[0];
			TUint8 secondByte = characters[1];
			
			// Heading byte values for unicode files
			const TInt KFFByte = 255;
			const TInt KFEByte = 254;
			
		    // If file isn't unicode KErrCorrupt is returned
			if( (firstByte!=KFFByte && secondByte!=KFEByte) && (secondByte!=KFFByte && firstByte!=KFEByte) )
				{
			    file.Close();
			    fs.Close();
				return KErrCorrupt;
				} 
	    	} 
    	}
    
    TFileText tf;
    tf.Set(file);
    err = tf.Read(aValue);
    // If the maximum length of the descriptor is insufficient to hold the record,
    // the Read() function returns KErrTooBig and the descriptor is filled to its maximum length.
    //
    // If Read() is called when the current position is the end of the file (that is, after
    // the last line delimiter in the file), KErrEof is returned, and the length of the buffer
    // is set to zero. In this case, this would mean an empty file, as this code always reads
    // from the beginning of the file.
    
    if (err == KErrNone || err == KErrTooBig)
    	{
    	if (aValue.Length() > KSysUtilVersionTextLength)
    		{
    		// File content is larger than 64 characters. Truncate to 64 characters.
    		aValue.Delete(KSysUtilVersionTextLength,aValue.Length() - KSysUtilVersionTextLength);
    		err = KErrTooBig;
    		}
    	if (aRemoveNewLines)
    		{
    		// Replace new-line patterns with real ones
            TInt error = aValue.Find(KNewLinePattern);
            while (error != KErrNotFound)
                {
                // error is a position
                aValue.Replace(error, KNewLinePattern().Length(), KNewline );
                error = aValue.Find(KNewLinePattern);
                }
    		}
    	}

    file.Close();
    fs.Close();
    
    return err;
    }

/**
Launches sysutilsetup.exe which is a helper executable. It should be launched 
when the current client does not have the correct capabilities to define Publish 
and Subscribe properties.

Flags are used to determine which Publish and Subscribe properties are defined 
and set. The supported flags can be found in sysutilsetup.h.

@param aFlags 			Used to determine which Publish and Subscribe properties 
						should be defined and set. See sysutilsetup.h for 
						supported flags.
@return KErrNotReady	The excutable has panicked or was not found.
@return -				Otherwise, one of the other system-wide error codes.
@internalComponent
*/
static TInt LaunchSetupExecutable( const TDesC& aFlags )
	{
	_LIT( KSysUtilSetupExeName, "Z:\\sys\\bin\\sysutilsetup.exe" );
	const TUidType KSetupUid( KNullUid, KNullUid, KSysUtilSetupUid );
	
	RProcess setupProcess;
	TInt err = setupProcess.Create( KSysUtilSetupExeName, aFlags, KSetupUid );
	if( KErrNone != err )
		{
		return KErrNotFound == err ? KErrNotReady : err;
		}
	
	TRequestStatus status;
	setupProcess.Rendezvous( status );
	if( KRequestPending != status.Int() )
		{
		setupProcess.Kill( 0 );		// Abort startup
		}
	else
		{
		setupProcess.Resume();		// Logon OK - start the server
		}
	
	User::WaitForRequest( status );	// Wait for start or death
	
	// We can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	err = setupProcess.ExitType() == EExitPanic ? KErrNotReady : status.Int();
	setupProcess.Close();
	
	return err;
	}

/**
Retrieves a string containing the version information for the given UID.

@param aVersionUid	The UID that corresponds to the version information which is required.
@param aValue		On return, contains the version string for the given aVersionUid.
@return -			One of the system-wide error codes.
@internalComponent
*/
static TInt GetVersionPropertyData( const TUid& aVersionUid, TDes& aValue )
	{
	// The P&S property contains the version data (max length 64) and the error code
	TBuf16<KSysUtilVersionTextLength + 1> temp;
	TInt16 err = RProperty::Get( KSysUtilSetupUid, aVersionUid.iUid, temp );
	
	// If the P&S properties are not set we need to set them
	if ( KErrNotFound == err )
		{
		__SYSUTIL_TRACE("Launching sysutilsetup.exe");
		err = LaunchSetupExecutable( KSetVersionPropertiesFlag );
		__SYSUTIL_TRACE("sysutilsetup.exe terminated");
		if( KErrNone != err )
			{
			return err;
			}
		
		err = RProperty::Get( KSysUtilSetupUid, aVersionUid.iUid, temp );
		if( KErrNone != err )
			{
			return err;
			}
		}
	
	if( aValue.MaxLength() < temp.Length() - 1 )
		{
		aValue = temp.MidTPtr( 1, aValue.MaxLength() );
		return KErrTooBig;
		}
	aValue = temp.MidTPtr( 1, temp.Length()  - 1 );
	
	return static_cast<TInt16>( temp[0] );
	}

/**
Retrieves the version information from aFileName and stores it in a Publish and 
Subscribe property which has a key of aKey.

@param aFileName	The name of the file which contains the version information.
@param aKey			The key to be used for the Publish and Subscribe property.
@leave -			One of the system-wide error codes.
@internalComponent
*/
static void SetVersionPropertyL( const TDesC& aFileName, const TUid& aKey )
	{
	TFileName filePath;
	User::LeaveIfError( GetFilePath( filePath ) );
	filePath.Append( aFileName );
	
	// The P&S property must store the version data (max length 64) and the error code.
	TUint16* versionData = static_cast<TUint16*>( User::AllocL( sizeof(TUint16) * (KSysUtilVersionTextLength + 1) ) );
	CleanupDeletePushL( versionData );
	
	// The error code is store in the first TUint16 and the rest is for data.
	TPtr16 versionDataPtr( versionData + 1, KSysUtilVersionTextLength );
	TInt err = GetTextFromFile( filePath, versionDataPtr, ETrue );
	if ( err != KErrNone )
		{
		__SYSUTIL_TRACE2( "Error: %d, while processing: %S", err, &filePath );
		}
	
	// Store the error code from GetTextFromFile in the first TUint16.
	versionData[0] = static_cast<TUint16>( err );
	
	err = RProperty::Define( aKey.iUid, RProperty::EByteArray, KSecurityPolicyNone, 
							 KSecurityPolicyWriteDeviceData, KSysUtilVersionTextLength + 1 );
	if ( KErrAlreadyExists != err )
		{
		User::LeaveIfError( err );
		}
	
	versionDataPtr.Set( versionData, versionDataPtr.Length() + 1, versionDataPtr.Length() + 1 );
	User::LeaveIfError( RProperty::Set( KSysUtilSetupUid, aKey.iUid, versionDataPtr ) );
	CleanupStack::PopAndDestroy( 1 );
	}

/**
Sets the version information Publish and Subscribe properties.

@leave -			One of the system-wide error codes.
@internalComponent
*/
EXPORT_C void SetVersionPropertiesL()
	{
	SetVersionPropertyL( KSWVersionFileName, KSWVersionUid );
	SetVersionPropertyL( KLangSWVersionFileName, KLangSWVersionUid );
	SetVersionPropertyL( KLangVersionFileName, KLangVersionUid);
	SetVersionPropertyL( KPRInformationFileName, KPRInformationUid );
	}

/**
Compares two UIDs.

@param 	aLeft	The first of two UIDs to compare.
@param	aRight	The second of two UIDs to compare.
@return	TInt	Zero if both UIDs are the same. Positive if aLeft is greater 
				than aRight. Negative if aLeft is less than aRight.
@leave -		One of the system-wide error codes.
@internalComponent
*/
static TInt CompareUids( const TInt32& aLeft, const TInt32& aRight )
	{
	TUint32 left = static_cast<TUint32>(aLeft);
	TUint32 right = static_cast<TUint32>(aRight);
	if( left == right )
		{
		return 0;
		}
	else if ( left < right )
		{
		return -1;
		}
	return 1;
	}

/**
Finds the deviceattributes.ini file which best match the current locale. If more 
than one INI file is found for the current locale (this is a result of having 
different versions of the INI file in different ROFS sections) then the contents 
of the INI file are merged. If a attribute appears more than once then the most 
recent value is taken. On return akeys and aValues will contain the merged 
results.

Note: The keys and values are in UID order from low to high.

@param	aKeys		On return, contains the attribute UIDs.
@param  aValues		On return, contains the attribute values.
@leave	KErrGeneral A UID in the INI file is missing the '0x' hex prefix.
@leave	-			Otherwise, one of the other system-wide error codes.
@internalComponent
*/
static void ReadDeviceAttribFilesAsArraysL( 
	RArray<TInt32>& aKeys, 
	CDesC16ArraySeg& aValues )
	{
	_LIT( KWildCardChar, "*" );
	
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	
	// Get the localised version of deviceattributes.ini, if available, and use
	// it to create a search pattern which will be used to find versions of the
	// file that may be available in additional ROFS sections.
	TFileName matchPattern;
	User::LeaveIfError( GetFilePath( matchPattern ) );
	matchPattern.Append( KDeviceAttributesFileName );
	BaflUtils::NearestLanguageFile( fs, matchPattern );
	matchPattern.Append( KWildCardChar );
	
	// Get a list of all the versions of the localised deviceattributes.ini
	// that may be available in additional ROFS sections. If an error occurs
	// the default values will be used.
	CDir* iniFileList = NULL;
	TInt err = fs.GetDir( matchPattern, KEntryAttReadOnly | KEntryAttHidden | 
							KEntryAttSystem | KEntryAttArchive, ESortByName, iniFileList );
	
	__SYSUTIL_TRACE1( "Error: %d, while getting a list of deviceattribute.ini files", err );
	
	if( err == KErrNone )
		{
		CleanupStack::PushL( iniFileList );
		// Go through each INI file (starting with the last file first) extracting 
		// the keys and values which are then stored in the two provided arrays. 
		// This keys and values are stored in key order from low to high. Only the 
		// most recent version of each value is stored i.e. the value in the INI 
		// file which is stored in the newest ROFS section.	
		TLinearOrder<TInt32> orderer( CompareUids );
		for( TInt iniNumber = iniFileList->Count() - 1; iniNumber >= 0; iniNumber-- )
			{
			// Get next INI file
			TFileName iniPath;
			User::LeaveIfError( GetFilePath( iniPath ) );
			iniPath.Append( (*iniFileList)[iniNumber].iName );
			
			BSUL::CIniDocument16* iniFile = NULL;
			TRAPD( err, iniFile = BSUL::CIniDocument16::NewL( fs, iniPath ) );
			__SYSUTIL_TRACE1("BSUL::CIniDocument16::NewL error = %d", err);
			if( err == KErrNone )
				{
				CleanupStack::PushL( iniFile );
				
				RArray<TPtrC16> iniFileSections;
				iniFile->GetSectionList( iniFileSections );
				CleanupClosePushL( iniFileSections );
				
				// For each section get the key/value pairs and insert in order to 
				// the provided arrays. If a key already exists do not insert the 
				// value in aValue.
				TInt numIniFileSections = iniFileSections.Count();
				for( TInt sectionNum = 0; sectionNum < numIniFileSections; sectionNum++ )
					{
					BSUL::CIniSecIter16* sectionIter = BSUL::CIniSecIter16::NewL( iniFileSections[sectionNum], iniFile );
					CleanupStack::PushL( sectionIter );
					
					TPtrC16 key;
					TPtrC16 value;
					while( sectionIter->Next( key, value ) )
						{
						const TChar char0('0');
						const TChar charX('x');
						
						TUint32 tempUid;
						TLex16 lex( key );
						
						// Skip over the '0' and 'x' hex number prefix
						if( lex.Get() != char0 || lex.Get() != charX )
							{
							User::Leave(KErrGeneral);
							}
						User::LeaveIfError( lex.Val( tempUid, EHex ) );	
						
						TInt err = aKeys.InsertInOrder( static_cast<TInt32>(tempUid), orderer );
						if( KErrAlreadyExists != err )
							{
							User::LeaveIfError( err );
							
							TInt index = aKeys.FindInOrder( static_cast<TInt32>(tempUid), orderer );
							aValues.InsertL( index, value );
							}
						}
					CleanupStack::PopAndDestroy( 1 ); // sectionIter
					}
				CleanupStack::PopAndDestroy( 2 ); // iniFile and iniFileSections
				}
			}
		CleanupStack::PopAndDestroy( 1 ); // iniFileList
		}
	CleanupStack::PopAndDestroy( 1 ); // fs
	}

/**
Retrieves the provisioned device type information attribute key/value pairs 
which are stored in aKeys and aValues.

Note: The keys and values are in UID order from low to high.

@param	aKeys		On return, contains the attribute UIDs.
@param	aValues		On return, contains the attribute values.
@leave	KErrCorrupt	There is a mismatch between the number of keys and values or
					an invalid key has been provided.
@leave	-			Otherwise, one of the other system-wide error codes.
@panic KErrTooBig	More than 450 attributes where provisioned.
@internalComponent
*/
static void GetDeviceAttributesL( RArray<TInt32>& aKeys, CDesC16ArraySeg& aValues )
	{
	RArray<TInt32> iniKeys;
	CleanupClosePushL( iniKeys );
	
	CDesC16ArraySeg* iniValues = new (ELeave) CDesC16ArraySeg( 8 );
	CleanupStack::PushL( iniValues );
	
	// First try to get the device type information attributes from the INI file/s.
	ReadDeviceAttribFilesAsArraysL( iniKeys, *iniValues );
	
	if( !KSysUtilDisableDeviceTypeInfoSetupExe )
		{
		// Now try to get the device type information attributes from a DLL.
		TFileName deviceAttributeDllLocation;
		GetDllLocationL(deviceAttributeDllLocation);
		
		RLibrary deviceAttributeDll;
		TInt err = deviceAttributeDll.Load( deviceAttributeDllLocation );
		CleanupClosePushL( deviceAttributeDll );
		
		if( KErrNone == err )
			{
			SysUtilPlugin::GetDeviceAttributesAsArraysFuncL GetDeviceAttributesAsArraysL = reinterpret_cast<SysUtilPlugin::GetDeviceAttributesAsArraysFuncL>( deviceAttributeDll.Lookup( SysUtilPlugin::EGetDeviceAttributesAsArraysLOrdinal ) );
			User::LeaveIfNull( &GetDeviceAttributesAsArraysL );
			GetDeviceAttributesAsArraysL( aKeys, aValues );
			}
		else
			{
			__SYSUTIL_TRACE1("Could not load sysutilplug.dll with error: %d ", err);
			}
		CleanupStack::PopAndDestroy( 1 );
		}
	
	// Merge results with the INI taking lowest priority i.e. if the value already
	// exists in aKeys/aValues then ignore the one in iniKeys/iniValues.
	TLinearOrder<TInt32> orderer( CompareUids );
	TInt numIniAttribs = iniKeys.Count();
	for( TInt keyNum = 0; keyNum < numIniAttribs; keyNum++ )
		{
		TInt err = aKeys.InsertInOrder( iniKeys[keyNum], orderer );
		if( KErrAlreadyExists != err )
			{
			User::LeaveIfError( err );
			
			TInt index = aKeys.FindInOrder( iniKeys[keyNum], orderer );
			aValues.InsertL( index, (*iniValues)[keyNum] );
			}
		}	
	CleanupStack::PopAndDestroy( 2 );
	
	// Check we have the same number of keys and values and that we don't have too many keys
	if( aKeys.Count() != aValues.Count() )
		{
		User::Leave( KErrCorrupt );
		}
	else if( aKeys.Count() > KAttributeLimit )
		{
		_LIT( KPanicReason, "KAttributeLimit exceeded" );
		User::Panic( KPanicReason, KErrTooBig );
		}
	}

/**
Validates major and minor version numbers. This means both values must be 
within the range 0 to KMaxTUint16. If the major, minor or both numbers are 
invalid their values will be replaced by KNullDesC16.

@param	aKeys	An array which contains a list of device type information attribute
				UIDs. The UIDs corrospond to the values in aValues.
@param	aValues	An array which contains a list of device type information attribute
				values. The values corrospond to the UIDs in aKeys.
@internalComponent
*/
static void ValidateVersionNumbersL( RArray<TInt32>& aKeys, CDesC16ArraySeg& aValues, const TUid& aMajorVersionUid, const TUid& aMinorVersionUid)
	{
	TLinearOrder<TInt32> orderer( CompareUids );
	TInt pos = aKeys.FindInOrder( aMajorVersionUid.iUid, orderer );
	if( KErrNotFound != pos )
		{
		if( aKeys[pos + 1] == aMinorVersionUid.iUid )
			{		
			TInt32 value;
			TLex16 lex( aValues[pos] );
			TInt err = lex.Val( value );
				
			if( KErrNone == err && value >= 0 && value <= KMaxTUint16 )
				{
				lex = aValues[pos+ 1];
				err = lex.Val( value );
				
				if( KErrNone == err && value >= 0 && value <= KMaxTUint16 )
					{
					return;
					}
				}
			aValues.Delete( pos + 1 );
			aValues.InsertL( pos + 1, KNullDesC16 );
			}
		aValues.Delete( pos );
		aValues.InsertL( pos, KNullDesC16 );
		return;
		}

	pos = aKeys.FindInOrder( aMinorVersionUid.iUid, orderer );
	if( KErrNotFound != pos )
		{
		aValues.Delete( pos );
		aValues.InsertL( pos, KNullDesC16 );
		}
	}

/**
Gets the device type information attributes and stores them in their formatted form.

@return	TUint16*	Formatted device type information attributes.
@internalComponent
*/
static TUint16* FormattedDeviceTypeInfoL()
	{
	RArray<TInt32> keys;
	CleanupClosePushL( keys );
	
	CDesC16ArraySeg* values = new (ELeave) CDesC16ArraySeg( 8 );
	CleanupStack::PushL( values );
	
	GetDeviceAttributesL( keys, *values );
	
	// Validate the major and minor UI and OS version numbers. This should be done now to avoid repeated processing
	// later on when the client calls either of the GetUIPlatformVersion APIs. An added advantage of doing 
	// this now is that we save memory space if the version numbers are corrupt.
	ValidateVersionNumbersL( keys, *values, KUIPlatformVersionMajorUid, KUIPlatformVersionMinorUid );
	ValidateVersionNumbersL( keys, *values, KOSVersionMajorUid, KOSVersionMinorUid );
	
	TUint16* deviceAttributes = CreateDeviceTypeInfoPSDataL( keys, *values );

	CleanupStack::PopAndDestroy( 2 );
	
	return deviceAttributes;
	}

/**
Sets the Device Type Information attributes Publish and Subscribe property 
value. If the property does not already exist it will define it. Otherwise, it 
will set the value again.

@leave -	One of the system-wide error codes.
@internalComponent
*/
EXPORT_C void SetDeviceTypeInfoPropertyL()
	{
	TUint16* deviceAttributes = FormattedDeviceTypeInfoL();
	CleanupStack::PushL( deviceAttributes );

	// Set the Publish and Subscribe property
	TPtrC16 deviceAttributesTPtrC( deviceAttributes, deviceAttributes[0] );
	TInt propertyType = deviceAttributesTPtrC.Size() < 512 ? RProperty::EByteArray : RProperty::ELargeByteArray;
	
	TInt err = RProperty::Define( KDeviceTypeInfoUid.iUid, propertyType, KSecurityPolicyNone, 
								  KSecurityPolicyWriteDeviceData, deviceAttributesTPtrC.Size() );
	if ( KErrAlreadyExists != err )
		{
		User::LeaveIfError( err );
		}	
	User::LeaveIfError( RProperty::Set( KSysUtilSetupUid, KDeviceTypeInfoUid.iUid, deviceAttributesTPtrC ) );

	CleanupStack::PopAndDestroy( 1 );
	}

// ========================= SysUtil MEMBER FUNCTIONS ==========================

/**
Obtains the displayable software version string. 

Usage example:
@code
TBuf<KSysUtilVersionTextLength> version;
if ( SysUtil::GetSWVersion( version ) == KErrNone )
	{
	// Use the version string.
	...
	}
@endcode
The software version is provisioned by the device creator into the ROM
as a Unicode UTF16 format displayable string, for example:

<code>V 1.0\\n29-07-07\\nBuild12345\\n(c) Symbian Software</code>

This provisioned text string contains only one line of text with "\n" sequences
in the text which indicate a new line. This API will parse the
text and remove any "\n" sequences that it finds and replace it with the 
Unicode newline sequence - 0x000A. The resulting
buffer is then a unicode string with newline sequences built in. This
then can for example, allow the buffer to be displayed directly to the
screen already formatted on multiple lines.

@param aValue On return, contains the software version string. The buffer should
have space for KSysUtilVersionTextLength characters. If the buffer is 
insufficient the descriptor is filled to its maximum length. If a buffer is 
provided that is longer than 64 characters, and the provisioned text is larger 
than 64 characters, the returned buffer is truncated at 64 characters (see error 
codes below).
@return KErrNone on success, KErrTooBig if the maximum length of the descriptor 
is insufficient to hold the provisioned text, or has been truncated (see above). 
In both cases new-line processing is done on the buffers. If a zero length
text string has been provisioned KErrEof is returned, and the length of the 
buffer is set to zero. If none of these cases apply then one of the Symbian 
error codes is returned if reading the version string fails.
*/
EXPORT_C TInt SysUtil::GetSWVersion( TDes& aValue )
	{
	if( KSysUtilDisableVersionSetupExe )
		{
		__SYSUTIL_TRACE("GetSWVersion caching disabled");
		
		TFileName filePath;
		TInt err = GetFilePath( filePath );
		if( KErrNone != err)
			{
			return err;
			}
		
		filePath.Append( KSWVersionFileName );
		
		err = GetTextFromFile( filePath, aValue, ETrue );
		if ( err != KErrNone )
			{
			__SYSUTIL_TRACE2("Error: %d, while processing: %S",err, &filePath);
			}
		
		return err;
		}
	else
		{
		__SYSUTIL_TRACE("GetSWVersion caching enabled");
		return GetVersionPropertyData( KSWVersionUid, aValue );
		}
	}

/**
Returns displayable software version which the currently installed language 
package is compatible with. 

This version text is provisioned by the device creator into the ROM
as a Unicode UTF16 format displayable string, for example:

<code>V 1.0\\n29-07-07\\nBuild12345\\n(c) Symbian Software</code>

This provisioned text string contains only one line of text with "\n" sequences
in the text which indicate a new line. This API will parse the
text and remove any "\n" sequences that it finds and replace it with the 
Unicode newline sequence - 0x000A. The resulting
buffer is then a unicode string with newline sequences built in. This
then can for example, allow the buffer to be displayed directly to the
screen already formatted on multiple lines.

@param aValue On return, contains the software version string. The buffer should
have space for KSysUtilVersionTextLength characters. If the buffer is 
insufficient the descriptor is filled to its maximum length. If a buffer is 
provided that is longer than 64 characters, and the provisioned text is larger 
than 64 characters, the returned buffer is truncated at 64 characters (see error 
codes below).
@return KErrNone on success, KErrTooBig if the maximum length of the descriptor 
is insufficient to hold the provisioned text, or has been truncated (see above). 
In both cases new-line processing is done on the buffers. If a zero length
text string has been provisioned KErrEof is returned, and the length of the 
buffer is set to zero. If none of these cases apply then one of the Symbian 
error codes is returned if reading the version string fails.
*/
EXPORT_C TInt SysUtil::GetLangSWVersion( TDes& aValue )
	{
	if( KSysUtilDisableVersionSetupExe )
		{
		__SYSUTIL_TRACE("GetLangSWVersion caching disabled");
		
		TFileName filePath;
		TInt err = GetFilePath( filePath );
		if( KErrNone != err)
			{
			return err;
			}
		
		filePath.Append( KLangSWVersionFileName );
		
		err = GetTextFromFile( filePath, aValue, ETrue );
		if ( err != KErrNone )
			{
			__SYSUTIL_TRACE2("Error: %d, while processing: %S",err, &filePath);
			}
		
		return err;
		}
	else
		{
		__SYSUTIL_TRACE("GetLangSWVersion caching enabled");
		return GetVersionPropertyData( KLangSWVersionUid, aValue );
		}
	}

/**
Obtains the displayable version of the currently installed language package.
This does NOT do any newline processing on the version text.
(unlike, for example GetLangSWVersion() or GetSWVersion()).

This version text is provisioned by the device creator into the ROM
as a Unicode UTF16 format displayable string.

@param aValue On return, contains the software version string. The buffer should
have space for KSysUtilVersionTextLength characters. If the buffer is 
insufficient the descriptor is filled to its maximum length. If a buffer is 
provided that is longer than 64 characters, and the provisioned text is larger 
than 64 characters, the returned buffer is truncated at 64 characters (see error 
codes below).
@return KErrNone on success, KErrTooBig if the maximum length of the descriptor 
is insufficient to hold the provisioned text, or has been truncated (see above). 
In both cases new-line processing is done on the buffers. If a zero length
text string has been provisioned KErrEof is returned, and the length of the 
buffer is set to zero. If none of these cases apply then one of the Symbian 
error codes is returned if reading the version string fails.
*/
EXPORT_C TInt SysUtil::GetLangVersion( TDes& aValue )
	{
	if( KSysUtilDisableVersionSetupExe )
		{
		__SYSUTIL_TRACE("GetLangVersion caching disabled");
		
		TFileName filePath;
		TInt err = GetFilePath( filePath );
		if( KErrNone != err)
			{
			return err;
			}
		
		filePath.Append( KLangVersionFileName );
		
		err = GetTextFromFile( filePath, aValue, ETrue );
		if ( err != KErrNone )
			{
			__SYSUTIL_TRACE2("Error: %d, while processing: %S",err, &filePath);
			}
		
		return err;
		}
	else
		{
		__SYSUTIL_TRACE("GetLangVersion caching enabled");
		return GetVersionPropertyData( KLangVersionUid, aValue );
		}
	}

/**
Obtains the displayable product release information string. 

Usage example:
@code
TBuf<KSysUtilVersionTextLength> prInfo;
if ( SysUtil::GetPRInformation( prInfo ) == KErrNone )
    {
    // Use the version string.
    ...
    }
@endcode
The product release information is provisioned by the device creator into the ROM
as a Unicode UTF16 format displayable string, for example:

<code>custom build\n20090626\nusb fix xyz</code>
or
<code>PR1.0</code>
or
<code>PR1.1</code>

This provisioned text string contains only one line of text with "\n" sequences
in the text which indicate a new line. This API will parse the
text and remove any "\n" sequences that it finds and replace it with the 
Unicode newline sequence - 0x000A. The resulting
buffer is then a unicode string with newline sequences built in. This
then can for example, allow the buffer to be displayed directly to the
screen already formatted on multiple lines.

@param aValue On return, contains the product release information string. The buffer should
have space for KSysUtilVersionTextLength characters. If the buffer is 
insufficient the descriptor is filled to its maximum length. If a buffer is 
provided that is longer than 64 characters, and the provisioned text is larger 
than 64 characters, the returned buffer is truncated at 64 characters.
@return KErrNone on success, KErrTooBig if the maximum length of the descriptor 
is insufficient to hold the provisioned text, or has been truncated. 
In both cases new-line processing is done on the buffers. If a zero length
text string has been provisioned KErrEof is returned, and the length of the 
buffer is set to zero. If none of these cases apply then one of the Symbian 
error codes is returned if reading the product release string fails.
*/
EXPORT_C TInt SysUtil::GetPRInformation( TDes& aValue )
    {
	TInt err = KErrNone;
    if( KSysUtilDisableVersionSetupExe )
        {
        __SYSUTIL_TRACE("GetPRInformation caching disabled");
        TFileName filePath;
        GetFilePath( filePath );
		filePath.Append( KPRInformationFileName );
		err = GetTextFromFile( filePath, aValue, ETrue );
		if ( err != KErrNone )
			{
			__SYSUTIL_TRACE2("Error: %d, while processing: %S",err, &filePath);
			}
		}
    else
        {
        __SYSUTIL_TRACE("GetPRInformation caching enabled");
        err = GetVersionPropertyData( KPRInformationUid, aValue );
        }
	return err;
    }
    
/**
Checks if free system drive storage space is or will fall below critical level.
The system drive (internal, read/write, persistent drive) is also known
as the FFS (internal flash file system).

To calculate if a critical level has been reached the critical level threshold
setting will be used. This setting is available in the patchable data.

RAM drives and non-RAM drives have different threshold levels, so the corresponding
setting will be used for the calculation depending on the drive type.

RAM drives are defined to be drives that have the media type of EMediaRam
as returned by RFs. @see RFs::Drive

To set the patchdata for the RAM drive threshold set the MACRO 
SYMBIAN_BAFL_SYSUTIL_RAM_DRIVE_CRITICAL_THRESHOLD at ROM build time to an
appropriate level. To set the patchdata for the non-RAM drive threshold
set the MACRO SYMBIAN_BAFL_SYSUTIL_OTHER_DISK_CRITICAL_THRESHOLD at ROM 
build time to an appropriate level.

This method also needs to determine which drive is the FFS drive. To do
this patchable data has been provided. If the patchable data has not been
set, then RFs::GetSystemDrive() will be used to determine the FFS drive. 

To set the patchdata to the FFS drive set the MACRO 
SYMBIAN_BAFL_SYSUTIL_DEFAULT_FFS_DRIVE to the appropriate drive letter. 
@see TDriveNumber

This function exists here to maintain binary compatibility .
@see  SysUtil::FFSSpaceBelowCriticalLevelL

@param aFs File server session. Must be given if available, e.g. from
EIKON environment. If NULL, this method will create a temporary session,
which causes the method to consume more time and system resources.
@param aBytesToWrite Number of bytes the caller is about to write to the
system drive. If value 0 is given, this method checks if the current system
drive space is already below critical level.
@return ETrue if system drive space would go below critical level after writing
aBytesToWrite more data, EFalse otherwise.
@leave System wide error codes
*/
EXPORT_C TBool SysUtil::FFSSpaceBelowCriticalLevel_OldL(
    RFs* aFs,
    TInt aBytesToWrite )
    {
    __SYSUTIL_TRACE("SysUtil::FFSSpaceBelowCriticalLevel_OldL");
    TBool retVal( EFalse );
    
    RFs fs;
    if ( !aFs )
        {
        User::LeaveIfError( fs.Connect() );  // Create temp session.
        CleanupClosePushL( fs );
        }
    else
        {
        if( aFs->Handle() == KNullHandle )
        	{
        	User::Panic(_L("BAFL"), EBafPanicRFsConnectArg );
        	}
        fs = *aFs;
        }

    TInt ffsDrive = GetFFSDriveLetter( fs );
    retVal = DiskSpaceBelowCriticalLevel_OldL(&fs, aBytesToWrite, ffsDrive );
    
	if ( !aFs )
	    {
	    CleanupStack::PopAndDestroy(); // Close temp session
	    }
    
    return retVal;
    }

/**
Checks if free disk drive storage space is or will fall below critical
level. 

To calculate if a critical level has been reached the critical level threshold
setting will be used. This setting is available in the patchable data.

RAM drives  and non-RAM drives have different threshold levels, so the corresponding
setting will be used for the calculation depending on the drive type.

RAM drives are defined to be drives that have the media type of EMediaRam
as returned by RFs. @see RFs::Drive
 
To set the patchdata for the RAM drive threshold set the MACRO 
SYMBIAN_BAFL_SYSUTIL_RAM_DRIVE_CRITICAL_THRESHOLD at ROM build time to an
appropriate level. To set the patchdata for the non-RAM drive threshold
set the MACRO SYMBIAN_BAFL_SYSUTIL_OTHER_DISK_CRITICAL_THRESHOLD at ROM 
build time to an appropriate level.

This function exists here to maintain binary compatibility .
@see  SysUtil::DiskSpaceBelowCriticalLevelL

@param aFs File server session. Must be given if available, e.g. from
EIKON environment. If NULL, this method will create a temporary session,
which causes the method to consume more time and system resources.
@param aBytesToWrite Number of bytes the caller is about to write to
disk. If value 0 is given, this method checks if the current disk space
is already below critical level.
@param aDrive Identifies the disk drive to be checked. Numeric values
for identifying disk drives are defined in TDriveNumber enumeration.
@see TDriveNumber in f32file.h.
@return ETrue if disk space would go below critical level after writing
aBytesToWrite more data, EFalse otherwise.
@leave System wide error codes
*/
EXPORT_C TBool SysUtil::DiskSpaceBelowCriticalLevel_OldL(
    RFs* aFs,
    TInt aBytesToWrite,
    TInt aDrive )
    {
    __SYSUTIL_TRACE1("SysUtil::DiskSpaceBelowCriticalLevel_OldL( %d )",aDrive);
    
    RFs fs;
    if ( !aFs )
        {
        User::LeaveIfError( fs.Connect() );  // Create temp session.
        CleanupClosePushL( fs );
        }
    else
        {
        if( aFs->Handle() == KNullHandle )
        	{
        	User::Panic(_L("BAFL"), EBafPanicRFsConnectArg );
        	}
        fs = *aFs;
        }

    TVolumeInfo vinfo;
    // This may leave e.g. KErrNotReady if no drive
    TInt errorCode = fs.Volume( vinfo, aDrive );
    
    if ( !aFs )
        {
        CleanupStack::PopAndDestroy(); // Close temp session
        }
        
	__SYSUTIL_TRACE1("SysUtil::DiskSpaceBelowCriticalLevel_OldL RFs::Volume returned error code %d.",errorCode);
    User::LeaveIfError( errorCode );

    const TInt64 criticalLevel = FindCriticalLevelTresholdL( vinfo.iDrive.iType );
    __SYSUTIL_TRACE3("SysUtil: CL treshold value: %Ld, Free: %Ld, Size: %Ld",criticalLevel,vinfo.iFree,vinfo.iSize);
  
    return ( vinfo.iFree - (TInt64)aBytesToWrite ) <= criticalLevel;
    }

/**
Checks if free MMC storage space is or will fall below critical
level. 

To calculate if a critical level has been reached the critical level threshold
setting will be used. This setting is available in the patchable data.

RAM drives and non-RAM drives have different threshold levels, so the corresponding
setting will be used for the calculation depending on the drive type.

RAM drives are defined to be drives that have the media type of EMediaRam
as returned by RFs. @see RFs::Drive

This method also needs to determine which drive is the MMC drive. To do
this a patchable data has been provided. If the patchable data
has not been set, then the drive will be fetched using BSUL.
@see CCachedDriveInfo

To set the patchdata for the RAM drive threshold set the MACRO 
SYMBIAN_BAFL_SYSUTIL_RAM_DRIVE_CRITICAL_THRESHOLD at ROM build time to an
appropriate level. To set the patchdata for the non-RAM drive threshold
set the MACRO SYMBIAN_BAFL_SYSUTIL_OTHER_DISK_CRITICAL_THRESHOLD at ROM 
build time to an appropriate level.

To set the patchdata to the MMC drive set the MACRO 
SYMBIAN_BAFL_SYSUTIL_DEFAULT_MMC_DRIVE to the appropriate drive letter. 
@see TDriveNumber

This function exists here to maintain binary compatibility .
@see  SysUtil::MMCSpaceBelowCriticalLevelL

@param aFs File server session. Must be given if available, e.g. from
EIKON environment. If NULL, this method will create a temporary session,
which causes the method to consume more time and system resources.
@param aBytesToWrite Number of bytes the caller is about to write to
MMC. If value 0 is given, this method checks if the current MMC space
is already below critical level.
@return ETrue if MMC space would go below critical level after writing
aBytesToWrite more data, EFalse otherwise.
@leave KErrNotFound if the MMC drive cannot be found, otherwise one of the
system-wide error codes.
*/
EXPORT_C TBool SysUtil::MMCSpaceBelowCriticalLevel_OldL(RFs* aFs, TInt aBytesToWrite)
	{
	__SYSUTIL_TRACE("SysUtil::MMCSpaceBelowCriticalLevel_OldL");
    TBool retVal(EFalse);

    RFs fs;
    if ( !aFs )
        {
        User::LeaveIfError( fs.Connect() );  // Create temp session.
        CleanupClosePushL( fs );
        }
    else
        {
        if( aFs->Handle() == KNullHandle )
        	{
        	User::Panic(_L("BAFL"), EBafPanicRFsConnectArg );
        	}
        fs = *aFs;
        } 
    
    TInt mmcDrive = GetMMCDriveLetter( fs );
    retVal = DiskSpaceBelowCriticalLevel_OldL(&fs, aBytesToWrite, mmcDrive );
 	
    if ( !aFs )
        {
        CleanupStack::PopAndDestroy(); // Close temp session
        }
 			
    return retVal;
    }

/**
Checks if free system drive storage space is or will fall below critical level.
The system drive (internal, read/write, persistent drive) is also known
as the FFS (internal flash file system).

To calculate if a critical level has been reached the critical level threshold
setting will be used. This setting is available in the patchable data.

RAM drives and non-RAM drives have different threshold levels, so the corresponding
setting will be used for the calculation depending on the drive type.

RAM drives are defined to be drives that have the media type of EMediaRam
as returned by RFs. @see RFs::Drive

To set the patchdata for the RAM drive threshold set the MACRO 
SYMBIAN_BAFL_SYSUTIL_RAM_DRIVE_CRITICAL_THRESHOLD at ROM build time to an
appropriate level. To set the patchdata for the non-RAM drive threshold
set the MACRO SYMBIAN_BAFL_SYSUTIL_OTHER_DISK_CRITICAL_THRESHOLD at ROM 
build time to an appropriate level.

This method also needs to determine which drive is the FFS drive. To do
this patchable data has been provided. If the patchable data has not been
set, then RFs::GetSystemDrive() will be used to determine the FFS drive. 

To set the patchdata to the FFS drive set the MACRO 
SYMBIAN_BAFL_SYSUTIL_DEFAULT_FFS_DRIVE to the appropriate drive letter. 
@see TDriveNumber

@param aFs File server session. Must be given if available, e.g. from
EIKON environment. If NULL, this method will create a temporary session,
which causes the method to consume more time and system resources.
@param aBytesToWrite Number of bytes the caller is about to write to the
system drive. If value 0 is given, this method checks if the current system
drive space is already below critical level.
@return ETrue if system drive space would go below critical level after writing
aBytesToWrite more data, EFalse otherwise.
@leave System wide error codes
*/

EXPORT_C TBool SysUtil::FFSSpaceBelowCriticalLevelL(
    RFs* aFs,
    TInt64 aBytesToWrite )
    {
    __SYSUTIL_TRACE("SysUtil::FFSSpaceBelowCriticalLevelL");
    TBool retVal( EFalse );
    
    RFs fs;
    if ( !aFs )
        {
        User::LeaveIfError( fs.Connect() );  // Create temp session.
        CleanupClosePushL( fs );
        }
    else
        {
        if( aFs->Handle() == KNullHandle )
        	{
        	User::Panic(_L("BAFL"), EBafPanicRFsConnectArg );
        	}
        fs = *aFs;
        }

    TInt ffsDrive = GetFFSDriveLetter( fs );
    retVal = DiskSpaceBelowCriticalLevelL(&fs, aBytesToWrite, ffsDrive );
    
	if ( !aFs )
	    {
	    CleanupStack::PopAndDestroy(); // Close temp session
	    }
    
    return retVal;
    }

/**
Checks if free disk drive storage space is or will fall below critical
level. 

To calculate if a critical level has been reached the critical level threshold
setting will be used. This setting is available in the patchable data.

RAM drives  and non-RAM drives have different threshold levels, so the corresponding
setting will be used for the calculation depending on the drive type.

RAM drives are defined to be drives that have the media type of EMediaRam
as returned by RFs. @see RFs::Drive
 
To set the patchdata for the RAM drive threshold set the MACRO 
SYMBIAN_BAFL_SYSUTIL_RAM_DRIVE_CRITICAL_THRESHOLD at ROM build time to an
appropriate level. To set the patchdata for the non-RAM drive threshold
set the MACRO SYMBIAN_BAFL_SYSUTIL_OTHER_DISK_CRITICAL_THRESHOLD at ROM 
build time to an appropriate level.

Usage example:
@code
TInt64 dataSize = 500000000;
if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFsSession, dataSize, EDriveC ) )
    {
    // Can not write the data, there's not enough free space on disk.
    ...
    }
else
    {
    // It's ok to actually write the data.
    ...
    }
@endcode
@param aFs File server session. Must be given if available, e.g. from
EIKON environment. If NULL, this method will create a temporary session,
which causes the method to consume more time and system resources.
@param aBytesToWrite Number of bytes the caller is about to write to
disk. If value 0 is given, this method checks if the current disk space
is already below critical level.
@param aDrive Identifies the disk drive to be checked. Numeric values
for identifying disk drives are defined in TDriveNumber enumeration.
@see TDriveNumber in f32file.h.
@return ETrue if disk space would go below critical level after writing
aBytesToWrite more data, EFalse otherwise.
@leave System wide error codes
*/

EXPORT_C TBool SysUtil::DiskSpaceBelowCriticalLevelL(
    RFs* aFs,
    TInt64 aBytesToWrite,
    TInt aDrive )
    {
    __SYSUTIL_TRACE1("SysUtil::DiskSpaceBelowCriticalLevelL( %d )",aDrive);
    
    RFs fs;
    if ( !aFs )
        {
        User::LeaveIfError( fs.Connect() );  // Create temp session.
        CleanupClosePushL( fs );
        }
    else
        {
        if( aFs->Handle() == KNullHandle )
        	{
        	User::Panic(_L("BAFL"), EBafPanicRFsConnectArg );
        	}
        fs = *aFs;
        }

    TVolumeInfo vinfo;
    // This may leave e.g. KErrNotReady if no drive
    TInt errorCode = fs.Volume( vinfo, aDrive );
    
    if ( !aFs )
        {
        CleanupStack::PopAndDestroy(); // Close temp session
        }
        
	__SYSUTIL_TRACE1("SysUtil::DiskSpaceBelowCriticalLevelL RFs::Volume returned error code %d.",errorCode);
    User::LeaveIfError( errorCode );

    const TInt64 criticalLevel = FindCriticalLevelTresholdL( vinfo.iDrive.iType );
    __SYSUTIL_TRACE3("SysUtil: CL treshold value: %Ld, Free: %Ld, Size: %Ld",criticalLevel,vinfo.iFree,vinfo.iSize);
  
    return ( vinfo.iFree - aBytesToWrite ) <= criticalLevel;
    }

/**
Checks if free MMC storage space is or will fall below critical
level. 

To calculate if a critical level has been reached the critical level threshold
setting will be used. This setting is available in the patchable data.

RAM drives and non-RAM drives have different threshold levels, so the corresponding
setting will be used for the calculation depending on the drive type.

RAM drives are defined to be drives that have the media type of EMediaRam
as returned by RFs. @see RFs::Drive

This method also needs to determine which drive is the MMC drive. To do
this a patchable data has been provided. If the patchable data
has not been set, then the drive will be fetched using BSUL.
@see CCachedDriveInfo

To set the patchdata for the RAM drive threshold set the MACRO 
SYMBIAN_BAFL_SYSUTIL_RAM_DRIVE_CRITICAL_THRESHOLD at ROM build time to an
appropriate level. To set the patchdata for the non-RAM drive threshold
set the MACRO SYMBIAN_BAFL_SYSUTIL_OTHER_DISK_CRITICAL_THRESHOLD at ROM 
build time to an appropriate level.

To set the patchdata to the MMC drive set the MACRO 
SYMBIAN_BAFL_SYSUTIL_DEFAULT_MMC_DRIVE to the appropriate drive letter. 
@see TDriveNumber

@param aFs File server session. Must be given if available, e.g. from
EIKON environment. If NULL, this method will create a temporary session,
which causes the method to consume more time and system resources.
@param aBytesToWrite Number of bytes the caller is about to write to
MMC. If value 0 is given, this method checks if the current MMC space
is already below critical level.
@return ETrue if MMC space would go below critical level after writing
aBytesToWrite more data, EFalse otherwise.
@leave KErrNotFound if the MMC drive cannot be found, otherwise one of the
system-wide error codes.
*/
EXPORT_C TBool SysUtil::MMCSpaceBelowCriticalLevelL(RFs* aFs, TInt64 aBytesToWrite)
	{
	__SYSUTIL_TRACE("SysUtil::MMCSpaceBelowCriticalLevelL");
    TBool retVal(EFalse);

    RFs fs;
    if ( !aFs )
        {
        User::LeaveIfError( fs.Connect() );  // Create temp session.
        CleanupClosePushL( fs );
        }
    else
        {
        if( aFs->Handle() == KNullHandle )
        	{
        	User::Panic(_L("BAFL"), EBafPanicRFsConnectArg );
        	}
        fs = *aFs;
        } 
    
    TInt mmcDrive = GetMMCDriveLetter( fs );
    retVal = DiskSpaceBelowCriticalLevelL(&fs, aBytesToWrite, mmcDrive );
 	
    if ( !aFs )
        {
        CleanupStack::PopAndDestroy(); // Close temp session
        }
 			
    return retVal;
    }


/**
Returns the FFS drive letter  

This method determines which drive is the FFS drive. To do
this a patchable data has been provided. If the patchable data
has not been set, then the drive will be fetched via a call to
GetSystemDrive().

To set the patchdata to the FFS drive set the MACRO 
SYMBIAN_BAFL_SYSUTIL_DEFAULT_FFS_DRIVE to the appropriate drive letter. 
@see TDriveNumber

@return TInt the integer value equivalent of the TDriveNumber
*/
EXPORT_C TInt SysUtil::GetFFSDriveLetter( RFs & aFs )
	{
	// Check the argument, and panic if necessary
    if( aFs.Handle() == KNullHandle )
    	{
    	User::Panic(_L("KERN-EXEC"), 0);
    	}
	
	// Check to see if FFS drive is set in patchable data
	TInt ffsDrive = KSysUtilDefaultFFSDrive;
	if( !(ffsDrive >= EDriveA && ffsDrive <= EDriveZ) )
		{
		ffsDrive =  aFs.GetSystemDrive();
		} 

	return ffsDrive;
	}

/**
Returns the MMC drive letter  

This method determines which drive is the MMC drive. To do
this a patchable data has been provided. 

To set the patchdata to the MMC drive set the MACRO 
SYMBIAN_BAFL_SYSUTIL_DEFAULT_MMC_DRIVE to the appropriate drive letter. 
@see TDriveNumber

@return TInt the integer value equivalent of the TDriveNumber
*/

EXPORT_C TInt SysUtil::GetMMCDriveLetter( RFs & aFs )
	{
	// Check the argument, and panic if necessary
    if( aFs.Handle() == KNullHandle )
    	{
    	User::Panic(_L("KERN-EXEC"), 0);
    	}
    
    // Check to see if MMC drive is set in patchable data
    TInt mmcDrive = KSysUtilDefaultMMCDrive;
    
	// If a valid mmcDrive is not currently found in patchable data, then search for one.
    if( !(mmcDrive >= EDriveA && mmcDrive <= EDriveZ) )
		{
		__SYSUTIL_TRACE("SysUtil::MMCSpaceBelowCriticalLevelL - searching for MMC drive");
	    // If mmcDrive is not set in patchable data, then fetch the MMC drive from BSUL.
	    BSUL::CCachedDriveInfo *cachedDriveInfo = BSUL::CCachedDriveInfo::NewLC( aFs );
	    // This will find valid MMC drive, leave if none available
	    mmcDrive = cachedDriveInfo->GetDefaultRemovableMemoryCardDriveL();
		__SYSUTIL_TRACE1("SysUtil::MMCSpaceBelowCriticalLevelL, Selected MMC drive %d.",mmcDrive);
		CleanupStack::PopAndDestroy(cachedDriveInfo);
		}

	return mmcDrive;
	}

/**
Creates and returns a populated CDeviceTypeInformation object which is used 
to access the device type information attributes.

All the attributes are provisioned by the device creator.

The returned object is owned by the calling code and so it is the calling
codes responsibility to delete this object when it no longer requires it. 

@return	CDeviceTypeInformation*	A pointer to an instance of CDeviceTypeInformation.
@leave	KErrNoMemory			If there is not enough memory to create the object. 
@leave	KErrNotReady			If there was a problem retrieving the device type
 								information attributes.
@leave	-			 			Otherwise one of the other system-wide error codes.
@see CDeviceTypeInformation
*/
EXPORT_C CDeviceTypeInformation* SysUtil::GetDeviceTypeInfoL()
	{
	__SYSUTIL_TRACE("SysUtil::GetDeviceTypeInfoL");
	return CDeviceTypeInformation::NewL();
	}

// ================= CDeviceTypeInformation MEMBER FUNCTIONS ===================

/**
Class destructor.

Performs any clean up such as deleting memory on the heap.
*/
EXPORT_C CDeviceTypeInformation::~CDeviceTypeInformation()
	{
	SDeviceAttributes* attr = reinterpret_cast<SDeviceAttributes*>( iImpl );
	if( attr != NULL )
		{
		delete attr->iDeviceAttributes;
		delete attr;
		}
	}

/**
Allocates and constructs an instance of CDeviceTypeInformation.

@return A pointer to an instance of CDeviceTypeInformation.
@leave KErrNoMemory, if there is not enough memory to create the object.
@leave -			 Otherwise one of the other system-wide error codes.
*/
CDeviceTypeInformation* CDeviceTypeInformation::NewL()
	{
	CDeviceTypeInformation* self = new (ELeave) CDeviceTypeInformation();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

/**
Class constructor.

Performs any class construction tasks that will not cause a leave.
*/
CDeviceTypeInformation::CDeviceTypeInformation() : iImpl( NULL )
	{
	}

/**
Performs class creation tasks as part of two phase construction.

Performs any class construction tasks that will cause a leave.
*/
void CDeviceTypeInformation::ConstructL()
	{
	SDeviceAttributes* attr = new (ELeave) SDeviceAttributes;
	attr->iDeviceAttributes = NULL;
	iImpl = reinterpret_cast<TImpl*>( attr );
	
	if( !KSysUtilDisableDeviceTypeInfoSetupExe )
		{
		__SYSUTIL_TRACE("GetDeviceTypeInfoL caching enabled");
		
		// Check if the P&S property is already set. If it isn't then define and set it. If it 
		// is we will most likely get KErrOverflow as we are only providing enough space to 
		// get the first TUint16. This stores the length of the attribute.
		TUint16 attributesLength;
		TPtr16 attributesLengthTPtr( &attributesLength, 1 );
		TInt err = RProperty::Get( KSysUtilSetupUid, KDeviceTypeInfoUid.iUid, attributesLengthTPtr );
		if ( KErrNotFound == err )
			{
			__SYSUTIL_TRACE("Launching sysutilsetup.exe");
			User::LeaveIfError( LaunchSetupExecutable( KSetDeviceTypeInfoPropertyFlag ) );
			__SYSUTIL_TRACE("sysutilsetup.exe terminated");
			
			err = RProperty::Get( KSysUtilSetupUid, KDeviceTypeInfoUid.iUid, attributesLengthTPtr );
			if( KErrOverflow != err )
				{
				User::LeaveIfError( err );
				}
			}
		else if( KErrOverflow != err )
			{
			User::LeaveIfError( err );
			}
		
		attr->iDeviceAttributes = static_cast<TUint16*>( User::AllocL( sizeof(TUint16) * attributesLength ) );
		TPtr16 deviceAttributesTPtr( attr->iDeviceAttributes, attributesLength );
		User::LeaveIfError( RProperty::Get( KSysUtilSetupUid, KDeviceTypeInfoUid.iUid, deviceAttributesTPtr ) );
		}
	else
		{
		__SYSUTIL_TRACE("GetDeviceTypeInfoL caching disabled");
		attr->iDeviceAttributes = FormattedDeviceTypeInfoL();
		}
	}

/**
Retrieves a reference to the attribute string which matches the provided 
UID. The attribute has a maximum length of KMaxAttributeLength UTF-16 
characters. 

The attribute is provisioned by the device creator. If the device creator 
does not provide an attribute value for the given UID then KNullDesC16 
will be supplied and KErrNotFound will be returned. If the device creator 
has supplied an attribute value which is longer than KMaxAttributeLength 
then the supplied value will be truncated. In this case KErrKErrOverflow 
will be returned.

@param	aAttributeUid	The UID of the required attribute.
@param	aValue			On return, contains the attribute value if successful 
						and KNullDesC16 otherwise.
@return KErrNone		Successful, the provisioned value has been returned.
@return KErrNotFound	A value for the given UID was not provisioned.
@return KErrOverflow	The provisioned attribute value has been truncated.
@return -				Otherwise, one of the other system-wide error codes.
@publishedPartner
@released
*/
EXPORT_C TInt CDeviceTypeInformation::GetAttribute( const TUid& aAttributeUid, TPtrC16& aValue ) const
	{
	SDeviceAttributes* attr = reinterpret_cast<SDeviceAttributes*>( this->iImpl );

	TInt attributeNumber = FindAttributeNumber( *attr, aAttributeUid );
	if( KErrNotFound != attributeNumber )
		{
		aValue.Set( attr->AttributePtr( attributeNumber ), attr->AttributeLength( attributeNumber ) );
		return attr->Error( attributeNumber );
		}

	aValue.Set( KNullDesC16 );
	return KErrNotFound;
	}

/**
Retrieves a reference to the Manufacturer Name string. This value conveys 
the name of the device manufacturer. The Manufacturer Name has a maximum 
length of KMaxAttributeLength UTF-16 characters. 

This is a standard device type information attribute (it is common to all 
device creators) as such it can be assumed that an attribute value will 
always be retrieved.

The Manufacturer Name is provisioned by the device creator. If the device 
creator does not provide a value for this attribute then a default value 
will be supplied instead and KDefaultValue will be returned. If the device 
creator has supplied a value and it is longer than KMaxAttributeLength 
then the provisioned value will be truncated. In this case KErrOverflow 
will be returned.

Note: This attribute is represented by a UID of 0x10286358. 

@param	aValue 			On return, contains the Manufacturer Name UTF-16 string.
@return KErrNone		Successful, the provisioned value has been returned.
@return KDefaultValue	Successful, the default value has been returned.
@return KErrOverflow	The provisioned attribute value has been truncated
@return -				Otherwise one of the other system-wide error codes.
*/
EXPORT_C TInt CDeviceTypeInformation::GetManufacturerName( TPtrC16& aValue ) const
	{
	TInt err = GetAttribute( KManufacturerNameUid, aValue );
	if( KErrNotFound == err )
		{
		aValue.Set( KDefaultManufacturer );
		return CDeviceTypeInformation::KDefaultValue;
		}
	
	return err;
	}

/**
Retrieves a reference to the Model Name string. This value conveys the 
model name of the device as recognisable by the end-user i.e. the consumer. 
The Model Name has a maximum length of KMaxAttributeLength UTF-16 
characters.  

This is a standard device type information attribute (it is common to all 
device creators) as such it can be assumed that an attribute value will 
always be retrieved.

The Model Name is provisioned by the device creator. If the device creator 
does not provide a value for this attribute then a default value will be 
supplied instead and KDefaultValue will be returned. If the device creator 
has supplied a value and it is longer than KMaxAttributeLength then the 
provisioned value will be truncated. In this case KErrKErrOverflow will be 
returned.

Note: This attribute is represented by a UID of 0x10286359. 

@param	aValue			On return, contains the Model Name.
@return KErrNone		Successful, the provisioned value has been returned.
@return KDefaultValue	Successful, the default value has been returned.
@return KErrOverflow	The provisioned attribute value has been truncated.
@return -				Otherwise, one of the other system-wide error codes.
*/
EXPORT_C TInt CDeviceTypeInformation::GetModelName( TPtrC16& aValue ) const
	{
	TInt err = GetAttribute( KModelNameUid, aValue );
	if( KErrNotFound == err )
		{
		aValue.Set( KDefaultModelName );
		return CDeviceTypeInformation::KDefaultValue;
		}
	
	return err;
	}

/**
Retrieves a reference to the Model Code string. This value conveys the 
internal model name or part number by which this model is known to the 
manufacturer. The Model Code has a maximum length of KMaxAttributeLength 
UTF-16 characters. 

This is a standard device type information attribute (it is common to all 
device creators) as such it can be assumed that an attribute value will 
always be retrieved.

The Model Code is provisioned by the device creator. If the device creator 
does not provide a value for this attribute then a default value will be 
supplied instead and KDefaultValue will be returned. If the device creator 
has supplied a value and it is longer than KMaxAttributeLength then the 
provisioned value will be truncated. In this case KErrKErrOverflow will be 
returned.

Note: This attribute is represented by a UID of 0x1028635A. 

@param	aValue			On return, contains the Model Code.
@return KErrNone		Successful, the provisioned value has been returned.
@return KDefaultValue	Successful, the default value has been returned.
@return KErrOverflow	The provisioned attribute value has been truncated.
@return -				Otherwise, one of the other system-wide error codes.
*/
EXPORT_C TInt CDeviceTypeInformation::GetModelCode( TPtrC16& aValue ) const
	{
	TInt err = GetAttribute( KModelCodeUid, aValue );
	if( KErrNotFound == err )
		{
		aValue.Set( KDefaultModelCode );
		return CDeviceTypeInformation::KDefaultValue;
		}
	
	return err;
	}

/**
Retrieves a reference to the Revision ID string. This value contains the 
device revision and/or variant identification string and conveys the 
specific version of the hardware and software used in the device. The 
Revision ID has a maximum length of KMaxAttributeLength UTF-16
characters. 

This is a standard device type information attribute (it is common to all 
device creators) as such it can be assumed that an attribute value will 
always be retrieved.

The Revision ID is provisioned by the device creator. If the device 
creator does not provide a value for this attribute then a default value 
will be supplied instead and KDefaultValue will be returned. If the device 
creator has supplied a value and it is longer than KMaxAttributeLength 
then the provisioned value will be truncated. In this case 
KErrKErrOverflow will be returned.

Note: This attribute is represented by a UID of 0x1028635B. 

@param	aValue			On return, contains the Revision ID (this also 
						includes the Variant ID).
@return KErrNone		Successful, the provisioned value has been returned.
@return KDefaultValue	Successful, the default value has been returned.
@return KErrOverflow	The provisioned attribute value has been truncated.
@return -				Otherwise, one of the other system-wide error codes.
*/
EXPORT_C TInt CDeviceTypeInformation::GetRevisionID( TPtrC16& aValue ) const
	{
	TInt err = GetAttribute( KRevisionIDUid, aValue );
	if( KErrNotFound == err )
		{
		aValue.Set( KDefaultRevisionID );
		return CDeviceTypeInformation::KDefaultValue;
		}
	
	return err;
	}

/**
Retrieves a copy of the default Device Name string. This value conveys 
the default name for the device as might be used for network identification 
e.g. Bluetooth nickname. The Device Name has a maximum length of 
KMaxAttributeLength UTF-16 characters. 

This is a standard device type information attribute (it is common to all 
device creators) as such it can be assumed that an attribute value will 
always be retrieved.

The Default Device Name is provisioned by the device creator. If the device 
creator does not provide a value for this attribute then a default value 
will be supplied instead and KDefaultValue will be returned. If the device 
creator has supplied a value and it is longer than KMaxAttributeLength or 
the user supplied descriptors length is less than KMaxAttributeLength then 
the provioned value will be truncated. In this case KErrKErrOverflow will 
be returned.

Note: This attribute is represented by a UID of 0x1028635C. 

@param	aValue			On return, contains the Default Device Name.
@return KErrNone		Successful, the provisioned value has been returned.
@return KDefaultValue	Successful, the default value has been returned.
@return KErrOverflow	The provisioned attribute value has been truncated.
@return -				Otherwise, one of the other system-wide error codes.
*/
EXPORT_C TInt CDeviceTypeInformation::GetDefaultDeviceName( TPtrC16& aValue ) const
	{
	TInt err = GetAttribute( KDefaultDeviceNameUid, aValue );
	if( KErrNotFound == err )
		{
		aValue.Set( KDefaultDeviceName );
		return CDeviceTypeInformation::KDefaultValue;
		}
	
	return err;
	}

/**
Retrieves a reference to the name of the UI Platform software used 
in the device. The UI Platform name has a maximum length of 
KMaxAttributeLength UTF-16 characters. 

This is a standard device type information attribute (it is common to all 
device creators) as such it can be assumed that an attribute value will 
always be retrieved.

The UI Platform is provisioned by the device creator. If the device 
creator does not provide a value for this attribute then a default value 
will be supplied instead and KDefaultValue will be returned. If the device 
creator has supplied a value and it is longer than KMaxAttributeLength 
then the provisioned value will be truncated. In this case 
KErrKErrOverflow will be returned.

Note: This attribute is represented by a UID of 0x1028635D. 

@param	aValue			On return, contains the UI Platform.
@return KErrNone		Successful, the provisioned value has been returned.
@return KDefaultValue	Successful, the default value has been returned.
@return KErrOverflow	The provisioned attribute value has been truncated.
@return -				Otherwise, one of the other system-wide error codes.
*/
EXPORT_C TInt CDeviceTypeInformation::GetUIPlatformName( TPtrC16& aValue ) const
	{
	TInt err = GetAttribute( KUIPlatformNameUid, aValue );
	if( KErrNotFound == err )
		{
		aValue.Set( KDefaultUIPlatform );
		return CDeviceTypeInformation::KDefaultValue;
		}
	
	return err;
	}

/**
Retrieves a reference to the UI Platform version used in the device for 
display, transmission or tagging purposes. The UI Platform version has a maximum 
length of KMaxAttributeLength UTF-16 characters. 

The string should never be used for programmatic decisions based on assumed
functionality present in the device as device creators can vary the content of
the device firmware. Instead Feature Manager should be used to query the 
functional capabilities of a device. @see CFeatureDiscovery

The UI Platform version is provisioned by the device creator. If the device 
creator does not provide a value for this attribute then a default value 
indicating unknown version will be supplied instead and KDefaultValue will 
be returned. If the device creator has supplied a value and it is longer than 
KMaxAttributeLength then the provisioned value will be truncated. In this case 
KErrOverflow will be returned. The format of the string is device dependent.

Note: This attribute is represented by a UID of 0x10286360. 

@param	aValue			On return, contains the UI version number of the 
						current UI platform.
@return KErrNone		Successful, the provisioned value has been returned.
@return KDefaultValue	Successful, the default value has been returned.
@return KErrOverflow	The provisioned attribute value has been truncated 
						due to the provided descriptor being too small.
@return -				Otherwise, one of the other system-wide error codes.
*/
EXPORT_C TInt CDeviceTypeInformation::GetUIPlatformVersion( TPtrC16& aValue ) const
	{
	TInt err = GetAttribute( KUIPlatformVersionUid, aValue );
	if( KErrNotFound == err )
		{
		aValue.Set( KDefaultUIPlatformVersion );
		return CDeviceTypeInformation::KDefaultValue;
		}
	
	return err;
	}

/**
Retrieves the UI platform major and minor version numbers as TUint16s. 

The retrieved values are both standard device type information attributes 
(they are common to all device creators) as such it can be assumed that 
values will always be retrieved.

The UI version is provisioned by the device creator. If the device creator 
provides invalid major and minor UI version numbers the default version 
numbers will be supplied and KErrCorrupt will be returned. If the device 
creator does not provide values for these attributes then default values 
will be supplied and KDefaultValue will be returned.

Note: The major UI version number attribute is represented by a UID of 
0x1028635E and minor UI version number is represented by a UID of 
0x1028635F. 

@param	aMajor			On return, contains the UI major version number.
@param	aMinor			On return, contains the UI minor version number.
@return KErrNone		Successful, the provisioned value has been returned.
@return KDefaultValue	Successful, the default value has been returned.
@return KErrCorrupt		The provisioned attribute value is invalid. The 
						default value has been returned.
@return -				Otherwise, one of the other system-wide error codes.
*/
EXPORT_C TInt CDeviceTypeInformation::GetUIPlatformVersion( 
	TUint16& aMajor, 
	TUint16& aMinor ) const
	{
	SDeviceAttributes* attr = reinterpret_cast<SDeviceAttributes*>( this->iImpl );
	TPtrC16 majorVersionNum( KDefaultUIPlatformVersionMajor );
	TPtrC16 minorVersionNum( KDefaultUIPlatformVersionMinor );
	
	// If we have valid major and minor numbers use these instead of the default values. 
	// Otherwise check to see if the minor version number exists. If it does the error 
	// number associated with this should be used instead of KDefaultValue as the 
	// version number (in major/minor form) is corrupt.
	TInt attributeNumber = FindAttributeNumber( *attr, KUIPlatformVersionMajorUid );
	if( KErrNotFound != attributeNumber )
		{
		if( attr->Uid( attributeNumber + 1 ) == KUIPlatformVersionMinorUid.iUid )
			{
			if( KErrNone == attr->Error( attributeNumber ) && KErrNone == attr->Error( attributeNumber + 1 ) )
				{
				majorVersionNum.Set( attr->AttributePtr( attributeNumber ), attr->AttributeLength( attributeNumber ) );
				minorVersionNum.Set( attr->AttributePtr( attributeNumber + 1 ), attr->AttributeLength( attributeNumber + 1 ) );
				}
			}
		}
	else
		{
		attributeNumber = FindAttributeNumber( *attr, KUIPlatformVersionMinorUid );
		}
	
	TLex16 lex( majorVersionNum );
	lex.Val( aMajor, EDecimal );
	
	lex.Assign( minorVersionNum );
	lex.Val( aMinor, EDecimal );

	return attributeNumber == KErrNotFound ? KDefaultValue : attr->Error( attributeNumber );
	}

/**
Retrieves a reference to the Symbian OS version used in the device for 
display, transmission or tagging purposes. The Symbian OS version has a maximum 
length of KMaxAttributeLength UTF-16 characters. 

The string should never be used for programmatic decisions based on assumed
functionality present in the device as device creators can very the content of
the device firmware. Instead Feature Manager should be used to query the 
functional capabilities of a device. @see CFeatureDiscovery

This is a standard device type information attribute (it is common to all 
devices) as such it can be assumed that an attribute value will 
always be retrieved. 
 
The Symbian OS version is provisioned by the device creator. If the device 
creator does not provide a value for this attribute then a default value 
indicating unknown version will be supplied instead and KDefaultValue will 
be returned. If the device creator has supplied a value and it is longer than 
KMaxAttributeLength then the provisioned value will be truncated. In this case 
KErrOverflow will be returned. The format of the string is device dependent.

Note: This attribute is represented by a UID of 0x10286363. 

@param	aValue			On return, contains the Symbian OS version number.
@return KErrNone		Successful, the provisioned value has been returned.
@return KDefaultValue	Successful, the default value has been returned.
@return KErrOverflow	The provisioned attribute value has been truncated 
						due to the provided descriptor being too small.
@return -				Otherwise one of the other system-wide error codes.
*/
EXPORT_C TInt CDeviceTypeInformation::GetOSVersion( TPtrC16& aValue ) const
	{
	TInt err = GetAttribute( KOSVersionUid, aValue );
	if( KErrNotFound == err )
		{
		aValue.Set( KDefaultOSVersion );
		return CDeviceTypeInformation::KDefaultValue;
		}
	
	return err;
	}

/**
Retrieves the Symbian OS major and minor version numbers as TUint16s. 

The retrieved values are both standard device type information attributes 
(they are common to all device creators) as such it can be assumed that 
values will always be retrieved.

The OS version is provisioned by the device creator. If the device creator 
provides invalid major and minor UI version numbers the default version 
numbers will be supplied and KErrCorrupt will be returned. If the device 
creator does not provide values for these attributes then default values 
will be supplied and KDefaultValue will be returned.

Note: The major UI version number attribute is represented by a UID of 
0x10286361 and minor UI version number is represented by a UID of 
0x10286362. 

@param	aMajor			On return, contains the OS major version number.
@param	aMinor			On return, contains the OS minor version number.
@return KErrNone		Successful, the provisioned value has been returned.
@return KDefaultValue	Successful, the default value has been returned.
@return KErrCorrupt		The provisioned attribute value is invalid. The 
						default value has been returned.
@return -				Otherwise, one of the other system-wide error codes.
*/
EXPORT_C TInt CDeviceTypeInformation::GetOSVersion( TUint16& aMajor,
	TUint16& aMinor ) const
	{
	SDeviceAttributes* attr = reinterpret_cast<SDeviceAttributes*>( this->iImpl );
	TPtrC16 majorVersionNum( KDefaultOSVersionMajor );
	TPtrC16 minorVersionNum( KDefaultOSVersionMinor );
	
	// If we have valid major and minor numbers use these instead of the default values. 
	// Otherwise check to see if the minor version number exists. If it does the error 
	// number associated with this should be used instead of KDefaultValue as the 
	// version number (in major/minor form) is corrupt.
	TInt attributeNumber = FindAttributeNumber( *attr, KOSVersionMajorUid );
	if( KErrNotFound != attributeNumber )
		{
		if( attr->Uid( attributeNumber + 1 ) == KOSVersionMinorUid.iUid )
			{
			if( KErrNone == attr->Error( attributeNumber ) && KErrNone == attr->Error( attributeNumber + 1 ) )
				{
				majorVersionNum.Set( attr->AttributePtr( attributeNumber ), attr->AttributeLength( attributeNumber ) );
				minorVersionNum.Set( attr->AttributePtr( attributeNumber + 1 ), attr->AttributeLength( attributeNumber + 1 ) );
				}
			}
		}
	else
		{
		attributeNumber = FindAttributeNumber( *attr, KOSVersionMinorUid );
		}
	
	TLex16 lex( majorVersionNum );
	lex.Val( aMajor, EDecimal );
	
	lex.Assign( minorVersionNum );
	lex.Val( aMinor, EDecimal );
	
	return attributeNumber == KErrNotFound ? KDefaultValue : attr->Error( attributeNumber );
	}
