#include "eabstractanalytic_cuda_worker.h"
#include "ace_settings.h"
#include "edebug.h"



//






/*!
 * Returns the total number of threads ACE uses to run CUDA workers in parallel.
 *
 * @return Total number of CUDA worker threads.
 */
int EAbstractAnalytic::CUDA::Worker::threadSize()
{
   EDEBUG_FUNC()

   return Ace::Settings::instance().threadSize();
}
