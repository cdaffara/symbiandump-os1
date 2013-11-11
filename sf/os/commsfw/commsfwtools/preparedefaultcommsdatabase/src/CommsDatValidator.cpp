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
// Validation rules for some comms database elements
// MAINTENANCE - Some rules may need adding or amending whenever the database schema changes
// No need to validate every entry  
// Mostly rely on compliance from writers as they are well trusted
// FILE CONTENTS :
// 1/  CCDValidator Base class implementation
// 2/  Validator classes for particular records
// a) CCDValidatorProxyRecord
// b) CCDValidatorLANServiceRecord
// c) CCDValidatorTierRecord
// d) CCDValidatorConnPref
// e) CCDValidatorIAPRecord
// All data in file
// 
//

/**
 @file CommsDatValidator.cpp
 @internalComponent
*/


#include "CommsDatInternalDefs.h"
#include <commsdattypesv1_1.h>
#include "CommsDatValidator.h"
#include <comms-infras/commsdatschema.h>
#include <commsdat.h>
#include "CommsDatMapperAndValidatorUtils.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypesv1_1_partner.h>
#include <commsdat_partner.h>
#endif

using namespace Meta;
using namespace CommsDatInternal;
using namespace CommsDat;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
//anonymous namespace
namespace
    {
    const TInt KNumServiceType = 12;
    
    struct TMapBearerToServiceType
    	{
    	TCommDbBearer				bearerMask;
    	TCommDbConnectionDirection	direction;
    	const TText*				serviceType;
    	};
    
    const TMapBearerToServiceType KMap[KNumServiceType] =
    	{
        	{KCommDbBearerCSD,		ECommDbConnectionDirectionOutgoing, DIAL_OUT_ISP},
        	{KCommDbBearerCSD,		ECommDbConnectionDirectionIncoming, DIAL_IN_ISP},
        	{KCommDbBearerWcdma,	ECommDbConnectionDirectionOutgoing, OUTGOING_GPRS},
        	 /** The following two mappings are only required for tornado */
        	{KCommDbBearerWcdma,	ECommDbConnectionDirectionIncoming, INCOMING_GPRS},
        	{KCommDbBearerLAN,		ECommDbConnectionDirectionIncoming,	LAN_SERVICE},
        	{KCommDbBearerLAN,		ECommDbConnectionDirectionOutgoing,	LAN_SERVICE},
        	{KCommDbBearerVirtual,	ECommDbConnectionDirectionIncoming,	VPN_SERVICE},
        	{KCommDbBearerVirtual,	ECommDbConnectionDirectionOutgoing,	VPN_SERVICE},
        	/** PAN profile uses LAN service table, plus an extension service table */
        	{KCommDbBearerPAN,		ECommDbConnectionDirectionIncoming,	LAN_SERVICE},
        	{KCommDbBearerPAN,		ECommDbConnectionDirectionOutgoing,	LAN_SERVICE},
        	/** WLAN uses LAN service table, plus an extension service table */
        	{KCommDbBearerWLAN,		ECommDbConnectionDirectionIncoming,	LAN_SERVICE},
        	{KCommDbBearerWLAN,		ECommDbConnectionDirectionOutgoing,	LAN_SERVICE}
    	};
        
        //this function returns the index in the array of the service type which should be used based
        //on the received bearer and direction parameters.
        //it returns KErrNotFound if the service type cannot be found based on the received params.
        TInt GetIAPServiceType(TUint32& aServTypeArrayStartIndex, TCommDbBearer aBearerSet, TCommDbConnectionDirection aDirection)
            {
            for (int i = aServTypeArrayStartIndex; i < KNumServiceType; i++)
        		{
        		if ((aBearerSet & KMap[i].bearerMask) != 0 &&
        			(aDirection == KMap[i].direction ||
        			 aDirection == ECommDbConnectionDirectionUnknown ))
        			{
        			aServTypeArrayStartIndex = i;
        			return i;
        			}
                }
            return KErrNotFound;
            }
        
        CMDBRecordLink<CCDAccessPointRecord>* GetTheAPxLinkFromAPPrioritySelPol(CCDAPPrioritySelectionPolicyRecord* aAPSelPolRec,
                                                                        TInt aRankingFromConnPref)
            {
            TMetaVTableIterator attribIter(aAPSelPolRec);
            SVDataTableEntry const* entry = 0;
            TInt fieldCounter(15);
            TInt linkNumber(aRankingFromConnPref);
            CMDBRecordLink<CCDAccessPointRecord>* apPrioritySelPolLink = NULL;
            
            while ( (entry = attribIter++) != NULL &&
                    linkNumber-- > 0 && 
                    fieldCounter-- > 0 )
                {
                apPrioritySelPolLink = reinterpret_cast<CMDBRecordLink<CCDAccessPointRecord>*>(aAPSelPolRec->GetAttribPtr(entry->iOffset));
                }
            
            return apPrioritySelPolLink;
            }
    }
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

/****************************************************************************************
 * 1/ CCDValidator   Base class implementaion
 ****************************************************************************************/

CCDValidator::CCDValidator(CMDBElement* aElement,CMDBSessionImpl& aSession):
iElement(aElement),iSession(aSession)
/*
@internalComponent
*/
    {
    iSession.iMapping = ETrue;
    }

CCDValidator::~CCDValidator()
/*
@internalComponent
*/
    {
    iSession.iMapping = EFalse;
    }
            

void CCDValidator::ValidateL(CMDBElement* aElement,CMDBSessionImpl& aSession
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                             ,TOperation aValidatorOperation
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                             )
/*
Creates a validation object of the appropriate type
if there are validation rules for aElement
and calls validate

@internalComponent
*/
	{
	
    if( aSession.iMapping )
        {
        return;
        }

    if( CommsDat::CommsDatSchema::IsTable(aElement->ElementId()) )
    	{
        return;
        }
    
    // then create a validator for the business data if there are any rules set
    CCDValidator* validator = NULL;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY        
    if (EModify == aValidatorOperation ||
    	EStore == aValidatorOperation)
    //this is the original behaviour of this method
        {
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        if (! CommsDat::CommsDatSchema::IsNode(aElement->ElementId()))
            {
            // First validate the field type (see CommsDatSchema)
            CommsDatSchema::ValidateFieldTypeL(aSession.iOwner, *aElement); 
            
            /* Only compliant processes can write to the database
            
              don't see it is valuable to validate every field*/

            //return;  
            }

        switch(aElement->ElementId() & KCDMaskShowRecordType) 
    		{
            case KCDTIdConnectionPrefsRecord:
                {
                validator = new(ELeave)CCDValidatorConnPref(aElement,aSession
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                , aValidatorOperation
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                ); 
                break;
                }

            case KCDTIdIAPRecord:
    		    {
                validator = new(ELeave)CCDValidatorIAPRecord(aElement,aSession
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                , aValidatorOperation
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                ); 
                break;
                }

            case KCDTIdLANServiceRecord:
    		    {
    			validator = new(ELeave)CCDValidatorLANServiceRecord(aElement,aSession); 
    			break;
    		    }

            case KCDTIdProxiesRecord:
    		    {
    			validator = new(ELeave)CCDValidatorProxyRecord(aElement,aSession); 
    			break;
    		    }

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
            case KCDTIdTierRecord:
    		    {
    			validator = new(ELeave)CCDValidatorTierRecord(aElement,aSession); 
    			break;
    		    }
    		
    #endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
            
            default :
                {
    			return; // Exit quickly if there's no validator
                }
            }
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        }
    else
    //the validator should be created for deleting some other data...
        {
        switch(aElement->ElementId() & KCDMaskShowRecordType) 
    		{
            case KCDTIdConnectionPrefsRecord:
                {
                validator = new(ELeave)CCDValidatorConnPref(aElement,aSession, aValidatorOperation); 
                break;
                }
            case KCDTIdIAPRecord:
    		    {
                validator = new(ELeave)CCDValidatorIAPRecord(aElement,aSession, aValidatorOperation); 
                break;
    		    }
            default :
                {
    			return; // Exit quickly if there's no validator
                }
    		}
        }
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

    CleanupStack::PushL(validator);
   
    validator->DoValidateL();

    CleanupStack::PopAndDestroy(validator);
	}

   
    
void CCDValidator::DoValidateL()
/*
Default is nothing to validate so return 

@internalComponent
*/
    {
    User::LeaveIfError(1 == 1);
    }
    

void CCDValidator::OnDeleteL()
/*
Default is nothing to delete;

@internalTechnology
*/
    {
    User::LeaveIfError(1 == 1);
    }





/***************************************************************************************    
 * 2/ VALIDATION CLASSES FOR INDIVIDUAL RECORDS
 **************************************************************************************/



/************************* 2a) Validate Proxy Record ************************************
Validate open link to service record
*/
void CCDValidatorProxyRecord::SetShadowElementL()
	{
	iShadowElement = CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord);
	iShadowElement->SetRecordId(iElement->RecordId());
	iShadowElement->LoadL(iSession.iOwner);
	
	if ( KCDMaskShowFieldType != (KCDMaskShowFieldType & iElement->ElementId()) )
		{
		//field validation
		CMDBElement* shadowFieldPtr = static_cast<CMDBRecordBase*>(iShadowElement)->GetFieldByIdL(iElement->ElementId());
		
		if ( KCDTIdRecordName == (iElement->ElementId() & KCDMaskShowFieldType) ||
			 KCDTIdProxyServiceType == (iElement->ElementId() & KCDMaskShowType) ||
			 KCDTIdServerName == (iElement->ElementId() & KCDMaskShowType) ||
			 KCDTIdProtocolName == (iElement->ElementId() & KCDMaskShowType) ||
			 KCDTIdExceptions == (iElement->ElementId() & KCDMaskShowType) )
			{
			CMDBField<TDesC>* nameField = static_cast<CMDBField<TDesC>*>(shadowFieldPtr);
			CMDBField<TDesC>* origField = static_cast<CMDBField<TDesC>*>(iElement);

			nameField->SetL(*origField);
			}
		else		
			{
			CMDBField<TInt>* numField = static_cast<CMDBField<TInt>*>(shadowFieldPtr);
			CMDBField<TInt>* origField = static_cast<CMDBField<TInt>*>(iElement);
			
			numField->SetL(*origField);
			}
		}
	
	iElement = iShadowElement;
	}


void CCDValidatorProxyRecord::DoValidateL()
/*
Just validate the linked field
@internalComponent
*/
    {
    if ( KCDMaskShowFieldType != (KCDMaskShowFieldType & iElement->ElementId()))
    	{
    	//field validation
    	SetShadowElementL(); 
    	}
    
#ifndef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
    CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>(iElement);

    if (! proxyRecord->iService.IsNull() && ! proxyRecord->iServiceType.IsNull())
        {
        TMDBElementId serviceTypeId = CommsDatSchemaV1_1::LookupTableId(proxyRecord->iServiceType.GetL());
        proxyRecord->iService = CommsDatSchema::GetLinkIdL(iSession.iOwner, proxyRecord->iService.ElementId(), proxyRecord->iService.GetL(), serviceTypeId/*, &proxyRecord*/);
        }
#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
    }


/************************* 2b) Validate LANService Record ***********************************
Validate open link to service extension record
*/

void CCDValidatorLANServiceRecord::SetShadowElementL()
	{
	iShadowElement = CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord);
	iShadowElement->SetRecordId(iElement->RecordId());
	iShadowElement->LoadL(iSession.iOwner);
	
	if ( KCDMaskShowFieldType != (KCDMaskShowFieldType & iElement->ElementId()) )
		{
		//field validation
		CMDBElement* shadowFieldPtr = static_cast<CMDBRecordBase*>(iShadowElement)->GetFieldByIdL(iElement->ElementId());
		
		if ( KCDTIdRecordTag == (iElement->ElementId() & KCDMaskShowFieldType) ||
			 KCDTIdLANIpAddrFromServer == (iElement->ElementId() & KCDMaskShowType) ||
			 KCDTIdLANIpDNSAddrFromServer == (iElement->ElementId() & KCDMaskShowType) ||
			 KCDTIdLANIp6DNSAddrFromServer == (iElement->ElementId() & KCDMaskShowType) ||
			 KCDTIdLANServiceExtensionTableRecordId == (iElement->ElementId() & KCDMaskShowType) )
			{
			CMDBField<TInt>* numField = static_cast<CMDBField<TInt>*>(shadowFieldPtr);
			CMDBField<TInt>* origField = static_cast<CMDBField<TInt>*>(iElement);
			
			numField->SetL(*origField);
			}
		else
			{
			CMDBField<TDesC>* nameField = static_cast<CMDBField<TDesC>*>(shadowFieldPtr);
			CMDBField<TDesC>* origField = static_cast<CMDBField<TDesC>*>(iElement);
			
			nameField->SetL(*origField);	
			}
		}
	
	iElement = iShadowElement;
	}

void CCDValidatorLANServiceRecord::DoValidateL()
/*
Just validate the linked field
@internalComponent
*/
	{
    if ( KCDMaskShowFieldType != (KCDMaskShowFieldType & iElement->ElementId()))
    	{
    	//field validation
    	SetShadowElementL(); 
    	}
    
#ifndef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
    CCDLANServiceRecord* lanServiceRecord = static_cast<CCDLANServiceRecord*>(iElement);

    if (! lanServiceRecord->iServiceExtensionTableRecordId.IsNull() && ! lanServiceRecord->iServiceExtensionTableName.IsNull())
        {
        //this was caught in testing but it is not valid to have a real record without these fields...
        TMDBElementId serviceExtTypeId = CommsDatSchemaV1_1::LookupTableId(lanServiceRecord->iServiceExtensionTableName.GetL());

        lanServiceRecord->iServiceExtensionTableRecordId = 
            CommsDatSchema::GetLinkIdL(iSession.iOwner, lanServiceRecord->iServiceExtensionTableRecordId.ElementId(), lanServiceRecord->iServiceExtensionTableRecordId.GetL(), serviceExtTypeId/*, &lanServiceRecord*/);    
        }
#endif
    }


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
// because tier record doesn't exist before then

/************************* 2c) Validate Tier Record *************************************
Tier thread name needs to be short name for Tier thread
*/

void CCDValidatorTierRecord::SetShadowElementL()
	{
	iShadowElement = CCDRecordBase::RecordFactoryL(KCDTIdTierRecord);
	iShadowElement->SetRecordId(iElement->RecordId());
	iShadowElement->LoadL(iSession.iOwner);
	
	if ( KCDMaskShowFieldType != (KCDMaskShowFieldType & iElement->ElementId()) )
		{
		//field validation
		CMDBElement* shadowFieldPtr = static_cast<CMDBRecordBase*>(iShadowElement)->GetFieldByIdL(iElement->ElementId());
		
		if ( KCDTIdRecordTag == (iElement->ElementId() & KCDMaskShowFieldType) ||
			 KCDTIdDefaultAccessPoint == (iElement->ElementId() & KCDMaskShowType) )
			{
			CMDBField<TInt>* numField = static_cast<CMDBField<TInt>*>(shadowFieldPtr);
			CMDBField<TInt>* origField = static_cast<CMDBField<TInt>*>(iElement);
			
			numField->SetL(*origField);
			}
		else
			{
			CMDBField<TDesC>* nameField = static_cast<CMDBField<TDesC>*>(shadowFieldPtr);
			CMDBField<TDesC>* origField = static_cast<CMDBField<TDesC>*>(iElement);
			
			nameField->SetL(*origField);	
			}
		}
	
	iElement = iShadowElement;
	}

void CCDValidatorTierRecord::DoValidateL()
/*
@internalComponent
*/	
    {
    if ( KCDMaskShowFieldType != (KCDMaskShowFieldType & iElement->ElementId()) )
    	{
    	//field validation
    	SetShadowElementL(); 
    	}
    
    CCDTierRecord* tierRec = static_cast<CCDTierRecord*>(iElement);
	if(static_cast<const TDesC16&>(tierRec->iTierThreadName).Length() > KShortTextLength)
		{
        __FLOG_STATIC(KLogComponent, KCDErrLog,
                            _L("CCDValidatorTierRecord::DoValidateL() - TierThreadName > KShortTextLength chars"));
		User::Leave(KErrTooBig);
		}
    }




/***************** 2d) Validate Connection Preference Record *****************************
*/
void CCDValidatorConnPref::SetShadowElementL()
	{	
	iShadowElement = CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
	iShadowElement->SetRecordId(iElement->RecordId());
	iShadowElement->LoadL(iSession.iOwner);
	
	if ( KCDMaskShowFieldType != (KCDMaskShowFieldType & iElement->ElementId()) )
		{
		//field validation
		CMDBElement* shadowFieldPtr = static_cast<CMDBRecordBase*>(iShadowElement)->GetFieldByIdL(iElement->ElementId());
		
	
		if (KCDTIdRecordName == (iElement->ElementId() & KCDMaskShowFieldType))
			{
			CMDBField<TDesC>* nameField = static_cast<CMDBField<TDesC>*>(shadowFieldPtr);
			CMDBField<TDesC>* origField = static_cast<CMDBField<TDesC>*>(iElement);
			
			nameField->SetL(*origField);		
			}
		else
			{
			//the only reason this cast can be done here is that the connPref record contains only
			//numerical based fields... (except the name field -> handled above...)
			CMDBField<TInt>* numField = static_cast<CMDBField<TInt>*>(shadowFieldPtr);
			CMDBField<TInt>* origField = static_cast<CMDBField<TInt>*>(iElement);
			
			if (EDelete == iOperation)
				{
				numField->SetL(0);
				}
			else
				{
				numField->SetL(*origField);
				}
				
			//in the case of the IAP link it has to be checked whether is it
			//given as a full link or only as a record number
			if (KCDTIdIAPLink == (iElement->ElementId() & KCDMaskShowType))
				{
				if ((*origField & KCDMaskShowType) == KCDMaskShowType)
					{
					//the value is only a record id
					numField->SetL(*origField);
					}
				else
					{
					//the value is a TMDBElementId. Now only the reocrdId is important -> the
					//connprefValidator is working only with the recId
					numField->SetL((*origField & KCDMaskShowRecordId) >> 8);
					}
				}
			}
		}
	
	iElement = iShadowElement;
	}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

void CCDValidatorConnPref::DoValidateL()
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	if ( KCDMaskShowFieldType != (KCDMaskShowFieldType & iElement->ElementId()) ||
    	 EDelete == iOperation )
    	{
    	// a) field validation -> in this case the validator should load the whole record from the DB
    	//    mimicing record validation
    	// b) this is a delete operation where only the record id is set on the record container
    	//    the validator should load the correct element in order to delete the 399 path
    	SetShadowElementL(); 
    	}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    
    TBool clearHiddenFlag = EFalse;
	
    CCDConnectionPrefsRecord* ptrConnPref = static_cast<CCDConnectionPrefsRecord*>(iElement);
	
	TInt rank = ptrConnPref->iRanking;
	TCommDbConnectionDirection direction = ptrConnPref->iDirection;
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	if (EDelete != iOperation)
	{
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		//this kind of validation is needed if there is some other operation than delete...
	    
		if(ptrConnPref->iDefaultIAP == 0)
			{
			/*
			Since phones get shipped without any predefined IAPs, when a connpref record
			is stored with the IAP(iDefaultIAP) not set, CED sets its value to 0.
			Such records hence should be exempted from validation.
			[Instead of a 0, the behaviour could have been a negative value to remove ambiguity and also
			could have been mandatory to set that value in the configuration file as well]		
			*/
		//	return; 
			}
		/* This test is here returniong KErrNotFound for BC reasons. 
	    It used to assume that value of iDefaultIAP would always be a straight record id
	    have now modified it to take account of different link values*/
	    if( ptrConnPref->iDefaultIAP != 0 &&
	        ptrConnPref->iDefaultIAP >= KCDMaxRecords )
	        {
	        if( (ptrConnPref->iDefaultIAP & KLinkableTag) != 0 ||
	            (ptrConnPref->iDefaultIAP & KCDMaskShowRecordType ) != KCDTIdIAPRecord )
	            {
	            // this can't be a valid IAP record
	            User::Leave(KErrNotFound);  
	            }
	        }
	
		if(rank > 2)
			{
			User::Leave(KErrOverflow); 
			}
	
		if (direction == ECommDbConnectionDirectionIncoming  &&  rank > 1)
			{
			User::Leave(KErrNotSupported);
			}
		//Re-setting the hidden flag in the case of a leave??????
	    clearHiddenFlag = ! iSession.IsSetAttributeMask(ECDHidden);
	    if (clearHiddenFlag)
	        {
	        iSession.SetAttributeMask(ECDHidden);
	        }
	
		//check if a record with the same rank already exists
		CCDConnectionPrefsRecord* ptrConnprefTestRecord = NULL;
	    
	    // secondly check records already in the db.
	
	    ptrConnprefTestRecord = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
		CleanupStack::PushL(ptrConnprefTestRecord);
	 
	    ptrConnprefTestRecord->iRanking = rank; 
		ptrConnprefTestRecord->iDirection = direction; 
	
		// conditions does not apply for rank 0
	   	if (rank > 0 && ptrConnprefTestRecord->FindL(iSession.iOwner))
			{
			// Since Validation can occur from ModifyL as well, only leave if this isn't the same record
			if (ptrConnprefTestRecord->RecordId() != ptrConnPref->RecordId() &&
	            ptrConnprefTestRecord->RecordId() != KCDDefaultRecord)
				{
			    User::Leave(KErrAlreadyExists);
				}
			}
		CleanupStack::PopAndDestroy(ptrConnprefTestRecord);
	
	    // Check the IAP record id.  look first in cache then in repository
	    if (ptrConnPref->iDefaultIAP != 0)
	        {
	        TInt boo = ptrConnPref->iDialogPref;
	    	TCommDbDialogPref dialogpref = (TCommDbDialogPref)boo;//TODO change the templated type def of this field;
	        
	
	    	if ( (*(ptrConnPref->iDefaultIAP.Data()) & KCDChangedFlag) == KCDChangedFlag ||
	    		 (*(ptrConnPref->iBearerSet.Data()) & KCDChangedFlag) == KCDChangedFlag ) 
	            {
	            CMDBField<TDesC>* iapServiceType = new(ELeave) CMDBField<TDesC>(KCDTIdIAPServiceType);
	    		CleanupStack::PushL(iapServiceType);
	            iapServiceType->SetMaxLengthL(KMaxTextLength);
	            iapServiceType->SetRecordId(ptrConnPref->iDefaultIAP);    
	            iapServiceType->LoadL(iSession.iOwner); 
	            TPtrC serviceType(*iapServiceType);
	        
	        	ValidateBearerAndDirectionL(serviceType, ptrConnPref->iDirection, ptrConnPref->iBearerSet, dialogpref);		
	    		CleanupStack::PopAndDestroy(iapServiceType);
	            }
	        }
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    	}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    if ( (ECommDbConnectionDirectionIncoming	!= direction &&		//  -|
          rank > 0 &&												//   | -> related to sore, modify
          !iSession.UsingLatestVersion()) )							//   | -> related to the latest session
        {
	    IPProtoAPGenerationOrModificationL();
        }
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    
    if (clearHiddenFlag)
        {
        iSession.ClearAttributeMask(ECDHidden);
        }
    }

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
CCDAPPrioritySelectionPolicyRecord* CCDValidatorConnPref::FindTheAPPrioritySelPolRecL(TInt aReferredIAPRecId)
    {
    //CCDConnectionPrefsRecord* ptrConnPref = static_cast<CCDConnectionPrefsRecord*>(&iElement);
    
    CCDAPPrioritySelectionPolicyRecord* defAPSelPolRec = NULL;
    
    //1st step - Search for the AP which has its tagID with the same value as the 
    //           recordNumber in the iDefaultIAP field in the ConnPref record or the calculated IAP number. 
    //           This AP will be the AP on the Link layer.
    
    //           This is a checking too to be sure that the AP is existing.
    CMDBField<TInt>* linkLayerAPTagIDField = new(ELeave) CMDBField<TInt>(KCDTIdAccessPointRecord | KCDTIdRecordTag);
    *linkLayerAPTagIDField = aReferredIAPRecId;
    CleanupStack::PushL(linkLayerAPTagIDField);
        
    if (EDelete == iOperation ||
        linkLayerAPTagIDField->FindL(iSession.iOwner))
        {
        //2nd step - Search for the default Tier in the GlobalSettings Record 
        //           (The Global Settings record has to be stored before inserting 
        //           the ConnectionPreferences record!!!!).
        CMDBRecordLink<CCDTierRecord>* defaultTierField = new(ELeave) CMDBRecordLink<CCDTierRecord>(KCDTIdGlobalSettingsRecord | KCDTIdDefaultTier);
        defaultTierField->SetRecordId(1);
        CleanupStack::PushL(defaultTierField);
        
        //this field HAS to be in the GlobalSettings table as we are in 399 config...
        defaultTierField->LoadL(iSession.iOwner);
        
        //3rd step - Follow the defaultAccessPoint link in the defaultTier record which points to
        //           the default AP record on the Network level.
        TInt recId = (*defaultTierField) << 8;
        CMDBRecordLink<CCDAccessPointRecord>* networkDefAPField = 
                        new(ELeave) CMDBRecordLink<CCDAccessPointRecord>(recId |
                                                                         KCDTIdDefaultAccessPoint);
        CleanupStack::PushL(networkDefAPField);
        
        networkDefAPField->LoadL(iSession.iOwner);
        
        //4th step - Follow the link from the default AP record to the linked APPrioritySelPol record.
        CMDBRecordLink<CCDAPPrioritySelectionPolicyRecord>* networkSelPolField = 
                        new(ELeave) CMDBRecordLink<CCDAPPrioritySelectionPolicyRecord>(((*networkDefAPField) & KCDMaskShowRecordId) |
                                                                                       KCDTIdSelectionPolicy);
        CleanupStack::PushL(networkSelPolField);
        
        networkSelPolField->LoadL(iSession.iOwner);
        
        //at this point we already know the complete elementId of the default APPrioritySelPol record.
        //Read the whole record because it will be needed by the IPProto AP generation logic...
        
        defAPSelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
                        
        CleanupStack::PushL(defAPSelPolRec);
        
        defAPSelPolRec->SetElementId(*networkSelPolField);
        defAPSelPolRec->LoadL(iSession.iOwner);
        
        //we will transfer the ownership of this object to the IPProtoGenerator object
        CleanupStack::Pop(defAPSelPolRec);
        CleanupStack::PopAndDestroy(networkSelPolField);
        CleanupStack::PopAndDestroy(networkDefAPField);
        CleanupStack::PopAndDestroy(defaultTierField);
        }
    else
    //The link layer AP is not existing. Leaving...
        {
        User::Leave(KErrNotFound);
        }
    
    CleanupStack::PopAndDestroy(linkLayerAPTagIDField);
    
    return defAPSelPolRec;
    }


void CCDValidatorConnPref::IPProtoAPGenerationOrModificationL()
    {
    CCDConnectionPrefsRecord* ptrConnPref = static_cast<CCDConnectionPrefsRecord*>(iElement);
    
    TInt referredIAPRecId = 0;
    
    if (EModify == iOperation ||
    	EStore == iOperation)
        {
        if (!(ptrConnPref->iDefaultIAP))
        /* There is no default IAP in the ConnPref record so this means that 
         * there is no default legacy path in the DB.
         *  
         * - If the connpref record is prompting:
         * 	 Insert an ipproto AP with a cprconfig field which contains the
         *   full TMDBElementID of the given connpref record
         * 
         * - If the connpref record is not prompting:
         * 	- If the rank1 record is the record which has the 0 as the IAP field 
         *    then:
         * 		- if there is IAP in the DB:
         * 			a default IAP has to be calculated
         * 		- if there isn't any IAP rec:
         * 			- if there is any default ipproto AP:
         * 				the Default ipproto AP should be used here as the 399 path.
         * 			- if there isn't any default ipproto AP:
         * 				one should be generated and linked
         * 
         * 	- If the rank2 record is modified like this then:
         * 		- if there is IAP in the DB:
         * 			a default IAP has to calculated
         * 		- if there isn't any IAP rec:
         * 			nothing can be done...
         */ 
            {
            if (1 == ptrConnPref->iRanking)
            	{
            	if (CommsDatMapperAndValidator::IsIAPRecInDB(iSession.iOwner))
            		{
            	    //there is IAP in the DB - > we can calculate a default one...
            		referredIAPRecId = FindIAPWithoutDefIAPL();
            	    }
            	else
            		{
            		//there is no IAP in the DB
		            CCDAPPrioritySelectionPolicyRecord* defAPSelPolRec = CommsDatMapperAndValidator::FindTheAPPrioritySelPolRecL(iSession.iOwner);
		            
		            CleanupStack::PushL(defAPSelPolRec);
		            
		            //Check whether the AP<ranking from the ConnPref rec> is already filled or not.
		            
		            // - If it's already filled this means that the IPProto level AP is already existing and
		            //   needs to be modified only.
		            
		            // - If it's not existing yet this means that a new one has to be generated and all the links
		            //   from or to the new record have to maintained.
		            CMDBRecordLink<CCDAccessPointRecord>* apPrioritySelPolLink = 
		                                                GetTheAPxLinkFromAPPrioritySelPol(defAPSelPolRec,
		                                                                                  ptrConnPref->iRanking);
		            
		            __ASSERT_DEBUG(apPrioritySelPolLink, User::Panic(KCommsDatName,EWrongAPPrioritySelPolRecord));
		            if (!apPrioritySelPolLink)
		                {
		                User::Leave(KErrArgument);
		                }
		            
		            CCDAccessPointRecord* defIPProtoAP = NULL;
		            
		            if (CommsDatMapperAndValidator::IsDefIPProtoAPInDB(iSession.iOwner))
		            	{
		            	//there is an AP with the customSelPol field CCDAccessPointRecord::KNoPolicy. Use that record.
		            	defIPProtoAP = CommsDatMapperAndValidator::GetTheDefIPProtoAP(iSession.iOwner);
		            	CleanupStack::PushL(defIPProtoAP);	            	
		            	}
		            else
		            	{
		            	//there isn't any default ipproto AP -> have to generate one
		            	defIPProtoAP = CommsDatMapperAndValidator::GenerateIPProtoAPL(IPProtoBaseTagId,
		    		                                                               	  CCDAccessPointRecord::KNoPolicy,
		    		                                                               	  iSession.iOwner);
		    	    	
		    		    CleanupStack::PushL(defIPProtoAP);
		    		    
		    		    defIPProtoAP->iRecordName.SetMaxLengthL(KDefaultIPProtoRecName().Length());
		    		    defIPProtoAP->iRecordName = KDefaultIPProtoRecName;
		    		    
		    		    //save the generated AP record
		    		    defIPProtoAP->StoreL(iSession.iOwner);
		            	}
		            
	        	    TMDBElementId elemId = defIPProtoAP->ElementId();
	        	    elemId &= KCDMaskHideAttrAndRes;
	        	    TMDBElementId tableAndRecordBitMask = KCDMaskShowRecordType | KCDMaskShowRecordId;
	        	    elemId &= tableAndRecordBitMask;
	        	    
	        	    //update the APPrioritySelPol record to point to the default IPProto AP record
	        	    CommsDatMapperAndValidator::ModifyAPPrioritySelPolRecL(apPrioritySelPolLink, 
	        	    													   defAPSelPolRec,
	        	                                                           elemId);
	            	
	        	    defAPSelPolRec->ModifyL(iSession.iOwner);
	        	    
	        	    //modify the ipproto tier
	    		    CMDBRecordLink<CCDAccessPointRecord>* ipProtoDefAccessPoint = new(ELeave)CMDBRecordLink<CCDAccessPointRecord>(KCDTIdDefaultAccessPoint);
	    		    CleanupStack::PushL(ipProtoDefAccessPoint);
	    		    
	    		    TInt ipProtoRecId = CommsDatMapperAndValidator::GetIPProtoTierRecordIdL(iSession.iOwner);
	    		    ipProtoDefAccessPoint->SetRecordId(ipProtoRecId);
	    		    
	    		    *ipProtoDefAccessPoint = elemId;
	    		    ipProtoDefAccessPoint->ModifyL(iSession.iOwner);
		            
	    		    if (KCDChangedFlag == (ptrConnPref->iDialogPref.ElementId() & KCDChangedFlag))
	    		    	{
	    		    	/* The dialogPref field has changed -> This means that:
	    		    	 *  a) we have to create a 'prompting AP' which means that the AP has to 
	    		    	 *     contain the full TMDBElement ID of the given connpref record in its' 
	    		    	 *     CprConfig field (prompting connpref rec).
	    		    	 *  b) the CprConfig field has to contain the IAP record number this 
	    		    	 *     IPProto AP points to (not prompting connpref rec).
	    		    	 */ 
	    		        TBool isPromptingAPNeeded;
	    		        
	    		        if (ECommDbDialogPrefUnknown    == ptrConnPref->iDialogPref ||
	    		            ECommDbDialogPrefDoNotPrompt== ptrConnPref->iDialogPref)
	    		            {
	    		            isPromptingAPNeeded = EFalse;
	    		            }
	    		        else
	    		            {
	    		            isPromptingAPNeeded = ETrue;
	    		            }
	    		    	
	    		        if (isPromptingAPNeeded)
	    		        	{
	    		        	//the cprConfig has to contain the full TMDBElementId of the connpref record
	    		        	defIPProtoAP->iCprConfig = ptrConnPref->ElementId();
	    		        	}
	    		        else
	    		        	{
	    		        	/* the cprConfig field has to contain the IAP record id
	    		        	 * Because this mapping/validation is done from a legacy configuration
	    		        	 * commsdat can assume here that there will be only 3 layers.
	    		        	 * 
	    		        	 * The following line says that the IAP pointed by the cprConfig and the
	    		        	 * Link level AP pointed by the customSelectionPolicy field are the same.
	    		        	 */
	    		        	defIPProtoAP->iCprConfig.SetL(ptrConnPref->iDefaultIAP);
	    		        	}
	    		        
	    		        defIPProtoAP->ModifyL(iSession.iOwner);
	    		    	}
	    		    
	    		    CleanupStack::PopAndDestroy(ipProtoDefAccessPoint);
		            CleanupStack::PopAndDestroy(defIPProtoAP);
		            CleanupStack::PopAndDestroy(defAPSelPolRec);
		            
		            return;
            		}
            	}
            else
            	{
            	//rank2 record is modified
            	if (CommsDatMapperAndValidator::IsIAPRecInDB(iSession.iOwner))
            		{
            		//there is IAP in the DB - > we can calculate a default one...
            		referredIAPRecId = FindIAPWithoutDefIAPL();
            		}
            	else
            		{
            		/* - there is no IAP in the DB
            		 * - somebody is inserting a ranking2 connpref record
            		 * 
            		 * Is this a normal case????? How should we handle it?
            		 */
            		return;
            		}
            	}
            }
        else
        //There is a default IAP in the record so just read it...
        //NOTE that this piece of code here assumes that the IAP in the ConnPref record is only a 
        //     number and _NOT_ a fully qualified link!!!
            {
            referredIAPRecId = ptrConnPref->iDefaultIAP.GetL();
            }
        }    
    
    //1st step - Search for the AP which has its tagID with the same value as the 
    //           recordNumber in the iDefaultIAP field in the ConnPref record or the calculated IAP number. 
    //           This AP will be the AP on the Link layer.
    
    //           This is a checking too to be sure that the AP is existing.
    CMDBField<TInt>* linkLayerAPTagIDField = new(ELeave) CMDBField<TInt>(KCDTIdAccessPointRecord | KCDTIdRecordTag);
    *linkLayerAPTagIDField = referredIAPRecId;
    CleanupStack::PushL(linkLayerAPTagIDField);
    
    TBool foundLinkLayerAP = linkLayerAPTagIDField->FindL(iSession.iOwner);
    
    if (EDelete == iOperation ||
    	foundLinkLayerAP )
        {
        //CCDAPPrioritySelectionPolicyRecord* defAPSelPolRec = FindTheAPPrioritySelPolRecL(referredIAPRecId);
        
        CCDAPPrioritySelectionPolicyRecord* defAPSelPolRec = CommsDatMapperAndValidator::FindTheAPPrioritySelPolRecL(iSession.iOwner);
        
        CleanupStack::PushL(defAPSelPolRec);
        
        //Check whether the AP<ranking from the ConnPref rec> is already filled or not.
        
        // - If it's already filled this means that the IPProto level AP is already existing and
        //   needs to be modified only.
        
        // - If it's not existing yet this means that a new one has to be generated and all the links
        //   from or to the new record have to maintained.
        CMDBRecordLink<CCDAccessPointRecord>* apPrioritySelPolLink = 
                                            GetTheAPxLinkFromAPPrioritySelPol(defAPSelPolRec,
                                                                              ptrConnPref->iRanking);
        
        __ASSERT_DEBUG(apPrioritySelPolLink, User::Panic(KCommsDatName,EWrongAPPrioritySelPolRecord));
        if (!apPrioritySelPolLink)
            {
            User::Leave(KErrArgument);
            }
        
        if (*apPrioritySelPolLink)
        //the AP<x> field has been already filled so the IPProto level AP is already existing. Only 
        //modification is needed.
            {
            if (EModify == iOperation ||
            	EStore == iOperation)
                {
                iIPProtoGenerator = new(ELeave) CIPProtoAPModifier(defAPSelPolRec,
                                                                   *iElement,  
                                                                   iSession,
                                                                   referredIAPRecId);
                
                //we transferred the ownership of this object to the IPProtoGenerator object
                CleanupStack::Pop(defAPSelPolRec);
                }
            else
            //The pointed AP should be deleted...
                {
                iIPProtoGenerator = new(ELeave) CIPProtoAPRemover(defAPSelPolRec,
                                                                  *iElement,  
                                                                  iSession);
                
                //we transferred the ownership of this object to the IPProtoGenerator object
                CleanupStack::Pop(defAPSelPolRec);
                }
            }
        else
        //the AP<x> field has not been filled yet so creation is needed.
            {
            if (EModify == iOperation ||
            	EStore == iOperation)
                {
                iIPProtoGenerator = new(ELeave) CIPProtoAPCreator(defAPSelPolRec,
                                                                  *iElement,  
                                                                  iSession,
                                                                  referredIAPRecId);
                
                //we transferred the ownership of this object to the IPProtoGenerator object
                CleanupStack::Pop(defAPSelPolRec);
                }
            else
            //hm... This is a delete operation and the AP<x> field is not filled in the
            //APPrioritySelPol record. Database is inconsistent - would be better to leave??
                {
                CleanupStack::PopAndDestroy(defAPSelPolRec);
                CleanupStack::PopAndDestroy(linkLayerAPTagIDField);
                return;
                }
            }
        
        //Modify or generate the IPProto level AP.
        iIPProtoGenerator->DoTheWorkL();
        }
    else
    //The link layer AP is not existing. Leaving...
        {
        User::Leave(KErrNotFound);
        }
    
    CleanupStack::PopAndDestroy(linkLayerAPTagIDField);
    }
    

/**
NOTE: This function is a modified copy from the void CCommsDbTableView::ConstructL(TUint32 aBearerSet,	TCommDbConnectionDirection aDirection)
      function (in the commdb/commdbshim/scdb/cdbtable.cpp)!!
*/
TInt CCDValidatorConnPref::FindIAPWithoutDefIAPL()
	{
    CCDConnectionPrefsRecord* ptrConnPref = static_cast<CCDConnectionPrefsRecord*>(iElement);
    
	CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(iapRecord);
	
    CMDBField<TCommDbConnectionDirection>& direction = ptrConnPref->iDirection;
	CMDBField<TUint32>& bearerSet = ptrConnPref->iBearerSet;
	
	TUint32 tempBearer = static_cast<TUint32>(bearerSet);
	TCommDbConnectionDirection tempDirection = static_cast<TCommDbConnectionDirection>(direction);
	
	TBool defIAPFound = EFalse;
	
    TUint32 servTypeArrayStartIndex = 0;
	
    while ((!defIAPFound) && ((TInt)servTypeArrayStartIndex < KNumServiceType))
    	{
	    TInt serviceTypeIndex = GetIAPServiceType( servTypeArrayStartIndex, (TCommDbBearer)tempBearer, tempDirection );
	    __ASSERT_DEBUG(serviceTypeIndex!=KErrNotFound, User::Panic(KCommsDatName,EWrongIAPServiceType));
	    if (KErrNotFound == serviceTypeIndex)
	        {
	        User::LeaveIfError(serviceTypeIndex);
	        }
	    
		const TPtrC serviceType(KMap[serviceTypeIndex].serviceType);
		
		// prime the service type
		iapRecord->iServiceType.SetMaxLengthL(serviceType.Length());
		iapRecord->iServiceType = serviceType;
		
		defIAPFound = iapRecord->FindL(iSession.iOwner);
		if (!defIAPFound)
		    {
		    //No IAP record can be found with the given service
		    ++servTypeArrayStartIndex;
		    }
    	}
	
	if (!defIAPFound)
	    {
	    //No IAP record can be found with the given service(s)
	    User::Leave(KErrNotFound);
	    }
	
	
	TInt tempRecId = iapRecord->RecordId();
	
	CleanupStack::PopAndDestroy(iapRecord);
	
    return tempRecId;
	}
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

void CCDValidatorConnPref::ValidateBearerAndDirectionL(const TDesC& aServiceType, TCommDbConnectionDirection aDirection, TUint32 aBearerSet, TCommDbDialogPref aDlgPref)
	{
	TCommDbConnectionDirection expectedDirection = ECommDbConnectionDirectionUnknown;
	TInt expectedBearer = KCommDbBearerUnknown;

	if (aServiceType == TPtrC(DIAL_OUT_ISP))
		{
		expectedDirection = ECommDbConnectionDirectionOutgoing;
		expectedBearer = KCommDbBearerCSD;
		}
	else if (aServiceType == TPtrC(DIAL_IN_ISP))
		{
		expectedDirection = ECommDbConnectionDirectionIncoming;
		expectedBearer = KCommDbBearerCSD;
		}
	else if (aServiceType == TPtrC(OUTGOING_GPRS))
		{
		expectedDirection = ECommDbConnectionDirectionOutgoing;
		expectedBearer = KCommDbBearerWcdma;
		}
	else if (aServiceType == TPtrC(INCOMING_GPRS))
		{
		expectedDirection = ECommDbConnectionDirectionIncoming;
		expectedBearer = KCommDbBearerWcdma;
		}
	else if (aServiceType == TPtrC(LAN_SERVICE))
		{
		expectedBearer = KCommDbBearerLAN | KCommDbBearerPAN | KCommDbBearerWLAN;
		}
	else if (aServiceType == TPtrC(VPN_SERVICE))
		{
		expectedBearer = KCommDbBearerVirtual;
		}
	
	TBool valid;
	if (aDlgPref == ECommDbDialogPrefPromptIfWrongMode)
		{
		// this pref should only be used for PSD bearers
		// if expectedBearer == KCommDbBearerWcdma then bearer set should
		// include something other than KCommDbBearerWcdma
		valid = (~expectedBearer & aBearerSet) != 0 && (expectedBearer == KCommDbBearerWcdma);
		}
	else if (aDlgPref == ECommDbDialogPrefPrompt)
		{
		// the expected bearer should be included in the bearer set
		// along with any other bearer

		if (aServiceType == TPtrC(VPN_SERVICE))	
			valid = ETrue;
		else
			valid = (expectedBearer & aBearerSet) != 0;
		}
	else
		{
		// no restriction on the dialog preference
		valid = ETrue;
		}

	if (valid)
		{
		valid = aDirection == expectedDirection ||
			expectedDirection == ECommDbConnectionDirectionUnknown;
		}

	if (!valid)
		User::Leave(KErrArgument);
	}



/************************ 2e) Validate IAP Record ************************************
*/

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

	
// The relation of an IAP to its mapped AccessPoint records is as depicted below:
//
//  IAP table (index == RecordId)               AP table (index == Tag)
//  ---------                                   --------
//
//                                              
//   -----                  iCprConfig == 1 (a)  -----
//  |    1|<------------------------------------|    1|
//  |     |- - - - - - - - - - - - - - - - - - >|     |<----------- +  
//   -----         (inferred association)        -----              |
//    ^ |                                   link AP                 |
//    | |                                                           |
//    | |                                                           |
//    | |                                                           |
//    | |                                                           |
//    | |                                                           |
//    | |   iAccessPoint == 256 (b)               -----             |
//    | +-------------------------------------->|  256| iSelectionPolicy == 257 (c)
//    |                                         |     |----+        |
//    |                                          -----     |        |
//    |                                     net AP         |        |
//    |                                                    |        |
//    |                     iCprConfig == 1 (a)  -----     |        |
//    +-----------------------------------------|  257|<---+        |
//                                              |     |-------------+ 
//                                               -----  iSelectionPolicy == 1 (d)
//                                          ipProto AP
//
//
// The following expressions are used to link the IAP and AP records:
// where:
// n = IAP record's recordId.
// B = the LinkBoundary (reserved Tags, currently 255)
//
// (a) = n
// (b) = B + 2n - 1
// (c) = (b) + 1
// (d) = n 
//

// The first 'KAPToIAPMappingThreshold' Tag Ids ('B' in the diagram above) are reserved for direct IAP to AP mapping,
// for those AP records where the index must match that of the RecordId of the corresponding IAP record.
//
// Note:
// We put the mapped AP's at the top of the permissible recordIds (being 254) and grows downwards.
//
// This should avoid colliding with any user created (either via input file, or dynamically). 
// At time of writing, in the XML files, the RecordId cannot be specified (unlike in .cfg files) and a record's Id 
// is calculated on the basis of its order of declaration in its table (in the markup).
// 
// In .cfg files (where it is able and usual to specify the recordId - using the well known "# COMMDB_ID = x" 
// statements) it is usual to start from 1 and count upwards.
//
// This might change, so is not included in the check at top of the function, when finding mapped AP records.
//
//static const TUint KAPToIAPTagMappingThreshold     = 255;

// TCleanupOperation function.
//
template <class T>
void DestroyRPointerArray(TAny* aPtr)
    {
    RPointerArray<T>* self = static_cast<RPointerArray<T>*>(aPtr);
    self->ResetAndDestroy();
    }


static const TUint32 KZero = 0;
    
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
void CCDValidatorIAPRecord::DeleteAssociatedLinkLevelAPL()
	{
	CCDAccessPointRecord* linkAPRecord = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(linkAPRecord);
	
	linkAPRecord->iRecordTag = iElement->RecordId();
	
	if (linkAPRecord->FindL(iSession.iOwner))
		{
		linkAPRecord->DeleteL(iSession.iOwner);
		}
	
	CleanupStack::PopAndDestroy(linkAPRecord);
	}
#endif

void CCDValidatorIAPRecord::ServiceAndBearerValidationL()
	{
#ifndef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	//at this point iElement has to be an IAP record!!!
	CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(iElement);
	
    if (!iapRecord->iService.IsNull() && !iapRecord->iServiceType.IsNull())
        {
        //this was caught in testing but actually it is not valid to have a real record without these fields...
        
        TMDBElementId tableTypeId = CommsDatSchemaV1_1::LookupTableId(iapRecord->iServiceType.GetL());
        iapRecord->iService = CommsDatSchema::GetLinkIdL(iSession.iOwner, iapRecord->iService.ElementId(), iapRecord->iService.GetL(), tableTypeId/*, &iapRecord*/);
        }

    if (!iapRecord->iBearer.IsNull() && !iapRecord->iBearerType.IsNull())
        {
        //this was caught in testing but actually it is not valid to have a real record without these fields...
        
        TMDBElementId tableTypeId = CommsDatSchemaV1_1::LookupTableId(iapRecord->iBearerType.GetL());
        iapRecord->iBearer = CommsDatSchema::GetLinkIdL(iSession.iOwner, iapRecord->iBearer.ElementId(), iapRecord->iBearer.GetL(), tableTypeId/*, &iapRecord*/);
        }
#endif
	}


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
void CCDValidatorIAPRecord::AssociatedLinkLevelAPGenerationL()
	{
    
    // map to AP record
	
	//at this point iElement has to be an IAP record!!!
    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(iElement);
    
    /*
    some of this work is a bit like mapping, but 
    the IAP record is still present in the database so doing this as validation
    */
    TInt err = KErrNone;
        
    //typedef enum {ENetworkAP, EIPProtocolAP, ELinkAP, EAPRecordIndicesLast = ELinkAP, EAPRecordIndicesRange = EAPRecordIndicesLast + 1} TAPRecordIndices;

    // Calculate the 3 potential AP indices
    //
	const TInt KThisIAPRecordId = iapRecord->RecordId();
	const TInt KLinkAPTag       = KThisIAPRecordId;
	//const TInt KNetworkAPTag    = KAPToIAPTagMappingThreshold + (2 * KThisIAPRecordId) - 1;
	//const TInt KIPProtoAPTag    = KNetworkAPTag + 1;

    __FLOG_STATIC1(KLogComponent, KCDInfoLog,
                        _L("CCDValidatorIAPRecord::DoValidateL() - Processing IAP record <%d>"),
                        KThisIAPRecordId);


    // 666
    // Should tidy these variables up into a class, but the turgidness of it all is just
    // distracting from getting immediate objective of getting over-night build tests going.
    //
    // Create copies of these recordIds, not so clever.
    //
    
    //const TInt  KAPRecordIdsTable[]    = {KNetworkAPTag, KIPProtoAPTag, KLinkAPTag};
    TUint numberOfAPRecordsFound = 0;
          
	//RPointerArray<CCDAccessPointRecord> apRecords;
    //CleanupStack::PushL(TCleanupItem(DestroyRPointerArray<CCDAccessPointRecord>, &apRecords));
    
    TBuf<KCDMaxFieldNameLength> tempName;
     _LIT(KMappedFrom, "MappedFromIAP");

   // See which, if any of the records of interest exist already.
    //
    //for (TUint i = 0; i <= EAPRecordIndicesLast; i++)
     //   {
    	CCDAccessPointRecord* record = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	    CleanupStack::PushL(record);
	    //apRecords.InsertL(record, i);
	    
        // Prime the search record and load it.

        TBool found = EFalse;        
        record->iRecordTag = KLinkAPTag; //KAPRecordIdsTable[i];
        TRAPD(loadErr, found = record->FindL(iSession.iOwner));           
        
        // Apply additional rules when validating, a mapped IAP must conform to.
        //
        if (loadErr == KErrNone && found)
            {
            _LIT(KAPNameCheckFormat, "%S%d");
            tempName.Format(KAPNameCheckFormat, &KMappedFrom, KThisIAPRecordId);

            // Its name should be that expected (ie which was applied by this function).
            //
            TPtrC tempPtr(static_cast<const TDesC&>(record->iRecordName));
            if (tempPtr.Find(tempName) != KErrNotFound
            	&& record->iSelectionPolicy == 0)

            // And the selection policy and cpr config should conform to the diagram at the top of this function.
            //
                 {
           	    numberOfAPRecordsFound++;
                 }
            }
        if (loadErr != KErrNone && loadErr != KErrNotFound)
            {
        	CleanupStack::PopAndDestroy(record);
        	record = NULL;
    		User::Leave(loadErr);
            }
       // } // for...
	

    // Now let's evaluate what we've found.
    //    
    if (numberOfAPRecordsFound == 0)
        {
        __FLOG_STATIC1(KLogComponent, KCDInfoLog,
                            _L("CCDValidatorIAPRecord::DoValidateL() - Creating AP record for IAP record <%d>"),
                                    KThisIAPRecordId);

        // No AP records exist for this IAP, the IAP record therefore needs to be mapped.
            
        // Go on and create the 3 new AP records... neatly presented from top to bottom of stack.
        
        _LIT(KAPNameFormat, "%S%S%d");


   	    CCDAccessPointRecord *newAPRecord =
   	                static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
		CleanupStack::PushL(newAPRecord);

        // ... and finally the link record

        // The clean up stack handling of 'pBearerType' seems a bit contorted.
        
        CCDBearerTypeRecord* pBearerType = NULL;
        TRAPD(ret, pBearerType = NewBearerTypeRecordL(iSession.iOwner, *iapRecord));
        if (ret != KErrNone || !pBearerType)
            {
            // Unfortunately, there are incorrect/unused IAPs in some of these configuration files (presumably they're dead).
            //
            __FLOG_STATIC1(KLogComponent, KCDInfoLog,
                                _L("CCDValidatorIAPRecord::DoValidateL() - error when mapping IAP record %d to its bearer"),
                                        KThisIAPRecordId);

            if (pBearerType)
                {
        		CleanupStack::PushL(pBearerType);
                }
                
            // But since we're not aiming to validate their config files, don't flag error, otherwise we'll leave and
            // the user will never get to create a (to them) usable database.
            }
        else if (ret == KErrNone && pBearerType)
            {
    		CleanupStack::PushL(pBearerType);

           	TPtrC name(pBearerType->iRecordName);
            tempName.Format(KAPNameFormat, &name, &KMappedFrom, KThisIAPRecordId);
            InitialiseAccessPointRecordL(
                *newAPRecord,
                tempName,
                KCDNewRecordRequest,
                KLinkAPTag,
                pBearerType->iTier,
                pBearerType->iMCpr,
                KZero,
                pBearerType->iCpr,
                KZero,
                pBearerType->iSCpr,
                pBearerType->iProtocol);
            newAPRecord->StoreL(iSession.iOwner);        // 666 - TRAP

			TInt linkAPTagId = newAPRecord->iRecordTag;
			
			if (!CommsDatMapperAndValidator::IsIPProtoAPAlreadyExistL(linkAPTagId, iSession.iOwner))
				   {
				   //The IPProto AP is not existing yet. Generate one.
				   CCDAccessPointRecord* ipprotoAP = CommsDatMapperAndValidator::GenerateIPProtoAPL(IPProtoBaseTagId, linkAPTagId, iSession.iOwner);
				   CleanupStack::PushL(ipprotoAP);
	    
				   //save the generated AP record
				   ipprotoAP->StoreL(iSession.iOwner);
				   CleanupStack::PopAndDestroy(ipprotoAP);
				   }

            
            __FLOG_STATIC1(KLogComponent, KCDInfoLog,
                                _L("CCDValidatorIAPRecord::DoValidateL() - Created AP records for IAP record <%d>"),
                                        KThisIAPRecordId);
        
    		// Update the IAP record itself to point to top-most AP tier record.
    		//
    		//thisIAPRecord->iAccessPoint = KNetworkAPTag;
    	    }
    	else
    	    {
            // 666 All 1 expected AP records exist and the links between records make sense.
            // So we assume that they have been correctly mapped already - perhaps have been called from ModifyL()?
            // Do nothing
            //
            __FLOG_STATIC1(KLogComponent, KCDInfoLog,
                                _L("CCDValidatorIAPRecord::DoValidateL() - Found existing valid AP record for IAP record <%d>"),
                                        KThisIAPRecordId);
    	    }


        if (pBearerType)    	    
            {
    	    CleanupStack::PopAndDestroy(pBearerType);
    	    }
		CleanupStack::PopAndDestroy(newAPRecord);
        }

	CleanupStack::PopAndDestroy(record);
	
	if (err != KErrNone)
	    {
		//User::Leave(err);
        __FLOG_STATIC2(KLogComponent, KCDErrLog,
              _L("CCDValidatorIAPRecord::DoValidateL() - error <%d> when mapping IAP record <%d> to AP - indeterminate state"),
              err, KThisIAPRecordId);
	    }

	}
#endif

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
void CCDValidatorIAPRecord::SetShadowElementL()
	{
	iShadowElement = CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
	iShadowElement->SetRecordId(iElement->RecordId());
	iShadowElement->LoadL(iSession.iOwner);
	
	if ( KCDMaskShowFieldType != (KCDMaskShowFieldType & iElement->ElementId()) )
		{
		//field validation
		CMDBElement* shadowFieldPtr = static_cast<CMDBRecordBase*>(iShadowElement)->GetFieldByIdL(iElement->ElementId());
		
	
		if ( KCDTIdRecordName == (iElement->ElementId() & KCDMaskShowFieldType) ||
			 KCDTIdIAPServiceType == (iElement->ElementId() & KCDMaskShowType) ||
			 KCDTIdIAPBearerType == (iElement->ElementId() & KCDMaskShowType) )
			{
			CMDBField<TDesC>* nameField = static_cast<CMDBField<TDesC>*>(shadowFieldPtr);
			CMDBField<TDesC>* origField = static_cast<CMDBField<TDesC>*>(iElement);
			
			nameField->SetL(*origField);		
			}
		else
			{
			//the only reason this cast can be done here is that the iap record contains only
			//numerical based fields... (except the name, bearertype, servicetype fields -> handled above...)
			CMDBField<TInt>* numField = static_cast<CMDBField<TInt>*>(shadowFieldPtr);
			CMDBField<TInt>* origField = static_cast<CMDBField<TInt>*>(iElement);
			
			if (EDelete == iOperation)
				{
				numField->SetL(0);
				}
			else
				{
				numField->SetL(*origField);
				}
			}
		}
	
	iElement = iShadowElement;
	}
#endif

void CCDValidatorIAPRecord::DoValidateL()
/*
 1/ Validate the record links for service and bearer records
 ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 2/ Create an AP record etc
 endif SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

@internalComponent
*/
    {
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	if (EDelete == iOperation )
    	{
	    if ( KCDMaskShowFieldType == (KCDMaskShowFieldType & iElement->ElementId()) )
	    	{
	    	//deleting a whole IAP record -> the associated AP record should be deleted as well
	    	DeleteAssociatedLinkLevelAPL();
	    	}
	    else
	    	{
	    	//deleting only a field from the record. Here the validator checks the inegrity of the IAP record.
	    	//So if the client deleted one of the bearer or service fields the integrity check won't be successful
	    	//we have to copy the origElemId because the SetShadowElementL will reset it...
	    	TUint origElemId = iElement->ElementId();
	    	
	    	SetShadowElementL();
	    	ServiceAndBearerValidationL();
	    	
	    	if ( KCDTIdIAPAppSid == (origElemId & KCDMaskShowType) )
	    		{
	    		//this field has to be mapped to the associated AP
	    		CMDBField<TInt>* linkAPTagId = new(ELeave)CMDBField<TInt>(KCDTIdAccessPointRecord | KCDTIdRecordTag);
	    		CleanupStack::PushL(linkAPTagId);
	    		
	    		*linkAPTagId = iElement->RecordId();
	    		if (linkAPTagId->FindL(iSession.iOwner))
	    			{
		    		CMDBField<TInt>* appSidField = new(ELeave)CMDBField<TInt>(KCDTIdAppSID);
		    		CleanupStack::PushL(appSidField);
		    		
		    		appSidField->SetRecordId(linkAPTagId->RecordId());
		    		appSidField->DeleteL(iSession.iOwner);
	    			
		    		CleanupStack::PopAndDestroy(appSidField);
	    			}
	    		CleanupStack::PopAndDestroy(linkAPTagId);
	    		}
	    	}
    	}
    else if (EStore == iOperation ||
    		 EModify == iOperation )
    	{
    	if ( KCDMaskShowFieldType == (KCDMaskShowFieldType & iElement->ElementId()) )
    		{
    		//ModifyL or StoreL is called on a record container
    		ServiceAndBearerValidationL();
    		}
    	else
    		{
    		//it's a field modification
    		SetShadowElementL();
    		ServiceAndBearerValidationL();
    		}
    	
    	if (EModify == iOperation)
    		{
    		DeleteAssociatedLinkLevelAPL();
    		}
    	
    	AssociatedLinkLevelAPGenerationL();
    	}
#else
    ServiceAndBearerValidationL();
#endif
    } // CCDMapperIAPRecord::DoValidateL()


void CCDValidatorIAPRecord::OnDeleteL()
    {
    // TODO Cleanup on deletion of IAP record (cleanup matching AP?) 
    // meanwhile do nothing
    User::LeaveIfError(ETrue); 
    }

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
void CCDValidatorIAPRecord::InitialiseAccessPointRecordL(      CCDAccessPointRecord& lhs,
                                                      const TDesC&                aRecordName,
                                                            TInt                  aRecordId,
                                                            TInt                  aTag,
                                                            TUint32               aTier,
                                                            TUint32               aMCpr,
                                                            TUint32               aSelectionPolicy,
                                                            TUint32               aCpr,
                                                            TUint32               aCprConfig,
                                                            TUint32               aSCpr,
                                                            TUint32               aProtocol)
    {
    lhs.iRecordName.SetMaxLengthL(aRecordName.Length());
	lhs.iRecordName      = aRecordName;
	lhs.SetRecordId(aRecordId);
    lhs.iRecordTag       = aTag;
	lhs.iTier            = aTier;
	lhs.iMCpr            = aMCpr;
	lhs.iSelectionPolicy = aSelectionPolicy;
	lhs.iCpr             = aCpr;
	lhs.iCprConfig       = aCprConfig;
	lhs.iSCpr            = aSCpr;
    lhs.iProtocol        = aProtocol;
    
    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(iElement);
    
	lhs.iAppSID.SetL(iapRecord->iAppSid);
	lhs.iPriority        = KMaxTUint;
    }
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
CCDBearerTypeRecord* CCDValidatorIAPRecord::NewBearerTypeRecordL(CMDBSession& aSessionOwner, CCDIAPRecord& aIAPRecord)
    {
    typedef enum {EModemBearer, ELANBearer, EVirtualBearer, TBearersLast = EVirtualBearer} TBearers;

    CCDBearerTypeRecord* pBearerType = NULL;
    
    __FLOG_STATIC1(KLogComponent, KCDInfoLog,
                        _L("CCDValidatorIAPRecord::NewBearerTypeRecordL() - Searching for bearer-type for IAP <%d>"),
                                aIAPRecord.RecordId());

    // Note:
    // These constants and tables are all grouped together at the top of the function.
    //
    // When intelligent links are added to CommsDat, much of this, and associated code, should become redundant.
    
    // Lookup table of the relevant bearer string identifiers.
    // Keys for the IAP record's 'iBearerType' field.
    //
    const TText* const KBearerLookupTable[] =
        {
        KCDTypeNameModemBearer,
        KCDTypeNameLANBearer,
        KCDTypeNameVirtualBearer
        };

    // Lookup table of the relevant TMDBElementIds
    //
    const TMDBElementId KBearerIdLookupTable[] =
        {
        KCDTIdModemBearerRecord,
        KCDTIdLANBearerRecord,
        KCDTIdVirtualBearerRecord
        };

    _LIT(KSpudNif,       "spud");
    _LIT(KSpudNifPrefix, "spud-%S");
    _LIT(KOutgoingGPRS,  "outgoinggprs");
    _LIT(KLowerNifKey,   "lowernif=");


    TBearers thisBearer = EModemBearer;
    TBool    found      = EFalse;
 	if (!aIAPRecord.iBearerType.IsNull())
   	    {
       	TPtrC bearerType(aIAPRecord.iBearerType);
       	
       	for (TInt i = 0; i <= TBearersLast; i++)
       	    {
       	    if (bearerType.CompareF(TPtrC(KBearerLookupTable[i])) == 0)
       	        {
       	        thisBearer = static_cast<TBearers>(i);
                __FLOG_STATIC1(KLogComponent, KCDInfoLog,
                                    _L("CCDMapperIAPRecord::NewBearerTypeRecordL() - Found bearer-type <%d>"),
                                            thisBearer);
                found = ETrue;
       	        break;
       	        }
       	    }
       	}


    if (!found)
        {
        return pBearerType;
        }
        

    CCDBearerRecordBase* pBearer = static_cast<CCDBearerRecordBase*>(CCDRecordBase::RecordFactoryL(KBearerIdLookupTable[thisBearer]));
    CleanupStack::PushL(pBearer);
    
    __FLOG_STATIC2(KLogComponent, KCDInfoLog,
                        _L("CCDMapperIAPRecord::NewBearerTypeRecordL() - Loading bearer record <%d>, for bearer-type <%d>"),
                                static_cast<TInt>(aIAPRecord.iBearer),
                                thisBearer);

    pBearer->SetRecordId(aIAPRecord.iBearer);
    pBearer->LoadL(aSessionOwner);
    

   	pBearerType = static_cast<CCDBearerTypeRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdBearerTypeRecord));
	CleanupStack::PushL(pBearerType);


    TBool keepGoing = ETrue;
    
    TBuf<KMaxName> thisNifName;

    // Get the bearer's nif name (shouldn't the NifName be a member of the base class?, oh well, too late now).
    // Requires horrible switch statement.
    //
    switch (thisBearer)
        {
        case EModemBearer:
            {
            // The usual modem bearer case is:
            //
            //      ModemBearer[IAP.iBearer]->iNifName
            //
            // SPUD is an exception (if ModemBearer[IAP.iBearer]->iNifName == "SPUD"):
            //
            //      OutgoingGPRS[IAP.Service]->iIfParame
            //          Extract the nif's name from the resultant string, of form "lowernif=xxxxx"
            

            TPtrC nifName(static_cast<CCDModemBearerRecord*>(pBearer)->iNifName);
            
            if (nifName.CompareF(KSpudNif) == 0)
                {
                // Do "special" SPUD related processing to find the lowernif from the OutgoingGPRS table.

               	TPtrC serviceType(aIAPRecord.iServiceType);

                // Check the expected service type
                //
           	    if (serviceType.CompareF(KOutgoingGPRS) == 0)
           	        {
                    CCDOutgoingGprsRecord* pOutgoingGprs =
                            static_cast<CCDOutgoingGprsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdOutgoingGprsRecord));
                    CleanupStack::PushL(pOutgoingGprs);

                    // Load the associated OutgoingGPRS record.
                    
                    pOutgoingGprs->SetRecordId(aIAPRecord.iService);
                    pOutgoingGprs->LoadL(aSessionOwner);

                    // Get the 'iGPRSIfParams' value.
                     
            	    TPtrC gprsIfParams(static_cast<CCDOutgoingGprsRecord*>(pOutgoingGprs)->iGPRSIfParams);

                    // And search for the key and its value (which should be the Nif we want)
                    //            	    
            	    TInt lowerNifIndex = gprsIfParams.FindF(KLowerNifKey);
            	    if (lowerNifIndex >= 0)
            	        {
            	        TPtrC lowerNifName = gprsIfParams.Mid(lowerNifIndex + KLowerNifKey().Length());
 
                        // Cannot work out how to use the iRecordName member itself to call Format() on.
                        
            	        thisNifName.Format(KSpudNifPrefix, &lowerNifName);
            	        }
            	    else
            	        {
                        __FLOG_STATIC1(KLogComponent, KCDErrLog,
                                            _L("CCDMapperIAPRecord::NewBearerTypeRecordL() - Failed to find \"lowernif=\" for IAP record <%d>"),
                                                    aIAPRecord.RecordId());
            	        keepGoing = EFalse;
            	        }
            	    
                    CleanupStack::PopAndDestroy(pOutgoingGprs);
           	        }
           	    else
           	        {
                    __FLOG_STATIC1(KLogComponent, KCDErrLog,
                                        _L("CCDMapperIAPRecord::NewBearerTypeRecordL() - unexpected SPUD bearer for IAP record <%d>"),
                                                aIAPRecord.RecordId());
                    keepGoing = EFalse;
           	        }
                }
            else
                {
                // That's easy            
                thisNifName = nifName;
                }
            }
            break;
            
        case ELANBearer:
            {
           	TPtrC nifName(static_cast<CCDLANBearerRecord*>(pBearer)->iLanBearerNifName);
            thisNifName = nifName;
           	}
           	break;
           	
    	case EVirtualBearer:
            {
           	TPtrC nifName(static_cast<CCDVirtualBearerRecord*>(pBearer)->iVirtualBearerNifName);
            thisNifName = nifName;
           	}
           	break;

        default:
            keepGoing = EFalse;
            break;
        }       
           
           	    
    if (keepGoing)
        {
        __FLOG_STATIC2(KLogComponent, KCDInfoLog,
                            _L("CCDMapperIAPRecord::NewBearerTypeRecordL() - Looking up data for bearer-type <%S> for IAP <%d>"),
                                    &thisNifName,
                                    aIAPRecord.RecordId());

        // The letters in the name of the bearer can appear in various cases. therefore
        // convert the name of the bearer (however it was found) to lower-case letters. 
        //
        // Note:
        // Constraint is that the names of the bearer types in the preface file must also be lower-case.
        //
        thisNifName.LowerCase();
        pBearerType->iRecordName.SetMaxLengthL(thisNifName.Length());
        pBearerType->iRecordName = thisNifName;
        
    	if (!pBearerType->FindL(aSessionOwner))
	        {
            __FLOG_STATIC2(KLogComponent, KCDErrLog,
                                _L("CCDMapperIAPRecord::NewBearerTypeRecordL() - Failed to find bearer-type <%S> for IAP <%d>"),
                                        &thisNifName,
                                        aIAPRecord.RecordId());

			User::Leave(KErrNotFound);
	        }
        }

    CleanupStack::Pop(pBearerType);
    CleanupStack::PopAndDestroy(pBearer);
        
    return pBearerType;
    } // NewBearerTypeRecordL()

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY



//---------- The IPProto AP handler classes -------------
    
/*virtual*/ void CIPProtoAPCreator::DoTheWorkL()
    {
    CCDConnectionPrefsRecord* ptrConnPref = static_cast<CCDConnectionPrefsRecord*>(&iElement);
    
    CMDBRecordLink<CCDAccessPointRecord>* apPrioritySelPolFieldToBeModified = 
                                                GetTheAPxLinkFromAPPrioritySelPol(iAPSelPolRec,
                                                                                  ptrConnPref->iRanking);
    CCDAccessPointRecord* ipprotoAP = NULL;
    
    TBool modified = EFalse;
	TBool isIPProtoAPExisting = EFalse;
	
    if (ECommDbDialogPrefUnknown    == ptrConnPref->iDialogPref ||
        ECommDbDialogPrefDoNotPrompt== ptrConnPref->iDialogPref)
        {
        //this is a non prompting connpref rec
        isIPProtoAPExisting = CommsDatMapperAndValidator::IsIPProtoAPAlreadyExistL(iLinkAPTagId, 
    															 				   iSession.iOwner);
        }
    else
    	{
    	//this connpref rec is prompting so we need to consider the cprConfig field of the possibly
    	//existing IPProtoAP too
    	isIPProtoAPExisting = CommsDatMapperAndValidator::IsIPProtoAPAlreadyExistL(iLinkAPTagId, 
    																			   ptrConnPref->ElementId(), 
    															 				   iSession.iOwner);
    	}
    
    if (!isIPProtoAPExisting)
    	{
    	//The IPProto AP is not existing yet. Generate one.
	    ipprotoAP = CommsDatMapperAndValidator::GenerateIPProtoAPL(IPProtoBaseTagId,
	                                                               iLinkAPTagId,
	                                                               iSession.iOwner);
	    
	    CleanupStack::PushL(ipprotoAP);
	    
	    ipprotoAP->StoreL(iSession.iOwner);
	    
	    //modified = ETrue;
    	}
    else
    	{
    	//The IPProto AP is already exsiting. Use that one.
    	ipprotoAP = CommsDatMapperAndValidator::LoadIPProtoAccessPointL(iLinkAPTagId, iSession.iOwner);
    	
    	CleanupStack::PushL(ipprotoAP);
    	}
    
    
    if (KCDChangedFlag == (ptrConnPref->iDialogPref.ElementId() & KCDChangedFlag))
    	{
    	/* The dialogPref field has changed -> This means that:
    	 *  a) we have to create a 'prompting AP' which means that the AP has to 
    	 *     contain the full TMDBElement ID of the given connpref record in its' 
    	 *     CprConfig field (prompting connpref rec).
    	 *  b) the CprConfig field has to contain the IAP record number this 
    	 *     IPProto AP points to (not prompting connpref rec).
    	 */ 
        TBool isPromptingAPNeeded;
        
        if (ECommDbDialogPrefUnknown    == ptrConnPref->iDialogPref ||
            ECommDbDialogPrefDoNotPrompt== ptrConnPref->iDialogPref)
            {
            isPromptingAPNeeded = EFalse;
            }
        else
            {
            isPromptingAPNeeded = ETrue;
            }
    	
        if (isPromptingAPNeeded)
        	{
        	//the cprConfig has to contain the full TMDBElementId of the connpref record
        	ipprotoAP->iCprConfig = ptrConnPref->ElementId();
        	}
        else
        	{
        	/* the cprConfig field has to contain the IAP record id
        	 * Because this mapping/validation is done from a legacy configuration
        	 * commsdat can assume here that there will be only 3 layers.
        	 * 
        	 * The fillowing line says that the IAP pointed by the cprConfig and the
        	 * Link level AP pointed by the customSelectionPolicy field are the same.
        	 */
        	ipprotoAP->iCprConfig.SetL(ipprotoAP->iCustomSelectionPolicy);
        	}
        
        modified = ETrue;
    	}
    
    if (modified)
    	{
    	ipprotoAP->ModifyL(iSession.iOwner);
    	}
    
    TMDBElementId elemId = ipprotoAP->ElementId();
    elemId &= KCDMaskHideAttrAndRes;
    TMDBElementId tableAndRecordBitMask = KCDMaskShowRecordType | KCDMaskShowRecordId;
    elemId &= tableAndRecordBitMask;
    
    //a) update the APPrioritySelPol record to point to the newly creeated AP record
    CommsDatMapperAndValidator::ModifyAPPrioritySelPolRecL(apPrioritySelPolFieldToBeModified, 
                                                          iAPSelPolRec,
                                                          elemId);
    
    
    iAPSelPolRec->ModifyL(iSession.iOwner);
    
    //maintain the link from the IPProto Tier record
    if (1 == ptrConnPref->iRanking)
        {
        CommsDatMapperAndValidator::ModifyDefaultTierRecordL(elemId, iSession.iOwner);
        }
    
    CleanupStack::PopAndDestroy(ipprotoAP);
    }



/*virtual*/ void CIPProtoAPModifier::DoTheWorkL()
    {
    TBool modified = EFalse;
    CCDConnectionPrefsRecord* ptrConnPref = static_cast<CCDConnectionPrefsRecord*>(&iElement);
    
    CMDBRecordLink<CCDAccessPointRecord>* apPrioritySelPolFieldToBeModified = 
                                                GetTheAPxLinkFromAPPrioritySelPol(iAPSelPolRec,
                                                                                  ptrConnPref->iRanking);
    
    CCDAccessPointRecord* apRecordToBeModified = NULL;
    
    /* At this point we have to check: 
     * 
     * Is there any other link to this AP.
     * If there is: 
     * ------------  
     * 		a new AP should be generated and linked from the given
     * 		APPrioritySelPol field
     * 
     * If there isn't:
     * --------------- 
     * 		Is there any other IPProto AP which contains the given link AP tagId
     * 		in its' customSelPolField?
     * 			If there is:
     * 				No new AP should be generated but the already existing one
     * 				should be used
     * 				The AP which is not used anymore should be deleted
     * 
     * 			If there isn't
     * 				this means that the given AP can be modified.
     */
    if (1 == CommsDatMapperAndValidator::CountReferenceToThisIPProtoAPL(*apPrioritySelPolFieldToBeModified,
    																	iSession.iOwner))
    	{
    	/* There is only 1 reference to this AP. Here the following should be considered:
    	 * If there is already an IPProto AP whith the given link AP tagId value in the customselPol field
    	 * no new AP should be generated but the one should be used, and the old one should be deleted!.
    	 * If there is no AP with the given link AP tagId value in the customselPol field
    	 * a new one should be generated.
    	 */
    	
        apRecordToBeModified = CommsDatMapperAndValidator::LoadIPProtoAccessPoint(iLinkAPTagId, iSession.iOwner);
    	if (!apRecordToBeModified)
    		{
    		//ok, there is no AP with the given customSelPol field
	        apRecordToBeModified = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	        CleanupStack::PushL(apRecordToBeModified);
	        
	        apRecordToBeModified->SetElementId(*apPrioritySelPolFieldToBeModified);
	        
	    	apRecordToBeModified->LoadL(iSession.iOwner);
    		}
    	else
    		{
    		//there is already an AP with the given customSelPol field
    		//delete the currently used IPProto AP
        	CleanupStack::PushL(apRecordToBeModified);
        	
    		CCDAccessPointRecord* apRecordToBeDeleted = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    		CleanupStack::PushL(apRecordToBeDeleted);
    		//DeleteL needs the '7F' in the FieldType...
    		apRecordToBeDeleted->SetElementId(*apPrioritySelPolFieldToBeModified | KCDMaskShowFieldType);
    		
        	if (apRecordToBeDeleted->RecordId() != apRecordToBeModified->RecordId())
        		{
        		//we can delete the actual AP...
	        	apRecordToBeDeleted->DeleteL(iSession.iOwner);
	        	
	    	    TMDBElementId elemId = apRecordToBeModified->ElementId();
	    	    elemId &= KCDMaskHideAttrAndRes;
	    	    TMDBElementId tableAndRecordBitMask = KCDMaskShowRecordType | KCDMaskShowRecordId;
	    	    elemId &= tableAndRecordBitMask;
	    	    
	    	    //update the APPrioritySelPol record to point to the newly creeated AP record
	    	    CommsDatMapperAndValidator::ModifyAPPrioritySelPolRecL(apPrioritySelPolFieldToBeModified, 
	    	                                                          iAPSelPolRec,
	    	                                                          elemId);
        		}
        	
        	CleanupStack::PopAndDestroy(apRecordToBeDeleted);
        	
    		}
    	}
    else
    	{
    	/* There is more than 1 link to the given AP record. 
    	 * If we would modify it with the same Link level TagId -> no necessary to do anything...
    	 * 		This could happen if we processed already the IAPPrioritySelPol record and there
    	 * 		is a ConnPref record too where the default IAP is the same as in the orig
    	 * 		IAPPriorioritySelPol record. 
    	 * If it's a different Link level TagId -> Can't modify the AP instead create a new one.
    	 */
    	
    	TBool isIPProtoAPExisting = EFalse;
    	
        if (ECommDbDialogPrefUnknown    == ptrConnPref->iDialogPref ||
            ECommDbDialogPrefDoNotPrompt== ptrConnPref->iDialogPref)
            {
            //this is a non prompting connpref rec
            isIPProtoAPExisting = CommsDatMapperAndValidator::IsIPProtoAPAlreadyExistL(iLinkAPTagId, 
        															 				   iSession.iOwner);
            }
        else
        	{
        	//this connpref rec is prompting so we need to consider the cprConfig field of the possibly
        	//existing IPProtoAP too
        	isIPProtoAPExisting = CommsDatMapperAndValidator::IsIPProtoAPAlreadyExistL(iLinkAPTagId, 
        																			   ptrConnPref->ElementId(), 
        															 				   iSession.iOwner);
        	}
        
    	if (isIPProtoAPExisting)
    		{
    		//the given IPProto is already existing with the received Link TagId (and cprConfig
    		//field if the connpref is prompting). Let's use that one
    		apRecordToBeModified = CommsDatMapperAndValidator::LoadIPProtoAccessPointL(iLinkAPTagId, iSession.iOwner);
        	
        	CleanupStack::PushL(apRecordToBeModified);
    		
    		}
    	else
    		{
	    	apRecordToBeModified = CommsDatMapperAndValidator::GenerateIPProtoAPL(IPProtoBaseTagId,
		                                                               			  iLinkAPTagId,
		                                                               			  iSession.iOwner);
	    	
		    CleanupStack::PushL(apRecordToBeModified);
		    
		    //save the generated AP record
		    apRecordToBeModified->StoreL(iSession.iOwner);
    		}
		    
	    TMDBElementId elemId = apRecordToBeModified->ElementId();
	    elemId &= KCDMaskHideAttrAndRes;
	    TMDBElementId tableAndRecordBitMask = KCDMaskShowRecordType | KCDMaskShowRecordId;
	    elemId &= tableAndRecordBitMask;
	    
	    //update the APPrioritySelPol record to point to the newly creeated AP record
	    CommsDatMapperAndValidator::ModifyAPPrioritySelPolRecL(apPrioritySelPolFieldToBeModified, 
	                                                          iAPSelPolRec,
	                                                          elemId);
    	}
    
    if (0 != apRecordToBeModified->iSelectionPolicy)
    //it is possible from the cfg files that the iSelectionPolicy field is used to link to the 
    //link layer AP. If this is the case it should be 0ed.
        {
        apRecordToBeModified->iSelectionPolicy.SetL(0);
        modified = ETrue;
        }
    
    if (apRecordToBeModified->iCustomSelectionPolicy != iLinkAPTagId)
        {
        apRecordToBeModified->iCustomSelectionPolicy.SetL(iLinkAPTagId);
        apRecordToBeModified->iCprConfig.SetL(iLinkAPTagId);
        modified = ETrue;
        }
    
    /* This is just cosmetics here. If we modify the default IPProto AP, which is
     * coming from the meshpreface, then the name of the record should be modified.
     */ 
    TPtrC recName(apRecordToBeModified->iRecordName);
    if (0 == recName.Compare(KDefaultIPProtoRecName))
    	{
        //buffer for the record name
        TBuf<KMaxName> recordName;
        CommsDatMapperAndValidator::GenerateRecordName(recordName, apRecordToBeModified->iRecordTag);
        apRecordToBeModified->iRecordName.SetMaxLengthL(recordName.Length());
        apRecordToBeModified->iRecordName = recordName;
        modified = ETrue;
    	}
        
    if (KCDChangedFlag == (ptrConnPref->iDialogPref.ElementId() & KCDChangedFlag))
    	{
    	/* The dialogPref field has changed -> This means that:
    	 *  a) we have to create a 'prompting AP' which means that the AP has to 
    	 *     contain the full TMDBElement ID of the given connpref record in its' 
    	 *     CprConfig field (prompting connpref rec).
    	 *  b) the CprConfig field has to contain the IAP record number this 
    	 *     IPProto AP points to (not prompting connpref rec).
    	 */ 
        TBool isPromptingAPNeeded;
        
        if (ECommDbDialogPrefUnknown    == ptrConnPref->iDialogPref ||
            ECommDbDialogPrefDoNotPrompt== ptrConnPref->iDialogPref)
            {
            isPromptingAPNeeded = EFalse;
            }
        else
            {
            isPromptingAPNeeded = ETrue;
            }
    	
        if (isPromptingAPNeeded)
        	{
        	//the cprConfig has to contain the full TMDBElementId of the connpref record
        	apRecordToBeModified->iCprConfig = ptrConnPref->ElementId();
        	}
        else
        	{
        	/* the cprConfig field has to contain the IAP record id
        	 * Because this mapping/validation is done from a legacy configuration
        	 * commsdat can assume here that there will be only 3 layers.
        	 * 
        	 * The following line says that the IAP pointed by the cprConfig and the
        	 * Link level AP pointed by the customSelectionPolicy field are the same.
        	 */
        	apRecordToBeModified->iCprConfig.SetL(apRecordToBeModified->iCustomSelectionPolicy);
        	}
        
        modified = ETrue;
    	}
    
    iAPSelPolRec->ModifyL(iSession.iOwner);
    
    if (modified)
        {
        apRecordToBeModified->ModifyL(iSession.iOwner);
        }
    
    //maintain the link from the IPProto Tier record
    if (1 == ptrConnPref->iRanking)
        {
	    TMDBElementId elemId = apRecordToBeModified->ElementId();
	    elemId &= KCDMaskHideAttrAndRes;
	    TMDBElementId tableAndRecordBitMask = KCDMaskShowRecordType | KCDMaskShowRecordId;
	    elemId &= tableAndRecordBitMask;
	    
        CommsDatMapperAndValidator::ModifyDefaultTierRecordL(elemId, iSession.iOwner);
        }
    
    
    
	/* one last checking: if there is another IPProto AP with the same cusomtSelPol
	 * and CprConfig fields then only 1 can be in the database the other should be deleted.
	 */
    if (CommsDatMapperAndValidator::GetNumberOfThisIPProtoAPL(apRecordToBeModified, iSession.iOwner) > 1)
    	{
    	//this means that we have 2 IPProto AP with the same CustomSelPol and CprConfig
    	//fields
    	
    	//Correct the links from the defaultTier and APPriotySelPol links
    	//This is the elementId which will be deleted
    	CommsDatMapperAndValidator::CorrectLinksL(apRecordToBeModified->ElementId(), iSession.iOwner);
    	
    	//delete the AP
    	apRecordToBeModified->DeleteL(iSession.iOwner);
    	}
    
    CleanupStack::PopAndDestroy(apRecordToBeModified);
    }

/*virtual*/ void CIPProtoAPRemover::DoTheWorkL()
    {
    CCDConnectionPrefsRecord* ptrConnPref = static_cast<CCDConnectionPrefsRecord*>(&iElement);
    
        
    CMDBRecordLink<CCDAccessPointRecord>* apPrioritySelPolFieldToBeModified = 
                                                GetTheAPxLinkFromAPPrioritySelPol(iAPSelPolRec,
                                                                                  ptrConnPref->iRanking);
    
    CCDAccessPointRecord* ipprotoAPRec = NULL;
    
    

    //if this was a ranking1 AP then the tier record has to be modified too...
    if (1 == ptrConnPref->iRanking)
        {
        /**
        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!    IMPORTANT:   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        
        The RANK1 connpref record is deleted which means that there is no default legacy path
        in the database. In the 399 architecture in this case we create the default IPProto AP
        which is in the meshpreface file by default (it's possible that it's overwritten) so 
        we modify the current AP by not deleting it but modifying it to contain the CCDAccessPointRecord::KNoPolicy value
        in its' CustomSelectionPolicy field.
        
        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        */
	    if (1 == CommsDatMapperAndValidator::CountReferenceToThisIPProtoAPL(*apPrioritySelPolFieldToBeModified,
	    																	iSession.iOwner))
	    	{
	    	//ok there is only 1 link to this AP -> we can modify it
	    	
	    	ipprotoAPRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	    	CleanupStack::PushL(ipprotoAPRec);
	    	
	    	ipprotoAPRec->SetRecordId((*apPrioritySelPolFieldToBeModified & KCDMaskShowRecordId) >> 8);
	    	
	    	
	    	ipprotoAPRec->LoadL(iSession.iOwner);
	        
	    	ipprotoAPRec->iCustomSelectionPolicy = CCDAccessPointRecord::KNoPolicy;
	        
		    ipprotoAPRec->iRecordName.SetMaxLengthL(KDefaultIPProtoRecName().Length());
		    ipprotoAPRec->iRecordName = KDefaultIPProtoRecName;
		    
		    ipprotoAPRec->iCprConfig = 0;
		    
	    	ipprotoAPRec->ModifyL(iSession.iOwner);
	    	}
	    else
	    	{
	    	//a new default AP should be generated
	    	ipprotoAPRec = CommsDatMapperAndValidator::GenerateIPProtoAPL(IPProtoBaseTagId,
		                                                               	  CCDAccessPointRecord::KNoPolicy,
		                                                               	  iSession.iOwner);
	    	
		    CleanupStack::PushL(ipprotoAPRec);
		    
		    ipprotoAPRec->iRecordName.SetMaxLengthL(KDefaultIPProtoRecName().Length());
		    ipprotoAPRec->iRecordName = KDefaultIPProtoRecName;
		    
		    //save the generated AP record
		    ipprotoAPRec->StoreL(iSession.iOwner);
		    
		    
		    TMDBElementId elemId = ipprotoAPRec->ElementId();
		    elemId &= KCDMaskHideAttrAndRes;
		    TMDBElementId tableAndRecordBitMask = KCDMaskShowRecordType | KCDMaskShowRecordId;
		    elemId &= tableAndRecordBitMask;
		    
		    //update the APPrioritySelPol record to point to the newly creeated AP record
		    CommsDatMapperAndValidator::ModifyAPPrioritySelPolRecL(apPrioritySelPolFieldToBeModified, 
		                                                          iAPSelPolRec,
		                                                          elemId);
		    
		    iAPSelPolRec->ModifyL(iSession.iOwner);
		    
		    //modify the IPProto tier to point to this new default AP
		    CMDBRecordLink<CCDAccessPointRecord>* ipProtoDefAccessPoint = new(ELeave)CMDBRecordLink<CCDAccessPointRecord>(KCDTIdDefaultAccessPoint);
		    CleanupStack::PushL(ipProtoDefAccessPoint);
		    
		    TInt ipProtoRecId = CommsDatMapperAndValidator::GetIPProtoTierRecordIdL(iSession.iOwner);
		    ipProtoDefAccessPoint->SetRecordId(ipProtoRecId);
		    
		    *ipProtoDefAccessPoint = elemId;
		    ipProtoDefAccessPoint->ModifyL(iSession.iOwner);
	        
		    CleanupStack::PopAndDestroy(ipProtoDefAccessPoint);
	    	}
        //no further modification is needed here...
        }
    else
    	{
    	//RANK2 record is deleted
	    iAPSelPolRec->iApCount.SetL(1);
	    
	    if (1 == CommsDatMapperAndValidator::CountReferenceToThisIPProtoAPL(*apPrioritySelPolFieldToBeModified,
	    																	iSession.iOwner))
	    	{
	    	//ok, there is only 1 link to this AP -> it can be deleted
	    	ipprotoAPRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	    	
	    	CleanupStack::PushL(ipprotoAPRec);
	    	
	    	ipprotoAPRec->SetRecordId((*apPrioritySelPolFieldToBeModified & KCDMaskShowRecordId) >> 8);
	    	
	    	ipprotoAPRec->DeleteL(iSession.iOwner);
	    	}
	    
	    //update the APPrioritySelPol record. 
	    *apPrioritySelPolFieldToBeModified = 0;
	    iAPSelPolRec->ModifyL(iSession.iOwner);
    	}
    
    if (ipprotoAPRec)
    	{
    	CleanupStack::PopAndDestroy(ipprotoAPRec);
    	}
    }
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
//EOF
