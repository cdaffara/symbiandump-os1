/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "sound_util.h"

// Translation table for RMdaDevSound::TMdaSoundEncoding set

_LIT(KStrEMdaSoundEncoding8BitPCM,			"EMdaSoundEncoding8BitPCM");
_LIT(KStrEMdaSoundEncoding16BitPCM,			"EMdaSoundEncoding16BitPCM");
_LIT(KStrEMdaSoundEncoding8BitALaw,			"EMdaSoundEncoding8BitALaw");
_LIT(KStrEMdaSoundEncoding8BitMuLaw,		"EMdaSoundEncoding8BitMuLaw");

// NB! Wrong member for negative testing
_LIT(KStrEMdaSoundEncoding40BitPCM,			"EMdaSoundEncoding40BitPCM");
_LIT(KStrNull,								"");

const CDataWrapperBase::TEnumEntryTable		TSoundUtil::iEnumTableSoundEncoding[] =
	{
//	Enum as a descriptor					Enum
	KStrEMdaSoundEncoding8BitPCM,			RMdaDevSound::EMdaSoundEncoding8BitPCM,
	KStrEMdaSoundEncoding16BitPCM,			RMdaDevSound::EMdaSoundEncoding16BitPCM,
	KStrEMdaSoundEncoding8BitALaw,			RMdaDevSound::EMdaSoundEncoding8BitALaw,
	KStrEMdaSoundEncoding8BitMuLaw,			RMdaDevSound::EMdaSoundEncoding8BitMuLaw,
	KStrEMdaSoundEncoding40BitPCM,			0x00000040,
	KStrNull,								-1
	};

/**
 * Loading of generinc sound driver required for RMDaDevSound interface
 * 
 * @param aLll 				LDD driver name
 * @param aPdd				PDD driver name
 *
 * @return					System wide error
 *
 * @leave					no
 */
TInt	TSoundUtil::LoadSoundDrivers(const TDesC& aLdd, const TDesC& aPdd)
	{
	TInt rerr = KErrNone;
	
	rerr = User::LoadPhysicalDevice(aPdd);
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		return rerr;
		}
	rerr = User::LoadLogicalDevice(aLdd);	
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		return rerr;
		}
	return KErrNone;	
	}

/**
 * Un-loading of generinc sound driver required for RMDaDevSound interface
 * 
 * @param aLll 				LDD driver name
 * @param aPdd				PDD driver name
 *
 * @return					System wide error
 *
 * @leave					no
 */
TInt	TSoundUtil::FreeSoundDrivers(const TDesC& aLdd, const TDesC& aPdd)
	{
	TInt rerr = KErrNone;
	
	rerr = User::FreePhysicalDevice(aPdd);
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		return rerr;
		}
	rerr = User::FreeLogicalDevice(aLdd);	
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		return rerr;
		}
	return KErrNone;	
	}

/**
 * Store current RBuf8 content into the binary file
 * 
 * @param aFile 			file name to save audio data
 *
 * @return					System wide error
 *
 * @leave					System wide error
 */
TInt	TSoundUtil::RAudioBuffer::SaveAudioDataL(const TDesC& aFile)
	{	
	RFs fsSession;
	::CleanupClosePushL(fsSession);
	User::LeaveIfError(fsSession.Connect()); 	
	TInt err = KErrNone;
	RFile file;
	err = file.Create(fsSession, aFile, EFileStream | EFileWrite);
	if (!err)
		{
		err = file.Write(*this);

		file.Close();
		}	
	fsSession.Close();
	CleanupStack::PopAndDestroy();		
	return err;
	}

/**
 * Load binary file content into RBuf8
 * 
 * @param aFile 			file name where to load audio data
 *
 * @return					System wide error
 *
 * @leave					System wide error
 */
TInt	TSoundUtil::RAudioBuffer::LoadAudioDataL(const TDesC& aFile)
	{
	RFs fsSession;
	::CleanupClosePushL(fsSession);
	User::LeaveIfError(fsSession.Connect());	
	TFindFile	findFile(fsSession);
	TInt err = findFile.FindByDir(aFile, KStrNull);
	if ( err == KErrNone )
		{
		const TDesC&	fileName=findFile.File();
		RFile file;
		err = file.Open(fsSession, fileName, EFileStream | EFileRead);
		if (!err)
			{
			err = file.Read(*this);
			file.Close();
			}
		}
	fsSession.Close();
	CleanupStack::PopAndDestroy();		
	return err;	
	}

/**
 * Delete file
 * 
 * @param aFile 			file name to delete
 *
 * @return					System wide error
 *
 * @leave					System wide error
 */
TInt	TSoundUtil::DeleteFileL(const TDesC& aFile)
	{
	RFs fsSession;
	CleanupClosePushL(fsSession);
	User::LeaveIfError(fsSession.Connect());
	TInt err;
	err = fsSession.Delete(aFile);	
	fsSession.Close();
	CleanupStack::PopAndDestroy();
	return err;
	}
