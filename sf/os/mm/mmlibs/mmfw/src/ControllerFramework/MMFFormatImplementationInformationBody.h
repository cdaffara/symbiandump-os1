// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MMFFORMATIMPLEMENTATIONINFORMATIONBODY_H
#define MMFFORMATIMPLEMENTATIONINFORMATIONBODY_H

#include <e32base.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

/**
This is the Body for CMMFFormatImplementationInformation. For descriptions of methods, see 
CMMFFormatImplementationInformation itself.
*/
NONSHARABLE_CLASS(CMMFFormatImplementationInformation::CBody) : public CBase
	{
public:
	/**
	Constructs a new format implementation information body object.

	@return The new object created.
	*/	
	static CBody* NewL();

	/**
	Constructs a new format implementation information body object.

	@return The new object created. Note that this will be on the cleanup stack.
	*/	
	static CBody* NewLC();

	/** 
	Destructor 
	*/
	virtual ~CBody();

	// public functions forwarded from CMMFFormatImplementationInformation
	const CDesC8Array& SupportedFileExtensions() const;
	const CDesC8Array& SupportedMimeTypes() const;
	const CDesC8Array& SupportedHeaderData() const;
	TBool SupportsFileExtension(const TDesC8& aFileExtension) const;
	TBool SupportsMimeType(const TDesC8& aMimeType) const;
	TBool SupportsHeaderDataL(const TDesC8& aHeaderData) const;
	TBool SupportsCustomInterfaces() const;
	
	/** 
	Adds a file extension to the array of extensions supported by this format. 
	*/
	void AddFileExtensionL(const TDesC8& aData);

	/**
	Adds a mime type to the array of types supported by this format.
	*/
	void AddMimeTypeL(const TDesC8& aData);

	/** 
	Adds a chunk of header data to the array of chunks supported by this format.
	*/
	void AddHeaderDataL(const TDesC8& aData);
	
	/** 
	Sets the flag that indicates whether custom interfaces are supported.
	*/
	void SetSupportsCustomInterfaces(const TBool aSupportsCustomInterfaces);

protected:

	/** 
	Constructor 
	*/
	CBody();
	
	/** 
	2nd phase constructor 
	*/	
	void ConstructL();
	
private:
	/** 
	The file extensions supported by this format. 
	*/
	CDesC8Array* iFileExtensions;

	/** 
	The mime types supported by this format. 
	*/
	CDesC8Array* iMimeTypes;

	/** 
	The header data chunks supported by this format. 
	*/	
	CDesC8Array* iHeaderData;

	/** 
	Indicates whether this format supports custom interfaces
	*/	
	TBool iSupportsCustomInterfaces;
	};

#endif
