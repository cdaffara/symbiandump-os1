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
//

#if !(defined COMMDBCONNPREF_H)
#define COMMDBCONNPREF_H

#include <connpref.h>
#include <cdbcols.h>

class TCommDbConnPref : public TConnPref
/** Connection preferences which override the contents of the ConnectionPreferences
tables in CommsDat. The only mandatory field is IAP.

An Internet Access Point(IAP) represents a IP bound bearer and a set of parameters
on that bearer, which the device can use to make a connection to the Internet.

A list of available IAPs and Networks can be retrieved from CommsDat using
CommsDat::CCDIAPRecord and CCDNetworkRecord.

@code
RConnection conn;
User::LeaveIfError(conn.Open(sockSvr));
CleanupClosePushL(conn);

TCommDbConnPref prefs;
prefs.SetIapId(3);
TInt error = conn.Start(prefs);
@endcode

@see CommsDat::CCDNetworkRecord
@see CommsDat::CCDIAPRecord
@see TCommDbDialogPref
@see TCommDbConnectionDirection
@see TCommDbBearer
@see RConnection::Start

@publishedAll
@released since v7.0s
*/
	{
	friend class TCommDbMultiConnPref;
public:
	IMPORT_C TCommDbConnPref();
	IMPORT_C static TVersion Version();

	IMPORT_C void SetIapId(TUint32 aIapId);
	IMPORT_C TUint32 IapId() const;

	IMPORT_C void SetNetId(TUint32 aNetId);
	IMPORT_C TUint32 NetId() const;

	IMPORT_C void SetDialogPreference(TCommDbDialogPref aDialogPref);
	IMPORT_C TCommDbDialogPref DialogPreference() const;

	IMPORT_C void SetDirection(TCommDbConnectionDirection aDirection);
	IMPORT_C TCommDbConnectionDirection Direction() const;

	IMPORT_C void SetBearerSet(TUint32 aBearerSet);
	IMPORT_C TUint32 BearerSet() const;

	IMPORT_C virtual TInt Compare(const TCommDbConnPref& aPref) const;
	inline TBool operator==(const TCommDbConnPref& aPref) const;
	inline TBool operator!=(const TCommDbConnPref& aPref) const;

	inline static TCommDbConnPref& Cast(const TConnPref& aPref);

public:
    struct SCommDbConnPref
        {
        TUint32 iIapId;
        TUint32 iNetId;
        TCommDbDialogPref iDialogPref;
        TCommDbConnectionDirection iDirection;
        TUint32 iBearerSet;
        };
	inline SCommDbConnPref* PrefPtr() const;
    const static TInt8 KMajorVersionNumber = 8;
    const static TInt8 KMinorVersionNumber = 0;
    const static TInt16 KBuildVersionNumber = 1;
	};



class TCommDbMultiConnPref : public TConnPref
/** A set of TCommDbConnPrefs which the device will use to attempt to connect
to the internet. The preferences at index 1 will be used first, and if this fails
the preferences at index 2 will be used, and so on until connection attempts
are exhausted.

The index counts from 1. There must be as many connection preferences as there
are connection attempts.

@code
RConnection conn;
User::LeaveIfError(conn.Open(sockSvr));
CleanupClosePushL(conn);

TCommDbConnPref prefsA;
prefsA.SetIapId(3);
TCommDbConnPref prefsB;
prefsB.SetIapId(5);

TCommDbMultiConnPref prefs;
prefs.SetPreference(1, prefsA);
prefs.SetPreference(2, prefsB);
prefs.SetConnectionAttempts(2);

TInt error = conn.Start(prefs);
@endcode

@see TCommDbConnPref
@see RConnection::Start

@publishedAll
@released since v7.0s
*/
	{
public:
	IMPORT_C TCommDbMultiConnPref();
	IMPORT_C static TVersion Version();

	IMPORT_C TInt SetPreference(TInt aIndex, const TCommDbConnPref& aPref);
	IMPORT_C TInt GetPreference(TInt aIndex, TCommDbConnPref& aPref);

	IMPORT_C void SetConnectionAttempts(TInt aNumAttempts);
	IMPORT_C TInt ConnectionAttempts();

	inline static TCommDbMultiConnPref& Cast(const TConnPref& aPref);

public:
    const static TInt KMaxMultiConnPrefCount = 2;
    struct SCommDbMultiConnPref
        {
        TInt iNumAttempts;
        struct TCommDbConnPref::SCommDbConnPref iPrefs[KMaxMultiConnPrefCount];
        };
	inline struct SCommDbMultiConnPref* PrefPtr() const;
    const static TInt8 KMajorVersionNumber = 8;
    const static TInt8 KMinorVersionNumber = 0;
    const static TInt16 KBuildVersionNumber = 1;
	};

#include <commdbconnpref.inl>

#endif
// COMMDBCONNPREF_H
