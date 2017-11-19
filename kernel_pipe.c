
#include "tinyos.h"
#include "kernel_pipe.h"
#include "kernel_dev.h"


int sys_Pipe(pipe_t* pipe){

	return -1;
}

int pipe_reader(void* dev, char *buf, unsigned int size){

    return -1;
}

int pipe_writer(void* dev, const char* buf, unsigned int size){

    return -1;
}

int pipe_reader_close(void* dev){

    return -1;
}

int pipe_writer_close(void* dev){

    return -1;
}

static file_ops reader_ops = {
        .Read = pipe_reader,
        .Write = pipe_writer,
        .Close = pipe_reader_close
};

static file_ops writer_ops = {
        .Read = pipe_reader,
        .Write = pipe_writer,
        .Close = pipe_writer_close
};