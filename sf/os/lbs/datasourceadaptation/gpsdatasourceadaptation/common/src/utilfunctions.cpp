// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
 @deprecated
*/

#include <lbssatellite.h>
#include "utilfunctions.h"
#include "nmeafunctions.h"

/**
Deep copy of position info data by type

@param aTo the destination position info object reference
@param aFrom the source position info object reference
*/
TInt CopyPositionTypes(TPositionInfoBase& aTo, const TPositionInfoBase& aFrom)	
	{
	// check for self assignment
	if(&aTo == &aFrom)
		{
		return KErrNone;
		}	

	return CopyPositionTypes2(aTo,aFrom);
	}
	
/**
Deep copy of position info data by type - 
allows for  source position to be smaller than destination position

@param aTo the destination position info object reference
@param aFrom the source position info object reference
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
		{
		return typeError;
		} // i.e. KErrNotSupported
	// check the aFromType
	typeError = SupportedType(fromClassType, fromClassSize);
	if(typeError != KErrNone)
		{
		return typeError;
		} // i.e. KErrNotSupported
	
	if(toClassType & EPositionGenericInfoClass)
		{
		return CopyGenericPositionType(aTo, aFrom);
		}
	else
		{
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
			{
			return KErrGeneral; // Or KErrNoMemory?
			}
		else
			{
			return KErrNone;
			}
		}
	}

TInt CopyGenericPositionType(TPositionInfoBase& aTo, const TPositionInfoBase& aFrom)	
	{
	TInt error;
    HPositionGenericInfo* posInfo = static_cast<HPositionGenericInfo*>(&aTo);
    
    // Have we got satellite info?
    if(aFrom.PositionClassType() & EPositionSatelliteInfoClass)	
    	{
    	const TPositionSatelliteInfo* satInfo = static_cast<const TPositionSatelliteInfo*>(&aFrom);
    	// copy any requested satellite info:
        if ( posInfo->IsRequestedField(EPositionFieldSatelliteNumInView) )
            {
            error = posInfo->SetValue(EPositionFieldSatelliteNumInView,static_cast<TInt8>(satInfo->NumSatellitesInView()));
            if(error != KErrNone)
            	{
            	return error;
            	}
            }       

        if ( posInfo->IsRequestedField(EPositionFieldSatelliteNumUsed) )
            {
            error = posInfo->SetValue(EPositionFieldSatelliteNumUsed, static_cast<TInt8>(satInfo->NumSatellitesUsed()));
            if(error != KErrNone)
            	{
            	return error;
            	}
            }

        if ( posInfo->IsRequestedField(EPositionFieldSatelliteTime) )
            {
            error = posInfo->SetValue(EPositionFieldSatelliteTime, satInfo->SatelliteTime());
            if(error != KErrNone)
            	{
            	return error;
            	}
            }

        if ( posInfo->IsRequestedField(EPositionFieldSatelliteHorizontalDoP) )
            {
            error = posInfo->SetValue(EPositionFieldSatelliteHorizontalDoP, static_cast<TReal32>(satInfo->HorizontalDoP()));
            if(error != KErrNone)
            	{
            	return error;
            	}
            }

        if ( posInfo->IsRequestedField(EPositionFieldSatelliteVerticalDoP) )
            {
            error = posInfo->SetValue(EPositionFieldSatelliteVerticalDoP, static_cast<TReal32>(satInfo->VerticalDoP()));
            if(error != KErrNone)
            	{
            	return error;
            	}
            }

        if ( posInfo->IsRequestedField(EPositionFieldSatelliteTimeDoP) )
            {
            error = posInfo->SetValue(EPositionFieldSatelliteTimeDoP, static_cast<TReal32>(satInfo->TimeDoP()));
            if(error != KErrNone)
            	{
            	return error;
            	}
            }

        if ( posInfo->IsRequestedField(EPositionFieldSatellitePositionDoP) )
            {
            TReal64 pdop;
            TReal64 hdop = satInfo->HorizontalDoP();
            TReal64 vdop = satInfo->VerticalDoP();
            if(!Math::IsNaN(hdop) && !Math::IsNaN(vdop))
                {
                Math::Sqrt(pdop, hdop*hdop + vdop*vdop);
                }
            else
                {
                TRealX nan;
                nan.SetNaN();
                pdop = nan;
                }
            error = posInfo->SetValue(EPositionFieldSatellitePositionDoP, static_cast<TReal32>(pdop));
            if(error != KErrNone)
                {
                return error;
                }
            }
    	}
    // Have we got course info?
    if(aFrom.PositionClassType() & EPositionCourseInfoClass)
    	{
        // copy any requested course info:
        const TPositionCourseInfo* courseInfo = static_cast<const TPositionCourseInfo*>(&aFrom);
        TCourse course;
        courseInfo->GetCourse(course);
        
        if ( posInfo->IsRequestedField(EPositionFieldHorizontalSpeed) )
            {
            error = posInfo->SetValue(EPositionFieldHorizontalSpeed, static_cast<TReal32>(course.Speed()));
            if(error != KErrNone)
            	{
            	return error;
            	}
            }
        if ( posInfo->IsRequestedField(EPositionFieldHorizontalSpeedError) )
            {
            error = posInfo->SetValue(EPositionFieldHorizontalSpeedError, static_cast<TReal32>(course.SpeedAccuracy()));
            if(error != KErrNone)
                {
                return error;
                }
            }
        if ( posInfo->IsRequestedField(EPositionFieldVerticalSpeed) )
            {
            error = posInfo->SetValue(EPositionFieldVerticalSpeed, static_cast<TReal32>(course.VerticalSpeed()));
            if(error != KErrNone)
                {
                return error;
                }
            }
        if ( posInfo->IsRequestedField(EPositionFieldVerticalSpeedError) )
            {
            error = posInfo->SetValue(EPositionFieldVerticalSpeedError, static_cast<TReal32>(course.VerticalSpeedAccuracy()));
            if(error != KErrNone)
                {
                return error;
                }
            }
        if ( posInfo->IsRequestedField(EPositionFieldTrueCourse) )
            {
            error = posInfo->SetValue(EPositionFieldTrueCourse, static_cast<TReal32>(course.Course()));
            if(error != KErrNone)
                {
                return error;
                }
            }
        if ( posInfo->IsRequestedField(EPositionFieldTrueCourseError) )
            {
            error = posInfo->SetValue(EPositionFieldTrueCourseError, static_cast<TReal32>(course.CourseAccuracy()));
            if(error != KErrNone)
                {
                return error;
                }
            }
        if ( posInfo->IsRequestedField(EPositionFieldHeading) )
            {
            error = posInfo->SetValue(EPositionFieldHeading, static_cast<TReal32>(course.Heading()));
            if(error != KErrNone)
            	{
            	return error;
            	}
            }
        if ( posInfo->IsRequestedField(EPositionFieldHeadingError) )
            {
            error = posInfo->SetValue(EPositionFieldHeadingError, static_cast<TReal32>(course.HeadingAccuracy()));
            if(error != KErrNone)
                {
                return error;
                }
            }
    	}
    // Have we got extended satellite info?
    if(aFrom.PositionClassType() & EPositionExtendedSatelliteInfoClass)
        {
        // copy any requested course info:
        const TPositionExtendedSatelliteInfo* extendedSatInfo = static_cast<const TPositionExtendedSatelliteInfo*>(&aFrom);

        if ( posInfo->IsRequestedField(EPositionFieldMagneticCourse) )
            {
            error = posInfo->SetValue(EPositionFieldMagneticCourse, static_cast<TReal32>(extendedSatInfo->CourseOverGroundMagnetic()));
            if(error != KErrNone)
                {
                return error;
                }
            }
        if ( posInfo->IsRequestedField(EPositionFieldMagneticCourseError) )
            {
            TCourse course;
            extendedSatInfo->GetCourse(course);
            error = posInfo->SetValue(EPositionFieldMagneticCourseError, static_cast<TReal32>(course.CourseAccuracy()));
            if(error != KErrNone)
                {
                return error;
                }
            }
        if ( posInfo->IsRequestedField(EPositionFieldMagneticHeading) )
            {
            TCourse course;
            extendedSatInfo->GetCourse(course);
            error = posInfo->SetValue(EPositionFieldMagneticHeading, static_cast<TReal32>(course.Heading() - extendedSatInfo->MagneticVariation()));
            if(error != KErrNone)
                {
                return error;
                }
            }
        if ( posInfo->IsRequestedField(EPositionFieldMagneticHeadingError) )
            {
            TCourse course;
            extendedSatInfo->GetCourse(course);
            error = posInfo->SetValue(EPositionFieldMagneticHeadingError, static_cast<TReal32>(course.HeadingAccuracy()));
            if(error != KErrNone)
                {
                return error;
                }
            }
        if ( posInfo->IsRequestedField(EPositionFieldSatelliteSeaLevelAltitude) )
            {
            TPosition pos;
            extendedSatInfo->GetPosition(pos);
            error = posInfo->SetValue(EPositionFieldSatelliteSeaLevelAltitude, static_cast<TReal32>(pos.Altitude() + extendedSatInfo->GeoidalSeparation()));
            if(error != KErrNone)
                {
                return error;
                }
            }
        if ( posInfo->IsRequestedField(EPositionFieldSatelliteGeoidalSeparation) )
            {
            error = posInfo->SetValue(EPositionFieldSatelliteGeoidalSeparation, static_cast<TReal32>(extendedSatInfo->GeoidalSeparation()));
            if(error != KErrNone)
                {
                return error;
                }
            }
        if ( posInfo->IsRequestedField(EPositionFieldNMEASentences) )
            {
            TBuf8<128> buffer;
            TUint8 sentences = 0;
            CreateGga(buffer, *extendedSatInfo);
            error = posInfo->SetValue(EPositionFieldNMEASentencesStart+sentences, buffer);
            if(error != KErrNone)
                {
                return error;
                }
            ++sentences;
            CreateGll(buffer, *extendedSatInfo);
            error = posInfo->SetValue(EPositionFieldNMEASentencesStart+sentences, buffer);
            if(error != KErrNone)
                {
                return error;
                }
            ++sentences;
            CreateGsa(buffer, *extendedSatInfo);
            error = posInfo->SetValue(EPositionFieldNMEASentencesStart+sentences, buffer);
            if(error != KErrNone)
                {
                return error;
                }
            ++sentences;
            CreateGst(buffer, *extendedSatInfo);
            error = posInfo->SetValue(EPositionFieldNMEASentencesStart+sentences, buffer);
            if(error != KErrNone)
                {
                return error;
                }
            ++sentences;
            // we need at least a GSV, and after the first one we'll know exactly how many are needed
            TInt totalGsvSentences = 1;
            TInt lastGsvSentence = 0;
            while(lastGsvSentence < totalGsvSentences)
                {
                CreateGsv(buffer, *extendedSatInfo, totalGsvSentences, lastGsvSentence);
                error = posInfo->SetValue(EPositionFieldNMEASentencesStart+sentences, buffer);
                if(error != KErrNone)
                    {
                    return error;
                    }
                ++sentences;                
                }
            CreateRmc(buffer, *extendedSatInfo);
            error = posInfo->SetValue(EPositionFieldNMEASentencesStart+sentences, buffer);
            if(error != KErrNone)
                {
                return error;
                }
            ++sentences;
            CreateVtg(buffer, *extendedSatInfo);
            error = posInfo->SetValue(EPositionFieldNMEASentencesStart+sentences, buffer);
            if(error != KErrNone)
                {
                return error;
                }
            ++sentences;
            error = posInfo->SetValue(EPositionFieldNMEASentences, sentences);
            if(error != KErrNone)
                {
                return error;
                }
            
            }
        }
    
    //copy position info 
    __ASSERT_DEBUG( (aTo.PositionClassType() & EPositionInfoClass) && (aFrom.PositionClassType() & EPositionInfoClass), User::Invariant());

    TPositionInfo* posInfoTo = static_cast<TPositionInfo*>(&aTo);
    const TPositionInfo* posInfoFrom = static_cast<const TPositionInfo*>(&aFrom);
    TPosition posFrom;
    posInfoFrom->GetPosition(posFrom);
    
    posInfoTo->SetPosition(posFrom);
    posInfoTo->SetModuleId(posInfoFrom->ModuleId());
    posInfoTo->SetPositionMode(posInfoFrom->PositionMode());
    posInfoTo->SetPositionModeReason(posInfoFrom->PositionModeReason());
    posInfoTo->SetUpdateType(posInfoFrom->UpdateType());
    
    return KErrNone;	
	}
/**
Check the size for supported position info type

@param aType the position info type
@param aSize the size of specified type
@return Symbian standard error code
*/
TInt SupportedType(const TUint32& aType, const TInt& aSize)
	{
	if(aType==EPositionInfoClass)
		{
		if(aSize!=sizeof(TPositionInfo))
			{
			return KErrNotSupported; // something weird. Type ok but the size is wrong
			}
		}
	else if(aType==(EPositionInfoClass|EPositionCourseInfoClass))
		{
		if(aSize!=sizeof(TPositionCourseInfo))
			{
			return KErrNotSupported; // something weird. Type ok but the size is wrong
			}
		}
	else if(aType==(EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass))
		{
		if(aSize!=sizeof(TPositionSatelliteInfo))
			{
			return KErrNotSupported; // something weird. Type ok but the size is wrong
			}
		}
	else if(aType==(EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass))
	    {
        if(aSize!=sizeof(TPositionExtendedSatelliteInfo))
            {
            return KErrNotSupported; // something weird. Type ok but the size is wrong
            }
	    }
	else if(aType == (EPositionInfoClass|EPositionGenericInfoClass))
		{
		if(aSize < sizeof(HPositionGenericInfo))
			{
			return KErrNotSupported; // something weird. Type ok but the size is wrong
			}
		}
	else	// no other types supported
		{
			return KErrNotSupported;	
		}
	// othwerwise the size and type are what we expected
	return KErrNone;
	}
	

TBool Partial(const TPosition& aPos)
	{
	TBool partial = EFalse;
	TReal64 lat = aPos.Latitude();
	TReal64 lng = aPos.Longitude();
	if(Math::IsNaN(lat) || Math::IsNaN(lng))
	{
		partial = ETrue;
	}
	return partial;
	}

//
