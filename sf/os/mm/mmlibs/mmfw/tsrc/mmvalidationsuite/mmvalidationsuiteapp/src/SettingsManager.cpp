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
// SettingsManage.cpp
// Part of the MVS Application for TechView
//

#include "SettingsManager.h"
#include "MVSAppUI.h"
#include "MVSConfigAudioFormatDialog.h"
#include "MVSConfigVideoFormatDialog.h"

_LIT(KFullPathOfFileStore,"C:\\private\\102737E8\\MvsSettings.dat");	

CSettingsManager::CSettingsManager()
:iStore(0),iHasSettings(1)
	{	
	
	}

CSettingsManager::~CSettingsManager()
	{
	iFsSession.Close();  //close the file session
	iArrUid.Close();
	iArrStreamId.Close();
	if(iStore)
		{
		
		}
	}

void CSettingsManager::ConstructL()
	{
	User::LeaveIfError(iFsSession.Connect()); // start a file session
	}
	
CSettingsManager* CSettingsManager::NewL()
	{
	CSettingsManager* self = new(ELeave) CSettingsManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


TBool CSettingsManager::OpenStore2ReadLC()
	{
	TParse	fileStoreName;
	iFsSession.Parse(KFullPathOfFileStore,fileStoreName);
	
	iStore = CPermanentFileStore::OpenLC(iFsSession,
										fileStoreName.FullName(),
										EFileRead );
	iHasSettings = ETrue;
	return iHasSettings;
	}

void CSettingsManager::OpenStore2WriteLC()
	{
	TPath privatePath(KFullPathOfFileStore);
	iFsSession.PrivatePath(privatePath);
	TParse	fileStoreName;
	iFsSession.MkDirAll(KFullPathOfFileStore);
	iFsSession.Parse(KFullPathOfFileStore,fileStoreName);
	
	if(iHasSettings)
		{
		iStore = CPermanentFileStore::OpenLC(iFsSession,
											fileStoreName.FullName(),
											EFileWrite );
		}
	else
		{
		iStore = CPermanentFileStore::CreateLC(iFsSession,
											fileStoreName.FullName(),
											EFileWrite );
		}
	
	iStore->SetTypeL(KPermanentFileStoreLayoutUid);	 	
	}


TBool CSettingsManager::HasSettings()
	{
	return iHasSettings;
	}

void CSettingsManager::ReadGeneralSettingsL(CMVSAppUi* aAppUI)
	{
	if(!FileExists())
		return;
	OpenStore2ReadLC();
	RStoreReadStream instream;
	iRootId = iStore->Root();
	instream.OpenLC(*iStore,iRootId); //open root stream for reading
	instream >> iGenSettingsId;
	CleanupStack::PopAndDestroy();			
	
	//check for the validity of the streamid
	if(!iGenSettingsId.Value())
		{
		CleanupStack::PopAndDestroy();
		return;
		}
	
	//open the stream for general settings
	instream.OpenLC(*iStore,iGenSettingsId); 
	aAppUI->InternalizeL(instream);
	CleanupStack::PopAndDestroy(2);	
	}


void CSettingsManager::WriteGeneralSettingsL(CMVSAppUi* aAppUI)
	{	
	OpenStore2WriteLC();
	
	RStoreWriteStream outstream;
	/*
	if already there is stream id for general settings open the existing
	stream; otherwise create new stream id 
	*/
	TBool updation = iGenSettingsId.Value();
	if(updation)
		{
		outstream.ReplaceLC(*iStore,iGenSettingsId);
		}
	else
		{
		iGenSettingsId = outstream.CreateLC(*iStore);
		}
	
	//write the general settings
	aAppUI->ExternalizeL(outstream);
	outstream.CommitL();
	CleanupStack::PopAndDestroy();
	
	if(!updation)
		{
		outstream.ReplaceLC(*iStore,iStore->Root());
		TUid tempUid;
		TStreamId tempStreamId(0);
		WriteIndexL(outstream,tempUid,tempStreamId);
		CleanupStack::PopAndDestroy();	
		}		
						
	iStore->CommitL();// commit the changes to the store
	CleanupStack::PopAndDestroy(); //for iStore 
	iFsSession.Close(); //close the file session
	}	  

/*
 *This function assumes that the index doesn't exist
 */
void CSettingsManager::MakeSeedIndexL()
	{
	OpenStore2WriteLC();
	RStoreWriteStream outstream;
	TStreamId invalidId(0); //caution: confirm the reliability of this value
	iRootId = outstream.CreateLC(*iStore);		
	
	//write an invalid  stream index for general settings i.e.iGenSettingsId
	outstream << invalidId;	 
		
	//write no. of controllers as 0
	outstream.WriteInt8L(0);
	
	outstream.CommitL(); //commit stream changes
	CleanupStack::PopAndDestroy();
	
	iStore->SetRootL(iRootId);
	iStore->CommitL(); //commit changes to store
	
	CleanupStack::PopAndDestroy(); //for iStore	
	}

// the stream should be in the beginning of controller section
void CSettingsManager::ReadControllerTableL(RReadStream& aStream)
	{
	iControllerCnt = aStream.ReadInt8L();	
	TStreamId tempId;
	iArrUid.Reset();
	iArrStreamId.Reset();
	//read the available pairs of Uid - Streamd Ids.
	for(TInt8 i = 0; i < iControllerCnt;i++)
		{
		iArrUid.AppendL(TUid::Uid(aStream.ReadInt32L()));
		aStream >> tempId;
		iArrStreamId.AppendL(tempId);
		}
	}


TBool CSettingsManager::IsControllerAvailableL(const TUid& aUid,
													  TStreamId&  aStreamId)
	{
	 // if there is no Store fiel return false
	if(!FileExists())
		return 0;
	OpenStore2ReadLC();
	RStoreReadStream instream;
	iRootId = iStore->Root();
	instream.OpenLC(*iStore,iRootId); //open root stream for reading
	instream >> iGenSettingsId;	 //read this to move to controller section
	ReadControllerTableL(instream);
	CleanupStack::PopAndDestroy(2);			
	for(TUint8 i = 0; i < iControllerCnt; i++)
		{
		if(iArrUid[i] == aUid)
			{
		  	aStreamId = iArrStreamId[i];
		  	return 1;	
			}					
	 	}
	return 0;
	}


TInt CSettingsManager::ReadAudioDataL(CMVSConfigAudioFormatDialog* aAudioFormatDlg,
 							                 const TUid& aUid)
	{
	TStreamId controllerId;
	// if the controller settings is not available return
	if(!IsControllerAvailableL(aUid,controllerId))
		return 0;
	
	//open the store to read
	if(!FileExists())
		return 0;
	OpenStore2ReadLC();
	//open the stream of the given controller for reading
	RStoreReadStream instream;
	instream.OpenLC(*iStore,controllerId);
	aAudioFormatDlg->InternalizeL(instream);
	CleanupStack::PopAndDestroy(2);
	return 1;
	}

void CSettingsManager::WriteAudioDataL(CMVSConfigAudioFormatDialog* aAudioFormat,
					  				const TUid& aUid)
	{
	TStreamId controllerId;		
	RStoreWriteStream outstream;
	TBool existingController = IsControllerAvailableL(aUid,controllerId);
	OpenStore2WriteLC();	
	if(!existingController)
		{
		//if controller settings is not available create new stream
		controllerId = outstream.CreateLC(*iStore);		
		}
	else //open the existing for updation
		{
		outstream.ReplaceLC(*iStore,controllerId); 				
		}
	aAudioFormat->ExternalizeL(outstream);
	outstream.CommitL();
	CleanupStack::PopAndDestroy();
	
    /*
    if there is no updation for controller i.e. new controller settings 
    is entered
    */
	if(!existingController)
		{
		outstream.ReplaceLC(*iStore,iRootId); 					
		WriteIndexL(outstream,aUid,controllerId);
		CleanupStack::PopAndDestroy();		
		}
	iStore->CommitL();
	CleanupStack::PopAndDestroy(); //for iStore
	}	

void CSettingsManager::WriteIndexL(RWriteStream& aStream,
								  const TUid& aUid,
								  TStreamId& aStreamId)
	{
	TBool bNewPlugin = aStreamId.Value();
	TUint8 uchExistingPluginCnt = iControllerCnt;
	//write root index
	aStream << iGenSettingsId;
	
	if(bNewPlugin)
		{
		iControllerCnt++;		
		}
	
	aStream.WriteInt8L(iControllerCnt);
	
	//write the uid-streamid for existing plugins
	for(TUint8 i = 0; i < uchExistingPluginCnt; i++)
		{
		aStream.WriteInt32L(iArrUid[i].iUid); //write uid
		aStream << 	iArrStreamId[i];	 //write streamid
		}
	
	if(!bNewPlugin)
		{
		aStream.CommitL();
		return;	
		}
	
	//write uid-streamId for new plugin
	aStream.WriteInt32L(aUid.iUid); //write uid
	aStream << aStreamId;			//write streamid
		
	iArrUid.AppendL(aUid);
	iArrStreamId.AppendL(aStreamId);	
	aStream.CommitL();		
	}


TBool CSettingsManager::ReadVideoDataL(CMVSConfigVideoFormatDialog* aVideoFormatDlg,
 							          const TUid& aUid)
	{
	TStreamId controllerId;
	// if the controller settings is not available return
	if(!IsControllerAvailableL(aUid,controllerId))
		return 0;
	
	//open the store to read
	if(!FileExists())
		return 0;
	OpenStore2ReadLC();
	//open the stream of the given controller for reading
	RStoreReadStream instream;
	instream.OpenLC(*iStore,controllerId);
	aVideoFormatDlg->InternalizeL(instream);
	CleanupStack::PopAndDestroy(2); 
	return 1;
	}


void  CSettingsManager::WriteVideoDataL( CMVSConfigVideoFormatDialog* aVideoFormatDlg,
						 				const TUid& aUid)
	{
	TStreamId controllerId;		
	RStoreWriteStream outstream;
	TBool existingController = IsControllerAvailableL(aUid,controllerId);
	OpenStore2WriteLC();	
	if(!existingController)
		{
		//if controller settings is not available create new stream
		controllerId = outstream.CreateLC(*iStore);		
		}
	else //open the existing for updation
		{
		outstream.ReplaceLC(*iStore,controllerId); 				
		}
	aVideoFormatDlg->ExternalizeL(outstream);
	outstream.CommitL();
	CleanupStack::PopAndDestroy();
	
    /*
    if there is no updation for controller i.e. new controller settings 
    is entered
    */
	if(!existingController)
		{
		outstream.ReplaceLC(*iStore,iRootId); 					
		WriteIndexL(outstream,aUid,controllerId);
		CleanupStack::PopAndDestroy();		
		}
	iStore->CommitL();
	CleanupStack::PopAndDestroy(); //for iStore	
	}

TBool CSettingsManager::FileExists()
	{
	TParse fileStoreName;
	iFsSession.Parse(KFullPathOfFileStore,fileStoreName);
	//check whether a settings file already exists
	RFile file;
	if(file.Open(iFsSession,fileStoreName.FullName(),EFileRead)!= KErrNone) // if the file doesn't exist already
		{
		iHasSettings = 0;
		return EFalse;
		}
	file.Close();
	return ETrue;
	}
