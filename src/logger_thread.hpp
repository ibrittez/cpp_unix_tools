/** @file logger_thread.hpp
 *
 * @brief A description of the module’s purpose.
 */

#ifndef LOGGER_THREAD_H
#define LOGGER_THREAD_H

#include "RingBuffer.hpp"
#include <atomic>

void logger_thread(RingBuffer& ring, std::atomic<bool>& running);

#endif /* LOGGER_THREAD_H */

/*** end of file ***/
