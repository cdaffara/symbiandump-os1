/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#if (!defined __T_MMCSDPsu_DRIVER_DATA_H__)
#define __T_MMCSDPsu_DRIVER_DATA_H__

//	User Includes
#include "T_MmcSDDriverData.h"

/**
 * Test MMCSD PowerSupply class
 *
 */
class CT_MMCSDPsuDriverData : public CT_MmcSDDriverData
	{
public:
	/**
	* Public destructor
	*/
	~CT_MMCSDPsuDriverData();

	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static	CT_MMCSDPsuDriverData*	NewL();

	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MMCSDPsuDriverData();
	void	ConstructL();

	void	DoCmdPsuInfo(const TDesC& aSection);
	void 	DoCmdSetPsuVoltage(const TDesC& aSection);
	void	DoCmdPbusState(const TDesC& aSection);
	void	DoCmdSetPsuState(const TDesC& aSection);
	void	DoCmdVccState(const TDesC& aSection);
	void 	DoCmdPsuNumber(const TDesC& aSection);
	void 	DoCmdMediaChangeID(const TDesC& aSection);
	void 	DoCmdPsuCurLimit(const TDesC& aSection);

private:
	static const TEnumEntryTable	iEnumTableVoltageCheckMethod[];
	static const TEnumEntryTable	iEnumTablePbusState[];
	static const TEnumEntryTable	iEnumTableVccState[];
	TInt							iAsyncErrorIndex;
	};

#endif /* __T_MMCSDPsu_DRIVER_DATA_H__ */
