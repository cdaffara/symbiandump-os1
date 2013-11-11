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
// Part of the MVS Agents for TechView
//

#include "agentsutility.h"
#include <e32def.h>
const TInt KMaxHeaderSize = 256;

//CMVSControllerPluginInfo methods

/**
Constructs and initialises a new instance of the controller plugin information class.

The function leaves if the controller plugin information object cannot be created.

No callback notification is made upon completion of NewL().

@return A pointer to the new controller plugin information object.

*/
EXPORT_C CMVSControllerPluginInfo* CMVSControllerPluginInfo::NewL()
	{
    CMVSControllerPluginInfo* self = new(ELeave) CMVSControllerPluginInfo();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


void CMVSControllerPluginInfo::ConstructL()
	{
    TInt err = KErrNone;
    CMMFControllerPluginSelectionParameters* cSelect=NULL;
    CMMFFormatSelectionParameters* fSelect=NULL;
    User::LeaveIfError(iFileLogger.Connect());
    iFileLogger.CreateLog(_L("LogMVSappUi"),_L("LogFile.txt"),EFileLoggingModeAppend);    
    cSelect =  CMMFControllerPluginSelectionParameters::NewLC();
    RArray<TUid> mediaIds; //search for both audio and video
	TRAP(err, mediaIds.Append(KUidMediaTypeAudio));
    TRAP(err, mediaIds.Append(KUidMediaTypeVideo));
    
    TRAP(err, cSelect->SetMediaIdsL(mediaIds,
         CMMFPluginSelectionParameters::EAllowOtherMediaIds));
    mediaIds.Close(); 
	
	fSelect = CMMFFormatSelectionParameters::NewLC();
	   
    TRAP(err, {
              cSelect->SetRequiredRecordFormatSupportL(*fSelect);
	          //Populate the controllers array
	          cSelect->ListImplementationsL(iControllers);
              });
	CleanupStack::PopAndDestroy(2);
    
    for(TInt count = 0;count < iControllers.Count();count++)
     	{
    	iUidArray.AppendL(iControllers[count]->Uid());
      	}
    //During the construction get all the supported controllers,not for a specific extension.
    CollectAudioControllersL(NULL); 
    CollectVideoControllersL(NULL);
    }


void CMVSControllerPluginInfo::CollectAudioControllersL(TDesC* aExt)
	{	
	TInt err = KErrNone;
    CMMFControllerPluginSelectionParameters* cSelect=NULL;
    CMMFFormatSelectionParameters* fSelect=NULL;

    cSelect =  CMMFControllerPluginSelectionParameters::NewLC();
    RArray<TUid> mediaIds; //search for audio only
	TRAP(err, mediaIds.Append(KUidMediaTypeAudio));
    
    TRAP(err, cSelect->SetMediaIdsL(mediaIds,
         CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds));
    	
    mediaIds.Close(); 
	
	fSelect = CMMFFormatSelectionParameters::NewLC();
    if(aExt!=NULL)
    	{
     	fSelect->SetMatchToFileNameL(*aExt);	
     	}   
    TRAP(err, {
              cSelect->SetRequiredRecordFormatSupportL(*fSelect);
	          //Populate the controllers array
	          cSelect->ListImplementationsL(iAudioControllers);
              });
	
	CleanupStack::PopAndDestroy(2);
	
    for(TInt count = 0;count < iAudioControllers.Count();count++)
    	{
     	iAudioUidArray.AppendL(iAudioControllers[count]->Uid());
      	} 	
  	}



void CMVSControllerPluginInfo::CollectAudioPlayControllersL(TDesC* aExt)
	{	
	TInt err = KErrNone;
    CMMFFormatSelectionParameters* sSelect=NULL;
    CMMFControllerPluginSelectionParameters* pSelect=NULL;
    
    RArray<TUid> mediaIds; //search for audio only
	pSelect =  CMMFControllerPluginSelectionParameters::NewLC();
    
	TRAP(err, mediaIds.Append(KUidMediaTypeAudio));
    
    TRAP(err, pSelect->SetMediaIdsL(mediaIds,
         CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds));
    
    mediaIds.Close(); 
	
	sSelect = CMMFFormatSelectionParameters::NewLC();
    if(aExt!=NULL)
    	{
     	sSelect->SetMatchToFileNameL(*aExt);	
     	}   
    
	TRAP(err,{
			 pSelect->SetRequiredPlayFormatSupportL(*sSelect);
	         //populate the play controllers array
	         pSelect->ListImplementationsL(iAudioPlayControllers);	
			 });
	CleanupStack::PopAndDestroy(2);
    
    for(TInt count = 0;count < iAudioPlayControllers.Count();count++)
    	{
     	iAudioPlayUidArray.AppendL(iAudioPlayControllers[count]->Uid());
      	} 	
    iFileLogger.Write(_L("CollectingAudioControllers "));  	
 	}


void CMVSControllerPluginInfo::CollectVideoControllersL(TDesC* aExt)
	{
	TInt err = KErrNone;
    CMMFControllerPluginSelectionParameters* cSelect=NULL;
    CMMFFormatSelectionParameters* fSelect=NULL;
        
    cSelect =  CMMFControllerPluginSelectionParameters::NewLC();
    RArray<TUid> mediaIds; //search for audio only
	TRAP(err, mediaIds.Append(KUidMediaTypeVideo));
    
    TRAP(err, cSelect->SetMediaIdsL(mediaIds,
         CMMFPluginSelectionParameters::EAllowOtherMediaIds));
    mediaIds.Close(); 
	
	fSelect = CMMFFormatSelectionParameters::NewLC();
	if(aExt!=NULL)
     	{
		fSelect->SetMatchToFileNameL(*aExt);    
     	}
    TRAP(err, {
              cSelect->SetRequiredRecordFormatSupportL(*fSelect);
	          //Populate the controllers array
	          cSelect->ListImplementationsL(iVideoControllers);
              });
	
	CleanupStack::PopAndDestroy(2);

    for(TInt count = 0;count < iVideoControllers.Count();count++)
    	{
     	iVideoUidArray.AppendL(iVideoControllers[count]->Uid());
      	}
    iFileLogger.Write(_L("CollectingVideoControllers ")) ;
	}

/**
Destructor.

Frees all resources owned by the object prior to its destruction.
*/
EXPORT_C CMVSControllerPluginInfo::~CMVSControllerPluginInfo()
    {
    iControllers.ResetAndDestroy();
	iControllers.Close();
	iSupportedControllers.ResetAndDestroy();
	iSupportedControllers.Close();
	iAudioControllers.ResetAndDestroy();
	iAudioControllers.Close();
	iAudioPlayControllers.ResetAndDestroy();
	iAudioPlayControllers.Close();
	iVideoControllers.ResetAndDestroy();
	iVideoControllers.Close();
    iUidArray.Close();
    iAudioUidArray.Close();
    iAudioPlayUidArray.Close();
    iVideoUidArray.Close();
    if(iFileLogger.Handle())
    	{
    	iFileLogger.CloseLog();
		iFileLogger.Close();
    	}
    }


CMVSControllerPluginInfo::CMVSControllerPluginInfo()
    {
    }


/**
Fetches an array of all the display names of the 
controller plugins existing currently in the system and an array of their Uid's

The data must be in a supported format (for example, WAV ,OGG or AVI).

@param	aDisplayNames
		This array will contain all the displaynames of the controllers when this method returns.
@param 	aUidArray
		This array will contain all the Uids when this method returns.

@return  number of plugins existing in the system.

*/
EXPORT_C TInt CMVSControllerPluginInfo::GetPluginListL(CDesCArrayFlat* aDisplayNames, RArray<TUid>& aUidArray)
	{
	for(TInt counter = 0;counter < iControllers.Count();counter++)
     	{
     	aDisplayNames->AppendL(iControllers[counter]->DisplayName());
     	aUidArray.Append(iUidArray[counter]);
     	}
    iFileLogger.Write(_L("Getting the Plugin List"));
    return aUidArray.Count();
    }
 

/**
Fetches an array of all the display names of the 
audio controller plugins existing currently in the system and an array of their Uid's

@param	aDisplayNames
		This array will contain all the displaynames of the audio controllers for the specified 
		extension, when this method returns.
@param 	aUidArray
		This array will contain all the corresponding Uids when this method returns.
@param  aExt
	    The specific extension (for example WAV or OGG) for which the supported controllers is to be retrieved 

@return number of audio plugins existing in the system.

*/
EXPORT_C TInt CMVSControllerPluginInfo::GetAudioPluginListL(CDesCArrayFlat* aDisplayNames, RArray<TUid>& aUidArray,RArray<TUid>& aUidPlayArray, TDesC* aExt)
	{
	iAudioControllers.ResetAndDestroy();
	iAudioUidArray.Reset();
	iAudioPlayControllers.ResetAndDestroy();
	iAudioPlayUidArray.Reset();
	CollectAudioControllersL(aExt);
	CollectAudioPlayControllersL(aExt);
	for(TInt counter = 0;counter < iAudioControllers.Count();counter++)
    	{
     	aDisplayNames->AppendL(iAudioControllers[counter]->DisplayName());
     	aUidArray.Append(iAudioUidArray[counter]);
     	}
    for(TInt counter = 0;counter < iAudioPlayControllers.Count();counter++)
     	{
      	aUidPlayArray.Append(iAudioPlayUidArray[counter]);
     	}
    iFileLogger.Write(_L("Getting the Audio Plugin List"));
    //Flushing out the existing seleced data and filling in with
    //all the audio controllers and corresponding UID's for the next run
    iAudioControllers.ResetAndDestroy();
	iAudioUidArray.Reset();
	iAudioPlayControllers.ResetAndDestroy();
	iAudioPlayUidArray.Reset();
    iFileLogger.Write(_L("Getting the Video Plugin List"));	
    CollectAudioControllersL(NULL);
    return aUidArray.Count();
	}
 

/**
Fetches an array of all the display names of the 
video controller plugins existing currently in the system and an array of their Uid's

@param	aDisplayNames
		This array will contain all the displaynames of the video controllers for the specified 
		extension, when this method returns.
@param 	aUidArray
		This array will contain all the corresponding Uids when this method returns.
@param  aExt
	    The specific extension (for example AVI) for which the supported controllers is to be retrieved.

@return number of video plugins existing in the system.

*/
EXPORT_C TInt CMVSControllerPluginInfo::GetVideoPluginListL(CDesCArrayFlat* aDisplayNames, RArray<TUid>& aUidArray,TDesC* aExt)
	{
	iVideoControllers.ResetAndDestroy();
	iVideoUidArray.Reset();
	CollectVideoControllersL(aExt);
	for(TInt counter = 0;counter < iVideoControllers.Count();counter++)
     	{
     	aDisplayNames->AppendL(iVideoControllers[counter]->DisplayName());
     	aUidArray.Append(iVideoUidArray[counter]);
     	}
    //Flushing out the existing seleced data and filling in with
    //all the video controllers and corresponding UID's for the next run
    iVideoControllers.ResetAndDestroy();
	iVideoUidArray.Reset();
    iFileLogger.Write(_L("Getting the Video Plugin List"));	
    CollectVideoControllersL(NULL);
    return aUidArray.Count();
   	}


/**
Extracts the list of all the extensions supported. 

All the supported extensions by the system are retrieved including both audio and video.

@param	aMediaType
		Specifies the media type ie audio/video.
@param 	aExtArray
		This array will contain all the supported extensions, when this method returns.

@return number of supported extensions existing in the system.

*/
EXPORT_C TInt CMVSControllerPluginInfo::GetExtensionListL(TBool aMediaType, CDesCArrayFlat* aExtArray)
	{
	CMMFFormatImplementationInformation* formatInfo;
	if(aMediaType)//selected video
		{
		if(iVideoUidArray.Count())
    		{
    		for(TInt counter = 0; counter < iVideoUidArray.Count(); counter++)
        		{
         	  	CMMFControllerImplementationInformation& plugin= *(iVideoControllers[counter]);
       			//Collect all supported 'Recording' formats
				const RMMFFormatImplInfoArray& recFormatInfo = plugin.RecordFormats();
       	 		for(TInt n = 0; n < recFormatInfo.Count(); n++)
					{
					formatInfo = recFormatInfo[n];
					const CDesC8Array& fileExtensions = formatInfo->SupportedFileExtensions();
					for(TInt innerCounter = 0;innerCounter < fileExtensions.Count();innerCounter++)
						{
						TBuf<16> buf;
						buf.Zero();
						buf.Copy(fileExtensions[innerCounter]);
						aExtArray->AppendL(buf);
						}
					}
          		}
        	}
		}
    else
    	{
    	if(iAudioUidArray.Count())
    		{
    		for(TInt counter = 0; counter < iAudioUidArray.Count(); counter++)
        		{
         	    CMMFControllerImplementationInformation& plugin= *(iAudioControllers[counter]);
       			//Collect all supported 'Recording' formats
				const RMMFFormatImplInfoArray& recFormatInfo = plugin.RecordFormats();
       	 		for(TInt n = 0; n < recFormatInfo.Count(); n++)
					{
					formatInfo = recFormatInfo[n];
					const CDesC8Array& fileExtensions = formatInfo->SupportedFileExtensions();
					for(TInt innerCounter = 0;innerCounter < fileExtensions.Count();innerCounter++)
						{
						TBuf<16> buf;
						buf.Zero();
						buf.Copy(fileExtensions[innerCounter]);
						aExtArray->AppendL(buf);
						}
					}
          		}
    		}
    	}
    iFileLogger.Write(_L("Getting the Extension List")); 
    return(aExtArray->Count());
    }


/**
Returns the type of the given media file. Returns NULL Uid if the 
	file is not supported by MMF.

@param	aFile
		The name of the media file.
		
@return	Uid of the media.
*/
EXPORT_C TUid CMVSControllerPluginInfo::GetMediaTypeL(TDesC& aFile)
	{
	CMMFControllerPluginSelectionParameters* cSelect=NULL;
    CMMFFormatSelectionParameters* fSelect=NULL;
    TUid uid=TUid::Null();
    
    iSupportedControllers.ResetAndDestroy();
    iSupportedControllers.Close();
    
    cSelect =  CMMFControllerPluginSelectionParameters::NewLC();

	fSelect = CMMFFormatSelectionParameters::NewLC();
   	fSelect->SetMatchToFileNameL(aFile);
    
   cSelect->SetRequiredPlayFormatSupportL(*fSelect);
   //Populate the controllers array
   cSelect->ListImplementationsL(iSupportedControllers);
               
	
    if(iSupportedControllers.Count())
   		{
   		//must be sufficient to check with one. not sure if MVS plays midi!
   		TBool video = iSupportedControllers[0]->SupportsMediaId(KUidMediaTypeVideo);
   		if(video)
   			{
   			uid=KUidMediaTypeVideo; 
   			}
   		else
   			{
   			uid=KUidMediaTypeAudio; 
   			}
   		CleanupStack::PopAndDestroy(2,cSelect);//fselect, cselect
   		}
   	else
   		{   	
	   	//If we are here, file extension has not matched any controller
	   	//Try to find controller based on header data
	   	HBufC8* headerData = HBufC8::NewLC(KMaxHeaderSize);
		TPtr8 headerDataPtr = headerData->Des();
					
		GetHeaderL(aFile, headerDataPtr);
		
		fSelect->SetMatchToHeaderDataL(headerDataPtr);
		cSelect->SetRequiredPlayFormatSupportL(*fSelect);
		//Populate the controllers array
		cSelect->ListImplementationsL(iSupportedControllers);
	               
		if(iSupportedControllers.Count())
	   		{
	   		TBool video = iSupportedControllers[0]->SupportsMediaId(KUidMediaTypeVideo);
	   		if(video)
	   			{
	   			uid=KUidMediaTypeVideo; 
	   			}
	   		else 
	   			{
	   			uid=KUidMediaTypeAudio; 
	   			}
	   		}
		CleanupStack::PopAndDestroy(3,cSelect);//fselect, cselect,headerData
   		}
    	
   	return uid;
   	}


/**
Returns the controller implementation information associated with the given controller

@param  aControllerUid
        The Uid of the controller plugin

@return	controller implementation structure 
*/
EXPORT_C CMMFControllerImplementationInformation& CMVSControllerPluginInfo::GetControllerInfo(TUid aControllerUid)
	{
    TInt index = -1;
    for(TInt counter = 0; counter < iUidArray.Count(); counter++)
       	{
       	TUid controllerUid = iUidArray[counter];
       	if(controllerUid == aControllerUid)
       		{
       		index = counter;
       		}
       	}
    iFileLogger.Write(_L("Getting the controller info"));  
    return *(iControllers[index]);
	}

void CMVSControllerPluginInfo::GetHeaderL(TDesC& aFileName, TDes8& aHeaderData)
	{
	RFile file;
	RFs	fs;
			
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(file.Open(fs, aFileName, EFileShareReadersOnly));
	CleanupClosePushL(file);
	User::LeaveIfError(file.Read(aHeaderData,KMaxHeaderSize));
		
	CleanupStack::PopAndDestroy(2,&fs);
	}

