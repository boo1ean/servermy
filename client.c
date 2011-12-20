#include </home/boo1ean/src/servermy/servermy_global.h>
#define N 10 // Size of square

char make_auth();
char send_sync_request();

char *local_memory;

// Trash
void make_some_job();

char scene[N][N];
int i,j;
void draw       ();
void fillTop    ();
void fillBottom ();
void fillLeft   ();
void fillRight  ();
void clearScene ();
int clock = 0;
int current_char = 'a';

int main(int argc, const char *argv[])
{
  // Init global server shared memory segment
  init_shared_memory_segment(); 

  // Init shared memory segment for current process's client
  if ((local_memory = init_client_shared_memory(getpid())) == NULL) {
    perror("Can't locate client memory");
    return SHM_CREAT_ERROR;
  }

  if (make_auth() == AUTH_FAIL) {
    printf("Auth error. Check if server is running.\n");
    return AUTH_FAIL;
  }
  
  clearScene();

  do {
    usleep(CLIENT_WAIT);
    if (send_sync_request() == _SYNC_DONE) {
      make_some_job();
    } else {
      printf("There is problem with server connection. Check if server is running.\n");
      return 1;
    }
  } while (1);

  return 0;
}

char make_auth() {
  int waiting = 0;
  // Wait until server become free
  while (shared_memory_pointer[_SERVER_STATUS] & _STATUS_BUSY) { 
    usleep(BUSY_WAIT);
    // Wait 100 times
    if (waiting++ == MAX_WAITING) {
      return AUTH_FAIL;
    }
  }
  
  // Send pid of current client to dta
  *((pid_t*)(shared_memory_pointer + _SERVER_DATA)) = getpid();
  // Clear local memory
  *local_memory = AUTH_FAIL;
  // Set cmd
  shared_memory_pointer[_SERVER_CMD] = _CMD_AUTH;

  // Wait for auth
  usleep(AUTH_WAIT);
  
  return *local_memory;
}

char send_sync_request() {
  int waiting = 0;
  *local_memory = _CMD_TIMER;

  // Max delay is MAX_WAITING times
  while (waiting++ < MAX_WAITING) {
    if (*local_memory == _SYNC_DONE) {
      return _SYNC_DONE;
    }
    usleep(SERVER_SYNC);
  }

  return *local_memory;
}

// It is trash
void draw() {
  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) {
      printf("%c", scene[i][j]);
    }
    printf("\n");
  }
} 

void fillTop() {
  for(i = 0; i < N; ++i) {
    scene[0][i] = current_char;
  }
}

void fillBottom() {
  for(i = 0; i < N; ++i) {
    scene[N-1][i] = current_char;
  }
}

void fillRight() {
  for(i = 0; i < N; ++i) {
    scene[i][N-1] = current_char;
  }
}

void fillLeft() {
  for(i = 0; i < N; ++i) {
    scene[i][0] = current_char;
  }
}

void clearScene() {
  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) {
      scene[i][j] = ' ';
    }
  }
}

void make_some_job() {
  system("clear");
  switch(clock++ % 4) {
    case 0: {
      fillLeft();
      break;
    }

    case 1: {
      fillTop();
      break;
    }

    case 2: {
      fillRight();
      break;
    }

    case 3: {
      clock = 0;
      fillBottom();
      break;
    }
    default: break;
  }

  current_char = (current_char == 'z') ? 'a' : current_char + 1;

  draw();
}
