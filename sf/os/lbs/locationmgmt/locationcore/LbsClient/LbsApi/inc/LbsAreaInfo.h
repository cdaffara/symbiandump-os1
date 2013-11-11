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


#ifndef LBS_AREAINFO_H
#define LBS_AREAINFO_H


#include <lbscommon.h>
#include <lbsfields.h>


/**
The base class for classes storing information on position area.

@publishedAll
@prototype
*/
class TPositionAreaInfoBase : public TPositionClassTypeBase
	{
protected:
	IMPORT_C TPositionAreaInfoBase();
	};
	

/**
This class provides the data structure used by RPositioner to get Position Area 
information. Position Area information is normally used together with
Position information and allows to define a rough accuracy of the position.

@publishedAll
@prototype
 */
class TPositionAreaInfo : public TPositionAreaInfoBase
	{
public:
	/** Defined type for TArea */
	typedef TUint32 TArea;
	
	/** Position area. Please note that the values assigned to each enumeration
	have no numerical meaning and should not be directly used.
	
	Note that the _TArea enum may be extended in the future by adding
	more enumerated values. To maintain compatibility any unrecognized values 
	must be handled as EAreaUnknown.*/
	enum _TArea
		{
		/** Data initialisation or unknown value. */
		EAreaUnknown		= 0,
		/** Accuracy is country size */
		EAreaCountry		= 500,
		/** Accuracy is region size */
		EAreaRegion		= 600,
		/** Accuracy is city size */
		EAreaCity			= 700,
		/** Accuracy is district size */
		EAreaDistrict		= 800,
		/** Accuracy is street size */
		EAreaStreet  		= 900
		};
		

	IMPORT_C TPositionAreaInfo();
	IMPORT_C TPositionAreaInfo(TArea aArea);
	
	IMPORT_C TArea Area() const;
	IMPORT_C void SetArea(TArea aArea);

protected:
	TArea iArea;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[16];
	};


/**
TPositionAreaExtendedInfo is a specialised area class and provides detailed
information about the match between the current network information and the network
information related to a known position.

In the future the class may be extended to provide information about a match between
other available location information (e.g. WiFi MAC address).

In order to provide a user with simplified area information, the match between
the two network information should be translated into a area information supported
by the TPositionAreaInfo type.

Network Info Match > Area info conversion table
@code
||==========================================================================================||
|| Mobile Country Code  | Mobile Network Code | Location Area Code | Cell Id | Area         ||
||==========================================================================================||
||           1          |          1          |         1          |    1    | EAreaCity    ||
||           1          |          1          |         1          |    0    | EAreaRegion  ||
||           1          |          1          |         0          |    0    | EAreaCountry ||
||           1          |          0          |         0          |    0    | EAreaCountry ||
||                                  Any other combination                    | EAreaUnknown ||
||==========================================================================================||
@endcode

Please note that the conversion of the Cell Id match assumes the worst case scenario, where cells
are big (e.g. tens of km radius). In centres of big cities the cell sizes are normally much smaller,
taking the area down to the District or even Street level.

@publishedAll
@prototype
*/
class TPositionAreaExtendedInfo : public TPositionAreaInfo
	{
public:
	IMPORT_C TPositionAreaExtendedInfo();
	
	IMPORT_C TBool MobileCountryCodeMatch() const;
	IMPORT_C TBool MobileNetworkCodeMatch() const;
	IMPORT_C TBool LocationAreaCodeMatch() const;
	IMPORT_C TBool CellIdMatch() const;
	
	IMPORT_C void SetMobileCountryCodeMatch(TBool aMccMatch);
	IMPORT_C void SetMobileNetworkCodeMatch(TBool aMncMatch);
	IMPORT_C void SetLocationAreaCodeMatch(TBool aLacMatch);
	IMPORT_C void SetCellIdMatch(TBool aCidMatch);
	
protected:
	TBool iMCC;
	TBool iMNC;
	TBool iLAC;
	TBool iCID;
	
private:
	/** Unused variable for future expansion. */
	TUint8 iReserved2[32];
	};

#endif //LBS_AREAINFO_H
