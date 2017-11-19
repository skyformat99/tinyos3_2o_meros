
#include "tinyos.h"
#include "kernel_sched.h"
#include "kernel_proc.h"
#include "kernel_threads.h"
#include "kernel_cc.h"

/** 
  @brief Create a new thread in the current process.
  */

Tid_t sys_CreateThread(Task task, int argl, void* args){

    PTCB* ptcb = initializePTCB(task,argl,args);                         /** Initialize PTCB **/
    
    ptcb->pcb_owner = CURPROC;

    if(task != NULL){
        TCB* tcb = spawn_thread(ptcb->pcb_owner,start_thread);             /** Create the TCB with arguments ptcb->owner and as *func a new function
                                                                               --start thread()-- which uses CURPTCB task , argl and args **/
        ptcb->tcb = tcb;
        ptcb->pcb_owner->threads_counter++;
        tcb->ptcb_parent = ptcb;
        wakeup(ptcb->tcb);                                                 /** Wake up TCB in order to be added to scheduler **/
    }

    rlnode_init(&ptcb->PTCB_node,ptcb);                                /** Initialize PTCB node **/
    rlist_push_back(&CURPROC->PTCB_list,&ptcb->PTCB_node);             /** Add PTCB in PTCB list of current PCB **/

    return (Tid_t) ptcb;                                 /** We return *PTCB (and not *TCB) casted as (Tid_t) in order to make safer deletion of PTCB and TCB **/
}

/**
  @brief Return the Tid of the current thread.
 */
Tid_t sys_ThreadSelf(){

    return (Tid_t) CURPTCB;
}

/**
  @brief Join the given thread.
  */
int sys_ThreadJoin(Tid_t tid, int* exitval){

    int ret_value = -1;

    PTCB* ptcb = (PTCB*) tid;

    /**
          Parameters to be aware:

            1. Thread to be joined must exist
            2. Thread to be joined must not be the same
            3. Thread must not detached
            4. Other threads must not try to join in the same time

     **/
                    /** 1 **/                                              /** 2 **/                /** 3 **/                     /** 4 **/
    if((rlist_find(&ptcb->pcb_owner->PTCB_list,ptcb,NULL) != NULL) && (ptcb != sys_ThreadSelf()) && (ptcb->state != DETACHED) && (CURTHREAD->state != STOPPED)){

        ptcb->ref_counter++;             /** Thread is to be joined **/

        while(ptcb->state != EXITTED && ptcb->state != DETACHED) kernel_wait(&ptcb->cv,SCHED_USER);  /** Thread to be joined will must wait while it is not exited and not detached **/

        ptcb->ref_counter --;                                       /** Thread has joined so there is no reason to be as a joined thread **/

        deletePTCB(ptcb);                                            /** Try to release PTCB **/

        if(exitval != NULL) *exitval = ptcb->exitval;               /** In case of exitval is non-NULL, we put this value in PTCB's exitval **/

        ret_value = 0;
        return ret_value;           /** Return 0 because we succeded in join **/
    }

    return ret_value;               /** Return -1 , because we failed to join thread **/
}

/**
  @brief Detach the given thread.
  */
int sys_ThreadDetach(Tid_t tid){

    PTCB* ptcb = (PTCB*) tid;

    int ret_value = detachPTCB(ptcb);       /** Try to detach thread **/

	return ret_value;
}

/**
  @brief Terminate the current thread.
  */
void sys_ThreadExit(int exitval){

    CURPTCB->exitval = exitval;

    Cond_Broadcast(&CURPTCB->cv);

    kernel_sleep(EXITED,SCHED_USER);

}

void start_thread(){

    int exitval;

    Task call = CURPTCB->pcb_task;
    int argl = CURPTCB->argl;
    void* args = CURPTCB->args;

    exitval = call(argl,args);
    ThreadExit(exitval);

}

PTCB* initializePTCB(Task task, int argl, void* args){

    PTCB* ptcb = (PTCB*) xmalloc(sizeof(PTCB));                          /** Allocate new PTCB via xmalloc **/

    ptcb->pcb_task = task;
    ptcb->argl = argl;
    ptcb->args = args;
    ptcb->ref_counter = 1;                                              /** This is the first reference ! **/
    ptcb->state = INIT_STATE;
    ptcb->cv = COND_INIT;
    ptcb->iam_mainthread = 0;

    return ptcb;

}

PTCB* initialize_main_PTCB(){

    PTCB* ptcb = (PTCB*) xmalloc(sizeof(PTCB));    /** Allocate new PTCB via xmalloc **/
    ptcb->state = INIT_STATE;
    ptcb->cv = COND_INIT;
    ptcb->iam_mainthread = 1;       /** It is main thread ! **/

    return ptcb;

};

void deletePTCB(PTCB* ptcb){

    if(ptcb->ref_counter == 0 && ptcb->state == EXITTED){   /** We release PTCB when thread is not joined with any other thread and state is exited **/
        rlist_remove(&ptcb->PTCB_node);
        free(ptcb);
    }
}

int detachPTCB(PTCB* ptcb){

    int ret_value = -1;

    if((rlist_find(&ptcb->pcb_owner->PTCB_list,ptcb,NULL) != NULL) && (ptcb->state != EXITTED)){          /** Try to find thread that will be detached **/
        ptcb->state = DETACHED;         /** Update state to DETACHED **/
        ptcb->ref_counter = 1;          /** Init reference counter **/
        Cond_Broadcast(&ptcb->cv);            /** Wake up all blocked PTCBs on this condition variable **/
        ret_value = 0;
        return ret_value;       /** We succeded in, return 0 **/
    }

    return  ret_value;            /** We failed to detach thread, return -1 **/
}