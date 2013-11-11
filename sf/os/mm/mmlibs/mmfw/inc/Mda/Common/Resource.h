// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mda\Common\Resource.h
// 
//

#ifndef __MDA_COMMON_RESOURCE_H__
#define __MDA_COMMON_RESOURCE_H__

// Standard EPOC32 includes
#include <e32base.h>

// Public Media Server includes
#include <mda/common/base.h>
#include <mda/common/port.h>
#include <mda/common/controller.h>
#include <mda/common/resource.hrh>

// Uids
MDA_UID(KUidMdaClipLocation) // Resource Clip Location/Format type Uids
MDA_UID(KUidMdaClipFormat) 
MDA_UID(KUidMdaStreamDevice) // Resource Stream Device Uid
MDA_UID(KUidMdaFileResLoc) // Clip Location Uids
MDA_UID(KUidMdaDesResLoc) 
MDA_UID(KUidMdaUrlResLoc) 

/**
@publishedAll
@deprecated
*/
const TInt KMdaRepeatForever = -2;

/**
@publishedAll
@deprecated

Abstract base class for providing the location of audio clip data.

The location could be a filename or the address of a descriptor. An object of the derived class 
is passed as a parameter to CMdaAudioRecorderUtility::OpenL() (to record a clip) or 
CMdaAudioConvertUtility::OpenL() (to convert a clip to another format). This class could be
derived from to create new types of clip location.

@see TMdaFileClipLocation
@see TMdaDesClipLocation
*/
class TMdaClipLocation : public TMdaPackage
	{
protected:
	inline TMdaClipLocation(TUid aUid, TInt aDerivedSize);
public:

	/**
	The size of the whole audio clip in bytes.
	*/
	TInt iSize; // In bytes

	/**
	The size of the part of the clip in bytes currently accessible. For example, a clip whose
	location is a URL might be larger than the amount currently available to the client.
	*/
	TInt iVisible; // In bytes
	};

/**
@publishedAll
@deprecated

The base class for audio clip sound formats.

This class is a package designed to wrap up the UID (and possibly other data) of an audio clip format.

A clip format object (for instance TMdaWavClipFormat, TMdaAuClipFormat or TMdaRawAudioClipFormat) is passed
as a parameter to CMdaAudioRecorderUtility::OpenL(), and to CMdaAudioConvertUtility::OpenL().
*/
class TMdaClipFormat : public TMdaPackage
	{
public:
	inline TMdaClipFormat();
protected:
	inline TMdaClipFormat(TUid aUid, TInt aDerivedSize);
	};

/**
@publishedAll
@deprecated

Not used by MMF
*/
class TMdaStreamDevice : public TMdaPackage
	{
protected:
	inline TMdaStreamDevice(TUid aUid, TInt aDerivedSize);
	};

// Clip Locations

/**
@publishedAll
@deprecated

A package containing the name of a file that stores audio clip data.

An object of this class can be passed to CMdaAudioRecorderUtility::OpenL(), or to CMdaAudioConvertUtility::OpenL().
*/
class TMdaFileClipLocation : public TMdaClipLocation
	{
public:
	inline TMdaFileClipLocation();
	inline TMdaFileClipLocation(const TDesC& aName);
public:
	/**
	The name of the file that contains the audio clip data.
	*/
	TBuf<256> iName;
	};

/**
@publishedAll
@deprecated

A package containing a non-modifiable 8 bit descriptor that stores binary audio clip data.

An object of this class can be passed to CMdaAudioRecorderUtility::OpenL(), or to CMdaAudioConvertUtility::OpenL().
*/
class TMdaDesClipLocation : public TMdaClipLocation
	{
public:
	inline TMdaDesClipLocation();
	inline TMdaDesClipLocation(TDes8& aWriteableDes);
	inline TMdaDesClipLocation(const TDesC8& aReadOnlyDes);
public:
	/**
	The ID of the current thread.
	*/
	TThreadId iThreadId;
	/**
	The descriptor that contains the binary audio clip data.
	*/
	TDesC8* iDes;
	};

/**
@publishedAll
@deprecated

Not used in MMF
*/
class TMdaUrlClipLocation : public TMdaClipLocation
	{
public:
	inline TMdaUrlClipLocation();
	inline TMdaUrlClipLocation(const TDesC& aUrl, TInt aIapId, const TDesC8& aMimeType = KNullDesC8);
public:
	TBuf<256> iUrl;
	TBuf8<256> iMimeType;
	TInt iIapId;
	};


// Inline functions
	
/**
Protected constructor. This can be called from a derived class, with a UID 
identifying the type of clip location and the size of the derived class.

@param  aUid
        A UID that identifies the type of package.
@param  aDerivedSize 
        The size of the derived class.
*/
inline TMdaClipLocation::TMdaClipLocation(TUid aUid, TInt aDerivedSize) :
TMdaPackage(KUidMdaClipLocation,aUid,aDerivedSize) {}

/**
The default constructor initialises the object so that it can be identified by the media server.
*/
inline TMdaClipFormat::TMdaClipFormat(TUid aUid, TInt aDerivedSize) :
TMdaPackage(KUidMdaClipFormat,aUid,aDerivedSize) {}

inline TMdaClipFormat::TMdaClipFormat() : 
TMdaPackage(KUidMdaClipFormat,KNullUid,sizeof(TMdaClipFormat)) {}

inline TMdaStreamDevice::TMdaStreamDevice(TUid aUid, TInt aDerivedSize) :
TMdaPackage(KUidMdaStreamDevice,aUid,aDerivedSize) {}

/**
Default constructor. Initialises the package attributes so that it can be sent to the server. The filename is not initialised.
*/
inline TMdaFileClipLocation::TMdaFileClipLocation() :
TMdaClipLocation(KUidMdaFileResLoc,sizeof(TMdaFileClipLocation)) {}

/**
Constructor with a filename.

@param  aName
        The name of the file that contains the audio clip data.
*/
inline TMdaFileClipLocation::TMdaFileClipLocation(const TDesC& aName) :
TMdaClipLocation(KUidMdaFileResLoc,sizeof(TMdaFileClipLocation)),
iName(aName) {}

inline TMdaUrlClipLocation::TMdaUrlClipLocation() :
TMdaClipLocation(KUidMdaUrlResLoc,sizeof(TMdaUrlClipLocation)) {}

inline TMdaUrlClipLocation::TMdaUrlClipLocation(const TDesC& aUrl, TInt aIapId, const TDesC8& aMimeType) :
TMdaClipLocation(KUidMdaUrlResLoc,sizeof(TMdaUrlClipLocation)),
iUrl(aUrl),iMimeType(aMimeType), iIapId(aIapId) {}


/**
Default constructor. Initialises the package attributes so that it can be sent to the server. The
thread ID is initialised, but the descriptor is not.
*/
inline TMdaDesClipLocation::TMdaDesClipLocation() :
TMdaClipLocation(KUidMdaDesResLoc,sizeof(TMdaDesClipLocation)),
iThreadId(RThread().Id()) {}

/**
Constructor with a non-modifiable descriptor. Initialises the thread ID and the descriptor.

@param  aReadOnlyDes 
        A non-modifiable descriptor which contains the sound data.
*/
inline TMdaDesClipLocation::TMdaDesClipLocation(const TDesC8& aReadOnlyDes) :
TMdaClipLocation(KUidMdaDesResLoc,sizeof(TMdaDesClipLocation)),
iThreadId(RThread().Id()),
iDes(&(TDesC8&)aReadOnlyDes) {}

/**
Constructor with a modifiable descriptor. Initialises the thread ID and the descriptor.

@param  aWriteableDes
        A modifiable descriptor which contains the sound data.
*/
inline TMdaDesClipLocation::TMdaDesClipLocation(TDes8& aWriteableDes) :
TMdaClipLocation(KUidMdaDesResLoc,sizeof(TMdaDesClipLocation)),
iThreadId(RThread().Id()),
iDes(&aWriteableDes) {}

#endif
