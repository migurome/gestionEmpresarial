#include "sched.h"

static task_t* pick_next_task_fcfs(runqueue_t* rq,int cpu) {    //coge la siguiente tarea que trae el pipe.se ejecuta cada vez que el simulador necesota una tarea.
    task_t* t=head_slist(&rq->tasks); //List sorted by CPU burst lenght (just pick the first one)
    
    if (t) {
        /* Current is not on the rq*/
        remove_slist(&rq->tasks,t); // borra la tarea de la lista
        t->on_rq=FALSE; //sice que ya no esta en la lista
        rq->cur_task=t; // la tarea actual en el pipe es esa 't'
    }
    
    return t;
}
/* se borra
static int compare_tasks_cpu_burst(void *t1,void *t2) {
	task_t* tsk1=(task_t*)t1;
	task_t* tsk2=(task_t*)t2;
	return tsk1->runnable_ticks_left-tsk2->runnable_ticks_left;
}*/

//encolar tarea
static void enqueue_task_fcfs(task_t* t,int cpu, int runnable) {    
    runqueue_t* rq=get_runqueue_cpu(cpu);
    
    if (t->on_rq || is_idle_task(t)) //si esta en la lista o es idle
        return;

    insert_slist(&rq->tasks,t);  //se inserta en la lista y actualizamos a true
    t->on_rq=TRUE;
    //numero de elementos en la lista
    if(!runnable){
        rq->nr_runnable++; // incrementar numero de elementos en la lista
        t->last_cpu=cpu;    
    }
   
}

// ejecuta un ciclio del simulador
static void task_tick_fcfs(runqueue_t* rq,int cpu){
    
    task_t* current=rq->cur_task;   //cogemos la tarea de la rq que esta en la ruta de datos
    
    if (is_idle_task(current))
        return;
    
    if (current->runnable_ticks_left==1) // si es el ultimo tick,  actualizas
        rq->nr_runnable--; // The task is either exiting or going to sleep right now    
}

static task_t* steal_task_fcfs(runqueue_t* rq,int cpu){
    task_t* t=tail_slist(&rq->tasks);
    
    if (t) {
        remove_slist(&rq->tasks,t);
        t->on_rq=FALSE;
        rq->nr_runnable--;
    }
    return t;    
}

sched_class_t fcfs_sched={
    .pick_next_task=pick_next_task_fcfs,
    .enqueue_task=enqueue_task_fcfs,
    .task_tick=task_tick_fcfs,
    .steal_task=steal_task_fcfs    
};