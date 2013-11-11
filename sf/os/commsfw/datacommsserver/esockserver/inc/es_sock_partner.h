// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(__ES_SOCK_PARTNER_H__)
#define __ES_SOCK_PARTNER_H__


#include <e32base.h>
#include <es_sock.h>


/**
@publishedPartner
@released
*/

const TUint KCOLConnection = 1;						// level for RConnection::Control()
const TUint KCOLProvider = 2;						// level for RConnection::Control()
const TUint KConnInternalOptionBit = 0x80000000;	// Must not be set for client requests
const TUint KConnWriteUserDataBit = 0x40000000;
const TUint KConnReadUserDataBit = 0x20000000;

/**
Level for RConnection::Control()

@publishedPartner
@deprecated in 8.1
@capability NetworkControl Restrict access to connection clients
@ref RConnection::Control()
*/
const TUint KCoEnumerateConnectionClients  =  1 | (KConnWriteUserDataBit | KConnReadUserDataBit);

/**
Level for RConnection::Control()
Information about client

@publishedPartner
@deprecated in 8.1
@capability NetworkControl Restrict access to connection client info
@ref RConnection::Control()
*/
const TUint KCoGetConnectionClientInfo     =  2 | (KConnWriteUserDataBit | KConnReadUserDataBit);

const TUint KCoEnumerateConnectionSockets  =  3 | (KConnWriteUserDataBit | KConnReadUserDataBit);

/**
Level for RConnection::Control()
Information about connected socket

@publishedPartner
@deprecated
@capability NetworkControl Restrict access to socket info on a connection
@ref RConnection::Control()
*/
const TUint KCoGetConnectionSocketInfo     =  4 | (KConnWriteUserDataBit | KConnReadUserDataBit);


/**
Setting only: enable processes to "clone" open this RConnection instance via a call to
RConnection::Open(..., TName&), as long as they conform to the security policy
passed as argument (specified as a TSecurityPolicyBuf).
@internalTechnology
*/
const TUint KCoEnableCloneOpen				= 5 | (KConnReadUserDataBit);

/**
Setting only: disable "clone" open of this RConnection instance, which was enabled via
a previous KCoEnableCloneOpen option.
@internalTechnology
*/
const TUint KCoDisableCloneOpen				= 6 | (KConnReadUserDataBit);

/**
Error code to force a KErrDisconnected from a layer in the stack if sent from an mcpr.
@publishedPartner
*/
const TInt KErrForceDisconnected = -17215;

class RParameterFamilyBundle;
class CSubConParameterBundle : public CObject
/** Container for (bundle of) SubConnection parameter families.

May contain and 0..N parameter families.

Note:
If the CSubConParameterBundle object takes ownership of any CSubConParameterFamily object, 
then when the bundle object is destroyed, any family owned by this object will also be 
destroyed.

THIS API IS DEPRECATED IN FAVOUR OF RParameterFamilyBundle

@publishedPartner
@deprecated since v9.6 */
	{
	friend class RSubConParameterBundle;
public:
	IMPORT_C static CSubConParameterBundle* NewL();
	IMPORT_C static CSubConParameterBundle* LoadL(TDesC8& aDes);
	
	IMPORT_C ~CSubConParameterBundle();

	IMPORT_C TUint Length() const;
	IMPORT_C TInt Load(const TDesC8& aDes);
	IMPORT_C TInt Store(TDes8& aDes) const;
	IMPORT_C void AddFamilyL(CSubConParameterFamily* aFamily);
	IMPORT_C CSubConParameterFamily* FindFamily(TUint32 aFamilyId);
	IMPORT_C CSubConParameterFamily* FindFamily(TUint32 aFamilyId) const;
	IMPORT_C void ClearAllParameters(CSubConParameterFamily::TParameterSetType aType);

protected:
	IMPORT_C CSubConParameterBundle();

private:
	CSubConParameterBundle(const CSubConParameterBundle& aBundle);
	CSubConParameterBundle& operator=(const CSubConParameterBundle& aBundle);

protected:
	RPointerArray<CSubConParameterFamily> iFamilies;
	};

#endif // __ES_SOCK_PARTNER_H__

