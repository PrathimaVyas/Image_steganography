#ifndef TYPES_H
#define TYPES_H

#define MAGIC_STRING "#*"
/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    e_success,
    e_failure
} Status;
typedef enum
{
    d_success,
    d_failure
} status;

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif
