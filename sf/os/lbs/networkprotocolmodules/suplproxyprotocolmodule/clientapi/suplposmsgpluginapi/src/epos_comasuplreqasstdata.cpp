/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Provides the interface between SUPL Framework and POS
*				 Message plugin. 
*
*/



// INCLUDE FILES

#include "epos_comasuplreqasstdata.h"
#include "epos_eomasuplposerrors.h"

// Constant definitions
const TInt KMINSatId = 0;
const TInt KMAXSatId = 63;

const TInt KMINIODE = 0;
const TInt KMAXIODE = 255;

const TInt KMINGpsWeek = 0;
const TInt KMAXGpsWeek = 1023;

const TInt KMINGpsToe = 0;
const TInt KMAXGpsToe = 167;

const TInt KMINNSat = 0;
const TInt KMAXNSat = 31;

const TInt KMINToeLimit = 0;
const TInt KMAXToeLimit = 10;


// ============================ MEMBER FUNCTIONS OF TOMASuplSatelliteInfoElement ===============================

// -----------------------------------------------------------------------------
// TOMASuplSatelliteInfoElement::TOMASuplSatelliteInfoElement,constuctor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TOMASuplSatelliteInfoElement::TOMASuplSatelliteInfoElement()
	{   
		iSatId = -1;
		iIode  = -1;
	}
// -----------------------------------------------------------------------------
// TOMASuplSatelliteInfoElement::GetSatInfoElement()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TOMASuplSatelliteInfoElement::GetSatInfoElement(TInt& aSatId,TInt& aIode)const 
	{
		aSatId = iSatId;
		aIode  = iIode;
	}
// -----------------------------------------------------------------------------
// TOMASuplSatelliteInfoElement::SetSatInfoElement()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TOMASuplSatelliteInfoElement::SetSatInfoElement(TInt aSatId,TInt aIode)
	{
		if(aSatId < KMINSatId || aSatId > KMAXSatId)
		{
			return KErrArgument;
		}
		
		if(aIode < KMINIODE || aIode > KMAXIODE)
		{
			return KErrArgument;
		}
		
		iSatId = aSatId;
		iIode  = aIode;	 
		
		return KErrNone;
	}

// ============================ MEMBER FUNCTIONS OF TOMASuplSatelliteInfoElement ===============================

// -----------------------------------------------------------------------------
// TOMASuplSatelliteInfoElement::TOMASuplSatelliteInfoElement ,Constuctor
// -----------------------------------------------------------------------------
//

EXPORT_C TOMASuplNavigationModel::TOMASuplNavigationModel()
	{
		iGPSWeek  = -1;
     	iGPSToe   = -1;
     	iNSat     = -1;
     	iToeLimit = -1;
     	iSatInfoPresent = EFalse;	
	}

// -----------------------------------------------------------------------------
// TOMASuplSatelliteInfoElement::TOMASuplSatelliteInfoElement ,desstuctor
// -----------------------------------------------------------------------------
//
EXPORT_C TOMASuplNavigationModel::~TOMASuplNavigationModel()
	{
		iSatInfo.Close();
	}
// -----------------------------------------------------------------------------
// COMASuplReqAsstData::GetSatInfoElement
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TInt TOMASuplNavigationModel::GetSatInfoElement(RArray<TOMASuplSatelliteInfoElement>& aSatInfo) const
    {
    	if(iSatInfoPresent)
    		{
    		TInt noOfElems = iSatInfo.Count();
    		aSatInfo.Reset();
    		for(TInt i = 0; i<noOfElems; i++)
    			{
    			TInt err = aSatInfo.Append(iSatInfo[i]);
				if(err != KErrNone)
					{
					return err;
					}
    			}    		
    		return KErrNone;
    		}
    	else
    		{
    		return KErrOMASuplParamNotSet;
    		}
    	
    }
// -----------------------------------------------------------------------------
// TOMASuplSatelliteInfoElement::SetSatInfoElement
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void TOMASuplNavigationModel::SetSatInfoElement(const RArray<TOMASuplSatelliteInfoElement>& aSatInfo )
	{
		TInt noOfElems = aSatInfo.Count();
		iSatInfo.Reset();
		
    	for(TInt i = 0; i<noOfElems; i++)
			{
				TInt err = iSatInfo.Append(aSatInfo[i]);
				if( err != KErrNone )
					{
					// Nothing can be done
					return;
					}
			}   
			 	
		iSatInfoPresent = ETrue;	
	}

// -----------------------------------------------------------------------------
// TOMASuplSatelliteInfoElement::overloaded assignment operator
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TOMASuplNavigationModel& TOMASuplNavigationModel::operator=(const TOMASuplNavigationModel& aNaviModel)	
	{
	iGPSWeek = aNaviModel.iGPSWeek;
	iGPSToe = aNaviModel.iGPSToe; 
	iNSat = aNaviModel.iNSat ;
	iToeLimit = aNaviModel.iToeLimit;
	iSatInfoPresent = aNaviModel.iSatInfoPresent;
	
	if(iSatInfoPresent)
		{
		iSatInfo.Reset();
		for(TInt i=0; i< aNaviModel.iSatInfo.Count();++i)
			{
				TRAP_IGNORE( iSatInfo.AppendL(aNaviModel.iSatInfo[i]) );
			}
			
		}
		
	return *this;
	
	}
// -----------------------------------------------------------------------------
// COMASuplReqAsstData::GetSatInfoElement
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TInt TOMASuplNavigationModel::GetSatInfoElement(TOMASuplSatelliteInfoElement& aSatInfo,
														 TInt aIndex) const
    {
    	if(iSatInfoPresent)
    		{
    		TInt noOfElems = iSatInfo.Count();
    		if(aIndex <= noOfElems)
    			{
    			aSatInfo = iSatInfo[aIndex];	
    			return KErrNone;	
    			}  
    		else
    			{
    			return KErrArgument;
    			}  		    		
    		}
    	else
    		{
    		return KErrOMASuplParamNotSet;
    		}
    	
    }
// -----------------------------------------------------------------------------
// COMASuplReqAsstData::SetSatInfoElement
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TInt TOMASuplNavigationModel::SetSatInfoElement(const TOMASuplSatelliteInfoElement& aSatInfo,
														 TInt aIndex )
	{
		TInt noOfElems = iSatInfo.Count();
		TInt error = KErrNone;
    	if(aIndex <= noOfElems)
			{
			error = iSatInfo.Insert(aSatInfo,aIndex);
			}    	
		else
			{
			error = iSatInfo.Append(aSatInfo);
			}
			
		iSatInfoPresent = ETrue;	
		return error;		
	}
// -------------------------------------------------------------z----------------
// COMASuplReqAsstData::GetNavigationModel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void TOMASuplNavigationModel::GetNavigationModel(TInt& aGpsWeek,TInt& aGpsToe,
														  TInt& aNSat,TInt& aToeLimit) const
    {
    	aGpsWeek  = iGPSWeek;
     	aGpsToe   = iGPSToe;
     	aNSat     = iNSat;
     	aToeLimit = iToeLimit;
    }
// -----------------------------------------------------------------------------
// TOMASuplSatelliteInfoElement::SetNavigationModel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TInt TOMASuplNavigationModel::SetNavigationModel(TInt aGpsWeek,TInt aGpsToe,
														  TInt aNSat,TInt aToeLimit)
	{
		if(aGpsWeek < KMINGpsWeek || aGpsWeek > KMAXGpsWeek)
		{
			return KErrArgument;
		}
		
		if(aGpsToe < KMINGpsToe || aGpsToe > KMAXGpsToe)
		{
			return KErrArgument;
		}
		
		if(aNSat < KMINNSat || aNSat > KMAXNSat)
		{
			return KErrArgument;
		}
		
		if(aToeLimit < KMINToeLimit || aToeLimit > KMAXToeLimit)
		{
			return KErrArgument;
		}
		
	   	iGPSWeek  = aGpsWeek;
     	iGPSToe   = aGpsToe;
     	iNSat     = aNSat;
     	iToeLimit = aToeLimit;		
     	
     	return KErrNone;
	}
// ============================ MEMBER FUNCTIONS OF COMASuplReqAsstData ===============================

// -----------------------------------------------------------------------------
// TOMASuplSatelliteInfoElement::TOMASuplSatelliteInfoElement ,Constuctor
// -----------------------------------------------------------------------------
//

EXPORT_C COMASuplReqAsstData::COMASuplReqAsstData()
	{
		iNavigationModelRequested   = EFalse;
		iAlmanacRequested 		    = EFalse;
		iUtcModelRequested 		    = EFalse;
		iIonosphericModelRequested  = EFalse;
		iDgpsCorrectionsRequested   = EFalse;
		iReferenceLocationRequested = EFalse;
		iReferenceTimeRequested 	= EFalse;
		iAcquisitionAssistance 		= EFalse;
		iRealTimeIntegrityRequested = EFalse;
		SetType(COMASuplInfoRequest::EOMASuplReqAsstData);
	}
//
// ---------------------------------------------------------
//  destructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplReqAsstData::~COMASuplReqAsstData()
    {
    }
    
//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  COMASuplReqAsstData* COMASuplReqAsstData::NewL()
    {
    COMASuplReqAsstData* self = new(ELeave) COMASuplReqAsstData;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}           

//
// -------------------------------------------------------------
// Two Phase constructor
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
void COMASuplReqAsstData::ConstructL()
    {
	}        
// -----------------------------------------------------------------------------
// TOMASuplSatelliteInfoElement::GetNavigationData
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TInt COMASuplReqAsstData::GetNavigationData(TOMASuplNavigationModel& aNaviModel) const
    {
    	if(iNavigationModelRequested)
    		{
    		aNaviModel = iNavigationData;
    		return KErrNone;
    		}
    	else
    		{
    		return KErrOMASuplParamNotSet;
    		}
    	
    }
// -----------------------------------------------------------------------------
// TOMASuplSatelliteInfoElement::SetSatInfoElement
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void COMASuplReqAsstData::SetNavigationData(const TOMASuplNavigationModel& aNaviModel)
	{
		iNavigationData = aNaviModel;
		iNavigationModelRequested = ETrue;	
	}
// -----------------------------------------------------------------------------
// COMASuplReqAsstData::GetReqAsstData
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void COMASuplReqAsstData::GetReqAsstData(TBool& aAlmanacReq,TBool& aUtcModel,TBool& aIonModel,
												  TBool& aDgpsCorrect,TBool& aRefeLocation,
												  TBool& aReferenceTimeRequested,TBool& aAcquisition,
		        					   			  TBool& aRealTime)const
    {
      aAlmanacReq 	=  iAlmanacRequested;
	  aUtcModel 	= 	 iUtcModelRequested;
      aIonModel 	= 	 iIonosphericModelRequested ;
      aDgpsCorrect 	=  	 iDgpsCorrectionsRequested ;
      aRefeLocation = 	 iReferenceLocationRequested;
      aReferenceTimeRequested =   iReferenceTimeRequested;
      aAcquisition 	=    iAcquisitionAssistance;
      aRealTime 	=  iRealTimeIntegrityRequested; 
    }
// -----------------------------------------------------------------------------
// TOMASuplSatelliteInfoElement::SetNavigationModel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void COMASuplReqAsstData::SetReqAsstData(TBool aAlmanacReq,TBool aUtcModel,TBool aIonModel,
												  TBool aDgpsCorrect,TBool aRefeLocation,
												  TBool aReferenceTimeRequested,TBool aAcquisition,
        					    		  		  TBool aRealTime)
	{
	   	  iAlmanacRequested 		  = aAlmanacReq;
    	  iUtcModelRequested    	  = aUtcModel;
		  iIonosphericModelRequested  = aIonModel;
		  iDgpsCorrectionsRequested   = aDgpsCorrect;
		  iReferenceLocationRequested = aRefeLocation;
		  iReferenceTimeRequested 	  = aReferenceTimeRequested;
		  iAcquisitionAssistance 	  = aAcquisition;
		  iRealTimeIntegrityRequested = aRealTime;
	}

// -----------------------------------------------------------------------------
// COMASuplReqAsstData::CloneL() 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
EXPORT_C COMASuplInfoRequest* COMASuplReqAsstData::CloneL()const
	{
	COMASuplReqAsstData*	cloneReqAsstData = COMASuplReqAsstData::NewL();

	cloneReqAsstData->iAlmanacRequested = iAlmanacRequested;
	cloneReqAsstData->iUtcModelRequested = iUtcModelRequested;
	cloneReqAsstData->iIonosphericModelRequested = iIonosphericModelRequested;
	cloneReqAsstData->iDgpsCorrectionsRequested = iDgpsCorrectionsRequested;
	cloneReqAsstData->iReferenceLocationRequested = iReferenceLocationRequested;
	cloneReqAsstData->iReferenceTimeRequested = iReferenceTimeRequested;
	cloneReqAsstData->iAcquisitionAssistance = iAcquisitionAssistance;
	cloneReqAsstData->iRealTimeIntegrityRequested = iRealTimeIntegrityRequested;
	
	cloneReqAsstData->iNavigationData = iNavigationData;
	cloneReqAsstData->iNavigationModelRequested = iNavigationModelRequested;	
	
	
	cloneReqAsstData->SetStatus(Status());
    cloneReqAsstData->SetType(Type());	
	return (cloneReqAsstData);
	}
	
//  End  of file

