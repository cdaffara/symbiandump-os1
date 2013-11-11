/**
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
* base class for mesh-machine based ESock sub-sessions
* THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
* 
*
*/



/**
 @file
 @internalTechnology
*/

#ifndef _SS_MMNODE_SUBSESS_H_
#define _SS_MMNODE_SUBSESS_H_

#include <ss_std.h>
#include <comms-infras/ss_mmnode.h>

namespace ESock
{

//base class for mesh-machine based subsessions
class CMMSockSubSession : public CSockSubSession,
                          public ACFMMNodeIdBase
    {
protected:
    CMMSockSubSession(const MeshMachine::TNodeActivityMap& aActivityMap, CSockSession* aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId)
    :	CSockSubSession(aSession, aPlayer, aSubSessionUniqueId),
    	ACFMMNodeIdBase(aActivityMap)
    	{
    	}

	virtual void HandleMessageReturnValue(MeshMachine::TNodeContextBase& aContext);
	void ConstructL();

	virtual void CommsApiExtBindIfaceL(const RMessage2& aMessage);
	virtual void CommsApiExtIfaceSendReceiveL(const RMessage2& aMessage);
	virtual void CloseExtensionInterface(const RMessage2& aMessage);
	virtual void CancelAndCloseAllClientExtIfaces();

public:
	void ReceivedL(TUint16 aIpcId, const Messages::TSignatureBase& aCFMessage); //Very useful function not to have to construct a message on the stack
	virtual Messages::RNodeInterface* ServiceProvider() const
		{
		return ACFMMNodeIdBase::ServiceProvider();
		}
    };

} //namespace Esock


#endif
//_SS_MMNODE_SUBSESS_H_

