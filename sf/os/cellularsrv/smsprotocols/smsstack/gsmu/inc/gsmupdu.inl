// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains inline method implementations from classes defined in gsmupdu.h
// 
//

/**
 @file
 
 Gets the PDU type.
 
 Once a PDU has been constructed it cannot change its type as the SMS
 elements present differ. Hence there is no function to set the type.
 
 @return PDU type 
 
*/
inline CSmsPDU::TSmsPDUType CSmsPDU::Type() const
	{
	return iSmsPDUType;
	}


/**
 *  Gets the Reply Path Error flag.
 *  
 *  @return True for Reply Path Error 
 */
inline TBool CSmsDeliverReport::IsRPError() const
	{
	return iIsRPError;
	}


/**
 *  Sets the Reply Path Error flag.
 *  
 *  @param aIsRPError Set to True for Reply Path Error 
 */
inline void CSmsDeliverReport::SetIsRPError(TBool aIsRPError)
	{
	iIsRPError=(TUint8) aIsRPError;
	}


/**
 *  Gets the Reply Path Error flag.
 *  
 *  @return True for Reply Path Error 
 */
inline TBool CSmsSubmitReport::IsRPError() const
	{
	return iIsRPError;
	}


/**
 *  Sets the Reply Path Error flag.
 *  
 *  @param aIsRPError Set to True for Reply Path Error 
 */
inline void CSmsSubmitReport::SetIsRPError(TBool aIsRPError)
	{
	iIsRPError=(TUint8) aIsRPError;
	}


/**
 *  Gets Paramater Indicator.
 *  
 *  @return True if Paramater Indicator is present 
 */
inline TBool CSmsStatusReport::ParameterIndicatorPresent() const
	{
	return iParameterIndicatorPresent;
	}


/**
 *  Sets Paramater Indicator.
 *  
 *  @param aPresent Set to True to set Parameter Indicator Present 
 */
inline void CSmsStatusReport::SetParameterIndicatorPresent(TBool aPresent)
	{
	iParameterIndicatorPresent=(TUint8) aPresent;
	}
