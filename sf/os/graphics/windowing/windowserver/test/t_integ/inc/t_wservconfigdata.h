// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @test
*/

#ifndef __T_WSERV_CONFIG_DATA_H__
#define __T_WSERV_CONFIG_DATA_H__

#include <e32base.h>

const TInt								KMaxWServConfigDataSize=256;
typedef TBuf<KMaxWServConfigDataSize>	TWServConfigData;

/**
 * Test Active Notification class
 *
 */
class CT_WServConfigData
	{
public:
	/**
	* Constructor
	*/
	CT_WServConfigData();

	/**
	* Destructor
	*/
	virtual ~CT_WServConfigData();

	/**
	* Add a data entry to the config
	*
	* @param	aName name of the config entry
	* @param	aData the data to add. 
	* @leave	KErrAlreadyExists if the config entry aName already exists
	*/
	void		AddDataL(const TDesC& aName, const TDesC& aData);

	/**
	* Add a data entry to the config
	*
	* @param	aName name of the config entry to be replaced
	* @param	aData the data to add. 
	* @leave	KErrNotFound if the config entry aName is not found
	*/
	void		ReplaceData(const TDesC& aName, const TDesC& aData);

	/**
	* Delete a data entry from the config
	*
	* @param	aName name of the config entry
	*
	* @leave	KErrNotFound if the config entry aName is not found
	*/
	void		DeleteDataL(const TDesC& aName);

	/**
	* Get the list of data
	*
	* @param	aIndex of an index into the list of data
	*
	* @return	Pointer to list of data
	*/
	const TWServConfigData& GetData(const TInt aIndex) const;

	/**
	* Get the list of names
	*
	* @param	aIndex of an index into the list of names
	*
	* @return	Pointer to list of names
	*/
	const TWServConfigData& GetName(const TInt aIndex) const;


	/**
	* Return the number of entries
	*
	* @return   The number of entries stored in the config data.
	* @leave	KErrGeneral
	*/
	TInt		NoOfEntriesL() const;

	/**
	* Empty the object config
	*/
	void		Empty();

private:
	RArray<TWServConfigData>	iArrayName;
	RArray<TWServConfigData>	iArrayData;
	};

#endif // __T_WSERV_CONFIG_DATA_H__

