// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file ss_protopt.h
 @internalTechnology
 @released
*/

#if !defined(__SS_PROTOPT_H__)
#define __SS_PROTOPT_H__

/*
 * T class which holds the parameters used for SetOpt( )
**/
class TProtocolOption
{
public:
	TUint iOptionName;
	TUint iOptionLevel;
	TUint iOptionValue;
};


/*
 * Generic class to implement SetOpt( ) at SapShim.
**/
class CSAPSetOpt : public CBase, public Meta::SMetaData
{
public:
	enum
	{
		ETypeId = 10,
		EUid = 0x101F7482,
	};

public:
	IMPORT_C CSAPSetOpt();
	virtual ~CSAPSetOpt();

	//Add the option to the RArray variable
	IMPORT_C void AddProtocolOptionL(TUint aOptionName, TUint aOptionLevel, TUint aOptionValue);

	//Update the value for an option
	IMPORT_C void UpdateProtocolOption(TUint aOptionName, TUint aOptionLevel, TUint aOptionValue);


public:
	RArray<TProtocolOption> iOption;
	
public:
 	DATA_VTABLE	
};


#endif //__SS_PROTOPT_H__

