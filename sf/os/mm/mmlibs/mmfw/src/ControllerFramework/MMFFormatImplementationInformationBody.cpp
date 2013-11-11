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

#include <badesca.h>
#include "MMFFormatImplementationInformationBody.h"

const TInt KDesCArrayGranularity = 1;

CMMFFormatImplementationInformation::CBody* CMMFFormatImplementationInformation::CBody::NewL()
	{
	CMMFFormatImplementationInformation::CBody* self = CMMFFormatImplementationInformation::CBody::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CMMFFormatImplementationInformation::CBody* CMMFFormatImplementationInformation::CBody::NewLC()
	{
	CMMFFormatImplementationInformation::CBody* self = new(ELeave) CMMFFormatImplementationInformation::CBody();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMMFFormatImplementationInformation::CBody::CBody()
	{
	}

CMMFFormatImplementationInformation::CBody::~CBody()
	{
	delete iFileExtensions;
	delete iMimeTypes;
	delete iHeaderData;
	}
	
void CMMFFormatImplementationInformation::CBody::ConstructL()
	{
	// Create the descriptor arrays
	iFileExtensions = new(ELeave) CDesC8ArrayFlat(KDesCArrayGranularity);
	iMimeTypes = new(ELeave) CDesC8ArrayFlat(KDesCArrayGranularity);
	iHeaderData = new(ELeave) CDesC8ArrayFlat(KDesCArrayGranularity);	
	}

void CMMFFormatImplementationInformation::CBody::AddFileExtensionL(const TDesC8& aData)
	{
	// Insert the new file extension into the array
	iFileExtensions->InsertIsqL(aData, ECmpFolded);//ensures there are no repeated entries
	}

void CMMFFormatImplementationInformation::CBody::AddMimeTypeL(const TDesC8& aData)
	{
	// Insert the new file extension into the array
	TInt position;
	if (iMimeTypes->Find(aData, position) != 0)
		iMimeTypes->AppendL(aData);//ensures there are no repeated entries
	}

void CMMFFormatImplementationInformation::CBody::AddHeaderDataL(const TDesC8& aData)
	{
	// Insert the new file extension into the array, only if its not empty
	if (aData.Length() > 0)
		{
		iHeaderData->InsertIsqL(aData, ECmpFolded);//ensures there are no repeated entries
		}
	}

void CMMFFormatImplementationInformation::CBody::SetSupportsCustomInterfaces(const TBool aSupportsCustomInterfaces)
	{
	iSupportsCustomInterfaces = aSupportsCustomInterfaces;
	}

const CDesC8Array& CMMFFormatImplementationInformation::CBody::SupportedFileExtensions() const
	{
	return *iFileExtensions;
	}

const CDesC8Array& CMMFFormatImplementationInformation::CBody::SupportedMimeTypes() const
	{
	return *iMimeTypes;
	}

const CDesC8Array& CMMFFormatImplementationInformation::CBody::SupportedHeaderData() const
	{
	return *iHeaderData;
	}


TBool CMMFFormatImplementationInformation::CBody::SupportsFileExtension(const TDesC8& aFileExtension) const
	{
	TInt position;
	TInt error = iFileExtensions->FindIsq(aFileExtension, position, ECmpFolded);
	return (error==KErrNone);
	}

TBool CMMFFormatImplementationInformation::CBody::SupportsMimeType(const TDesC8& aMimeType) const
	{
	TInt position;
	TInt error = iMimeTypes->Find(aMimeType, position, ECmpFolded);
	return (error==KErrNone);
	}

TBool CMMFFormatImplementationInformation::CBody::SupportsHeaderDataL(const TDesC8& aHeaderData) const
	{
	TBool matchFound = EFalse;
	// Search aHeaderData for each chunk of header data in iHeaderData
	for (TInt i=0; i<iHeaderData->Count(); i++)
		{
		TPtrC8 headerData = (*iHeaderData)[i];
		// search for match section in the main buffer.
		// ...by putting it in wildcards!
		HBufC8* wildMatch = HBufC8::NewLC( headerData.Length() + 1) ;
		TPtr8 wildMatchPtr = wildMatch->Des() ;
		wildMatchPtr.Copy( headerData ) ;
		_LIT8(KStar, "*");
		wildMatchPtr.Append( KStar ) ;
		if (aHeaderData.MatchF(*wildMatch) != KErrNotFound)
			{
			matchFound = ETrue;
			}
		CleanupStack::PopAndDestroy(wildMatch);//wildMatch
		if (matchFound)
			break;
		}
	return matchFound;
	}

TBool CMMFFormatImplementationInformation::CBody::SupportsCustomInterfaces() const
	{
	return iSupportsCustomInterfaces;
	}

