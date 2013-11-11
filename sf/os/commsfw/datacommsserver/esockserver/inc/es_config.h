// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined (__ES_CONFIG_H__)
#define __ES_CONFIG_H__

#include <in_sock.h>

// configuration daemon structures for use with
// RConnection::Ioctl()

class TConnectionAddress
/**
  * TConnectionAddress class
  *
  * For use when querying for the configured
  * address with RConnection::Ioctl() using 
  * the KConnGetCurrentAddr constant. The client
  * must provide the IP version the request is meant for
  *
  * @publishedPartner
  * @released
  */
	{
public:
	TUint32 iAddressFamily;		// KAfInet or KAfInet6
	TSockAddr iAddr;
	};
typedef TPckgBuf<TConnectionAddress> TConnectionAddrBuf;


class TConnectionLeaseInfo
/**
  * TConnectionLeaseInfo class
  *
  * For use when querying for the lease time
  * remaining with RConnection::Ioctl() using
  * the KConnGetLeaseTimeRemain constant.  The client
  * must provide the IP version the request is meant for
  *
  * @publishedPartner
  * @released
  */
	{
public:
	TUint iAddressFamily;		// KAfInet or KAfInet6	
	TUint iSecondsRemaining;
	};
typedef TPckgBuf<TConnectionLeaseInfo> TConnectionLeaseInfoBuf;


template <class T>
class TPckgDes
/**
  * TPckgDes - dynamic packaging using user allocated buffer
  *
  * The buffer starts with structure \<T\> followed by an arbitrary buffer starting at iPtr8
  * the idea is that we can send a structure and dynamically allocated buffer as one continuous
  * descriptor rather than pack a structure having a pointer to another descriptor.
  *
  * @publishedPartner
  * @released
  */
	{
public:
	TPckgDes(TDes8& aDes) :
		iDes(&aDes)
		/**
		 * @param aDes User buffer to hold data fetched.
		 */
		{
		}
		
	inline T& operator()()
		{
	 	return(*((T *)iDes->Ptr()));
		}
		
	inline TPtr8 Buf()
		{
	 	return TPtr8(const_cast<TUint8*>(iDes->Ptr()) + sizeof(T), iDes->Length() - sizeof(T), iDes->MaxLength() - sizeof(T));
		}

	inline void SetBufLengthL(TInt aNewLength)
		{
		TInt newLength = aNewLength + sizeof(T);
		if (newLength > iDes->MaxLength())
			{
    		User::Leave(KErrOverflow);
     		}
	 	iDes->SetLength(newLength);
		}

protected:
   TDes8* iDes; //user allocated descriptor
   };
   

class TDhcpRawOptionDataPckg : public TPckgDes<TUint8>
/**
  * TDhcpRawOptionDataPckg - provide simple interface for sending the dhcp raw option.
  *
  * The data contained in the package is the opcode and the value of the raw option.
  * The length of the buffer is set with the length of the descriptor containing 'the
  * opcode + the raw option data' by the DHCP component.
  *
  * e.g. The DNS option in DHCP message is represented by opcode 6. On requesting for DNS option using 
  * RConnection::Ioctl(), if the data received in the buffer is 6 192 168 0 1 192 168 0 2 then  
  * 6 is the DNS option code and 192 168 0 1 and 192 168 0 2 are the IP addresses of DNS servers.
  *
  * @publishedPartner
  * @released
  */
	{
public:
 	TDhcpRawOptionDataPckg(TDes8& aDes)  :
		TPckgDes<TUint8>(aDes)
		/**
		 * @param aDes User buffer to hold data fetched.
		 */
		{
		}	

	inline void SetOpCode(TUint8 aOpCode)
		/**
		 * SetOpCode. Make sure the buffer is at least 4 bytes long.
		 * @param aOpCode Desired DHCP Raw option.
		 *
		 */
		{
		TBuf8<1> buf;
		buf.FillZ(1);
		buf[0]=aOpCode;
		iDes->Replace(0,1,buf);
		}

	inline TUint8 OpCode()
		{
		return *(iDes->Ptr());
		} 	
	};

/**
  * SSipServerAddr structure
  *
  * For use when querying for the SIP server addresses with 
  * RConnection::Ioctl() using the KConnGetSipServerAddr constant.  
  * The client must provide the index of the address being
  * requested.
  *
  * @publishedPartner
  * @released
  */
struct SSipServerAddr
	{
	TInt index;
	TInetAddr address;
	};

typedef TPckgBuf<SSipServerAddr> TSipServerAddrBuf;

/**
  * SSipServerDomain structure
  *
  * For use when querying for the SIP server domains with
  * RConnection::Ioctl() using the KConnGetSipServerDomain constant.
  * The client must provide the index of the name being
  * requested.
  *
  * @publishedPartner
  * @released
  */
struct SSipServerDomain
	{
	TInt index;
	THostName domainName;
	};

typedef TPckgBuf<SSipServerDomain> TSipServerDomainBuf;
#ifdef SYMBIAN_TCPIPDHCP_UPDATE 
/**
  * SDomainSearchList structure
  *
  * For use when querying for the list of domain names with
  * RConnection::Ioctl() using the KConnGetDomainSearchList constant.
  * The client must provide the index of the name being
  * requested.
  *
  * @publishedPartner
  * @released
  */
struct SDomainSearchList
	{
	TInt index;
	THostName domainname;
	};

typedef TPckgBuf<SDomainSearchList> TDomainSearchListBuf;

/**
  * SDNSServerAddr structure
  *
  * For use when querying for the list of Recursive DNS Servers with
  * RConnection::Ioctl() using the KConnGetDNSServerList constant.
  * The client must provide the index of the name being
  * requested.
  *
  * @publishedPartner
  * @released
  */
struct SDNSServerAddr
	{
	TInt index;
	TInetAddr addres;
	};

typedef TPckgBuf<SDNSServerAddr> TDNSServerAddrBuf;
#endif //SYMBIAN_TCPIPDHCP_UPDATE 

struct STftpServerAddr
/**
  * STftpServerAddr structure
  *
  * For use when querying for the TFTP server addresses with 
  * RConnection::Ioctl() using the KConnGetTftpServerAddr constant.  
  * The client must provide the index of the address being
  * requested.
  *
  * @publishedPartner
  * @released
  */
	{
	TInt index;
	TInetAddr address;
	};
typedef TPckgBuf<STftpServerAddr> TTftpServerAddrBuf;


/**
  * TTftpServerNameBuf structure
  *
  * For use when querying for the TFTP server name with 
  * RConnection::Ioctl() using the KConnGetSipServerName constant.  
  *
  * @publishedPartner
  * @released
  */
typedef TBuf<256> TTtfpServNameBuf;

class TDhcpRawOptionMultipleDataPckg : public TPckgDes<TUint8>
/**
 * For use when an application wants to access multiple raw parameter options.
 * The descriptor contains a list of OpCodes. After the RConnection::Ioctl() call has 
 * completed, the descriptor will contain the number of parameters in the first byte 
 * followed by parameter opcode, length and data for each parameter option.
 *	
 *	@code
 *     Message	
 *	     -------------------------------   
 *		|	|	|	|		|
 *		|op1|op2|-	|		|
 *		 -------------------------------
 *  @endcode
 * @publishedPartner
 * @released
*/
	{
public:
 	TDhcpRawOptionMultipleDataPckg(TDes8& aDes)  :
		TPckgDes<TUint8>(aDes)
	//Constructor for the class. Pushing Buffer containing number
	//of OpCode in iDes.  
		{
		//Setting data length of descriptor as NULL
		iDes->SetLength(NULL);
		}

	inline void AddRawOptionCodeL(const TUint8 aOpCode)
	/**
	 * Sets parameters one at a time.
	 * The only thing the application provides is the OpCode.
	 * @param aOpCode The OpCode supplied by the user.
	*/
		{
		//  ---------------------
		// |      |		|..........................
		// |OP1   |	OP2	|
		// |      |     |.......................
		// ----------------------
		//Data will be appended at the end of the buffer. Length of buffer will indicate
		//number of opcodes as every thing is 1 byte
		iDes->Append(aOpCode);
		   
		}
		
   	inline TInt GetRawParameterData(const TUint8 aOpCode ,TPtrC8& aDes)
   	/**
   	 * Returns the data portion corresponding to the supplied opcode.  
	 * It's only meaningful to call this after an Ioctl() call.
	 *
   	 * @code
   	 *  ---------------------------------------------------
   	 * |No. of |      |Data  |      |       |Data  |       |
   	 * |opcodes| OP1  |Length| Data |  OP2  |Length| Data  |
   	 * |       |      |      |      |       |      |       |
   	 *  --------------------------------------------------- 
   	 * @endcode
   	 *
   	 * @param aOpCode The opcode passed by the user.
   	 * @param aDes On return, it contains the message opcode data.
   	 * @return KErrNone if aOpCode is found, else KErrNotFound 
   	*/
		{
		TUint8 opCodeIterated=0;
		TUint8 opCodeIterator= 1;
		const TUint8 sizeOfOpcodeAndDataBytes = 2;
		const TUint8 sizeOfLengthByte = 1;
	
		//extracting number of opcodes. First byte of Descriptor will contain number of bytes
		TUint8 numberOfOpcodes  = *(iDes->Ptr());
		
		//taking pointer to base location of aDes.
		const TUint8* desBasePtr=iDes->Ptr();
	
		//While loop true conditions are opCodeFound if Particular opcode is extracted or All opcode in 
		//descriptor are iterated and OpCode is not found.
		while( opCodeIterated < numberOfOpcodes)
			{
			//If opcode is found then come out of loop and TPtr will be passed in the descriptor. Pointer is returned to the 
			//start byte of data for that particular Opcode with length of the data.
 			if(aOpCode==*(desBasePtr+opCodeIterator))
				{
				TInt dataLength = *(desBasePtr+(opCodeIterator+sizeOfLengthByte));
  				aDes.Set(const_cast<TUint8*>((desBasePtr+opCodeIterator)+sizeOfOpcodeAndDataBytes),dataLength);	
				return KErrNone;
				}
		
			//Opcode iterator is iterated in such a way that it will always point to opcode location.
			opCodeIterator += *(desBasePtr+(opCodeIterator+sizeOfLengthByte))+sizeOfOpcodeAndDataBytes;
			opCodeIterated++;
			}//while loop.
		return KErrNotFound;
		}
		
	inline TUint8 NumOfOptionRetrieved()
	/**
	 * Gets the number of opcode data buffers.
	 * It's only meaningful to call this after an Ioctl() call.
	 * @return The number of opcode data buffers.
	 */
		{
		return *(iDes->Ptr());
		}
	};



#endif




