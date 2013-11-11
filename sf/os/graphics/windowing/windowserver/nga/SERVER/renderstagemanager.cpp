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

#include <e32base.h>
#include <e32std.h>
#include "inifile.h"
#include "wstop.h"

#include "renderstagemanager.h"
#include "wspluginmanager.h"

#include "graphics/WsRenderStageFactory.h"

//  Constants

_LIT(KWhiteSpace,									" ");
_LIT(KDefaultRenderStages, 							"flickerbuffer display");
_LIT(KRenderStages,									"RENDERSTAGES");

// Globals

GLREF_D CDebugLogBase *wsDebugLog;

/**
@internalComponent
@prototype
*/
LOCAL_C HBufC* ReverseListLC(const TDesC* aRenderStageList)
	{
	HBufC* writeableCopy = HBufC::NewLC(aRenderStageList->Length());

	TPtr reversedList(writeableCopy->Des());
	TLex lex(*aRenderStageList);
	TBool firstTime = true;
	
	while (true)
		{
		TPtrC currentPluginString = lex.NextToken();
		if (!currentPluginString.Length())
			{
			break;
			}
		if (firstTime)
			{
			reversedList.Insert(0, currentPluginString);
			firstTime = false;
			}
		else
			{
			reversedList.Insert(0, KWhiteSpace());
			reversedList.Insert(0, currentPluginString);
			}
		}
	return writeableCopy;
	}

/**
@internalComponent
@prototype
*/
LOCAL_C	CWsRenderStage *ConstructRenderStageChain(HBufC* reversedRenderStageList, TInt aScreenNumber, MWsScreen *aScreen, MWsScreenRedraw *aScreenRedraw)
	{
	TInt err = KErrNone;

	CWsPluginManager * pluginMgr = CWsTop::PluginManager();

	if (!pluginMgr)
		{
#if _DEBUG
		RDebug::Printf("ConstructRenderStageChain: Failed to obtain Plugin Manager at File: %s, Line: %d\n", __FILE__, __LINE__);
#endif
		return NULL;
		}

	CWsRenderStage* stage;
	CWsRenderStage* savedStage = NULL;
	TBool isLastRenderStage = ETrue;

	TLex lex(*reversedRenderStageList);

	while (true)
		{
		TPtrC ptr = lex.NextToken();
		if (ptr.Length() > 0)
			{
			MWsRenderStageFactory * factory = pluginMgr->FindNamedImplementation<MWsRenderStageFactory>(ptr);
			if (factory)
				{
				if (isLastRenderStage)
					{
					isLastRenderStage = EFalse;
					stage = NULL;
					TRAP(err, stage = factory->CreateFinalRenderStageL(aScreen, aScreenRedraw, aScreenNumber));
					if (err)
						{
#if _DEBUG
						RDebug::Printf("ConstructRenderStageChain: Create final renderstage error at File: %s, Line: %d Error Code: %d\n", __FILE__, __LINE__, err);
#endif
						}
					}
				else
					{
					stage = NULL;
					TRAP(err, stage = factory->CreateRenderStageL(aScreen, aScreenRedraw, savedStage));
					if (err)
						{
#if _DEBUG
						RDebug::Printf("ConstructRenderStageChain: Create renderstage error at File: %s, Line: %d Error Code: %d\n", __FILE__, __LINE__, err);
#endif
						}
					}
				
				if (!stage)
					{
					err = KErrNotFound;
					}
				else
					{
					savedStage = stage;
					}
				}
			else
				{
				err = KErrNotFound;
				TBuf<256> buf;
				_LIT(KTxt1, "ConstructRenderStageChain: Factory ");
				buf.Append(KTxt1);
				buf.Append(ptr);
				_LIT(KTxt2, " not found at File: ");
				buf.Append(KTxt2);
#define FILENAME(f)	_LIT(KFileName, f);
				FILENAME(__FILE__);
				buf.Append(KFileName);
				TBuf<32> buf1;
				_LIT(KTxt3," Line: %d Error Code: %d, \n");
				buf1.Format(KTxt3, __LINE__, err);
				buf.Append(buf1);
				RDebug::RawPrint(buf);
				}

			if (err)
				{
				if (wsDebugLog)
					{
					TBuf<64> buf;
					_LIT(KMissingRenderStage,"Failed to add render stage (%d): ");
					buf.Append(KMissingRenderStage);
					buf.Append(ptr);
					wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,buf,err);
					}
				// This method must fail if *ANY* of the render stages fails during construction
				break;
				}
			else if (wsDebugLog)
				{
				TBuf<64> buf;
				_LIT(KAddedRenderStage,"Added render stage: ");
				buf.Append(KAddedRenderStage);
				buf.Append(ptr);
				wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,buf);
				}
			}
		else
			{
			break;
			}
		}

	if (err)
		{
		delete savedStage;
		savedStage = NULL;
		}

	return savedStage;
	}

CWsRenderStage *CRenderStageManager::ConnectL(TInt aScreenNumber, MWsScreen *aScreen, MWsScreenRedraw *aScreenRedraw)
	{
	TPtrC stagesString;
	const TDesC *renderStageList;
	const TBool customStages = WsIniFile->FindVar(aScreenNumber, KRenderStages, stagesString);

	if (customStages)
		{
		renderStageList = &stagesString;
		}
	else
		{
		renderStageList = &KDefaultRenderStages();
		}
	
	LEAVE_LOG_INSTALL_C;
	HBufC* reversedRenderStageList = ReverseListLC(renderStageList);
	CWsRenderStage *firstRenderStage;
	firstRenderStage = ConstructRenderStageChain(reversedRenderStageList, aScreenNumber, aScreen, aScreenRedraw);
	
	CleanupStack::PopAndDestroy(reversedRenderStageList);
	LEAVE_LOG_UNINSTALL_C;
	
	return firstRenderStage;
	}

void CRenderStageManager::Release(CWsRenderStage *aRenderStage)
	{
		delete aRenderStage; // this deletes the render stage and its children
	}
