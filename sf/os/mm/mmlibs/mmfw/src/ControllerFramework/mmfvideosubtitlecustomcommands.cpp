// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mmfvideosubtitlecustomcommands.h"

/**
@internalComponent
*/
enum TMMFSubtitleSupportMessages
	{
	EMMFSubtitleSupportAvailable,
	EMMFSubtitleSupportEnable,
	EMMFSubtitleSupportDisable,
	EMMFSubtitleSupportGetLanguageSupportCount,
	EMMFSubtitleSupportGetLanguageSupportData,
	EMMFSubtitleSupportSetLanguage,
	EMMFSubtitleSupportGetLanguage,
	EMMFSubtitleSupportUpdateSubtitleConfig,
	EMMFSubtitleSupportAddSubtitleConfig,
	EMMFSubtitleSupportRemoveSubtitleConfig,
	EMMFSubtitleSupportGetCrpParameter
	};

/**
@internalComponent
*/
class TCrpParameters
	{
public:
	TCrpParameters() : iId(TWsGraphicId::EUninitialized) {}
	
	TWsGraphicId iId;
	TRect iCrpRect;
	};

/**
 * Constructor.
 * 
 * @param aController The client side controller object to be used by this custom 
 * command interface.
 */
EXPORT_C RMMFVideoPlaySubtitleSupportCustomCommands::RMMFVideoPlaySubtitleSupportCustomCommands(RMMFController& aController)
	: RMMFCustomCommandsBase(aController, KUidInterfaceMMFVideoPlaySubtitleSupport)
	{
	}

/**
 * Adds subtitle configuration data for a window.
 * 
 * @param aConfig Subtitle configuration data for a window.
 * @return KErrNone if completed succesfully, otherwise one of the system wide error codes.
 */
EXPORT_C TInt RMMFVideoPlaySubtitleSupportCustomCommands::AddSubtitleConfig(const TMMFSubtitleWindowConfig& aConfig)
	{
	TPckgBuf<TMMFSubtitleWindowConfig> configPckg(aConfig);
	
	return iController.CustomCommandSync(iDestinationPckg,
			EMMFSubtitleSupportAddSubtitleConfig, 
			configPckg,
			KNullDesC8);
	}

/**
 * Removes the subtitle configuration data associated with the given window.
 * 
 * @param aWindowId Unique identifier of the window associated with the 
 * configuration data being removed.
 * @return KErrNone if completed succesfully, otherwise one of the system wide error codes.
 */
EXPORT_C TInt RMMFVideoPlaySubtitleSupportCustomCommands::RemoveSubtitleConfig(TInt aWindowId)
	{
	TPckgBuf<TInt> windowPckg(aWindowId);
	
	return iController.CustomCommandSync(iDestinationPckg,
			EMMFSubtitleSupportRemoveSubtitleConfig,
			windowPckg,
			KNullDesC8);
	}

/** 
 * Reconfigures subtitle configuration.
 * 
 * @param aConfig Subtitle configuration data for a window.
 * @return KErrNone if completed succesfully, otherwise one of the system wide error codes.
 */
EXPORT_C TInt RMMFVideoPlaySubtitleSupportCustomCommands::UpdateSubtitleConfig(const TMMFSubtitleWindowConfig& aConfig)
	{
	TPckgBuf<TMMFSubtitleWindowConfig> configPckg(aConfig);
	
	return iController.CustomCommandSync(iDestinationPckg,
			EMMFSubtitleSupportUpdateSubtitleConfig, 
			configPckg,
			KNullDesC8);
	}

/** 
 * Checks if subtitles are available in the current video stream.  
 * 
 * @param aAvailable When this function returns, this is set to ETrue if the 
 * controller supports subtitle extensions and subtitles are available in the 
 * current video stream.
 * @return KErrNone if subtitles are supported by the controller; KErrNotSupported 
 * if the controller does not support subtitles; otherwise other system error code.
 */
EXPORT_C TInt RMMFVideoPlaySubtitleSupportCustomCommands::GetSubtitlesAvailable(TBool& aAvailable)
	{
	TPckgBuf<TBool> avail;
	
	TInt err = iController.CustomCommandSync(iDestinationPckg,
			EMMFSubtitleSupportAvailable,
			KNullDesC8,
			KNullDesC8,
			avail);
	
	aAvailable = (KErrNone == err) && avail();
	return err;
	}

/**
 * Disables subtitles during video playback.
 * 
 * @return KErrNone if completed succesfully, otherwise one of the system wide error codes.
 */
EXPORT_C TInt RMMFVideoPlaySubtitleSupportCustomCommands::DisableSubtitles()
	{
	return iController.CustomCommandSync(iDestinationPckg,
			EMMFSubtitleSupportDisable,
			KNullDesC8,
			KNullDesC8);
	}

/**
 * Enables subtitles during video playback.
 * 
 * @return KErrNone if subtitles are supported; KErrNotSupported if 
 * controller does not support subtitles; KErrNotFound if subtitle data 
 * not found; otherwise other system error code.
 */
EXPORT_C TInt RMMFVideoPlaySubtitleSupportCustomCommands::EnableSubtitles()
	{	
	return iController.CustomCommandSync(iDestinationPckg,
			EMMFSubtitleSupportEnable,
			KNullDesC8,
			KNullDesC8);
	}

/**
 * Gets the CRP parameters associated with a display
 * 
 * @param aWindowId The window id used in the query.
 * @param aId Return the CRP id associated with the given display id
 * @param aCrpRect Return the subtitle region for drawing the CRP
 * 
 * @return KErrNone on success; system wide error code otherwise.
 */
EXPORT_C TInt RMMFVideoPlaySubtitleSupportCustomCommands::GetCrpParameters(TInt aWindowId, TWsGraphicId& aId, TRect& aCrpRect)
	{
	TPckgBuf<TInt> windowIdPckg(aWindowId);
	TPckgBuf<TCrpParameters> parameterPckg;
	
	TInt err = iController.CustomCommandSync(iDestinationPckg,
			EMMFSubtitleSupportGetCrpParameter,
			windowIdPckg,
			KNullDesC8,
			parameterPckg);
	
	if (KErrNone == err)
		{
		aId = parameterPckg().iId;
		aCrpRect = parameterPckg().iCrpRect;
		}
	
	return err;
	}

/**
 * Gets the current subtitle language
 * 
 * @param aLanguage On return, set the the current language.
 * @return KErrNone on success. KErrNotSupported when the enabled subtitle does not 
 * contain language information.  Otherwise system wide error code.
 */
EXPORT_C TInt RMMFVideoPlaySubtitleSupportCustomCommands::GetSubtitleLanguage(TLanguage& aLanguage)
	{
	TPckgBuf<TLanguage> languagePckg;
	TInt err = iController.CustomCommandSync(iDestinationPckg,
			EMMFSubtitleSupportGetLanguage,
			KNullDesC8,
			KNullDesC8,
			languagePckg);
	
	if (KErrNone == err)
		{
		aLanguage = languagePckg();
		}
	
	return err;
	}

/**
 * Gets a list of all available languages.
 * 
 * @param aAvailable Array of available languages
 * @leave KErrNotSupported If the current controller does not support subtitles.
 * @leave KErrNotFound If the controller cannot find subtitle data.
 * @leave Otherwise leaves with any of the system wide error codes.
 */
EXPORT_C void RMMFVideoPlaySubtitleSupportCustomCommands::GetSupportedSubtitleLanguagesL(RArray<TLanguage>& aSubtitleLanguages)
	{
	aSubtitleLanguages.Reset();
	
	TPckgBuf<TInt> countPckg;
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg,
			EMMFSubtitleSupportGetLanguageSupportCount,
			KNullDesC8,
			KNullDesC8,
			countPckg));
	
	TInt count = countPckg();
	
	if (count > 0)
		{	
		HBufC8* buf = HBufC8::NewLC(count * sizeof(TLanguage));	
		TPtr8 ptr = buf->Des();
	
		User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
				EMMFSubtitleSupportGetLanguageSupportData, 
				KNullDesC8,
				KNullDesC8,
				ptr));
		
		TPckgBuf<TLanguage> langPckg;
		RDesReadStream stream(ptr);
		CleanupClosePushL(stream);
	
		for (TInt i = 0; i < count; i++)
			{
			stream.ReadL(langPckg);
			aSubtitleLanguages.AppendL(langPckg());
			}
	
		CleanupStack::PopAndDestroy(2, buf); //stream, buf
		}
	}

/**
 * Sets the current subtitle language
 * 
 * @param aSubtitleLanguage Language to be used for subtitle stream.
 * @return KErrNone on success.  KErrNotSupported when the enabled subtitle 
 * does not contain language information.  Otherwise system wide error code.
 */
EXPORT_C TInt RMMFVideoPlaySubtitleSupportCustomCommands::SetSubtitleLanguage(TLanguage aSubtitleLanguage)
	{
	TPckgBuf<TLanguage> languagePckg(aSubtitleLanguage);
	
	return iController.CustomCommandSync(iDestinationPckg,
			EMMFSubtitleSupportSetLanguage,
			languagePckg,
			KNullDesC8);
	}

/**
 * Creates a new custom command parser capable of handling subtitle support commands.
 * 
 * @param aImplementor A reference to the controller plugin that owns this new 
 * object.
 * 
 * @return A pointer to the object created.
 */
EXPORT_C CMMFVideoPlaySubtitleSupportCustomCommandParser* CMMFVideoPlaySubtitleSupportCustomCommandParser::NewL(
		MMMFVideoPlaySubtitleSupportCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFVideoPlaySubtitleSupportCustomCommandParser(aImplementor);
	}

CMMFVideoPlaySubtitleSupportCustomCommandParser::CMMFVideoPlaySubtitleSupportCustomCommandParser(MMMFVideoPlaySubtitleSupportCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFVideoPlaySubtitleSupport),
	iImplementor(aImplementor)
	{
	}

/**
 * Destructor.
 */
EXPORT_C CMMFVideoPlaySubtitleSupportCustomCommandParser::~CMMFVideoPlaySubtitleSupportCustomCommandParser()
	{
	iAvailableLanguages.Close();
	}

/**
 * Handles a request from the client. Called by the controller framework.
 * 
 * @param aMessage The message to be handled.
 */
void CMMFVideoPlaySubtitleSupportCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFVideoPlaySubtitleSupport)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		aMessage.Complete(error);
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFVideoPlaySubtitleSupportCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch (aMessage.Function())
		{
	case EMMFSubtitleSupportAvailable:
		DoGetSubtitlesAvailableL(aMessage);
		break;
	case EMMFSubtitleSupportEnable:
		DoEnableSubtitlesL(aMessage);
		break;
	case EMMFSubtitleSupportDisable:
		DoDisableSubtitlesL(aMessage);
		break;
	case EMMFSubtitleSupportGetLanguageSupportCount:
		DoGetSupportedLanguagesCountL(aMessage);
		break;
	case EMMFSubtitleSupportGetLanguageSupportData:
		DoGetSupportedLanguagesDataL(aMessage);
		break;
	case EMMFSubtitleSupportGetLanguage:
		DoGetSubtitleLanguageL(aMessage);
		break;
	case EMMFSubtitleSupportSetLanguage:
		DoSetSubtitleLanguageL(aMessage);
		break;
	case EMMFSubtitleSupportUpdateSubtitleConfig:
		DoUpdateSubtitleConfigL(aMessage);
		break;
	case EMMFSubtitleSupportAddSubtitleConfig:
		DoAddSubtitleConfigL(aMessage);
		break;
	case EMMFSubtitleSupportRemoveSubtitleConfig:
		DoRemoveSubtitleConfigL(aMessage);
		break;
	case EMMFSubtitleSupportGetCrpParameter:
		DoGetCrpParametersL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	}

void CMMFVideoPlaySubtitleSupportCustomCommandParser::DoGetSubtitlesAvailableL(TMMFMessage& aMessage)
	{
	TBool avail;
	iImplementor.MvpsusGetSubtitlesAvailableL(avail);
	TPckgBuf<TBool> availPckg(avail);
	aMessage.WriteDataToClientL(availPckg);
	}

void CMMFVideoPlaySubtitleSupportCustomCommandParser::DoUpdateSubtitleConfigL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFSubtitleWindowConfig> configPckg;
	aMessage.ReadData1FromClientL(configPckg);
	iImplementor.MvpsusUpdateSubtitleConfigL(configPckg());
	}

void CMMFVideoPlaySubtitleSupportCustomCommandParser::DoAddSubtitleConfigL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFSubtitleWindowConfig> configPckg;
	aMessage.ReadData1FromClientL(configPckg);
	iImplementor.MvpsusAddSubtitleConfigL(configPckg());
	}

void CMMFVideoPlaySubtitleSupportCustomCommandParser::DoRemoveSubtitleConfigL(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> windowPckg;
	aMessage.ReadData1FromClientL(windowPckg);
	iImplementor.MvpsusRemoveSubtitleConfigL(windowPckg());
	}

void CMMFVideoPlaySubtitleSupportCustomCommandParser::DoDisableSubtitlesL(TMMFMessage& /*aMessage*/)
	{
	iImplementor.MvpsusDisableSubtitlesL();
	}

void CMMFVideoPlaySubtitleSupportCustomCommandParser::DoEnableSubtitlesL(TMMFMessage& /*aMessage*/)
	{
	iImplementor.MvpsusEnableSubtitlesL();
	}

void CMMFVideoPlaySubtitleSupportCustomCommandParser::DoGetSupportedLanguagesDataL(TMMFMessage& aMessage)
	{
	HBufC8* buf = HBufC8::NewLC(iAvailableLanguages.Count() * sizeof(TLanguage));
	TPtr8 ptr = buf->Des();
	RDesWriteStream stream(ptr);
	CleanupClosePushL(stream);
	
	for (TInt i = 0; i < iAvailableLanguages.Count(); i++)
		{
		TPckgBuf<TLanguage> langPckg(iAvailableLanguages[i]);
		stream.WriteL(langPckg);
		}
	
	stream.CommitL();
	aMessage.WriteDataToClientL(ptr);
	CleanupStack::PopAndDestroy(2, buf); // stream, buf
	}

void CMMFVideoPlaySubtitleSupportCustomCommandParser::DoGetSupportedLanguagesCountL(TMMFMessage& aMessage)
	{
	iAvailableLanguages.Reset();
	iImplementor.MvpsusGetSupportedSubtitleLanguagesL(iAvailableLanguages);
	
	TPckgBuf<TInt> countPckg(iAvailableLanguages.Count());
	aMessage.WriteDataToClientL(countPckg);
	}

void CMMFVideoPlaySubtitleSupportCustomCommandParser::DoGetSubtitleLanguageL(TMMFMessage& aMessage)
	{
	TLanguage lang;
	iImplementor.MvpsusGetSubtitleLanguageL(lang);
	TPckgBuf<TLanguage> langPckg(lang);
	aMessage.WriteDataToClientL(langPckg);
	}

void CMMFVideoPlaySubtitleSupportCustomCommandParser::DoSetSubtitleLanguageL(TMMFMessage& aMessage)
	{
	TPckgBuf<TLanguage> langPckg;
	aMessage.ReadData1FromClientL(langPckg);
	iImplementor.MvpsusSetSubtitleLanguageL(langPckg());
	}

void CMMFVideoPlaySubtitleSupportCustomCommandParser::DoGetCrpParametersL(TMMFMessage& aMessage)
	{
	TCrpParameters crpParams;
	TPckgBuf<TInt> displayPckg;
	aMessage.ReadData1FromClientL(displayPckg);
	iImplementor.MvpsusGetCrpParametersL(displayPckg(), crpParams.iId, crpParams.iCrpRect);
	
	TPckgBuf<TCrpParameters> paramPckg(crpParams);
	aMessage.WriteDataToClientL(paramPckg);
	}
