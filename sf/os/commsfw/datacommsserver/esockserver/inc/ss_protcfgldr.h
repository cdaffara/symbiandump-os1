// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/
#include <commsdattypesv1_1.h> // CommsDat
#include <ecom/ecom.h>
#include <comms-infras/ss_metaconnprov.h>

#ifndef SYMBIAN_SS_PROTCFGLDR_H
#define SYMBIAN_SS_PROTCFGLDR_H

namespace ESock {

const TUid KCProtocolConfigLoaderUid = {0x10282300};

/*
  @internalTechnology
 */
class CProtocolConfigLoader : public CBase
    {
public:
    /*
      Find protocol loader uid for protocol in commsdat load it, If not found load dummy loader
    */
    IMPORT_C static CProtocolConfigLoader* NewL(CommsDat::CMDBSession* aDbs, const TUid aProtCfgLdrUid);
    IMPORT_C static CProtocolConfigLoader* NewL(const TUid aProtCfgLdrUid);
    
    IMPORT_C virtual ~CProtocolConfigLoader();

    IMPORT_C static void ListImplementationsL(RImplInfoPtrArray& aImplInfoArray);
	
    /*
      Load the config for protocol into provisioning object and append object as extension to
      Access Point Config.
     */
    virtual void LoadConfigL(RMetaExtensionContainer &aApc, const TUid& aUid, const TUint aId) = 0;
    
protected:
    inline void SetDatabaseSession(CommsDat::CMDBSession* aDbs) { iDbs = aDbs; }
    inline CommsDat::CMDBSession* DatabaseSession() const { return iDbs; }
    inline void SetProtocolUid(const TUid aUid) { iProtocolUid = aUid; }
    inline TUid ProtocolUid() const { return iProtocolUid; }
    
private:
    TUid iDtor_ID_Key;
    CommsDat::CMDBSession* iDbs;
    TUid iProtocolUid;
    };


/*
  @internalTechnology
*/
template<typename ConfigClass>
class CGenericProtoCfgLoader: public CProtocolConfigLoader
	{
  public:
	static CGenericProtoCfgLoader* NewL();

	virtual void LoadConfigL(RMetaExtensionContainer& aApc, const TUid& aUid, const TUint aId);

  protected:
	CGenericProtoCfgLoader();
	};


template<typename ConfigClass>
inline CGenericProtoCfgLoader<ConfigClass>::CGenericProtoCfgLoader()
	{
	}


template<typename ConfigClass>
CGenericProtoCfgLoader<ConfigClass>* CGenericProtoCfgLoader<ConfigClass>::NewL()
	{
	return new(ELeave) CGenericProtoCfgLoader<ConfigClass>();
	}


template<typename ConfigClass>
void CGenericProtoCfgLoader<ConfigClass>::LoadConfigL(RMetaExtensionContainer& aApc,
        const TUid& /*aUid*/, const TUint aId)
	{
	CommsDat::CMDBSession* dbs = CommsDat::CMDBSession::NewL(KCDVersion1_2);
	CleanupStack::PushL(dbs);

	ConfigClass* config = ConfigClass::NewL();
	CleanupStack::PushL(config);

	TUint32 recordid = aId;
	config->InitialiseConfigL(*dbs, recordid);
	aApc.AppendExtensionL(config);

	CleanupStack::Pop(config);
	CleanupStack::PopAndDestroy(dbs);
	}

} // namespace ESock


#endif // SYMBIAN_SS_PROTCFGLDR_H

