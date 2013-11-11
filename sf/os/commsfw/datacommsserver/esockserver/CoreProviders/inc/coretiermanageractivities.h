/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* generic framework node state machine states
* THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
* 
*
*/



/**
 @file coretiermanageractivities.h
 @internalTechnology
*/

#ifndef SYMBIAN_CORETIERMANAGERACTIVITIES_H
#define SYMBIAN_CORETIERMANAGERACTIVITIES_H

#include <comms-infras/coretiermanager.h>
#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/coretiernotificationactivity.h>
#include <ss_select.h>


namespace TMClientLeaveActivity
{
	DECLARE_EXPORT_NODEACTIVITY(TMClientLeave)
}

namespace TMAvailabilityNotificationActivity_Forward
{
	DECLARE_EXPORT_NODEACTIVITY(AvailabilityActivity_Forward)
}

namespace TMActivities
{
DECLARE_EXPORT_ACTIVITY_MAP(coreTMActivitiesSuper)
DECLARE_EXPORT_ACTIVITY_MAP(coreTMActivities)

///////////////////////////////////////////////////////////////////////////////
//CSelectProviderActivity
class CSelectProviderActivity : public MeshMachine::CNodeParallelActivityBase
	{
	friend class ESock::TSelectionNotify<CSelectProviderActivity>;

public:
    IMPORT_C static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CSelectProviderActivity();
	IMPORT_C const ESock::TAnyFn& InterfaceVTable();

protected:
	CSelectProviderActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount)
	:	MeshMachine::CNodeParallelActivityBase(aActivitySig, aNode, aActivitiesCount)
		{
		}

private:
	//ISelectionNotify callbacks
	void SelectComplete(ESock::MProviderSelector* aSelector, ESock::CMetaConnectionProviderBase* aProvider);
	void StateChangeNotification(ESock::MProviderSelector* aSelector, TInt aStage, TInt aError);
	void SelectError(ESock::MProviderSelector* aSelector, TInt aError);

private:
	static const ESock::TAnyFn iInterfaceVTable[];

public:
	ESock::MProviderSelector* iSelector;
	};



} //namespace TMActivities

#endif
// SYMBIAN_CORETIERMANAGERACTIVITIES_H

