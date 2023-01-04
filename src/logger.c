/*
 *
 * This file is part of guard.
 *
 * guard is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) any later version.
 * 
 * guard is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with guard. 
 * If not, see <https://www.gnu.org/licenses/>. 
 * 
 */

#include "logger.h"

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#define DEFAULT_LOG_MSG_BUFFER_SIZE 256

struct log_msg {

  /**
   * The log level
   */
  enum log_level level;
  
  /**
   * The file where the message originates from
   */
  const char * file;

  /**
   * The line where the message originates from
   */
  int line;

  /**
   * The message buffer
   */
  char * buffer;

  /**
   * The buffer capacity
   */
  size_t cap;

  /**
   * The buffer length
   */
  size_t len;

  /**
   * Number of writes still to be completed
   */
  size_t count;

  /**
   * A pointer to the next message
   */
  struct log_msg * next;

};

/**
 * An entry in a log queue
 */
struct log_entry {

  /**
   * A pointer to a message
   */
  struct log_msg * msg;

  /**
   * The next entry in the queue
   */
  struct log_entry * next;
};

/**
 * A queue of log messages
 */
struct log_queue {

  /**
   * The first entry
   */
  struct log_entry * head;

  /**
   * The last entry
   */
  struct log_entry * tail;
};

/**
 * An output channel
 */
struct log_output {

  /**
  * The output file
  */
  FILE * file;

  /**
   *  Whether this output should keep running
   */
  bool running;
  
  /**
   * The queue of messages to print
   */
  struct log_queue queue;
  
  /**
   * Mutex protecting the log queue and the running flag
   */
  pthread_mutex_t mutex;

  /**
   * Condition variable to signal the output worker thread
   */
  pthread_cond_t cond;

  /**
   * The worker thread
   */
  pthread_t thread;

};

/**
 * A ring of available messages
 */
static struct log_msg * messages;

/**
 * A linked list of available entries
 */
static struct log_entry * entries;

/**
 *  The mutex protecting the available message and entry queue
 */
static pthread_mutex_t mutex;

/**
 * The minimum log level
 */
static enum log_level min_level;

/**
 * Log output array
 */
static struct log_output * outputs;

/**
 * The number outputs
 */
static size_t output_len;

/**
 * The capacity of the output buffer
 */
static size_t output_cap;

/**
 * Prefixes for log level messages
 */
static const char * log_level_labels[] = {
					  "DEBUG:  ",
					  "INFO:   ",
					  "WARNING:",
					  "ERROR:  "
};


/*
 * Log message / entry functions
 */

/**
 * Destroys these entries, without destroying the message
 * \param head the first entry or NULL
 */
static void destroy_log_entries(struct log_entry * head) {
  while(head != NULL) {
    struct log_entry * next = head->next;
    free(head);
    head = next;
  }
}

/**
 * Destroys these messages
 * \param head the first message
 */
static void destroy_log_messages(struct log_msg * head) {
  while(head != NULL) {
    struct log_msg * next = head->next;
    if(head->buffer != NULL) {
      free(head->buffer);
    }
    free(head);
    head = next;
  }
}

/**
 * Acquires a linked list of log entries linked to the same message, one for each output
 * \return a linked list or NULL on failure or if there are no outputs
 */
struct log_entry * acquire_log_entries() {
  if(output_len == 0) {
    return NULL;
  }
  if(pthread_mutex_lock(&mutex) != 0) {
    return NULL;
  }
  
  struct log_msg * msg = messages;
  if(msg != NULL) {
    messages = messages->next;
  }

  size_t count = 0;
  struct log_entry * head = entries;
  for(;count < output_len && entries != NULL; ++count) {
    entries = entries->next;
  }
  
  if(pthread_mutex_unlock(&mutex) != 0) {
    return NULL;
  }

  if(msg == NULL) {
    msg = (struct log_msg *) malloc(sizeof(struct log_msg));
    if(msg == NULL) {
      //no sense in trying to recover this
      return NULL;
    }
    char * buffer = (char *) malloc(DEFAULT_LOG_MSG_BUFFER_SIZE);
    if(buffer == NULL) {
      free(msg);
      return NULL;
    }
    msg->buffer = buffer;
    msg->cap = DEFAULT_LOG_MSG_BUFFER_SIZE;
  }

  msg->next = NULL;

  struct log_entry * tail = head;
  for(size_t i = 0; i < count; ++i, tail = tail->next) {
    tail->msg = msg;
  }
  
  for(;count < output_len; ++count) {
    struct log_entry * entry = (struct log_entry *) malloc(sizeof(struct log_entry));
    if(entry == NULL) {
      //very bad => clean up as best we can
      destroy_log_entries(head);
      destroy_log_messages(msg);
      return NULL;
    }
    if(tail == NULL) {
      head = entry;
      tail = entry;
    } else {
      tail->next = entry;
      tail = entry;
    }
    entry->msg = msg;
    entry->next = NULL;
  }
  msg->count = output_len;

  return head;
}

/**
 * Releases the entries
 * \head the first entry or NULL
 * \return 0 on success, -1 on error
 */
static int release_log_entries(struct log_entry * head) {
  if(head != NULL) {
    if(pthread_mutex_lock(&mutex) != 0) {
      return -1;
    }

    struct log_entry * entry = head;
    do {
      struct log_msg * msg = entry->msg;
      if(msg != NULL){
	assert(msg->count != 0);
	--msg->count;
	if(msg->count == 0) {
	  msg->next = messages;
	  messages = msg;
	}
      }
      entry->msg = NULL;
      struct log_entry * next = entry->next;
      entry->next = entries;
      entries = entry;
      entry = next;
    }while(entry != NULL);
  }

  if(pthread_mutex_unlock(&mutex) != 0) {
    return -1;
  }
  
  return 0;
}

/**
 * Destroys all entries and messages
 * Should not be called while the logger is active
 */
static void destroy_log_buffers() {
  destroy_log_messages(messages);
  messages = NULL;
  destroy_log_entries(entries);
  entries = NULL;
}


/**
 * Destroys a linked list of log entries and its associated message
 * \param head the head of the list
 */
static void destroy_log_entries_and_msg(struct log_entry * head) {
  assert(head != NULL);
  assert(head->msg != NULL);
  destroy_log_messages(head->msg);
  destroy_log_entries(head);
}

/**
 * Reallocates the message buffer to the specified capacity
 * \param msg the message
 * \param cap the minimum required capacity
 * \return 0 on success, -1 on failure
 */
static int realloc_log_msg_buffer(struct log_msg * msg, size_t cap) {
  assert(msg != NULL);

  char * buffer = (char *)realloc(msg->buffer, cap);
  if(buffer == NULL) {
    return -1;
  } else {
    msg->buffer = buffer;
    msg->cap = cap;
    return 0;
  }
}

/*
 * Log queue functions
 */

/**
 * Initializes an empty FIFO log queue
 * \param q the queue
 */
void init_log_queue(struct log_queue * q) {
  assert(q != NULL);
  q->head = NULL;
  q->tail = NULL;
}

/**
 * Pushes an entry onto the queue
 * \param q the queue
 * \param e the entry
 */
void push_onto_log_queue(struct log_queue * q, struct log_entry * e) {
  assert(q != NULL);
  assert(e != NULL);
  if(q->tail == NULL) {
    assert(q->head == NULL);
    q->head = q->tail = e;
  } else {
    q->tail->next = e;
    q->tail = e;
  }
  e->next = NULL;
}

/**
 * Pops an entry from a queue
 * \param q the queue
 * \return an entry from the head of the queue or NULL if the queue is empty
 */
struct log_entry * pop_from_log_queue(struct log_queue * q) {
  assert(q != NULL);

  struct log_entry * e = q->head;
  if(e == NULL) {
    assert(q->tail == NULL);
  } else {
    q->head = e->next;
    if(q->head == NULL) {
      q->tail = NULL;
    }
  }
  return e;
} 

/**
 * Moves all entries from the source queue to the destination queue
 * \param dest the destination queue
 * \param src the source queue
 */
void move_between_log_queues(struct log_queue * dest, struct log_queue * src) {
  assert(dest != NULL);
  assert(src != NULL);

  if(src->head != NULL) {
    assert(src->tail != NULL);
    if(dest->tail == NULL) {
      assert(dest->head == NULL);
      dest->head = src->head;
      dest->tail = src->tail;
    } else {
      assert(dest->head != NULL);
      dest->tail->next = src->head;
      dest->tail = src->tail;
    }
    src->head = src->tail = NULL;
  } else {
    assert(src->tail == NULL);
  }
}

/**
 * Clears all messages from the log queue
 */
int clear_log_queue(struct log_queue * q) {
  assert(q != NULL);
  int result = release_log_entries(q->head);
  q->head = NULL;
  q->tail = NULL;
  return result;
}
/**
 * Destroys the log queue, recycling all the messages
 * May block
 * \param q the queue
 * \return 0 if all entries were recovered, -1 otherwise
 */
int dispose_log_queue(struct log_queue * q) {
  assert(q != NULL);
  return release_log_entries(q->head);
}

/*
 * Log output functions
 */

/**
 * Print the log messages
 * \param file the output file
 * \param msg the message to be printed
 */
static void print_log_msg(FILE * file, struct log_msg * msg) {
  assert(file != NULL);
  assert(msg != NULL);
  
  fprintf(file, "%s %s:%d: %s\n", log_level_labels[(int)msg->level], msg->file, msg->line, msg->buffer);
}

/**
 * Prints the log messages on the queue
 * \param file the output file
 * \param q a queue of messages
 */
static void print_log_queue(FILE * file, const struct log_queue * q) {
  assert(file != NULL);
  assert(q != NULL);
  
  struct log_entry * entry = q->head;
  while(entry != NULL) {
    
    print_log_msg(file, entry->msg);

    entry = entry->next;
  }
}

/**
 * Log output worker thread function
 * \param arg the output cast as a void *
 * \return always NULL
 */
static void * run_log_output(void * arg) {
  
  struct log_output * output = (struct log_output *)arg;
  struct log_queue q;

  init_log_queue(&q);
  
  bool run = true;
  
  if(pthread_mutex_lock(&output->mutex) != 0) {
    return NULL;
  }
  do {
    if(!output->running) {
      run = false;
    }
    if(run && output->queue.head == NULL) {
      if(pthread_cond_wait(&output->cond, &output->mutex) != 0) {
	break;
      }
    }
    if(!output->running) {
      run = false;
    }
    move_between_log_queues(&q, &output->queue);
    if(pthread_mutex_unlock(&output->mutex) != 0) {
      return NULL;
    }

    print_log_queue(output->file, &q);

    if(clear_log_queue(&q) != 0) {
      return NULL;
    }
    
    if(pthread_mutex_lock(&output->mutex) != 0) {
      return NULL;
    }
  } while(run);

  pthread_mutex_unlock(&output->mutex);
  
  dispose_log_queue(&q);
  return NULL;
}

/**
 * Starts a log output
 * \param output the output
 * \return 0 on success, -1 otherwise
 */
static int start_log_output(struct log_output * output) {
  assert(output != NULL);
  if(pthread_mutex_init(&output->mutex, NULL) != 0) {
    return -1;
  }
  if(pthread_cond_init(&output->cond, NULL) != 0) {
    pthread_mutex_destroy(&output->mutex);
    return -1;
  }
  output->running = true;
  init_log_queue(&output->queue);

  if(pthread_create(&output->thread, NULL, run_log_output, output) != 0) {
    pthread_cond_destroy(&output->cond);
    pthread_mutex_destroy(&output->mutex);
    return -1;
  }
  return 0;
}


/**
 * Add log entries to the outputs
 * \param head a linked list of entries with a message
 */
static int add_log_entries_to_outputs(struct log_entry * head) {
  for(size_t i = 0; i < output_len; ++i) {
    assert(head != NULL);
    struct log_entry * next = head;

    if(pthread_mutex_lock(&outputs[i].mutex) != 0) {  
      return -1;
    }
    push_onto_log_queue(&outputs[i].queue, head);
    if(pthread_mutex_unlock(&outputs[i].mutex) != 0) {
      return -1;
    }
  }
  return 0;
}

/**
 * Signals the output that it has to stop logging
 * \param output the output
 * \return 0 on success, -1 on failure
 */
static int signal_log_output_stop(struct log_output * output) {
  assert(output != NULL);
  if(pthread_mutex_lock(&output->mutex) != 0) {
    return -1;
  }
  output->running = false;
  if(pthread_cond_signal(&output->cond) != 0) {
    return -1;
  }
  if(pthread_mutex_unlock(&output->mutex) != 0) {
    return -1;
  }
  return 0;
}

/**
 * Wait for an output to stop
 * \param the output
 */
static void wait_for_log_output_stop(struct log_output * output) {
  assert(output != NULL);
  pthread_join(output->thread, NULL);
}

/**
 * Disposes the output
 * \param output the output
 */
static void dispose_log_output(struct log_output * output) {
  assert(output != NULL);
  pthread_cond_destroy(&output->cond);
  pthread_mutex_destroy(&output->mutex);
  dispose_log_queue(&output->queue);
}

/**
 * Stops all outputs
 * \param started the number of log outputs that has actually started
 */
static void stop_and_dispose_log_outputs(size_t started) {
  for(size_t i = 0; i < started; ++i) {
    if(signal_log_output_stop(outputs + i) != 0) {
	// no sense trying
      return;
    }
  }
  for(size_t i = 0; i < started; ++i) {
    wait_for_log_output_stop(outputs + i);
    dispose_log_output(outputs + i);
  }
}

/*
 * Public API implementation
 */

int init_logger(enum log_level min_level_) {
  messages = NULL;
  entries = NULL;
  if(pthread_mutex_init(&mutex, NULL) != 0) {
    return -1;
  }
  min_level = min_level_;
  outputs = NULL;
  output_len = 0;
  output_cap = 0;
  return 0;
}

int add_logger_output(FILE * file) {
  assert(file != NULL);

  if(output_len == output_cap) {
    size_t new_cap;
    if(output_cap == 0){
      new_cap = 1;
    } else {
      new_cap = output_cap * 2;
    }
    struct log_output * buf = (struct log_output *)realloc(outputs, sizeof(struct log_output) * new_cap);
    if(buf == NULL) {
      return -1;
    }
    outputs = buf;
    output_cap = new_cap;
  }
  outputs[output_len].file = file;
  ++output_len;
  return 0;
}

int start_logger() {
  size_t started = 0;
  int result = 0;
  for(; result == 0 && started < output_len; ++started) {
    int r = start_log_output(outputs + started);
    if(r != 0) {
      result = r;
    }
  }
  if(result != 0) {
    stop_and_dispose_log_outputs(started);
  }
  return result;
}

int add_log_message(enum log_level level, const char * file, int line, const char * format, ...) {
  assert(file != NULL);
  assert(format != NULL);

  struct log_entry * head = acquire_log_entries();
  if(head == NULL) {
    return -1;
  }
  struct log_msg * msg = head->msg;
  
  va_list args;
  va_start(args, format);
  int result = vsnprintf(msg->buffer, msg->cap, format, args);
  va_end(args);

  if(result < 0) {
    destroy_log_entries_and_msg(head);
    return -1;
  }
  
  if(result + 1 >= (int)msg->cap) {
    // allocate bigger buffer and try again
    if(realloc_log_msg_buffer(head->msg, (size_t)(result + 1)) != 0) {
      destroy_log_entries_and_msg(head);
      return -1;
    }
    
    va_list args2;
    va_start(args2, format);
    result = vsnprintf(msg->buffer, msg->cap, format, args2);
    va_end(args2);
    if(result < 0 || result + 1 >= (int)msg->cap) {
      destroy_log_entries_and_msg(head);
      return -1;
    }
  }
  
  msg->buffer[result] = '\0';
  msg->level = level;
  msg->file = file;
  msg->line = line;
  return add_log_entries_to_outputs(head);
}

enum log_level get_min_log_level() {
  return min_level;
}

int stop_logger() {
  stop_and_dispose_log_outputs(output_len);
}

void dispose_logger() {
  pthread_mutex_destroy(&mutex);
  destroy_log_buffers();
  free(outputs);
}
