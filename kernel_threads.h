#ifndef __KERNEL_THREADS_H
#define __KERNEL_THREADS_H

#include "tinyos.h"
#include "kernel_sched.h"
#include "util.h"
#include "kernel_streams.h"

#define CURPTCB  (CURTHREAD->ptcb_parent)


typedef enum ptcb_state_e{
    INIT_STATE,
    DETACHED,
    EXITTED
} ptcb_state;

typedef struct process_thread_control_block{

    rlnode PTCB_node;            /** PTCB node of PTCB list which is in PCB **/

    int iam_mainthread;

    int exitval;                /** The exit value */
    int ref_counter;            /** A counter of joined threads */

    int argl;                   /** The main thread's argument length */
    void* args;                 /** The main thread's argument string */

    CondVar cv;
    Task pcb_task;

    ptcb_state state;
    PCB* pcb_owner;
    TCB* tcb;

} PTCB;

void start_thread();

PTCB* initializePTCB(Task task, int argl, void* args);

PTCB* initialize_main_PTCB();

void deletePTCB(PTCB* ptcb);

int detachPTCB(PTCB* ptcb);


#endif

