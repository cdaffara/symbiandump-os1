// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Core CPR Activities related to User Prompt Service (UPS)
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef SYMBIAN_UPSCPRACTIVITIES_H
#define SYMBIAN_UPSCPRACTIVITIES_H

#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/corecpr.h>

namespace UpsCprActivities
{
	
class CDeferredCtrlClientJoinActivity : public MeshMachine::CNodeParallelActivityBase
/**
Class used to implement a control client join activity that needs to be deferred until
after some authorisation has taken place.

Main purpose is to temporarily store the client id whilst a derived class performs
the authorisation.  Then, if all goes well, the control client is added and a
a join complete message sent to the originator(s).  Note that this class is not UPS
specific.
*/
	{
public:
	typedef MeshMachine::TNodeContext<ESock::CMMCommsProviderBase, PRStates::TContext, CDeferredCtrlClientJoinActivity> TContext;

    IMPORT_C static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

	void SetClient(const Messages::TNodeId& aCommsId);
	const Messages::TNodeId& Client();

	void SetFlags(TInt aFlags);
	TInt Flags();

	EXPORT_DECLARE_SMELEMENT_HEADER( TStoreControlClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		IMPORT_C virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TStoreControlClient )

	EXPORT_DECLARE_SMELEMENT_HEADER( TAddControlClientAndSendJoinComplete, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		IMPORT_C virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TAddControlClientAndSendJoinComplete )

protected:
    IMPORT_C CDeferredCtrlClientJoinActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount);
	IMPORT_C virtual ~CDeferredCtrlClientJoinActivity();

private:
	Messages::TNodeId iClient;		// control client to be joined
	TInt iFlags;					// join flags
	};

} // namespace UpsActivities

#endif //SYMBIAN_UPSCPRACTIVITIES_H

