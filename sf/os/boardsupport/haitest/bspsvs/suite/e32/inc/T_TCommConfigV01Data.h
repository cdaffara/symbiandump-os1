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


#if (!defined __T_TCOMM_CONFIG_V01_DATA_H__)
#define __T_TCOMM_CONFIG_V01_DATA_H__

//	User Includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <d32comm.h>

/**
 * Test Active Notification class
 *
 */
class CT_TCommConfigV01Data : public CDataWrapperBase
	{
public:
	CT_TCommConfigV01Data();
	~CT_TCommConfigV01Data();

	virtual TDes8*	Descriptor();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	virtual TCommConfigV01&		GetCommConfigV01();

private:
	void	DestroyData();

	TInt	DoCmdConstructor();
	void	DoCmdDestructor();
	void	DoCmdiDataBits(const TDesC& aSection);
	void	DoCmdiFifo(const TDesC& aSection);
	void	DoCmdiHandshake(const TDesC& aSection);
	void	DoCmdiParity(const TDesC& aSection);
	void	DoCmdiParityError(const TDesC& aSection);
	void	DoCmdiParityErrorChar(const TDesC& aSection);
	void	DoCmdiRate(const TDesC& aSection);
	void	DoCmdiSIREnable(const TDesC& aSection);
	void	DoCmdiSIRSettings(const TDesC& aSection);
	void	DoCmdiSpecialRate(const TDesC& aSection);
	void	DoCmdiStopBits(const TDesC& aSection);
	void	DoCmdiTerminator(const TDesC& aSection);
	void	DoCmdiTerminatorCount(const TDesC& aSection);
	void	DoCmdiXoffChar(const TDesC& aSection);
	void	DoCmdiXonChar(const TDesC& aSection);

private:
	TCommConfig*	iCommConfig;

	static const TEnumEntryTable	iEnumTableDataBits[];
	static const TEnumEntryTable	iEnumTableFifo[];
	static const TEnumEntryTable	iEnumTableHandshake[];
	static const TEnumEntryTable	iEnumTableParity[];
	static const TEnumEntryTable	iEnumTableParityError[];
	static const TEnumEntryTable	iEnumTableRate[];
	static const TEnumEntryTable	iEnumTableSir[];
	static const TEnumEntryTable	iEnumTableSirSetting[];
	static const TEnumEntryTable	iEnumTableStopBits[];
	};

#endif /* __T_TCOMM_CONFIG_V01_DATA_H__ */
