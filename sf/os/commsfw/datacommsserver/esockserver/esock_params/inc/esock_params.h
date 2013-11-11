// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ESOCK_PARAMS_H
#define ESOCK_PARAMS_H

#include <e32base.h>
#include <e32std.h>	

#include <comms-infras/metadata.h>
#include <es_prot.h>

class TConnAPPref : public Meta::SMetaDataECom
/**
Connection preference to specify an access point to start. 

However, this preference cannot be used with RConnection::Start as a TConnPref. It
must be part of a TConnPrefList.

TConnAPPref can be added to the list to chose the top level access point. For the
KAfInet tier this type behaves in the same way as TConnSnapPref.

It is recommended that at most one TConnAPPref from each tier is added to the
TConnPrefList.

If no TConnAPPref is specified, then the default access point for the KAfInet
tier will be used for selection.

@see TConnSnapPref
@see TConnPrefList
@see RConnection::Start

@publishedAll
@released since 9.5
*/
	{
public:
	inline static TConnAPPref* NewL();
	inline static TConnAPPref* NewL(TUint aAP);
		
	enum
	{
		EUid = 0x10285E27,
		ETypeId = 2
	};

	inline void SetAP(TUint aAP);
	inline TInt GetAP();
	
protected:
	DATA_VTABLE
	
	TUint iAP;
	};


class TConnAutoStartPref : public Meta::SMetaDataECom
/** The connection preference requires that the connection waits until its selected access point is available
before starting. The start request does not complete until the connection is fully up.

Without this preference(e.g in the TConnPrefList), if the selected access point is unavailable due to contention or
lack of coverage, the RConnection::Start is errored immediately.

TConnAutoStartPref does not aid access point selection. When specified without any other preference in the
TConnPrefList, the default access point in the KAfInet tier will be selected.

@code
RConnection conn;
User::LeaveIfError(conn.Open(sockSvr));
CleanupClosePushL(conn);

TConnPrefList* prefs = TConnPrefList::NewL();
CleanupStack::PushL(prefs);
TConnAPPref* APPref = TConnAPPref::NewL(aPara->iAp);
CleanupStack::PushL(APPref);
TConnAutoStartPref autoPref = TConnAutoStartPref::NewL();
CleanupStack::PushL(autoPref);

prefs->AppendL(autoPref);
prefs->AppendL(APPref);

error = conn.Start(pref);
@endcode

@see RConnection::Start
@see TConnPrefList

@publishedAll
@released since 9.5
*/
	{
public:
	inline static TConnAutoStartPref* NewL();
	enum
	{
		EUid = 0x10285E27,
		ETypeId = 3
	};

protected:
	DATA_VTABLE
	};


#include <comms-infras/esock_params.inl>

class CESockParamaterFactory : public CBase
/** Connection generic parameter set factory.

@internalComponent
*/
	{
public:
	static Meta::SMetaDataECom* NewL(TAny* aConstructionParameters);
	};

#endif

