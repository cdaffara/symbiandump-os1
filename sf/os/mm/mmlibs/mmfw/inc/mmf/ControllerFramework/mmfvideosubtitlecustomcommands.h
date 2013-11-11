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

#ifndef MMFVIDEOSUBTITLECUSTOMCOMMANDS_H
#define MMFVIDEOSUBTITLECUSTOMCOMMANDS_H

#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfvideo.h>
#include <gdi.h>
#include <w32std.h>

/**
 * Unique identifier for subtitle custom command interface
 * @publishedPartner
 * @prototype
 */
const TUid KUidInterfaceMMFVideoPlaySubtitleSupport = {0x10285C9D};

/**
 * Identifier for subtitle message Crp Ready, used by AviController to indicate 
 * the CRP is ready to be drawn.
 * @publishedPartner
 * @prototype
 */
const TUid KMMFEventCategoryVideoSubtitleCrpReady = {0x10285C9E};

/**
 * Class encapsulates all window information required for DevSubtitle to initialize the CRP.
 * @publishedPartner
 * @prototype
 * This class is used to send subtitle window client configuration to controller
 */
class TMMFSubtitleWindowConfig
	{
public:
	/** Window unique identifier */
	TInt iWindowId; 

	/** Video window clip rectangle */
	TRect iWindowClipRect;

	/** Video window display mode */
	TDisplayMode iDisplayMode;
	
	/** Video rotation angle */
	TVideoRotation iRotation;
	};

/**
 * @publishedPartner 
 * @prototype
 * 
 * Mixin class for subtitle custom commands implemented by the controller. The custom
 * command parser calls methods in this class to deliver the requests to the
 * controller.
 */
class MMMFVideoPlaySubtitleSupportCustomCommandImplementor
	{
public:
	/**
	 * Gets the CRP parameters associated with a display
	 * 
	 * @param aWindowId Unique identifier of the window.
	 * @param aId Return the CRP id associated with the given display id
	 * @param aCrpRect Return the subtitle region for drawing the CRP
	 */
	virtual void MvpsusGetCrpParametersL(TInt aWindowId, TWsGraphicId& aId, TRect& aCrpRect) = 0;
	
	/**
	 * Adds subtitle configuration data for a window.
	 * 
	 * @param aConfig Subtitle configuration data for a window.
	 */
	virtual void MvpsusAddSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfig) = 0;
	
	/**
	 * Removes the subtitle configuration data associated with the given window.
	 * 
	 * @param aWindowId Unique identifier of the window associated with the 
	 * configuration data being removed.
	 */
	virtual void MvpsusRemoveSubtitleConfigL(TInt aWindowId) = 0;
	
	/** 
	 * Reconfigures the subtitle configuration.
	 * 
	 * @param aConfig Subtitle configuration data for a window.
	 */
	virtual void MvpsusUpdateSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfig) = 0;
	
	/** 
	 * Checks if subtitles are available in the current video stream.  
	 * 
	 * @param aAvailable When this method returns, this is set the ETrue if the 
	 * current video stream subtitles.  
	 * @leave KErrNotSupported if the controller does not support subtitles.  Otherwise
	 * another system-wide error code.
	 */
	virtual void MvpsusGetSubtitlesAvailableL(TBool& aAvailable) = 0;
	
	/**
	 * Disables subtitles during video playback.
	 * 
	 * @leave System wide error code
	 */
	virtual void MvpsusDisableSubtitlesL() = 0;
	
	/**
	 * Enables subtitles during video playback.
	 * 
	 * @leave KErrNotSupported if subtitles are not supported
	 * @leave KErrNotFound if subtitle data is not found
	 * @leave Otherwise leaves with any of the system wide error codes.
	 */
	virtual void MvpsusEnableSubtitlesL() = 0;
	
	/**
	 * Gets the current subtitle language
	 * 
	 * @param aLanguage When this method returns, this is set the the current subtitle language.
	 * @leave KErrNotSupported The enabled subtitle does not contain language
	 * information.
	 * @leave Otherwise leaves with any of the system wide error codes.
	 */
	virtual void MvpsusGetSubtitleLanguageL(TLanguage& aLanguage) = 0;
	
	/**
	 * Gets array of all supported languages that are available in the current 
	 * video player.
	 * 
	 * @param aAvailable When this function returns, this array will hold the
	 * supported languages.
	 * @leave KErrNotSupported The enabled subtitle does not contain language 
	 * information
	 * @leave Otherwise leaves with any of the system wide error codes.
	 */
	virtual void MvpsusGetSupportedSubtitleLanguagesL(RArray<TLanguage>& aAvailable) = 0;
	
	/**
	 * Sets the current subtitle language
	 * 
	 * @param aLanguage Language to be used for subtitle stream.
	 * @leave KErrNotSupported The enabled subtitle does not support the requested 
	 * language.
	 * @leave Otherwise leaves with any of the system wide error codes.
	 */
	virtual void MvpsusSetSubtitleLanguageL(TLanguage aLanguage) = 0;
	};

/**
 * @publishedPartner 
 * @prototype
 * 
 * Custom command parser class to be used by controller plugins wishing to support 
 * custom subtitle commands in a video play controller.
 * 
 * The controller plugin must be derived from 
 * MMMFVideoPlaySubtitleSupportCustomCommandImplementor to use this class. The 
 * controller plugin should create an object of this type and add it to the list of 
 * custom command parsers in the controller framework.
 * 
 * @see MMMFVideoPlaySubtitleSupportCustomCommandImplementor
 * @see RMMFVideoPlaySubtitleSupportCustomCommands
 */
class CMMFVideoPlaySubtitleSupportCustomCommandParser : public CMMFCustomCommandParserBase
	{
public:
	IMPORT_C static CMMFVideoPlaySubtitleSupportCustomCommandParser* NewL(MMMFVideoPlaySubtitleSupportCustomCommandImplementor& aImplementor);
	IMPORT_C ~CMMFVideoPlaySubtitleSupportCustomCommandParser();
	void HandleRequest(TMMFMessage& aMessage);
	
private:
	CMMFVideoPlaySubtitleSupportCustomCommandParser(MMMFVideoPlaySubtitleSupportCustomCommandImplementor& aImplementor);
	
	void DoHandleRequestL(TMMFMessage& aMessage);
	
	void DoAddSubtitleConfigL(TMMFMessage& aMessage);
	void DoRemoveSubtitleConfigL(TMMFMessage& aMessage);
	void DoUpdateSubtitleConfigL(TMMFMessage& aMessage);
	void DoGetSubtitlesAvailableL(TMMFMessage& aMessage);
	void DoDisableSubtitlesL(TMMFMessage& aMessage);
	void DoEnableSubtitlesL(TMMFMessage& aMessage);
	void DoGetSupportedLanguagesDataL(TMMFMessage& aMessage);
	void DoGetSupportedLanguagesCountL(TMMFMessage& aMessage);
	void DoGetSubtitleLanguageL(TMMFMessage& aMessage);
	void DoSetSubtitleLanguageL(TMMFMessage& aMessage);
	void DoGetCrpParametersL(TMMFMessage& aMessage);

private:
	MMMFVideoPlaySubtitleSupportCustomCommandImplementor& iImplementor;
	RArray<TLanguage> iAvailableLanguages;
	};

/**
 * @publishedPartner 
 * @prototype
 * 
 * Client class to access functionality specific to a subtitle supported playback 
 * controller. The class uses the custom command function of the controller plugin, 
 * and removes the necessity for the client to formulate the custom commands.
 */
class RMMFVideoPlaySubtitleSupportCustomCommands : public RMMFCustomCommandsBase
	{
public:

	IMPORT_C RMMFVideoPlaySubtitleSupportCustomCommands(RMMFController& aController);
	IMPORT_C TInt AddSubtitleConfig(const TMMFSubtitleWindowConfig& aConfig);
	IMPORT_C TInt RemoveSubtitleConfig(TInt aWindowId);
	IMPORT_C TInt UpdateSubtitleConfig(const TMMFSubtitleWindowConfig& aConfig);
	IMPORT_C TInt GetSubtitlesAvailable(TBool& aAvailable);
	IMPORT_C TInt DisableSubtitles();
	IMPORT_C TInt EnableSubtitles();
	IMPORT_C TInt GetCrpParameters(TInt aWindowId, TWsGraphicId& aId, TRect& aCrpRect);
	IMPORT_C TInt GetSubtitleLanguage(TLanguage& aLanguage);
	IMPORT_C void GetSupportedSubtitleLanguagesL(RArray<TLanguage>& aSubtitleLanguages);
	IMPORT_C TInt SetSubtitleLanguage(TLanguage aSubtitleLanguage);
	};

#endif // MMFVIDEOSUBTITLECUSTOMCOMMANDS_H
