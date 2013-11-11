// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the body of the MMF recognizer utility class
// 
//

#include <badesca.h>

#include <mmf/common/mmfcontrollerpluginresolver.h>
#include "RecMmfUtilBody.h"
#include <mmf/server/mmfdatasourcesink.hrh>


// MMF Recognizer Utility class body constructor
CMmfRecognizerUtil::CBody::CBody():
	CActive(CActive::EPriorityStandard)
	{
	}

CMmfRecognizerUtil::CBody::~CBody()
	{
	Cancel();
	iEcomSession.Close();
	iControllers.ResetAndDestroy();
	}

// Request ECom to be notified when interface implementation registration 
// data changes so that we can refresh the list of interface implementations.
void CMmfRecognizerUtil::CBody::StartNotification()
	{
	iEcomSession.NotifyOnChange(iStatus);
	SetActive();
	}

// build a list of interface implementation objects
void CMmfRecognizerUtil::CBody::BuildControllerListL()
	{
	iControllers.ResetAndDestroy();
	CMMFControllerPluginSelectionParameters* cSelect = 
		CMMFControllerPluginSelectionParameters::NewLC();
	
	CMMFFormatSelectionParameters* fSelect = 
		CMMFFormatSelectionParameters::NewLC();

	cSelect->SetRequiredPlayFormatSupportL(*fSelect);

	// Set the media ids
	RArray<TUid> mediaIds;
	CleanupClosePushL(mediaIds);

	User::LeaveIfError(mediaIds.Append(KUidMediaTypeAudio));
	User::LeaveIfError(mediaIds.Append(KUidMediaTypeVideo));
	cSelect->SetMediaIdsL(mediaIds, CMMFPluginSelectionParameters::EAllowOtherMediaIds);
																	
	cSelect->ListImplementationsL(iControllers);

	// Clean up
	CleanupStack::PopAndDestroy(3, cSelect);// mediaIds, fSelect, cSelect
	}

void CMmfRecognizerUtil::CBody::ConstructL()
	{
	BuildControllerListL();

	CActiveScheduler::Add(this);

	iEcomSession = REComSession::OpenL();

	// request notification from ECOM of any file system changes
	StartNotification();
	}

void CMmfRecognizerUtil::CBody::RunL()
	{
	BuildControllerListL();
	StartNotification();
	}

void CMmfRecognizerUtil::CBody::DoCancel()
	{
	if (iStatus == KRequestPending)
		iEcomSession.CancelNotifyOnChange(iStatus);
	}

/* 	Determine whether the supplied data header on its own is recognized,
	or the data header plus the file suffix together are recognized,
	and if so return the associated MIME type.
	
	@param	aFileName
			The name of the file
	@param	aImageData
			A descriptor containing the header
	@param	aMimeType
			A user-supplied descriptor in which the MIME type is returned
	@leave	KErrUnderflow 
			Not enough data in descriptor to identify whether it is supported
			by any plugin.
	@leave	This method may also leave with one of the system-wide error codes.
	@return	EMatchNone if a match was not found.
			EMatchData if a data match was found.
			EMatchName if a data and file suffix match was found.
	@post	If recognized, the caller's descriptor is filled with the MIME types
*/
CMmfRecognizerUtil::TMatchLevel CMmfRecognizerUtil::CBody::GetMimeTypeL(const TDesC& aFileName, const TDesC8& aImageData, TDes8& aMimeType)
	{
	CMmfRecognizerUtil::TMatchLevel matchLevel = EMatchNone;

	//Get the suffix
	TParse fileName;
	
	// aFileName's length could be greater than KMaxFileName
	// so don't use TFileName here
	TPtrC fName(aFileName);
	if (aFileName.Match(_L("::*")) == 0)
 		{
		fName.Set(aFileName.Mid(2));
 		}
	User::LeaveIfError(fileName.Set(fName, NULL, NULL));

	HBufC8* fileSuffix = NULL;
	if(fileName.ExtPresent())
		{
		TPtrC suffixPtr(fileName.Ext());
		fileSuffix = HBufC8::NewLC(suffixPtr.Length());
		fileSuffix->Des().Copy(suffixPtr);
		}

	// loop through every supported Format of every controller 
	// until we find one that matches the data header on its own,
	// or the data header plus the file suffix together
	for (TInt nController = 0, countControllers = iControllers.Count(); nController < countControllers; nController++)
		{
		CMMFControllerImplementationInformation* controllerImpInfo = iControllers[nController];

		for (TInt nPlayFormat = 0, countFormats = controllerImpInfo->PlayFormats().Count(); nPlayFormat < countFormats; nPlayFormat++)
			{
			const CMMFFormatImplementationInformation* formatImpInfo = 
				controllerImpInfo->PlayFormats()[nPlayFormat];
			if (formatImpInfo->SupportsHeaderDataL(aImageData))
				{
				const CDesC8Array& supportedMimeTypes = formatImpInfo->SupportedMimeTypes();
				if (supportedMimeTypes.Count() > 0)
					{
					CMmfRecognizerUtil::TMatchLevel previousMatchLevel = matchLevel;
					if(fileSuffix && formatImpInfo->SupportsFileExtension(*fileSuffix))
						matchLevel = EMatchName;
					else
						matchLevel = EMatchData;
					// Use first data match but update if suffix match is found
					if(matchLevel!=previousMatchLevel)
						aMimeType = supportedMimeTypes[0]; // just return the first mime type in the array
					}
				}
			if(matchLevel==EMatchName || (!fileSuffix && matchLevel==EMatchData))
				break;
			}
		if(matchLevel==EMatchName || (!fileSuffix && matchLevel==EMatchData))
			break;
		}

	if(fileSuffix)
		CleanupStack::PopAndDestroy(fileSuffix);

	return matchLevel;
	}

// Static factory constructor. Uses two phase
// construction and leaves nothing on the cleanup stack
// @leave KErrNoMemory
// @return A pointer to the newly created CMmfRecognizerUtilBody object
CMmfRecognizerUtil::CBody* CMmfRecognizerUtil::CBody::NewL()
	{
	CBody* self=new (ELeave) CBody();   
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Get all the mime types supported by MMF
// @param	aMimeTypes
//          A caller-supplied array of descriptors.
// @leave	This method may leave with one of the system-wide error codes.
// @post    The caller's array is filled with the supported MIME types
void CMmfRecognizerUtil::CBody::GetMimeTypesL(CDesC8Array* aMimeTypes)
	{

	CMMFControllerPluginSelectionParameters* cSelect = 
												CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the play format selection parameters to be blank  
	//  - format support Is only retrieved if requested.
	cSelect->SetRequiredPlayFormatSupportL(*fSelect);

	// Set the media ids
	RArray<TUid> mediaIds;
	CleanupClosePushL(mediaIds);	
	User::LeaveIfError(mediaIds.Append(KUidMediaTypeVideo));
	User::LeaveIfError(mediaIds.Append(KUidMediaTypeAudio));
	cSelect->SetMediaIdsL(mediaIds, CMMFPluginSelectionParameters::EAllowOtherMediaIds);
																		   
	// Array to hold all the controller plugins
	RMMFControllerImplInfoArray controllers; 

	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);

	for (TInt nController = 0, controllerCount = controllers.Count(); nController < controllerCount; nController++)
		{
		for (TInt nPlayFormat = 0, formatCount = controllers[nController]->PlayFormats().Count(); nPlayFormat < formatCount; nPlayFormat++)
			{
			const CMMFFormatImplementationInformation* formatImpInfo = controllers[nController]->PlayFormats()[nPlayFormat];
			const CDesC8Array& supportedMimeTypes = formatImpInfo->SupportedMimeTypes();
			for (TInt nMimeType = 0, mimeTypeCount = supportedMimeTypes.Count(); nMimeType < mimeTypeCount; nMimeType++)
				{
				aMimeTypes->AppendL(supportedMimeTypes[nMimeType]);
				}
			}
		}
					
	// Clean up
	CleanupStack::PopAndDestroy(4, cSelect);//controllers, mediaIds, fSelect, cSelect
	}




 
