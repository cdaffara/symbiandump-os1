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

/**
 * @file CommsDatMapperAndValidatorUtils.h
 *
 * Definitions for internal use within CommsDat component for the Mapper and
 * Validator objects
 *
 * @internalTechnology
 * @released
 *
 *
 */
#ifndef COMMSDATMAPPERANDVALIDATORUTILS_H
#define COMMSDATMAPPERANDVALIDATORUTILS_H

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

#include <metadatabase.h>
#include <cdbcols.h>
#include <in_sock.h> //KAfInet
//forward declarations
namespace CommsDat
    {
    class CMDBSession;
    class CCDAPPrioritySelectionPolicyRecord;
    class CCDAccessPointRecord;
    
    template<class RECORD_TYPE>
    class CMDBRecordLink;
    
    
    //Possible operations which the validatior should handle
    typedef enum 
        {
        ELoad,
        ERefresh,
        EStore,
        EModify,    //ModifyL or StoreL method
        EDelete,    //DeleteL method
        EFind		//FindL method
        } TOperation;
    }

namespace CommsDatInternal
/*
@internalTechnology
*/
{
_LIT(KNetworkAPTemplateRecName,"NetworkDefaultTemplate");
_LIT(KIPProtoTemplateRecName,"IPProtoDefaultTemplate");
_LIT(KIPProtoTag, "IPProto_generated_");
_LIT(KDefaultIPProtoRecName, "IPProtoDefault");

const TInt MAX_BUFFER_LEN_FOR_NAMING = 10;
const TInt IPProtoBaseTagId = 10001;
//the maximum number of IAP links from the ConnPref table (1/record)
const TInt MaxNumberOfLinks = 2;
//the maximum number of AP/IAP links in an APPrioritySelPol/IAPPrioritySelPol record
const TInt MaxLinkCount = 15;

NONSHARABLE_CLASS(CommsDatMapperAndValidator)
/*
Contains definitions for the Validator and Mapper objects. The functions 
are not exported.

@internalTechnology
*/
    {
    public:
    
    enum
        {
        ENetworkLevelTierId = KAfInet,  // Network Tier Id : KAfInet=0x0800 
        EIPProtoTierId = 271064560,
        ELinkTierId = 271064565
        };
        
    typedef enum
        {
        EUndefined,
        ENetworkLevel,
        EIPProtoLevel,
        ELinkLevel
        } TAPTypes;
    
    typedef enum
        {
        EConnPref,
        EIAPPriorSelPol
        } TSelPolModLogic;
    
    static TInt GenerateTagIdL(TInt aBaseTagId, CommsDat::CMDBSession& aSession);
    static CommsDat::CCDAPPrioritySelectionPolicyRecord* FindTheAPPrioritySelPolRecL(CommsDat::CMDBSession& aSession);
    static CommsDat::CCDAccessPointRecord* GenerateIPProtoAPL(TInt aTagGenerationBase, 
                                                              TInt aLinkLevelAPTagId,
                                                              CommsDat::CMDBSession& aSession);
    static void GenerateRecordName(TDes& aBuffer, TInt aUniqueNumber);


    static void ModifyAPPrioritySelPolRecL(CommsDat::CMDBRecordLink<CommsDat::CCDAccessPointRecord>*& aFieldInRecord,
                                          CommsDat::CCDAPPrioritySelectionPolicyRecord*& aAPSelPolRec,
                                          TInt aAPElementId);

    static void ModifyDefaultTierRecordL(TInt aAPElementId, 
                                         CommsDat::CMDBSession& aSession);
    
    static TInt GetDefAPPrioritySelPolLinkValueL(CommsDat::CMDBSession& aSession);
    
    static TInt GetNetworkTierRecordIdL(CommsDat::CMDBSession& aSession);
    
    static TInt GetIPProtoTierRecordIdL(CommsDat::CMDBSession& aSession);

    static TBool IsIPProtoAPAlreadyExistL(TInt aLinkLevelTagId, CommsDat::CMDBSession& aSession);
    
    static TBool IsIPProtoAPAlreadyExistL(TInt aLinkLevelTagId, TInt aConnPrefElementId, CommsDat::CMDBSession& aSession);
    
    static inline CommsDat::CCDAccessPointRecord* LoadIPProtoAccessPointL(TInt aLinkLevelTagId, CommsDat::CMDBSession& aSession);
    static CommsDat::CCDAccessPointRecord* LoadIPProtoAccessPoint(TInt aLinkLevelTagId, CommsDat::CMDBSession& aSession);
    
    static TInt CountReferenceToThisIPProtoAPL(TUint aElementId, CommsDat::CMDBSession& aSession);
    
    static TInt CountReferenceToThisIPProtoAPL(TUint aElementId, 
    										   CommsDat::CCDAPPrioritySelectionPolicyRecord* aTheAlreadyDeletedRecord,
    										   CommsDat::CMDBSession& aSession);
    
    static TBool IsIAPRecInDB(CommsDat::CMDBSession& aSession);
    
    static TBool IsDefIPProtoAPInDB(CommsDat::CMDBSession& aSession);
    
    static CommsDat::CCDAccessPointRecord* GetTheDefIPProtoAP(CommsDat::CMDBSession& aSession);
    
    static TInt GetNumberOfThisIPProtoAPL(CommsDat::CCDAccessPointRecord* aAPRecToBeDeleted, CommsDat::CMDBSession& aSession);
    
    static void CorrectLinksL(TUint aElementIdToBeDeleted, CommsDat::CMDBSession& aSession);
    
    private:
    CommsDatMapperAndValidator();
    CommsDatMapperAndValidator(const CommsDatMapperAndValidator&);
    CommsDatMapperAndValidator& operator=(const CommsDatMapperAndValidator&);
    
    static void CheckTheAPPrioritySelPolFromConnPrefModL(CommsDat::CCDAPPrioritySelectionPolicyRecord*& aAPSelPolRec);
    static TBool FindIPProtoAccessPointRecordL(CommsDat::CCDAccessPointRecord& aAccessPoints, CommsDat::CMDBSession& aSession);
    
    }; //CommsDatMapperAndValidator
    
inline CommsDat::CCDAccessPointRecord* CommsDatMapperAndValidator::LoadIPProtoAccessPointL(TInt aLinkLevelTagId, CommsDat::CMDBSession& aSession)
    {
    CommsDat::CCDAccessPointRecord* ap = LoadIPProtoAccessPoint(aLinkLevelTagId, aSession);
    if (!ap)
        {
        User::Leave(KErrNotFound);
        }
    return ap;
    }

} //end of namespace CommsDatInternal

#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

#endif //COMMSDATMAPPERANDVALIDATORUTILS_H

