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
#include "lbsassistancedatabuildersetimpl.h"
#include <lbs/lbsassistancedatabuilderset.h>

RLbsAssistanceDataBuilderSetImpl::RLbsAssistanceDataBuilderSetImpl()
{
	
}

// Opens all the builders
void RLbsAssistanceDataBuilderSetImpl::OpenL()
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

// Frees memory used by the builders
void RLbsAssistanceDataBuilderSetImpl::Close()
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

void RLbsAssistanceDataBuilderSetImpl::ClearAllBuilders()
	{
	iUtcModel.ClearAllFields();
	iReferenceTime.ClearAllFields();
	iReferenceLocation.ClearAllFields();
	iNavigationModel.ClearAllFields();
	iIonosphericModel.ClearAllFields();
	iBadSatList.ClearAllFields();
	iAquisitionAssistance.ClearAllFields();
	iAlmanac.ClearAllFields();
	}

void RLbsAssistanceDataBuilderSetImpl::ExternalizeL(RWriteStream &aStream)
	{

	TUint32 mask = 0; // To mark presence/absence of each builder in the stream
	
	// Build a mask to tell what builders
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
    
    // Externalize each builder that contains data
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

void RLbsAssistanceDataBuilderSetImpl::InternalizeL(RReadStream &aStream)
	{
	TUint32 mask;
	
	//Internalize mask
    mask = aStream.ReadUint32L();
    TPtr8 ptr(NULL,0);
    
    // Internalize builders in the stream
    //
    if (mask & EAssistanceDataAlmanac)
		{
		ptr.Set(iAlmanac.DataBufferAccess());
		aStream >> ptr;
		}
	
	if (mask & EAssistanceDataAquisitionAssistance)
		{
		ptr.Set(iAquisitionAssistance.DataBufferAccess());
		aStream >> ptr;
		}
	
	if (mask & EAssistanceDataBadSatList)
		{
		ptr.Set(iBadSatList.DataBufferAccess());
		aStream >> ptr;
		}
	
	if (mask & EAssistanceDataIonosphericModel)
		{
		ptr.Set(iIonosphericModel.DataBufferAccess());
		aStream >> ptr;
		}
	
	if (mask & EAssistanceDataNavigationModel)
		{
		ptr.Set(iNavigationModel.DataBufferAccess());
		aStream >> ptr;
		}
	
	if (mask & EAssistanceDataReferenceLocation)
		{
		ptr.Set(iReferenceLocation.DataBufferAccess());
		aStream >> ptr;
		}
		
	if (mask & EAssistanceDataReferenceTime)
		{
		ptr.Set(iReferenceTime.DataBufferAccess());
		aStream >> ptr;
		}

	if (mask & EAssistanceDataPositioningGpsUtcModel)
		{
		ptr.Set(iUtcModel.DataBufferAccess());
		aStream >> ptr;
		}
	}

/**
 For each builder in the set, compares the timestamps of the builder in aSet and the
 corresponding builder in this set. If the builder in aSet is newer, copy that builder
 into this set, replacing the existing one.
 @param
 @return
 */
void RLbsAssistanceDataBuilderSetImpl::MergeL(RLbsAssistanceDataBuilderSet& aSet)
{
	// Merge iAlmanac
	{
	RUEPositioningGpsAlmanacBuilder* almanacBuilderPtr;
	aSet.GetDataBuilder(almanacBuilderPtr);
	if (almanacBuilderPtr)
		{
		if(almanacBuilderPtr->IsDataAvailable())
			{
			// Incoming data available. Ensure this set's builder is open.
			if (!iAlmanac.IsOpen())
				{
				// Create a buffer. Time stamp is set to zero.
				iAlmanac.OpenL();
				}

			if (iAlmanac.TimeStamp() < almanacBuilderPtr->TimeStamp())
				{
				// Builder in aSet is newer. Copy builder in aSet into this one
				iAlmanac.SetTimeStamp(almanacBuilderPtr->TimeStamp());
				iAlmanac.DataBufferAccess() = almanacBuilderPtr->DataBuffer();
				}
			}
		}
	}
		
	// Merge iIonosphericModel
	{
	RUEPositioningGpsIonosphericModelBuilder* ionosphericModelPtr;
	aSet.GetDataBuilder(ionosphericModelPtr);
	if (ionosphericModelPtr)
		{
		if(ionosphericModelPtr->IsDataAvailable())
			{
			// Incoming data available. Ensure this set's builder is open.
			if (!iIonosphericModel.IsOpen())
				{
				// Create a buffer. Time stamp is set to zero.
				iIonosphericModel.OpenL();
				}

			if (iIonosphericModel.TimeStamp() < ionosphericModelPtr->TimeStamp())
				{
				// Builder in aSet is newer. Copy builder in aSet into this one
				iIonosphericModel.SetTimeStamp(ionosphericModelPtr->TimeStamp());
				iIonosphericModel.DataBufferAccess() = ionosphericModelPtr->DataBuffer();
				}
			}
		}
	}
	
	// Merge iNavigationModel
	{
	RUEPositioningGpsNavigationModelBuilder* navigationModelPtr;
	aSet.GetDataBuilder(navigationModelPtr);
	if (navigationModelPtr)
		{
		if(navigationModelPtr->IsDataAvailable())
			{
			// Incoming data available. Ensure this set's builder is open.
			if (!iNavigationModel.IsOpen())
				{
				// Create a buffer. Time stamp is set to zero.
				iNavigationModel.OpenL();
				}

			if (iNavigationModel.TimeStamp() < navigationModelPtr->TimeStamp())
				{
				// Builder in aSet is newer. Copy builder in aSet into this one
				iNavigationModel.SetTimeStamp(navigationModelPtr->TimeStamp());
				iNavigationModel.DataBufferAccess() = navigationModelPtr->DataBuffer();
				}
			}
		}
	}
	
	// Merge iReferenceTime
	{
	RUEPositioningGpsReferenceTimeBuilder* referenceTimePtr;
	aSet.GetDataBuilder(referenceTimePtr);
	if (referenceTimePtr)
		{
		if(referenceTimePtr->IsDataAvailable())
			{
			// Incoming data available. Ensure this set's builder is open.
			if (!iReferenceTime.IsOpen())
				{
				// Create a buffer. Time stamp is set to zero.
				iReferenceTime.OpenL();
				}

			if (iReferenceTime.TimeStamp() < referenceTimePtr->TimeStamp())
				{
				// Builder in aSet is newer. Copy builder in aSet into this one
				iReferenceTime.SetTimeStamp(referenceTimePtr->TimeStamp());
				iReferenceTime.DataBufferAccess() = referenceTimePtr->DataBuffer();
				}
			}
		}
	}
	
	// Merge iUtcModel
	{
	RUEPositioningGpsUtcModelBuilder* utcModelPtr;
	aSet.GetDataBuilder(utcModelPtr);
	if (utcModelPtr)
		{
		if(utcModelPtr->IsDataAvailable())
			{
			// Incoming data available. Ensure this set's builder is open.
			if (!iUtcModel.IsOpen())
				{
				// Create a buffer. Time stamp is set to zero.
				iUtcModel.OpenL();
				}

			if (iUtcModel.TimeStamp() < utcModelPtr->TimeStamp())
				{
				// Builder in aSet is newer. Copy builder in aSet into this one
				iUtcModel.SetTimeStamp(utcModelPtr->TimeStamp());
				iUtcModel.DataBufferAccess() = utcModelPtr->DataBuffer();
				}
			}
		}
	}
	
	// Merge iAquisitionAssistance
	{
	RUEPositioningGpsAcquisitionAssistanceBuilder* aquisitionAssistancePtr;
	aSet.GetDataBuilder(aquisitionAssistancePtr);
	if (aquisitionAssistancePtr)
		{
		if(aquisitionAssistancePtr->IsDataAvailable())
			{
			// Incoming data available. Ensure this set's builder is open.
			if (!iAquisitionAssistance.IsOpen())
				{
				// Create a buffer. Time stamp is set to zero.
				iAquisitionAssistance.OpenL();
				}

			if (iAquisitionAssistance.TimeStamp() < aquisitionAssistancePtr->TimeStamp())
				{
				// Builder in aSet is newer. Copy builder in aSet into this one
				iAquisitionAssistance.SetTimeStamp(aquisitionAssistancePtr->TimeStamp());
				iAquisitionAssistance.DataBufferAccess() = aquisitionAssistancePtr->DataBuffer();
				}
			}
		}
	}
	
	// Merge iBadSatList
	{
	RBadSatListBuilder* badSatListPtr;
	aSet.GetDataBuilder(badSatListPtr);
	if (badSatListPtr)
		{
		if(badSatListPtr->IsDataAvailable())
			{
			// Incoming data available. Ensure this set's builder is open.
			if (!iBadSatList.IsOpen())
				{
				// Create a buffer. Time stamp is set to zero.
				iBadSatList.OpenL();
				}

			if (iBadSatList.TimeStamp() < badSatListPtr->TimeStamp())
				{
				// Builder in aSet is newer. Copy builder in aSet into this one
				iBadSatList.SetTimeStamp(badSatListPtr->TimeStamp());
				iBadSatList.DataBufferAccess() = badSatListPtr->DataBuffer();
				}
			}
		}
	}
	
	// Merge iReferenceLocation
	{
	RReferenceLocationBuilder* referenceLocationPtr;
	aSet.GetDataBuilder(referenceLocationPtr);
	if (referenceLocationPtr)
		{
		if(referenceLocationPtr->IsDataAvailable())
			{
			// Incoming data available. Ensure this set's builder is open.
			if (!iReferenceLocation.IsOpen())
				{
				// Create a buffer. Time stamp is set to zero.
				iReferenceLocation.OpenL();
				}

			if (iReferenceLocation.TimeStamp() < referenceLocationPtr->TimeStamp())
				{
				// Builder in aSet is newer. Copy builder in aSet into this one
				iReferenceLocation.SetTimeStamp(referenceLocationPtr->TimeStamp());
				iReferenceLocation.DataBufferAccess() = referenceLocationPtr->DataBuffer();
				}
			}
		}
	}
}

void RLbsAssistanceDataBuilderSetImpl::GetDataBuilder(RUEPositioningGpsAlmanacBuilder*& aBuilder)
	{
	 aBuilder = &iAlmanac;
	}

void RLbsAssistanceDataBuilderSetImpl::GetDataBuilder(RUEPositioningGpsIonosphericModelBuilder*& aBuilder)
	{
	aBuilder = &iIonosphericModel;
	}

void RLbsAssistanceDataBuilderSetImpl::GetDataBuilder(RUEPositioningGpsNavigationModelBuilder*& aBuilder)
	{
	aBuilder = &iNavigationModel;
	}

void RLbsAssistanceDataBuilderSetImpl::GetDataBuilder(RUEPositioningGpsReferenceTimeBuilder*& aBuilder)
	{
	aBuilder = &iReferenceTime;
	}

void RLbsAssistanceDataBuilderSetImpl::GetDataBuilder(RUEPositioningGpsUtcModelBuilder*& aBuilder)
	{
	aBuilder = &iUtcModel;
	}

void RLbsAssistanceDataBuilderSetImpl::GetDataBuilder(RUEPositioningGpsAcquisitionAssistanceBuilder*& aBuilder)
	{
	aBuilder = &iAquisitionAssistance;
	}

void RLbsAssistanceDataBuilderSetImpl::GetDataBuilder(RBadSatListBuilder*& aBuilder)
	{
	aBuilder = &iBadSatList;
	}

void RLbsAssistanceDataBuilderSetImpl::GetDataBuilder(RReferenceLocationBuilder*& aBuilder)
	{
	aBuilder = &iReferenceLocation;
	}
