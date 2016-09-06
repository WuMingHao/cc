#ifndef SEMP_H
#define SEMP_H

#include <pthread.h>


/** The structure contains the semaphore value, mutex and green light flag
 */
typedef struct semp_t
{
  pthread_cond_t condition;
  pthread_mutex_t mutex;
  unsigned int semcount;
} semp_t;


class SempUtils {
public:
	/*
	 * \brief Initializes the semaphore at a given count
	 * @param semp the semaphore to initialize
	 * @param val the initial value of the semaphore
	 */
	static void semp_init (semp_t *semp, unsigned int count);

	/*
	 * \brief Deletes the semaphore
	 * @param semp the semaphore to destroy
	 */
	static void semp_deinit (semp_t *semp);

	/*
	 * \brief Decreases the value of the semaphore. Blocks if the semaphore
	 * value is zero.
	 * @param semp the semaphore to decrease
 	 */
	static void semp_pend (semp_t *semp);

	/*
	 * \brief Increases the value of the semaphore
	 * @param semp the semaphore to increase
	 */
	static void semp_post (semp_t *semp);

	/*
	 * \brief Reset the value of the semaphore
	 * @param semp the semaphore to reset
  	 */
	static void semp_reset (semp_t *semp);

	/*
	 * \brief Wait on the condition.
	 * @param semp the semaphore to wait
	 */
	static void semp_wait (semp_t *semp);

	/*
	 * \brief Signal the condition,if waiting
	 * @param semp the semaphore to signal
	 */
	static void semp_signal (semp_t *semp);
};
#endif	// SEMP_H














