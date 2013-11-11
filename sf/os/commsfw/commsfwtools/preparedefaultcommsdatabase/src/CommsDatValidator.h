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
// Data validation rules for CommsDat data
// FILE CONTENTS :
// 1/  CCDValidator Base class
// 2/  Validator classes for particular records
// a) CCDValidatorProxiesRecord
// b) CCDValidatorLANServiceRecord
// c) CCDValidatorTierRecord
// d) CCDValidatorConnPref
// e) CCDValidatorIAPRecord
// VALIDATION
// CCDValidator and derived classes implement validation rules for elements in the database
// Not every element needs validation.  Database writers are trusted components
// Validation is only necessary when data is written to the database
// MAINTENANCE
// Rules may need modification when the CommsDat schema changes
// 
//

/**
 @file 
 @internalTechnology
*/


#if (!defined COMMSDATVALIDATOR_H)
#define       COMMSDATVALIDATOR_H

#include <cdbcols.h>

//forward declarations
//class CCDAPPrioritySelectionPolicyRecord;
//class CCDAccessPointRecord;

namespace CommsDatInternal
/*
@internalTechnology
*/
{


/**************************************************************************************************
 * 1/ VALIDATION BASE CLASSES 
 *************************************************************************************************/


NONSHARABLE_CLASS(CCDValidator) : public CBase
/*
Validates input data 

Only for use where it is vital the data is correct
Not all data entry is validated

@internalComponent
*/
    {
public :
    
    // Creates a validator class if there are validation rules for aElement
    static void ValidateL(CMDBElement* aElement,CMDBSessionImpl& aSession
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                          , TOperation aValidatorOperation = EModify
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                          );
            
    virtual ~CCDValidator();

    // Performs any housekeeping if necessary upon deletion of a record.
    // For example, applies the opposite rules to ValidateL().
    virtual void OnDeleteL();

    // Only ever created by the factory
    CCDValidator(CMDBElement* aElement,CMDBSessionImpl& aSession);
    

private:

    // Validates data using validation rules
    // Leaves if it finds any errors in the data being submitted
    virtual void DoValidateL();

    CCDValidator();

protected:
    
    //CMDBElement& iElement; // A reference to the element being validated
    
	// A pointer to the element being validated (the object DOESN'T 
	// own the pointer!!!!!!)
    CMDBElement* iElement; 
    
    CMDBSessionImpl& iSession;

    };



    
/**************************************************************************************************
 * 2/ VALIDATOR CLASSES FOR INDIVIDUAL ELEMENTS
 *************************************************************************************************/



/************************* 2 a) Validator for Proxy Table ******************************************
 */
NONSHARABLE_CLASS(CCDValidatorProxyRecord) : public CCDValidator
    {
public:
    
    virtual ~CCDValidatorProxyRecord()
    	{
    	delete iShadowElement;
    	}
    
    CCDValidatorProxyRecord(CMDBElement* aElement,CMDBSessionImpl& aSession):CCDValidator(aElement,aSession){}
    
protected:

    void DoValidateL();

private:
    //when only field validation is done the validator reads the whole record from the DB and acts as
    //originally a record validation would be called. This element points to the record read from the
    //dB.
    CMDBElement* iShadowElement;
    
	void SetShadowElementL();
    };


/************************* 2 b) Validator for LanService Table ******************************************
 */

NONSHARABLE_CLASS(CCDValidatorLANServiceRecord) : public CCDValidator
    {
public:
    
    virtual ~CCDValidatorLANServiceRecord()
    	{
    	delete iShadowElement;
    	}
    
    CCDValidatorLANServiceRecord(CMDBElement* aElement,CMDBSessionImpl& aSession):CCDValidator(aElement,aSession){}
    
protected:

    void DoValidateL();
    
private:
    //when only field validation is done the validator reads the whole record from the DB and acts as
    //originally a record validation would be called. This element points to the record read from the
    //dB.
    CMDBElement* iShadowElement;
    
	void SetShadowElementL();
    };



#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

 /************************* 2 c) Validator for Tier Table ******************************************
 */
NONSHARABLE_CLASS(CCDValidatorTierRecord) : public CCDValidator
    {
public:
    
    virtual ~CCDValidatorTierRecord()
    	{
    	delete iShadowElement;
    	}
    
    CCDValidatorTierRecord(CMDBElement* aElement,CMDBSessionImpl& aSession):CCDValidator(aElement,aSession){}
    
protected:

    void DoValidateL();

private:
    //when only field validation is done the validator reads the whole record from the DB and acts as
    //originally a record validation would be called. This element points to the record read from the
    //dB.
    CMDBElement* iShadowElement;
    
	void SetShadowElementL();
    };

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
 This class is used by the CCDValidatorConnPref to generate the IPProto level AccessPoints
 by mapping the ConnectionPreferences records to AccessPoint records.
*/
NONSHARABLE_CLASS(CIPProtoAPGenerator) : public CBase
    {
    public:
        CIPProtoAPGenerator(CCDAPPrioritySelectionPolicyRecord* aSelPolRec,
                            CMDBElement& aElement,  
                            CMDBSessionImpl& aSession,
                            TInt aLinkAPTagId)
            : iSession(aSession)
            , iElement(aElement)
            , iAPSelPolRec(aSelPolRec)
            , iLinkAPTagId(aLinkAPTagId)
            {}
                                       
        virtual void DoTheWorkL() = 0;
        
        virtual ~CIPProtoAPGenerator() 
            {
            delete iAPSelPolRec;
            }
    
    //protected to be able to access to these members from the derived classes.
    //There is no point to write accessors just to access these members form the 
    //dervied classes...
    protected:
        //void GenerateRecordName(TDes& aBuffer);
        
        
        CMDBSessionImpl& iSession;
        CMDBElement& iElement; // A reference to the element being validated                               CMDBSessionImpl& iSession
        CCDAPPrioritySelectionPolicyRecord* iAPSelPolRec; //the ownership is here...
        TInt iLinkAPTagId;
    };

NONSHARABLE_CLASS(CIPProtoAPModifier) : public CIPProtoAPGenerator
    {
    public:
        CIPProtoAPModifier(CCDAPPrioritySelectionPolicyRecord* aSelPolRec,
                           CMDBElement& aElement,  
                           CMDBSessionImpl& aSession,
                           TInt aLinkAPTagId)
            : CIPProtoAPGenerator(aSelPolRec, aElement, aSession, aLinkAPTagId)
            {}
                           
        virtual void DoTheWorkL();
    };

NONSHARABLE_CLASS(CIPProtoAPCreator) : public CIPProtoAPGenerator
    {
    public:
        CIPProtoAPCreator(CCDAPPrioritySelectionPolicyRecord* aSelPolRec,
                          CMDBElement& aElement,  
                          CMDBSessionImpl& aSession,
                          TInt aLinkAPTagId)
            : CIPProtoAPGenerator(aSelPolRec, aElement, aSession, aLinkAPTagId)
            {}
        
        virtual void DoTheWorkL();
    };

NONSHARABLE_CLASS(CIPProtoAPRemover) : public CIPProtoAPGenerator
    {
    public:
        CIPProtoAPRemover(CCDAPPrioritySelectionPolicyRecord* aSelPolRec,
                          CMDBElement& aElement,  
                          CMDBSessionImpl& aSession)
            : CIPProtoAPGenerator(aSelPolRec, aElement, aSession, 0)
            {}
        
        virtual void DoTheWorkL();
    };

#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

/************************* 2 d) Validator for Connection Preference Table ************************
*/    
NONSHARABLE_CLASS(CCDValidatorConnPref) : public CCDValidator
    {
public:

    virtual ~CCDValidatorConnPref()
        {
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        delete iIPProtoGenerator;
        delete iShadowElement;
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        }

    CCDValidatorConnPref(CMDBElement* aElement,CMDBSessionImpl& aSession
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                         ,TOperation aValidatorOperation
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                         )
        :CCDValidator(aElement,aSession)
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        , iOperation(aValidatorOperation)
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        {}
    
protected:

    void DoValidateL();
    
private:

    void ValidateBearerAndDirectionL(const TDesC& aServiceType, 
                                     TCommDbConnectionDirection aDirection, 
                                     TUint32 aBearerSet, 
                                     TCommDbDialogPref aDlgPref);
    
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CCDAPPrioritySelectionPolicyRecord* FindTheAPPrioritySelPolRecL(TInt aReferredIAPRecId);
    
    //This function returns with an IAP record number in the case if the defaultIAP
    //field is not filled in the ConnPref record.
    TInt FindIAPWithoutDefIAPL();
    
    void IPProtoAPGenerationOrModificationL();
    
    void SetShadowElementL();
    
    CIPProtoAPGenerator* iIPProtoGenerator;
    TOperation iOperation;
    //when only field validation is done the validator reads the whole record from the DB and acts as
    //originally a record validation would be called. This element points to the record read from the
    //dB.
    CMDBElement* iShadowElement;
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    };



/************************* 2 e) Validator for IAP Table ******************************************
*/  
  
NONSHARABLE_CLASS(CCDValidatorIAPRecord) : public CCDValidator
    {
public:
        
    virtual ~CCDValidatorIAPRecord()
    	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    	delete iShadowElement;
#endif
    	}

    CCDValidatorIAPRecord(CMDBElement* aElement,CMDBSessionImpl& aSession
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                         ,TOperation aValidatorOperation
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                         )
    	:CCDValidator(aElement,aSession)
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        , iOperation(aValidatorOperation)
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        {}

protected:

    void DoValidateL();
    
    void OnDeleteL();

private:

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	TOperation iOperation;
	
    //when only field validation is done the validator reads the whole record from the DB and acts as
    //originally a record validation would be called. This element points to the record read from the
    //dB.
    CMDBElement* iShadowElement;
    
    void InitialiseAccessPointRecordL(      CCDAccessPointRecord& lhs,
                                      const TDesC&                aRecordName,
                                            TInt                  aRecordId,
                                            TInt                  aTag,
                                            TUint32               aTier,
                                            TUint32               aMCpr,
                                            TUint32               aSelectionPolicy,
                                            TUint32               aCpr,
                                            TUint32               aCprConfig,
                                            TUint32               aSCpr,
                                            TUint32               aProtocol);
   

    CCDBearerTypeRecord* NewBearerTypeRecordL(CMDBSession& aSessionOwner, CCDIAPRecord& aIAPRecord);
    
    void DeleteAssociatedLinkLevelAPL();
    void AssociatedLinkLevelAPGenerationL();
    void SetShadowElementL();
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

    void ServiceAndBearerValidationL();    
    };

} //end namespace CommsDatInternal


       
#endif //COMMSDATMAPPER_H
        
















