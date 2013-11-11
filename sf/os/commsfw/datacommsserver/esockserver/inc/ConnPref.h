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

/**
 @file
 @publishedAll
 @released
*/

#if !(defined __CONNPREF_H__)
#define __CONNPREF_H__

#define TCommSnapPref TConnSnapPref
#include <e32std.h>
#include <comms-infras/metadata.h>
#include <comms-infras/metacontainer.h>

using namespace Meta;

/**
@publishedAll
@released since v7.0s
*/
const TUint KMaxConnPrefSize = 0x40;

/**
@internalTechnology
*/
struct SConnPref
	{
	TInt16 iExtensionId; //< Extension ID
	TInt16 iSpare;
	};

class TConnPref : public TBuf8<KMaxConnPrefSize>
/**
Overrides the connection preferences.
@publishedAll
@released since v7.0s
*/
	{
public:
	enum
		{
		EConnPrefUnknown = 0x00,     //< Unknown Connection preference
		EConnPrefCommDb = 0x01,      //< CommDb Connection preference
		EConnPrefCommDbMulti = 0x02, //< CommDbMulti Connection preferenc
		EConnPrefSnap = 0x03,
		EConnPrefIdList = 0x04,
		EConnPrefSip = 0x05,         //< Sip Connection preference
		EConnPrefProviderInfo = 0x06,
		EConnPrefEComList = 0x07
		};

	IMPORT_C TConnPref();
	IMPORT_C TConnPref(TInt aExtensionId);

	IMPORT_C TInt ExtensionId() const;
	IMPORT_C TInt GetUserLen();

protected:
	IMPORT_C void SetExtensionId(TInt aExtensionId);
	IMPORT_C void SetUserLen(TInt aLen);
	inline TUint8* UserPtr() const;

private:
	inline SConnPref* BasePtr() const;
	};

inline SConnPref* TConnPref::BasePtr() const
	{
	return (SConnPref*)iBuf;
	}

inline TUint8* TConnPref::UserPtr() const
	{
	return (TUint8*)(BasePtr()+1);
	}

class TConnSnapPref : public TConnPref
/**
An instance of the class is passed to RConnection::Start when user wants to start connection using
a Service Network Access Point. A Service Network Access Point is an access point in the KAfInet
tier.

A list of available access points can be retrieved from CommsDat using CCDAccessPointRecord. It is
recommended that users only attempt to start access points whose tier is KAfInet. Access points in
the KAfInet can use multiple access points on lower tiers to access the internet.

@code
RConnection conn;
User::LeaveIfError(conn.Open(sockSvr));
CleanupClosePushL(conn);

TCommSnapPref pref(52);

TInt error = conn.Start(pref);
@endcode

@see CommsDat::CCDAccessPointRecord
@see RConnection::Start
@see TConnAPPref

@publishedAll
@released since v9.1
*/
	{
public:
	IMPORT_C TConnSnapPref();
	IMPORT_C TConnSnapPref(TUint32 aSnap);

	IMPORT_C void SetSnap(TUint32 aSnap);
	IMPORT_C TUint32 Snap() const;
	};

class TConnPrefList : public TConnPref
/**
This class is used to send a collection of connection preferences expressed
as SMetaData objects between RConnection and Comms server which uses them to
construct it stack.

Each tier will look up the preferences it understands, act of them, consume them
if applicable (see individual preferences) and pass the rest down to the next
tier.

@code
RConnection conn;
User::LeaveIfError(conn.Open(sockSvr));
CleanupClosePushL(conn);

TConnPrefList* prefs = TConnPrefList::NewL();
CleanupStack::PushL(prefs);
TConnAPPref* APPref = TConnAPPref::NewL(aPara->iAp);
CleanupStack::PushL(APPref);
prefs->AppendL(APPref);

error = conn.Start(pref);
@endcode

@see TConnAPPref
@see TConnAutoStartPref
@see TConnSnapPref
@see RConnection::Start

@publishedAll
@released
*/
	{
public:
	IMPORT_C static TConnPrefList* NewL();
	IMPORT_C static TConnPrefList* LoadL(TDesC8& aDes);

	IMPORT_C TConnPrefList();
	IMPORT_C ~TConnPrefList();

	IMPORT_C TInt Length();
	IMPORT_C TInt Load(TDesC8& aDes);
	IMPORT_C TInt Store(TDes8& aDes);

	IMPORT_C void AppendL(SMetaDataECom* aFamily);
	IMPORT_C SMetaData* operator[](TInt aIndex);
	IMPORT_C void Remove(TInt aIndex);

	IMPORT_C TInt Count();

private:
	Meta::RMetaDataEComContainer iPrefs;
	};

#endif // __CONNPREF_H__

