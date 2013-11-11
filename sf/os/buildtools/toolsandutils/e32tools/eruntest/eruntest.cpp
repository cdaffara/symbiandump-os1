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
//

#include <windows.h>

#ifdef __MSVCDOTNET__
 #include <iostream>
 #include <fstream>
 using namespace std;
#else //!__MSVCDOTNET__
 #include <iostream.h>
 #include <fstream.h>
#endif //__MSVCDOTNET__

#include <stdio.h>
#include <errno.h>

#if defined(__VC32__) && !defined(__MSVCDOTNET__)
#pragma warning( disable : 4710 )	// 'fn': function not inlined
#endif // old MSVC

const unsigned int KMaxLineLen=256;
const unsigned int KTimeout=600000; // 10 minutes


void NumFileLines(ifstream& inStream, unsigned int &aNumLines)
	{
	// finds out how many lines are in the file specified

	aNumLines=0;
	inStream.clear();
	inStream.seekg(0);

	char str[KMaxLineLen];
	while (!inStream.eof())
		{
		inStream.getline(str, KMaxLineLen);
		aNumLines++;
		}
	}

void GetTestData(ifstream& inStream, char (*aDataArray)[KMaxLineLen], unsigned int &aNumTests)
	{
	// fill the test data structure from the file stream

	aNumTests=0;
	inStream.clear();
	inStream.seekg(0);

	char str[KMaxLineLen]="";
	while (!inStream.eof())
		{
		bool charsPresent=0;
		inStream.getline(str, KMaxLineLen);
		if (strlen(str))
			{
			unsigned int len=strlen(str);
			for (unsigned int i=0; i<len; i++)
				{
				if (!isspace(str[i]))
					{
					charsPresent=1;
					break;
					}
				}
			if (charsPresent)
				{
				strcpy(aDataArray[aNumTests], str);
				aNumTests++;
				}
			}
		}
	}

int GetTestOutFileName(char* aOutFile)
	{
	// Gets the temporary file in which RTest puts its output
		
	const char KTestOutFileName[16]="epocwind.out";
	char tmpDir[KMaxLineLen]="";
	int r=0;

	aOutFile[0]='\0';
	int len=GetTempPath(KMaxLineLen, tmpDir);
	if (len==0||len>KMaxLineLen)
		r=1;
	
	if (!r)
		if (KMaxLineLen > (strlen(KTestOutFileName)+strlen(tmpDir)))
			{
			strcpy(aOutFile, tmpDir);
			strcat(aOutFile, KTestOutFileName);
			}
		else
			r=1;
	return(r);
	}

void RemoveLeadingSpaces(char* aStr)
	{
	// removes leading whitespace from a string

	int spaces=0;
	while (isspace(aStr[spaces]))
		spaces++;
	int newLen=strlen(aStr)-spaces;
	for (int j=0;j<=newLen;j++)
		aStr[j]=aStr[j+spaces];
	}

int TestSucceeded(char* aLastLineBut1)
	{
	// checks whether an EPOC RTest has succeeded by comparing the
	// last line but 1 in the EPOCWIND.OUT file with a template success
	// string

	int r=0;
	const char KSuccessResult[20]="RTEST: SUCCESS :";

	char testStr[KMaxLineLen];
	strcpy(testStr,aLastLineBut1);
	RemoveLeadingSpaces(testStr);
	testStr[strlen(KSuccessResult)]='\0';
	if (strcmp(testStr, KSuccessResult)==0)
		r=1;
	return(r);
	}

int GetPenultimateLines(char* aFile, char* aLastLineBut1, char* aLastLineBut2)
	{
	// Gets the two penultimate lines in a file.
	// Returns 0 if successful, 1 otherwise.

	int r=0;

	aLastLineBut1[0]='\0';
	aLastLineBut2[0]='\0';

	ifstream fileStream(aFile);
	if (!fileStream)
		r=1;

	if (!r)
		{
		char lastLine[KMaxLineLen]="";
		while (!fileStream.eof())
			{
			strcpy(aLastLineBut2, aLastLineBut1);
			strcpy(aLastLineBut1, lastLine);
			fileStream.getline(lastLine, KMaxLineLen);
			}
		}
	return(r);	
	}

int main(int argc, char *argv[])
	{
	FILE *stream = NULL;
	if (argc != 2)
		{
		cerr << "Syntax: eruntest <batch_file>" << endl;
		return(1);
		}
	// Check if input file exists
	if( (stream  = fopen(argv[1], "r" )) == NULL)
		{
		cerr << "ERROR: Cannot open input file " << argv[1] << endl;
		return(1);
		}
	else 
		fclose(stream); 
//	stream the input file
	ifstream inFile(argv[1]);
	
	// get the number of lines in the input file
	unsigned int numLines=0;
	NumFileLines(inFile, numLines);
	
	// allocate space for the tests names
	char (*pTests)[KMaxLineLen];
	pTests = new char[numLines][KMaxLineLen];
	if (!pTests)
		{
		cerr << "ERROR: Out of memory" << endl;
		return(1);
		}

	// populate the data structure for the tests
	unsigned int numTests=0;
	GetTestData(inFile, pTests, numTests);

	// Get the test output file name
	char testOutFile[KMaxLineLen];
	if (GetTestOutFileName(testOutFile)!=0)
		{
		cerr << "Error getting temporary path" << endl;
		return(1);
		}

	// Get the current directory
	char currentDir[KMaxLineLen]="";
	GetCurrentDirectory(KMaxLineLen, currentDir);
	strcat(currentDir, "\\");

	// Retrieve the STARTUPINFO structure for the current process
	STARTUPINFO startUpInfo;
	PROCESS_INFORMATION procInfo;
	GetStartupInfo(&startUpInfo);
	
	unsigned failCount=0;
	unsigned timeoutCount=0;
	unsigned cantStartCount=0;
	unsigned unknownCount=0;

	// run each test in turn
	for (unsigned int i=0; i<numTests; i++)
		{

		// remove epocwind.out
		remove(testOutFile);
		if (errno==EACCES)
			{
			cerr << "Cannot remove " << testOutFile << endl;
			return(1);
			}

		// Create the child process
		if (!CreateProcess(0, pTests[i], 0, 0, FALSE, 0,0, 0, &startUpInfo, &procInfo))
			{
			// int error=GetLastError();
			cout << "CAN'T START: " << currentDir << pTests[i] << endl;
			cantStartCount++;
			continue;
			}

		// Wait for the child process to complete
		int ret=WaitForSingleObject(procInfo.hProcess, KTimeout);
		ifstream testOutFileStream(testOutFile);

		char lastLineBut1[KMaxLineLen]="";
		char lastLineBut2[KMaxLineLen]="";
		switch (ret)
			{
			case WAIT_OBJECT_0:
				// find out if the test terminated successfully
				if (GetPenultimateLines(testOutFile, lastLineBut1, lastLineBut2)!=0)
					{
					cout << "UNKNOWN: " << currentDir << pTests[i] << endl;
					cout << "  <no test output file>" << endl;
					unknownCount++;
					}
				else
					{
					// make the comparison
					if (TestSucceeded(lastLineBut1))
						cout << "PASSED: " << currentDir << pTests[i] << endl;
					else
						{
						cout << "FAILED(RTest): " << currentDir << pTests[i] << endl;
						cout << "  " << lastLineBut2 << endl;
						cout << "  " << lastLineBut1 << endl;
						cout << endl;
						failCount++;
						}
					}
				break;
			case WAIT_FAILED:
				cout << "FAILED: " << currentDir << pTests[i] << endl;
				if (GetPenultimateLines(testOutFile, lastLineBut1, lastLineBut2)!=0)
					{
					cout << "  <no test output file>" << endl;
					}
				else
					{
					cout << "  " << lastLineBut2 << endl;
					cout << "  " << lastLineBut1 << endl;
					cout << endl;
					}
				failCount++;
				break;
			case WAIT_TIMEOUT:
				cout << "TIMED OUT: " << currentDir << pTests[i] << endl;
				if (GetPenultimateLines(testOutFile, lastLineBut1, lastLineBut2)!=0)
					{
					cout << "  <no test output file>" << endl;
					}
				else
					{
					cout << "  " << lastLineBut2 << endl;
					cout << "  " << lastLineBut1 << endl;
					cout << endl;
					}
				timeoutCount++;
				if (!TerminateProcess(procInfo.hProcess, 1))
					{
					cout << "FROZEN: " << currentDir << endl;
					cout << "  Cannot terminate - kill via Task Manager"  << endl;
					cout << endl;
					}
				break;
			}
		}

	delete [] pTests;

	cout << endl;
	cout << "TotalErrors   " << dec << (failCount+timeoutCount+unknownCount+cantStartCount) << endl;
	cout << "  Failures    " << dec << failCount << endl;
	cout << "  Timeouts    " << dec << timeoutCount << endl;
	cout << "  Unknown     " << dec << unknownCount << endl;
	cout << "  Can't start " << dec << cantStartCount << endl;
	cout << endl;

	return(0);
	}
