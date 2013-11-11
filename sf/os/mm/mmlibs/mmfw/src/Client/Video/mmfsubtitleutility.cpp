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

/**
 @file
 @internalComponent
*/

#include "mmfsubtitleutility.h"


CMMFSubtitleUtility* CMMFSubtitleUtility::NewL(RMMFController& aController, RWsSession &aWs)
	{
	CMMFSubtitleUtility* self = new(ELeave) CMMFSubtitleUtility(aController);
	CleanupStack::PushL(self);
	self->ConstructL(aWs);
	CleanupStack::Pop();
	return self;	
	}

CMMFSubtitleUtility::CMMFSubtitleUtility(RMMFController& aController): iSubtitleSupportCustomCommands(aController)
	{
	}

void CMMFSubtitleUtility::ConstructL(RWsSession &aWs)
	{
	iDevice = new (ELeave) CWsScreenDevice(aWs);
	User::LeaveIfError(iDevice->Construct());
	iSubtitleGc = new (ELeave) CWindowGc(iDevice);
	User::LeaveIfError(iSubtitleGc->Construct());
	}

CMMFSubtitleUtility::~CMMFSubtitleUtility()
	{
	delete iSubtitleGc;
	delete iDevice;
	iSubtitleLanguages.Close();
	iCrpDataArray.Close();
	}

// Add subtitle related config to controller
TInt CMMFSubtitleUtility::AddSubtitleConfig(const TMMFSubtitleWindowConfig& aConfig)
	{
	TCrpData crpData(aConfig.iWindowId);
	// Add the crp data before calling custom commands in case append fails
	TInt err = iCrpDataArray.Append(crpData);
	if (KErrNone == err)
		{
		err = iSubtitleSupportCustomCommands.AddSubtitleConfig(aConfig);
	
		if (KErrNone != err)
			{
			// removed the added crp data on error
			iCrpDataArray.Remove(iCrpDataArray.Count()-1);
			}
		}
	
	return err;
	}

// Found the index of the subtitle data in iCrpDataArray given aWindowId
TInt CMMFSubtitleUtility::FindCrpArrayIndex(TInt aWindowId)
	{
	TInt ret = KErrNotFound;
	for (TInt i = iCrpDataArray.Count(); --i >= 0; )
		{
		if (iCrpDataArray[i].iWindowId == aWindowId)
			{
			ret = i;
			break;
			}
		}
	return ret;
	}

// Remove subtitle related config from controller
TInt CMMFSubtitleUtility::RemoveSubtitleConfig(TInt aWindowId)
	{
	TInt ret = FindCrpArrayIndex(aWindowId);
	if (ret >= 0)
		{		
		iCrpDataArray.Remove(ret);
		ret = iSubtitleSupportCustomCommands.RemoveSubtitleConfig(aWindowId);
		}
	return ret;
	}

// Update subtitle related config from controller and clear the subtitle region
TInt CMMFSubtitleUtility::UpdateSubtitleConfig(const TMMFSubtitleWindowConfig& aConfig)
	{
	TInt ret = FindCrpArrayIndex(aConfig.iWindowId);
	if (ret >= 0)
		{
		// clear the CRP id
		iCrpDataArray[ret].iCrpId.Set(0);

		ret = iSubtitleSupportCustomCommands.UpdateSubtitleConfig(aConfig);
		}
	return ret;
	}

// Return if subtitle is available
TBool CMMFSubtitleUtility::SubtitlesAvailable(RMMFController& aController)
	{
	TBool available = EFalse;
	RMMFVideoPlaySubtitleSupportCustomCommands subtitleSupportCustomCommands(aController);
	subtitleSupportCustomCommands.GetSubtitlesAvailable(available);
	return available;
	}

TInt CMMFSubtitleUtility::EnableSubtitles()
	{
	return iSubtitleSupportCustomCommands.EnableSubtitles();
	}

void CMMFSubtitleUtility::DisableSubtitles()
	{
	for (TInt i = iCrpDataArray.Count(); --i >= 0; )
		{
		iSubtitleSupportCustomCommands.RemoveSubtitleConfig(iCrpDataArray[i].iWindowId);
		}

	iCrpDataArray.Reset();
	
	// still need to send disable command to controller event if array count was 0
	// because window may have been removed after subtitle is enabled
	iSubtitleSupportCustomCommands.DisableSubtitles();
	}

// Helper function to draw CRP
void CMMFSubtitleUtility::DrawCrp(RWindow& aWindow, TInt aCrpIdx, TBool aCallBeginRedraw)
	{
	TCrpData& crpData = iCrpDataArray[aCrpIdx];

	if (crpData.iCrpId.Id() != 0)
		{

		if (aCallBeginRedraw)
			{
			// CRP is ready to be drawn
			aWindow.Invalidate(crpData.iCrpRect);
			aWindow.BeginRedraw(crpData.iCrpRect);
			}
		
		iSubtitleGc->Activate(aWindow);
		iSubtitleGc->DrawWsGraphic(crpData.iCrpId, crpData.iCrpRect);
		iSubtitleGc->Deactivate();
		
		if (aCallBeginRedraw)
			{
			aWindow.EndRedraw();
			}
		}
	}

// Handle CRP ready event from controller and draw the CRP
void CMMFSubtitleUtility::HandleCrpReady(RWindow& aWindow)
	{
	TInt index = FindCrpArrayIndex(aWindow.WsHandle());
	if (index >= 0)
		{
		TCrpData& crpData = iCrpDataArray[index];
		TInt err = iSubtitleSupportCustomCommands.GetCrpParameters(aWindow.WsHandle(), 
																	crpData.iCrpId, 
																	crpData.iCrpRect);

		if (KErrNone == err)
			{
			DrawCrp(aWindow, index, ETrue);
			}
		else
			{
			// Window may have been removed before the event was received, ignore event
			RDebug::Print(_L("CMMFSubtitleUtility::HandleCrpReady aWindowId=%d, err==%d"), aWindow.WsHandle(), err);
			}
		}
	}

// Redraw subtitle CRP if redraw rect intersect with subtitle region
void CMMFSubtitleUtility::RedrawSubtitle(RWindow& aWindow, const TRect& aRedrawRect)
	{
	TInt index = FindCrpArrayIndex(aWindow.WsHandle());
	if (index >= 0 && aRedrawRect.Intersects(iCrpDataArray[index].iCrpRect))
		{
		DrawCrp(aWindow, index, EFalse);
		}
	}

TInt CMMFSubtitleUtility::SetSubtitleLanguage(TLanguage aSubtitleLanguage)
	{
	return iSubtitleSupportCustomCommands.SetSubtitleLanguage(aSubtitleLanguage);
	}

TArray<TLanguage> CMMFSubtitleUtility::SupportedSubtitleLanguagesL()
	{
	TRAPD(err, iSubtitleSupportCustomCommands.GetSupportedSubtitleLanguagesL(iSubtitleLanguages));
	
	// Do not propagate KErrNotSupported; return empty list of languages.
	if (KErrNone != err && KErrNotSupported != err)
		{
		User::Leave(err);
		}
	return iSubtitleLanguages.Array();
	}

TLanguage CMMFSubtitleUtility::SubtitleLanguage()
	{
	TLanguage language = ELangNone;
	
	// ignore returned value, language parameter is unchanged on error
	iSubtitleSupportCustomCommands.GetSubtitleLanguage(language);

	return language;
	}
