/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#include <e32base.h>
#include <lbsareainfo.h>
#include <lbserrors.h>


//-----------------------------------------------------------------------------
// TPositionAreaInfoBase
//-----------------------------------------------------------------------------
/** Default constructor for TPositionAreaInfoBase */
EXPORT_C TPositionAreaInfoBase::TPositionAreaInfoBase()
:	TPositionClassTypeBase()
	{
	iPosClassType = EPositionInfoUnknownClass;
	}


//-----------------------------------------------------------------------------
// TPositionAreaInfo
//-----------------------------------------------------------------------------
/** Default constructor for TPositionAreaInfo */
EXPORT_C TPositionAreaInfo::TPositionAreaInfo()
:	TPositionAreaInfoBase()
	{
	iPosClassType |= EPositionAreaInfoClass;
	iPosClassSize = sizeof(TPositionAreaInfo);
	}

/** Overloaded constructor for TPositionAreaInfo 
@param aArea [In] The area information
*/
EXPORT_C TPositionAreaInfo::TPositionAreaInfo(TPositionAreaInfo::TArea aArea)
:	TPositionAreaInfoBase(),
	iArea(aArea)
	{
	iPosClassType |= EPositionAreaInfoClass;
	iPosClassSize = sizeof(TPositionAreaInfo);
	}

/**
Returns area information
@return The area information
*/
EXPORT_C TPositionAreaInfo::TArea TPositionAreaInfo::Area() const
	{
	return iArea;
	}

/**
Sets area information
@param aArea [In] The area information
*/
EXPORT_C void TPositionAreaInfo::SetArea(TPositionAreaInfo::TArea aArea)
	{
	iArea = aArea;
	}


//-----------------------------------------------------------------------------
// TPositionAreaExtendedInfo
//-----------------------------------------------------------------------------
/** Constructor for TPositionAreaExtendedInfo
@param aArea [In] The area information
*/
EXPORT_C TPositionAreaExtendedInfo::
		 TPositionAreaExtendedInfo()
:	TPositionAreaInfo(),
	iMCC(EFalse),
	iMNC(EFalse),
	iLAC(EFalse),
	iCID(EFalse)
	{
	iPosClassType |= EPositionAreaExtendedInfoClass;
	iPosClassSize = sizeof(TPositionAreaExtendedInfo);
	}

/**
Returns Country Code match.
Country codes are network operator independent.

@return TRUE if the current Country Code and the Country Code associated
with the position match; FALSE otherwise.
*/
EXPORT_C TBool TPositionAreaExtendedInfo::MobileCountryCodeMatch() const
	{
	return iMCC;
	}
	
/**
Returns Mobile Network code match.

@return TRUE if the current Network Code and the Network Code associated
with the position match; FALSE otherwise.
*/
EXPORT_C TBool TPositionAreaExtendedInfo::MobileNetworkCodeMatch() const
	{
	return iMNC;
	}

/**
Returns Location Area code match.

Typically a Location Area consists of 10s or even 100s of Cells.

@return TRUE if the current Location Area Code and the Location Area Code associated
with the position match; FALSE otherwise.

@see CellIdMatch()
*/
EXPORT_C TBool TPositionAreaExtendedInfo::LocationAreaCodeMatch() const
	{
	return iLAC;
	}
	
/**
Returns Cell Id code match.

In practice cell radius varies from 10s of meters in buildings, through 100s of meters
in cities up to 10s of kilometres in countryside, but typically do not exceed 35 kilometres.

@return TRUE if the current Cell Id and the Cell Id associated
with the position match; FALSE otherwise.
*/
EXPORT_C TBool TPositionAreaExtendedInfo::CellIdMatch() const
	{
	return iCID;
	}
	
	
/**
Set whether a match for the Mobile Country Code was
found or not (this method is called by the server, not
the client)
*/
EXPORT_C void TPositionAreaExtendedInfo::SetMobileCountryCodeMatch(TBool aMccMatch)
	{
	iMCC = aMccMatch;
	}
	
/**
Set whether a match for the Mobile Network Code was
found or not (this method is called by the server, not
the client)
*/		
EXPORT_C void TPositionAreaExtendedInfo::SetMobileNetworkCodeMatch(TBool aMncMatch)
	{
	iMNC = aMncMatch;
	}
	
/**
Set whether a match for the Loca Area Code was
found or not (this method is called by the server, not
the client)
*/	
EXPORT_C void TPositionAreaExtendedInfo::SetLocationAreaCodeMatch(TBool aLacMatch)
	{
	iLAC = aLacMatch;
	}
	
/**
Set whether a match for the Cell ID was
found or not (this method is called by the server, not
the client)
*/	
EXPORT_C void TPositionAreaExtendedInfo::SetCellIdMatch(TBool aCidMatch)
	{
	iCID = aCidMatch;
	}

