#ifndef U2_COMMON_H
#define U2_COMMON_H

#pragma pack(1)

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <memory.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <GL/gl.h>
#include <GL/glut.h>

#define MAX_BUFFERS 10
#define LOGFILE "/tmp/example.log"
#define SEM_MUTEX_NAME "/sem-mutex"
#define SEM_BUFFER_COUNT_NAME "/sem-buffer-count"
#define SEM_SPOOL_SIGNAL_NAME "/sem-spool-signal"
#define SHARED_MEM_NAME "/posix-shared-mem-example"

struct shared_memory {
    char buf[MAX_BUFFERS][256];
    int buffer_index;
    int buffer_print_index;
};
const float indentXStringScale = 40.0;
const float radius1 = 500.0;
const float radius2 = 250.0;
const float radius3 = 125.0;
const int countSegments = 90;

const char *radius1String = "500";
const char *radius2String = "250";
const char *radius3String = "125";
const int SIZE_BUFFER = 2;
const int SIZE_BUFFER_TEXT = 256;
const int LEN_TEXT_OTKAZ = 16;

const uint32_t TYPE_MESSAGE_AZIMUTH = 1;
const uint32_t TYPE_MESSAGE_DISTANCE = 2;
const uint32_t TYPE_MESSAGE_SCALE = 3;
const uint32_t TYPE_MESSAGE_CONFIRMATION = 4;
static const float CMR_AZIMUTH = 90.0 / pow(2.0, 14.0);
static const float CMR_DISTANCE = 512.0 / pow(2.0, 11.0);
const int WIDTH_WINDOW = 1200;
const int HEIGHT_WINDOW = 1000;


typedef struct {
    uint32_t type;
} Header;


typedef struct {
    Header header;
    uint32_t id: 8;
    uint32_t : 5;
    uint32_t azimuth: 15;
    uint32_t sign: 1;
    uint32_t: 2;
    uint32_t isNew: 1;
} StructAzimuth;

typedef struct {
    Header header;
    uint32_t id: 8;
    uint32_t : 8;
    uint32_t distance: 12;
    uint32_t : 3;
    uint32_t isNew: 1;
} StructDistance;

typedef struct {
    Header header;
    uint32_t id: 8;
    uint32_t : 9;
    uint32_t scale: 12;
    uint32_t : 2;
    uint32_t isNew: 1;
} StructScale;

typedef struct {
    Header header;
    uint32_t typeConfirmation: 8;
    uint32_t idMessage: 24;
} StructConfirmation;

void error(char *msg) {
    perror(msg);
    exit(1);
}

#endif //U2_COMMON_H
