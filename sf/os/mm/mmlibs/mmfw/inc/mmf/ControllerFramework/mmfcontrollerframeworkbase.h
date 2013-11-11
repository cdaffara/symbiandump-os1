// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MMFCONTROLLERFRAMEWORKBASE_H__
#define __MMFCONTROLLERFRAMEWORKBASE_H__

#include <e32base.h>
#include <e32std.h>
#include <s32mem.h>

/**
@publishedAll
@released

Constant that can be passed into the client API OpenURL() interfaces (as the aIapId parameter)
to request that the default Internet Access Point be used.
*/
const TInt KUseDefaultIap = -1;


/**
@publishedAll
@released

General purpose class to describe an event.

Contains a UID to define the actual event type, and an integer to define the event code.

Controller plugins can define their own event types, but should use
KMMFErrorCategoryControllerGeneralError for any errors that can be described by
the standard system-wide error codes.

@since 7.0s
*/
class TMMFEvent
	{
public:

	/**
	Constructor.
	
	@param  aEventType
	        A UID to define the type of event.
	@param  aErrorCode
	        The error code associated with the event.

	@since 7.0s
	*/
	IMPORT_C TMMFEvent(TUid aEventType, TInt aErrorCode);

	/**
	Default constructor.
	
	Provided so this class can be packaged in a TPckgBuf<>.
	@since 7.0s
	*/
	IMPORT_C TMMFEvent();

	/**
	A UID to define the event type.
	*/
	TUid iEventType;

	/**
	The error code associated with the event.
	*/
	TInt iErrorCode;
private:
	/**
	This member is internal and not intended for use.
	*/
	TInt iReserved1;
	};

/**
@publishedAll
@released

Package buffer for TMMFEvent
*/
typedef TPckgBuf<TMMFEvent> TMMFEventPckg;

/**
@publishedAll
@released

Package buffer for TUid
*/
typedef TPckgBuf<TUid> TMMFUidPckg;

/**
@publishedAll
@released

Package buffer for TTimeIntervalMicroSeconds
*/
typedef TPckgBuf<TTimeIntervalMicroSeconds> TMMFTimeIntervalMicroSecondsPckg;


/**
@publishedAll
@released

A piece of meta data.

Meta data is often contained in the header of multimedia clips and is used to define
attributes such as the author and copyright details.

Each piece of meta data has a name, or category, and a value.

@since 7.0s
*/
class CMMFMetaDataEntry : public CBase
	{
public:

	/**
	Constructs a new meta data object using a name and value.

	This method can leave with one of the standard system-wide error codes.

	An example of a name might be:
	@code
	For example, <Copyright>
	@endcode

	An example of a value might be:
	@code
	<Symbian (c) 2002>
	@endcode

	@param	aName
	        The name, or category, to be given to this piece of meta data.
	@param  aValue
	        The value of this piece of meta data.

	@return The newly created meta data object.

	@since	7.0s
	*/
	IMPORT_C static CMMFMetaDataEntry* NewL(const TDesC& aName, const TDesC& aValue);

	/**
	Copy constructor. Returns a replica of the meta data passed in.

	This method can leave with one of the standard system-wide error codes.

	@param  aOther
	        The meta data to be copied.

	@return The newly created meta data object.

	@since	7.0s
	*/
	IMPORT_C static CMMFMetaDataEntry* NewL(const CMMFMetaDataEntry& aOther);

	/**
	Default constructor. Returns a blank meta data object. To be used when internalizing
	data into the object.

	This method can leave with one of the standard system-wide error codes.

	@return  The newly created meta data object.

	@since	7.0s
	*/
	IMPORT_C static CMMFMetaDataEntry* NewL();

	/**
	Destructor.

	@since	7.0s
	*/
	IMPORT_C ~CMMFMetaDataEntry();

	/**
	Returns the name, or category, of the meta data.

	@return The name of the meta data object.

	@since	7.0s
	*/
	IMPORT_C const TDesC& Name() const;

	/**
	Returns the value field of the meta data.

	@return The value field of the meta data object.

	@since  7.0s
	*/
	IMPORT_C const TDesC& Value() const;

	/**
	Sets the name, or category, of the meta data.

	This method can leave with one of the standard system-wide error codes.

	@param  aName
	        The new name of the meta data object

	@since	7.0s
	*/
	IMPORT_C void SetNameL(const TDesC& aName);

	/**
	Sets the value field of the meta data.

	This method can leave with one of the standard system-wide error codes.

	@param  aValue
	        The new value field of the meta data object

	@since	7.0s
	*/
	IMPORT_C void SetValueL(const TDesC& aValue);

	/**
	Writes the data contained within the object to a stream so it can be copied
	over IPC or written to a file.
	
	This method can leave with one of the standard system-wide error codes.

	@param  aStream
	        The stream to be written to.

	@since	7.0s
	*/
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	/**
	Reads data from a stream and copies it into this object.  Used when copying
	a meta data object over IPC or reading it from a file.

	This method can leave with one of the standard system-wide error codes.

	@param  aStream
	        The stream to be read.

	@since	7.0s
	*/
	IMPORT_C void InternalizeL(RReadStream& aStream);
private:

	/**
	Constructor.

	@since 7.0s
	*/
	CMMFMetaDataEntry();

	/**
	Second phase constructor.

	An example of aName might be:
	@code
	<Copyright>
	@endcode

	An example of aValue might be:
	@code
	<Symbian (c) 2002>
	@endcode

	This method can leave with one of the standard system-wide error codes.

	@param  aName
	        The name, or category, to be given to this piece of meta data.
	@param  aValue
	        The value of this piece of meta data.

	@since	7.0s
	*/
	void ConstructL(const TDesC& aName, const TDesC& aValue);
private:
	/**
	The name, or category, of the meta data.
	*/
	HBufC* iName;

	/**
	The value of the meta data.
	*/
	HBufC* iValue;
	};




/**
@publishedAll
@released

Class to be used to configure a URL source or sink.

Contains a variable length string to represent the URL, and whether to use a specified
Internet Access Point or use the default.

@since 7.0s
*/
class CMMFUrlParams : public CBase
	{
public:

	/**
	Factory function to create a CMMFUrlParams object.

	This method can leave with one of the standard system-wide error codes.

	@param  aUrl
	        The url, for example http://www.symbian.com/clip.mpg
	@param  aIAPId
	        The Inernet Access Point to be used.  This should be a valid IAP ID
	        retrieved from CommDB.  A special value of KUseDefaultIap can be used
	        to signify that the plugin should just use the default IAP.

	@return A pointer to the newly created object.

	@since	7.0s
	*/
	IMPORT_C static CMMFUrlParams* NewL(const TDesC& aUrl, TInt aIAPId=KUseDefaultIap);

	/**
	Factory function to create a CMMFUrlParams object.

	This method can leave with one of the standard system-wide error codes.

	@param  aUrl
	        The url, for example http://www.symbian.com/clip.mpg
	@param  aIAPId
	        The Inernet Access Point to be used.  This should be a valid IAP ID
	        retrieved from CommDB.  A special value of KUseDefaultIap can be used
	        to signify that the plugin should just use the default IAP.
	@return A pointer to the newly created object. The object will be left on the cleanup stack.

	@since	7.0s
	*/
	IMPORT_C static CMMFUrlParams* NewLC(const TDesC& aUrl, TInt aIAPId=KUseDefaultIap);

	/**
	Factory function to create a CMMFUrlParams object by internalizing data from a stream.

	This method can leave with one of the standard system-wide error codes.

	@param  aStream
	        The stream from which the object should be internalized.

	@return The newly created object. The object will be left on the cleanup stack.

	@since	7.0s
	*/
	IMPORT_C static CMMFUrlParams* NewLC(RReadStream& aStream);

	/**
	Externalize this object to a stream.

	This method can leave with one of the standard system-wide error codes.

	@param  aStream
	        The stream to which the object will be externalized.

	@since	7.0s
	*/
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	/**
	Externalize this object into a newly created CBufFlat buffer.
	
	This method can leave with one of the standard system-wide error codes.

	@return The newly created buffer containing the externalized data. The CBufFlat object will be
	        left on the cleanup stack.

	@since	7.0s
	*/
	IMPORT_C CBufFlat* ExternalizeToCBufFlatLC() const;

	/**
	Destructor.
	*/
	~CMMFUrlParams();


	/**
	Returns the URL string, for example http://www.symbian.com/clip/mpg.

	@return The URL string.
	@since	7.0s
	*/
	IMPORT_C const TDesC& Url() const;

	/**
	Returns the Internet Access Point ID.
	
	@return IAP ID.

	@since	7.0s
	*/
	IMPORT_C TInt IAPId() const;

	/**
	Signifies whether the IAP ID should be used. This is determined internally by comparing
	the IAP ID with KUseDefaultIap.

	@return A boolean indicating if the IAP ID should be used. ETrue if the supplied IAPId should
	        be used, EFalse if not.

	@since  7.0s
	*/
	IMPORT_C TBool UseIAPId() const;
private:
	CMMFUrlParams();
	void ConstructL(const TDesC& aUrl, TInt aIAPId);
	void ConstructL(RReadStream& aStream);
private:
	HBufC* iUrl;
	TInt iIAPId;
	};


#endif
