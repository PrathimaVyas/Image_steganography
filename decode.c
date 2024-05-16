#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "decode.h"
#include "types.h"


status read_and_validate_decode_args(int argc, char *argv[],DecodeInfo *decInfo)
{
    if(argc >= 3)
    {
	if(strstr(argv[2],".bmp") != NULL)   // if argument 2 in cla is .bmp file then store it in a pointer
	{
	    decInfo -> stego_image_fname = argv[2];
	    printf("Decoded file -> %s\n",decInfo -> stego_image_fname);      
	    if(argc == 4)
	    {
		if(strchr(argv[3],'.') != NULL)  //if argument 3 in cla is having . then remove string after .
		{
		    decInfo -> secret_fname = argv[3];      //store the file name without extension in structure
		    decInfo -> secret_fname = strcpy(decInfo -> secret_fname,strtok(argv[3],"."));
		    printf("secret file name -> %s\n",decInfo -> secret_fname);
		}
		else
		{
		    decInfo -> secret_fname = argv[3];
		    strcpy(decInfo -> secret_fname,argv[3]);       

		    printf("Secret file name -> %s\n",decInfo -> secret_fname);
		}
	    }
	    else
	    {
		argv[3] = "secret_output";  //otherwise give defualt name 
		decInfo -> secret_fname = argv[3];
		printf("Secret file name -> %s\n",decInfo -> secret_fname);
	    }
	}
	else
	{
	    return d_failure;
	}
    }
    else
    {
	printf("Invalid number of arguments\n");
	exit(0);
    }
    return d_success;

}

status do_decoding(DecodeInfo *decInfo)        // All function calls should done in this function 
{

    if(open_file(decInfo) == d_success)
    {
	printf("Files open successfully\n");
	if(decode_magic_string(decInfo->magic_string,decInfo) == d_success)
	{
	    printf("Magic string decoded successfully\n");
	    if(decode_secret_file_extn_size(decInfo) == d_success)
	    {
		printf("Secret file extension size decoded successfully\n");
		if(decode_secret_file_extn(decInfo -> file_extn,decInfo) == d_success)
		{
		    printf("Secret file extension decoded successfully\n");
		    if(decode_secret_file_size(decInfo->secret_file_size,decInfo) == d_success)
		    {
			printf("Secret file size decoded successfully\n");
			if(decode_secret_file_data(decInfo ->secret_data,decInfo) == d_success)
			{
			    printf("Secret file data decoded successfully\n");
			    return d_success;
			}
			else
			{
			    printf("Secret file data decoding unsuccessful\n");
			    return d_failure;
			}
		    }
		    else
		    {
			printf("Secret file size decoding unsuccessful\n");
			return d_failure;
		    }
		}
		else
		{
		    printf("Secret file extension decoding is unsuccessful\n");
		    return d_failure;
		}
	    }
	    else
	    {
		printf("Secret file extension size decoding is unsuccessful\n");
		return d_failure;
	    }
	}
	else
	{
	    printf("Magic string decoding is unsuccessful\n");
	    return d_failure;
	}
    }
    else
    {
	printf("files opening is unsuccessful\n");
	return d_failure;
    }
    return d_success;
}


status open_file(DecodeInfo *decInfo)
{
    decInfo -> fptr_stego_image = fopen(decInfo -> stego_image_fname,"r");

    fseek(decInfo -> fptr_stego_image,54,SEEK_SET);                  //open the encoded file and then set the cursor to 55th byte

    if(decInfo -> fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr,"ERROR: Unable to open file %s\n",decInfo -> stego_image_fname);
	return d_failure;
    }
    decInfo -> magic_string = (char *)malloc(strlen(MAGIC_STRING)+1);
    // dynamic memory allocation for magic_string with magicstring + 1 because to store the magic string and null character
    return d_success;
}

status decode_magic_string(char *magic_string,DecodeInfo *decInfo)
{

    char buffer[8];
    char ch;
    for(int i=0;i<strlen(MAGIC_STRING);i++)
    {
	fread(buffer,8,1,decInfo ->fptr_stego_image);
	ch =decode_byte_from_lsb(&magic_string[i],buffer);         //decode each character by passing 8 bytes of data to the function
	magic_string[i] = ch;
    }

    printf("magic string -> %s\n",decInfo -> magic_string);
    if(strcmp(MAGIC_STRING,decInfo->magic_string)==0)
	return d_success;
    else
	return d_failure;
}


status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    int size = 0; //to find the size of file_extn we need to take it as 0 and then pass it to a function in that function we can find the size
    fread(buffer,32,1,decInfo -> fptr_stego_image);
    decode_size_from_lsb(buffer,&size);
    decInfo -> file_extn_size = size;       //store the file extension size after changing in the function
    decInfo -> file_extn =(char *)malloc(decInfo -> file_extn_size + 1);  //to store the file extn as a string we need extra byte to store null character
    printf("File extension size is %d\n",decInfo -> file_extn_size);
    return d_success;
}

status decode_size_from_lsb(char *buffer,int *size)
{
    for(int i=0;i<32;i++)
    {
	*size = ((buffer[i] & 0x01 ) << (31-i)) | *size;
    }
    printf("size  = %d\n",*size);
    return *size;
}

char decode_byte_from_lsb(char *data,char *buffer)
{
    unsigned char ch = 0x00;
    for(int i=0;i<8;i++)
    {
	ch = (( buffer[i] & 0x01 ) << (7-i)) | ch;
    }
    return ch;               //returning the character after decoding of the encoded output file
}

static char str2[30];
status decode_secret_file_extn(char *file_extn,DecodeInfo *decInfo)
{
    char str[8];  

    for(int i=0;i<decInfo->file_extn_size;i++)
    {
	fread(str,8,1,decInfo ->fptr_stego_image);
	char ch = decode_byte_from_lsb(&file_extn[i],str);
	file_extn[i] = ch;
    }
    printf("File extn = %s\n",decInfo -> file_extn);

    char str1[8];
    strcpy(str1,decInfo -> file_extn);      //after getting the file extension store it in a string to concatenate with the given file name or default file name

    strcpy(str2,decInfo -> secret_fname);

    strcat(str2,str1);


    printf("Secret file name with extension -> %s\n",str2);

    return d_success;
}


status decode_secret_file_size(int secret_file_size,DecodeInfo *decInfo)
{
    char image_buffer[32];
    int size=0;
    fread(image_buffer,32,1,decInfo -> fptr_stego_image);  //read 32 bytes from stego image
    decode_size_from_lsb(image_buffer,&size);
    decInfo -> secret_file_size = size;
    decInfo -> secret_data =  (char *)malloc(decInfo -> secret_file_size+1); // dynamic memory allocation to store the secret data
    printf("Size of secret file -> %d\n",decInfo ->secret_file_size);

    return d_success;
}

status decode_secret_file_data(char *secret_data,DecodeInfo *decInfo)
{
char str[8];
    int i;
    printf("str2 is %s\n",str2);
    decInfo -> fptr_secret = fopen(str2,"w"); // Open file to store the secret data
    if ( decInfo -> fptr_secret == NULL )
    {
	fprintf(stderr,"can't open the file\n");
	return d_failure;
    }
    else
    {
	for ( i = 0;i < decInfo -> secret_file_size ;i++ )
	{
	    fread(str,8,1,decInfo-> fptr_stego_image);
	    // Read 8 bytes from stego image
	    char ch = decode_byte_from_lsb(&secret_data[i],str); // Decode bytes then we get some data of secret file which we encoded then store into secret data pointer
	    fwrite(&ch,1,1,decInfo -> fptr_secret);
	    secret_data[i] = ch;
	}
	return d_success;
    }
}



