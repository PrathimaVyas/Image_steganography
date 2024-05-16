#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;    //encInfo is variable name for structure

    DecodeInfo decInfo;    //decInfo is variable name for structure DecodeInfo

    uint img_size;         //img_size is variable name for unsigned int

    if(argc == 1)
    {
	printf("INFO : Please pass command line arguments\n");
	return 0;
    }
    int ret;
    ret = check_operation_type(argv);

    if(ret == e_encode) //if ret is 0 then it will perform encoding
    {

        if(read_and_validate_encode_args(argc,argv,&encInfo) == e_success)
        {
            if(do_encoding(&encInfo) == e_success)
            {

                printf("Encoding successful\n");

            }
        }
    }
   else if(ret == e_decode)  //if ret is 1 then it will perform decoding
    {
        if(read_and_validate_decode_args(argc,argv,&decInfo) == d_success)
        {
            if(do_decoding(&decInfo) == d_success)
            {
                printf("Decode successful\n");
            }
        }
    }
    else    //if ret != 0 or 1 then it will print error msg
    {
        printf("No operation is selected\n");
        return 0;
    }
    return 0;
}



OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e") == 0)
    {
        return e_encode; //return 0
    }
    else if(strcmp(argv[1],"-d") == 0)
    {
        return e_decode;  //return 1
    }
    else
    {
        return e_unsupported;  //return 2
    }
}
