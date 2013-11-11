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
// Core activity mutex policies
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_MM_MUTEXPOLICIES_H
#define SYMBIAN_MM_MUTEXPOLICIES_H

#include <elements/mm_context.h>

namespace MeshMachine
{

/**
Template to aggregate two mutexes with an logical AND operator

@code
typedef TAggregatedMutex_AND<THasDataClientsMutex, TNodeIsStartedMutex> THasDataClientsAndNodeIsStartedMutex;
@endcode

@param TMUTEX1 First mutex to test
@param TMUTEX2 Second mutex to test
*/
template <class TMUTEX1, class TMUTEX2>
class TAggregatedMutex_AND
	{
public:
	/**
	@param aContext context in which to evaluate the mutexes
	@return ETrue if both mutex conditions have been met, EFalse otherwise
	*/
    inline static TBool IsBlocked(const MeshMachine::TNodeContextBase& aContext)
    	{
#ifdef __GCCXML__
		return EFalse;
#else
		return TMUTEX1::IsBlocked(aContext) && TMUTEX2::IsBlocked(aContext);
#endif
    	}
	};

/**
Template to aggregate two mutexes with an logical OR operator

@code
typedef TAggregatedMutex_OR<THaveDataClientsMutex, THaveControlClientsMutex> THaveDataClientsOrControlClientsMutex;
@endcode

@param TMUTEX1 First mutex to test
@param TMUTEX2 Second mutex to test
*/
template <class TMUTEX1, class TMUTEX2>
class TAggregatedMutex_OR
	{
public:
	/**
	@param aContext context in which to evaluate the mutexes
	@return ETrue if operation should be blocked, EFalse if neither mutex condition has been met
	*/
    inline static TBool IsBlocked(const MeshMachine::TNodeContextBase& aContext)
    	{
#ifdef __GCCXML__
		return EFalse;
#else
		return TMUTEX1::IsBlocked(aContext) || TMUTEX2::IsBlocked(aContext);
#endif
    	}
	};


/**
Template to block if an activity is running
@param ACTIVITYID activity to test for
*/
template <TInt ACTIVITYID>
class TActivityIdMutex
	{
public:
	/**
	@param aContext context in which to evaluate the mutexes
	@return ETrue if operation should be blocked, EFalse if mutex condition has been met
	*/
    inline static TBool IsBlocked(const MeshMachine::TNodeContextBase& aContext)
    	{
		TInt count = aContext.iNode.CountActivities(ACTIVITYID);
		MESH_LOG((KMeshMachineSubTag, _L8("TActivityIdMutex: Node [%08x] Activity [%08x]: Number of actid (%d) = %d"), aContext.NodeId().Ptr(), aContext.iNodeActivity,
				 ACTIVITYID, count));

		return count != 0;
    	}
	};
	
/**
Template to block if there is an activity on a node in a non waiting state
@param ACTIVITYID id of activities to search for
 */
template <TInt ACTIVITYID>
class TNonWaitingActivityIdMutex
	{
public:
	/**
	@param aContext context in which to evaluate the mutexes
	@return ETrue if there exists activities with specified id, which are not in a waiting state.
	*/
    inline static TBool IsBlocked(const MeshMachine::TNodeContextBase& aContext)
    	{
		const RPointerArray<CNodeActivityBase>& activities = aContext.iNode.Activities();
		TInt i = activities.Count();
		while (i>0)
			{
			CNodeActivityBase* a = activities[--i];
			if (ACTIVITYID == a->ActivitySigId())
				{
				AActivitySemaphore* as = static_cast<AActivitySemaphore*>(a->FetchExtInterface(AActivitySemaphore::KInterfaceId));
				if (as && !as->IsWaiting())
					{
					return ETrue;
					}
				}
			}
		return EFalse;
    	}
	};

/**
Mutex checking multiple non waiting activities
@param ACTIVITYID1 First activity to check for
@param ACTIVITYID2 Second activity to check for
@param ACTIVITYID3 Third activity to check for, defaults to 0 in which case the parameter is ignored
@param ACTIVITYID4 Fourth activity to check for, defaults to 0 in which case the parameter is ignored
@param ACTIVITYID5 Fifth activity to check for, defaults to 0 in which case the parameter is ignored
*/
template <TInt ACTIVITYID1, TInt ACTIVITYID2, TInt ACTIVITYID3 = 0, TInt ACTIVITYID4 = 0, TInt ACTIVITYID5 = 0>
class TNonWaitingActivitiesIdMutex
	{
public:
	/**
	@param aContext context in which to evaluate the mutexes
	@return ETrue if there exists all the activities with the specified ids, which are not in a waiting state.
	*/
    inline static TBool IsBlocked(const MeshMachine::TNodeContextBase& aContext)
    	{
		const RPointerArray<CNodeActivityBase>& activities = aContext.iNode.Activities();
		TInt i = activities.Count();
		while (i>0)
			{
			CNodeActivityBase* a = activities[--i];
			TInt sigId = a->ActivitySigId();
			
			if (ACTIVITYID1 == sigId || ACTIVITYID2 == sigId || ACTIVITYID3 == sigId || ACTIVITYID4 == sigId || ACTIVITYID5 == sigId)
				{
				AActivitySemaphore* as = static_cast<AActivitySemaphore*>(a->FetchExtInterface(AActivitySemaphore::KInterfaceId));
				if (as && !as->IsWaiting())
					{
					return ETrue;
					}
				}
			}
		return EFalse;
    	}
	};

/**
Mutex checking multiple incompatible activities ids
@param ACTIVITYID1 First activity to check for
@param ACTIVITYID2 Second activity to check for
@param ACTIVITYID3 Third activity to check for, defaults to 0 in which case the parameter is ignored
@param ACTIVITYID4 Fourth activity to check for, defaults to 0 in which case the parameter is ignored
@param ACTIVITYID5 Fifth activity to check for, defaults to 0 in which case the parameter is ignored
*/
template <TInt ACTIVITYID1, TInt ACTIVITYID2, TInt ACTIVITYID3 = 0, TInt ACTIVITYID4 = 0, TInt ACTIVITYID5 = 0>
class TActivitiesIdMutex
	{
public:
	/**
	@param aContext context in which to evaluate the mutexes
	@return ETrue if there exists all the activities with the specified ids, EFalse otherwise
	*/
    inline static TBool IsBlocked(const MeshMachine::TNodeContextBase& aContext)
    	{

		MESH_LOG((KMeshMachineSubTag, _L8("TActivitiesIdMutex: Node [%08x] Activity [%08x]: [2] Number of actid (%d) = %d"), aContext.NodeId().Ptr(), aContext.iNodeActivity,
				ACTIVITYID2, aContext.iNode.CountActivities(ACTIVITYID2)));
		if (ACTIVITYID3)
			{
			MESH_LOG((KMeshMachineSubTag, _L8("TActivitiesIdMutex: Node [%08x] Activity [%08x]: [3] Number of actid (%d) = %d"), aContext.NodeId().Ptr(), aContext.iNodeActivity,
					ACTIVITYID3, aContext.iNode.CountActivities(ACTIVITYID3)));
			if (ACTIVITYID4)
				{
				MESH_LOG((KMeshMachineSubTag, _L8("TActivitiesIdMutex: Node [%08x] Activity [%08x]: [4] Number of actid (%d) = %d"), aContext.NodeId().Ptr(), aContext.iNodeActivity,
						ACTIVITYID4, aContext.iNode.CountActivities(ACTIVITYID4)));
				if (ACTIVITYID5)
					{
					MESH_LOG((KMeshMachineSubTag, _L8("TActivitiesIdMutex: Node [%08x] Activity [%08x]: [5] Number of actid (%d) = %d"), aContext.NodeId().Ptr(), aContext.iNodeActivity,
							ACTIVITYID5, aContext.iNode.CountActivities(ACTIVITYID5)));
					}
				}
			}
		TInt count = 0;
		count = aContext.iNode.CountActivities(ACTIVITYID1);
		MESH_LOG((KMeshMachineSubTag, _L8("TActivitiesIdMutex: Node [%08x] Activity [%08x]: [1] Number of actid (%d) = %d"), aContext.NodeId().Ptr(), aContext.iNodeActivity, ACTIVITYID1, count));
    	TBool isBlocked = (count != 0);

		count = aContext.iNode.CountActivities(ACTIVITYID2);
		MESH_LOG((KMeshMachineSubTag, _L8("TActivitiesIdMutex: Node [%08x] Activity [%08x]: [2] Number of actid (%d) = %d"), aContext.NodeId().Ptr(), aContext.iNodeActivity, ACTIVITYID2, count));
    	isBlocked |= (count != 0);
		
		if (ACTIVITYID3)
			{
			count = aContext.iNode.CountActivities(ACTIVITYID3);
			MESH_LOG((KMeshMachineSubTag, _L8("TActivitiesIdMutex: Node [%08x] Activity [%08x]: [3] Number of actid (%d) = %d"), aContext.NodeId().Ptr(), aContext.iNodeActivity, ACTIVITYID3, count));
			isBlocked |= (count != 0);

			if (ACTIVITYID4)
				{
				count = aContext.iNode.CountActivities(ACTIVITYID4);
				MESH_LOG((KMeshMachineSubTag, _L8("TActivitiesIdMutex: Node [%08x] Activity [%08x]: [4] Number of actid (%d) = %d"), aContext.NodeId().Ptr(), aContext.iNodeActivity, ACTIVITYID4, count));
				isBlocked |= (count != 0);

				if (ACTIVITYID5)
					{
					count = aContext.iNode.CountActivities(ACTIVITYID5);
					MESH_LOG((KMeshMachineSubTag, _L8("TActivitiesIdMutex: Node [%08x] Activity [%08x]: [5] Number of actid (%d) = %d"), aContext.NodeId().Ptr(), aContext.iNodeActivity, ACTIVITYID5, count));
					isBlocked |= (count != 0);
					}
				}
			}
		
		return isBlocked;
    	}
	};

/**
Mutex checking all activities
*/
class TAllActivitiesMutex
	{
public:
	/**
	@param aContext context in which to evaluate the mutexes
	@return ETrue if there are any activities running, EFalse otherwise
	*/
    inline static TBool IsBlocked(const MeshMachine::TNodeContextBase& aContext)
    	{
		TInt count = aContext.iNode.CountAllActivities();
		MESH_LOG((KMeshMachineSubTag, _L8("Node [%08x] Activity [%08x]: Count all activities %d"), aContext.NodeId().Ptr(), aContext.iNodeActivity, count));
    	return count != 0;
    	}
	};

/**
Mutex checking presence of a particular client
@param TMATCHPOLICY Match policy
@param TYPE Type to match against
@param FLAGS Flags to match against

@see ANodeBase::CountClients
*/
template <class TMATCHPOLICY, TInt TYPE, TInt FLAGS = 0>
class TClientMutex
	{
public:
	/**
	@param aContext context in which to evaluate the mutexes
	@return ETrue if a client exists which matches the criteria. EFalse otherwise
	*/
    inline static TBool IsBlocked(const MeshMachine::TNodeContextBase& aContext)
    	{
#ifdef __GCCXML__
		return EFalse;
#else
		TInt count = aContext.iNode.CountClients<TMATCHPOLICY>(Messages::TClientType(TYPE,FLAGS));
		MESH_LOG((KMeshMachineSubTag, _L8("TClientMutex Node [%08x] Activity [%08x]: Count Clients (%x, %x) = %d"), aContext.NodeId().Ptr(), aContext.iNodeActivity, TYPE, FLAGS, count));
		return count != 0;
#endif
    	}
	};

/**
Mutex checking lack of presence of a particular client
@param TMATCHPOLICY Match policy
@param TYPE Type to match against
@param FLAGS Flags to match against

@see ANodeBase::CountClients
*/
template <class TMATCHPOLICY, TInt TYPE, TInt FLAGS = 0>
class TNoClientMutex
	{
public:
	/**
	@param aContext context in which to evaluate the mutexes
	@return ETrue if a client does not exist which matches the criteria. EFalse otherwise
	*/
    inline static TBool IsBlocked(const MeshMachine::TNodeContextBase& aContext)
    	{
#ifdef __GCCXML__
		return EFalse;
#else
		TInt count = aContext.iNode.CountClients<TMATCHPOLICY>(Messages::TClientType(TYPE,FLAGS));
		MESH_LOG((KMeshMachineSubTag, _L8("TNoClientMutex Node [%08x] Activity [%08x]: Count Clients (%x, %x) = %d"), aContext.NodeId().Ptr(), aContext.iNodeActivity, TYPE, FLAGS, count));
		return count == 0;
#endif
    	}
	};

} //namespace MeshMachine

#endif
	//SYMBIAN_MM_MUTEXPOLICIES_H

