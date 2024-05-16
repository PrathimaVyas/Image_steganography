#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"

char file_extn[MAX_FILE_SUFFIX];

Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo)  //Status is enum variable we have to return e_success or e_failure
{
    printf("length of magic string= %ld\n",strlen(MAGIC_STRING));
    if(argc >= 4)
    {
	if(strstr(argv[2],".bmp") != NULL) // check if 3rd argument in cla is .bmp file or not
	{
	    encInfo->src_image_fname = argv[2];
	    printf("Input file -> %s\n",encInfo->src_image_fname);
	    if(strchr(argv[3],'.') != NULL)  //check if 4th argument in cla is having . or not if it is present then store from . to null in structure
	    {
		strcpy(encInfo->extn_secret_file,strchr(argv[3],'.'));
		encInfo->secret_fname = argv[3];         
		printf("secret file extension -> %s\n",encInfo->extn_secret_file);  //print secretfile extension
		printf("secret file name -> %s\n",encInfo->secret_fname);
		if(argc == 5)
		{
		    if(strstr(argv[4],".bmp") != NULL)   //check if 5th argument in file is .bmp file or not
		    {
			encInfo->stego_image_fname = argv[4];           //store the secret file name in structure
			printf("output file name -> %s\n",encInfo->stego_image_fname);
		    }
		}
		else
		{
		    argv[4] = "output.bmp";
		    encInfo->stego_image_fname = argv[4] ;         //if there is no bmp file then give default name for the output file
		    printf("output file name -> %s\n",encInfo->stego_image_fname);
		}
	    }
	    else
	    {
		return e_failure;
	    }
	}
	else
	{
	    return e_failure;
	}
	return e_success;
    }
    else
    {
	printf("Invalid number of arguments\n");
	exit(0);
    }
}


Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success) //if we give &encInfo it will act as double pointer
    {
	printf("Files opening successful\n");
	if(check_capacity(encInfo) == e_success)
	{
	    printf("Check capacity is done\n");
	    if(copy_bmp_header(encInfo -> fptr_src_image,encInfo ->  fptr_stego_image) == e_success)
	    {
		printf("Copy bmp header is done\n");
		if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
		{
		    printf("Magic string encoded successfully\n");
		    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success)
		    {
			printf("Secret file extn size encoded successfully\n");
			if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)
			{
			    printf("secret file extn encoded successfully\n");
			    if(encode_secret_file_size(encInfo -> size_secret_file,encInfo) == e_success)
			    {
				printf("secret file size encoded successfully\n");
				if(encode_secret_file_data(encInfo) == e_success)
				{
				    printf("secret file data encoded successfully\n");
				    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo -> fptr_stego_image) == e_success)
				    {
					printf("Remaining data copied successfully\n");
					return e_success;
				    }
				    else
				    {
					printf("Copying remaining data unsuccessful\n");
				    }
				}
				else
				{
				    printf("secret file data encoding unsuccessful\n");
				}
			    }
			    else
			    {
				printf("secret file size encoding unsuccessful\n");
			    }
			}
			else
			{
			    printf("secret file extension encoding unsuccessful\n");
			}

		    }
		    else
		    {
			printf("Secret file extension size encoding is unsuccessful\n");
		    }

		}
		else
		{
		    printf("Magic string encoding is unsuccessful\n");
		}
	    }
	    else
	    {
		printf("copy bmp header is unsuccessful\n");
	    }
	}
	else
	{
	    printf("check capacity is unsuccessful\n");
	}
    }
    else
    {
	printf("files opening is unsuccessful\n");
    }
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");  //source image is taken in arguments
								     // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_file_size(encInfo -> fptr_src_image);  //input image file size
    printf("size of bmp file -> %d\n",encInfo -> image_capacity);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
    printf("size of secret file -> %ld\n",encInfo -> size_secret_file);  //secret file size

    // int magic_string_length = strlen(MAGIC_STRING);
    int len_of_extn = strlen(encInfo -> extn_secret_file); //length of secret file size extension

    if(((strlen(MAGIC_STRING) + 4 + 4 + len_of_extn + encInfo -> size_secret_file ) * 8) < get_image_size_for_bmp(encInfo -> fptr_src_image)) //if the sgiven size is less than input file size then we can do encoding
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_stego_image)        //copy 54 bytes of header to the output file
{
    char buffer[54];
    rewind(fptr_src_image);
    fread(buffer, 54 , 1, fptr_src_image);  //read 54 bytes from the input file
    fwrite(buffer, 54, 1, fptr_stego_image); //write 54 bytes to the output file
    return e_success;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);              //to find the size of input file and secret file
}

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)              //to find the size of the file
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;     //image size for the bmp file
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo)
{
    encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image,FILE *fptr_stego_image) //data will be full string
{
    char image_buffer[8];
    for(int i=0;i<size;i++)  //size --> length of magic string
    {
	fread(image_buffer , 8 , 1 , fptr_src_image);          //magic string contains characters so each time it will read 8 bytes of data from input image to encode 8 bits of data
	encode_byte_to_lsb(data[i],image_buffer); // data means magic string    each time 1 byte data will be passed               
	fwrite(image_buffer , 8, 1, fptr_stego_image);
    }
    return e_success;
}


Status encode_byte_to_lsb(char data, char *image_buffer)
{
    unsigned int s = data;
    for(int i=0;i<8;i++)
    {
	image_buffer[i] =(image_buffer[i] & 0xFE) | (unsigned)((s & ( 1 << (7 - i))) >> (7 - i));    //replacing the lsb bit each time based on decimal values of the characters
    }
    return e_success;
}


Status encode_secret_file_extn_size(long extn_size,EncodeInfo *encInfo)
{
    // extn_size =4;
    char image_buffer2[32];  //secret file extension is 4 bytes it has 32 bits so we need 32 bytes to encode
    fread(image_buffer2 , 32 , 1, encInfo -> fptr_src_image);  //read 32 bytes after 54 bytes of header
    encode_size_to_lsb(extn_size,image_buffer2);
    fwrite(image_buffer2 , 32, 1, encInfo -> fptr_stego_image);  //write 32 bytes of inputfile after encoding
    return e_success;
}

Status encode_size_to_lsb(long extn_size,char *image_buffer)
{
    for(int i=0;i<32;i++)
    {
	image_buffer[i] = (image_buffer[i] & 0xFE ) | (unsigned)((extn_size & (1 << (31-i))) >> (31-i));  //to replacee the lsb bit for the size of the files we need 32 bytes of data because size is int for int there are 32 bits to encode each 32 bits of data we need 32 bytes
    }
    return e_success;
}

Status encode_secret_file_extn(char *file_extn , EncodeInfo *encInfo)
{
    printf("Length of file extension is %ld\n",strlen(encInfo->extn_secret_file));      //print the length secret file extension
    encode_data_to_image(file_extn,strlen(encInfo->extn_secret_file),encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    return e_success;
}


Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)
{
    char image_buffer2[32];
    fread(image_buffer2 , 32 , 1, encInfo -> fptr_src_image);
    encode_size_to_lsb(file_size,image_buffer2);
    fwrite(image_buffer2 , 32, 1, encInfo -> fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data( EncodeInfo *encInfo )
{
	char secret_data[encInfo -> size_secret_file];
	fseek(encInfo -> fptr_secret,0,SEEK_SET); // Moving the file pointer to the first position
	fread( secret_data,encInfo -> size_secret_file,1,encInfo -> fptr_secret ); // reading secret file size bytes (25 bytes) of data from fptr_secret
	char str[8];
	for ( int i = 0;i <  encInfo -> size_secret_file; i++ )
	{
		fread(str,8,1,encInfo -> fptr_src_image);
		encode_byte_to_lsb(secret_data[i],str);
		fwrite(str,8,1,encInfo -> fptr_stego_image);
	}
	return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,1,1,fptr_src) > 0)     //copy remaining data after encoding the bits
    {
	fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
}


