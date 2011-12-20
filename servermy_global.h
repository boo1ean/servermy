#ifndef SERVERMY_GLOBAL
#define SERVERMY_GLOBAL

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <linux/shm.h>
#include <malloc.h>



/* PATH SECTION */
#define LOCK_PATH              "/tmp/servermy.lock" // Path to server lockfile
#define PATH_FOR_IPC_KEY       "lol"                // Path for creaing ipc key through ftok



/* STATUS SECTION */
#define SERVER_ALREADY_RUNNING 111                  // Error code when server is already running
#define SERVER_SHM_ERROR       2                    // Something wrong with shared memory
#define SHM_CREAT_ERROR        -1                   // Shared memory creating error status
#define SHM_CREAT_SUCCESS      0                    // Shared memory creating error status
#define LOCKING_ERROR          -1                   // Locking error status
#define LOCKING_SUCCESS        0                    // Locking success status
#define AUTH_FAIL              -1                   // Fail auth
#define AUTH_SUCCESS           0                    // Success auth
#define CLIENT_EXISTS          -1                   // Client already in queue
#define CLIENT_NEW             0                    // New client was added
#define CLIENT_SHM_ERROR       1                    // Error with locating client shm



/* OTHER CONST SECTION */
#define MILI                   1000                 // 1 milisec = 1000 micro (param for usleep() function)
#define BUSY_WAIT              10                   // How often interview server status
#define AUTH_WAIT              100 * MILI           // Wait for auth response
#define CLIENT_WAIT            55 * MILI            // Client frequency
#define MAX_WAITING            100                  // Wait 100 times
#define SERVER_SYNC            500                  // Server frequency - 500 microsec (param for usleep() function)
#define MAX_SERVER_DELAY       4 * SERVER_SYNC      // Max response delay
#define SERVER_CHECK_LOCKING   10                   // How often check locking
#define SHARED_MEMORY_SIZE     (2 + sizeof(pid_t))  // Size of shared server segment
#define CLIENT_MEMORY_SIZE     1                    // Clients segment memory size
#define true                   1                    // TRUE
#define false                  0                    // FALSE



/* DISP SECTION */
#define _SERVER_STATUS        1                     // Server status byte
#define _SERVER_CMD           2                     // Server cmd byte
#define _SERVER_DATA          3                     // Server DTA



/* CMD SECTION */
#define _CMD_AUTH             1                     // Try to auth
#define _CMD_TIMER            2                     // Try to get timer sync
#define _SYNC_DONE            42                    // Status for successfully sync



/* SERVER STATUS BYTE */
#define _STATUS_BUSY          1                     // Server is busy now



// Global vars
int    lock_file;              // Lockfile descriptor
struct flock lock;             // Locking struct
char   *shared_memory_pointer; // Pointer to begining of shared memory segment
key_t  shared_memory_key;      // Key for getting access to shared memory segment
int    shared_memory_id;       // Unique shared memory segment id

char init_shared_memory_segment();           // Try to guess
char *init_client_shared_memory(pid_t pid);  // Return local pointer to client shm segment


#endif // SERVERMY_GLOBAL 
