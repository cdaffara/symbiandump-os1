// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains ESock Test cases from section 13
// 
//

// EPOC includes
#include <e32base.h>


// Test system includes

#include "SocketTestSection13.h"


// Test step 13.1
const TDesC& CSocketTest13_1::GetTestName()
	{
	_LIT(ret,"Test13.1");
	return ret;
	}

enum TVerdict CSocketTest13_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Utility Classes"));
	
	TUint8 data[]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10};
	
	Logger().WriteFormat(_L("Big endian get"));
	
	TUint32 res32=BigEndian::Get32(data+1);
	
	// Integer bytes are tested in reverse order as Intel is LittleEndian
	TESTL(res32 == 0x02030405);
	
	TUint16 res16 = BigEndian::Get16(data+5);
	TESTL(res16 == 0x0607);
	
	Logger().WriteFormat(_L("Big endian set"));
	BigEndian::Put32(data+3, 0x10090807);
	TESTL(data[3] == 0x10);
	TESTL(data[4] == 0x09);
	TESTL(data[5] == 0x08);
	TESTL(data[6] == 0x07);
	
	BigEndian::Put16(data+7, 0x0201);
	TESTL(data[7] == 0x02);
	TESTL(data[8] == 0x01);
	
	Logger().WriteFormat(_L("Little endian get"));
	
	res32 = LittleEndian::Get32(data+1);
	TESTL(res32 == 0x09100302);
	
	res16 = LittleEndian::Get16(data+5);
	TESTL(res16 == 0x0708);
	
	Logger().WriteFormat(_L("Little endian set"));
	LittleEndian::Put32(data+1, 0x05060708);
	TESTL(data[1] == 0x08);
	TESTL(data[2] == 0x07);
	TESTL(data[3] == 0x06);
	TESTL(data[4] == 0x05);
	
	LittleEndian::Put16(data+1, 0x0102);
	TESTL(data[0] == 0x01);
	TESTL(data[1] == 0x02);
	TESTL(data[2] == 0x01);
	TESTL(data[3] == 0x06);
	TESTL(data[4] == 0x05);
	TESTL(data[5] == 0x08);
	TESTL(data[6] == 0x07);
	TESTL(data[7] == 0x02);
	TESTL(data[8] == 0x01);
	TESTL(data[9] == 0x10);
	
	LittleEndian::Put32(data+1,0xFeedFace);
	TUint16 res1 = BigEndian::Get16(data+1);
	TUint16 res2 = BigEndian::Get16(data+3);
	LittleEndian::Put16(data+3, res1);
	LittleEndian::Put16(data+1, res2);
	res32 = BigEndian::Get32(data+1);
	TESTL(res32 == 0xFeedFace);
	
	BigEndian::Put32(data+1, 0xFeedFace);
	res1 = LittleEndian::Get16(data+1);
	res2 = LittleEndian::Get16(data+3);
	BigEndian::Put16(data+3, res1);
	BigEndian::Put16(data+1, res2);
	res32 = LittleEndian::Get32(data+1);
	TESTL(res32 == 0xFeedFace);
	
	SetTestStepResult(verdict);
	return verdict;
	}

