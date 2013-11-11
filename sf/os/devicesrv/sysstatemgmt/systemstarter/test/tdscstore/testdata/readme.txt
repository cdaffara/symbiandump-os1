----------------------------------------------------------------------------------------------------------------------
_________
TEST DATA
_________

INI files are used to specify the data for PREQ871.
INI files contain the data for all the test cases for PREQ871.

The structure of data in INI file is divided into 3 parts : 
1. DSC UID
	This is the UID to be created/modified for the particular DSC.
2. AMA
	This contains the AMA filenames and arguments which will be passed to the EXE/APP during its launch.
	Various commands can be mentioned here with respect to AMA Params like CreateWith,ModifyWith or CompareWith.
	CreateWith command creates the AMA record in the data store with AMA params mentioned.
	CompareWith command compares the parameters specified in the data (INI) file with that present in the
	data store.
	ModifyWith command updates the DSC record with the AMA parameters.
	The commands are mentioned with AMA Params, to be used for creation, updation or comparison.
3. AMA Params

The division of data in the above metioned way in the data  (INI) file, make reuse of the same data for multiple test
cases possible. Same AMAs or same AMA Params can be used for multiple test cases and thus preventing the over growing
of test data without any specific requirement.

----------------------------------------------------------------------------------------------------------------------
________________
ENUMERATION FILE
________________

To make the data in the data file (INI) readable, the actual enums are specified there which are used in production code.
There can be multiple enum files (../sysstarttest/ini/tdscstore_enumfile.ini) if required , and the enumfile can be specified in
the test data file, which will be used to resolve the enumerations to actual value.
Allowing this way, we may not be required to change the test code with any new values introduced or any existing values
modified.

----------------------------------------------------------------------------------------------------------------------