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
// Implementation of MetaDatabase classes declared in MetaDatabase.h
// 
//

/**
 @file
 @publishedAll - implementation of interface functions
 @released 
*/


#include "CommsDatInternalDefs.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_internal.h>
#include <commsdat_partner.h>
#endif

using namespace CommsDat;
using namespace CommsDatInternal;
using namespace Meta;


//

/**
Instantiates a TMDBNumBaseVisitor (TMDBVisitor of type TInt)
*/
MMetaType* Meta::TMDBNumBaseVisitorFactoryL(const TAny* mem, const TAny* data)
    {
    CommsDat::TMDBVisitor<TInt>* ptr = CommsDat::TMDBVisitor<TInt>::NewL(mem,data);
    return ptr;
    }


/**
Instantiates a TMDBTextBaseVisitor (TMDBVisitor of type HBufC* with max length = max text length)
*/
MMetaType* Meta::TMDBTextBaseVisitorFactoryL(const TAny* mem, const TAny* data)

    {
    CommsDat::TMDBVisitorBase* ptr = CommsDat::TMDBVisitor<HBufC*>::NewL(mem,data);
    ptr->SetMaxLength(KMaxTextLength); // NB User can set greater length
    return ptr;
    }

/**
Instantiates a TMDBBinBaseVisitor (TMDBVisitor of type HBufC8* with max length = max long text length))
*/
MMetaType* Meta::TMDBBinBaseVisitorFactoryL(const TAny* mem, const TAny* data)

    {
    CommsDat::TMDBVisitor<HBufC8*>* ptr = CommsDat::TMDBVisitor<HBufC8*>::NewL(mem,data);
    ptr->SetMaxLength(KMaxLongTextLength);
    return ptr;
    }

/**
This element has been deprecated and is not stored in the database.  
So there is nothing to do here.
The element will be handled by mapping if a client has started the session 
with a deprecated version.  Otherwise both commsdat and the client will 
ignore the element (up to client to comply as clearly stated in the interface).
BTW don't change this to leave with KErrUnsupported because other elements in the 
record will still need processing
*/
MMetaType* Meta::TMDBDeprecatedVisitorFactoryL(const TAny* /*mem*/, const TAny* /*data*/)
    {
    // No-op (must be non-leaving)
    User::LeaveIfError(KErrNone); // HERE ONLY TO SATISFY LEAVESCAN AS HAVE TO NAME THIS FUNCTION "blahVisitorFactoryL" to satisfy macro.
    
    return NULL;
    }



/**
Instantiates a TMDBNumVisitor (TMDBVisitor of type TInt)
*/
EXPORT_C Meta::MMetaType* Meta::TMDBNumVisitorFactoryL(const TAny* mem, const TAny* data)

    {
    CommsDat::TMDBVisitor<TInt>* ptr = CommsDat::TMDBVisitor<TInt>::CreateL(mem,data);
    return ptr;
    }
/**
Instantiates a TMDBRecordLinkVisitor (TMDBVisitor of type Link type)
*/
EXPORT_C Meta::MMetaType* Meta::TMDBLinkNumVisitorFactoryL(const TAny* mem, const TAny* data)
    {
    CommsDat::TMDBRecordLinkVisitor* ptr = CommsDat::TMDBRecordLinkVisitor::NewL(mem,NULL);
    // deliberate c cast
    ptr->iTarget = (SMDBMetaElement<TInt>*)( ((CMDBElement*)data)->Data() );
	ptr->SetElementId(&(ptr->iTarget->iElementId));
	return ptr;
    }

/**
Instantiates a TMDBTextVisitor (TMDBVisitor of type HBufC* with max length = max text length))
*/
EXPORT_C Meta::MMetaType* Meta::TMDBTextVisitorFactoryL(const TAny* mem, const TAny* data)
    {
    CommsDat::TMDBVisitor<HBufC*>* ptr = CommsDat::TMDBVisitor<HBufC*>::CreateL(mem,data);
    ptr->SetMaxLength(KMaxTextLength);
    // Check for NULL field then check text length
   	if((ptr->iTarget->iValue) && (ptr->iTarget->iValue->Length() > KMaxTextLength))
   		{
   		User::Leave(KErrOverflow);
   		}
    return ptr;
    }

/**
Instantiates a TMDBMedTextVisitor (TMDBVisitor of type HBufC* with max length = max medium text length))
*/
EXPORT_C MMetaType* Meta::TMDBMedTextVisitorFactoryL(const TAny* mem, const TAny* data)
    {
    CommsDat::TMDBVisitor<HBufC*>* ptr = CommsDat::TMDBVisitor<HBufC*>::CreateL(mem,data);
    ptr->SetMaxLength(KMaxMedTextLength);
    // Check for NULL field then check text length
   	if((ptr->iTarget->iValue) && (ptr->iTarget->iValue->Length() > KMaxMedTextLength))
   		{
   		User::Leave(KErrOverflow);
   		}
    return ptr;
    }

/**
Instantiates a TMDBLongTextVisitor (TMDBVisitor of type HBufC8* with max length = max long text length)
*/
EXPORT_C MMetaType* Meta::TMDBLongTextVisitorFactoryL(const TAny* mem, const TAny* data)
    {
    CommsDat::TMDBVisitor<HBufC*>* ptr = CommsDat::TMDBVisitor<HBufC*>::CreateL(mem,data);
    ptr->SetMaxLength(KMaxLongTextLength);
    // Check for NULL field then check text length
   	if((ptr->iTarget->iValue) && (ptr->iTarget->iValue->Length() > KMaxLongTextLength))
   		{
   		User::Leave(KErrOverflow);
   		}
    return ptr;
    }

/**
Instantiates a TMDBBinVisitor (TMDBVisitor of type HBufC8* with max length = max long text length))
*/
EXPORT_C MMetaType* Meta::TMDBBinVisitorFactoryL(const TAny* mem, const TAny* data)
    {
    CommsDat::TMDBVisitor<HBufC8*>*  ptr = CommsDat::TMDBVisitor<HBufC8*>::CreateL(mem,data);
    ptr->SetMaxLength(KMaxLongTextLength);
    return ptr;
    }

/**
Instantiates a TMDBLinkVisitor
*/
EXPORT_C MMetaType* Meta::TMDBLinkVisitorFactoryL(const TAny* mem, const TAny* data)
    {
    CommsDat::TMDBRecordLinkVisitor* ptr = CommsDat::TMDBRecordLinkVisitor::NewL(mem,data);
	return ptr;
    }

/**
Instantiates a TMDBRecordSetVisitor
*/
EXPORT_C MMetaType* Meta::TMDBRecordSetVisitorFactoryL(const TAny* mem, const TAny* data)
    {
    CommsDat::TMDBRecordSetVisitor* ptr = CommsDat::TMDBRecordSetVisitor::NewL(mem,data);
    return ptr;
    }

/**
Instantiates TMDBGenericRecordVisitor
*/
EXPORT_C Meta::MMetaType* Meta::TMDBGenericRecordVisitorFactoryL(const TAny* mem, const TAny* data)
    {
    CommsDat::TMDBGenericRecordVisitor* ptr = CommsDat::TMDBGenericRecordVisitor::NewL(mem,data);
	return ptr;
    }


/*
Default Constructor

@publishedAll
*/

EXPORT_C CMDBElement::CMDBElement() 
    : iElementId(0), iValue(0)
/* 
Default Constructor

@publishedAll
*/
    {
    }

EXPORT_C CMDBElement::CMDBElement(TMDBElementId aElementId)
    : iElementId(aElementId), iValue(0)
/*
Constructor setting element id

@publishedAll
*/
    {
    } 
    
    
EXPORT_C CMDBElement::~CMDBElement()
/* 
Destructor - nothing to delete

@publishedAll
*/
    {
    }

    
    
EXPORT_C TMDBElementId CMDBElement::ElementId() const
/* 
Gets the ElementId

@publishedAll
*/
    {
    return iElementId;
    }


EXPORT_C TMDBElementId CMDBElement::TypeId() const
/* 
Gets the TypeId (Table and Column id) from the element id

@publishedAll
*/
    {
    return iElementId & KCDMaskShowType;
    }


EXPORT_C TMDBElementId CMDBElement::TableId() const
/* 
Gets the TableId from the element id

@publishedAll
*/
    {
    return iElementId & KCDMaskShowRecordType;
    }


EXPORT_C TInt CMDBElement::RecordId() const
/* 
Gets the RecordId from the element id

@publishedAll
*/
    {
    return ((ElementId() & KCDMaskShowRecordId) >> 8);
    }
    
    
EXPORT_C TMDBAttributeFlags CMDBElement::Attributes() const
/* 
Gets the Attributes

@publishedAll
*/
    {
    return iElementId & (KCDMaskShowAttributes);
    }

    
EXPORT_C void CMDBElement::SetAttributes(TMDBAttributeFlags aAttributes)
/* 
Sets the given attributes
Access control attribute settings may not be effective.  
This depends on platform security capability in client

@publishedAll
*/
    {
    if((iElementId & KCDMaskShowAttributes) != (aAttributes & KCDMaskShowAttributes))
    	{
	    iElementId |= (aAttributes & KCDMaskShowAttributes);
	    iElementId |= KCDChangedFlag;
    	}
    
    /*
      if field or column, do nothing more
      if recordset, propegate to all records in recordset
      if record, propagate to all fields in record
    */
    if ((iElementId & KCDMaskShowColumnTypeAndRecordId) == KCDMaskShowColumnTypeAndRecordId) // RecordSet
	{
	CMDBRecordSetBase *rs = reinterpret_cast<CMDBRecordSetBase*>(this);
	for (TInt i = 0; i < rs->iRecords.Count(); i++)
	    rs->iRecords[i]->SetAttributes(aAttributes);
	}
    /**
     * This currently does not work
     else if ((iElementId & KCDMaskShowFieldType) == KCDMaskShowFieldType
	     && (iElementId & KCDMaskShowRecordId) != KCDMaskShowRecordId ) // Record
	{
	TMetaVTableIterator attribIter(this);
	SVDataTableEntry const* entry = NULL;
	while ( (entry = attribIter++) != NULL )
	    {
	    CMDBElement* elementPtr = reinterpret_cast<CMDBElement*>(GetAttribPtr(entry->iOffset));
	    
	    if (elementPtr != NULL && elementPtr->ElementId() != 0)
	    elementPtr->SetAttributes(aAttributes);
		}
	    }
    */
    }

    
EXPORT_C void CMDBElement::ClearAttributes(TMDBAttributeFlags aAttributes)
/* 
Clears the given attributes 
Access control attribute settings may not be effective.  
This depends on platform security capability in client

@publishedAll
*/
    {
    iElementId &= ~aAttributes;

    /*
      if field, do nothing more
      if recordset, propegate to all records in recordset
      if record, propagate to all fields in record
    */
    if ((iElementId & KCDMaskShowColumnTypeAndRecordId) == KCDMaskShowColumnTypeAndRecordId) // RecordSet
	{
	CMDBRecordSetBase *rs = reinterpret_cast<CMDBRecordSetBase*>(this);
	for (TInt i = 0; i < rs->iRecords.Count(); i++)
	    rs->iRecords[i]->ClearAttributes(aAttributes);
	}
    /**
     * This currently does not work
    else if ((iElementId & KCDMaskShowFieldType) == KCDMaskShowFieldType
	     && (iElementId & KCDMaskShowRecordId) != KCDMaskShowRecordId ) // Record
	{
	TMetaVTableIterator attribIter(this);
	SVDataTableEntry const* entry = NULL;
	while ( (entry = attribIter++) != NULL )
	    {
	    CMDBElement* elementPtr = reinterpret_cast<CMDBElement*>(GetAttribPtr(entry->iOffset));
	    
	    if (elementPtr != NULL && elementPtr->ElementId() != 0)
		elementPtr->ClearAttributes(aAttributes);
	    }
        }
    */
    }

        
EXPORT_C TBool CMDBElement::IsSetAttribute(TMDBAttributeFlags aAttributes) const
/* 
Checks whether the given attribute is set

@publishedAll
*/
    {
    if ( (iElementId & (aAttributes & KCDMaskShowAttributes)) == (aAttributes & KCDMaskShowAttributes) )
        return ETrue;

    return EFalse;
    }    


EXPORT_C void CMDBElement::SetTypeId(TMDBElementId aElementId) 
/* 
Sets the TypeId

@publishedAll
*/
    {
    iElementId = (iElementId & ~KCDMaskShowType) | (aElementId & KCDMaskShowType);
    }



EXPORT_C void CMDBElement::SetRecordId(TInt aRecordId)
/* 
Sets the RecordId

@publishedAll
*/
    {
    if (aRecordId == (TInt)KCDNewRecordRequest)
        {
        iElementId = (iElementId & ~KCDNewRecordRequest & ~KCDMaskShowRecordId) | KCDNewRecordRequest;
        }
    else if(aRecordId <= (KCDMaskShowRecordId >> 8))
    	{
    	iElementId = (iElementId & ~KCDMaskShowRecordId) | ((aRecordId << 8) & KCDMaskShowRecordId);	
    	}
    else 
        { // assume this is a full element id
        if (0 != (aRecordId & KCDMaskShowRecordType))
        	{
        	/* This is a quick check here. If someone enters 257 (0x00000101) as a recordId 
        	 * commsdat would interpret this a valid elementId without this check. 
        	 * However in this case the table id is 0, which means the elementId is not valid,
        	 * so no reocrdId is set.
        	 */
        	iElementId = (iElementId & ~KCDMaskShowRecordId) | (aRecordId & KCDMaskShowRecordId);
        	}
    	}
    }


EXPORT_C void CMDBElement::SetElementId(TMDBElementId aElementId)
/* 
Sets the ElementId

@publishedAll
*/
    {
    iElementId = aElementId;
    }
        
EXPORT_C TBool CMDBElement::IsNull() const
/* 
Checks if the field value is obtained from the database

@publishedAll
*/
    {
	if(ElementId() & KCDNotNullFlag)
		{
		return EFalse;
		}
	else
		{
		return ETrue;
		}   
    }

EXP_START_ATTRIBUTE_TABLE(CMDBElement, KCDTIdMDBElement,  KCDTIdMDBElement )
END_ATTRIBUTE_TABLE()
            



//
//  CMDBTextFieldBase
//

/*
Constructor setting Field's ElementId
    @publishedAll
*/
EXPORT_C CMDBTextFieldBase::CMDBTextFieldBase()
    : CMDBElement(0)
    {
    }

EXPORT_C CMDBTextFieldBase::CMDBTextFieldBase(TMDBElementId aFieldId) 
    : CMDBElement(aFieldId) 
    {
    }

EXPORT_C CMDBTextFieldBase::~CMDBTextFieldBase()
    {
    delete reinterpret_cast<HBufC*>(iValue);
    }


EXPORT_C CMDBTextFieldBase& CMDBTextFieldBase::operator=(const TDesC& aValue)
    {
    ASSERT(Type()==EText);
	if ( iValue && reinterpret_cast<HBufC*>(iValue)->Length() >= aValue.Length() )
	    {
	    HBufC* buf = reinterpret_cast<HBufC*>(iValue);
		*buf = aValue; 
	        	
		iElementId |= (KCDChangedFlag | KCDNotNullFlag);
	    }
	
	return *this;
    }


EXPORT_C void CMDBTextFieldBase::SetMaxLengthL(TInt aLength)
// create a new buffer if necessary.
// but don't reset the length once it is fixed
    {
    ASSERT(Type()==EText);
	if ( ! iValue || reinterpret_cast<HBufC*>(iValue)->Length() <= aLength )
	    {
	    delete reinterpret_cast<HBufC*>(iValue);
	    iValue=NULL;
		HBufC* tmp = HBufC::NewMaxL(aLength);
		//TPtr forZeroing(tmp->Des());
		//forZeroing.FillZ(aLength);
		iValue = tmp; 
        }
    }
	
	
EXP_START_ATTRIBUTE_TABLE(CMDBTextFieldBase, KCDTIdMDBTextFieldBase,  KCDTIdMDBTextFieldBase )
    X_REGISTER_ATTRIBUTE(CMDBTextFieldBase, iElementId, TMDBTextBase)
END_ATTRIBUTE_TABLE()



//
//  CMDBBinFieldBase
//

    /*
    Constructor setting Field's ElementId
        @publishedAll
    */
EXPORT_C CMDBBinFieldBase::CMDBBinFieldBase()
    : CMDBElement(0)
    {
    }

EXPORT_C CMDBBinFieldBase::CMDBBinFieldBase(TMDBElementId aFieldId) 
    : CMDBElement(aFieldId) 
    {
    }

EXPORT_C CMDBBinFieldBase::~CMDBBinFieldBase()
    {
    delete reinterpret_cast<HBufC8*>(iValue);
    }


EXPORT_C void CMDBBinFieldBase::SetMaxLengthL(TInt aLength)
// create a new buffer if necessary.
    {
    ASSERT(Type()==EDesC8);
	if ( ! iValue || reinterpret_cast<HBufC8*>(iValue)->Length() <= aLength )
	    {
	    delete reinterpret_cast<HBufC8*>(iValue);
		iValue = HBufC8::NewMaxL(aLength);
	    }
    }

EXPORT_C CMDBBinFieldBase& CMDBBinFieldBase::operator=(const TDesC8& aValue)
    {
    ASSERT(Type()==EDesC8);
	if ( iValue && reinterpret_cast<HBufC8*>(iValue)->Length() >= aValue.Length() )
	    {
	    HBufC8* buf = reinterpret_cast<HBufC8*>(iValue);
		*buf = aValue; 
	        	
		iElementId |= (KCDChangedFlag | KCDNotNullFlag);
	    }
	
	return *this;
    }


	
EXP_START_ATTRIBUTE_TABLE(CMDBBinFieldBase, KCDTIdMDBBinFieldBase,  KCDTIdMDBBinFieldBase )
    X_REGISTER_ATTRIBUTE(CMDBBinFieldBase, iElementId, TMDBBinBase)
END_ATTRIBUTE_TABLE()



//
//  CMDBNumFieldBase
//

    /*
    Constructor setting Field's ElementId
        @publishedAll
    */
    EXPORT_C CMDBNumFieldBase::CMDBNumFieldBase()
        : CMDBElement(0)
        {
        }

    EXPORT_C CMDBNumFieldBase::CMDBNumFieldBase(TMDBElementId aFieldId) 
        : CMDBElement(aFieldId) 
        {
        }

    /*
	Assignment operator for field value

	@publishedAll
	*/
	EXPORT_C CMDBNumFieldBase& CMDBNumFieldBase::operator=(const TInt aValue)
		{
	    ASSERT(Type()==EInt | Type()==ELink);
		iValue = reinterpret_cast<TAny*>(aValue);
		
		iElementId |= (KCDChangedFlag | KCDNotNullFlag);
        
		return *this;
		}


    EXPORT_C void CMDBNumFieldBase::SetMaxLengthL(TInt /*aLength*/)
	    {
		ASSERT(Type()==EInt);
		}


    TMDBElementId CMDBNumFieldBase::GetL()
        // does not leave - just leaving for forms sake
	{
		return reinterpret_cast<TInt&>(iValue);
	}
EXP_START_ATTRIBUTE_TABLE(CMDBNumFieldBase, KCDTIdMDBNumFieldBase,  KCDTIdMDBNumFieldBase )
    X_REGISTER_ATTRIBUTE(CMDBNumFieldBase, iElementId, TMDBNumBase)
END_ATTRIBUTE_TABLE()


//
// CMDBRecordLinkBase
//
    
/*
Constructor/Destructor

@publishedAll
*/
    
EXPORT_C CMDBRecordLinkBase::CMDBRecordLinkBase()
    {
    }

EXPORT_C CMDBRecordLinkBase::CMDBRecordLinkBase(TMDBElementId aLinkingFieldId) 
    : CMDBNumFieldBase(aLinkingFieldId)
    {
    }

EXPORT_C CMDBRecordLinkBase::CMDBRecordLinkBase(TMDBElementId aLinkingFieldId, CMDBElement* aLinkedRecord) 
    : CMDBNumFieldBase(aLinkingFieldId | KCDChangedFlag), iLinkedRecord(aLinkedRecord)
    {
    }
    
    
EXPORT_C CMDBRecordLinkBase::~CMDBRecordLinkBase()
    {   
    delete iLinkedRecord; 
    }
   

EXPORT_C CMDBRecordLinkBase& CMDBRecordLinkBase::operator=(const TMDBElementId aValue) 
    /*
    Assignment operator for field value
    which sets the id of the element list
    
    @publishedAll
    */
    {
    iValue = (TAny*)aValue;
    
    iElementId |= (KCDChangedFlag | KCDNotNullFlag);
            
    return *this;
    }
         

EXPORT_C CMDBRecordLinkBase::operator TMDBElementId()
/*
Conversion operator for field value
which gets the id of the element list

@publishedAll
*/
    { 
    return (TMDBElementId&)iValue; 
    }


EXP_START_ATTRIBUTE_TABLE(CMDBRecordLinkBase, KCDTIdMDBRecordLinkBase,  KCDTIdMDBRecordLinkBase )
    X_REGISTER_ATTRIBUTE(CMDBRecordLinkBase,   iLinkedRecord, TMDBLink)
END_ATTRIBUTE_TABLE()



//
// CMDBRecordSetBase
//
 
    EXPORT_C CMDBRecordSetBase::CMDBRecordSetBase()
        {
        } 
        
    EXPORT_C CMDBRecordSetBase::CMDBRecordSetBase(TMDBElementId aElementId) 
        : CMDBTextFieldBase((aElementId & ( KCDMaskShowAttributes | KCDMaskShowRecordType )) | KCDMaskShowFieldType | KCDMaskShowRecordId ) 
        {
        }
        
    EXPORT_C CMDBRecordSetBase::~CMDBRecordSetBase()
        { 
            iRecords.ResetAndDestroy();
        }

EXP_START_ATTRIBUTE_TABLE(CMDBRecordSetBase, KCDTIdMDBRecordSetBase,  KCDTIdMDBRecordSetBase )
    X_REGISTER_ATTRIBUTE(CMDBRecordSetBase,   iRecords,  TMDBRecordSet)
END_ATTRIBUTE_TABLE()




//
//  CMDBRecordBase
//


TBool  MatchTypeInfoByName(const SGenericRecordTypeInfo& aTypeInfo, const SGenericRecordTypeInfo& aAnotherTypeInfo)

//Utility function to test match of field ID in two fields.
//@internalTechnology

{
    if(aTypeInfo.iTypeName.CompareF(aAnotherTypeInfo.iTypeName) == 0 )
        {
           return ETrue; 
        }
              
    return EFalse;
}
 

CMDBElement* GetFieldByNameHelperL(TInt& aValType,
                                   const SRecordTypeInfo* aRecordInfo,
                                   CMDBRecordBase* aRecordBase)
    {
    /* Mask is here because only want to return basic type info, 
    not extended info about record links etc */
    aValType = aRecordInfo->iValType & KCDShowBasicTypeInfo;
    
    //This is an experiment!!!!!!
    
    // HACK TO SAVE NIFMAN - FIX PROPERLY IN NIFMAN AND REMOVE
    if (aValType == ELink)
        {
        aValType = EInt;
        }

    //
    return aRecordBase->GetFieldByIdL(aRecordInfo->iTypeId);
    }


EXPORT_C CMDBElement* CMDBRecordBase::GetFieldByNameL(const TPtrC& aFieldName, TInt& aValType) 
    {
    const SRecordTypeInfo* recordInfo = GetRecordInfo();
	
    if (recordInfo)
        {
        while( recordInfo->iTypeId != 0)
            {
           TPtrC one(recordInfo->iTypeName);
           if (one.CompareF(aFieldName) == 0 )
                {
                return GetFieldByNameHelperL(aValType, recordInfo, this);
                }
           	
            ++recordInfo;
            }
        }
          
    User::Leave(KErrNotFound);
    
    return NULL; //Never get here
    }


 EXPORT_C const SRecordTypeInfo* CMDBRecordBase::GetRecordInfo()
    {
    // Meaningless in record base class, but don't want an abstract class.
    return NULL;
    }
 
 
 EXPORT_C CMDBElement* CMDBRecordBase::GetFieldByIdL(TMDBElementId aId)
    { 
    TMetaVTableIterator attribIter(this);
    SVDataTableEntry const* entry = NULL;
    while ( (entry = attribIter++) != NULL )
        {
	    CMDBElement* fieldPtr = reinterpret_cast<CMDBElement*>(GetAttribPtr(entry->iOffset));

		if ( ( fieldPtr != NULL )  &&  
			 ( fieldPtr->ElementId() != 0 ) && 
			 ( fieldPtr->ElementId() & KCDMaskShowFieldType ) == (aId & KCDMaskShowFieldType ))
			{
            return fieldPtr;
            }
        }

    User::Leave(KErrNotFound);

    return NULL; //Never get here
    }



//
// CMDBUserDefinedRecord
//


/*EXPORT_C CMDBGenericRecord* CMDBGenericRecord::NewLC(TMDBElementId aId)

NewLC

@publishedAll
*
    {
	CMDBGenericRecord* r = new (ELeave) CMDBGenericRecord(aId);
	CleanupStack::PushL(r);
	return r;
    }*/


EXPORT_C CMDBGenericRecord::~CMDBGenericRecord()
    { 
    // I don't understand why it's necessary to Close() then ResetAndDestroy()
    iFields.ResetAndDestroy();
    iFields.Close();
    iRecordInfo.Close();
    }


EXPORT_C CMDBElement* CMDBGenericRecord::GetFieldByNameL(const TPtrC& aFieldName, TInt& aValType ) 
    {
    
    if (iRecordInfo.Count() == 0)
        {
        User::Leave(KErrNotReady);
        }
        
    SGenericRecordTypeInfo typeInfo(0, 0, ENoAttrs, aFieldName);        
    
    TInt loc = iRecordInfo.Find(typeInfo, MatchTypeInfoByName);
    
    User::LeaveIfError(loc);
           
    aValType = iRecordInfo[loc].iValType;
   
    return GetFieldByIdL(iRecordInfo[loc].iTypeId);
    }


TBool  MatchFieldByElementId(const CMDBElement& aField, const CMDBElement& aAnotherField)
// @internalTechnology
// Utility function to test match of field ID in two fields.
    {
    return ((aField.ElementId() & KCDMaskShowType) == (aAnotherField.ElementId() & KCDMaskShowType));
    }
    

TBool MatchTypeInfoById(const SGenericRecordTypeInfo& aTypeInfo, const SGenericRecordTypeInfo& aAnotherTypeInfo)

//Utility function to test match of field ID in two fields.
//@internalTechnology

	{
    return (aTypeInfo.iTypeId == aAnotherTypeInfo.iTypeId);
	}


EXPORT_C CMDBElement* CMDBGenericRecord::GetFieldByIdL(TMDBElementId aId)
    { 
    CMDBField<TInt> field(aId | (ElementId() & KCDMaskShowRecordType));
     
    TInt ret = iFields.Find(&field, MatchFieldByElementId);
    
    if ( ret != KErrNotFound )
   		{
    	return (CMDBElement*)iFields[ret];
    	}
    else
        {
        SGenericRecordTypeInfo typeInfo((aId & KCDMaskShowFieldType), 0, ENoAttrs);      
    	TInt index = iRecordInfo.Find(typeInfo, MatchTypeInfoById);
    	
        User::LeaveIfError(index);
        
    	CMDBElement* newField = NULL;
    	
    	//todo this mask got to be outta there somewhere..find it..
    	TMDBElementId tableAndRecordId = (ElementId() & KCDMaskShowRecordType) | (ElementId() & KCDMaskShowRecordId) ;
    	
        switch (iRecordInfo[index].iValType)
            {
            case EText : 
            case EMedText :
            case ELongText :  
                {
                newField = new (ELeave) CMDBField<TDesC>(aId | tableAndRecordId); 
                break;           
                }
                
            case EDesC8 : 
                {
                newField = new (ELeave) CMDBField<TDesC8>(aId | tableAndRecordId);
                break;
                }
                
            case ELink : 
                {
                newField = new (ELeave) CMDBRecordLinkBase(aId | tableAndRecordId);
                break;
                }
                
            case EUint32 :
                {
                newField = new (ELeave) CMDBField<TUint32>(aId | tableAndRecordId);
                break;
                }

            case EInt  :
                {
                newField = new (ELeave) CMDBField<TInt>(aId | tableAndRecordId);
                break;
                }
                
            case EBool :
                {
                newField = new (ELeave) CMDBField<TBool>(aId | tableAndRecordId);
                break;
                }

            default : 
                {
                User::Leave(KErrNotFound);
                break;
                }
            }
        iFields.AppendL(newField);
        return newField;          
        }
    }


EXPORT_C void CMDBGenericRecord::InitializeL(const TDesC& aTableName, const SGenericRecordTypeInfo*  aNewTableInfo)
    {
    
    if (iRecordInfo.Count() > 0)
        {
        // This object has already been initialised once before.
      	User::Leave(KErrInUse);
        }

    iRecordType.Copy(aTableName);
    
    if (aNewTableInfo != NULL)
        {
        // Loop through the input record info table and add...
    	while ( aNewTableInfo->iTypeId != 0 )
            {
            iRecordInfo.AppendL(*aNewTableInfo++);
            }
        }
    }



/*EXPORT_C void CMDBGenericRecord::SetFieldInfoL(	     RGenericRecordTypeInfoArray* const aContainer, 
                                               const SGenericRecordTypeInfo*            aInitialInfo,
                                                     TBool                              aAppend)
    {
    // Validate the inputs.
    if (!aContainer)  
        {
        User::Leave(KErrArgument);
        }

    if (!iRecordInfo)
        {
        iRecordInfo = aContainer;
        }
        
    if (aInitialInfo != NULL)
        {        
        if (!aAppend)
            {
            // Purge any existing contents.
            iRecordInfo.Reset();
            }

        // Loop through the input record info table and add...
    	while ( aInitialInfo->iTypeId != 0 )
            {
            iRecordInfo.Append(aInitialInfo++);
            }
        }
    }*/



EXP_START_ATTRIBUTE_TABLE(CMDBGenericRecord, KCDTIdMDBGenericRecord,  KCDTIdMDBGenericRecord)
    X_REGISTER_ATTRIBUTE(CMDBGenericRecord,   iFields,  TMDBGenericRecord)
END_ATTRIBUTE_TABLE()
 
//EOF
