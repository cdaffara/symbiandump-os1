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
//

#include "registeredsurfacemap.h"
#include "panics.h"
#include <graphics/wsscene.h>
#include <w32std.h>

/**
Destructor for the surface map, will unregister every surface in every client.
*/
CRegisteredSurfaceMap::~CRegisteredSurfaceMap()
	{
	TInt tempCount = iSessionSurfaces.Count();
	for (TInt ii = 0; ii < tempCount; ii++)
		{
		iSessionSurfaces[ii].RemoveAll(iScene);
		}
	iSessionSurfaces.Close();
	}

/**
Constructor for the surface map.
*/
CRegisteredSurfaceMap::CRegisteredSurfaceMap(MWsScene& aScene):iScene(aScene)
	{}

/**
Basic compare function, looking at the address of each session.
@param aLeft Left session for comparison.
@param aRight Right session for comparison.
@return Negative if left is smaller than right, positive if left is greater than right,
zero if they are the same.
*/
TInt CRegisteredSurfaceMap::CompareDeviceSurfaces (const TSessionSurfaces& aLeft, const TSessionSurfaces& aRight)
	{
	return (TInt)&aLeft.Session()-(TInt)&aRight.Session();
	}

/**
Function to register a surface on a client.  Will go through a process to add the surface to a list
for the client to keep track of which surfaces are registered for which clients.
@param aClient The client for which to register the surface.
@param aSurfaceId The surface to register on the client.
@return KErrNone on success or a system-wide error code.
@see CWsClient::RegisterSurface
*/
TInt CRegisteredSurfaceMap::Add(const CWsClient& aClient, const TSurfaceId& aSurfaceId)
	{
	TInt err;
	TInt tempPos;
	TSessionSurfaces tempDeviceSurface(aClient);
	
	//find client
	err = iSessionSurfaces.FindInOrder(tempDeviceSurface, tempPos, TLinearOrder<TSessionSurfaces>(CompareDeviceSurfaces));
	
	//if the client is found
	if (err == KErrNone)
		{
		err = iSessionSurfaces[tempPos].AddSurfaceId(aSurfaceId, iScene);
		}
	else
		{
		if (err == KErrNotFound)
			{
			err = iSessionSurfaces.Insert(tempDeviceSurface, tempPos);
			if (err == KErrNone)
				{
				err = iSessionSurfaces[tempPos].AddSurfaceId(aSurfaceId, iScene);
				if ((err != KErrNone) && (err != KErrInUse))	//it didnt manage to register the surface id
					{
					iSessionSurfaces.Remove(tempPos);
					}
				}
			}
		}
	return err;
	}

/**
Function to unregister a specific surface on a specific client.  Goes through a process to check
that the surface is registered for the client before remove it from the list and unregistering it.
@param aClient The client for which to unregister the surface.
@param aSurfaceId The surface to unregister on the client.
@return KErrNone on success or a system-wide error code.
@see CWsClient::UnregisterSurface
*/
TInt CRegisteredSurfaceMap::Remove(CWsClient& aClient, const TSurfaceId& aSurfaceId)
	{
	TInt err;
	TInt tempPos;
	TSessionSurfaces tempDeviceSurface(aClient);
	
	//find client
	err = iSessionSurfaces.FindInOrder(tempDeviceSurface, tempPos, TLinearOrder<TSessionSurfaces>(CompareDeviceSurfaces));
	TInt surfaceAmountLeftOrError;
	//if found
	if (err == KErrNone)
		{
		surfaceAmountLeftOrError = iSessionSurfaces[tempPos].RemoveSurfaceId(aSurfaceId, iScene);
		if (surfaceAmountLeftOrError == 0)	//there are no more surfaces registered for the client
			{
			iSessionSurfaces.Remove(tempPos);
			}
		if (surfaceAmountLeftOrError < 0)	//if an error was returned
			{
			err = surfaceAmountLeftOrError;
			}
		}
	return err;
	}

/**
Function that will unregister all surfaces for a specific client.
@param aClient The client for which to unregister all clients.
@return KErrNone on success or a system-wide error code.
@see CWsTop::ClearSurfaceMap
@see CWsClient::~CWsClient()
*/
TInt CRegisteredSurfaceMap::RemoveAll(CWsClient& aClient)
	{
	TInt err;
	TInt tempPos;
	TSessionSurfaces tempDeviceSurface(aClient);
	
	//find client
	err = iSessionSurfaces.FindInOrder(tempDeviceSurface, tempPos, TLinearOrder<TSessionSurfaces>(CompareDeviceSurfaces));
	
	//if found
	if (err == KErrNone)
		{
		err = iSessionSurfaces[tempPos].RemoveAll(iScene);
		}
	if (err == KErrNone || err == KErrInUse)
		{
		iSessionSurfaces.Remove(tempPos);
		}
	return err;
	}

TSessionSurfaces::TSessionSurfaces(const CWsClient& aSession):iSession(aSession)
	{}

TInt TSessionSurfaces::CompareIds(const TSurfaceId& aLeft, const TSurfaceId& aRight)
	{
	//Compare each internal id of the surface
	TUint32 ll;
	TUint32 rr;
	TInt32 tempResult;
	ll = aLeft.iInternal[0];
	rr = aRight.iInternal[0];
	tempResult = ll-rr;
	if (tempResult == 0)
		{
		ll = aLeft.iInternal[1];
		rr = aRight.iInternal[1];
		tempResult = ll-rr;
		if (tempResult == 0)
			{
			ll = aLeft.iInternal[2];
			rr = aRight.iInternal[2];
			tempResult = ll-rr;
			if (tempResult == 0)
				{
				ll = aLeft.iInternal[3];
				rr = aRight.iInternal[3];
				tempResult = ll-rr;
				}
			}
		}
	if (tempResult != 0)	//if they are different ids
		{
		if (ll < rr)
			{
			return -1;
			}
		else
			{
			return 1;
			}
		}
	return 0;
	}

TInt TSessionSurfaces::AddSurfaceId(const TSurfaceId& aSurfaceId, MWsScene& aScene)
	{
	TInt err;
	TInt tempPos;
	//find surface id
	err = iSurfaces.FindInOrder(aSurfaceId, tempPos, TLinearOrder<TSurfaceId>(CompareIds));
	
	if (err == KErrNotFound)
		{
		err = iSurfaces.Insert(aSurfaceId, tempPos);
		if (err == KErrNone)	//successfully added surface, can now register
			{
			err = aScene.RegisterSurface(aSurfaceId);
			}
		}
	else
		{
		if (err == KErrNone)
			{
			//shouldnt be registered more than once
			err = KErrInUse;
			}
		}
	return err;
	}

TInt TSessionSurfaces::RemoveSurfaceId(const TSurfaceId& aSurfaceId, MWsScene& aScene)
	{
	TInt surfaceAmountLeftOrError;
	TInt tempPos;

	//find surface id
	surfaceAmountLeftOrError = iSurfaces.FindInOrder(aSurfaceId, tempPos, 
			TLinearOrder<TSurfaceId>(CompareIds));
	if (surfaceAmountLeftOrError!=KErrNone)
		surfaceAmountLeftOrError = iSurfaces.FindInOrder(aSurfaceId, tempPos, 
				TLinearOrder<TSurfaceId>(CompareIds));
	//if found
	if (surfaceAmountLeftOrError == KErrNone)
		{
		
		surfaceAmountLeftOrError = aScene.UnregisterSurface(aSurfaceId);
		if ((surfaceAmountLeftOrError == KErrNone) || (surfaceAmountLeftOrError == KErrInUse))
			{
			iSurfaces.Remove(tempPos);
			surfaceAmountLeftOrError = iSurfaces.Count();
			}
		else
			{
			WS_ASSERT_DEBUG(EFalse,EWsPanicSurfaceMapError);	//Unexpected error
			}
		}
	return surfaceAmountLeftOrError;
	}

TInt TSessionSurfaces::RemoveAll(MWsScene& aScene)
	{
    TInt err = KErrNone;
    TInt returnValue = KErrNone;
    TInt tempSize = iSurfaces.Count();
    for (TInt ii = 0; ii < tempSize; ii++)
        {
        err = aScene.UnregisterSurface(iSurfaces[ii]);
        WS_ASSERT_ALWAYS((err == KErrNone) || (err == KErrInUse), EWsPanicSurfaceMapError);
        if (err != KErrNone && returnValue == KErrNone)
            {   //return first error code
            returnValue = err;
            }
        }
    iSurfaces.Close();
    return returnValue;
	}
