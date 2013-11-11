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


#if (!defined __T_CONVERT_TO_SYMBIAN_ERROR_DATA_H__)
#define __T_CONVERT_TO_SYMBIAN_ERROR_DATA_H__

// User Includes
#include "DataWrapperBase.h"

class CT_ConvertToSymbianErrorData : public CDataWrapperBase
	{
public:
	/**
	 * Public destructor
	 */
	~CT_ConvertToSymbianErrorData();

	/**
	 * Two phase constructor
	 */
	static CT_ConvertToSymbianErrorData* NewL();

	/**
	 * Process a command read from the ini file
	 *
	 * @param aCommand			The command to process
	 * @param aSection			The section in the ini containing data for the command
	 * @param aAsyncErrorIndex	Command index for async calls to return errors to
	 *
	 * @return					ETrue if the command is processed
	 *
	 * @leave					System wide error
	 */
	virtual TBool DoCommandL(const TTEFFunction& aCommand,
			const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	/**
	 * Return a pointer to the object that the data wraps
	 *
	 * @return	pointer to the object that the data wraps
	 */

	virtual TAny* GetObject();




protected:
	CT_ConvertToSymbianErrorData();
	void ConstructL();

private:

	inline void DoCmdAvdtpError(const TDesC& aSection);
	inline void DoCmdGavdpError(const TDesC& aSection);
	inline void DoCmdA2dpError(const TDesC& aSection);
	inline void DoCmdVdpError(const TDesC& aSection);
	};

#endif /* __T_CONVERT_TO_SYMBIAN_ERROR_DATA_H__*/
