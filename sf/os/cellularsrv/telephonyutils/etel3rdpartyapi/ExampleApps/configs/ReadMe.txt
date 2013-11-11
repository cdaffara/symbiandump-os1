Instructions for installing config files for sim tsy:

-1-

Place config.txt and TelephonyISVAppConfig.cfg into epoc32\winscw\c\ directory of phone

From the command prompt go to the your source root directory 

Change directory to epoc32/release/winscw/udeb

run this command	

#>ced c:\TelephonyISVAppConfig.cfg

check the generated ced.log file to be sure the config file has been read successfully

-2-

I suggest putting this line in \\epoc32\data\epoc.ini

startupmode 1

As the applications use a purely console based interface.


-- you have now successfully configured the simtsy to run the applications--

Now import the mmp file of the application you'd like to run, build and run the project.