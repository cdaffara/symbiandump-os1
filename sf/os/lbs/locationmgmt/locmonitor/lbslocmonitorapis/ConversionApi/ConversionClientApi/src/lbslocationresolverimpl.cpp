/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CLbsLocationResolverImpl class.
*
*/

#include <s32mem.h>
#include "lbslocationresolverimpl.h"
#include <lbs/lbslocationresolver.h>


void CleanUpAreaInfoArray(TAny* aArray)
    {
	 	static_cast<RLbsAreaInfoBaseArray*>(aArray)->ResetAndDestroy();
    }

//----------------------------------------------------------------
// CLbsLocationResolverImpl::NewL
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocationResolverImpl* CLbsLocationResolverImpl::NewL(
                                     MLbsLocationResolverObserver& aObserver, 
                                     const TUid aConverterModuleId )
    {
    CLbsLocationResolverImpl* self = new( ELeave ) CLbsLocationResolverImpl(
                                                                  aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aConverterModuleId );
    CleanupStack::Pop( self );
    return self;
    }


//----------------------------------------------------------------
// CLbsLocationResolverImpl::~CLbsLocationResolverImpl
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocationResolverImpl::~CLbsLocationResolverImpl()
    {
    if(iLocationInfo)
        {
        delete iLocationInfo;
        iLocationInfo = NULL;
        }
    iLocInfoConverter.Close();
    iLocMoniterSession.Close();
    }

//----------------------------------------------------------------
// CLbsLocationResolverImpl::CLbsLocationResolverImpl
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocationResolverImpl::CLbsLocationResolverImpl( 
                                  MLbsLocationResolverObserver& aObserver):
    CActive(EPriorityStandard),
    iObserver(aObserver)
    {
    CActiveScheduler::Add( this );
    }
                                  
//----------------------------------------------------------------
// CLbsLocationResolverImpl::ConstructL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocationResolverImpl::ConstructL( const TUid aConverterModuleId )
    {
    iConversionModuleId = aConverterModuleId;
    User::LeaveIfError( iLocMoniterSession.Connect() );
    iLocInfoConverter.OpenL( iLocMoniterSession );
    }

//----------------------------------------------------------------
// CLbsLocationResolverImpl::ConvertLocationInfoL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocationResolverImpl::ResolveLocationL(const CLbsLocationInfoBase& aLocationInfo)
    {
    if(IsActive())
        {
        User::Leave(KErrInUse);
        }
    
    CBufFlat* buffer = CBufFlat::NewL(512);
    CleanupStack::PushL(buffer);
    RBufWriteStream writeStream;
    writeStream.Open(*buffer);
    CleanupClosePushL(writeStream);
        
        
    aLocationInfo.ExternaliseL(writeStream);
    writeStream.CommitL();
            
    RBufReadStream readStream(*buffer);
    CleanupClosePushL(readStream);
    iLocationInfo = CLbsLocationInfo::NewL();
    iLocationInfo->InternaliseL(readStream);
    
    iLocInfoConverter.ConvertLocationInfoL(*iLocationInfo, iConversionPrefs, 
                                            ELbsConversionOutputPosition,
                                           iConversionModuleId, iStatus);
    CleanupStack::PopAndDestroy(&readStream);
    CleanupStack::PopAndDestroy(&writeStream);
    CleanupStack::PopAndDestroy(buffer);
    
    SetActive();
    }



//----------------------------------------------------------------
// CLbsLocationResolverImpl::CancelConvertLocationInfoL
// (other items were commented in a header).
//-------;---------------------------------------------------------
void CLbsLocationResolverImpl::CancelResolveLocation()
    {   
    Cancel();
    }

//----------------------------------------------------------------
// CLbsLocationResolverImpl::RunL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocationResolverImpl::RunL()
    {   
    TLocality result;
    
    if(iStatus.Int() == KErrNone)
      {
      if(iLocationInfo)
        {
        RLbsAreaInfoBaseArray areaInfoArray;
     	CleanupStack::PushL(TCleanupItem(*CleanUpAreaInfoArray, &areaInfoArray ));
        iLocationInfo->GetAreaInfoL(areaInfoArray, CLbsLocationInfo::ELbsPosInfo);
        TInt count = areaInfoArray.Count();
        if(count >0)
          {
          CLbsPositionInfo* posInfo = static_cast<CLbsPositionInfo*>(areaInfoArray[0]);
          posInfo->GetLocality(result); 
          }         
        else
          {
          iObserver.OnResolveLocationComplete(KErrGeneral, result);        
          }  
        CleanupStack::PopAndDestroy( &areaInfoArray );
        delete iLocationInfo;
        iLocationInfo = NULL;
        }
      }
    if(iStatus.Int() != KErrCancel)
        {
        iObserver.OnResolveLocationComplete(iStatus.Int(), result);
        }
    }

//----------------------------------------------------------------
// CLbsLocationResolverImpl::DoCancel
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocationResolverImpl::DoCancel()
    {   
    iLocInfoConverter.CancelConvertLocationInfo();
    if(iLocationInfo)
        {
        delete iLocationInfo;
        iLocationInfo = NULL;
        }
    }


//----------------------------------------------------------------
// CLbsLocationResolverImpl::SetConversionPreferences
// (other items were commented in a header).
//----------------------------------------------------------------
TInt CLbsLocationResolverImpl::SetConversionPreferences(TLbsConversionPrefs aConversionPrefs)
    {
    TInt retVal = KErrNone;
    //The acceptable values for the TLbsConversionPrefs Enums are such that only one of the 32 bits is set
    //and bit combinations are not permitted.
    TLbsConversionPrefs copyConvPrefs = aConversionPrefs;
    TUint bitCount = 0;
    for (TUint count = 1; count <= 32; count++)
        {
        //Check if the LeastSignificantBit is set to 1
        if ( (copyConvPrefs) & (1) )
            {
            bitCount++;
            if (bitCount > 1)
                {
                break;
                }
            }
        copyConvPrefs = (copyConvPrefs >> 1);
        if (copyConvPrefs == 0)
            {
            break;
            }
        }

    //Check if only one of the 32 bits is set in aConversionPref
    if (bitCount == 1)
        {
        iConversionPrefs = aConversionPrefs;
        }
    else
        {
        retVal = KErrArgument;
        }

    return retVal;
    }

//----------------------------------------------------------------
// CLbsLocationResolverImpl::ConversionPreferences
// (other items were commented in a header).
//----------------------------------------------------------------
 TLbsConversionPrefs CLbsLocationResolverImpl::ConversionPreferences()
    {
    return iConversionPrefs;
    }



// End of file

