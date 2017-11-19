#ifndef __KERNEL_PIPE_H
#define __KERNEL_PIPE_H

#define SIZE_BUFFER 32768            /** 32 KB  **/


typedef struct pipe_control_block {

    char buffer[SIZE_BUFFER];

    int rp;             /** Reader Pointer :  a pointer to the last byte red **/
    int wp;             /** Writer Pointer :  a pointer to the last byte written **/

    int leftovers;      /** Bytes left to read **/
    int avail_space;    /** SIZE_BUFFER - leftovers **/

    CondVar has_space;  /**  **/
    CondVar has_data;   /**  **/

} PPCB;

int pipe_reader(void* dev, char *buf, unsigned int size);

int pipe_writer(void* dev, const char* buf, unsigned int size);

int pipe_reader_close(void* dev);

int pipe_writer_close(void* dev);

#endif