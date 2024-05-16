#ifndef DECODE_H
#define DECODE_H

#include "types.h"

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char *magic_string;


    /* secret file Info */
    char *secret_fname;
    FILE *fptr_secret;
    int file_extn_size;
    char *file_extn;
    char *secret_data;
    int secret_file_size;
} DecodeInfo;


/* decode the secret file data */
status decode_secret_file_data(char *secret_data,DecodeInfo *decInfo);

/* decode secret file size */
status decode_secret_file_size(int secret_file_size,DecodeInfo *decInfo);

/* decode file extension size */
status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* decode file extension */
//status decode_secret_file_extn(char *secret_file_extn,DecodeInfo *decInfo);
status decode_secret_file_extn(char *file_extn,DecodeInfo *decInfo);

/* decode magic string */
status decode_magic_string(char *magic_string,DecodeInfo *decInfo);

/* no.of arguments in cla should be atlest 3 */
status read_and_validate_decode_args(int argc,char *argv[],DecodeInfo *decInfo);

/* do decoding for the encoded data */
status do_decoding(DecodeInfo *decInfo);

/* open the files */
status open_file(DecodeInfo *decInfo);

/* decode the size from the lsb position in each byte */
status decode_size_from_lsb(char *buffer,int *size);

/* decode the each byte from the lsb position */
char decode_byte_from_lsb(char *data,char *buffer);




#endif
