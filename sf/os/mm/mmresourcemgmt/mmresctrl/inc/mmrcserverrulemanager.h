// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// mmrcserverpolicaymanager.h
// 
//

#ifndef MMRCSERVERPOLICYMANAGER_H__
#define MMRCSERVERPOLICYMANAGER_H__

#include <e32std.h>
#include <e32base.h>
#include <a3f/a3fbase.h>
#include "mmrcclientserver.h"

class CMMRCServerSession;
class CMMRCServerController;
class MMMRCServerInfoState;

/**
@internalComponent

This class implements the MMRC Server Rule Manager.
The main purpose of this class is to
*/
NONSHARABLE_CLASS( CMMRCServerRuleManager ): public CBase
	{
public:
	/**
	 * Constructs, and returns a pointer to, a new CMMRCServerRuleManager object.
	 * Leaves on failure.
	 * @param CMMRCServerController& A reference on the MMRC Server controller
	 * @return CMMRCServerRuleManager* A pointer to newly created utlitly object.
	 */
	static CMMRCServerRuleManager* NewL(CMMRCServerController& aServerController);

	/**
	 * Constructs, leaves object on the cleanup stack, and returns a pointer
	 * to, a new CMMRCServerRuleManager object.
	 * Leaves on failure.
	 * @param CMMRCServerController& A reference on the MMRC Server controller
	 * @return CMMRCServerRuleManager* A pointer to newly created utlitly object.
	 */
	static CMMRCServerRuleManager* NewLC(CMMRCServerController& aServerController);

	/**
	 * Destructor.
	 */
	~CMMRCServerRuleManager();

public:
	/**
	 * Deduce the rule on the message according to the previous and current state
	 * Return KErrNotFound if no rule has been found.
	 * Leaves on failure.
	 * @param TAudioState aAudioStateAllocatedResource
	 * @param TAudioState aAudioStateRequiringProcess
	 * @return TMMRCRule rule
	 */
	TMMRCRule DecisionRule( TAudioState aAudioStateAllocatedResource, TAudioState aAudioStateRequiringProcess );

	/**
	 * Deduce the rule on the message according to the previous and current state
	 * Return KErrNotFound if no rule has been found.
	 * @param TAudioState aAudioStateAllocatedResource
	 * @param TAudioState aAudioStateRequiringProcess
	 * @return TMMRCRule rule
	 */
	TReason ActionReason( TAudioState aAudioStateLastCommited, TAudioState aAudioStateRequested );

private:
	/**
	 * By default Symbian 2nd phase constructor is private.
	 * @param CMMRCServerController& A reference on the MMRC Server controller
	 */
	CMMRCServerRuleManager(CMMRCServerController& aServerController);

	/**
	 * By default Symbian 2nd phase constructor is private.
	 */
	void ConstructL();

private:
	CMMRCServerController& iMMRCServerController;
	};

#endif //__MMRCSERVERPOLICYMANAGER_H__

