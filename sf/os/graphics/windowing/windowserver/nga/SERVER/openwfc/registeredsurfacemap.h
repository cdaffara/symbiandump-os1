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

#ifndef REGISTEREDSURFACEMAP_H
#define REGISTEREDSURFACEMAP_H

#include <e32base.h>
#include <graphics/surface.h>

class CWindowElementSet;
class CWsClient;
class TSessionSurfaces;
class MWsScene;

class CRegisteredSurfaceMap : public CBase
{
public:
	~CRegisteredSurfaceMap();
	CRegisteredSurfaceMap(MWsScene& aScene);
	TInt Add(const CWsClient& aClient, const TSurfaceId& aSurfaceId);
	TInt Remove(CWsClient& aClient, const TSurfaceId& aSurfaceId);
	TInt RemoveAll(CWsClient& aClient);
private:
	static TInt CompareDeviceSurfaces (const TSessionSurfaces& aLeft, const TSessionSurfaces& aRight);
private:
	RArray<TSessionSurfaces> iSessionSurfaces;
	MWsScene& iScene;
};

class TSessionSurfaces
{
private:
	static TInt CompareIds (const TSurfaceId& aLeft, const TSurfaceId& aRight);
public:
	TSessionSurfaces(const CWsClient& aSession);
	inline const CWsClient& Session() const {return iSession;}
	TInt AddSurfaceId(const TSurfaceId& aSurfaceId, MWsScene& aScene);
	TInt RemoveSurfaceId(const TSurfaceId& aSurfaceId, MWsScene& aScene);
	TInt RemoveAll(MWsScene& aScene);
private:	
	const CWsClient& iSession;
public:
	RArray<TSurfaceId> iSurfaces;
};

#endif // REGISTEREDSURFACEMAP_H
