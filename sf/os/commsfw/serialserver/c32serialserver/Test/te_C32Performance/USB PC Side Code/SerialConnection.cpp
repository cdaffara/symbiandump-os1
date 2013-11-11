// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SerialConnection.cpp: implementation of the CSerialConnection class.
// This class opens a virtual comm port to perform read/write operation between PC and hardware 
// through USB cable.
// This application is used to receive data from the H4 board.An USB driver should be installed on the PC 
// which maps the USB port to the virtual com port. 
// The data received from the hardware is displayed to debug output window.
// 
//

/**
 @file
 @internalComponent.
*/

#include "stdafx.h"
#include "SerialPort.h"
#include "SerialConnection.h"
#include "SerialPortDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//set the baud rate
#define BAUD_RATE					115200
//set the buffer size to receive data
#define BUFFER_SIZE					64

//
// Construction/Destruction
//
static HANDLE hPort;
CSerialConnection::CSerialConnection()
{
}
CSerialConnection::~CSerialConnection()	//destructor
{
	if( CloseHandle(hPort)>0 )
	OutputDebugString ("Com port is closed successfully");
}
BOOL CSerialConnection::PortInitialize(CString strPort)
{

   DCB PortDCB;
   COMMTIMEOUTS CommTimeouts;
	
   //open the serial port.
   hPort = CreateFile ( strPort, 
						GENERIC_READ | GENERIC_WRITE,
                        0,            
						NULL,         
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,            
						NULL);        

  // If it fails to open the port, return FALSE.
  if ( hPort == INVALID_HANDLE_VALUE ) 
  {
     AfxMessageBox("Unable to open port");
	 return FALSE;
  }

  PortDCB.DCBlength = sizeof (DCB);     

  //Get the default port setting information.
  if (!GetCommState (hPort, &PortDCB))
  {
	 AfxMessageBox("Unable to retrieve default port setting information");
	 return FALSE;
  }

  //Change the DCB structure settings.

  PortDCB.BaudRate = BAUD_RATE;         // Current baud 
  PortDCB.fBinary = FALSE;              // Binary mode; no EOF check 
  PortDCB.fParity = FALSE;              // Enable parity checking. 
  PortDCB.fOutxCtsFlow = FALSE;         // No CTS output flow control 
  PortDCB.fOutxDsrFlow = FALSE;         // No DSR output flow control 
  PortDCB.fDtrControl = DTR_CONTROL_ENABLE; 
                                        // DTR flow control type 
  PortDCB.fDsrSensitivity = FALSE;      // DSR sensitivity 
  PortDCB.fTXContinueOnXoff = TRUE;     // XOFF continues Tx 
  PortDCB.fOutX = FALSE;                // No XON/XOFF out flow control 
  PortDCB.fInX = FALSE;                 // No XON/XOFF in flow control 
  PortDCB.fErrorChar = FALSE;           // Disable error replacement. 
  PortDCB.fNull = FALSE;                // Disable null stripping. 
  PortDCB.fRtsControl = RTS_CONTROL_ENABLE; 
                                        // RTS flow control 
  PortDCB.fAbortOnError = FALSE;        // Do not abort reads/writes on 
                                        // error.
  PortDCB.ByteSize = 8;                 // Number of bits/bytes, 4-8 
  PortDCB.Parity = NOPARITY;            // 0-4=no,odd,even,mark,space 
  PortDCB.StopBits = ONESTOPBIT;        // 0,1,2 = 1, 1.5, 2 

  // Configure the port according to the specifications of the DCB 
  // structure.

  if (!SetCommState (hPort, &PortDCB))
  {
	 // could not create the read thread.
	 AfxMessageBox("Unable to configure the serial port");
	 return FALSE;
  }

  // retrieve the time-out parameters for all read and write operations
  // on the port. 
   if (!GetCommTimeouts (hPort, &CommTimeouts))
   {
	   AfxMessageBox("Unable to retrieve the time-out parameters");
	   return FALSE;
   }

  // Change the COMMTIMEOUTS structure settings.
  CommTimeouts.ReadIntervalTimeout = MAXDWORD;  
  CommTimeouts.ReadTotalTimeoutMultiplier = 0;  
  CommTimeouts.ReadTotalTimeoutConstant = 0;    
  CommTimeouts.WriteTotalTimeoutMultiplier = 10;  
  CommTimeouts.WriteTotalTimeoutConstant = 1000;    

  // Set the time-out parameters for all read and write operations
  // on the port. 
  if (!SetCommTimeouts (hPort, &CommTimeouts))
  {
    AfxMessageBox("Unable to set the time-out parameters");
    return FALSE;
  }
    
  LPVOID pPARAM=NULL;
  AfxBeginThread(PortReadThread,pPARAM);
 
  return TRUE;
}

UINT CSerialConnection::PortReadThread(LPVOID lpvoid)
{
	int loopSize = 0;

	BYTE readByte[BUFFER_SIZE];
	BYTE writeByte = 255;

	DWORD dwCommModemStatus;
	DWORD dwBytesTransferred;
	DWORD dwNumBytesWritten;
  
	CString str_LoopSize;

	if (hPort != INVALID_HANDLE_VALUE) 
	{
		/*
		send some data to the harware.This is used for handshaking with the hardware.
		The USB Test code running on hardware will be on wait state until it receives any
		data from the PC 
		*/

		WriteFile (hPort, &writeByte,1,&dwNumBytesWritten,NULL);
		//when a character appears it unblocks waitcommevent 
		SetCommMask (hPort,EV_RXCHAR);
		//waits until a character appears at port
		WaitCommEvent (hPort, &dwCommModemStatus, 0);
		
		while(1)
		{
			if (EV_RXCHAR) 
			{
				SetCommMask (hPort, EV_RXCHAR );
				//read data send from the hardware		
				ReadFile (hPort, &readByte, BUFFER_SIZE, &dwBytesTransferred, 0);
							
				if (dwBytesTransferred >= 1)
				{
					CString str_PortData((LPCSTR)&readByte,sizeof(readByte));
					str_LoopSize.Format("%d",loopSize);
				    loopSize++;
					//output the data to debug output window
					OutputDebugString (str_LoopSize + "Data send by USB = "+ str_PortData + "\n");
				}
			}
		}
	}

    return 0;
}










