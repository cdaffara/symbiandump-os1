// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef _SS_MMNODE_H_
#define _SS_MMNODE_H_

#include <comms-infras/ss_commsprov.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <elements/mm_node.h>

namespace PRStates
    {
    class TStoreProvision;
    }

namespace ESock
{
/**
Aggregation base class for mesh-machine based nodes specifically for networking
mesh of nodes
*/
class ACFMMNodeBase : public MeshMachine::AMMNodeBase
	{
protected:
	/**
	@param aActivityMap Map of activities this node will be able to perform
	@param aNodeId Id of this node
	*/
    explicit ACFMMNodeBase(const MeshMachine::TNodeActivityMap& aActivityMap, const Messages::TNodeId& aNodeId)
    :   MeshMachine::AMMNodeBase(aActivityMap, aNodeId)
        {
        }

    IMPORT_C void ConstructL(TInt aSize = MeshMachine::KUseDefaultPreallocatedSize); //When KUseDefaultPreallocatedSize, KDefaultMaxPreallocatedActivitySize will be used

	/**
	Fetch the interface control for a given interface id
	@param aInterfaceId Interface id to fetch control for.
	@return Pointer to the interface control
	*/
    IMPORT_C virtual NetInterfaces::TInterfaceControl* FetchNodeInterfaceControlL(TInt aInterfaceId);

public:

	/**
	The method returns the one and only service provider. The method accepts that 'this' can be in relation with
	many EServProviders (e.g.: like MCPRs are), but only one of them is conceptually the chosen (active) 
	EServProvider. The method will assume the chosen one is the (first) one marked with TCFClientType::EActive.
	*/
    Messages::RNodeInterface* ServiceProvider() const
    	{
		return GetFirstClient<Messages::TDefaultClientMatchPolicy>(Messages::TClientType(TCFClientType::EServProvider, TCFClientType::EActive),
			Messages::TClientType(TCFClientType::EServProvider, Messages::TClientType::ELeaving));
    	}

	/**
	Return the control provider. The control providers sits to the right of a node, and manages the lifetime of the node.

	@return pointer to the node interface for the control provider, or NULL if there is no control provider 
	*/
    Messages::RNodeInterface* ControlProvider() const
    	{
    	return GetFirstClient<Messages::TDefaultClientMatchPolicy>(Messages::TClientType(TCFClientType::ECtrlProvider));
    	}

protected:
	/**
	Handle a message that has been delivered to the node, but not picked up by any activity.
	@param aContext context in which the message was received
	 */
	IMPORT_C virtual void HandlePassThroughMessage(MeshMachine::TNodeContextBase& aContext);
	};

class ACFMMNodeIdBase : public Messages::ASimpleNodeIdBase, public ACFMMNodeBase
 	{
protected:
 	explicit ACFMMNodeIdBase(const MeshMachine::TNodeActivityMap& aActivityMap)
     :   ASimpleNodeIdBase(),
     	 ACFMMNodeBase(aActivityMap, Messages::ANodeId::Id())
         {
         }

     IMPORT_C virtual NetInterfaces::TInterfaceControl* FetchNodeInterfaceControlL(TInt aInterfaceId);

    //this needs to be here provisionally - subclasses complain about imbiguous access to ::Id()
public:
	/**
	@return the node id of this node
	*/
 	const Messages::TNodeId& Id() const
 		{
 		return NodeId();
 		}
 	};
} //namespace Esock


#endif
//_SS_NODEACTIVITIES_H_

