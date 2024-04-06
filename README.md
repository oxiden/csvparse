csvparse
=====
Parse CSV files and output them as is.

When the filter mode is specified, replace line breaks in the CSV data with the following strings and output them.
* LF...`${LF}`
* CR...`${CR}`
* CRLF...`${CRLF}`

Installation
-----
```
$ make
```

Usage
-----
```
$ ./csvparse [--filter] file [file...]
```
