//TODO: copyrights

#ifndef __CPU_WORKER_H__
#define __CPU_WORKER_H__

/**
* \brief Main loop of the CPU worker threads
*
* This function is run by as many thread as they are CPU cores on the host
* system. As explained by \ref events , this function waits until there
* are \c Coal::Event objects to process and handle them.
*/
void *worker(void *data);

#endif
