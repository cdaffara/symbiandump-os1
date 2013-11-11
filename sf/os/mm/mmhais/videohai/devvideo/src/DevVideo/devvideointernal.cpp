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
//

#include "devvideointernal.h"
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
#include <mdf/codecapiresolverdata.h>
#include <mdf/codecapiuids.hrh>
#include <mdf/codecapiresolver.hrh>
#include <mm/mmpluginutils.h>

#include "videoplayhwdevice.h"

void DevVideoUtilities::FindVideoDecoderPluginsL(const TDesC8& aMimeType, RImplInfoPtrArray& aImplInfoArray)
	{
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	customMatchData->SetMatchType(EMatchInputDataFormat);
	customMatchData->SetImplementationType(TUid::Uid(KUidVideoDecoder));
	// string value of the input source data	
	customMatchData->SetInputDataL(aMimeType);
	
  	HBufC8* package  = customMatchData->NewPackLC(); // parameters to match sent as "default data"
	MmPluginUtils::FindImplementationsL(TUid::Uid(KUidMdfProcessingUnit), aImplInfoArray, *package, TUid::Uid(KUidCodecApiResolverImplementation));
	CleanupStack::PopAndDestroy(2, customMatchData); // package, customMatchData
	}


void DevVideoUtilities::FindVideoEncoderPluginsL(const TDesC8& aMimeType, RImplInfoPtrArray& aImplInfoArray)
	{
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	customMatchData->SetMatchType(EMatchOutputDataFormat);
	customMatchData->SetImplementationType(TUid::Uid(KUidVideoEncoder));
	// string value of the input source data	
	customMatchData->SetOutputDataL(aMimeType);
	
 	HBufC8* package  = customMatchData->NewPackLC(); // parameters to match sent as "default data"	
	MmPluginUtils::FindImplementationsL(TUid::Uid(KUidMdfProcessingUnit), aImplInfoArray, *package, TUid::Uid(KUidCodecApiResolverImplementation));
	CleanupStack::PopAndDestroy(2, customMatchData); // package, customMatchData
	}

void DevVideoUtilities::CreatePuListL(RImplInfoPtrArray& aImplInfoArray)
	{
	// we can use REComSession::ListImplementationsL() straight, and skip the sort step of MmPluginUtils
	// as this list is used purely to feedback into FindPu below, and see if the plugin is a PU and not a HwDevice
	aImplInfoArray.Reset();
	REComSession::ListImplementationsL(TUid::Uid(KUidMdfProcessingUnit), aImplInfoArray);
	}
	
	
const CImplementationInformation* DevVideoUtilities::FindPu(const RImplInfoPtrArray& aImplInfoArray, TUid aPuUid)
	{
	CImplementationInformation* info = NULL;
	for (TInt i=0;i<aImplInfoArray.Count() && info == NULL;i++)
		{
		if (aImplInfoArray[i]->ImplementationUid() == aPuUid)
			{
			info = aImplInfoArray[i];
			}
		}
	return info;
	}

#endif // SYMBIAN_MULTIMEDIA_CODEC_API

TInt DevVideoUtilities::ConvertTextToTUint32(const TDesC8& aData, TUint32& aNumber)
	{
	// Work out whether aData is in hex or not by looking for an 0x at the beginning
	TInt error = KErrNone;

	_LIT8(K0x, "0x");
	if (aData.FindF(K0x) == 0)
		{
		// Data is in hex
		// Discard the '0x'
		TLex8 lex(aData.Right(aData.Length() - K0x().Length()));
		error = lex.Val(aNumber, EHex);
		}
	else
		{
		// Assume aData is in decimal
		TLex8 lex(aData);
		error = lex.Val(aNumber, EDecimal);
		}

	return error;
	}

void DevVideoUtilities::MatchPrePostProcessorCapabilitiesL(const RImplInfoPtrArray& aPlugins, TUint32 aReqPrePostProcType, RArray<TUid>& aMatchingPlugins)
	{
	aMatchingPlugins.Reset();

	TInt count = aPlugins.Count();
	for (TInt i=0; i<count; i++)
		{
		const CImplementationInformation* plugin = aPlugins[i];
		TUint32 pluginPostProcType = 0;
		TInt error = ConvertTextToTUint32(plugin->OpaqueData(), pluginPostProcType);
		if (error == KErrNone)
			{
			// Check the plugin has at least the pre/post processing capabilities demanded by the client
			if ((pluginPostProcType & aReqPrePostProcType) == aReqPrePostProcType)
				{
				User::LeaveIfError(aMatchingPlugins.Append(plugin->ImplementationUid()));
				}
			}
		else if (error == KErrCorrupt)
			{
			// In debug mode leave, in release mode just swallow the error since we
			// don't want to allow broken plugins to prevent other plugins being recognised.
#ifdef _DEBUG
			User::Leave(error);
#endif //_DEBUG
			}
		else
			{
			User::Leave(error);
			}
		}
	}

void DevVideoUtilities::SelectPluginBasedOnMatchType(const TDesC8& aMatchType, RImplInfoPtrArray& aImplInfoArray)
	{
	// In this function if allowing wildcards then TDesC8::Match is used which returns 
	// the position of the match or KErrNotFound
	// If not allowing wildcards then TDesC8::Compare is used which returns a TInt which
	// indicates if one descriptor is bigger than the other (0 if they are identical)
	TBool matchResult = ETrue;
	
	if(aMatchType.Length() == 0)
		{
		return;
		}
	else
		{
		TInt count = aImplInfoArray.Count();

		_LIT8(KdataSeparator, "||");
		const TInt separatorLength = KdataSeparator().Length();

		for (TInt index = 0; index < count;)
			{
			CImplementationInformation* plugin = aImplInfoArray[index];
			const TDesC8& mimeType = plugin->DataType();
		
			// Look for the section separator marker '||'
			TInt separatorPos = mimeType.Find(KdataSeparator);

			if(separatorPos == KErrNotFound)
				{
					matchResult = aMatchType.MatchF(mimeType) != KErrNotFound;
				}
			else
				{
				// Find the first section, up to the separator
				TPtrC8 dataSection = mimeType.Left(separatorPos);
				TPtrC8 remainingData = mimeType.Mid(separatorPos + separatorLength);

				// Match against each section in turn
				while(separatorPos != KErrNotFound)
					{
					matchResult = aMatchType.MatchF(dataSection) != KErrNotFound;

					// If we found it then no need to continue, so return
					if(matchResult)
						break;

					// Move on to the next section
					separatorPos = remainingData.Find(KdataSeparator);
					if(separatorPos != KErrNotFound)
						{
						dataSection.Set(remainingData.Left(separatorPos));
						remainingData.Set(remainingData.Mid(separatorPos + separatorLength));
						}
					else
						{
						dataSection.Set(remainingData);
						}
					} // end of while

					// Check the final part
					matchResult = aMatchType.MatchF(dataSection) != KErrNotFound;
					
				} //end of else

				// if a match was found increment the array,
				// otherwise remove the element from the array
				if (matchResult)
					{
					index++;
					}
				else
					{
					delete aImplInfoArray[index];
					aImplInfoArray.Remove(index);
	    			count--;
					}
				
			} //end of for
		}
	}


