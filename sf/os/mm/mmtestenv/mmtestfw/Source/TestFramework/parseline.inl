// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
 
 CStepData accessor : step
 
 @return "TPtrC"
 The step description
 
 @xxxx
 
 
*/
inline TPtrC CStepData::Step() const
	{ 
	return iStep; 
	}

/**
 *
 * CStepData accessor : config
 *
 * @return "TPtrC"
 *			The config description
 *
 * @xxxx
 *
 */
inline TPtrC CStepData::Config() const
	{
	return iConfig;
	}

/**
 *
 * CStepData accessor : suite
 *
 * @return "TPtrC"
 *			The step description
 *
 * @xxxx
 *
 */
inline CSuiteDll* CStepData::SuiteDll() const
	{
	return iSuite;
	}



