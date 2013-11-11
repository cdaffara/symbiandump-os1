// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <etelmm.h>
#include "cmockphonebookstoremesshandler.h"
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "cmmmessagerouter.h"
#include "cmockltsyengine.h"
#include "tmockltsydata.h"

#include <ctsy/serviceapi/cmmgsmphonestorageutility.h>


/**
Factory function
*/
CMockPhoneBookStoreMessHandler* CMockPhoneBookStoreMessHandler::NewL(CMmMessageRouter* aMessageRouter) 
    {
    CMockPhoneBookStoreMessHandler* self = new(ELeave)CMockPhoneBookStoreMessHandler(aMessageRouter);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

/**
Constructor
*/
CMockPhoneBookStoreMessHandler::CMockPhoneBookStoreMessHandler(CMmMessageRouter* aMessageRouter)
	:iMessageRouter(aMessageRouter)
    {
    }

/**
2nd phase contructor
*/
void CMockPhoneBookStoreMessHandler::ConstructL()
    {
    }

/**
Destructor
*/
CMockPhoneBookStoreMessHandler::~CMockPhoneBookStoreMessHandler()
    {
    
	for (TInt i(0); i < iEnRespArray.Count(); i++)
		{
		if ( iEnRespArray[i] )
			{
			iEnRespArray[i]->ResetAndDestroy();
			}
		}
	iEnRespArray.ResetAndDestroy();
    }

/**
Execute a request
*/
TInt CMockPhoneBookStoreMessHandler::ExtFuncL(TInt aIpc,const CMmDataPackage* aMmDataPackage) 
    {
    
    switch(aIpc)
        {            
    	case EMmTsyENStoreReadIPC:
        case EMmTsyONStoreReadIPC:
        case EMmTsyENStoreGetInfoIPC:
    	case EMmTsyONStoreWriteSizeIPC:
        case EMmTsyONStoreDeleteIPC:
        case EMmTsyONStoreGetInfoIPC:
        case EMmTsyENStoreReadAllPhase1IPC:
    	    {
    	    TInt index;
    	    
    	    aMmDataPackage->UnPackData(index);
    	    
		    TMockLtsyData1< TInt > data(index);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    	    }
        case EMmTsyONStoreWriteEntryIPC:
            {
    	    RMobileONStore::TMobileONEntryV1 entry;
    	    
    	    aMmDataPackage->UnPackData(entry);
    	    
		    TMockLtsyData1< RMobileONStore::TMobileONEntryV1 > data(entry);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
            }
            
        case EMmTsyONStoreWriteIPC:
            {
            RMobileONStore::TMobileONEntryV1Pckg* ptr;
	        	    
    	    aMmDataPackage->UnPackData(ptr);
    	    
            RMobileONStore::TMobileONEntryV1& entry = ( *ptr )();
    	    
  	        TMockLtsyData1<RMobileONStore::TMobileONEntryV1> data(entry);
    	    
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
            }
            
        case EMmTsyONStoreDeleteAllIPC:
            {
		    TMockLtsyData0 data;
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
            }
        case EMmTsyONStoreReadEntryIPC:
        case EMmTsyONStoreReadSizeIPC:
            {
            TInt index;
    	    aMmDataPackage->UnPackData(index);
		    TMockLtsyData1<TInt>  data(index);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
            }

    	default:
    	    break;
        }
    
    
 	const CPhoneBookDataPackage* phoneBookPackage = static_cast<const CPhoneBookDataPackage*>(aMmDataPackage);
 	TName phoneBookName;
	phoneBookPackage->GetPhoneBookName(phoneBookName);    
	
    switch (aIpc)
    	{    	
    	case EMmTsyPhoneBookStoreInitIPC:
    	    {
		    TMockLtsyPhoneBookData0 data(phoneBookName);		    
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    	    }
    	case EMmTsyPhoneBookStoreReadIPC:
    	    {
            RMobilePhoneBookStore::TPBIndexAndNumEntries* indexAndEntriesPtr;
            phoneBookPackage->UnPackData(indexAndEntriesPtr);
            
            TMockLtsyPhoneBookData1< RMobilePhoneBookStore::TPBIndexAndNumEntries > 
                    data(phoneBookName, *indexAndEntriesPtr);
                    
            return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    	    }
    	case EMmTsyPhoneBookStoreCacheIPC:
    	    {
		    TMockLtsyPhoneBookData0 data(phoneBookName);		    
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    	    }
    	case EMmTsyPhoneBookStoreWriteIPC:
    	    {
    	    CPhoneBookStoreEntry* entryPtr;
    	    TInt index;

    	    phoneBookPackage->UnPackData(index, entryPtr);
    	    
		    TMockLtsyPhoneBookData2< TInt, CPhoneBookStoreEntry* > data(phoneBookName, index, entryPtr);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    	    }

    	case EMmTsyPhoneBookStoreDeleteIPC:
    	    {
    	    TInt index;
    	    
    	    phoneBookPackage->UnPackData(index);
    	    
		    TMockLtsyPhoneBookData1< TInt > data(phoneBookName, index);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    	    }
            
    	default:
    		{
		    TMockLtsyPhoneBookData0 data(phoneBookName);		    
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	}
    }

/**
Complete a request
*/
void CMockPhoneBookStoreMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	CPhoneBookDataPackage dataPackage;
	CArrayPtrSeg<CPhoneBookStoreEntry>* pbEntryArray = NULL;

	switch (aIpc)
		{
    	case EMmTsyPhoneBookStoreInitIPC:
		    {
            CStorageInfoData info;    		
            
	        TMockLtsyPhoneBookData1< CStorageInfoData > retStoreAndName(info);
	        retStoreAndName.DeserialiseL(aData);
	        
	        dataPackage.SetPhoneBookName(retStoreAndName.PhoneBookName());

			dataPackage.PackData(retStoreAndName.Data1Ptr());
		    }
		    break;
		case EMmTsyPhoneBookStoreCacheIPC:
		    { 		    		    	    
		    TMockLtsyPhoneBookData1Buf <CArrayPtrSeg<CPhoneBookStoreEntry>*> retStoreAndName;		    		    
			CleanupClosePushL(retStoreAndName);	  	 
		    
            retStoreAndName.DeserialiseL(aData);    
	        dataPackage.SetPhoneBookName(retStoreAndName.PhoneBookName());	        
	        
			pbEntryArray = retStoreAndName.Data1();			
			dataPackage.PackData( &pbEntryArray );
			
			iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);
			
			CleanupStack::PopAndDestroy(1); 
			
			return;
		    }
    	case EMmTsyPhoneBookStoreReadIPC:
    	    {
		    TMockLtsyPhoneBookData1Buf<CArrayPtrSeg<CPhoneBookStoreEntry>*> retStoreAndName;
			CleanupClosePushL(retStoreAndName);
		    
		    retStoreAndName.DeserialiseL(aData);
            
            dataPackage.SetPhoneBookName(retStoreAndName.PhoneBookName());

            pbEntryArray = retStoreAndName.Data1();
            dataPackage.PackData( &pbEntryArray );
            
            iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);

			CleanupStack::PopAndDestroy(1);
			
			return;
    	    }    		
    	case EMmTsyPhoneBookStoreWriteIPC:
    	case EMmTsyPhoneBookStoreDeleteIPC:
    	case EMmTsyPhoneBookStoreDeleteAllIPC:
    	    {
            TPBEntryInfo pbEntryInfo;
            
	        TMockLtsyPhoneBookData1<TPBEntryInfo> ret(pbEntryInfo);
	        ret.DeserialiseL(aData);

	        dataPackage.SetPhoneBookName(ret.PhoneBookName());
		    
			dataPackage.PackData(ret.Data1Ptr());
    	    }    		
    	    break;
    	case EMmTsyPhoneBookStoreGetInfoIPC    	    :
    	    {
            TInt usedEntries;
            
	        TMockLtsyPhoneBookData1<TInt> ret(usedEntries);
	        ret.DeserialiseL(aData);

	        dataPackage.SetPhoneBookName(ret.PhoneBookName());
		    
			dataPackage.PackData(ret.Data1Ptr());
    	    }    		
    	    break;
		case EMmTsyPhoneBookStoreFdnInfoIPC:
		    {
        	TPBFdnInfo fdnInfo;

	        TMockLtsyPhoneBookData1<TPBFdnInfo> ret(fdnInfo);
	        ret.DeserialiseL(aData);

	        dataPackage.SetPhoneBookName(ret.PhoneBookName());
		    
			dataPackage.PackData(ret.Data1Ptr());
		    }
		    break;
    	case EMmTsyENStoreReadIPC:
            {
            TENStoreResponse enStoreResp;
            
	        TMockLtsyData1<TENStoreResponse> ret(enStoreResp);
	        ret.DeserialiseL(aData);
		    
			dataPackage.PackData(ret.Data1Ptr());

            }
            break;
        case EMmTsyENStoreGetInfoIPC:
            {
            TInt usedEntries;
            
	        TMockLtsyData1<TInt> ret(usedEntries);
	        ret.DeserialiseL(aData);
		    
			dataPackage.PackData(ret.Data1Ptr());
            }
            break;
		case EMmTsyONStoreWriteSizeIPC:
		    {
            TInt size;
            
	        TMockLtsyData1<TInt> ret(size);
	        ret.DeserialiseL(aData);
		    
			dataPackage.PackData(ret.Data1Ptr());
		    }
		    break;
    	case EMmTsyONStoreReadIPC:
    	    {
   	        TONStoreMsg onStoreMsg;
            TMockLtsyData1<TONStoreMsg> ret(onStoreMsg);
            ret.DeserialiseL(aData);

            TONStoreMsg* onStoreMsgPtr = ret.Data1Ptr();
            
			dataPackage.PackData(&onStoreMsgPtr);
    	    }
    	    break;
        case EMmTsyONStoreWriteIPC:
    	    {
            TInt location;
            
	        TMockLtsyData1<TInt> ret(location);
	        ret.DeserialiseL(aData);
		    
			dataPackage.PackData(ret.Data1Ptr());
		    }
		    break;
    	    
		case EMmTsyONStoreGetInfoIPC:
	        {
	        TServiceType serviceType;
	         
            TMockLtsyData1<TServiceType> ret(serviceType);           
             
	        ret.DeserialiseL(aData);
		    
			dataPackage.PackData(ret.Data1Ptr());
	        }
	        break;

		case EMmTsyENStoreReadAllPhase1IPC:
			if ( KErrNone == aResult )
				{
				TMockLtsyData1Buf<CRespArray*> data;
				data.DeserialiseL(aData);
				if ( data.Data1() )
					{
					iEnRespArray.Append(data.Data1());
					}
				dataPackage.PackData(data.Data1Ptr());
				}
			break;

		case EMmTsyONStoreDeleteIPC:
   		case EMmTsyONStoreDeleteAllIPC:
		    break;
		case EMmTsyONStoreReadSizeIPC:
			{
	        TInt size;
            TMockLtsyData1<TInt> ret(size);           
	        ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr());
	        }
	        break;
		case EMmTsyONStoreReadEntryIPC:
			{
	        TONStoreMsg msg;
            TMockLtsyData1<TONStoreMsg> ret(msg);           
	        ret.DeserialiseL(aData);
			dataPackage.PackData(ret.Data1Ptr());
	        }
	        break;
		
		case EMmTsyONStoreReadAllPhase1IPC:
        case EMmTsyONStoreWriteEntryIPC:
            {
            // complete without data
            }
            break;
		default:
			{
	        TMockLtsyPhoneBookData0 ret;
	        ret.DeserialiseL(aData);
	        dataPackage.SetPhoneBookName(ret.PhoneBookName());
			}
			break;
		}
	iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);

	}

// End of File
