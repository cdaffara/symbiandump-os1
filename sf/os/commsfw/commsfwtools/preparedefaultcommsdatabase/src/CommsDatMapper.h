// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mapping and data validation rules for CommsDat data
// MAPPING
// CCDMapper and derived classes implement mapping rules for elements in the database
// wherever elements have been deprecated
// There are Mapping rules for every supported deprecated data schema version translating 
// to the current data schema.
// Mapping of deprecated elements is necessary for every operation (read, write and delete)
// MAINTENANCE
// - Every rule will need renaming whenever new version of Comms Data format is adopted.
// - Many rules will need updating whenever new version of CommsData format is adopted
// - Rules for deprecated data version need to be deleted when data version not supported.
// 
//

/**
 @file 
 @internalTechnology
*/


#if (!defined COMMSDATMAPPER_H)
#define       COMMSDATMAPPER_H

#include <cdbcols.h>
#include "CommsDatMapperAndValidatorUtils.h"

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
// Forward declarations of mapped elements
namespace CommsDat
{
class CCDAccessPointRecord;
class CCDBearerTypeRecord;
class CCDIAPRecord;
class CCDTierRecord;
class CCDMCprRecord;
class CCDCprRecord;
class CCDSCprRecord;
class CCDSCprRecord;
class CCDIAPPrioritySelectionPolicyRecord;
}

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

namespace CommsDatInternal
/*
@internalTechnology
*/
{


/**************************************************************************************************
 * 1/ MAPPER BASE CLASS
 ******************************************************************************************/

NONSHARABLE_CLASS(CCDMapper) : public CMDBRecordBase
/*
Base class for mapping rules

Records and Fields can be mapped, but not Tables
Generic Records are never mapped (fields, records or tables)


NB Mapping class must only hold data that is a direct map of the original
Other data can be created or modified during a map but must not be held
in the mapper class as if it need not be directly loaded, deleted or stored 
via MMetaDatabase operations

0/  The mapper class is created via a factory if there is anything to map
    in the input element.

1/  First the mapper is constructed.  
Any changed data in the input element is copied into the mapper element.

2/  When storing, a mapper client is then prepared with PrepareToStoreL
which may call CMDBSessionImpl::MaybeCreateNodeL for mapper elements

3/  VCT - When deleting a mapper client will call MaybeDeleteNode
It may sometimes be necessary to do a custom operation on delete especially 
to delete a whole node if this has been created.
Often this function will be a no-op as the general algorithm
will handle normal deletion of mapped fields

4/  A mapper client can then use the mapped data directly during database operations

5/  When the client has finished the database operations it calls MapResults
    which transfers changed data from the map back to the original element following
    the individual mapper class's rules

6/  The mapper can now be deleted


@internalComponent
*/
    {
public :
    
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    // Creates a mapping class of the appropriate type
    static CCDMapper* MaybeCreateMapperLC(CMDBElement& aElement,CMDBSessionImpl& aSession, const TOperation& aOperation);
#endif
    
    virtual ~CCDMapper();
    
    // Constructs the mapper by finding the id of each field
    // Populates the mapper with data from the input element
    virtual void ConstructL();

    // Prepares the mapper for storage
    // will use data from the input element and may read the database too
    // Populates the mapper with data from the input element
    virtual void PrepareToStoreL();

    // Maps the data the result of the mapping after the database operation
    // into related element
    // Should be called after a database operation has been performed
    virtual void MapResultsL();
    
    // With this method some post mappings can be done after storing the mapper object.
    // By default it just returns but in the case of the IAPPrioritySelectionPolicy mapper
    // the IPProto layer generation happens here - as this requires the already mapped and 
    // stored APPrioritySelectionPolicy table.
    virtual void PostMappingsL();
    
    // With this method some pre mappings can be done before the actual mapping takes place.
    // Usually this method just returns however in the case of IAPPriotySelPol mapping this method
    // loads the original and yet unchanged mapped APPriotySelPol table. 
    virtual void PreMappingsL();
    
    // Return ETrue if there is a map for this element id
    // else return EFalse
    virtual TBool Mapped(TMDBElementId aElementId) =0;

    // Return ETrue if mapping is needed for this element
    // else return EFalse
    virtual TBool NeedsMapping(CMDBElement& aElement) =0;
    
      
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    // only ever constructed from the factory
    CCDMapper(TMDBElementId aElementId, CMDBElement& aElement,CMDBSessionImpl& aSession, const TOperation& aOperation);
#endif
    
private:

    CCDMapper();
        
protected:

    TInt FindLinkElementIdL(TInt aVal, TInt aTargetTable);

    TMDBElementId CheckIsSnapL(TMDBElementId aInputId);

protected:

    CMDBElement& iElement; // A reference to the element that needs mapping
       
    CMDBSessionImpl& iSession;
    
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    const TOperation iOperation;
#endif
    };





/**************************************************************************************************
 * 2/ MAPPER CLASSES FOR INDIVIDUAL RECORDS
 ****************************************************************************************/

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

/************************* 2 a) Mapper for GlobalSettings Table ************************************
 * KCDTIdGlobalSettingsRecord
 *
 * These 2 fields are now stored against the Default Tier record in the Tier table
 *      DefaultSnap     KCDTIdDefaultSnap    CCDGlobalSettingsRecord::iDefaultSnap
 *      PromptUser      KCDTIdPromptForSnap  CCDGlobalSettingsRecord::iPromptForSnap
 *
 * Corresponding fields in the Tier table (KCDTIdTierRecord) are
 *      DefaultSnap     KCDTIdDefaultAccessPoint  CCDTierRecord::iDefaultAccessPoint
 *	    PromptUser      KCDTIdPromptUser          CCDTierRecord::iPromptUser
 *
 * The mapping relies on the presence of the DefaultTier field in the GlobalSettings Table
 *      DefaultTier     KCDTIdDefaultTier    CCDGlobalSettingsRecord::iDefaultTier
 *
 * The DefaultTier must always be present as a real or default value in the GlobalSettings
 * Table.  A preface configuration file for Ced may be the best way to set this value.
 * 
 * If KCDTIdDefaultTier field is not present, these fields cannot be mapped
 */

NONSHARABLE_CLASS(CCDMapperGlobalSettingsRecord) : public CCDMapper
    {
public:
    
    void ConstructL();
    
    virtual TBool NeedsMapping(CMDBElement& aElement);
    virtual TBool Mapped(TMDBElementId aElementId);
    /*virtual*/ void PrepareToStoreL();
    
    void MapResultsL();
    
    DATA_VTABLE

    virtual ~CCDMapperGlobalSettingsRecord(){}

    CCDMapperGlobalSettingsRecord(CMDBElement& aElement,CMDBSessionImpl& aSession, const TOperation& aOperation);
    
private:
    
    void CreateMapL();

public:

    CMDBField<TBool>                        iPromptUser;          // Whether or not to prompt the user during selection.
	CMDBRecordLink<CCDAccessPointRecord>    iDefaultAccessPoint;  // Tier's default access point.

    };




/************************* 2 b) Mapper for IAPPrioritySelectionPolicy Table **************
 * KCDTIdIapPrioritySelectionPolicyRecord   CCDIAPPrioritySelectionPolicyRecord
 * 
 * The IAP Selection table is deprecated and completely replaced by the APSelectionTable,
 *
 * KCDTIdApPrioritySelectionPolicyRecord  CCDIAPPrioritySelectionPolicyRecord
 *
 * As IAPs map directly to APs, the IAP Selection Table that links to IAPs can map directly to 
 * an AP selecton Table that points to APs that each map to a single IAP
 * 
 * The mapping can be very simple because the record id, name, tag and so on can be identical
 */

NONSHARABLE_CLASS(CCDMapperIAPPrioritySelectionPolicyRecord) : public CCDMapper
    {
public:
    
    void ConstructL();

    virtual TBool NeedsMapping(CMDBElement& aElement);
    virtual TBool Mapped(TMDBElementId aElementId);
    /*virtual*/ void PrepareToStoreL();
    /*virtual*/ void PostMappingsL();
    /*virtual*/ void PreMappingsL();
    void MapResultsL();
   
    DATA_VTABLE

    virtual ~CCDMapperIAPPrioritySelectionPolicyRecord();

    CCDMapperIAPPrioritySelectionPolicyRecord(CMDBElement& aElement,CMDBSessionImpl& aSession, const TOperation& aOperation);
        
public:	
		CMDBRecordLink<CCDAccessPointRecord> iAp1;
		CMDBRecordLink<CCDAccessPointRecord> iAp2;
		CMDBRecordLink<CCDAccessPointRecord> iAp3;
		CMDBRecordLink<CCDAccessPointRecord> iAp4;
		CMDBRecordLink<CCDAccessPointRecord> iAp5;
		CMDBRecordLink<CCDAccessPointRecord> iAp6;
		CMDBRecordLink<CCDAccessPointRecord> iAp7;
		CMDBRecordLink<CCDAccessPointRecord> iAp8;
		CMDBRecordLink<CCDAccessPointRecord> iAp9;
		CMDBRecordLink<CCDAccessPointRecord> iAp10;
		CMDBRecordLink<CCDAccessPointRecord> iAp11;
		CMDBRecordLink<CCDAccessPointRecord> iAp12;
		CMDBRecordLink<CCDAccessPointRecord> iAp13;
		CMDBRecordLink<CCDAccessPointRecord> iAp14;
		CMDBRecordLink<CCDAccessPointRecord> iAp15;

		CMDBField<TUint32>  iApCount;
        CMDBField<TInt>     iRecordTag;
        CMDBField<TDesC>    iRecordName;
private:
	    /*
	     * this field here is needed for example when mapping an IAPPrioritySelPol table modification. In this 
	     * case we need to know what was the original AP link in the APPrioritySelPol recrod because when someone
	     * deletes an IAP record link form the IAPPriotitySelPol record we have to have some information that what
	     * is the mapped AP link in the APPrioritySelPol record to be able to modify the linked IPProto level AP.
	     */ 
	    
	    CMDBElement* origMappedDataInDB;
    
        //Checks that the AP record, given as an ElementId, is a Network, IPProto or Link level AP
        CommsDatMapperAndValidator::TAPTypes CheckLinkLevelAPL(TMDBElementId aElementId);
        
        void GenerateIPProtoAPL(CMDBRecordLink<CCDAccessPointRecord>*& aAPPriorityFieldToModify,
                                TInt aLinkNumber,
                                TBool aIsDefaultAP);
        
        void CheckIPProtoAPL(CMDBRecordLink<CCDAccessPointRecord>*& aAPPriorityField,
                             TInt aLinkNumber,
                             TBool aIsDefaultAP);
        
        void DeleteIPProtoAPL(CMDBRecordLink<CCDAccessPointRecord>*& aAPPriorityField);
        
        /* There is a little trick here. If there are 2 APPrioritySelPol records where
         * 1 link in each record is linking against the same IPProto AP then the situation 
         * can be the following:
         * APPrioritySelPolRec_1:
         * 		AP1 = IPProto_link_record1
         * 
         * APPrioritySelPolRec_2:
         * 		AP1 = IPProto_link_record1
         * 
         * 1 APPrioritySelPol record can be already deleted (by the MaybeDeleteNodeL call).
         * So when the program logic tries to find that how many reference does a given
         * IPProto AP have (IPProto_link_record1) then it finds that it's only 1 because the
         * other record from which it was linked the 2nd time is already deleted. That's why
         * we give for the search algorithm the original (already deleted) APPrioritySelPol record.
         */ 
        void DeleteIPProtoAPL(CMDBRecordLink<CCDAccessPointRecord>*& aAPPriorityField,
        									 CCDAPPrioritySelectionPolicyRecord* aTheAlreadyDeletedRecord);
        
        TInt GetTheLinkAPTagIdL(TMDBElementId aElementId);
        TInt GetTheLinkAPTagIdL(CCDIAPPrioritySelectionPolicyRecord* aOrigIAPPrioritySelPolRec,
                                TInt aLinkNumber);
        TInt GetTheLinkAPTagIdL(CMDBRecordLink<CCDIAPRecord>* aOrigIAPPrioritySelPolField/*,
                                                                           TInt aLinkNumber*/);
        
        TBool IsItTheDefaultAPPrioritySelPolRec(/*CCDAPPrioritySelectionPolicyRecord* aDefAPPrioritySelPolRec*/);
        
        TInt ForcedCountAPLinks();
};


/************************* 2 c) Mapper for AccessPoint Table ***************************************
 * KCDTIdAccessPointRecord      CCDAccessPointRecord
 *
 * The following fields are deprecated
 *   APGID                  CCDAccessPointRecord::iAccessPointGID
 * 
 * They map to new fields within the AccessPointRecord
 *   APTier                 CCDAccessPointRecord::iTier
 *   
 * The following fields have some deprecated usage which is mapped 
 * though the same field remains
 *   APSelectionPolicy      CCDAccessPointRecord::iSelectionPolicy
 * 
 * 
 */

NONSHARABLE_CLASS(CCDMapperAccessPointRecord) : public CCDMapper
    {
public:
    
    void ConstructL();
    
    void PrepareToStoreL();

    TBool NeedsMapping(CMDBElement& aElement);
    TBool Mapped(TMDBElementId aElementId);
    /*virtual*/ void PreMappingsL();
    void MapResultsL();
    
    /*virtual*/ void FinalPreparationsL();
    
    DATA_VTABLE

    virtual ~CCDMapperAccessPointRecord(){}

    CCDMapperAccessPointRecord(CMDBElement& aElement,CMDBSessionImpl& aSession, const TOperation& aOperation);
    
public:
	CMDBRecordLink<CCDTierRecord> iTier;		//Tier. The type of the access point. The tier this AP belongs to. Links to a record in the Tier table.
	CMDBRecordLink<CCDMCprRecord> iMCpr;		//MCPR to load for this AP.  Links to a record in the MCPR table.
	CMDBRecordLink<CCDCprRecord>  iCpr;			//CPR to load for this AP. Links to a record in the CPR table.
	CMDBRecordLink<CCDSCprRecord> iSCpr;		//SCPR to load for this AP. Links to a record in the SCPR table.
	CMDBRecordLink<CCDSCprRecord> iProtocol;	//CFP to load for this AP. Links to a record in teh CFProtocol table.
	CMDBField<TUint32>            iCprConfig;   //MCPR's config id. Value interpreted by an MCPR to configure the AP.
	CMDBField<TUint32>            iAppSID;		//Application SID. Marks this AP for use by single Application only
	CMDBField<TDesC>              iConfigAPIdList;   //List of additional SCprs (from ConfigAccessPoint table) 
	
	/**
	The selection policy which links to the APPrioritySelectionPolicy record. As this is a mapper object
	we can be sure that this AP will be a network level AP which has to have a valid APPrioritySelPol link.
	The problem is that the linked record is not inserted (will be generated when the IAPPrioritySelPol record
    will be inserted), we just know from the input record that what will be the elementID of it, that's why
    it's a numeric field and not a linked one.
	*/
    CMDBField<TUint32>            iSelectionPolicy;
    
    /* The mapper has to have its own TagId. If the input container doesn't
     * contain any this will be used to hold the generated TagId.
     */
    CMDBField<TInt>	 			  iRecordTag;
   };


#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY


} //end namespace CommsDatInternal


       
#endif //COMMSDATMAPPER_H
        
















