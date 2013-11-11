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

#ifndef SYMBIAN_SS_COMMSPROV_H
#define SYMBIAN_SS_COMMSPROV_H

#include <comms-infras/metadata.h>
#include <comms-infras/ss_commsprov_internal.h>
#include <comms-infras/ss_rmetaextensioncontainer.h>

class CIpProtoProviderSelector;

namespace Messages
{
    class TSignatureBase;
}

//class CIpProtoProviderSelector;

namespace ESock
{

/**
Information used by provider factories to uniquely identify providers.
*/
class TProviderInfo
    {
public:
    inline TProviderInfo()
        : iAPId(0), iTierId(TUid::Uid(0)), iInstance(0)
        {
        }
    
    /**
    @param aTierId Tier on which the provider is created
    @param aAPId   Access point id of the provider
    @param aInstance  Unique information used to deferenciate providers with the same aAPId and aTierId (used for legacy selection)
    */
    inline TProviderInfo(TUid aTierId, TUint32 aAPId, TAny* aInstance = NULL)
        : iAPId(aAPId), iTierId(aTierId), iInstance(aInstance)
        {
        }

    /**
    Do a binary comparison of this against another instance of TProviderInfo
    @param aRHS The other TProviderInfo
    @return ETrue if the passed in TProviderInfo matches this.
    */
    inline TBool operator==(const TProviderInfo& aRHS) const
        {
        return Mem::Compare((const TUint8*)this,sizeof(*this),(const TUint8*)&aRHS,sizeof(*this))==0;
        }

    /**
    @return the tier id of the provider
    */
    inline TUid TierId() const {return iTierId;}

    /**
    @return the access point id of the provider
    */
    inline TUint APId() const {return iAPId;}
    inline TUid  APId(const TInt /*aDummy*/) const {return TUid::Uid(iAPId);}

    /**
    @return the instance information of the provider
    */
    inline TAny* Instance() const {return iInstance;}

    /**
    Set the instance information of the provider
    @param aInstance The information used to uniquely identify the instance.
    */
    inline void SetInstance(TAny* aInstance) {iInstance = aInstance;}

private:
    TUint iAPId;    //it has to be first so that it maps to TCommDbConnPref in case of legacy attach
                    //see class TAttachPrefs
    TUid iTierId;
    TAny* iInstance;

public:
    /**
    Pointer to the null provider info instance
    */
    static const TProviderInfo* const KNullProviderInfo;

    /**
    @return a pointer to the null provider info instance
    */
    IMPORT_C static const TProviderInfo& NullProviderInfo();
    };

/**
Information used by provider factories to uniquely identify providers.
*/
class TProviderInfoExt : public Meta::SMetaData
    {
public:
    enum { ETypeId = 0, EUid = 0x10286B59 };
    
    /**
    @param aProviderInfo TProviderInfo object to store in the extension
    */
    inline TProviderInfoExt(const TProviderInfo& aProviderInfo)
        : iProviderInfo(aProviderInfo)
        {
        }

public:
    EXPORT_DATA_VTABLE_AND_FN

public:
    const TProviderInfo& iProviderInfo;
    };

class CMetaConnectionProviderBase;


class TLayerConfig : public Meta::SMetaData
    {
public:
    enum { ETypeId = 0, EUid = 0x10286B58 };

    TLayerConfig()
        : iMCprUid(TUid::Uid(0)), iCprUid(TUid::Uid(0)), iSCprUid(TUid::Uid(0)),
        iProtocolUid(TUid::Uid(0)), iTierId(TUid::Uid(0))   
        {
        }
    
    TLayerConfig(TUid aMCprUid, TUid aCprUid, TUid aSCprUid, TUid aProtocolUid, TUid aTierId)
        : iMCprUid(aMCprUid), iCprUid(aCprUid), iSCprUid(aSCprUid),
        iProtocolUid(aProtocolUid), iTierId(aTierId)   
        {
        }

    /**
    @return the uid of the mcpr for the layer
    */
    inline TUid MCprUid() const
        {
        return iMCprUid;
        }
    
    /**
    @return the uid of the cpr for the layer
    */
    inline TUid CprUid() const
        {
        return iCprUid;
        }
    
    /**
    @return the uid of the scpr for the layer
    */
    inline TUid SCprUid() const
        {
        return iSCprUid;
        }
    
    /**
    @return the uid of the flow for the layer
    */
    inline TUid ProtocolUid() const
        {
        return iProtocolUid;
        }
    
    /**
    @return the uid of the tier this layer is in
    */
    inline TUid TierId() const
        {
        return iTierId;
        }

public:
    EXPORT_DATA_VTABLE_AND_FN

private:
    TUid iMCprUid;
    TUid iCprUid;
    TUid iSCprUid;
    TUid iProtocolUid;
    TUid iTierId;
    };


class TLayerSelectionInfo : public Meta::SMetaData
    {
    friend class ::CIpProtoProviderSelector;

public:
    enum { ETypeId = 0, EUid = 0x10286B57 };

    TLayerSelectionInfo()
        : iSelectionPolicy(0), iCprConfig(0), iCustomSelectionPolicy(0)
        {
        }
    
    TLayerSelectionInfo(TUint aSelectionPolicy, TUint aCprConfig, TUint aCustomSelectionPolicy)
        : iSelectionPolicy(aSelectionPolicy), iCprConfig(aCprConfig), iCustomSelectionPolicy(aCustomSelectionPolicy)
        {
        }
    
    /**
    @return the selection policy for the layer. How this is used is up to the layer.
    */
    inline TUint SelectionPolicy() const
        {
        return iSelectionPolicy;
        }
    
    /**
    @return the cpr configuration for the layer. How this is used is up to the layer. For ipproto and link layer this is equivalent to the IAP.
    */
    inline TUint CprConfig() const
        {
        return iCprConfig;
        }

    /** 
    @return the custom seleciton policy for this layer
    */
    inline TUint CustomSelectionPolicy() const
        {
        return iCustomSelectionPolicy;
        }

public:
    EXPORT_DATA_VTABLE_AND_FN

private:
    TUint iSelectionPolicy;
    TUint iCprConfig;
    TUint iCustomSelectionPolicy;   
    };


class TAccessPointPriority : public Meta::SMetaData
    {
    public:
        enum { ETypeId = 0, EUid = 0x10286B56 };

    TAccessPointPriority()
        {
        iPriority = KMaxTUint;
        }
        
    inline TUint Priority() const
        {
        return iPriority;
        }
    
    inline void SetPriority(TUint aPriority)
        {
        iPriority = aPriority;
        }
    
public:
    EXPORT_DATA_VTABLE_AND_FN

private:
    TUint iPriority;
    };


class TAppSidConfig : public Meta::SMetaData
    {
public:
    enum { ETypeId = 0, EUid = 0x10286B55 };

    explicit TAppSidConfig(TUint aAppSid)
        : iAppSid(aAppSid)
        {
        }
    
    /**
    @return the Application secure ID for this layer
    */
    inline TUint AppSid() const
        {
        return iAppSid;
        }

public:
    EXPORT_DATA_VTABLE_AND_FN

private:
    TUint iAppSid;
    };

} //namespace ESock

#endif
//SYMBIAN_SS_COMMSPROV_H

