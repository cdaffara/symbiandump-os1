LET = LEave Test-framework


fl:  a cpp filelist
fl (dir1)?

let: main testframework
let -f cpp_file_list -o log_dir
let --std-in -o log_dir

logdiff: print the changes between to log dir
logdiff log_dir_1 log_dir_2