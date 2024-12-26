# ofx

This package aims to implement a subset of the [OFX specification](https://www.financialdataexchange.org/FDX/About/OFX-Work-Group.aspx?WebsiteKey=deae9d6d-1a7a-457b-a678-8a5517f8a474&a315d1c24e44=2#a315d1c24e44) that helps importing downloaded transactions from a financial instituion.

At the moment, an ofx file that begins with  
	OFXHEADER:100  
	DATA:OFXSGML  
	VERSION:102  
	SECURITY:NONE  
	ENCODING:USASCII  
	CHARSET:1252  
	COMPRESSION:NONE  
	OLDFILEUID:NONE  
	NEWFILEUID:NONE  
is supported.
