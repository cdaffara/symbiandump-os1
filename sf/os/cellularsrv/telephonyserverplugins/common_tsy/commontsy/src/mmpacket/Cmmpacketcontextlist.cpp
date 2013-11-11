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

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CmmpacketcontextlistTraces.h"
#endif

#include "Cmmpacketcontextlist.h"
#include "Cmmpacketservicetsy.h"
#include "Cmmpacketcontexttsy.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CMmPacketContextList::CMmPacketContextList
// C++ constructor
// ---------------------------------------------------------------------------
//
CMmPacketContextList::CMmPacketContextList()
    {
    }

CMmPacketContextList* CMmPacketContextList::NewL(
        CMmPacketServiceTsy* const aPacketService )
    {

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTLIST_NEWL_1, "TSY: CMmPacketContextList* CMmPacketContextList::NewL." );

    CMmPacketContextList* const contextList =
        new ( ELeave ) CMmPacketContextList();

    CleanupStack::PushL( contextList );
    contextList->iPacketService = aPacketService;
    contextList->ConstructL();
    CleanupStack::Pop();

    return contextList;

    }

void CMmPacketContextList::ConstructL()
    {

    //Create container
    CreateContainerL();

    InitAttributesAndListsL();

    }

CMmPacketContextList::~CMmPacketContextList()
    {
    //delete object container
    DeleteContainer();    
    }
   
// ---------------------------------------------------------------------------
// CMmPacketContextList::InitInternalAttributes
// Initialises miscellaneous internal attributes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextList::InitAttributesAndListsL()
    {

    // Max number of supported contexts is limited to be 11
    // Get defined channel
    TInt channel = iPacketService->MessageManager()->HandleRequestL( 
        EPacketInitProxiesIPC );

    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        iProxyIdList[i].iChannelId = (TUint8)channel;
        // Mark proxy id free
        iProxyIdList[i].iIsFree = ETrue;
        // Initialize context info list
        iContextInfoArray[i].iObject = NULL;
        iContextInfoArray[i].iChannelId = 0xFF;
        
        iContextInfoArray[i].iContextName.Zero();
#ifndef DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING
        channel++;
#endif //DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING        
        }

    iNumberOfCreatedContexts = 0;

    // Valid values selected to be between 100 and 255
    iIndividualNameForContext = 100;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextList::AddObject
// Adds a CMmPacketContextTsy object to the tsy object and context info lists.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextList::AddObject(
        CMmPacketContextTsy* const aContext, 
        const TInt aChannelId,              
        const TUint8 /* aObjectId */,
        const TInfoName& aContextName )      
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTLIST_ADDOBJECT_1, "TSY: CMmPacketContextList::AddObject. Context Name: %S Channel Id: %d", aContextName, aChannelId );

    TInt ret( KErrArgument );

    if ( !( aContext->IsDialUpContext( NULL ) ) )
        {

        if ( KMmMaxNumberOfContexts > iNumberOfCreatedContexts )
            {

            // Find given proxy id from proxy id list
            for (  TInt i = 0; i < KMmMaxNumberOfContexts; i++  )
                {
                if ( ( aChannelId == iProxyIdList[i].iChannelId ) && 
                     iProxyIdList[i].iIsFree )
                    {

                    // Add CMmPacketContextTsy object to the tsy object list
                    ret = CMmTsyObjectList::AddObject( aContext );

                    if ( KErrNone == ret )
                        {

                        // Set CMmPacketContextTsy to the context info list
                        iContextInfoArray[iNumberOfCreatedContexts].iObject =
                            aContext;

                        iContextInfoArray[iNumberOfCreatedContexts].iChannelId =
                            aChannelId;

                        iContextInfoArray[iNumberOfCreatedContexts].
                            iContextName = aContextName;

                        // Set proxy id reserved
                        iProxyIdList[i].iIsFree = EFalse;

                        // Increase number of created contexts
                        iNumberOfCreatedContexts++;
                
                        }

                    // proxy id found, break
                    i = KMmMaxNumberOfContexts;

                    }
                }
            }
        }
    else
        {

        ret = CMmTsyObjectList::AddObject( aContext );

        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextList::RemoveObject
// Removes a CMmPacketContextTsy object from the tsy object context info
// lists. Sets used proxy id released in proxy id list.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextList::RemoveObject(
        CMmPacketContextTsy* const aContext )
    {

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTLIST_REMOVEOBJECT_1, "TSY: CMmPacketContextList::RemoveObject." );

    // Set ret to KErrArgument to check that context is in list
    TInt ret( KErrArgument );

    if ( !( aContext->IsDialUpContext( NULL ) ) )
        {

        for ( TInt i = 0; i < iNumberOfCreatedContexts; i++ )
            {

            // Find aContext from context info list
            if ( aContext == iContextInfoArray[i].iObject )
                {

                for ( TInt j = 0; j < KMmMaxNumberOfContexts; j++ )
                    {
                    // Find reserved channel id from the proxy id list
                    if ( ( iContextInfoArray[i].iChannelId ==
                         iProxyIdList[j].iChannelId ) && ( EFalse ==
                         iProxyIdList[j].iIsFree ) )
                        {
                        // Remove aContext from the tsy object list
                        ret = CMmTsyObjectList::RemoveObject( aContext );

                        if ( KErrNone == ret )
                            {

OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTLIST_REMOVEOBJECT_2, "TSY: CMmPacketContextList::RemoveObject. Context Name: %S", iContextInfoArray[i].iContextName );
                            // Release Proxy id
                            iProxyIdList[j].iIsFree = ETrue;
            
                            // Get index to last reserved cell in the list
                            const TInt lastContext(
                                iNumberOfCreatedContexts - 1 );

                            // If not already last object
                            if ( i < lastContext )
                                {

                                iContextInfoArray[i].iChannelId =
                                    iContextInfoArray[lastContext].iChannelId;

                                iContextInfoArray[i].iObject =
                                    iContextInfoArray[lastContext].iObject;
                                iContextInfoArray[i].iContextName =
                                    iContextInfoArray[lastContext].iContextName;
                                }

                            // Reset object information of the last object
                            iContextInfoArray[lastContext].iObject = NULL;

                            iContextInfoArray[lastContext].iChannelId = 0xFF;

                            iContextInfoArray[lastContext].iContextName.Zero();
                            // Decrease number of created contexts 
                            iNumberOfCreatedContexts--;
        
                            }

                        // Proxy id found, break
                        j = KMmMaxNumberOfContexts;
                        }

                    }

                // aContext found, break
                i = iNumberOfCreatedContexts;

                }
            }
        }
    else
        {

        ret = CMmTsyObjectList::RemoveObject( aContext );

        }

    return ret;

    }


// ---------------------------------------------------------------------------
// CMmPacketContextList::PacketContextByIndex
// Returns Packet Context by index. Before calling ContextByIndex() method
// it is assumed that caller has called method NumberOfContexts() to get
// number of created contexts.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPacketContextTsy* CMmPacketContextList::PacketContextByIndex(
        const TInt aIndex ) const
    {

    if ( ( 0 <= aIndex ) && ( KMmMaxNumberOfContexts > aIndex ) )
        {

        // Return object
        return iContextInfoArray[aIndex].iObject;

        }

    return NULL;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextList::PacketContextByName
// Returns Packet Context by name.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPacketContextTsy* CMmPacketContextList::PacketContextByName(
        const TInfoName* const aName ) const //name of the context
    {

    // Find name from context info list
    for ( TInt i = 0; i < iNumberOfCreatedContexts; i++ )
        {

        if ( *aName == iContextInfoArray[i].iContextName )
            {

            // Name found, return object 
            return iContextInfoArray[i].iObject;

            }
        }

    // Object not found, return NULL
    return NULL;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextList::NumberOfContexts
// This function returns the number of created contexts.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextList::NumberOfContexts() const
    {
    return iNumberOfCreatedContexts;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextList::GenerateNewContextName
// This function generates new proxy id and context name.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextList::GenerateNewContextName(
        TDes& aNewName,     //name of the context
        TUint8& aProxyId )    //proxy id of the context
    {

    TInt ret( KErrOverflow );

    // Find first free proxy id from the list
    for ( TInt i = 0; i < KMmMaxNumberOfContexts; i++ )
        {
        if ( iProxyIdList[i].iIsFree )
            {
            // Set proxy id to the iChannelId
            aProxyId = ( TUint8 )iProxyIdList[i].iChannelId;
   
            aNewName.Zero();

            TInfoName contextName;

            do
                {
                contextName.Zero();
                // Make sure number to add is between 100 and 255 (TUint8)
                if ( 100 > iIndividualNameForContext )
                    {
                    iIndividualNameForContext = 100;
                    }
                contextName.AppendNum( ( TInt )iIndividualNameForContext++ );
                // Internally created context name
                _LIT( KContextNameInt, "Int_" );
                contextName.Append( KContextNameInt );
                contextName.AppendNum( ( TInt )aProxyId );
                }
            while ( NULL != CMmPacketContextList::PacketContextByName( 
					&contextName ) );
            
            aNewName = contextName;
            // Proxy id selected, set ret to KErrNone
            ret = KErrNone;
            
            i = KMmMaxNumberOfContexts;
            }
        }

    return ret;
        
    }


// ---------------------------------------------------------------------------
// CMmPacketContextList::GetContextStatusByIndex
// Returns context status by index.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextList::GetContextStatusByIndex(
        const TInt aIndex,  
        RPacketContext::TContextStatus& aStatus ) const
    {

    if ( 0 <= aIndex && aIndex < iNumberOfCreatedContexts )
        {

        // Set aStatus from the context info list
        aStatus = iContextInfoArray[aIndex].iObject->ContextStatus();
        // Index valid, return KErrNone
        return KErrNone;

        }

    // Invalid index, return KErrArgument
    return KErrArgument;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextList::GetContextNameByIndex
// This function returns context name by index.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextList::GetContextNameByIndex(
        const TInt aIndex,  
        TName& aName ) const 
    {

    if ( 0 <= aIndex && aIndex < iNumberOfCreatedContexts )
        {

        // Set aName from the context info list
        aName = iContextInfoArray[aIndex].iContextName;
        // Index valid, return KErrNone
        return KErrNone;

        }

    // Invalid index, return KErrArgument
    return KErrArgument;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextList::EnumerateNifs
// Returns number of network interfaces
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextList::EnumerateNifs(
        TInt* aCount )
    {

    TInt count( 0 );

    RPacketContext::TProtocolAddress listOfNifs[KMaxNumberOfNifs];
    
    RPacketContext::TProtocolAddress pdpAddress;

    // steps through iContextInfoArray Contexts to find all nifs
    for ( TInt i = 0 ; CMmPacketContextList::NumberOfContexts() > i ; i++ )
        {

        CMmPacketContextTsy* contextObjectForPrimary = 
            iContextInfoArray[i].iObject;

        TBool isPdpAddressSet( EFalse );
        // if context config type is set
        if ( NULL != contextObjectForPrimary->PacketContextGsmWcdmaExt()->
            ContextConfig() )
            {

            pdpAddress = contextObjectForPrimary->PacketContextGsmWcdmaExt()->
                ContextConfig()->iPdpAddress; 

            isPdpAddressSet = ETrue;

            }

        TBool isPdpAddressInArray( EFalse );

        for ( TInt j = 0 ; ( count > j ) && ( isPdpAddressSet ) ; j++ )
            {

            if ( pdpAddress == listOfNifs[j] )
                {

                j = count;
                isPdpAddressInArray = ETrue;

                }
            }

        // if pdpaddress have not already increased count
        if ( !isPdpAddressInArray )
            {

            listOfNifs[count] = pdpAddress;
            count++;

            }
        }

    *aCount = count;

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTLIST_ENUMERATENIFS_1, "TSY: CMmPacketContextList::EnumerateNifs. Number of Nifs: %d", aCount );
    }

// ---------------------------------------------------------------------------
// CMmPacketContextList::GetNifInfo
// Return information about network interface
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextList::GetNifInfo(
        const TInt* aIndex,   
        RPacketService::TNifInfoV2* aNifInfoV2 ) 
    {

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTLIST_GETNIFINFO_1, "TSY: CMmPacketContextList::GetNifInfo." );

    TInt index = *aIndex;

    TInt nifContextsCount( 0 );

    TInt ret( KErrArgument );

    RPacketContext::TProtocolAddress listOfNifs[KMaxNumberOfNifs];

    RPacketContext::TProtocolAddress pdpAddress;

    if ( CMmPacketContextList::NumberOfContexts() > index )
        {

        TInt count( 0 );
        
        for ( TInt i = 0 ; CMmPacketContextList::NumberOfContexts() > i ; i++ )
            {

            // get context object from array
            CMmPacketContextTsy* contextObject = iContextInfoArray[i].iObject;

            TBool isPdpAddressSet( EFalse );

            if ( NULL != contextObject->PacketContextGsmWcdmaExt()->
                ContextConfig() )
                {

                pdpAddress = contextObject->PacketContextGsmWcdmaExt()->
                    ContextConfig()->iPdpAddress; 

                isPdpAddressSet = ETrue;

                }

            TBool isPdpAddressInArray( EFalse );

            // steps through listOfNifsArray and make sure that pdpAddress 
            // isn't already in it
            for ( TInt j = 0 ; ( count > j ) && ( isPdpAddressSet ) ; j++ )
                {

                if ( pdpAddress == listOfNifs[j] )
                    {

                    j = count;
                    isPdpAddressInArray = ETrue;

                    }
                }

            // if pdpAddress isn't in array, then we put it in there
            if ( !isPdpAddressInArray && isPdpAddressSet )
                {

                listOfNifs[count] = pdpAddress;
                count++;

                }


            // when index and count-1 are equal, we have found right Nif
            if ( index == count-1 )
                {

                aNifInfoV2->iNifStatus = contextObject->ContextStatus();

                if ( NULL != contextObject->
                    PacketContextGsmWcdmaExt()->ContextConfig() )
                    {

                    aNifInfoV2->iPdpAddress = contextObject->
                        PacketContextGsmWcdmaExt()->ContextConfig()->
                        iPdpAddress;

                    }


                aNifInfoV2->iContextType = RPacketService::EInternalContext;

                TInfoName contextName = iContextInfoArray[i].iContextName;

                aNifInfoV2->iContextName = contextName;

                EnumerateContextsInNif( contextName , &nifContextsCount );

                aNifInfoV2->iNumberOfContexts = nifContextsCount;

                ret = KErrNone;

                i = CMmPacketContextList::NumberOfContexts();

                }
            }
        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextList::EnumerateContextsInNif
// Returns number of contexts in Nif
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextList::EnumerateContextsInNif(
        const TInfoName aExistingContextName, 
        TInt* aCount )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTLIST_ENUMERATECONTEXTSINNIF_1, "TSY: CMmPacketContextList::EnumerateContextsInNif." );

    TInt count( 0 );

    for ( TInt g = 0 ;CMmPacketContextList::NumberOfContexts() > g ; g++ )
        {

        TInfoName contextName = iContextInfoArray[g].iContextName;

        // if we found right nif from iContextInfoArray
        if ( aExistingContextName == contextName )
            {

            CMmPacketContextTsy* contextObject = iContextInfoArray[g].iObject;

            // go through array again to find all contexts with same pdp 
            // address
            for ( TInt i = 0 ; CMmPacketContextList::NumberOfContexts() > i ; i++ )
                {

                CMmPacketContextTsy* contextObjectSecondContext = 
                        iContextInfoArray[i].iObject;

                RPacketContext::TProtocolAddress pdpAddress;
                
                RPacketContext::TProtocolAddress pdpAddressForSecondContext;

                TBool isPdpAddressSet( EFalse );

                if ( NULL != contextObject->PacketContextGsmWcdmaExt()->
                    ContextConfig() )
                    {

                    pdpAddress = contextObject->PacketContextGsmWcdmaExt()->
                        ContextConfig()->iPdpAddress; 

                    isPdpAddressSet = ETrue;

                    }

                if ( NULL != contextObjectSecondContext->
                    PacketContextGsmWcdmaExt()->ContextConfig() )
                    {

                    pdpAddressForSecondContext = contextObjectSecondContext->
                        PacketContextGsmWcdmaExt()->ContextConfig()->
                        iPdpAddress;

                    }

                // contexts with same pdp address belong to this Nif
                if ( ( pdpAddress == pdpAddressForSecondContext ) && 
                    ( isPdpAddressSet ) )
                    {

                    count++;

                    }
                }
            }
        }

   *aCount = count;

   }

// ---------------------------------------------------------------------------
// CMmPacketContextList::GetContextNameInNif
// Returns context in this Nif by aContextNameInNif
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextList::GetContextNameInNif(
        const RPacketService::TContextNameInNif* aContextNameInNif,
        TDes* aContextName )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTLIST_GETCONTEXTNAMEINNIF_1, "TSY: CMmPacketContextList::GetContextNameInNif." );

    TInt index = aContextNameInNif->iIndex;

    TInt count( 0 );

    TInt ret( KErrArgument );

    for ( TInt g = 0 ; CMmPacketContextList::NumberOfContexts() > g ; g++ )
        {

        if ( aContextNameInNif->iExistingContextName == iContextInfoArray[g].iContextName )
            {

            CMmPacketContextTsy* contextObject = iContextInfoArray[g].iObject;

            for ( TInt i = 0 ; CMmPacketContextList::NumberOfContexts() > i ; i++ )
                {

                RPacketContext::TProtocolAddress pdpAddress;

                RPacketContext::TProtocolAddress pdpAddressForSecondContext;

                CMmPacketContextTsy* contextObjectSecondContext = 
                    iContextInfoArray[i].iObject;

                TBool isPdpAddressSet( EFalse );

                if ( NULL != contextObject->PacketContextGsmWcdmaExt()->
                    ContextConfig() )
                    {

                    pdpAddress = contextObject->PacketContextGsmWcdmaExt()->
                        ContextConfig()->iPdpAddress;

                    isPdpAddressSet = ETrue;

                    }

                if ( NULL != contextObjectSecondContext->
                    PacketContextGsmWcdmaExt()->ContextConfig() )
                    {

                    pdpAddressForSecondContext = contextObjectSecondContext->
                        PacketContextGsmWcdmaExt()->ContextConfig()->
                        iPdpAddress;

                    }

                if ( ( pdpAddress == pdpAddressForSecondContext ) &&
                    ( isPdpAddressSet ) )
                    {

                    if ( index == count )
                        {
                        
						if ( aContextName->MaxLength() >= iContextInfoArray[i].iContextName.Length() )
							{
                        	*aContextName = iContextInfoArray[i].iContextName;
                        	                    
                        	ret = KErrNone;
                        	
                        	}
                        
						i = g = CMmPacketContextList::NumberOfContexts();

                        }

                    count++;

                    }
                }
            }
        }

    return ret;

    }

// ================= OTHER EXPORTED FUNCTIONS ===============================


//  End of File 



