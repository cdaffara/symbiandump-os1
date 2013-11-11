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

#include <badesca.h>
#include <utf.h>
#include <uri8.h>
#include <uriutils.h>
#include <mmf/plugin/mmfplugininterfaceuids.hrh>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include "mmfmatchdata.h"
#include <mmf/server/mmfdatasourcesink.hrh>
#include "MMFFormatImplementationInformationBody.h"
#include <mm/mmpluginutils.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/taggeddataparser.h>
#endif

_LIT8(KSupplier, "<s>");
_LIT8(KMediaId, "<i>");
_LIT8(KUriScheme,"<u>");
_LIT8(KNonNetwork,"<n>");
_LIT8(KPlayFormatCollectionUid, "<p>");
_LIT8(KRecordFormatCollectionUid, "<r>");
_LIT8(KFormatFileExtension, "<e>");
_LIT8(KFormatMimeType, "<m>");
_LIT8(KFormatHeaderData, "<h>");
_LIT8(KHeapSize, "<a>");
_LIT8(KCustomInterfaceSupport, "<c>");
_LIT8(KSecureDRMProcessMode, "<d>");
_LIT8(KStackSize, "<t>");
_LIT8(KTagMatch, "*<?>*");
_LIT8(KTagYes,"yes");

const TInt KTagLength = 3;

const TInt KMaxExtLen = 5 ;
const TInt KDesCArrayGranularity = 1;

const TInt KUriPriorityHigh = 3;
const TInt KUriPriorityMedium = 2;
const TInt KUriPriorityLow = 1;
const TInt KUriPriorityNone = 0;

static const TUid KUidInterfaceFormatDecode = {KMmfUidPluginInterfaceFormatDecode};
static const TUid KUidInterfaceFormatEncode = {KMmfUidPluginInterfaceFormatEncode};
static const TUid KUidInterfaceMMFController = {KMmfUidPluginInterfaceController};


EXPORT_C CMMFFormatSelectionParameters* CMMFFormatSelectionParameters::NewL()
	{
	CMMFFormatSelectionParameters* s = CMMFFormatSelectionParameters::NewLC();
	CleanupStack::Pop(s);
	return s;
	}

EXPORT_C CMMFFormatSelectionParameters* CMMFFormatSelectionParameters::NewLC()
	{
	CMMFFormatSelectionParameters* s = new(ELeave) CMMFFormatSelectionParameters;
	CleanupStack::PushL(s);
	return s;
	}

CMMFFormatSelectionParameters* CMMFFormatSelectionParameters::NewL(const CMMFFormatSelectionParameters& aParams)
	{
	CMMFFormatSelectionParameters* s = CMMFFormatSelectionParameters::NewLC();
	s->ConstructL(aParams);
	CleanupStack::Pop(s);
	return s;
	}

void CMMFFormatSelectionParameters::ConstructL(const CMMFFormatSelectionParameters& aParams)
	{
	iMatchReqData = CMatchData::CreateL();
	iMatchReqData->SetMatchDataL(aParams.MatchData());
	iMatchReqData->SetMatchUriSchemeL(aParams.MatchUriScheme());
	iMatchDataType = aParams.MatchDataType();
	}

CMMFFormatSelectionParameters::~CMMFFormatSelectionParameters()
	{
	delete iMatchReqData;
	iMatchReqData= NULL;
	}

CMMFFormatSelectionParameters::CMMFFormatSelectionParameters()
	{
	iMatchDataType = EMatchAny;
	}
	
EXPORT_C void CMMFFormatSelectionParameters::SetMatchToFileNameL(const TDesC& aFileName)
	{
	delete iMatchReqData;
	iMatchReqData = NULL;
	iMatchDataType = EMatchAny;	
	// Extract the extension from the data passed in

	// Parse the path and extract the extension
	_LIT( KDot, "." ) ;
	_LIT8( KDot8, "." );

	// If there is no dot "." in aFileName then assume that we have been passed the extension only (if KMaxExtLen or less)
	if ( (aFileName.Length() <= KMaxExtLen) && (aFileName.Find( KDot ) == KErrNotFound) )
		{
		RBuf8 temp;
		CleanupClosePushL(temp);
		temp.CreateL(aFileName.Length()+1);
		User::LeaveIfError(CnvUtfConverter::ConvertFromUnicodeToUtf8(temp, aFileName));
		temp.Insert(0,KDot8);
		
		iMatchReqData = CMatchData::CreateL();
		iMatchReqData->SetMatchDataL(temp);
		
		CleanupStack::PopAndDestroy(&temp);
		
		}
	else if ( aFileName.Find( KDot ) == 0 )  // the first character is dot so assume extension only
		{
		RBuf8 temp;
		CleanupClosePushL(temp);
		temp.CreateL(aFileName.Length());
		User::LeaveIfError(CnvUtfConverter::ConvertFromUnicodeToUtf8(temp, aFileName));
			
		iMatchReqData = CMatchData::CreateL();
		iMatchReqData->SetMatchDataL(temp);
		
		CleanupStack::PopAndDestroy(&temp);
		
		}
	else // We have been given the whole filename.  Use TParse to extract the extension.
		{
		TParse parser ;
		parser.Set( aFileName, NULL, NULL ) ;
		if ( !( parser.NamePresent() ) )
			User::Leave( KErrBadName ) ;
		if ( !( parser.PathPresent() ) )
			{
			RFs fsSession ;
			User::LeaveIfError(fsSession.Connect());
			TInt error = fsSession.Parse(aFileName, parser);
			fsSession.Close();
			User::LeaveIfError(error);
			}
		// Parser should now have the full filename and path
		TPtrC extension = parser.Ext();
		
		RBuf8 temp;
		CleanupClosePushL(temp);
		temp.CreateL(extension.Length());
		User::LeaveIfError(CnvUtfConverter::ConvertFromUnicodeToUtf8(temp, extension));
			
		iMatchReqData = CMatchData::CreateL();
		iMatchReqData->SetMatchDataL(temp);
		
		CleanupStack::PopAndDestroy(&temp);
		
		}

	// If we're here, we must now have the file extension
	iMatchDataType = EMatchFileExtension;
	}
	
EXPORT_C void CMMFFormatSelectionParameters::SetMatchToUriL(const TDesC& aUrl)
	{
	delete iMatchReqData;
	iMatchReqData = NULL;
	iMatchDataType = EMatchAny;	
	
	CUri8* uri = UriUtils::CreateUriL(aUrl); //Converts the TDesC16 aUrl to TDesC8 type
   	CleanupStack::PushL(uri);
  	const TDesC8& path = uri->Uri().Extract(EUriPath);
	
	// Now parse the file name 
   	TInt pos = path.LocateReverse('.');
   
   	if(pos != KErrNotFound) // if not found, then by default match data is NULL
   		{
   		TPtrC8 extension(path.Right(path.Length()-pos));
   		iMatchReqData = CMatchData::CreateL();
   		iMatchReqData->SetMatchDataL(extension);
  		}
   	CleanupStack::PopAndDestroy(uri);
   
   	// If we're here, we must now have the file extension
   	// Use match file extension, because we are matching to the file extension of the file specified by the URI
  	iMatchDataType = EMatchFileExtension;
	}
	
/**
	@publishedPartner
	@prototype
	
	Sets this object to match to uri scheme and file extension specified by a URI.
	
	The Uri scheme and extension are saved in iMatchReqData. Further,iMatchData contains uri extension, 
	iMatchUriScheme contains uri scheme.

	@param  aUri
	        The URI containing the scheme and uri extension to be matched. 

*/	
EXPORT_C void CMMFFormatSelectionParameters::SetMatchToUriSupportL(const TDesC& aUrl)
	{
	delete iMatchReqData;
	iMatchReqData = NULL;
	iMatchDataType = EMatchAny;	
	
	CUri8* uri = UriUtils::CreateUriL(aUrl); //Converts the TDesC16 aUrl to TDesC8 type
	CleanupStack::PushL(uri);
	
	const TDesC8& scheme = uri->Uri().Extract(EUriScheme);  //get the uri scheme
	iMatchReqData = CMatchData::CreateL();
	iMatchReqData->SetMatchUriSchemeL(scheme);
	
	const TDesC8& path = uri->Uri().Extract(EUriPath); 		
	// Now parse the file name 
	TInt pos = path.LocateReverse('.');
	
	if(pos != KErrNotFound) // if not found, by default match data is NULL
		{
		TPtrC8 extension(path.Right(path.Length()-pos));
		iMatchReqData->SetMatchDataL(extension);
 		}
		
	CleanupStack::PopAndDestroy(uri);
	// If we're here, we must now have the uri saved in iMatchData. Now, set match type 
	// to EMatchUri for checking schema and uri extension
	iMatchDataType = EMatchUri;
	}
	


EXPORT_C void CMMFFormatSelectionParameters::SetMatchToMimeTypeL(const TDesC8& aMimeType)
	{
	delete iMatchReqData;
	iMatchReqData = NULL;
	iMatchDataType = EMatchAny;	
	
	iMatchReqData = CMatchData::CreateL();
	iMatchReqData->SetMatchDataL(aMimeType);
	iMatchDataType = EMatchMimeType;
	}

EXPORT_C void CMMFFormatSelectionParameters::SetMatchToHeaderDataL(const TDesC8& aHeaderData)
	{
	delete iMatchReqData;
	iMatchReqData = NULL;
	iMatchDataType = EMatchAny;	
	
	iMatchReqData = CMatchData::CreateL();
	iMatchReqData->SetMatchDataL(aHeaderData);	
	iMatchDataType = EMatchHeaderData;
	}

EXPORT_C const TDesC8& CMMFFormatSelectionParameters::MatchData() const
	{
	if (iMatchReqData)
		{
		return iMatchReqData->MatchData();
		}
	else
		{
		return KNullDesC8;	
		}
	}

/**
	@publishedPartner
	@prototype
	
	Returns the uri scheme used to perform the plugin match.
	
	@return The uri scheme.

*/	
EXPORT_C const TDesC8& CMMFFormatSelectionParameters::MatchUriScheme() const
	{
	if (iMatchReqData)
		{
		return iMatchReqData->MatchUriScheme();
		}
	else
		{
		return KNullDesC8;
		}
	}
	
EXPORT_C CMMFFormatSelectionParameters::TMatchDataType CMMFFormatSelectionParameters::MatchDataType() const
	{
	return iMatchDataType;
	}

CMMFPluginSelectionParameters::CMMFPluginSelectionParameters(TUid aPluginInterfaceUid) :
	iPluginInterfaceUid(aPluginInterfaceUid)
	{
	iPreferredSupplierMatchType = ENoPreferredSupplierMatch;
	iMediaIdMatchType = ENoMediaIdMatch;
	}

CMMFPluginSelectionParameters::~CMMFPluginSelectionParameters()
	{
	delete iPreferredSupplier;
	iMediaIds.Reset();
	iMediaIds.Close();
	}

EXPORT_C void CMMFPluginSelectionParameters::SetPreferredSupplierL(const TDesC& aPreferredSupplier, TPreferredSupplierMatchType aMatchType)
	{
	delete iPreferredSupplier;
	iPreferredSupplier = NULL;
	iPreferredSupplier = aPreferredSupplier.AllocL();
	iPreferredSupplierMatchType = aMatchType;
	}

EXPORT_C void CMMFPluginSelectionParameters::SetMediaIdsL(const RArray<TUid>& aMediaIds, TMediaIdMatchType aMatchType)
	{
	iMediaIds.Reset();
	for (TInt i=0; i<aMediaIds.Count(); i++)
		{
		User::LeaveIfError(iMediaIds.Append(aMediaIds[i]));
		}
	iMediaIdMatchType = aMatchType;
	}

EXPORT_C const TDesC& CMMFPluginSelectionParameters::PreferredSupplier() const
	{
	if (iPreferredSupplier)
		return *iPreferredSupplier;
	else
		return KNullDesC;
	}

EXPORT_C CMMFPluginSelectionParameters::TPreferredSupplierMatchType CMMFPluginSelectionParameters::PreferredSupplierMatchType() const
	{
	return iPreferredSupplierMatchType;
	}

EXPORT_C const RArray<TUid>& CMMFPluginSelectionParameters::MediaIds() const
	{
	return iMediaIds;
	}

EXPORT_C CMMFPluginSelectionParameters::TMediaIdMatchType CMMFPluginSelectionParameters::MediaIdMatchType() const
	{
	return iMediaIdMatchType;
	}

EXPORT_C TUid CMMFPluginSelectionParameters::InterfaceUid() const
	{
	return iPluginInterfaceUid;
	}


TBool CMMFPluginSelectionParameters::CheckMediaIdSupportL(const CMMFPluginImplementationInformation& aPlugin) const
	{
	TBool ret = EFalse;
	switch (MediaIdMatchType())
		{
	case CMMFPluginSelectionParameters::ENoMediaIdMatch:
		// No match required so suitable
		ret = ETrue;
		break;
	case CMMFPluginSelectionParameters::EAllowOtherMediaIds:
		// Just check that the requested media id is supported by the plugin
		{
		for (TInt i=0; i<MediaIds().Count(); i++)
			{
			if (aPlugin.SupportsMediaId(MediaIds()[i]))
				{
				ret = ETrue;
				break;
				}
			}
		break;
		}
	case CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds:
		// Check the media id counts are the same, and that all requested ones are present
		{
		TInt found=0;
		for (TInt i=0; i<MediaIds().Count(); i++)
			{
			if (aPlugin.SupportsMediaId(MediaIds()[i]))
				found++;
			}
		// Check all request mediaIds are present
		if ((found == MediaIds().Count()) && (found == aPlugin.SupportedMediaIds().Count()))
			ret = ETrue;
		break;
		}
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return ret;
	}



EXPORT_C CMMFControllerPluginSelectionParameters* CMMFControllerPluginSelectionParameters::NewL()
	{
	CMMFControllerPluginSelectionParameters* s = CMMFControllerPluginSelectionParameters::NewLC();
	CleanupStack::Pop(s);
	return s;	
	}

EXPORT_C CMMFControllerPluginSelectionParameters* CMMFControllerPluginSelectionParameters::NewLC()
	{
	CMMFControllerPluginSelectionParameters* s = new(ELeave) CMMFControllerPluginSelectionParameters;
	CleanupStack::PushL(s);
	return s;
	}

CMMFControllerPluginSelectionParameters::CMMFControllerPluginSelectionParameters() :
	CMMFPluginSelectionParameters(KUidInterfaceMMFController)
	{
	}

CMMFControllerPluginSelectionParameters::~CMMFControllerPluginSelectionParameters()
	{
	delete iRequiredPlayFormatSupport;
	delete iRequiredRecordFormatSupport;
	}

EXPORT_C void CMMFControllerPluginSelectionParameters::SetRequiredPlayFormatSupportL(const CMMFFormatSelectionParameters& aRequiredSupport)
	{
	delete iRequiredPlayFormatSupport;
	iRequiredPlayFormatSupport = NULL;
	iRequiredPlayFormatSupport = CMMFFormatSelectionParameters::NewL(aRequiredSupport);
	}

EXPORT_C void CMMFControllerPluginSelectionParameters::SetRequiredRecordFormatSupportL(const CMMFFormatSelectionParameters& aRequiredSupport)
	{
	delete iRequiredRecordFormatSupport;
	iRequiredRecordFormatSupport = NULL;
	iRequiredRecordFormatSupport = CMMFFormatSelectionParameters::NewL(aRequiredSupport);
	}

EXPORT_C void CMMFControllerPluginSelectionParameters::ListImplementationsL(RMMFControllerImplInfoArray& aImplementations) const
	{
	aImplementations.ResetAndDestroy();

	RImplInfoPtrArray ecomArray;
	CleanupResetAndDestroyPushL(ecomArray);

	MmPluginUtils::FindImplementationsL(InterfaceUid(), ecomArray);

	TInt index;
	// Create Controller Implementation Information for each entry
	for (index=0; index<ecomArray.Count(); index++)
		{
		CMMFControllerImplementationInformation* c = NULL;
		if (ecomArray[index] == NULL)
			{
			User::Leave(KErrNoMemory);
			}
		TRAPD(error, c = CMMFControllerImplementationInformation::NewL(*(ecomArray[index])));

		if (error == KErrNone)
			{
			CleanupStack::PushL(c);
			
		// If required, get the play and record formats for the controller.
			if (iRequiredPlayFormatSupport)
				{
				c->GetPlayFormatsL();
				}
			if (iRequiredRecordFormatSupport)
				{
				c->GetRecordFormatsL();
				}
			
			// Find out whether this controller matches the client's requirements...
			TBool suitable = EFalse;
			TInt arrayPos; 
			suitable = CheckUriSupport(iRequiredPlayFormatSupport, c, c->PlayFormats());
			if(suitable)
				{
				suitable = CheckUriSupport(iRequiredRecordFormatSupport, c, c->RecordFormats());
				}
			if(suitable)
				{
				MatchImplementationToSelectParamsL(aImplementations, *c, arrayPos);
				}
			else
				{
				arrayPos = -1;	
				}
			
			if (arrayPos >=0)
				{
				// This plugin is suitable - insert it into the array at the suggested position
				User::LeaveIfError(aImplementations.Insert(c, arrayPos));
				CleanupStack::Pop(c);
				}
			else
				{
				// This plugin isn't suitable so just destroy it
				CleanupStack::PopAndDestroy(c);
				}
			}
		else if (error != KErrCorrupt)
			{
			// Ignore the plugin if it is corrupt.  Otherwise, leave.
			// if error !=KErrNone, c hasn't been constructed so it is safe to leave
			User::Leave(error);
			}
		}

	CleanupStack::PopAndDestroy();//ecomArray
	}

void CMMFControllerPluginSelectionParameters::MatchImplementationToSelectParamsL(RMMFControllerImplInfoArray& aImplementations, const CMMFControllerImplementationInformation& aPlugin, TInt& aArrayPos) const
	{
	TBool suitable = EFalse;
	
	// First, check whether the plugin supports the required play formats
	suitable = CheckFormatSupportL(iRequiredPlayFormatSupport, aPlugin.PlayFormats());
	
	// Next, check the record formats
	if (suitable)
		suitable = CheckFormatSupportL(iRequiredRecordFormatSupport, aPlugin.RecordFormats());
		
	// Next, check for correct media id support
	if (suitable)
		suitable = CheckMediaIdSupportL(aPlugin);

	// Finally, calculate the position the plugin should take in aImplementations depending on the preferred supplier and version number.
	if (suitable)
		aArrayPos = CheckPreferredSupplierL(aImplementations, aPlugin);
	else
		aArrayPos = -1;
	}

/**
	Checks the given Controller for uri support. Uri support may be there 
	if either requiredscheme or extension matches with that given controller
	
	@param  aSelectParams 
			Describes the selection parameter which a controller needs to 
			support in order to be selected
			
	@param	aPlugin
			The controller plugin which is checked for uri support 
			
	@param	aFormats
			The play or record formats looked for extension match
*/	
TBool CMMFControllerPluginSelectionParameters::CheckUriSupport(CMMFFormatSelectionParameters* aSelectParams, CMMFControllerImplementationInformation* aPlugin, const RMMFFormatImplInfoArray& aFormats) const
	{
	// If aSelectParams are NULL (ie none were set) then the plugin must be suitable!
	if (aSelectParams == NULL)
		return ETrue;
	
	//If EMatchUri not set then the plugin might be suitable!
	if(aSelectParams->MatchDataType() != CMMFFormatSelectionParameters::EMatchUri)
		{
		return ETrue;
		}
		
	TBool suitable = EFalse;
	TInt index;
	
	//If <n>yes is there in the aPlugin's opaque data, 
	//URI support not assumed. So, don't load the controller
	if(!aPlugin->SupportsNetworkCapability())
		{
		return EFalse;	
		}

    //the uri scheme to be matched for
	if (aPlugin->SupportsUriScheme(aSelectParams->MatchUriScheme()))
		{
		aPlugin->SetUriPriority(KUriPriorityMedium);
		suitable = ETrue;
		}
	else
		{
		//if other uri scheme support declared, but not the required one,
		// even then controller not supported
		if(aPlugin->SupportedUriSchemes().Count() > 0)
			{
			return EFalse;		
			}
		}
	
		
	//then match Uri extension. .
	
	//if ctrl has no network capability, then lowest priority given on matching file extension
	for (index=0;index<aFormats.Count();index++)
		{
		if (aFormats[index]-> SupportsFileExtension(aSelectParams->MatchData()))
			{
			if(!suitable)
				{
				aPlugin->SetUriPriority(KUriPriorityLow);	
				}
			else
				{
				aPlugin->SetUriPriority(KUriPriorityHigh);
				}
			
			suitable = ETrue;
			break;
			}
		}
	
	return suitable;
	}

TInt CMMFControllerPluginSelectionParameters::CheckPreferredSupplierL(RMMFControllerImplInfoArray& aImplementations, const CMMFControllerImplementationInformation& aPlugin) const
	{
	// Set the return value to indicated the plugin is not suitable.
	TBool pluginSuitable = EFalse;
	TBool needToPlaceInVersionOrder = EFalse;
	TBool needToPlaceInUriPriorityOrder = EFalse;

	if((iRequiredPlayFormatSupport && (iRequiredPlayFormatSupport->MatchDataType() == CMMFFormatSelectionParameters::EMatchUri))
	 || (iRequiredRecordFormatSupport && (iRequiredRecordFormatSupport->MatchDataType() == CMMFFormatSelectionParameters::EMatchUri)))
		{
		needToPlaceInUriPriorityOrder = ETrue;
		}
	
	switch (PreferredSupplierMatchType())
		{
	case ENoPreferredSupplierMatch:
		// No match, so suitable.
		pluginSuitable = ETrue;
	    break;
	case EPreferredSupplierPluginsFirstInList:
		pluginSuitable = ETrue;
		if (aPlugin.SupportsSupplier(PreferredSupplier()))
			needToPlaceInVersionOrder = ETrue;
		break;
	case EOnlyPreferredSupplierPluginsReturned:
		if (aPlugin.SupportsSupplier(PreferredSupplier()))
			{
			pluginSuitable = ETrue;
			needToPlaceInVersionOrder = ETrue;
			}
		break;
	default:
		User::Leave(KErrNotSupported);
		}

	TInt arrayPos;
			
	if (!pluginSuitable)
		{
		arrayPos = -1;
		}
		
	else
		{
		if(needToPlaceInUriPriorityOrder)
			{
			arrayPos = aImplementations.Count();
			// Cycle through aImplementations to find the first plugin with a uri priority LOWER than aPlugin's
			for (TInt i=0; i<aImplementations.Count(); i++)
				{
				CMMFControllerImplementationInformation* c = aImplementations[i];
				
				if(PreferredSupplierMatchType() == ENoPreferredSupplierMatch) //case 1
					{
					/**
					Place the plugin based on its Uri priority.
					Arrange the plugins in Decreasing order of their priority. In case if two or more plugins 
					have similar priority, append the next one at the end. 
					*/
					if (c->UriPriority() < aPlugin.UriPriority())
						{
						arrayPos = i;//plugin will be inserted before c in the array
						break;
						}
					}
				else
					{
					if(!needToPlaceInVersionOrder)  //case 2
						{
						/**
						This reflects the case EPreferredSupplierPluginsFirstInList and 
						aPlugin supplier not matching.
						Place the plugin based on its Uri Priority after the required suppliers plugin.
						If priority is equal, new plugin will be placed last.
						*/
						if( (c->Supplier() != PreferredSupplier()) && (c->UriPriority() < aPlugin.UriPriority()))
							{
							arrayPos = i;//plugin will be inserted before c in the array
							break;
							}
						}
					else  //case 3
						{
						/** 
						This reflects the case where 
						Supplier matches and EPreferredSupplierPluginsFirstInList is specified.
						OR
						Supplier matches and EOnlyPreferredSupplierPluginsReturned is specified.
						*/
						if(c->Supplier() == PreferredSupplier()) 
							{
							if(c->UriPriority() == aPlugin.UriPriority())
								{
								if(c->Version() <= aPlugin.Version())
									{
									arrayPos = i;//plugin will be inserted before c in the array
									break;
									}
								}
							else 
								{
								if(c->UriPriority() < aPlugin.UriPriority())
									{
									arrayPos = i;//plugin will be inserted before c in the array
									break;
									}
								}
							}
						else
							{
							/**
							This is a case of aImplementations now having unpreferred suppliers 
							when EPreferredSupplierPluginsFirstInList is specified and aPlugin is
							of preferred supplier but least priority.
							*/
							arrayPos = i;//plugin will be inserted before c in the array
							break;
							}
						}
					}
				}
			}
			
		else
			{
			if (!needToPlaceInVersionOrder)  
				{
				/**
				place it at the end.
				*/
				arrayPos = aImplementations.Count();
				}
			else
				{
				// Insert the plugin at the beginning of the array, in version order if possible.
				// Make an assumption: if we've been asked for format support, then only plugins 
				// that support the same format will be in the array - so ordering them by version
				// will be meaningful.  Otherwise, there's no point.
				if ((iRequiredPlayFormatSupport && (iRequiredPlayFormatSupport->MatchDataType() != CMMFFormatSelectionParameters::EMatchAny))
					|| (iRequiredRecordFormatSupport && (iRequiredRecordFormatSupport->MatchDataType() != CMMFFormatSelectionParameters::EMatchAny)))
					{
					// Put the plugin in version order at the beginning of the list

					// Set aArrayPos to the end of the array in case this plugin has the lowest version number
					arrayPos = aImplementations.Count();
					// Cycle through aImplementations to find the first plugin with a version number LOWER than aPlugin
					for (TInt i=0; i<aImplementations.Count(); i++)
						{
						CMMFControllerImplementationInformation* c = aImplementations[i];
						if (c->Supplier() == aPlugin.Supplier())
							{
							if (c->Version() <= aPlugin.Version())
								{
								arrayPos = i;//plugin will be inserted before c in the array
								break;
								}
							}
						else
							{
							arrayPos = i; //c has wrong supplier so this plugin must go before c
							break;
							}
						}
					}
				else
					{
					// We can't use the version numbers meaningfully, so just put this plugin
					// at the top of the list.
					arrayPos = 0;
					}
				}
			}
		}
	
	return arrayPos;
	}

TBool CMMFControllerPluginSelectionParameters::CheckFormatSupportL(CMMFFormatSelectionParameters* aSelectParams, const RMMFFormatImplInfoArray& aFormats) const
	{
	// If aSelectParams are NULL (ie none were set) then the plugin must be suitable!
	if (aSelectParams == NULL)
		return ETrue;

	TBool suitable = EFalse;

	// Check all the formats in aFormats.  If any support the required data type, return ETrue.
	TInt index;
	switch (aSelectParams->MatchDataType())
		{
	case CMMFFormatSelectionParameters::EMatchAny:
		// All plugins intrinsically match this!
		suitable = ETrue;
		break;
	case CMMFFormatSelectionParameters::EMatchFileExtension:
		for (index=0;index<aFormats.Count();index++)
			{
			if (aFormats[index]->SupportsFileExtension(aSelectParams->MatchData()))
				{
				suitable = ETrue;
				break;
				}
			}
		break;
	case CMMFFormatSelectionParameters::EMatchMimeType:
		for (index=0;index<aFormats.Count();index++)
			{
			if (aFormats[index]->SupportsMimeType(aSelectParams->MatchData()))
				{
				suitable = ETrue;
				break;
				}
			}
		break;
	case CMMFFormatSelectionParameters::EMatchHeaderData:
		for (index=0;index<aFormats.Count();index++)
			{
			if (aFormats[index]->SupportsHeaderDataL(aSelectParams->MatchData()))
				{
				suitable = ETrue;
				break;
				}
			}
		break;
		
	case CMMFFormatSelectionParameters::EMatchUri:
		{
		suitable = ETrue; //if uri match specifically looked , then that has been already matched in a CheckUriSupport()
		break;	
		}
		

	default:
		User::Leave(KErrNotSupported);
		};

	return suitable;
	}



EXPORT_C CMMFControllerSecureDrmPluginSelectionParameters* CMMFControllerSecureDrmPluginSelectionParameters::NewL()
	{
	CMMFControllerSecureDrmPluginSelectionParameters* s = CMMFControllerSecureDrmPluginSelectionParameters::NewLC();
	CleanupStack::Pop(s);
	return s;	
	}

EXPORT_C CMMFControllerSecureDrmPluginSelectionParameters* CMMFControllerSecureDrmPluginSelectionParameters::NewLC()
	{
	CMMFControllerSecureDrmPluginSelectionParameters* s = new(ELeave) CMMFControllerSecureDrmPluginSelectionParameters;
	CleanupStack::PushL(s);
	return s;
	}

CMMFControllerSecureDrmPluginSelectionParameters::CMMFControllerSecureDrmPluginSelectionParameters() :
	CMMFControllerPluginSelectionParameters()
	{
	}

EXPORT_C void CMMFControllerSecureDrmPluginSelectionParameters::ListImplementationsL(RMMFControllerImplInfoArray& aImplementations) const
	{
	CMMFControllerPluginSelectionParameters::ListImplementationsL(aImplementations);
	TInt pluginsCount = aImplementations.Count();
	for (TInt i = pluginsCount - 1; i >= 0; i--)
		{
		CMMFControllerImplementationInformation* c = aImplementations[i];
		if (!c->SupportsSecureDRMProcessMode())
			{
			aImplementations.Remove(i);
			delete c;
			}
		}
	aImplementations.Compress();
	}



CMMFFormatPluginSelectionParameters::CMMFFormatPluginSelectionParameters(TUid aInterfaceUid) :
	CMMFPluginSelectionParameters(aInterfaceUid)
	{
	}

CMMFFormatPluginSelectionParameters::~CMMFFormatPluginSelectionParameters()
	{
	delete iRequiredFormatSupport;
	}

EXPORT_C void CMMFFormatPluginSelectionParameters::SetRequiredFormatSupportL(const CMMFFormatSelectionParameters& aRequiredSupport)
	{
	delete iRequiredFormatSupport;
	iRequiredFormatSupport = NULL;
	iRequiredFormatSupport = CMMFFormatSelectionParameters::NewL(aRequiredSupport);
	}

EXPORT_C void CMMFFormatPluginSelectionParameters::ListImplementationsL(RMMFFormatImplInfoArray& aImplementations) const
	{
	aImplementations.ResetAndDestroy();

	RImplInfoPtrArray ecomArray;
	CleanupResetAndDestroyPushL(ecomArray);

	MmPluginUtils::FindImplementationsL(InterfaceUid(), ecomArray);


	TInt index;
	// Create Format Implementation Information for each entry
	for (index=0; index<ecomArray.Count(); index++)
		{
		CMMFFormatImplementationInformation* c = NULL;
		TRAPD(error, c = CMMFFormatImplementationInformation::NewL(*(ecomArray[index])));

		if (error == KErrNone)
			{
			CleanupStack::PushL(c);
			// Find out whether this format matches the client's requirements...
			TInt arrayPos;
			MatchImplementationToSelectParamsL(aImplementations, *c, arrayPos);
			if (arrayPos >=0)
				{
				// This plugin is suitable - insert it into the array at the suggested position
				User::LeaveIfError(aImplementations.Insert(c, arrayPos));
				CleanupStack::Pop(c);
				}
			else
				{
				// This plugin isn't suitable so just destroy it
				CleanupStack::PopAndDestroy(c);
				}
			}
		else if (error != KErrCorrupt)
			{
			// Ignore the plugin if it is corrupt.  Otherwise, leave.
			// if error !=KErrNone, c hasn't been constructed so it is safe to leave
			User::Leave(error);
			}
		}

	CleanupStack::PopAndDestroy();//ecomArray
	}

void CMMFFormatPluginSelectionParameters::MatchImplementationToSelectParamsL(RMMFFormatImplInfoArray& aImplementations, const CMMFFormatImplementationInformation& aPlugin, TInt& aArrayPos) const
	{
	TBool suitable = EFalse;

	// First, check whether the plugin supports the required play formats
	suitable = CheckFormatSupportL(aPlugin);

	// Next, check for correct media id support
	if (suitable)
		suitable = CheckMediaIdSupportL(aPlugin);

	// Finally, calculate the position the plugin should take in aImplementations depending on the preferred supplier and version number.
	if (suitable)
		aArrayPos = CheckPreferredSupplierL(aImplementations, aPlugin);
	else
		aArrayPos = -1;
	}

TInt CMMFFormatPluginSelectionParameters::CheckPreferredSupplierL(RMMFFormatImplInfoArray& aImplementations, const CMMFFormatImplementationInformation& aPlugin) const
	{
	// Set the return value to indicated the plugin is not suitable.
	TBool pluginSuitable = EFalse;
	TBool needToPlaceInVersionOrder = EFalse;

	switch (PreferredSupplierMatchType())
		{
	case ENoPreferredSupplierMatch:
		// No match, so suitable.
		pluginSuitable = ETrue;
		break;
	case EPreferredSupplierPluginsFirstInList:
		pluginSuitable = ETrue;
		if (aPlugin.SupportsSupplier(PreferredSupplier()))
			needToPlaceInVersionOrder = ETrue;
		break;
	case EOnlyPreferredSupplierPluginsReturned:
		if (aPlugin.SupportsSupplier(PreferredSupplier()))
			{
			pluginSuitable = ETrue;
			needToPlaceInVersionOrder = ETrue;
			}
		break;
	default:
		User::Leave(KErrNotSupported);
		}


	TInt arrayPos;

	if (!pluginSuitable)
		{
		arrayPos = -1;
		}
	else
		{
		if (!needToPlaceInVersionOrder)
			{
			arrayPos = aImplementations.Count();
			}
		else
			{
			// Insert the plugin at the beginning of the array, in version order if possible.
			// Make an assumption: if we've been asked for format support, then only plugins 
			// that support the same format will be in the array - so ordering them by version
			// will be meaningful.  Otherwise, there's no point.
			if (iRequiredFormatSupport && (iRequiredFormatSupport->MatchDataType() != CMMFFormatSelectionParameters::EMatchAny))
				{
				// Put the plugin in version order at the beginning of the list

				// Set aArrayPos to the end of the array in case this plugin has the lowest version number
				arrayPos = aImplementations.Count();
				// Cycle through aImplementations to find the first plugin with a version number LOWER than aPlugin
				for (TInt i=0; i<aImplementations.Count(); i++)
					{
					CMMFFormatImplementationInformation* c = aImplementations[i];
					if ((c->Supplier() == aPlugin.Supplier())
						&& (c->Version() <= aPlugin.Version()))
						{
						arrayPos = i;//plugin will be inserted before c in the array
						break;
						}
					}
				}
			else
				{
				// We can't use the version numbers meaningfully, so just put this plugin
				// at the top of the list.
				arrayPos = 0;
				}
			}
		}
	return arrayPos;
	}

TBool CMMFFormatPluginSelectionParameters::CheckFormatSupportL(const CMMFFormatImplementationInformation& aPlugin) const
	{
	// If iRequiredFormatSupport is NULL (ie no requirements set) then the plugin must be suitable!
	if (iRequiredFormatSupport == NULL)
		return ETrue;

	TBool suitable = EFalse;

	switch (iRequiredFormatSupport->MatchDataType())
		{
	case CMMFFormatSelectionParameters::EMatchAny:
		suitable = ETrue;
		break;
	case CMMFFormatSelectionParameters::EMatchFileExtension:
		if (aPlugin.SupportsFileExtension(iRequiredFormatSupport->MatchData()))
			suitable = ETrue;
		break;
	case CMMFFormatSelectionParameters::EMatchMimeType:
		if (aPlugin.SupportsMimeType(iRequiredFormatSupport->MatchData()))
			suitable = ETrue;
		break;
	case CMMFFormatSelectionParameters::EMatchHeaderData:
		if (aPlugin.SupportsHeaderDataL(iRequiredFormatSupport->MatchData()))
			suitable = ETrue;
		break;
	default:
		User::Leave(KErrNotSupported);
		};

	return suitable;
	}


EXPORT_C CMMFFormatEncodePluginSelectionParameters* CMMFFormatEncodePluginSelectionParameters::NewL()
	{
	CMMFFormatEncodePluginSelectionParameters* s = CMMFFormatEncodePluginSelectionParameters::NewLC();
	CleanupStack::Pop(s);
	return s;
	}

EXPORT_C CMMFFormatEncodePluginSelectionParameters* CMMFFormatEncodePluginSelectionParameters::NewLC()
	{
	CMMFFormatEncodePluginSelectionParameters* s = new(ELeave) CMMFFormatEncodePluginSelectionParameters;
	CleanupStack::PushL(s);
	return s;
	}

CMMFFormatEncodePluginSelectionParameters::CMMFFormatEncodePluginSelectionParameters() :
	CMMFFormatPluginSelectionParameters(KUidInterfaceFormatEncode)
	{
	}


EXPORT_C CMMFFormatDecodePluginSelectionParameters* CMMFFormatDecodePluginSelectionParameters::NewL()
	{
	CMMFFormatDecodePluginSelectionParameters* s = CMMFFormatDecodePluginSelectionParameters::NewLC();
	CleanupStack::Pop(s);
	return s;
	}

EXPORT_C CMMFFormatDecodePluginSelectionParameters* CMMFFormatDecodePluginSelectionParameters::NewLC()
	{
	CMMFFormatDecodePluginSelectionParameters* s = new(ELeave) CMMFFormatDecodePluginSelectionParameters;
	CleanupStack::PushL(s);
	return s;
	}

CMMFFormatDecodePluginSelectionParameters::CMMFFormatDecodePluginSelectionParameters() :
	CMMFFormatPluginSelectionParameters(KUidInterfaceFormatDecode)
	{
	}





void TaggedDataParser::ParseTaggedDataL(const TDesC8& aData, MTaggedDataParserClient& aClient)
	{
	TPtrC8 data(aData);
	TInt readPosition = 0;
	TBool moreData = data.Length() ? ETrue : EFalse;
	while (moreData)
		{
		// Assumes that this segment will begin with a tag
		TPtrC8 restOfData = data.Mid(readPosition);

		TInt endPos = restOfData.MatchF(KTagMatch);
		if (endPos == KErrNotFound)
			User::Leave(KErrCorrupt);

		// extract the tag
		TPtrC8 tag = restOfData.Left(KTagLength);
		
	
		readPosition += KTagLength;

		// Find the next tag
		restOfData.Set(data.Mid(readPosition));
		endPos = restOfData.MatchF(KTagMatch);

		TPtrC8 tagData;
		if (endPos == KErrNotFound)
			{
			// If we didn't find a tag, we must be at the end of the data
			tagData.Set(restOfData);
			readPosition = restOfData.Length();
			moreData = EFalse;
			}
		else
			{
			tagData.Set(restOfData.Left(endPos));
			readPosition += endPos;
			}

		aClient.ProcessTaggedDataL(tag, tagData);		
		}
	}

void TaggedDataParser::ConvertTextToUidL(const TDesC8& aData, TUid& aUid)
	{
	// Make sure aData is in the correct format - "0x12345678"
	_LIT8(K0x, "0x");
	_LIT8(K0X, "0X");
	if ((aData.Length() == 10) && ((aData.FindF(K0x) == 0) || (aData.FindF(K0X) == 0)))
		{
		// only take the right 8 characters (ie discard the "0x")
		TLex8 lex(aData.Right(8));
		TUint32 value = 0;
		User::LeaveIfError(lex.Val(value, EHex));
		aUid.iUid = value;
		}
	else
		User::Leave(KErrCorrupt);
	}


void TaggedDataParser::ConvertTextToTUintL(const TDesC8& aData, TUint& aUint)
	{
	// Determine whether hex or decimal then parse as such
	_LIT8(K0x, "0x");
	_LIT8(K0X, "0X");
	if (((aData.FindF(K0x) == 0) || (aData.FindF(K0X) == 0)) && (aData.Length() >= 3))
		{
		// only take the characters after "0x"
		TLex8 lex(aData.Right(aData.Length()-2));
		TUint32 value = 0;
		User::LeaveIfError(lex.Val(value, EHex));
		aUint = value;
		}
	else if (aData.Length() > 0)
		{
		TLex8 lex(aData.Right(aData.Length()));
		TUint32 value = 0;
		User::LeaveIfError(lex.Val(value, EDecimal));
		aUint = value;
		}
	else
		User::Leave(KErrCorrupt);
	}


EXPORT_C TBool CMMFPluginImplementationInformation::SupportsSupplier(const TDesC& aSupplier) const
	{
	if (iSupplier)
		{
		if (aSupplier.CompareF(*iSupplier) == KErrNone)
			return ETrue;
		}
	return EFalse;
	}

EXPORT_C TBool CMMFPluginImplementationInformation::SupportsMediaId(TUid aMediaId) const
	{
	TInt location = iMediaIds.Find(aMediaId);
	return (location != KErrNotFound);
	}
	
EXPORT_C TUid CMMFPluginImplementationInformation::Uid() const
	{
	return iUid;
	}

EXPORT_C const TDesC& CMMFPluginImplementationInformation::DisplayName() const
	{
	if (iDisplayName)
		return *iDisplayName;
	else
		return KNullDesC;
	}

EXPORT_C const TDesC& CMMFPluginImplementationInformation::Supplier() const
	{
	if (iSupplier)
		return *iSupplier;
	else
		return KNullDesC;
	}

EXPORT_C TInt CMMFPluginImplementationInformation::Version() const
	{
	return iVersion;
	}
	
EXPORT_C const RArray<TUid>& CMMFPluginImplementationInformation::SupportedMediaIds() const
	{
	return iMediaIds;
	}

CMMFPluginImplementationInformation::~CMMFPluginImplementationInformation()
	{
	delete iDisplayName;
	delete iSupplier;
	iMediaIds.Close();
	}


CMMFPluginImplementationInformation::CMMFPluginImplementationInformation()
	{
	}

void CMMFPluginImplementationInformation::SetSupplierL(const TDesC8& aData)
	{
	delete iSupplier;
	iSupplier = NULL;
	// Convert aData to unicode...
	iSupplier = HBufC::NewL(aData.Length());
	TPtr ptr = iSupplier->Des();
	User::LeaveIfError(CnvUtfConverter::ConvertToUnicodeFromUtf8(ptr, aData));
	}

void CMMFPluginImplementationInformation::AddMediaIdL(const TDesC8& aData)
	{
	TUid mediaId;
	TaggedDataParser::ConvertTextToUidL(aData, mediaId);
	User::LeaveIfError(iMediaIds.Append(mediaId));
	}

CMMFControllerImplementationInformation* CMMFControllerImplementationInformation::NewL(const CImplementationInformation& aImplInfo)
	{
	CMMFControllerImplementationInformation* s = CMMFControllerImplementationInformation::NewLC(aImplInfo);
	CleanupStack::Pop(s);
	return s;
	}

CMMFControllerImplementationInformation* CMMFControllerImplementationInformation::NewLC(const CImplementationInformation& aImplInfo)
	{
	CMMFControllerImplementationInformation* s = new(ELeave) CMMFControllerImplementationInformation;
	CleanupStack::PushL(s);
	s->ConstructL(aImplInfo);
	return s;
	}
	
EXPORT_C CMMFControllerImplementationInformation* CMMFControllerImplementationInformation::NewL(TUid aUid)
	{
	RImplInfoPtrArray ecomArray;
	CleanupResetAndDestroyPushL(ecomArray);
	
	MmPluginUtils::FindImplementationsL(KUidInterfaceMMFController, ecomArray);

	TInt index;
	CMMFControllerImplementationInformation* controller = NULL;
	// Create Controller Implementation Information for the entry with the requested UID
	for (index=0; index<ecomArray.Count() && (controller==NULL); index++)
		{
		if (ecomArray[index] == NULL)
			{
			User::Leave(KErrNoMemory);
			}
			
		if (ecomArray[index]->ImplementationUid()==aUid)
			{
			// Create the impl info object, and get the play and record formats supported by the plugin
			controller = CMMFControllerImplementationInformation::NewL(*(ecomArray[index]));

			CleanupStack::PushL(controller); // INC023207 - Placed controller on CleanupStack
			controller->GetPlayFormatsL();
			controller->GetRecordFormatsL(); // INC023207
			CleanupStack::Pop();
			}
		}

	if (controller == NULL)
		{
		User::Leave(KErrNotFound);
		}
	
	CleanupStack::PopAndDestroy(); // ecomArray
	return controller;	
	}

EXPORT_C const RMMFFormatImplInfoArray& CMMFControllerImplementationInformation::PlayFormats() const
	{
	return iPlayFormats;
	}

EXPORT_C const RMMFFormatImplInfoArray& CMMFControllerImplementationInformation::RecordFormats() const
	{
	return iRecordFormats;
	}


EXPORT_C TUint CMMFControllerImplementationInformation::HeapSpaceRequired() const
	{
	return iHeapSpaceRequired;
	}

EXPORT_C TUint CMMFControllerImplementationInformation::StackSize() const
	{
	return iStackSize;
	}
/**
	@publishedPartner
	@prototype
	
	Returns the uri schemes of this plugin.

	@return The array of uri schemes.
*/
EXPORT_C const CDesC8Array& CMMFControllerImplementationInformation::SupportedUriSchemes() const  
	{
	return *iUriSchemes;
	}

/**
	@publishedPartner
	@prototype
	
	Tests whether the plugin supports aUriScheme.
	
	@param  aUriScheme
	        The required Uri Scheme.

	@return A boolean indicating if the plugin supports aUriScheme. ETrue if this plugin supports aUriScheme, EFalse if not.
*/
EXPORT_C TBool CMMFControllerImplementationInformation::SupportsUriScheme(const TDesC8& aUriScheme) const
  	{
  	TInt position;
  	TInt error = iUriSchemes->FindIsq(aUriScheme, position, ECmpFolded);
  	return (error==KErrNone);
  	}
	
/**
	@publishedPartner
	@prototype

	Sets the uri priority of this controller

	@param  aUriPriority
	        The Uri priority to be assigned.
*/
EXPORT_C void CMMFControllerImplementationInformation::SetUriPriority(TInt aUriPriority)
	{
	iUriPriority = aUriPriority;
	}

/**
	@publishedPartner
	@prototype
	
	Retrieves the uri priority of this controller
	
	@return  The assigned Uri priority.
*/
EXPORT_C TInt CMMFControllerImplementationInformation::UriPriority() const
	{
	return iUriPriority;
	}

/**
	@publishedPartner
	@prototype
	
	Tests whether the controller plugin supports url
		
	@return A boolean indicating if the plugin supports url. ETrue for uri supporting controller, EFalse if not.
*/
EXPORT_C TBool CMMFControllerImplementationInformation::SupportsNetworkCapability() const
	{
	return iIsNetworkCtrl;
	}

/**
	@publishedPartner
	@prototype
	
	Check whether the controller plugin supports secure DRM process mode
	
	@return  A boolean indicating if the plugin supports secure DRM process mode. 
*/
EXPORT_C TBool CMMFControllerImplementationInformation::SupportsSecureDRMProcessMode() const
	{
	return iSupportsSecureDRMProcessMode;
	}

CMMFControllerImplementationInformation::CMMFControllerImplementationInformation()
	{
	iUriPriority = KUriPriorityNone;
	iIsNetworkCtrl = ETrue;
	iPlayFormatCollectionUid = KNullUid;
	iRecordFormatCollectionUid = KNullUid;
	iHeapSpaceRequired = KMMFDefaultControllerThreadHeapSize;
	iSupportsSecureDRMProcessMode = EFalse;
	iStackSize = KDefaultStackSize;
	}

void CMMFControllerImplementationInformation::ConstructL(const CImplementationInformation& aImplInfo)
	{
	iUriSchemes = new(ELeave) CDesC8ArrayFlat(KDesCArrayGranularity);
	iUid = aImplInfo.ImplementationUid();
	iDisplayName = aImplInfo.DisplayName().AllocL();
	iVersion = aImplInfo.Version();

	// Parse the opaque data...
	TaggedDataParser::ParseTaggedDataL(aImplInfo.OpaqueData(), *this);
	}

void CMMFControllerImplementationInformation::ProcessTaggedDataL(const TDesC8& aTag, const TDesC8& aData)
	{
	if (aTag==KSupplier)
		{
		SetSupplierL(aData);
		}
	else if (aTag==KMediaId)
		{
		AddMediaIdL(aData);
		}
	else if (aTag==KUriScheme)
		{
		SetUriSchemeL(aData);
		}
	else if (aTag==KNonNetwork)
		{
		SetNetworkCapabilityL(aData);
		}
	else if (aTag==KPlayFormatCollectionUid)
		{
		SetPlayFormatCollectionUidL(aData);
		}
	else if (aTag==KRecordFormatCollectionUid)
		{
		SetRecordFormatCollectionUidL(aData);
		}
	else if(aTag==KHeapSize)
		{
		SetHeapSizeL(aData);
		}
	else if (aTag==KSecureDRMProcessMode)
		{
		iSupportsSecureDRMProcessMode = ETrue;
		}
	else if (aTag==KStackSize)
		{
		SetStackSizeL(aData);
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}


CMMFControllerImplementationInformation::~CMMFControllerImplementationInformation()
	{
	iPlayFormats.ResetAndDestroy();
	iRecordFormats.ResetAndDestroy();
	delete iUriSchemes;
	}


void CMMFControllerImplementationInformation::SetPlayFormatCollectionUidL(const TDesC8& aData)
	{
	TaggedDataParser::ConvertTextToUidL(aData, iPlayFormatCollectionUid);
	}

void CMMFControllerImplementationInformation::SetRecordFormatCollectionUidL(const TDesC8& aData)
	{
	TaggedDataParser::ConvertTextToUidL(aData, iRecordFormatCollectionUid);
	}

void CMMFControllerImplementationInformation::SetHeapSizeL(const TDesC8& aData)
	{
	TaggedDataParser::ConvertTextToTUintL(aData, iHeapSpaceRequired);
	}

void CMMFControllerImplementationInformation::SetStackSizeL(const TDesC8& aData)
	{
	TaggedDataParser::ConvertTextToTUintL(aData, iStackSize);
	}


void CMMFControllerImplementationInformation::AddFormatsSwallowCorruptL(RImplInfoPtrArray& aEcomArray, RMMFFormatImplInfoArray& aFormatArray)
	{
	for (TInt index=0; index<aEcomArray.Count(); index++)
		{
		CMMFFormatImplementationInformation* c = NULL;
		TRAPD(error, c = CMMFFormatImplementationInformation::NewL(*(aEcomArray[index])));
		if (error==KErrNone)
			{
			CleanupStack::PushL(c);
			User::LeaveIfError(aFormatArray.Append(c));
			CleanupStack::Pop(c);
			}
		else if (error != KErrCorrupt)// make sure we don't leave because of a corrupt resource file
			User::Leave(error);
		}
	}


void CMMFControllerImplementationInformation::GetFormatsL(TUid aFormatCollectionUid, TUid aFormatPluginCollectionUid, RMMFFormatImplInfoArray& aFormatArray)
	{
	RImplInfoPtrArray ecomArray;
	CleanupResetAndDestroyPushL(ecomArray);
	// If we have a valid play format collection uid, get the play formats defined by this plugin
	if (aFormatCollectionUid != KNullUid)
		{
		MmPluginUtils::FindImplementationsL(aFormatCollectionUid, ecomArray);
		// Create format implementation information for each entry
		AddFormatsSwallowCorruptL(ecomArray, aFormatArray);
		}

	// Now get all the format plugins attached to this controller

	// Create a descriptor and fill it with the uid of this controller plugin
	TBuf8<10> controllerUid;
	_LIT8(K0x, "0x");
	controllerUid.Append(K0x);
	controllerUid.AppendNumFixedWidth(iUid.iUid, EHex, 8);

	MmPluginUtils::FindImplementationsL(aFormatPluginCollectionUid, ecomArray, controllerUid);
	// Create format implementation information for each entry
	AddFormatsSwallowCorruptL(ecomArray, aFormatArray);

	CleanupStack::PopAndDestroy();//ecomArray
	}

void CMMFControllerImplementationInformation::GetPlayFormatsL()
	{
	GetFormatsL(iPlayFormatCollectionUid, KUidInterfaceFormatDecode, iPlayFormats);
	}

void CMMFControllerImplementationInformation::GetRecordFormatsL()
	{
	GetFormatsL(iRecordFormatCollectionUid, KUidInterfaceFormatEncode, iRecordFormats);
	}

/**
	Sets the Uri Scheme found in opaque data
	
	@param  aUriScheme
			Gives the uri scheme supported by the controller
*/
void CMMFControllerImplementationInformation::SetUriSchemeL(const TDesC8& aUriScheme)
	{
	//If No uri support already declared in the resource file, 
	//then mentioning a particular schema support is illegal
	if(!iIsNetworkCtrl)
		{
		User::Leave(KErrArgument);
		}
	
	// Insert the new uri scheme into the array	
	iUriSchemes->InsertIsqL(aUriScheme, ECmpFolded);//ensures there are no repeated entries
	}

/**
	Sets the Network capability found in opaque data
	
	@param  aNetworkCapable
			Declares the incapability to support uri if matches to 'yes'. 
			If this is the case, iIsNetworkCtrl is set to EFalse
*/
void CMMFControllerImplementationInformation::SetNetworkCapabilityL(const TDesC8& aNetworkCapable)
	{
	//If a uri scheme is already declared in the resource file, 
	//then mentioning no url support is illegal
	if(iUriSchemes->Count() > 0)
		{
		User::Leave(KErrArgument);
		}
		
	if (aNetworkCapable.CompareF(KTagYes) == KErrNone)
		{
		iIsNetworkCtrl = EFalse;
		}
	else
		{
		User::Leave(KErrBadName); // will leave if aNetworkCapable is anything other than 'yes'.
		}
	}




CMMFFormatImplementationInformation* CMMFFormatImplementationInformation::NewL(const CImplementationInformation& aImplInfo)
	{
	CMMFFormatImplementationInformation* s = CMMFFormatImplementationInformation::NewLC(aImplInfo);
	CleanupStack::Pop(s);
	return s;
	}

CMMFFormatImplementationInformation* CMMFFormatImplementationInformation::NewLC(const CImplementationInformation& aImplInfo)
	{
	CMMFFormatImplementationInformation* s = new(ELeave) CMMFFormatImplementationInformation();
	CleanupStack::PushL(s);
	s->ConstructL(aImplInfo);
	return s;
	}

CMMFFormatImplementationInformation::CMMFFormatImplementationInformation()
	{
	}

void CMMFFormatImplementationInformation::ConstructL(const CImplementationInformation& aImplInfo)
	{
	iUid = aImplInfo.ImplementationUid();
	iDisplayName = aImplInfo.DisplayName().AllocL();

	iBody = CMMFFormatImplementationInformation::CBody::NewL();

	// Extract the rest of the data from the opaque data field of aImplInfo...
	// Parse the opaque data...
	TaggedDataParser::ParseTaggedDataL(aImplInfo.OpaqueData(), *this);
	}

void CMMFFormatImplementationInformation::ProcessTaggedDataL(const TDesC8& aTag, const TDesC8& aTagData)
	{
	if (aTag == KSupplier)
		SetSupplierL(aTagData);
	else if (aTag == KMediaId)
		AddMediaIdL(aTagData);
	else if (aTag == KFormatFileExtension)
		AddFileExtensionL(aTagData);
	else if (aTag == KFormatMimeType)
		AddMimeTypeL(aTagData);
	else if (aTag == KFormatHeaderData)
		AddHeaderDataL(aTagData);
	else if ((aTag == KCustomInterfaceSupport) && (aTagData.CompareF(KTagYes) == 0))
		iBody->SetSupportsCustomInterfaces(ETrue);
	else
		User::Leave(KErrCorrupt);
	}

void CMMFFormatImplementationInformation::AddFileExtensionL(const TDesC8& aData)
	{
	iBody->AddFileExtensionL(aData);
	}

void CMMFFormatImplementationInformation::AddMimeTypeL(const TDesC8& aData)
	{
	iBody->AddMimeTypeL(aData);
	}

void CMMFFormatImplementationInformation::AddHeaderDataL(const TDesC8& aData)
	{
	iBody->AddHeaderDataL(aData);
	}

CMMFFormatImplementationInformation::~CMMFFormatImplementationInformation()
	{
	delete iBody;
	}

EXPORT_C const CDesC8Array& CMMFFormatImplementationInformation::SupportedFileExtensions() const
	{
	return iBody->SupportedFileExtensions();
	}

EXPORT_C const CDesC8Array& CMMFFormatImplementationInformation::SupportedMimeTypes() const
	{
	return iBody->SupportedMimeTypes();
	}

EXPORT_C const CDesC8Array& CMMFFormatImplementationInformation::SupportedHeaderData() const
	{
 	return iBody->SupportedHeaderData();
	}


EXPORT_C TBool CMMFFormatImplementationInformation::SupportsFileExtension(const TDesC8& aFileExtension) const
	{
	return iBody->SupportsFileExtension(aFileExtension);
	}

EXPORT_C TBool CMMFFormatImplementationInformation::SupportsMimeType(const TDesC8& aMimeType) const
	{
	return iBody->SupportsMimeType(aMimeType);
	}

EXPORT_C TBool CMMFFormatImplementationInformation::SupportsHeaderDataL(const TDesC8& aHeaderData) const
	{
	return iBody->SupportsHeaderDataL(aHeaderData);
	}

EXPORT_C TBool CMMFFormatImplementationInformation::SupportsCustomInterfaces() const
	{
	return iBody->SupportsCustomInterfaces();
	}

CMatchData* CMatchData::CreateL()
	{
	return new (ELeave) CMatchData();
	}
	
void CMatchData::SetMatchDataL(const TDesC8& aMatchData)
	{
	delete iMatchData;
	iMatchData = NULL;
		
	iMatchData = aMatchData.AllocL();	
	}

void CMatchData::SetMatchUriSchemeL(const TDesC8& aMatchUriScheme)
	{
	delete iMatchUriScheme;
	iMatchUriScheme = NULL;
	
	iMatchUriScheme = aMatchUriScheme.AllocL();	
	}
	
const TDesC8& CMatchData::MatchData() const
	{
	if (iMatchData )
		{
		return *iMatchData;
		}
	else
		{
		return KNullDesC8;
		}
	}

const TDesC8& CMatchData::MatchUriScheme() const
	{
	if (iMatchUriScheme )
		{
		return *iMatchUriScheme;
		}
	else
		{
		return KNullDesC8;
		}
	}

CMatchData::~CMatchData()
	{
	delete iMatchData;
	delete iMatchUriScheme;
	}

