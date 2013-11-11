/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file MetaDatabaseVisitor.h
 @internalComponent
*/
 
#if (!defined METADATABASEVISITOR_H)
#define       METADATABASEVISITOR_H


namespace CommsDat
/*
Namespace encapsulates all types that are able to perform MMetaDatabase actions on specific record data types
*/
    {

using namespace CommsDatInternal;
using namespace Meta;



NONSHARABLE_CLASS(TMDBVisitorBase) : public Meta::MMetaType
/**
A general database visitor 
@internalComponent
*/
    {  
public:

    virtual ~TMDBVisitorBase();

    ////////// From Meta::MMetaType /////////////////////////////////////////
       TInt Load(TPtrC8& aBuffer){ aBuffer == aBuffer; return 0; }
       TInt Store(TDes8& aBuffer) const{aBuffer == aBuffer; return 0; }
       void Copy(const TAny* aData)  {aData = aData;}
       TInt Length() const {return 0;}
   	///////////////////////////////////////////////////////////////////
    
    // GET
    virtual TInt LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags) = 0; 
    virtual TInt LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags, RArray<TUint32>& aIds) = 0;
    virtual TInt FindL(CMDBSessionImpl* aSession, RArray<TUint32>& aIds) = 0;
    virtual TInt RefreshL(CMDBSessionImpl* aSession) = 0;
	virtual TInt GetL(CMDBSessionImpl* aSession);
   
    // SET
    virtual TInt StoreL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags) = 0;
    virtual TInt ModifyL(CMDBSessionImpl* aSession, TMDBAttributeFlags aAttributeFlags) = 0;
 
    virtual void SetOwner(CMDBElement* aOwner);
    virtual TInt Changed() const = 0;  
    virtual void Sync();
    virtual void ClearValue(){}

public:
    
    // These do not need to be specialised in inheriting templates
    void Change();
    void FieldLoaded();
    TBool IsFieldLoaded();
    void SetFieldNull();
    
	TUint32 TypeId();
    TUint32 ElementId();
    void SetElementId(TMDBElementId* aId);
    void SetElementId(TMDBElementId aId);
       
    void SetMaxLength(TInt aLength);

    /* Removes all of the flags from the elementId of the DBVisitor - use only if the given record
     * which needs to be loaded is NOT a template record!! Doesn't have to be virtual.
     */
    void ResetElementId(TInt aRecId);
    
protected:

    TMDBVisitorBase(TMDBElementId *aElementId);
    TMDBVisitorBase();
    
    void FindRecordsUsingTemplateL(CMDBSessionImpl* aSession, RArray<TUint32>& aRecordIds, RArray<TUint32>& aCandidateIds);

    TInt            iMaxLength;  
    CMDBElement*    iOwner; 
    
    TMDBElementId*  iElementId;
    };




////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TMDBVisitor
// 
/**
Generic visitor for any simple MetaDatabase element.
Class is specialised to act on fields of type TMDBMetaField<T>
where T is 
    TInt    - (for any number - reals not used in comms database)
    HBufC   - for any string
    HBufC8  - for binary data
*/


/*Simple encapsulation of a metadatabase element for use by visitors
@internalComponent
*/
template<class TYPE> 
struct SMDBMetaElement 
    {
    TMDBElementId   iElementId;
    TYPE            iValue;
    };

/*
Implementation of MMetaDBVisitor for a general type 
@internalComponent
*/


template <class TYPE> 
class TMDBVisitor : public TMDBVisitorBase
/**
Implementation of MMetaDBVisitor for a general element type 
@internalComponent
*/
    {    
public:

    inline static TMDBVisitor<TYPE>* NewL(const TAny* aMem, const TAny* aData);
    inline static TMDBVisitor<TYPE>* CreateL(const TAny* aMem, const TAny* aData);

    // GET
    TInt LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags); 
    TInt LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags, RArray<TUint32>& aIds);
    TInt FindL(CMDBSessionImpl* aSession, RArray<TUint32>& aIds);
    TInt RefreshL(CMDBSessionImpl* aSession);
	TInt GetL(CMDBSessionImpl* aSession);
   
    // SET
    TInt StoreL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags);
    TInt ModifyL(CMDBSessionImpl* aSession, TMDBAttributeFlags aAttributeFlags);  
    
	TInt Changed() const;
    //void SetOwner(CMDBElement* aOwner);
    void ClearValue();

public:
    
    SMDBMetaElement<TYPE>* iTarget;
    
protected:

    TMDBVisitor(SMDBMetaElement<TYPE>* aData);
    };



template <class TYPE> 
inline TMDBVisitor<TYPE>::TMDBVisitor(SMDBMetaElement<TYPE>* aData)
: TMDBVisitorBase(&(aData->iElementId)), iTarget(aData)
/**
 * Constructor
 */
    {
    }



template<class TYPE> 
inline TMDBVisitor<TYPE>* TMDBVisitor<TYPE>::NewL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a Visitor of a particular type.
 * Used for attribute registration (in the data v-table).
 */
    {
    return static_cast<TMDBVisitor<TYPE>*>( ::new ((TUint8*)aMem) TMDBVisitor( (SMDBMetaElement<TYPE>*)(aData) ) );
    }

template<class TYPE> 
inline TMDBVisitor<TYPE>* TMDBVisitor<TYPE>::CreateL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a Visitor of a particular type.
 * Used for attribute registration (in the data v-table).
 */
    {
    return static_cast<TMDBVisitor<TYPE>*>( ::new ((TUint8*)aMem) TMDBVisitor( (SMDBMetaElement<TYPE>*)(((CMDBElement*)aData)->Data()) ) );
    }


template <class TYPE> 
inline TInt TMDBVisitor<TYPE>::Changed() const
/*
Has the value been changed by the caller?

@internalComponent
*/  { 
    if (iElementId)
        {
        return ( *iElementId & KCDChangedFlag );
    	}
    return 0;
    } 


template<class TYPE>
inline void TMDBVisitor<TYPE>::ClearValue() 
    {
    if (iTarget)
        {
        iTarget->iValue = 0;
        SetFieldNull();
        }
    }


template<>
inline void TMDBVisitor<HBufC16*>::ClearValue() 
    {
    if (iTarget)
        {
        delete iTarget->iValue;
        iTarget->iValue = 0;
        SetFieldNull();
        }
    }


template<>
inline void TMDBVisitor<HBufC8*>::ClearValue() 
    {
    if (iTarget)
        { 
        delete iTarget->iValue;
        iTarget->iValue = 0;
        SetFieldNull();
        }
    }


template<class TYPE> 
inline TInt TMDBVisitor<TYPE>::RefreshL(CMDBSessionImpl* aSession) 
	{
	return LoadL(aSession,*iElementId,*iElementId);
	}


template<class TYPE> 
inline TInt TMDBVisitor<TYPE>::GetL(CMDBSessionImpl* aSession) 
/*
Assumes fully resolved element Id but still needs mask for reserved bits

@internalComponent
*/
    {
    if (! iTarget )
        {
        User::Leave(KErrNotReady);
        }

    TUint32 id = ElementId() & ~KCDMaskShowRes;

    TInt err = aSession->StorageL()->Get(id, iTarget->iValue);
     
    if(err == KErrNone)
		{
		FieldLoaded(); // value has been retrieved
		}
    else 
        {
        Sync(); // indicates the field is still null.  
        
        if (err != KErrNotFound && err != KErrPermissionDenied)
            {
            __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("TMDBVisitor<TYPE> GetL() storage server Get returned err = %d for id %08x"), err, id);
            User::Leave(err);
            }
        }
	return err;
    }



template<> 
inline TInt TMDBVisitor<HBufC16*>::GetL(CMDBSessionImpl* aSession) 
/*
Specialisation for HBufC16 
Assumes fully resolved element Id but still needs mask for reserved bits

@internalComponent
*/
	{
	if (! iTarget )
        {
        User::Leave(KErrNotReady);
        }

    if( ! iTarget->iValue  || 
		iTarget->iValue->Length() < iMaxLength)
		{
		delete iTarget->iValue;
        iTarget->iValue = NULL;
		iTarget->iValue = HBufC16::NewMaxL(iMaxLength);
		}

	TPtr16 ptr = iTarget->iValue->Des();
	TUint32 id = ElementId() & KCDMaskHideRes;

    TInt err = aSession->StorageL()->Get(id, ptr);   
    
    if(err == KErrNone)
		{
		FieldLoaded(); // value has been loaded

		HBufC16* temp = iTarget->iValue;
		iTarget->iValue = HBufC16::NewMaxL(ptr.Length());
		TPtr16 ptrNew = iTarget->iValue->Des();;
		ptrNew = ptr;
		delete temp;
		}
    else 
        {
        delete iTarget->iValue;
        iTarget->iValue = NULL;
        
        Sync(); // indicates the field is null.  

        if (err != KErrNotFound  &&  err != KErrPermissionDenied)
            {
            __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("TMDBVisitor<HBufC16*> GetL() storage server Get returned err = %d for id %08x"), err, id);
            User::Leave(err);
            }
        }
		
    return err;
	}



template<> 
inline TInt TMDBVisitor<HBufC8*>::GetL(CMDBSessionImpl* aSession) 
/*
Specialisation for HBufC8*
Assumes fully resolved element Id but still needs mask for reserved bits

@internalTechnology
*/
    {
    if (! iTarget )
        {
        User::Leave(KErrNotReady);
        }

    if (! iTarget->iValue || 
		iTarget->iValue->Length() < iMaxLength)
		{
		delete iTarget->iValue;
		iTarget->iValue = NULL;
		iTarget->iValue = HBufC8::NewMaxL(iMaxLength);
		}
	
	TPtr8 ptr = iTarget->iValue->Des();
    TUint32 id = ElementId() & KCDMaskHideRes;

	TInt err = aSession->StorageL()->Get(id, ptr);    
     
    if(err == KErrNone)
		{
		FieldLoaded();// Value was retrieved

        /*
		HBufC8* temp = iTarget->iValue;
		iTarget->iValue = HBufC8::NewMaxL(ptr.Length());
		TPtr8 ptrNew = iTarget->iValue->Des();;
		ptrNew = ptr;
		delete temp;
        */
		}
    else 
        {
        delete iTarget->iValue;
        iTarget->iValue = NULL;
        
        Sync(); // indicates the field is null.  

        if (err != KErrNotFound && err != KErrPermissionDenied)
            {
            __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("TMDBVisitor<HBufC8*> GetL() storage server Get returned err = %d for id %08x"), err, id);
            User::Leave(err);
            }
        }
		
	return err;
    }



template<class TYPE> 
inline TInt TMDBVisitor<TYPE>::LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags /*aAttributeFlags*/) 
/*
Assumes type and record id set by caller

@internalComponent
*/
    {
    if (! iTarget)
        {
        User::Leave(KErrNotReady);
        }

    // set the element id to the full id set by the caller
    iTarget->iElementId = (iTarget->iElementId & (KCDMaskShowType | KCDMaskShowRes)) | (aRecordId & KCDMaskShowRecordId);  

    RArray<TUint32> ids;
	CleanupClosePushL(ids);

	TUint32 mask = KCDMaskShowType | KCDMaskShowRecordId | aSession->GetReadAttributeMask();

	TUint32 id = ElementId() & KCDMaskHideAttrAndRes;

    TInt err = (aSession->StorageL())->FindL(id, mask, ids);  
     
    if ( err != KErrNone  &&  err != KErrNotFound )
        {
        __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("TMDBVisitor<TYPE> LoadL() storage server FindL returned err = %d when looking for %08x, masked with %08x"), err, id, mask);
        }
#ifdef __DEBUG    
    else if ( ids.Count() > 1 )
        {
        __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("TMDBVisitor<TYPE> LoadL() storage server FindL/FindEqL %d fields found for %08x.  Should be 1 or 0."), aIds.Count(), id & mask);
        }
#endif
  
    // Load the field data
    if (err == KErrNone)
        {
	    // Set the full id 
		iTarget->iElementId = ids[0];
		err = GetL(aSession); 		
        }

	CleanupStack::PopAndDestroy(&ids);
    if ( err == KErrNotFound )
        {
        // it's ok not to find a field.  
        // Don't report that error back.
       // VCT - experimenting without this err = KErrNone;
        }

	return err;
    }



template<class TYPE> 
inline TInt TMDBVisitor<TYPE>::LoadL(CMDBSessionImpl* aSession, TMDBElementId /*aRecordId*/, TMDBAttributeFlags /*aAttributeFlags*/,RArray<TUint32>& aIds ) 
/*
Assumes type and record id set by caller

@internalComponent
*/
    {
    if (aIds.Count() == 0)
        {
        User::Leave(KErrNotFound);
        }

    return LoadL(aSession, aIds[0] & KCDMaskShowRecordId, aIds[0] & KCDMaskShowAttributes);
    }



template <class TYPE> 
inline TInt TMDBVisitor<TYPE>::FindL(CMDBSessionImpl* aSession, RArray<TUint32>& aIds) 
/*
Only find dont actually load
Assumes fully resolved element Id but still needs mask for reserved bits

@internalComponent
*/
    {
    if (! iTarget )
        {
        User::Leave(KErrNotReady);
        }

    TInt err(KErrNotFound);

    // want to find this field in any record of this type that has appropriate attributes
    // so mask out recordid, reserved bits and any attributes that need not be observed.
    // TUint32 mask = KCDMaskShowType | (~aAttributeFlags & KCDMaskShowAttributes);
    TUint32 mask = KCDMaskShowType | aSession->GetReadAttributeMask();
    
    if (Changed())
        {
        err = aSession->StorageL()->FindEqL(ElementId() & ~aSession->GetReadAttributeMask(), mask, iTarget->iValue, aIds);
    
        RArray<TUint32> ids;
        CleanupClosePushL(ids);
        
        mask = (mask & ~ECDHidden) | KCDMaskShowRecordId;

        // Now look in template record to find other records
        err = aSession->StorageL()->FindEqL(ElementId() & ~KCDMaskShowRecordId, mask, iTarget->iValue, ids);
                
        if ( err == KErrNone )
            {
            FindRecordsUsingTemplateL(aSession, ids, aIds);
            }

        if ( aIds.Count() )
            {
            err = KErrNone;
            }
    
        CleanupStack::PopAndDestroy(&ids);
    }
    else
        {  
        err = aSession->StorageL()->FindL(ElementId() & ~aSession->GetReadAttributeMask(), mask, aIds);  
        }

    if ( aIds.Count() == 0)
        {
        __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("TMDBVisitor<TYPE> FindL() storage server FindL returned err = %d when looking for %08x, masked with %08x"), err, ElementId(), mask);
        if (err == KErrNone || err == KErrNotFound)
            {
            __FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("TMDBVisitor<TYPE> FindL() storage server FindL() did not find <%08x> masked with <%08x>"), ElementId(), mask);
            err = KErrNotFound;
            }
        else
            {
            __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("TMDBVisitor<TYPE> FindL() storage server FindL returned err = %d when looking for <%08x>, masked with <%08x>"), err, ElementId(), mask);
            }
        }
#ifdef __DEBUG
    else if ( aIds->Count() > 1)
        {
        __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("TMDBVisitor<TYPE> FindL() storage server FindL/FindEqL %d fields found for id %08x.  Should be 1 or 0."), aIds.Count(), id & mask);
        }
#endif

    return err;
    }



template <> 
inline TInt TMDBVisitor<HBufC16*>::FindL(CMDBSessionImpl* aSession, RArray<TUint32>& aIds) 
/*
Only find dont actually load
Assumes fully resolved element Id but still needs mask for reserved bits

@internalComponent
*/
    {
    if (! iTarget || ! iTarget->iValue )
        {
        User::Leave(KErrNotReady);
        }
    
    // want to find this field in any record of this type that has appropriate attributes
    // so mask out recordid, reserved bits and any attributes that need not be observed.
    // TUint32 mask = KCDMaskShowType | (~aAttributeFlags & KCDMaskShowAttributes);
    TUint32 mask = KCDMaskShowType | aSession->GetReadAttributeMask();
    
    TInt err(KErrNotFound);

    if (Changed())
        {
        const TDesC16& val = *iTarget->iValue;
        // An absent text field is treated as being synonymous with one that is present but blank, ie
        // searching for an empty descriptor matches both actual blanks and fields that aren't found. Because
        // CenRep doesn't support such a thing we're forced to get handle this case by brute force: get the set
        // of all record ids, fetch the desired field for each, and remove those records where the field is present
        // and non blank.
       	if(val.Length() == 0)
       		{
       		// Get the set of all recordIds
			TUint32 maskId = KCDMaskHideAttrAndRes & ~KCDColumnTypeInfo;

			TUint32 tableId = ElementId() | KCDMaskShowColumnTypeAndRecordId;

			err = (aSession->StorageL())->FindL(tableId, maskId, aIds);  
			 
			// remove node (side effect from query)
			for (TInt idx = aIds.Count() - 1; idx >= 0; -- idx)
				{
				if( (aIds[idx] & ~KCDMaskShowAttrAndRes) == (tableId & ~KCDMaskShowAttrAndRes) )
					{
					aIds.Remove(idx);
					}
				}

			if ( err != KErrNone  &&  err != KErrNotFound )
				{
				__FLOG_STATIC3(KLogComponent, KCDErrLog, _L("TMDBVisitor<HBufC16*> FindL() storage server FindL returned err = %d when looking for <%08x>, masked with <%08x>"), err, tableId, mask);
				}
			else
				{
				// Fetch the desired field for each record
				RArray<TUint32> fieldIds;
				CleanupClosePushL(fieldIds);

				TUint32 fieldId = (ElementId() & ~KCDMaskShowRecordId);
				maskId = KCDMaskShowType | aSession->GetReadAttributeMask();
				
				TInt errFieldSearch = aSession->StorageL()->FindL(fieldId, maskId, fieldIds);
				if(errFieldSearch == KErrNone)
					{
					for(TInt fieldIdx = fieldIds.Count() - 1; fieldIdx >= 0; --fieldIdx)
						{
						for (TInt idx = aIds.Count() - 1; idx >= 0; -- idx)
							{
							// Remove those records where the field is present and non blank
							if( (fieldIds[fieldIdx] & KCDMaskShowRecordId) == (aIds[idx] & KCDMaskShowRecordId ))
								{
								aIds.Remove(idx);
								}
							}
						}
					}

				CleanupStack::PopAndDestroy(&fieldIds);
				}
       		}
       	else
       		{
       		if((ElementId() & KCDMaskHideRes) == KCDMaskGenericTableName)
       			{
       			mask = KCDMaskGenericTableName;	
       			}
       	
	        err = aSession->StorageL()->FindEqL(ElementId() & ~aSession->GetReadAttributeMask(), mask, val, aIds);

            RArray<TUint32> ids;
            CleanupClosePushL(ids);
        
            mask = (mask & ~ECDHidden) | KCDMaskShowRecordId;

            // Now look in template record
            err = aSession->StorageL()->FindEqL(ElementId() & ~KCDMaskShowRecordId, mask, val, ids);
        
            if ( err == KErrNone )
                {
                FindRecordsUsingTemplateL(aSession, ids, aIds);
                }

            if ( aIds.Count() )
                {
                err = KErrNone;
                }

            CleanupStack::PopAndDestroy(&ids);     
            }
        }
    else
        {  
        err = aSession->StorageL()->FindL(ElementId() & ~aSession->GetReadAttributeMask(), mask, aIds);  
        }
    
    if ( aIds.Count() == 0)
        {
        __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("TMDBVisitor<HBufC16*> FindL() storage server FindL returned err = %d when looking for %08x, masked with %08x"), err, ElementId(), mask);
        if (err == KErrNone || err == KErrNotFound)
            {
            __FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("TMDBVisitor<HBufC16*> FindL() storage server FindL() did not find <%08x> masked with %08x"), ElementId(), mask);
            err = KErrNotFound;
            }
        else
            {
            __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("TMDBVisitor<HBufC16*> FindL() storage server FindL() returned err = %d when looking for <%08x>, masked with <%08x>"), err, ElementId(), mask);
            }
        }       
#ifdef __DEBUG
    else if ( aIds->Count() > 1)
        {
        __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("TMDBVisitor<HBufC16*> FindL() storage server FindL/FindEqL %d fields found for id.  Should be 1 or 0."), aIds.Count(), ElementId() & mask);
        }
#endif
 
    return err;
    }



template <> 
inline TInt TMDBVisitor<HBufC8*>::FindL(CMDBSessionImpl* aSession, RArray<TUint32>& aIds) 
/*
Only find dont actually load
Assumes fully resolved element Id but still needs mask for reserved bits

@internalComponent
*/
    {
    if (! iTarget || ! iTarget->iValue )
        {
        User::Leave(KErrNotReady);
        }

    // want to find this field in any record of this type that has appropriate attributes
    // so mask out recordid, reserved bits and any attributes that need not be observed.
    // TUint32 mask = KCDMaskShowType | (~aAttributeFlags & KCDMaskShowAttributes);
    TUint32 mask = KCDMaskShowType | aSession->GetReadAttributeMask();
    
    TInt err(KErrNotFound);
   
    if (Changed())
        {
        const TDesC8& val = *iTarget->iValue;
        // An absent text field is treated as being synonymous with one that is present but blank, ie
        // searching for an empty descriptor matches both actual blanks and fields that aren't found. Because
        // CenRep doesn't support such a thing we're forced to get handle this case by brute force: get the set
        // of all record ids, fetch the desired field for each, and remove those records where the field is present
        // and non blank.
       	// @TODO - find a cleaner way to achieve this
       	if(val.Length() == 0)
       		{
        	TUint32 recId = (ElementId() & ~KCDMaskShowFieldType) | KCDInitialColumnId;
        	err = aSession->StorageL()->FindL(recId & ~aSession->GetReadAttributeMask(), mask, aIds);
        	if(err == KErrNone)
        		{
        		TBuf8<1> dummyVal;	// doesn't matter if it overflows; mere presence of non-blank is all we seek
        		for(TInt idx = aIds.Count() - 1; idx >= 0; --idx)
        			{
        			TUint32 fldId = (aIds[idx] & ~KCDMaskShowFieldType) | (ElementId() & KCDMaskShowFieldType);
        			TInt lookupErr = aSession->StorageL()->Get(fldId, dummyVal);
        			if(lookupErr == KErrOverflow || (lookupErr == KErrNone && dummyVal.Length() != 0))
        				{
        				// comparison field does exist in this specific record but didn't match "" - remove
        				aIds.Remove(idx);
        				}
        			}
        		}
       		}
       	else
       		{
	        err = aSession->StorageL()->FindEqL(ElementId() & ~aSession->GetReadAttributeMask(), mask, val, aIds);
       	
            RArray<TUint32> ids;
            CleanupClosePushL(ids);
        
            mask = (mask & ~ECDHidden) | KCDMaskShowRecordId;

            // Now look in template record
            err = aSession->StorageL()->FindEqL(ElementId() & ~KCDMaskShowRecordId, mask, val, ids);
                
            if ( err == KErrNone )
                {
                FindRecordsUsingTemplateL(aSession, ids, aIds);
                }
       	    
            if ( aIds.Count() )
                {
                err = KErrNone;
                }

            CleanupStack::PopAndDestroy(&ids);
            }
        }
    else
        {  
        err = aSession->StorageL()->FindL(ElementId() & ~aSession->GetReadAttributeMask(), mask, aIds);  
        }
    
    if ( aIds.Count() == 0)
        {
        __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("TMDBVisitor<HBufC8*> FindL() storage server FindL returned err = %d when looking for %08x, masked with %08x"), err, ElementId(), mask);
        if (err == KErrNone || err == KErrNotFound)
            {
            __FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("TMDBVisitor<HBufC8*> FindL() storage server FindL did not find <%08x> masked with <%08x>"), ElementId(), mask);
            err = KErrNotFound;
            }
        else
            {
            __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("TMDBVisitor<HBufC8*> FindL() storage server FindL returned err = %d when looking for <%08x>, masked with <%08x>"), err, ElementId(), mask);
            }
        }   
#ifdef __DEBUG
    else if ( aIds->Count() > 1)
        {
        __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("TMDBVisitor<HBufC8*> FindL() storage server FindL/FindEqL %d fields found when looking for <%08x>.  Should be 1 or 0."), aIds.Count(), ElementId() & mask);
        }
#endif
 
    return err;
    }



template<class TYPE> 
inline TInt TMDBVisitor<TYPE>::StoreL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags)
/*
 Store the field in the repository
 
 @internalComponent
*/
    {
    if (! iTarget || ! iElementId )
        {
        User::Leave(KErrNotReady);
        }

    *iElementId = (*iElementId & ~KCDMaskShowRecordId) | (aRecordId & KCDMaskShowRecordId) | aAttributeFlags;            
    return aSession->StorageL()->Create( ElementId() & KCDMaskHideRes, iTarget->iValue);
    }



template<> 
inline TInt TMDBVisitor<HBufC16*>::StoreL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags)
/*
 Store the field in the repository
 
 @internalComponent
*/
    {
    if (! iTarget || ! iElementId )
        {
        User::Leave(KErrNotReady);
        }
    
    *iElementId = (*iElementId & ~KCDMaskShowRecordId) | (aRecordId & KCDMaskShowRecordId) | aAttributeFlags;
    return aSession->StorageL()->Create( ElementId() & KCDMaskHideRes, *(iTarget->iValue));
    }



template<> 
inline TInt TMDBVisitor<HBufC8*>::StoreL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags)
/*
 Store the field in the repository
 
 @internalComponent
*/
    {
    if (! iTarget || ! iTarget->iValue )
        {
        User::Leave(KErrNotReady);
        }
    
    *iElementId = (*iElementId & ~KCDMaskShowRecordId) | (aRecordId & KCDMaskShowRecordId) | aAttributeFlags;
   	return aSession->StorageL()->Create( ElementId() & KCDMaskHideRes, *(iTarget->iValue));
    }



template<class TYPE> 
inline TInt TMDBVisitor<TYPE>::ModifyL(CMDBSessionImpl* aSession, TMDBAttributeFlags aAttributeFlags)
/*
Modify the value of a field if the field exists
Never need to modify attributes as this is always done via the node.

@internalComponent
*/
    { 
    TInt err(KErrNotFound);
    
    if (! iTarget || !iElementId )
        {
        User::Leave(KErrNotReady);
        }
        
    *iElementId |= aAttributeFlags;

    TUint32 mask = (KCDMaskShowType | KCDMaskShowRecordId);

    RArray<TUint32> ids;
    CleanupClosePushL(ids);

    err = aSession->StorageL()->FindL(ElementId(), mask, ids);  
   
    if (ids.Count() > 0)
        {

#ifdef __DEBUG
        if (ids.Count() > 1)
            {
            __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("TMDBVisitor<TYPE> ModifyL() storage server FindL/FindEqL %d fields found when looking for id %08x.  Should be 1 or 0."), aIds.Count(), ElementId() & mask);
            }
#endif

    	err = aSession->StorageL()->Set(ElementId() & KCDMaskHideRes, iTarget->iValue);
        }
    else
        {
        err = aSession->StorageL()->Create(ElementId() & KCDMaskHideRes, iTarget->iValue);  
        }
    
    CleanupStack::PopAndDestroy(&ids);
    return err;
    }




template<> 
inline TInt TMDBVisitor<HBufC16*>::ModifyL(CMDBSessionImpl* aSession, TMDBAttributeFlags aAttributeFlags)
/*
ElementId must be fully resolved before this call

@internalComponent
*/
    {
    if (! iTarget || !iElementId )
        {
        User::Leave(KErrNotReady);
        }
  
    *iElementId |= aAttributeFlags;
    TUint32 mask = KCDMaskShowType | KCDMaskShowRecordId;

    RArray<TUint32> ids;
    CleanupClosePushL(ids);

    TInt err = aSession->StorageL()->FindL(ElementId(), mask, ids);  
   
    if (ids.Count() > 0)
        {
#ifdef DEBUG
        if (ids.Count() > 1)
            {
            __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("TMDBVisitor<TYPE> ModifyL() storage server FindL/FindEqL %d fields found when looking for id %08x.  Should be 1 or 0."), aIds.Count(), ElementId() & mask);
            }
#endif

        if(! iTarget->iValue)
        	{
    	    return KErrNone;
    	    }

		err = aSession->StorageL()->Set(ElementId() & KCDMaskHideRes, *(iTarget->iValue));
        }
    else
    	{
        if(! iTarget->iValue)
    	    {
    	    return KErrNone;
    	    }
    	err = aSession->StorageL()->Create(ElementId() & KCDMaskHideRes, *(iTarget->iValue));
    	}
    	
    CleanupStack::PopAndDestroy(&ids);
    return err;
    }



template<> 
inline TInt TMDBVisitor<HBufC8*>::ModifyL(CMDBSessionImpl* aSession, TMDBAttributeFlags aAttributeFlags)
/*
ElementId must be fully resolved before this call

@internalComponent
*/
    { 
    if (! iTarget || !iElementId )
        {
        User::Leave(KErrNotReady);
        }       
 
    *iElementId |= aAttributeFlags;
    TUint32 mask = KCDMaskShowType | KCDMaskShowRecordId;

    RArray<TUint32> ids;
    CleanupClosePushL(ids);

    TInt err = aSession->StorageL()->FindL(ElementId(), mask, ids);  
   
    if (ids.Count() > 0)
        {
#ifdef __DEBUG
        if (ids.Count() > 1)
            {
            __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("TMDBVisitor<HBufC8*> ModifyL() storage server FindL/FindEqL %d fields found when looking for %08x.  Should be 1 or 0."), ids.Count(), ElementId() & mask);
            }
#endif

        if(! iTarget->iValue)
       	    {
    	    return KErrNone;
    	    }

		err = aSession->StorageL()->Set(ElementId() & KCDMaskHideRes, *(iTarget->iValue));
        }
    else
    	{
        if(! iTarget->iValue)
           	{
    	    return KErrNone;
    	    }
 
    	err = aSession->StorageL()->Create(ElementId() & KCDMaskHideRes, *(iTarget->iValue));
    	}

    CleanupStack::PopAndDestroy(&ids);
    return err;
    }



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Record Set Visitor
//

NONSHARABLE_CLASS(TMDBRecordSetVisitor) : public TMDBVisitorBase
/*
A visitor for CMDBRecordSet.data

@internalComponent
*/
    {
public:

    static TMDBRecordSetVisitor* NewL(const TAny* aMem, const TAny* aData);

    // GET
    TInt LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags);
    TInt LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags, RArray<TUint32>& aIds);
    TInt FindL(CMDBSessionImpl* aSession, RArray<TUint32>& aIds);
    TInt RefreshL(CMDBSessionImpl* aSession);
   
    // SET
    TInt StoreL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags);
    TInt ModifyL(CMDBSessionImpl* aSession, TMDBAttributeFlags aAttributeFlags);
 
    TInt Changed() const;
    void Sync(){} // do nothing - leave changed flag set

    //void SetOwner(CMDBElement* aOwner);
      
public:
   
    RPointerArray<CMDBRecordBase>*   iTarget; 
         
protected:

    TMDBRecordSetVisitor(RPointerArray<CMDBRecordBase>* aData); 

private:
    
    void DoFindL(CMDBSessionImpl& aSession, CMDBElement& aElement, TInt& aErr, RArray<TUint32>& aCandidates, RArray<TUint32>& aMatches);
    };  




////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Record Link Visitor
//


NONSHARABLE_CLASS(TMDBRecordLinkVisitor) : public TMDBVisitor<TInt>
/*
A visitor for linked record data

@internalComponent
*/
    {
public:
    static TMDBRecordLinkVisitor* NewL(const TAny* aMem, const TAny* aData);

    // GET
    TInt LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags);
    TInt LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags, RArray<TUint32>& aIds);
    TInt FindL(CMDBSessionImpl* aSession, RArray<TUint32>& aIds);
    TInt RefreshL(CMDBSessionImpl* aSession);
    TInt GetL(CMDBSessionImpl* aSession);
   
    // SET
    TInt StoreL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags);
    TInt ModifyL(CMDBSessionImpl* aSession, TMDBAttributeFlags aAttributeFlags);

	TInt Changed() const;
    void SetOwner(CMDBElement* aOwner);

private:
    void SetLinkValueL(CMDBSession& aSession);

    void CheckLinkedRecordL(TMDBElementId aLinkedElementId);

    void CreateLinkedRecordL(CMDBSession& aSession, TMDBElementId aLinkedElementId);

    void CheckLinkValueL(TMDBElementId aLinkedElementId);

    void MaintainBCForLegacyLinks();
public:
    
    CMDBElement** iLinkedRecord; 
    
protected:

    TMDBRecordLinkVisitor(CMDBElement** aData);
    };  




////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  User defined Record Visitor
//


NONSHARABLE_CLASS(TMDBGenericRecordVisitor) : public TMDBVisitorBase
/*
A visitor for data inside a user-defined record

@internalComponent
*/
    {
public:
 
    static TMDBGenericRecordVisitor* NewL(const TAny* aMem, const TAny* aData);

    // GET
    void LoadTableInfoL(CMDBSessionImpl& aSession, TMDBAttributeFlags aAttributeFlags, CMDBGenericRecord& aRealOwner);
    TInt LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags);
    TInt LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags, RArray<TUint32>& aIds);
    void FindTableL(CMDBSessionImpl& aSession, CMDBGenericRecord& aRealOwner, RArray<TUint32>& aMatchingIds);
    TInt FindL(CMDBSessionImpl* aSession, RArray<TUint32>& aIds);
    TInt RefreshL(CMDBSessionImpl* aSession);
   
    // SET
    void StoreTableInfoL(CMDBSessionImpl& aSession, CMDBGenericRecord& aRealOwner);
    TInt StoreL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags);
    TInt ModifyL(CMDBSessionImpl* aSession, TMDBAttributeFlags aAttributeFlags);

	TInt Changed() const;
    void Sync(){} // do nothing - leave changed flag set
    //void SetOwner(CMDBElement* aOwner);
     
public:

    RPointerArray<CMDBElement>* iTarget;    
    
protected:

    TMDBGenericRecordVisitor(RPointerArray<CMDBElement>* aData);

    TMDBVisitorBase* CreateVisitorL(TAny* mem, CMDBElement* element, SGenericRecordTypeInfo* aRecordInfo);

private:

    CMDBGenericRecord* SetReadyL();
    };  



    } // End namespace CommsDat



#endif //METADATABASEVISITOR_H
