#include </home/boo1ean/src/servermy/servermy_global.h>

// Struct for storing clients database
typedef struct clients_queue{
  pid_t  pid;
  char   *shared_memory_pointer;
  struct clients_queue *next;
} clients_queue;

// Pointers to start and end of clients queque
struct clients_queue *first_client, *last_client;

// Check if lock for current server is ok
char servermy_lock(); 

// Auth clien
char client_auth();

// Check if client already exists
clients_queue *client_exists(pid_t);

// Set sync to clients
void listen_queue();


/***************************************/
/**************** MAIN *****************/
int main(int argc, const char *argv[]) {
  
  first_client = last_client = NULL;

  // Try to init shared memory segment
  if (init_shared_memory_segment() == SHM_CREAT_ERROR) {
    return SERVER_SHM_ERROR;
  }
  
  // Set counter to check lock at startup
  char check_locking_counter = SERVER_CHECK_LOCKING;

  // Set very powerfull usermask
  umask(0666);

  // Init locking struct
  lock.l_type    = F_WRLCK;   // Set lock

  // Clear server status
  shared_memory_pointer[_SERVER_STATUS] = shared_memory_pointer[_SERVER_CMD] = 0;

  // Listen events
  do {
    
    // Check for locking every SERVER_CHECK_LOCKING time
    if (check_locking_counter == SERVER_CHECK_LOCKING) {
      check_locking_counter = 0;
      if (servermy_lock() == LOCKING_ERROR) {
        return SERVER_ALREADY_RUNNING;
      }
    } else {
      check_locking_counter++;
    }
    
    // If there is some request
    if (shared_memory_pointer[_SERVER_CMD]) {
      // Set busy status
      shared_memory_pointer[_SERVER_STATUS] = 0 | _STATUS_BUSY;

      // Switch commands
      switch (shared_memory_pointer[_SERVER_CMD]) {
        // Make auth by pid
        case _CMD_AUTH  : {
          printf("Authentication\t");
          client_auth();
          break;
        }
      }
      
      // Server is free now
      shared_memory_pointer[_SERVER_STATUS] = shared_memory_pointer[_SERVER_CMD] = 0;
    }
    
    // Serve all clients
    listen_queue();

    // Relax
    usleep(SERVER_SYNC);

  } while (1);

  return 0;
}
/************** ENDMAIN ****************/
/***************************************/


/* 
 * retrun LOCKING_ERROR on fail
 * return LOCKING_SUCCESS on success
 *
 */
char servermy_lock() {
  // Close current lock_file
  close(lock_file);
  
  // Create or open lockfile
  if ((lock_file = open(LOCK_PATH, O_CREAT | O_RDWR)) == -1) {
    perror("Something went wrong. Sure that only one instance of servermy is running.");
    return LOCKING_ERROR;
  }
  
  // Try to set lock for lockfile
  if (fcntl(lock_file, F_SETLK, &lock) == -1) {
    // If can't set file lock, show error message and return
    printf("Servermy locking error. Servermy is already running.\n");
    return LOCKING_ERROR;
  }
  // Successfully lock server
  return LOCKING_SUCCESS;
}

char client_auth() {
  struct clients_queue *node;
  // Get pid of sender
  pid_t  pid = *((pid_t*)(shared_memory_pointer + _SERVER_DATA));

  // Check if there is queue
  if (first_client == NULL) {
    first_client = node = (clients_queue*)malloc(sizeof(clients_queue));
  } else if (client_exists(pid) != NULL) {
    // If client with those pid already exists
    return CLIENT_EXISTS;
  } else {
    node = last_client->next = (clients_queue*)malloc(sizeof(clients_queue));
  }

  if ((node->shared_memory_pointer = init_client_shared_memory(pid)) == NULL) {
    // If can't locate client memory segment delete node
    free(node);
    last_client->next = NULL;
    return CLIENT_SHM_ERROR;
  } else {
    // Ok
    printf("added new client with pid %d\n", pid);
    node->pid = pid;
    *(node->shared_memory_pointer) = AUTH_SUCCESS;
    last_client = node;
    node->next  = NULL;
    return CLIENT_NEW;
  }
  
}

clients_queue *client_exists(pid_t pid) {
  // Start surfing queue
  clients_queue *node = first_client;
  
  // Check if there is record with those pid
  do {
    if (node->pid == pid) {
      return node;
    } else {
      node = node->next;
    }
  } while (node != NULL);

  return NULL;
}

void listen_queue() {
  // Check if queue is empty
  if (first_client == NULL) return;

  clients_queue *node = first_client;
  // Just set everywhere _SYNC_DONE if there is _CMD_TIMER
  do {
    if (*node->shared_memory_pointer == _CMD_TIMER) {
      *node->shared_memory_pointer = _SYNC_DONE;
    }
    node = node->next;
  } while (node != NULL);
}
