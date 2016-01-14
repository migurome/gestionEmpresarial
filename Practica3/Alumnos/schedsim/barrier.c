#include "barrier.h"
#include <errno.h>


#ifdef POSIX_BARRIER

/* Wrapper functions to use pthread barriers */

int sys_barrier_init(sys_barrier_t* barrier, unsigned int nthreads) {   
    return pthread_barrier_init(barrier,NULL,nthreads);
}

int sys_barrier_destroy(sys_barrier_t* barrier) {
    return pthread_barrier_destroy(barrier);
}

int sys_barrier_wait(sys_barrier_t *barrier) {
    return pthread_barrier_wait(barrier);
}

#else


/* Barrier initialization function */
int sys_barrier_init(sys_barrier_t *barrier, unsigned int nr_threads)
{
   /* Initialize fields in sys_barrier_t
        ... To be completed ....
   */
    pthread_cond_init(&barrier->cond,0);
    barrier->max_threads = nr_threads;
    barrier->nr_threads_arrived[0] = 0;


    return 0;
}

/* Destroy barrier resources */
int sys_barrier_destroy(sys_barrier_t *barrier)
{
   /* Destroy synchronization resources associated with the barrier
         ... To be completed ....
   */
    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
   return 0;
}

/* Main synchronization operation */
int sys_barrier_wait(sys_barrier_t *barrier)
{
    /* Implementation outline:
       - Every thread arriving at the barrier adquires the lock and increments the nr_threads_arrived 
        counter atomically
         * In the event this is not the last thread to arrive at the barrier, the thread
           must block in the condition variable
         * Otherwise...
            1. Reset the barrier state in preparation for the next invocation of sys_barrier_wait() and
            2. Wake up all threads blocked in the barrier
       - Don't forget to release the lock before returning from the function
       
        ... To be completed ....  
    */
/*
    pthread_mutex_lock(&barrier->mutex);  // echa el cerrojo
    ++(barrier->nr_threads_arrived[0]);  //incrementar el numero de CPUS
    
    if(barrier->nr_threads_arrived[0] >= barrier->max_threads){  //si llegan todas
      barrier->nr_threads_arrived[0] = 0;  //se inicializa el contador de CPUs
      pthread_cond_broadcast(&barrier->cond); //despertamos al resto de CPUs
      pthread_mutex_unlock(&barrier->mutex);  
      return 1;
    }
    else{ // se echa a dormir
      pthread_cond_wait(&barrier->cond, &(barrier-> mutex));
      pthread_mutex_unlock(&barrier->mutex);
    }
    */
    return 0;
}

#endif /* POSIX_BARRIER */
