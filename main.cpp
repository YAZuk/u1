#include "common.h"
#include "utils.h"


static float xCurrent = 0.0;
static float yCurrent = 0.0;
static float centerX = 0.0;
static float centerY = 0.0;
static int socketSend = 0;
static uint32_t countMessageAzimuth = 0;
static uint32_t countMessageDistance = 0;
static struct sockaddr_in addressSend;
static time_t timeSendAzimuth;
static time_t timeReceiveAzimuth;
char textOtkaz[LEN_TEXT_OTKAZ];

void display() {

    char buf[SIZE_BUFFER_TEXT];
    centerX = glutGet(GLUT_WINDOW_WIDTH) / 2.0;
    centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2.0;

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(5);
    glLineWidth(2);

    glColor4f(1.0, 0.0, 0.0, 0.0);
    drawAirplane(xCurrent, yCurrent);

    glColor4f(0.65, 0.62, 0.62, 0.0);
    drawCircle(centerX, centerY, radius1, countSegments);
    drawCircle(centerX, centerY, radius2, countSegments);
    drawCircle(centerX, centerY, radius3, countSegments);


    snprintf(buf, SIZE_BUFFER_TEXT, "x:%.1f y:%.1f", xCurrent, yCurrent);
    renderBitmapString(5, 90, 0, GLUT_BITMAP_HELVETICA_18, buf);
    snprintf(buf, SIZE_BUFFER_TEXT, "Distance:%.1f",
             sqrt(pow((centerX - xCurrent), 2) + (pow((centerY - yCurrent), 2))));
    renderBitmapString(5, 60, 0, GLUT_BITMAP_HELVETICA_18, buf);
    float azimuth = atan2((yCurrent - centerY), (xCurrent - centerX)) / M_PI * 180.0;
//    azimuth = (azimuth < 0.0) ? azimuth + 360.0 : azimuth;
    snprintf(buf, SIZE_BUFFER_TEXT, "Azimuth:%.5f", azimuth);
    renderBitmapString(5, 30, 0, GLUT_BITMAP_HELVETICA_18, buf);
    renderBitmapString(1100, 950, 0, GLUT_BITMAP_HELVETICA_18, textOtkaz);

    renderBitmapString(centerX - radius1 - indentXStringScale, centerY, 0, GLUT_BITMAP_HELVETICA_18, radius1String);
    renderBitmapString(centerX - radius2 - indentXStringScale, centerY, 0, GLUT_BITMAP_HELVETICA_18, radius2String);
    renderBitmapString(centerX - radius3 - indentXStringScale, centerY, 0, GLUT_BITMAP_HELVETICA_18, radius3String);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


int sendMessages() {
    fd_set setWrite;
    ssize_t countBytes = 0;
    time_t currentTime;

    StructAzimuth messageAzimuth;
    StructDistance messageDistance;

    time_t differenceBetweenSendingTime = abs(timeSendAzimuth - timeReceiveAzimuth);
    time(&currentTime);
    time_t differenceTime = abs(currentTime - timeSendAzimuth);


    /***
     * Отказ
     *
    ***/
    if ((differenceBetweenSendingTime >= 1) and (differenceTime <= 5)){
        fprintf(stdout, "Otkaz U1\n");
        snprintf(textOtkaz, LEN_TEXT_OTKAZ , "Otkaz U1");
        return -1;
    }


    /***
     * после отказа снова попытка отправить с квитанцией
     * ***/
    if (differenceTime > 5) {
        time(&timeSendAzimuth);
        time(&timeReceiveAzimuth);
        snprintf(textOtkaz, LEN_TEXT_OTKAZ , " ");
    }

    memset(&messageAzimuth, 0, sizeof(StructAzimuth));
    memset(&messageDistance, 0, sizeof(StructDistance));

    messageAzimuth.header.type = TYPE_MESSAGE_AZIMUTH;
    messageDistance.header.type = TYPE_MESSAGE_DISTANCE;

    countMessageAzimuth++;
    countMessageDistance++;

    messageAzimuth.id = countMessageAzimuth;
    messageDistance.id = countMessageDistance;


    messageAzimuth.isNew = 1;
    messageDistance.isNew = 1;


    float angle = atan2((yCurrent - centerY), (xCurrent - centerX)) / M_PI * 180.0;
    if (angle < 0.0)
        messageAzimuth.sign = 1;
    messageAzimuth.azimuth = angle / CMR_AZIMUTH;
    messageDistance.distance = sqrt(pow((centerX - xCurrent), 2) + (pow((centerY - yCurrent), 2))) / CMR_DISTANCE;

    FD_ZERO(&setWrite);
    FD_SET(socketSend, &setWrite);
    if (select(socketSend + 1, NULL, &setWrite, NULL, NULL) != -1) {
        if (FD_ISSET(socketSend, &setWrite)) {
            for (int i = 0; i < SIZE_BUFFER; i++)
                *((uint32_t * ) & messageAzimuth + i) = htonl(*((uint32_t * ) & messageAzimuth + i));

            for (int i = 0; i < SIZE_BUFFER; i++)
                *((uint32_t * ) & messageDistance + i) = htonl(*((uint32_t * ) & messageDistance + i));

        } else return -1;
    } else return -1;


    if ((countBytes = sendto(socketSend, &messageAzimuth, sizeof(StructAzimuth), 0,
                             (struct sockaddr *) &addressSend,
                             sizeof(struct sockaddr_in))) == -1)
        fprintf(stdout, "Error send message azimuth");
    else {
        time(&timeSendAzimuth);
        fprintf(stdout, "Send aizmuth message-->>%ld\n", countBytes);
    }


    if ((countBytes = sendto(socketSend, &messageDistance, sizeof(StructDistance), 0,
                             (struct sockaddr *) &addressSend,
                             sizeof(struct sockaddr_in))) == -1)
        fprintf(stdout, "Error send message distance");
    else
        fprintf(stdout, "Send distance message-->>%ld\n", countBytes);


    return 0;

}

void keyboardEvent(unsigned char key, int a, int b) {
    

    fprintf(stdout, "Hello World\n");
    switch (key) {
        case GLUT_KEY_UP:
            yCurrent += 1.0;
            glutPostRedisplay();
            sendMessages();
            break;
        case GLUT_KEY_DOWN:
            yCurrent -= 1.0;
            glutPostRedisplay();
            sendMessages();
            break;
        case GLUT_KEY_LEFT:
            xCurrent -= 1.0;
            glutPostRedisplay();
            sendMessages();
            break;
        case GLUT_KEY_RIGHT:
            xCurrent += 1.0;
            glutPostRedisplay();
            sendMessages();
            break;
        default:
            break;
    }
}

void *receiveThread(void *arg) {
    int *sock = (int *) arg;
    fd_set setReceive;
    struct sockaddr_in addressClient;
    socklen_t len = 0;
    ssize_t countBytes = 0;
    uint32_t buffer[SIZE_BUFFER];
    for (;;) {
        FD_ZERO(&setReceive);
        FD_SET(*sock, &setReceive);
        if (select(*sock + 1, &setReceive, NULL, NULL, NULL) == -1) {
            fprintf(stdout, "Error select receive\n");
            continue;
        }

        if (!FD_ISSET(*sock, &setReceive))
            continue;
        if ((countBytes = recvfrom(*sock, buffer,
                                   SIZE_BUFFER * sizeof(unsigned long), 0,
                                   (struct sockaddr *) &addressClient, &len)) == -1)
            continue;

        for (int i = 0; i < SIZE_BUFFER; i++)
            buffer[i] = ntohl(buffer[i]);

        Header *header = (Header *) &buffer;
        switch (header->type) {
            case TYPE_MESSAGE_CONFIRMATION: {
                StructConfirmation *confirmation = (StructConfirmation *) &buffer;
                fprintf(stdout, "Confirmation:%u %u\n", confirmation->idMessage,
                        confirmation->typeConfirmation);

                time(&timeReceiveAzimuth);
                break;
            }
            default:
                break;

        }// switch
    }//for(;;)
    return NULL;
}


int main(int argc, char **argv) {

    int errorThread = 0;
    int portReceive = 25000;
    int portSend = 25001;
    int on = 1;
    int socketReceive = 0;

    struct sockaddr_in addressReceive;
    snprintf(textOtkaz, LEN_TEXT_OTKAZ , " ");

    pthread_t threadReceive = 0;

    memset(&addressReceive, 0, sizeof(struct sockaddr_in));
    memset(&addressSend, 0, sizeof(struct sockaddr_in));

    addressReceive.sin_family = AF_INET;
    addressReceive.sin_port = htons(portReceive);

    addressSend.sin_family = AF_INET;
    addressSend.sin_port = htons(portSend);
    addressSend.sin_addr.s_addr = htonl(inet_network("127.0.0.1"));

    if ((socketReceive = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        fprintf(stdout, "Error socket receive\n");
        return 0;
    }

    if ((socketSend = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        fprintf(stdout, "Error socket send\n");
        return 0;
    }


    if (setsockopt(socketReceive, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        fprintf(stdout, "Error setsockopt socket receive\n");
        return 0;
    }

    if (bind(socketReceive, (struct sockaddr *) &addressReceive, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stdout, "Error bind socket receive\n");
        return 0;
    }


    if ((errorThread = pthread_create(&threadReceive, NULL, receiveThread, &socketReceive)) != 0) {
        fprintf(stdout, "Error pthread_create\n");
        return 0;
    }


    // struct shared_memory *shared_mem_ptr;
    // sem_t *mutex_sem, *buffer_count_sem, *spool_signal_sem;
    // int fd_shm, fd_log;
    // char mybuf [256];

    // // Open log file
    // if ((fd_log = open (LOGFILE, O_CREAT | O_WRONLY | O_APPEND | O_SYNC, 0666)) == -1)
    //     error ("fopen");

    // //  mutual exclusion semaphore, mutex_sem with an initial value 0.
    // if ((mutex_sem = sem_open (SEM_MUTEX_NAME, O_CREAT, 0660, 0)) == SEM_FAILED)
    //     error ("sem_open");

    // // Get shared memory
    // if ((fd_shm = shm_open (SHARED_MEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0660)) == -1)
    //     error ("shm_open");

    // if (ftruncate (fd_shm, sizeof (struct shared_memory)) == -1)
    //     error ("ftruncate");

    // if ((shared_mem_ptr = mmap (NULL, sizeof (struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED,
    //                             fd_shm, 0)) == MAP_FAILED)
    //     error ("mmap");
    // // Initialize the shared memory
    // shared_mem_ptr -> buffer_index = shared_mem_ptr -> buffer_print_index = 0;

    // // counting semaphore, indicating the number of available buffers. Initial value = MAX_BUFFERS
    // if ((buffer_count_sem = sem_open (SEM_BUFFER_COUNT_NAME, O_CREAT | O_EXCL, 0660, MAX_BUFFERS)) == SEM_FAILED)
    //     error ("sem_open");

    // // counting semaphore, indicating the number of strings to be printed. Initial value = 0
    // if ((spool_signal_sem = sem_open (SEM_SPOOL_SIGNAL_NAME, O_CREAT | O_EXCL, 0660, 0)) == SEM_FAILED)
    //     error ("sem_open");

    // // Initialization complete; now we can set mutex semaphore as 1 to
    // // indicate shared memory segment is available
    // if (sem_post (mutex_sem) == -1)
    //     error ("sem_post: mutex_sem");

    // while (1) {  // forever
    //     // Is there a string to print? P (spool_signal_sem);
    //     if (sem_wait (spool_signal_sem) == -1)
    //         error ("sem_wait: spool_signal_sem");

    //     strcpy (mybuf, shared_mem_ptr -> buf [shared_mem_ptr -> buffer_print_index]);

    //     /* Since there is only one process (the logger) using the
    //        buffer_print_index, mutex semaphore is not necessary */
    //     (shared_mem_ptr -> buffer_print_index)++;
    //     if (shared_mem_ptr -> buffer_print_index == MAX_BUFFERS)
    //         shared_mem_ptr -> buffer_print_index = 0;

    //     /* Contents of one buffer has been printed.
    //        One more buffer is available for use by producers.
    //        Release buffer: V (buffer_count_sem);  */
    //     if (sem_post (buffer_count_sem) == -1)
    //         error ("sem_post: buffer_count_sem");

    //     // write the string to file
    //     if (write (fd_log, mybuf, strlen (mybuf)) != strlen (mybuf))
    //         error ("write: logfile");
    // }


    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowSize(WIDTH_WINDOW, HEIGHT_WINDOW);
    glutCreateWindow("U2");

    glutKeyboardFunc(keyboardEvent);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    xCurrent = glutGet(GLUT_WINDOW_WIDTH) / 2.0;
    yCurrent = glutGet(GLUT_WINDOW_HEIGHT) / 2.0;


    time(&timeReceiveAzimuth);
    time(&timeSendAzimuth);

    glutMainLoop();
    return 0;
}
