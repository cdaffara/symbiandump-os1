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
// utility fucntions, and odds and sods for various bits of the location server
// 
//

#include <lbs/lbsextendedsatellite.h>
#include "utilfunctions.h"
#include "LbsInternalInterface.h"

/**
Deap copy of position info data by type

@param aTo the destination position info object reference
@param aFrom the source position info object reference
@internalComponent
@released
*/
TInt CopyPositionTypes(TPositionInfoBase& aTo, const TPositionInfoBase& aFrom)	
	{
	const TUint32 toClassType = aTo.PositionClassType();
	const TUint toClassSize = aTo.PositionClassSize();
	const TUint32 fromClassType = aFrom.PositionClassType();
	const TUint fromClassSize = aFrom.PositionClassSize();
	
	// check for self assignment
	if(&aTo == &aFrom)
		{
		return KErrNone; // copy is implicit and already done :)
		}	
	// check the assignment is even possible the source type must be at
	// at least as BIG as the dest, or  you leave unitialized data in the destination
	if(fromClassSize < toClassSize)
		return KErrArgument;
	return CopyPositionTypes2(aTo,aFrom);
	}
	
	/**
Deap copy of position info data by type - same as CopyPositionTypes2 but
allows for  source posituin to be smaller than destination position

@param aTo the destination position info object reference
@param aFrom the source position info object reference
@internalComponent
@released
*/
TInt CopyPositionTypes2(TPositionInfoBase& aTo, const TPositionInfoBase& aFrom)	
	{
	const TUint32 toClassType = aTo.PositionClassType();
	const TUint toClassSize = aTo.PositionClassSize();
	const TUint32 fromClassType = aFrom.PositionClassType();
	const TUint fromClassSize = aFrom.PositionClassSize();
	
	// check for self assignment
	if(&aTo == &aFrom)
		{
		return KErrNone; // copy is implicit and already done :)
		}	
	// check the assignment is even possible the source type must be at
	// at least as BIG as the dest, or  you leave unitialized data in the destination
//	if(fromClassSize < toClassSize)
//		return KErrArgument;
	// check we are not assigning base types
	if(toClassSize == sizeof(TPositionInfoBase)
			|| fromClassSize == sizeof(TPositionInfoBase)
			|| toClassType == EPositionInfoUnknownClass // this the type of a TPositionInfoBase
			|| fromClassType == EPositionInfoUnknownClass)
		{
		return KErrArgument; // bad types - trying to copy between base types
		}
	// check the aTo type
	TInt typeError = SupportedType(toClassType, toClassSize);
	if(typeError != KErrNone)
		return typeError; // i.e. KErrNotSupported
	// check the aFromType
	typeError = SupportedType(fromClassType, fromClassSize);
	if(typeError != KErrNone)
		return typeError; // i.e. KErrNotSupported
	// Ok now we know we can copy these things from one to the other
	// so here goes - the catch is we cannot overwrite the type and size
	// of the destination, so we need a bit of pointer math.
	// NB this relies on the being a class inheritance such that all of the
	// types are derived from TPositionInfoBase first. Or the math goes
	// haywire. This also implies a whole slew of assumptions about the ram
	// layout of these classes....
	TUint8* baseToAddr = reinterpret_cast<TUint8*>(&aTo)+sizeof(TPositionClassTypeBase);			
	const TUint8* const baseFromAddr = reinterpret_cast<const TUint8*>(&aFrom)+sizeof(TPositionClassTypeBase);
	TUint32 dataLength = toClassSize-sizeof(TPositionClassTypeBase); // we copy only this many bytes form the source
	TUint8* endAddr = Mem::Copy(baseToAddr, baseFromAddr, dataLength);
	// Sanity check the Mem::Copy() - just in case...
	if(endAddr != baseToAddr+dataLength)
		return KErrGeneral; // Or KErrNoMemory?
	else
		return KErrNone;
	}

/**
Check the size for supported position info type

@param aType the position info type
@param aSize the size of specified type
@return Symbian standard error code
@internalComponent
@released
*/
TInt SupportedType(const TUint32& aType, const TInt& aSize)
	{
	if(aType==EPositionInfoClass)
		{
		if(aSize!=sizeof(TPositionInfo))
			return KErrNotSupported; // something weird. Type ok but the size is wrong			
		}
	else if(aType==(EPositionInfoClass|EPositionCourseInfoClass))
		{
		if(aSize!=sizeof(TPositionCourseInfo))
			return KErrNotSupported; // something weird. Type ok but the size is wrong			
		}
	else if(aType==(EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass))
		{
		if(aSize!=sizeof(TPositionSatelliteInfo))
			return KErrNotSupported; // something weird. Type ok but the size is wrong			
		}
	else if(aType==(EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass))
	        {
	        if(aSize!=sizeof(TPositionExtendedSatelliteInfo))
	            return KErrNotSupported; // something weird. Type ok but the size is wrong          
	        }
		
	// othwerwise the size and type are what we expected
	return KErrNone;
	}
	
/**
Loop over the module list and return the index of module with module ID aModuelId

@internalComponent
@released
*/
TInt GetIndexFromModuleIdL(const TUid& aModuleId)
	{
	TUint numOfModules;
	TPositionModuleInfo modInfo;
	User::LeaveIfError(LbsModuleInfo::GetNumModules(numOfModules));
	for(TInt i=0; i< numOfModules; ++i)
		{
		User::LeaveIfError(LbsModuleInfo::GetModuleInfoByIndex(i,&modInfo, sizeof(TPositionModuleInfo)));
		if(modInfo.ModuleId() == aModuleId)
			{
			return i;
			}
		}
	return KErrNotFound;
	}
