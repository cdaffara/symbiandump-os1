// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <etelpckt.h>
#include <pcktlist.h>
#include "mbmstypes.h"
#include "cmmmbmsmonitoredlist.h"


// ============================ MEMBER FUNCTIONS ===============================

CMmMBMSMonitoredList::CMmMBMSMonitoredList()
    {
    }
    
CMmMBMSMonitoredList* CMmMBMSMonitoredList::NewL(
    CMmPacketServiceTsy* aMmPacketService,
    TInt aMaxEntries )
    {
    CMmMBMSMonitoredList* monitoredList = new ( ELeave ) CMmMBMSMonitoredList();

    CleanupStack::PushL( monitoredList );
    
    monitoredList->ConstructL(aMmPacketService, aMaxEntries);
    CleanupStack::Pop();
    
    return monitoredList;            
    }
    
void CMmMBMSMonitoredList::ConstructL(CMmPacketServiceTsy* aMmPacketService, TInt aMaxEntries)    
    {
    iMmPacketService = aMmPacketService;
    iServiceList = CPcktMbmsMonitoredServiceList::NewL(); 
    iServiceList->SetMaxNumberEntries(aMaxEntries);
    }

CMmMBMSMonitoredList::~CMmMBMSMonitoredList()
    {
    if(iServiceList != NULL)
    	{
        delete iServiceList;
        iServiceList = NULL;
    	}
    if(iMbmsMonitoredListData.iListOfChanges != NULL)
    	{
    	delete iMbmsMonitoredListData.iListOfChanges;
    	iMbmsMonitoredListData.iListOfChanges = NULL;   
    	}
    }

// ---------------------------------------------------------------------------
// CMmMBMSMonitoredList::SaveReqUpdValues
// Save changes to temp list
// ---------------------------------------------------------------------------
// 
TInt CMmMBMSMonitoredList::SaveUpdateValuesToTempL( 
	TDes8* aList, 
	TMbmsAction aAction )
	{
	TInt ret( KErrNone );	

	iMbmsMonitoredListData.iActionType = aAction;
	iMbmsMonitoredListData.iServiceList = iServiceList;
	
	if(iMbmsMonitoredListData.iListOfChanges == NULL)
		{
		iMbmsMonitoredListData.iListOfChanges = CPcktMbmsMonitoredServiceList::NewL();	
		}

	if( aList == NULL )
		{
		while( iMbmsMonitoredListData.iListOfChanges->Enumerate() > 0 )
			{
			iMbmsMonitoredListData.iListOfChanges->DeleteEntryL(0);
			}
		}
	else
		{
		iMbmsMonitoredListData.iListOfChanges->RestoreL(*aList);
		
		if( ( EAddEntries == aAction ) || ( ERemoveEntries == aAction ))
			{
			RPacketService::TMbmsServiceAvailabilityV1 entry;
			TInt i = 0;
			while(i != iMbmsMonitoredListData.iListOfChanges->Enumerate() )
			    {
				entry = iMbmsMonitoredListData.iListOfChanges->GetEntryL(i);
	            // check if entry already exists in service list
	            if( ( ( EntryExistL( entry ) ) && ( EAddEntries == aAction )) ||
	            	( (!EntryExistL( entry ) ) && ( ERemoveEntries == aAction)))
	            	{
	            	iMbmsMonitoredListData.iListOfChanges->DeleteEntryL(i);
	                }
	             else
	            	{
		            i++;                	
		            }
	            }
	        if( 0 == iMbmsMonitoredListData.iListOfChanges->Enumerate())	
		        {
		        ret = KErrAlreadyExists;
		        ResetTempListL();
		        }
			}
		}
	
	return ret;
	
	}

// ---------------------------------------------------------------------------
// CMmMBMSMonitoredList::PackData()
// Pack data to send to the LTSY
// ---------------------------------------------------------------------------
// 
TInt CMmMBMSMonitoredList::PackDataL()
 	{
 	
    TInt ret( KErrNone );

 	CMmDataPackage dataPackage;
 	switch( iMbmsMonitoredListData.iActionType )
 		{
 		case ERemoveAllEntries:
 			// pack data (old list + action)
 			ResetTempListL(); 
 			//No break since we stll need to pack
 		case EAddEntries:
 		case ERemoveEntries:
 			// Pack data (old list + change list + action)
       		dataPackage.PackData( &iMbmsMonitoredListData ); 
       		 ret = iMmPacketService->MessageManager()->HandleRequestL(EPacketUpdateMbmsMonitorServiceList, &dataPackage);
 			break;
 		default:
            ret = KErrNotSupported;  
            break;
 		}
 	if( KErrNone != ret )
	 	{
	 	ResetTempListL();
	 	}
 		
 	return ret;
 	}

// ---------------------------------------------------------------------------
// CMmMBMSMonitoredList::ProcessEntriesL
// Packet Data specific functionality of CMmPacketContextTsy::ExtFunc
// ---------------------------------------------------------------------------
//         
TInt CMmMBMSMonitoredList::ProcessEntriesL( 
    const CPcktMbmsMonitoredServiceList *aList, 
    TMbmsAction aAction )
    {    
    TInt ret( KErrNone );
    
    switch( aAction )
        {
        case EAddEntries:
            ret = AddTempEntriesToCacheL( aList );
            break;
        case ERemoveEntries:
            DeleteEntriesL( aList );
            break;
        case ERemoveAllEntries: 
            ClearListL( iServiceList );
            break;
        default:
            ret = KErrNotSupported;
            break;                   
        }
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmMBMSMonitoredList::AddEntries
// Add new entry/entries into monitored service list
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TInt CMmMBMSMonitoredList::AddEntriesL( const CPcktMbmsMonitoredServiceList *aList )    
    {
    // verify we dont cause overflow
    if(iServiceList->Enumerate() + aList->Enumerate() >= iServiceList->MaxNumberEntries() )
    	{
    	return KErrOverflow;
    	}
    TInt size = aList->Enumerate();
    for( TInt i = 0; i < size; ++i)
    	{
    	if( EntryExistL( aList->GetEntryL(i) ) )
    		{
    		return KErrAlreadyExists;
    		}
    	}
    for( TInt i = 0; i < size; ++i)
    	{
    	iServiceList->AddEntryL( aList->GetEntryL(i) );
    	}
    return KErrNone;
    }
    
   
// ---------------------------------------------------------------------------
// CMmMBMSMonitoredList::DeleteEntriesL
// Remove entry/entries from monitored service list
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//     
void CMmMBMSMonitoredList::DeleteEntriesL( const CPcktMbmsMonitoredServiceList *aList )  
    {
    
    if( iMbmsMonitoredListData.iListOfChanges == NULL )
    	{
    	return ;
    	}
    // those entries should not be removed
    if( aList )
	    {
	   	// check if entries could not be added 
		if( aList->Enumerate() > 0 )
			{
			
			//iterate through failed entries vs. iMbmsMonitoredListData.iListOfChanges
	    	for( TInt i = 0; i < iMbmsMonitoredListData.iListOfChanges->Enumerate(); i++ )
			    {
			    for(TInt j = 0; j < aList->Enumerate(); j++ )
				    {
				    //if entry could not be removed -> remove it from temp list
				    //if no match found. leave it into temp list
			       	if( iMbmsMonitoredListData.iListOfChanges->GetEntryL(i).iTmgi.GetServiceId() == aList->GetEntryL(j).iTmgi.GetServiceId() )
			            {
			            iMbmsMonitoredListData.iListOfChanges->DeleteEntryL(i);
			           	j= aList->Enumerate();
			           	i--;
					    }
				    }
			    }
			}
	    }

	// temp list is up-to-date
    // remove entries in temp list from the main list
    for( TInt i = 0; i < iMbmsMonitoredListData.iListOfChanges->Enumerate(); i++ )
		{
		//iterate through temo list vs. main list
    	for( TInt ii = 0; ii < iServiceList->Enumerate(); )
		    {
		    //if entry could not be removed -> remove it from services list
		    //if no match found. leave it into temp list
	       	if( iMbmsMonitoredListData.iListOfChanges->GetEntryL(i).iTmgi.GetServiceId() == 
	       		iServiceList->GetEntryL(ii).iTmgi.GetServiceId())
	            {
	           	iServiceList->DeleteEntryL(ii);
			    }
	       	else
	       		{
	       		++ii;
	       		}
		    }
		
		}
    }

	    
	        
// ---------------------------------------------------------------------------
// CMmMBMSMonitoredList::ClearListL
// Clear given service list
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//      
void CMmMBMSMonitoredList::ClearListL( CPcktMbmsMonitoredServiceList *aList )    
    {
    
	while(aList->Enumerate() > 0 )
		{
		aList->DeleteEntryL(0);	
		}
    }
   
// ---------------------------------------------------------------------------
// CMmMBMSMonitoredList::EntryExistL
// Clear given service list
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//      
TBool CMmMBMSMonitoredList::EntryExistL( const RPacketService::TMbmsServiceAvailabilityV1 &aEntry )    
    {
    
    for( TInt i = 0; i < iServiceList->Enumerate(); i++ )
        {
        if( aEntry.iTmgi.GetServiceId() == iServiceList->GetEntryL(i).iTmgi.GetServiceId() )
            {
            return ETrue;
            }        
        }
    return EFalse;
    
    }
  
// ---------------------------------------------------------------------------
// CMmMBMSMonitoredList::NumberOfMonitoredList
// This function returns the number of monitored service list.
// ---------------------------------------------------------------------------
//
TInt CMmMBMSMonitoredList::NumberOfMonitoredList() 
    {
    return iServiceList->Enumerate();
    }

// ---------------------------------------------------------------------------
// CMmMBMSMonitoredList::SetServicesAvailabilityL
// This function sets the availability status of the service list.
// ---------------------------------------------------------------------------
//  
void CMmMBMSMonitoredList::SetServicesAvailabilityL(CPcktMbmsMonitoredServiceList *aAvailableList)
	{
	if ( aAvailableList )
		{
#ifndef USING_CTSY_DISPATCHER
		//if the available list is empty (i.e. no services) then all entries in
		//the service list should be marked unavailable
		//the header says aAvailableList is  "a list of services that should be set to available (the rest will be unavailable)"
		//this check prevents the the services being marked unavilable in the case the list is empty
		if (aAvailableList->Enumerate() > 0)
#endif //USING_CTSY_DISPATCHER
			{
			RPacketService::TMbmsServiceAvailabilityV1 updEntry;
			RPacketService::TMbmsServiceAvailabilityV1 origEntry;
			
			//Set available status to EMbmsUnavailable
			for( TInt index1 = 0; index1 < iServiceList->Enumerate(); index1++ )
				{
				origEntry = iServiceList->GetEntryL(index1);
				origEntry.iMbmsAvailabilityStatus = EMbmsUnavailable;
				iServiceList->ChangeEntryL(index1, origEntry);
				}
			
			// check throught the updated monitor list
			for( TInt index2 = 0; index2 < aAvailableList->Enumerate(); index2++ )
				{
				updEntry = aAvailableList->GetEntryL(index2);
				
				// Seach same entry to original list
				for( TInt index3 = 0; index3 < iServiceList->Enumerate(); index3++ )
					{
					origEntry = iServiceList->GetEntryL(index3);
					
					if (origEntry.iTmgi.GetServiceId() == updEntry.iTmgi.GetServiceId())
						{
						// availability status is EMbmsAvailable
						origEntry.iMbmsAvailabilityStatus = EMbmsAvailable;
						iServiceList->ChangeEntryL(index3, origEntry);
						}
					}
				}
			}
		}
	}
	
// ---------------------------------------------------------------------------
// CMmMBMSMonitoredList::AddTempEntriesToCache
// This function add the succesfully added monitor entries to main monitor service
// list
// ---------------------------------------------------------------------------
//  	
TInt CMmMBMSMonitoredList::AddTempEntriesToCacheL(const CPcktMbmsMonitoredServiceList* aFailedMonitorEntries)
	{
	
	if(iMbmsMonitoredListData.iListOfChanges == NULL)
		{
		return KErrNone;
		}
	// some entries could not be removed->update temp list for 
    if( aFailedMonitorEntries )
	    {
	    //iterate through failed entries vs. iMbmsMonitoredListData.iListOfChanges
	    for(TInt j = 0; j < aFailedMonitorEntries->Enumerate(); j++ )
	    	{
	    	for( TInt i = 0; i < iMbmsMonitoredListData.iListOfChanges->Enumerate(); i++ )
	    		{
	    		//if entry could not be removed -> remove it from temp list
	    		//if no match found. leave it into temp list
	    		if( iMbmsMonitoredListData.iListOfChanges->GetEntryL(i).iTmgi.GetServiceId() ==	    		
	    			aFailedMonitorEntries->GetEntryL(j).iTmgi.GetServiceId() )
	    			{
	    			//delete failed entry fom temp list before append them to main list
	    			iMbmsMonitoredListData.iListOfChanges->DeleteEntryL(i);
	    			i = iMbmsMonitoredListData.iListOfChanges->Enumerate();
	    			}
	    		}
	    	}
	    }
	
    if( iMbmsMonitoredListData.iListOfChanges->Enumerate() + iServiceList->Enumerate() >= 
    		iServiceList->MaxNumberEntries() )
    	{
    	return KErrOverflow;
    	}
	// temp list is up-to-date
    // temporary saved entries can be added to the main list
	for(TInt i=0;i < iMbmsMonitoredListData.iListOfChanges->Enumerate(); i++)
		{
		iServiceList->AddEntryL(iMbmsMonitoredListData.iListOfChanges->GetEntryL(i));
		}
	ResetTempListL();
	return  KErrNone;
	}
	
// ---------------------------------------------------------------------------
// CMmMBMSMonitoredList::ResetTempListL
// This function clears the temporary monitors service list e.g. in case of timeout
// 
// ---------------------------------------------------------------------------
//  		
void CMmMBMSMonitoredList::ResetTempListL()
	{
	if(iMbmsMonitoredListData.iListOfChanges != NULL)
		{
		ClearListL(iMbmsMonitoredListData.iListOfChanges);	 
		delete iMbmsMonitoredListData.iListOfChanges;
		iMbmsMonitoredListData.iListOfChanges = NULL;
		}
	}
		

// ---------------------------------------------------------------------------
// CMmMBMSMonitoredList::ServiceList
// This function return a pointer to the service list
// 
// ---------------------------------------------------------------------------
//  		
CPcktMbmsMonitoredServiceList* CMmMBMSMonitoredList::ServiceList()
	{
	return iServiceList;
	}

//  End of File 

