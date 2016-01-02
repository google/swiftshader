// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include <string.h>
#include <stdio.h>

#include "serialvalid.h"
#include "crc.h"

static void DecryptString( char* string_to_decrypt, int string_length );

bool ValidateSerialNumber( const char* inSerialNumber, const char* checksum_key, const char* prefix)
{
	if (strlen(inSerialNumber) != SERIAL_LENGTH)
	{
		return false;
	}
	
	if (strstr(inSerialNumber, prefix) != inSerialNumber)
	{
		return false;
	}

	char working_buffer[256];
	char decrypted_serial[SERIAL_LENGTH+1];
	long actual_checksum = 0;
	
	strncpy(decrypted_serial, inSerialNumber, SERIAL_LENGTH);
	decrypted_serial[SERIAL_LENGTH] = '\0';
	
	DecryptString( &(decrypted_serial[PREFIX_LENGTH]),ROOT_LENGTH + CHECKSUM_LENGTH);
	
	char dummy_prefix[10];
	long long serial_number;

	
	sscanf( decrypted_serial, "%2s%10llx%4lx", dummy_prefix, &serial_number, &actual_checksum);
	strcpy(working_buffer, checksum_key);
	strcat(working_buffer, decrypted_serial);
	working_buffer[strlen(checksum_key) + PREFIX_LENGTH + ROOT_LENGTH] = '\0';

	crcInit();
	crc calculated_checksum = crcFast((unsigned char*)working_buffer, strlen(working_buffer));
	
	
	if (calculated_checksum == actual_checksum)
		return true;
	else
		return false;
}


static void DecryptString( char* string_to_decrypt, int string_length )
{
	for ( int i = 0 ; i < string_length ; i++)
	{
		switch (string_to_decrypt[i])
		{
			case 'c':
			case 'C':
				string_to_decrypt[i] = 'a';
				break;
			case 'd':
			case 'D':			
				string_to_decrypt[i] = 'b';
				break;
			case 'g':
			case 'G':			
				string_to_decrypt[i] = 'c';
				break;
			case 'f':
			case 'F':			
				string_to_decrypt[i] = 'd';
				break;
			case 'h':
			case 'H':			
				string_to_decrypt[i] = 'e';
				break;
			case 'b':
			case 'B':			
				string_to_decrypt[i] = 'f';
				break;
			case '2':			
				string_to_decrypt[i] = '0';
				break;
			case '3':			
				string_to_decrypt[i] = '1';
				break;
			case '4':			
				string_to_decrypt[i] = '2';
				break;
			case '5':			
				string_to_decrypt[i] = '3';
				break;
			case '6':			
				string_to_decrypt[i] = '4';
				break;
			case '7':			
				string_to_decrypt[i] = '5';
				break;
			case '8':			
				string_to_decrypt[i] = '6';
				break;
			case '9':			
				string_to_decrypt[i] = '7';
				break;
			case 'j':
			case 'J':			
				string_to_decrypt[i] = '8';
				break;
			case 'k':
			case 'K':			
				string_to_decrypt[i] = '9';
				break;
			default:
				break;
		
		}
	}
}
