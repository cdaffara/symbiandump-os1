// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains ESock Test cases from section 19
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection19.h"


// Test step 19.1
const TDesC& CEsockTest19_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test19.1");
	return ret;
	}

CEsockTest19_1::~CEsockTest19_1()
	{
	}

enum TVerdict CEsockTest19_1::easyTestStepL()
	{
	TInetAddr addr;
	TBuf<39> buf;
	
	// setting the IP address
	
	const TIp6Addr KInet6Addr19_1 = {{{0xff,0xfe,0,0,0,0,0,0,0,0,0,0,0,0,0x29,0xfe}}};
	const TIp6Addr KInet6Addr19_2 = {{{0xff,0xf1,0,1,0,1,0,1,0,1,0,1,0,1,0x29,0xff}}};
	
	// setting the IP address
	
	// set the address
	addr.SetAddress(KInet6Addr19_1);
	
	// check it has been set correctly
	addr.OutputWithScope(buf);
	TESTL(buf==_L("fffe::29fe"));
	
	// check port number initialised to 0
	TESTL(addr.Port()==0);
	
	// change the address
	addr.SetAddress(KInet6Addr19_2);
	
	// check it has been set correctly
	addr.OutputWithScope(buf);
	TESTL(buf==_L("fff1:1:1:1:1:1:1:29ff"));
	
	// check port number is still set to 0
	TESTL(addr.Port()==0);
	
	return EPass;
	}


// Test step 19.2
const TDesC& CEsockTest19_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test19.2");
	return ret;
	}

CEsockTest19_2::~CEsockTest19_2()
	{
	}

enum TVerdict CEsockTest19_2::easyTestStepL()
	{
	TInetAddr addrCompat, addrIpv4;
	TInt port;
	TBuf<39> ipv4Buf;
	TBuf<39> ipv4compatBuf;
	
	// setting an IPv4 compatible address
	
	// get ip addresses from script (IPv4 format)
	TESTL(GetIpAddressFromConfig(_L("Test_19.2"), _L("ipAddress"), addrIpv4));
	
	// get port number from script
	TESTL(GetIntFromConfig(_L("Test_19.2"), _L("port"), port));
	
	// set port number
	addrIpv4.SetPort(port);
	
	// get address into a TUint32
	TUint32 longAddr = addrIpv4.Address();
	
	// set an IPv4 compatible address
	addrCompat.SetV4CompatAddress(longAddr);
	
	// get string representations of the addresses
	addrIpv4.OutputWithScope(ipv4Buf);  // original IPv4 address
	addrCompat.OutputWithScope(ipv4compatBuf); // IPv4 compat. address
	
	// check IPv4 compatible address set correctly
	// looks like original with :: prefix
	ipv4Buf.Insert(0, _L("::"));
	TESTL(ipv4Buf==ipv4compatBuf);
	
	// check the port initialised to 0
	TESTL(addrCompat.Port() == 0);
	
	// repeat for unspecified address
	addrCompat.SetV4CompatAddress(INET_ADDR(0, 0, 0, 0));
	addrCompat.OutputWithScope(ipv4compatBuf);
	TESTL(ipv4compatBuf==_L("::"));
	TESTL(addrCompat.Port() == 0);
	
	return EPass;
	}


// Test step 19.3
const TDesC& CEsockTest19_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test19.3");
	return ret;
	}

CEsockTest19_3::~CEsockTest19_3()
	{
	}

enum TVerdict CEsockTest19_3::easyTestStepL()
	{
	TInetAddr addrMapped, addrIpv4_1, addrIpv4_2;
	TInt port1, port2;
	TBuf<39> ipv4Buf;
	TBuf<39> ipv4mappedBuf;
	
	// setting an IPv4 mapped address
	
	// get ip addresses from script (IPv4 format)
	TESTL(GetIpAddressFromConfig(_L("Test_19.3"), _L("ipAddress1"), addrIpv4_1));
	TESTL(GetIpAddressFromConfig(_L("Test_19.3"), _L("ipAddress2"), addrIpv4_2));
	
	// get port numbers from script
	TESTL(GetIntFromConfig(_L("Test_19.3"), _L("port1"), port1));
	TESTL(GetIntFromConfig(_L("Test_19.3"), _L("port2"), port2));
	
	// set port number of ip address
	addrIpv4_1.SetPort(port1);
	
	// get address into a TUint32
	TUint32 longAddr = addrIpv4_1.Address();
	
	// set an IPv4 mapped address
	addrMapped.SetV4MappedAddress(longAddr);
	
	// get string representations of the addresses
	addrIpv4_1.OutputWithScope(ipv4Buf);
	addrMapped.OutputWithScope(ipv4mappedBuf);
	
	// check IPv4 mapped address set correctly
	
	
	TESTL(ipv4Buf==ipv4mappedBuf);
	
	// check the port number
	TESTL(addrMapped.Port() == 0);
	
	// repeat for 2nd ip address
	addrIpv4_2.SetPort(port2);
	longAddr = addrIpv4_2.Address();
	addrMapped.SetV4MappedAddress(longAddr);
	addrIpv4_2.OutputWithScope(ipv4Buf);
	addrMapped.OutputWithScope(ipv4mappedBuf);
	
	TESTL(ipv4Buf==ipv4mappedBuf);
	TESTL(addrMapped.Port() == 0);
	
	return EPass;
	}


// Test step 19.4
const TDesC& CEsockTest19_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test19.4");
	return ret;
	}

CEsockTest19_4::~CEsockTest19_4()
	{
	}

enum TVerdict CEsockTest19_4::easyTestStepL()
	{
	TInetAddr /*addrCompat,*/ addrIpv4_1, addrIpv4_2;
	TInt port1, port2;
	TBuf<39> ipv4Buf;
	TBuf<39> ipv4compatBuf;
	
	// converting to an IPv4 compatible address
	
	// get ip addresses from script (IPv4 format)
	TESTL(GetIpAddressFromConfig(_L("Test_19.4"), _L("ipAddress1"), addrIpv4_1));
	TESTL(GetIpAddressFromConfig(_L("Test_19.4"), _L("ipAddress2"), addrIpv4_2));
	
	// get port numbers from script
	TESTL(GetIntFromConfig(_L("Test_19.4"), _L("port1"), port1));
	TESTL(GetIntFromConfig(_L("Test_19.4"), _L("port2"), port2));
	
	// set the port number
	addrIpv4_1.SetPort(port1);
	
	// get string representation of the input address
	addrIpv4_1.OutputWithScope(ipv4Buf);
	
	// convert to IPv4 compatible address
	addrIpv4_1.ConvertToV4Compat();
	
	// get string representation of the converted address
	addrIpv4_1.OutputWithScope(ipv4compatBuf);
	
	// check IPv4 compatible address set correctly
	if (addrIpv4_1.IsUnspecified())  // special case - unspecifed address
		ipv4Buf=_L("::");		
	else
		ipv4Buf.Insert(0, _L("::"));
	
	TESTL(ipv4Buf==ipv4compatBuf);
	
	// check the port number
	TESTL(addrIpv4_1.Port() == (TUint)port1);
	
	// repeat for 2nd ip address
	addrIpv4_2.SetPort(port2);
	addrIpv4_2.OutputWithScope(ipv4Buf);
	addrIpv4_2.ConvertToV4Compat();
	addrIpv4_2.OutputWithScope(ipv4compatBuf);
	if (addrIpv4_2.IsUnspecified())
		ipv4Buf=_L("::");		
	else
		ipv4Buf.Insert(0, _L("::"));
	TESTL(ipv4Buf==ipv4compatBuf);
	TESTL(addrIpv4_2.Port() == (TUint)port2);
	
	return EPass;
	}


// Test step 19.5
const TDesC& CEsockTest19_5::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test19.5");
	return ret;
	}

CEsockTest19_5::~CEsockTest19_5()
	{
	}

enum TVerdict CEsockTest19_5::easyTestStepL()
	{
	TInetAddr addrIpv4_1, addrIpv4_2;
	TInt port1, port2;
	TBuf<39> ipv4Buf;
	TBuf<39> ipv4mappedBuf;
	
	// converting to an IPv4 mapped address
	
	// get ip addresses from script (IPv4 format)
	TESTL(GetIpAddressFromConfig(_L("Test_19.5"), _L("ipAddress1"), addrIpv4_1));
	TESTL(GetIpAddressFromConfig(_L("Test_19.5"), _L("ipAddress2"), addrIpv4_2));
	
	// get port numbers from script
	TESTL(GetIntFromConfig(_L("Test_19.5"), _L("port1"), port1));
	TESTL(GetIntFromConfig(_L("Test_19.5"), _L("port2"), port2));
	
	// set the port number
	addrIpv4_1.SetPort(port1);
	
	// get string representation of the input address
	addrIpv4_1.OutputWithScope(ipv4Buf);
	
	// convert to IPv4 compatible address
	addrIpv4_1.ConvertToV4Mapped();
	
	// get string representation of the converted address
	addrIpv4_1.OutputWithScope(ipv4mappedBuf);
	
	// check IPv4 compatible address set correctly
	
	TESTL(ipv4Buf==ipv4mappedBuf);
	
	// check the port number
	TESTL(addrIpv4_1.Port() == (TUint)port1);
	
	// repeat for 2nd ip address
	addrIpv4_2.SetPort(port2);
	addrIpv4_2.OutputWithScope(ipv4Buf);
	addrIpv4_2.ConvertToV4Mapped();
	addrIpv4_2.OutputWithScope(ipv4mappedBuf);
	
	TESTL(ipv4Buf==ipv4mappedBuf);
	TESTL(addrIpv4_2.Port() == (TUint)port2);
	
	return EPass;
	}


// Test step 19.6
const TDesC& CEsockTest19_6::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test19.6");
	return ret;
	}

CEsockTest19_6::~CEsockTest19_6()
	{
	}

enum TVerdict CEsockTest19_6::easyTestStepL()
	{
	TInetAddr addr;
	TBuf<39> buf;
	
	// convert to V4
	
	// set address to IPv6 unspecified address
	addr.Input(_L("::"));
	
	// convert to IPv4 format
	addr.ConvertToV4();
	addr.OutputWithScope(buf);
	
	// check address has been reformatted correctly
	addr.OutputWithScope(buf);
	TESTL(buf==_L("0.0.0.0"));
	
	// change address to IPv4 compatible address
	addr.Input(_L("::199.12.255.21"));
	
	// convert to IPv4 format
	addr.ConvertToV4();
	
	// check address has been reformatted correctly
	addr.OutputWithScope(buf);
	TESTL(buf==_L("199.12.255.21"));
	
	// change address to IPv4 mapped address
	addr.Input(_L("::FFFF:149.124.223.12"));
	
	// convert to IPv4 format
	addr.ConvertToV4();
	
	// check address has been reformatted correctly
	addr.OutputWithScope(buf);
	TESTL(buf==_L("149.124.223.12"));
	
	return EPass;
	}


// Test step 19.7
const TDesC& CEsockTest19_7::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test19.7");
	return ret;
	}

CEsockTest19_7::~CEsockTest19_7()
	{
	}

enum TVerdict CEsockTest19_7::easyTestStepL()
	{
	TInetAddr addr;
	TBuf<39> buf;
	
	// set IP address using a prefix length
	
	// set IP address
	addr.PrefixMask(60);
	
	// check it has been set correctly
	addr.OutputWithScope(buf);
	TESTL(buf==_L("ffff:ffff:ffff:fff0::"));
	
	// change the IP address
	addr.Input(_L("1.1.1.1.1.1.1.1"));
	
	// set the IP address using a different prefisx length
	addr.PrefixMask(58);
	
	// check it has been set correctly
	addr.OutputWithScope(buf);
	TESTL(buf==_L("ffff:ffff:ffff:ffc0::"));
	
	return EPass;
	}


// Test step 19.8
const TDesC& CEsockTest19_8::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test19.8");
	return ret;
	}

CEsockTest19_8::~CEsockTest19_8()
	{
	}

enum TVerdict CEsockTest19_8::easyTestStepL()
	{
	TInetAddr addr, addr1;
	TBuf<39> buf;
	
	// setting IP address using an IP address and a prefix length
	
	// set an IP address 
	addr1.Input(_L("ffee:fffc:0001:0001:1:1:1:1"));
	
	// set the IP address from addr and prefix length
	addr.Prefix(addr1, 18);
	
	// check that the address has been set correctly
	addr.OutputWithScope(buf);
	TESTL(buf==(_L("ffee:c000::")));
	
	// set the IP address using a different length
	addr.Prefix(addr1, 64);
	
	// check that the address has been set correctly
	addr.OutputWithScope(buf);
	TESTL(buf==(_L("ffee:fffc:1:1::")));
	
	return EPass;
	}


// Test step 19.9
const TDesC& CEsockTest19_9::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test19.9");
	return ret;
	}

CEsockTest19_9::~CEsockTest19_9()
	{
	}

enum TVerdict CEsockTest19_9::easyTestStepL()
	{
	TInetAddr addr, addrOut;
	TBuf<39> buf;
	
	// getting the IP address
	
	// set an IPv6 address
	const TIp6Addr KInet6Addr19_3 = {{{0xff,0xff,0,0,0,0,0,0,0,0,0,0,0x1,0x78,0,0x1}}};
	addr.SetAddress(KInet6Addr19_3);
	
	// get the address - check the return value is 0
	TESTL(addr.Address()==0);
	
	// set up an IPv4 compatible address
	TInetAddr addr2(INET_ADDR(140, 179, 229, 12), 23);
	addr2.ConvertToV4Compat();
	
	// check conversion
	addr2.OutputWithScope(buf);
	TESTL(buf==_L("::140.179.229.12"));
	
	// get the address - check it is returned as an IPv4 address
	addrOut.SetAddress(addr2.Address());
	addrOut.OutputWithScope(buf);
	TESTL(buf==_L("140.179.229.12"));
	
	// set up an IPv4 mapped address
	TInetAddr addr3(INET_ADDR(140, 179, 193, 121), 24);
	addr3.ConvertToV4Mapped();
	
	// check conversion
	addr3.OutputWithScope(buf);
	
	TESTL(buf==_L("140.179.193.121"));
	
	// get the address - check it is returned as an IPv4 address
	addrOut.SetAddress(addr3.Address());
	addrOut.OutputWithScope(buf);
	TESTL(buf==_L("140.179.193.121"));
	
	return EPass;
	}

