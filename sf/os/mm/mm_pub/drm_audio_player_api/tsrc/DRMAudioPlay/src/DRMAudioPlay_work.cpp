/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: DRM Player
*
*/

#include <StifTestInterface.h>
#include "DRMAudioPlay.h"
#include "SimpleSoundPlayer.h"
#include <badesca.h>
//#include "RecordFormats.h"

TInt CDRMAudioPlay::TestCasePlayFileMetaInfoL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;
	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewLC( FileName, TestModuleIf() , *iLogger);

		sndPlayer->iMetaInfo = true;

		CStifItemParser *item;
		TBool MetaInfoTest = 0;
		item = section->GetItemLineL(KTagMetaInfoTest);
		if (item)
			{
			CleanupStack::PushL(item);
			item->GetInt(KTagMetaInfoTest, MetaInfoTest);
			CleanupStack::PopAndDestroy(item);

			if (MetaInfoTest)
				{
				sndPlayer->iMetaInfoTest = true;
				}
			}

		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		aResult.iResult = sndPlayer->iFinalError;
		if (aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}
		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;
		}
	else
		{
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

#define USE_FILE_FORMATS
TInt CDRMAudioPlay::TestCasePlayFileFormatsL(CStifSectionParser * section , TTestResult & aResult)
	{
#ifndef USE_FILE_FORMATS
	return KErrNotSupported;
#endif
#ifdef USE_FILE_FORMATS

	//=/TInt KErrRecFormats = -1130;
	//[*]TPtrC FileNamePtr;
	TInt Error=KErrNone;
	//[*]if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
	//[*]	{
	//[*]	TFileName FileName = FileNamePtr;
	//[*]	iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
	//[*]	CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewLC( FileName, TestModuleIf() , *iLogger);

		// * ************************************************

		REComSession session;
		TRAPD(err,session = REComSession::OpenL());
		if (err != KErrNone)
			{
			iLogger->Log(_L("Error (%d) opening REComSession"),err);
			aResult.iResult = err;
			aResult.iResultDes.Copy( KTestCaseResultFail() );
			return KErrExecuted;
			}

		//-/for (TInt i=0; i<1; i++)
			//-/{
			//TUid id;
			//id.iUid = KUidInterfaceMMFController; //KMmfUidPluginInterfaceController; //KMmfUidPluginInterfaceFormatDecode; //KMmfUidPluginInterfaceFormatEncode
			//TUid id = KUidInterfaceMMFController;
			TBool UsingDefault;
			TUid id;
			id.iUid = GetIntL(section, KTagInterfaceId, UsingDefault, KUidInterfaceMMFController.iUid);
			RImplInfoPtrArray aArray;

			iLogger->Log(_L("Calling REComSession::ListImplementationsL()") );
			TRAPD(err2,REComSession::ListImplementationsL(id,aArray));
			if (err2 != KErrNone)
				{
				iLogger->Log(_L("Error (%d) getting implementations (ListImplementationsL)"),err2);
				aResult.iResult = err;
				aResult.iResultDes.Copy( KTestCaseResultFail() );
				return KErrExecuted;
				//return KErrRecFormats;
				}

			for (TInt j=0; j<aArray.Count() ; j++)
				{
				CMMFControllerImplementationInformation* controller = NULL;

				iLogger->Log(_L("Calling CMMFControllerImplementationInformation::NewL()"));
				TRAPD(err3, controller=CMMFControllerImplementationInformation::NewL(id));
				//TRAPD(err3,controller = CMMFControllerImplementationInformation::NewL( *(aArray[j]) )  );

				if (err3 != KErrNone)
					{
					iLogger->Log(_L("Error %d openning controller"),err3);
					aResult.iResult = Error = err3;
					break;
					//continue;
					}
				CleanupStack::PushL(controller);

				iLogger->Log(_L("Calling CMMFControllerImplementationInformation::PlayFormats()"));
				RMMFFormatImplInfoArray formats = controller->PlayFormats();
				for (TInt k = 0; k < formats.Count(); k++)
					{
					CMMFFormatImplementationInformation* info = formats[k];
					CleanupStack::PushL(info);

					for (TInt l = 0; l < (info->SupportedFileExtensions()).Count(); l++)
						{
						iLogger->Log(_L("Calling CMMFFormatImplementationInformation::SupportedFileExtensions()"));
						iLogger->Log(_L("Format supported: %S"),(info->SupportedFileExtensions())[l]);
						}
					CleanupStack::PopAndDestroy();
					}
				formats.Close();
				CleanupStack::PopAndDestroy(controller);
				CleanupStack::PopAndDestroy();
				}
			//-/}
		aArray.Close();

		session.Close();

		//return KErrNone;

		// * ************************************************
		//[*]iLogger->Log(_L("Starting scheduler"));
		//[*]CActiveScheduler::Start();

		//[*]iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		//[*]aResult.iResult = sndPlayer->iFinalError ;
		if (aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}
		//[*]CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;
	//[*]	}
	//[*]else
	//[*]	{
	//[*]	//Falta el nombre del archivo a reproducir
	//[*]	aResult.iResultDes.Copy(KConfigInvalid());
	//[*]	aResult.iResult = KErrNotExecuted;
	//[*]	return KErrConfigInvalid;
	//[*]	}
#endif
	}

TInt CDRMAudioPlay::GetIntL(CStifSectionParser *aSection, const TDesC &aTag, TBool &aUsingDefault, const TInt &aDefaultInt, TBool aContinue)
	{
	TInt ParseError=KErrNotFound;
	TInt TempDelay=0;
	TInt ReturnValue=aDefaultInt;

	CStifItemParser *item;
	if (aContinue)
		{
		item = aSection->GetNextItemLineL(aTag);
		}
	else
		{
		 item = aSection->GetItemLineL(aTag);
		}
	if (item)
		{
		CleanupStack::PushL(item);
		ParseError = item->GetInt(aTag, TempDelay) ;
		CleanupStack::PopAndDestroy(item);
		}

	if (ParseError)
		{	//Integer not found, using defaults
		aUsingDefault=ETrue;
		}
	else
		{	//Integer found.
		aUsingDefault=EFalse;
		ReturnValue=TempDelay;
		}

	if (ParseError) {iLogger->Log(_L("Error retrieving integer, [%S] not found"), &aTag);}
	iLogger->Log(_L("Integer read, value (%d)") , ReturnValue);
	return ReturnValue;
	}
