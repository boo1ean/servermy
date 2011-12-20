#include </home/boo1ean/src/servermy/servermy_global.h>

char init_shared_memory_segment() {
  // Generate ipc key
  shared_memory_key = ftok(PATH_FOR_IPC_KEY); 

  // Init share memory segment
  // Use or between creat and excl flags to have possibility check error
  shared_memory_id = shmget(shared_memory_key, SHARED_MEMORY_SIZE, IPC_CREAT);

  // Check if there are any errors with creation of shared memory segment
  if (shared_memory_id == -1) {
    perror("Shared memory creation error");
    return SHM_CREAT_ERROR;
  }

  // Check if there are any errors with placement of shared memory segment
  if ((shared_memory_pointer = (char*)shmat(shared_memory_id, 0, 0)) == (char*)(-1)) {
    perror("Shared memory placement error");
    return SHM_CREAT_ERROR;
  }

  return SHM_CREAT_SUCCESS;
}

char *init_client_shared_memory(pid_t pid) {
  // String for generating ipc key
  char *path_for_key = (char*)malloc(100);
  char *shared_memory_pointer;

  // Convert pid to string 
  sprintf(path_for_key, "%d", pid);

  // Generate ipc key
  key_t shared_memory_key = ftok(path_for_key); 

  // Init share memory segment
  // Use or between creat and excl flags to have possibility check error
  int shared_memory_id = shmget(shared_memory_key, SHARED_MEMORY_SIZE, IPC_CREAT);

  // Check if there are any errors with creation of shared memory segment
  if (shared_memory_id == -1) {
    perror("Shared memory creation error");
    return NULL;
  }

  // Check if there are any errors with placement of shared memory segment
  if ((shared_memory_pointer = (char*)shmat(shared_memory_id, 0, 0)) == (char*)(-1)) {
    perror("Shared memory placement error");
    return NULL;
  }

  free(path_for_key);

  return shared_memory_pointer;
}
