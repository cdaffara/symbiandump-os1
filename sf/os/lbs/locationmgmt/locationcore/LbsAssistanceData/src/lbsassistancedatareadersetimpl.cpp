// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32strm.h>
#include "lbsassistancedatareadersetimpl.h"
#include <lbs/lbsassistancedatareaderset.h>

RLbsAssistanceDataReaderSetImpl::RLbsAssistanceDataReaderSetImpl()
{
	
}

// Opens all the readers
void RLbsAssistanceDataReaderSetImpl::OpenL()
{
	iUtcModel.OpenL();
	iReferenceTime.OpenL();
	iReferenceLocation.OpenL();
	iNavigationModel.OpenL();
	iIonosphericModel.OpenL();
	iBadSatList.OpenL();
	iAquisitionAssistance.OpenL();
	iAlmanac.OpenL();
}

// Frees memory used by the readers
void RLbsAssistanceDataReaderSetImpl::Close()
{
	iUtcModel.Close();
	iReferenceTime.Close();
	iReferenceLocation.Close();
	iNavigationModel.Close();
	iIonosphericModel.Close();
	iBadSatList.Close();
	iAquisitionAssistance.Close();
	iAlmanac.Close();
}

void RLbsAssistanceDataReaderSetImpl::ExternalizeL(RWriteStream &aStream)
	{

	TUint32 mask = 0; // To mark presence/absence of each reader in the stream
	
	// Build a mask to tell what readers
	// contain data.
    if (iAlmanac.IsDataAvailable())
    	{
    	mask |= EAssistanceDataAlmanac;
    	}
    if (iAquisitionAssistance.IsDataAvailable())
    	{
    	mask |= EAssistanceDataAquisitionAssistance;
    	}
    if (iBadSatList.IsDataAvailable())
    	{
    	mask |= EAssistanceDataBadSatList;
    	}
    if (iIonosphericModel.IsDataAvailable())
    	{
    	mask |= EAssistanceDataIonosphericModel;
    	}
    if (iNavigationModel.IsDataAvailable())
    	{
    	mask |= EAssistanceDataNavigationModel;
    	}
    if (iReferenceLocation.IsDataAvailable())
    	{
    	mask |= EAssistanceDataReferenceLocation;
    	}    	    	    	    	
    if (iReferenceTime.IsDataAvailable())
    	{
    	mask |= EAssistanceDataReferenceTime;
    	}
    if (iUtcModel.IsDataAvailable())
    	{
    	mask |= EAssistanceDataPositioningGpsUtcModel;
    	}

    // Externalize mask	
    aStream.WriteUint32L(mask);
    
    // Externalize each reader that contains data
    if (mask & EAssistanceDataAlmanac)
		{
		aStream << iAlmanac.DataBuffer();
		}
	if (mask & EAssistanceDataAquisitionAssistance)
		{
		aStream << iAquisitionAssistance.DataBuffer();
		}
	if (mask & EAssistanceDataBadSatList)
		{
		aStream << iBadSatList.DataBuffer();
		}
	if (mask & EAssistanceDataIonosphericModel)
		{
		aStream << iIonosphericModel.DataBuffer();
		}
	if (mask & EAssistanceDataNavigationModel)
		{
		aStream << iNavigationModel.DataBuffer();
		}
	if (mask & EAssistanceDataReferenceLocation)
		{
		aStream << iReferenceLocation.DataBuffer();
		}
	if (mask & EAssistanceDataReferenceTime)
		{	
		aStream << iReferenceTime.DataBuffer();
		}
	if (mask & EAssistanceDataPositioningGpsUtcModel)
		{	
		aStream << iUtcModel.DataBuffer();
		}
	}

void RLbsAssistanceDataReaderSetImpl::InternalizeL(RReadStream &aStream)
	{
	TUint32 mask;
	
	//Internalize mask
    mask = aStream.ReadUint32L();
    TPtr8 ptr(NULL,0);
    
    // Internalize readers in the stream
    //
    if (mask & EAssistanceDataAlmanac)
		{
		ptr.Set(iAlmanac.DataBuffer());
		aStream >> ptr;
		}
	
	if (mask & EAssistanceDataAquisitionAssistance)
		{
		ptr.Set(iAquisitionAssistance.DataBuffer());
		aStream >> ptr;
		}
	
	if (mask & EAssistanceDataBadSatList)
		{
		ptr.Set(iBadSatList.DataBuffer());
		aStream >> ptr;
		}
	
	if (mask & EAssistanceDataIonosphericModel)
		{
		ptr.Set(iIonosphericModel.DataBuffer());
		aStream >> ptr;
		}
	
	if (mask & EAssistanceDataNavigationModel)
		{
		ptr.Set(iNavigationModel.DataBuffer());
		aStream >> ptr;
		}
	
	if (mask & EAssistanceDataReferenceLocation)
		{
		ptr.Set(iReferenceLocation.DataBuffer());
		aStream >> ptr;
		}
		
	if (mask & EAssistanceDataReferenceTime)
		{
		ptr.Set(iReferenceTime.DataBuffer());
		aStream >> ptr;
		}

	if (mask & EAssistanceDataPositioningGpsUtcModel)
		{
		ptr.Set(iUtcModel.DataBuffer());
		aStream >> ptr;
		}
	}

void RLbsAssistanceDataReaderSetImpl::GetDataReader(RUEPositioningGpsAlmanacReader*& aReader)
	{
	 aReader = &iAlmanac;
	}

void RLbsAssistanceDataReaderSetImpl::GetDataReader(RUEPositioningGpsIonosphericModelReader*& aReader)
	{
	aReader = &iIonosphericModel;
	}

void RLbsAssistanceDataReaderSetImpl::GetDataReader(RUEPositioningGpsNavigationModelReader*& aReader)
	{
	aReader = &iNavigationModel;
	}

void RLbsAssistanceDataReaderSetImpl::GetDataReader(RUEPositioningGpsReferenceTimeReader*& aReader)
	{
	aReader = &iReferenceTime;
	}

void RLbsAssistanceDataReaderSetImpl::GetDataReader(RUEPositioningGpsUtcModelReader*& aReader)
	{
	aReader = &iUtcModel;
	}

void RLbsAssistanceDataReaderSetImpl::GetDataReader(RUEPositioningGpsAcquisitionAssistanceReader*& aReader)
	{
	aReader = &iAquisitionAssistance;
	}

void RLbsAssistanceDataReaderSetImpl::GetDataReader(RBadSatListReader*& aReader)
	{
	aReader = &iBadSatList;
	}

void RLbsAssistanceDataReaderSetImpl::GetDataReader(RReferenceLocationReader*& aReader)
	{
	aReader = &iReferenceLocation;
	}
