// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CAF wrapper classes for Multimedia.
// 
//

#ifndef MMCAF_H
#define MMCAF_H

#include <f32file.h>
#include <caf/caf.h>



const TUid KUidMMFileSource = {0x101F7D3E};
const TUid KUidMMFileHandleSource = {0x101F7D3F};

/**
@publishedAll
@released

Base class which provides a lightweight encapsulation of a DRM protected
content source.
*/
class TMMSource
	{
public:

	/**
	Get the universal identifier of the content source.
	
	@return The universal identifier of the content source.
	*/
	IMPORT_C TUid SourceType() const;
	
	/**
    Get the Unique Id of the source object within the DRM archive.
    
    @return A reference to a descriptor containing the Unique Id.
	*/
	IMPORT_C const TDesC& UniqueId() const;
	/**
	Get the Intent used to open the source object within the DRM archive.
	
	@see ContentAccess::TIntent
	@return The Intent.
	*/
	IMPORT_C ContentAccess::TIntent Intent() const;

	/**
	Get the flag that indicates whether an agent's UI is enabled.
	
	@return A boolean flag that has the value ETrue if the UI is enabled, EFalse otherwise
	*/
	IMPORT_C TBool IsUIEnabled() const;
	
protected:
    /**
    Initialise DRM protected content source
    
    @param aSourceType The source type to differentiate
    @param aUniqueId Unique Id of the source object within the DRM archive
    @param aIntent Usage intent when opening content
    */
	IMPORT_C TMMSource(TUid aSourceType, const TDesC& aUniqueId, ContentAccess::TIntent aIntent);

    /**
    Initialise DRM protected content source
    
    @param aSourceType The source type to differentiate
    @param aUniqueId Unique Id of the source object within the DRM archive
    @param aIntent Usage intent when opening content
    @param aEnableUI The agent's UI is enabled or not.
    */
	IMPORT_C TMMSource(TUid aSourceType, const TDesC& aUniqueId, ContentAccess::TIntent aIntent, TBool aEnableUI);

private:
	/** A univeral identifier to differentiate between content sources. */
	TUid iSourceType;
	/** The Unique Id of the source object within the DRM archive. */
	const TDesC& iUniqueId;
	/** Intent to use when opening content. */
	ContentAccess::TIntent iIntent;
	/** Flag whether an agent's property to enable the UI may be set prior
		to evaluating the intent.	*/
	TBool iEnableUI;	
	/** Reserved to avoid BC break. */
	TInt iReserved1;
	/** Reserved to avoid BC break. */
	TInt iReserved2;
	};


/**
@publishedAll
@released

Provides a file handle to DRM protected content source
*/
class TMMFileHandleSource : public TMMSource
	{
public:
	/**
	Constructs a new TMMFileHandleSource object using a file handle. The Unique Id defaults
	to an empty descriptor.  The Intent defaults to ContentAccess::EUnknown.
	
	@param aFile A reference to a file handle object.
	*/
	IMPORT_C TMMFileHandleSource(RFile& aFile);
	/**
	Constructs a new TMMFileHandleSource object using a file handle and the Unique Id
	of the source object within the DRM archive.  The Intent defaults to ContentAccess::EUnknown.
	
	@param aFile A reference to a file handle object.
	@param aUniqueId A reference to a descriptor containing the Unique Id.
	*/
	IMPORT_C TMMFileHandleSource(RFile& aFile, const TDesC& aUniqueId);
	/**
	Constructs a new TMMFileHandleSource object using a file handle and the Unique Id
	of the source object within the DRM archive.  The Intent defaults to ContentAccess::EUnknown.
	
	@param aFile A reference to a file handle object.
	@param aUniqueId A reference to a descriptor containing the Unique Id.
	@param aIntent the Intent to use when opening the source object within the DRM archive.
	*/
	IMPORT_C TMMFileHandleSource(RFile& aFile, const TDesC& aUniqueId, ContentAccess::TIntent aIntent);

	/**
	Constructs a new TMMFileHandleSource object using a file handle and the Unique Id
	of the source object within the DRM archive.  The Intent defaults to ContentAccess::EUnknown.
	
	@param aFile A reference to a file handle object.
	@param aUniqueId A reference to a descriptor containing the Unique Id.
	@param aIntent the Intent to use when opening the source object within the DRM archive.
	@param aEnableUI a flag to enable the agent's UI.
	*/
	IMPORT_C TMMFileHandleSource(RFile& aFile, const TDesC& aUniqueId, ContentAccess::TIntent aIntent, TBool aEnableUI);

	/**
    Get the file handle of the source object within the DRM archive.
    
    @return A reference to the file handle.
	*/
	IMPORT_C RFile& Handle() const;

private:
	/** The file handle to use when opening content. */
	RFile& iFile;
	};


/**
@publishedAll
@released

A class that provides a consistent interface between filename based and file handle based API's.
In particular this API:

- provides support for container files with embedded content
- allows enabling/disabling of Agent provided user interface features (AgentUI)
- allows a client to take explicit responsibility for the execution of intent
*/
class TMMFileSource : public TMMSource
	{
public:
	/**
	Constructs a new TMMFileSource object using a filename. The Unique Id defaults
	to an empty descriptor.  The Intent defaults to ContentAccess::EUnknown.
	
	@param aName A reference to a filename descriptor.
	*/
	IMPORT_C TMMFileSource(const TDesC& aName);
	/**
	Constructs a new TMMFileSource object using a filename and the Unique Id
	of the source object within the DRM archive.  The Intent defaults to ContentAccess::EUnknown.
	
	@param aName A reference to a filename descriptor.
	@param aUniqueId A reference to a descriptor containing the Unique Id.
	*/
	IMPORT_C TMMFileSource(const TDesC& aName, const TDesC& aUniqueId);
	/**
	Constructs a new TMMFileSource object using a filename and the Unique Id
	of the source object within the DRM archive.  The Intent defaults to ContentAccess::EUnknown.
	
	@param aName A reference to a filename descriptor.
	@param aUniqueId A reference to a descriptor containing the Unique Id.
	@param aIntent the Intent to use when opening the source object within the DRM archive.
	*/
	IMPORT_C TMMFileSource(const TDesC& aName, const TDesC& aUniqueId, ContentAccess::TIntent aIntent);
	
	/**
	Constructs a new TMMFileHandleSource object using a file handle and the Unique Id
	of the source object within the DRM archive.  The Intent defaults to ContentAccess::EUnknown.
	
	@param aName A reference to a filename descriptor.
	@param aUniqueId A reference to a descriptor containing the Unique Id.
	@param aIntent the Intent to use when opening the source object within the DRM archive.
	@param aEnableUI a flag to enable the agent's UI.
	*/
	IMPORT_C TMMFileSource(const TDesC& aName, const TDesC& aUniqueId, ContentAccess::TIntent aIntent, TBool aEnableUI);

	IMPORT_C const TDesC& Name() const;
	
private:
	/** The filename to use when opening content. */
	const TDesC& iName;
	};

	
#endif // MMCAF_H
