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
// Implementation of visitor class to interact with the central repository
// for a class of type CMDBGenericRecord
// 
//

/**
 @file 
 @internalComponent
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif

#include "CommsDatInternalDefs.h"
#include "CommsDatTypesV1_1.h"


using namespace CommsDat;
using namespace Meta;


// TODO - need to check that all ids are greater than KCDInitialUdefRecordId


CommsDat::TMDBGenericRecordVisitor::TMDBGenericRecordVisitor(RPointerArray<CMDBElement>* aData)
: TMDBVisitorBase(), iTarget(aData)
/*
Constructor

@internalComponent
 */
    {
    }


CommsDat::TMDBGenericRecordVisitor* CommsDat::TMDBGenericRecordVisitor::NewL(const TAny* aMem, const TAny* aData)
/*
Instantiates a Generic Record Visitor of a particular type.
Used for attribute registration (in the data v-table).

@internalComponent
 */
    {
	return static_cast<CommsDat::TMDBGenericRecordVisitor*>( ::new ((TUint8*)aMem) TMDBGenericRecordVisitor( (RPointerArray<CMDBElement>*)(aData) ) );
    }



TInt CommsDat::TMDBGenericRecordVisitor::Changed() const
/*
Indicates whether data has been changed by client
this composite class always returns a positive response so an iterator will look for changed members.

@internalComponent
*/
    {
    return 1;
    }

void CommsDat::TMDBGenericRecordVisitor::LoadTableInfoL(CMDBSessionImpl& aSession, TMDBAttributeFlags aAttributeFlags, CMDBGenericRecord& aRealOwner) 
    {
    // find out field info ids from storage server
    RArray<TUint32> infoIds;
    CleanupClosePushL(infoIds);

	
    const TUint32 KNoWildCardOnTopBit = 0x80000000;
    TInt err(KErrNone);
    if(TypeId() & KCDMaskShowRecordType)
    	{
    	err = aSession.StorageL()->FindL( TypeId() | KCDMaskShowRecordId, 
                                                   ~aAttributeFlags & (KNoWildCardOnTopBit | KCDMaskShowRecordType | KCDMaskShowInstance), 
                                                   infoIds );	
    	}
	else 
		{
		//Search using table name
		RArray<TUint32> Ids;
		CleanupClosePushL(Ids);

        TUint32 mask = (KCDMaskGenericTableName | aSession.GetReadAttributeMask());
        err = aSession.StorageL()->FindEqL(KCDMaskGenericTableName, mask, aRealOwner.iRecordType, Ids);
        
		if(Ids.Count())							 
			{
			aRealOwner.SetElementId( (Ids[0] & KCDMaskShowRecordType) | aRealOwner.ElementId());
			}
		err = aSession.StorageL()->FindL( TypeId() | KCDMaskShowRecordId, 
                                                   ~aAttributeFlags & (KNoWildCardOnTopBit | KCDMaskShowRecordType | KCDMaskShowInstance), 
                                                   infoIds );									 
		CleanupStack::PopAndDestroy(&Ids);                                                   									 
		}   	
    User::LeaveIfError(err);

    const TInt count = infoIds.Count();
    if (count > 1 ) //todo correct? its tableplaceholder (nos: 1) + schema ids (nos: x)
        {
               
	    HBufC8* buf = HBufC8::NewMaxL(sizeof(SGenericRecordTypeInfo));       
	    CleanupStack::PushL(buf);	    
	        
	    TInt length(0);
	    TInt offset(0);
	    // Load the type info
	    for ( TInt i = 0; i < count; i++ )
	        {
	        const TMDBElementId currentId = infoIds[i];
	        if( ((currentId & KCDTableTypeInfo)     == KCDNewTableRequest) || 
	            ((currentId & KCDMaskShowFieldType) == KCDMaskShowFieldType) )
        		{
        		// Ignore the general plceholder or the table instance.
        		continue;
        		}
        		        
	        SGenericRecordTypeInfo* typeInfo = new (ELeave) SGenericRecordTypeInfo();
	        CleanupStack::PushL(typeInfo);

	        TPtr8 ptrBuf = buf->Des();
	        User::LeaveIfError(aSession.StorageL()->Get(currentId, ptrBuf ));
	       
			length = sizeof(typeInfo->iTypeId);
			Mem::Copy(static_cast<TAny*>(&(typeInfo->iTypeId)), ptrBuf.Ptr(), length);					

			offset = length;					
			length = sizeof(typeInfo->iValType);
			Mem::Copy(static_cast<TAny*>(&(typeInfo->iValType)), (ptrBuf.Ptr() + offset),length);

			offset = offset + length;
			length = sizeof(typeInfo->iTypeAttr);
			Mem::Copy(static_cast<TAny*>(&(typeInfo->iTypeAttr)), (ptrBuf.Ptr() + offset),length);
			
			offset = offset + length;			
			length = (ptrBuf.Length() - offset);
			Mem::Copy(static_cast<TAny*>(&(typeInfo->iTypeName)), (ptrBuf.Ptr() + offset),length);
			
	        aRealOwner.iRecordInfo.AppendL(*typeInfo); // always append
	        CleanupStack::PopAndDestroy(typeInfo);
	        }
		CleanupStack::PopAndDestroy(buf);
        }

    CleanupStack::PopAndDestroy(&infoIds);
    }



TBool MatchTypeInfosById(const SGenericRecordTypeInfo& aTypeInfo, const SGenericRecordTypeInfo& aAnotherTypeInfo)

//Utility function to test match of field ID in two fields.
//@internalTechnology

	{
    if( aTypeInfo.iTypeId == aAnotherTypeInfo.iTypeId)
        {
        return ETrue; 
        }              
    return EFalse;
	}
	
	
	

TInt CommsDat::TMDBGenericRecordVisitor::LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags) 
/*
Load the fields in this record 
Also load the table information if necessary.
@internalComponent
*/
    {
    __FLOG_STATIC0(KLogComponent, KCDInfoLog, _L("TMDBGenericRecordVisitor::LoadL()"));

    TInt err = KErrNone;
       
    CMDBGenericRecord* realOwner = SetReadyL();

    // Our container may only contain a small set of all the potential fields (for example because 
    // it's being used as a priming record for a FindL() operation, therefore we always want to perform
    // the following...
    
    if (realOwner->iRecordInfo.Count() == 0)
        {
        // Load the column information for the whole table
        LoadTableInfoL(*aSession, aAttributeFlags, *realOwner);
        
        if(aRecordId==0)
        	{
        	//got to be someone loading a record to initialize it with the table info so that they can store records
        	return KErrNone;	
        	}
        }

    // Find the field ids from the storage server
    RArray<TUint32> ids;
    CleanupClosePushL(ids);

    err = aSession->StorageL()->FindL( TypeId() | aRecordId, 
                                       ~aAttributeFlags & (KCDMaskShowRecordType | KCDMaskShowInstance),
                                       ids );
	
		
    if (ids.Count() == 0)
        {
        err = KErrNotFound;
        }

    User::LeaveIfError(err);

        
    // and create and load the fields
    //nithin
    TInt count(ids.Count());
    while(count > 0)
    	{
    	// Find the matching schema for every field id we have.
    	count--;
    	
    	if((ids[count] & KCDMaskShowFieldType) == KCDMaskShowFieldType)
			{
			//The ids would also include the placeholders and nodes of the record..we shouldnt be processing them here
			continue;	
			}	
    	
    	SGenericRecordTypeInfo typeInfo((ids[count] & KCDMaskShowFieldType), 0, ENoAttrs);
		TInt infoIndex = realOwner->iRecordInfo.Find(typeInfo, MatchTypeInfosById);
		if(infoIndex >= 0)
			{
			// Found the matching schema, now create the field object 
			CMDBElement* field = NULL;
			switch (realOwner->iRecordInfo[infoIndex].iValType)
   	            {
                case EText : 
                case EMedText :
                case ELongText :  
	                {
                    field = new (ELeave) CMDBField<TDesC>(ids[count]);            
	                break;
	                }
	                
                case EDesC8 : 
                    {
                    field = new (ELeave) CMDBField<TDesC8>(ids[count]);
	                break;
                    }
                    
                case ELink : 
                    {
                    field = new (ELeave) CMDBRecordLinkBase(ids[count]);
	                break;
                    }

   	            case EUint32 :
                    {
                    field = new (ELeave) CMDBField<TUint32>(ids[count]);
	                break;
                    }

   	            case EInt  :
                    {
                    field = new (ELeave) CMDBField<TInt>(ids[count]);
	                break;
                    }
                    
   	            case EBool :
                    {
                    field = new (ELeave) CMDBField<TBool>(ids[count]);
	                break;
                    }

                default:
                    {
                    User::Leave(KErrNotFound);
                    break;
                    }
                }  
			CleanupStack::PushL(field);
            User::LeaveIfError(iTarget->Append(field)); 
            CleanupStack::Pop(field); 				
			}                            
		else
			{
			//This is scary, a generic record that doesnt thave a matching schema
			User::Leave(KErrArgument);    				
			}   	       	
    	}
    
    CleanupStack::PopAndDestroy(&ids);
     
   
    // fields already existed or have just been created so just iterate through and load each one.

    TMDBVisitorBase *visitor = NULL;
    TAny* mem[KMMetaDBVisitorMaxSize];
   
    for (TInt i = 0; i < iTarget->Count(); i++)
        {
        visitor = CreateVisitorL(mem, (*iTarget)[i],NULL/*(*realOwner->iRecordInfo)[(dodgyArray[i])]*/);
        err = visitor->LoadL(aSession, aRecordId, aAttributeFlags); 
        if (err != KErrNone  &&  err != KErrNotFound)
            {
            break;
            }
        }
 	return err;
    }


TInt CommsDat::TMDBGenericRecordVisitor::LoadL(CMDBSessionImpl* aSession, TMDBElementId aElementId, TMDBAttributeFlags aAttributeFlags, RArray<TUint32>& /*aIds*/)    
/* 
*/
    {
    return LoadL(aSession, aElementId, aAttributeFlags);
    }
 


void CommsDat::TMDBGenericRecordVisitor::FindTableL(CMDBSessionImpl& aSession, CMDBGenericRecord& aRealOwner, RArray<TUint32>& aMatchingIds)
/*
Find the Table name in the database and return the id of the table and the contents of the column sets to the caller.

Only called from FindL when no fields are present in the record
@internalComponent
*/
    {
    TInt err(KErrNone);

        {
        TUint32 id = aRealOwner.ElementId(); // looking for the node
        TUint32 mask = KCDMaskShowField | aSession.GetReadAttributeMask();

        err = aSession.StorageL()->FindEqL(id, mask, aRealOwner.iRecordType, aMatchingIds);
        }

    if (err == KErrNone)
        {
         if (aMatchingIds.Count() == 2) // have found both placeholder and node
             {
             // set the id of the record to the id of the node and set changed flag.
             aRealOwner.SetElementId(aMatchingIds[0] | KCDChangedFlag);

             LoadTableInfoL(aSession, aMatchingIds[0] | KCDMaskShowAttributes, aRealOwner);
             }
         else 
             {
             // log err of how many ids are found.

             User::Leave(KErrPermissionDenied); // strictly speaking only true if < 2 found.
             }
        }
    else
        {
        User::Leave(err);
        }
    }


TInt CommsDat::TMDBGenericRecordVisitor::FindL(CMDBSessionImpl* aSession, RArray<TUint32>& aMatchingIds)
/*
Load all requested fields that match any instantiated values

If there are no fields in the field list and the Table Name has been set, find the Table and return the id of the table and the contents of the column sets to the caller.
@internalComponent
*/
    {       
    __FLOG_STATIC0(KLogComponent, KCDInfoLog, _L("TMDBGenericRecordVisitor::FindL()"));

    TInt retval = KErrNone;
   
    CMDBGenericRecord* realOwner = SetReadyL();



    if (iTarget->Count() == 0  &&  realOwner->iRecordType.Length() > 0)
        {
        FindTableL(*aSession, *realOwner, aMatchingIds);
        return KErrNone;
        }



    TMDBVisitorBase *visitor = NULL;
    TAny* mem[KMMetaDBVisitorMaxSize];


    RArray<TUint32> candidates;
    CleanupClosePushL(candidates);	
    
    for (TInt i = 0; i < iTarget->Count(); i++)
        {
        if( (*iTarget)[i])
            {
            visitor = CreateVisitorL(mem, (*iTarget)[i], NULL);
            retval = visitor->FindL(aSession, aMatchingIds); 
            if (retval == KErrNone)
                {
                retval = aSession->FindMatchL(candidates, aMatchingIds);
                }
            else
                {
                __FLOG_STATIC1(KLogComponent, KCDErrLog, _L("TMDBGenericRecordVisitor::FindL() no match found for primed record of type %08x"), realOwner->ElementId());
                break;
                }
            }
        }

   	CleanupStack::PopAndDestroy(&candidates);
    return retval;
    }
 
 
 
TInt CommsDat::TMDBGenericRecordVisitor::RefreshL(CMDBSessionImpl* aSession)
/*
Refresh existing fields in the container where they have not been changed by the client
May return
    KErrNotFound
    KErrPermissionDenied     
@internalComponent
*/
    {        
    __FLOG_STATIC0(KLogComponent, KCDInfoLog, _L("TMDBGenericRecordVisitor::RefreshL()"));

    TMDBVisitorBase *visitor = NULL;
    TAny* mem[KMMetaDBVisitorMaxSize];
   
    for (TInt i = 0; i < iTarget->Count(); i++)
        {
        if( (*iTarget)[i] && ! ((*iTarget)[i]->ElementId() & KCDChangedFlag))
            {
            // Use the type info bound into the field to create the visitor, not that in iRecordInfo.
            visitor = CreateVisitorL(mem, (*iTarget)[i], NULL);
            visitor->RefreshL(aSession); 
            }
        }
    
    return KErrNone;
    }


void CommsDat::TMDBGenericRecordVisitor::StoreTableInfoL(CMDBSessionImpl& aSession, CMDBGenericRecord& aRealOwner)
/*
Create new Set of column ids in the repository

  @internalComponent
*/
    {     
    TMDBElementId elementId(ElementId());
    
    RArray<TUint32> Ids; //todo handle alloc errors during leave
	aSession.StorageL()->FindEqL(KCDMaskGenericTableName, (KCDMaskGenericTableName|KCDMaskShowAttrAndRes|KCDMaskShowRes), aRealOwner.iRecordType, Ids);	
	TInt idCount = Ids.Count();
	Ids.Close();
	if(idCount)							 
		{
		//the table name allready exist....
		if ( (elementId & KCDMaskHideAttrAndRes) == 
              ( (KCDNewRecordRequest & KCDMaskHideAttrAndRes) | (elementId & KCDMaskShowRecordType) ) )
	    	{
	    	return;
	    	}
		User::Leave(KErrAlreadyExists);		    			
		}
		
    
    if ( (elementId & KCDMaskHideAttrAndRes) == KCDNewTableRequest )
        {
        // assign new table id and create node
        elementId &= ~KCDUtilityFlag;
        //todo hardcoded flag..oops
    	aSession.SetNodeIdL(elementId, 0x807FFF00, KCDInitialTableId, KCDLastUDefRecordType, KCDMaxUserDefTables);
        SetElementId(&elementId);
        aRealOwner.SetElementId(elementId);
        }

    
    // first store the info ids
    for (TInt i = 0; i < aRealOwner.iRecordInfo.Count(); i++)
        {
        // prepare id for this column
        elementId = (elementId & ~KCDMaskShowFieldType) | (aRealOwner.iRecordInfo[i].iTypeId & KCDMaskShowFieldType);

        TInt err;
        // Create placeholder if necessary
        TRAP(err, aSession.CreatePlaceholderL(elementId, KCDMaskShowFieldType, aRealOwner.iRecordType));
       
        if ( err != KErrNone && err != KErrNotFound)
            {
            User::Leave(err);     
            }

        // Create node if necessary
        TRAP(err, aSession.CreateNodeL(elementId, KCDMaskShowFieldType, aRealOwner, &(aRealOwner.iRecordInfo[i])));
        
        if ( err != KErrNone && err != KErrNotFound)
            {
            User::Leave(err);
            }
        }
    }



TInt CommsDat::TMDBGenericRecordVisitor::StoreL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags)
/*
Create new Fields in the repository 

  @internalComponent
*/
    {     
    __FLOG_STATIC0(KLogComponent, KCDInfoLog, _L("TMDBGenericRecordVisitor::StoreL()"));

    CMDBGenericRecord* realOwner = SetReadyL();
    
    if ( (ElementId() & KCDMaskHideAttrAndRes) == KCDNewTableRequest )
    	{
    	if ((realOwner->iRecordInfo.Count() > 0) && realOwner->iRecordType.Length())	
    		{
    		StoreTableInfoL(*aSession, *realOwner);		
    		}
    	else
    		{
    		User::Leave(KErrArgument);
    		}
    	}

    TMDBVisitorBase *visitor = NULL;
    TAny* mem[KMMetaDBVisitorMaxSize];
    
    // then store the fields
    for (TInt i = 0; i < iTarget->Count(); i++)
        {
        if( (*iTarget)[i] && ((*iTarget)[i]->ElementId() & KCDChangedFlag))
            {
            visitor = CreateVisitorL(mem, (*iTarget)[i], &(realOwner->iRecordInfo[i]));
            visitor->StoreL(aSession, aRecordId, aAttributeFlags); 
            }
        }
    
    return KErrNone;
    }


 
TInt CommsDat::TMDBGenericRecordVisitor::ModifyL(CMDBSessionImpl* aSession, TMDBAttributeFlags aAttributeFlags)
/*
Modify all changed fields in the store

Note - no function to modify table info.  

@internalComponent
*/
    {          
    __FLOG_STATIC0(KLogComponent, KCDInfoLog, _L("TMDBGenericRecordVisitor::ModifyL()"));


    TMDBVisitorBase *visitor = NULL;
    TAny* mem[KMMetaDBVisitorMaxSize];
   
    for (TInt i = 0; i < iTarget->Count(); i++)
        {
        if( (*iTarget)[i] && ((*iTarget)[i]->ElementId() & KCDChangedFlag))
            {
            // Use the type info bound into the field to create the visitor, not that in iRecordInfo.
            visitor = CreateVisitorL(mem, (*iTarget)[i], NULL);
            visitor->ModifyL(aSession, aAttributeFlags);
            }
        }
    
    return KErrNone;
    }
 


TMDBVisitorBase* TMDBGenericRecordVisitor::CreateVisitorL(TAny* aMem, CMDBElement* aElement, SGenericRecordTypeInfo* aRecordInfo)
    {
    if (! aElement /*|| ! aRecordInfo todo decide decide */)
        {
        User::Leave(KErrNotReady);
        }

    TMDBVisitorBase* visitor = NULL;
    TInt valueType;
    if(aRecordInfo)
    	{
    	valueType = aRecordInfo->iValType;
    	}
    else
    	{
    	valueType = aElement->Type();	
    	}
    	
    switch(valueType)
        {
        case EText : 
            {
            visitor = static_cast<TMDBVisitorBase*>(TMDBTextVisitorFactoryL(aMem,aElement));
            break;
            }
        case EMedText :
            {
            visitor = static_cast<TMDBVisitorBase*>(TMDBMedTextVisitorFactoryL(aMem,aElement));
            break;
            }
        case ELongText :  
 	        {
            visitor = static_cast<TMDBVisitorBase*>(TMDBLongTextVisitorFactoryL(aMem,aElement));
            break;
            }
        case EDesC8 : 
            {
            visitor = static_cast<TMDBVisitorBase*>(TMDBBinVisitorFactoryL(aMem,aElement));
            break;
            }
        default : 
            {
            visitor = static_cast<TMDBVisitorBase*>(TMDBNumVisitorFactoryL(aMem,aElement));
            }
        };
      
    visitor->SetOwner(this->iOwner);

    return visitor;
    }

CMDBGenericRecord* TMDBGenericRecordVisitor::SetReadyL()
    {
    if (! iOwner || ! iElementId || ! iTarget )
        {
        User::Leave(KErrNotReady);
        }

    CMDBGenericRecord* realOwner = static_cast<CMDBGenericRecord*>(iOwner);
    
    

    return realOwner;
    }
    
//EOF
