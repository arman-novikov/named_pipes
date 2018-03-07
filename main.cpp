#include <iostream>
#include <thread>
#include <atomic>

#include <sys/stat.h> // for mkfifo()
#include <fcntl.h> //for open(), O_XXXX, S_XXXX
#include <string.h> //for memset
#include <stdio.h>
#include <unistd.h> // close(), read(), write()
#include <signal.h>

void sigint_poc(int sig);
int reciever();
int transmitter();

const char           NAMEDPIPE_TX[] = {"/tmp/t_pipe"};
const char           NAMEDPIPE_RX[] = {"/tmp/r_pipe"};
const unsigned short BUFSIZE          = 50;

int file_descr_TX, file_descr_RX;
volatile bool flag = true;

using namespace std;

int main(int argc, char* argv[])
{
    int num;
    thread R(reciever), T(transmitter);

    R.detach();
    T.detach();

    sleep(1);

    cout<<"\ninput any num: ";
    cin>>num;
    flag = false;

    //signal(SIGINT, sigint_poc);
    return 0;
}

void sigint_poc(int sig)
{
    close(file_descr_RX);
    remove(NAMEDPIPE_RX);

    close(file_descr_TX);
    remove(NAMEDPIPE_TX);

    std::cout<<"\ncleaned\n";
    exit(sig);
}

int reciever()
{
    char buffer[BUFSIZE];
    int res;

    res = mkfifo(NAMEDPIPE_RX, 0777);
    if (res) {
        perror("mkfifo");
        //return res;
    }
    printf("\n%s is created\n", NAMEDPIPE_RX);


    file_descr_RX = open(NAMEDPIPE_RX, O_RDONLY);//, S_IWOTH | S_IRUSR); //O_WRONLY, O_RDWR, O_RDONLY
    if (file_descr_RX <= 0) {
        perror("open");
        return file_descr_RX;
    }
    printf("\n%s is opened\n", NAMEDPIPE_RX);

    do {
        int len;

        if(len = read(file_descr_RX, buffer, BUFSIZE) < 0) {
            perror("read");
            //close(file_descr_RX);
            //remove(NAMEDPIPE_RX);
            //return len;
        }

        if (*buffer) {
            printf("\nincoming msg(%d): %s\n", len, buffer);
            memset(buffer, 0, BUFSIZE);
        }

    } while (flag);

    std::cout<<"\nreciever is out\n";

    return 0;

}

int transmitter()
{
    char buffer[] = {"Hi! I am here and there!"};

    file_descr_TX = open(NAMEDPIPE_TX, O_WRONLY); // | O_SYNC
    if (file_descr_TX <= 0) {
        perror("open");
        return file_descr_TX;
    }
    printf("\n%s is opened\n", NAMEDPIPE_RX);

    do {
        int len;

        len = write(file_descr_TX, buffer,strlen(buffer));
        if (len <= 0) {
            perror("write");
            //close(file_descr_TX);
            //remove(NAMEDPIPE_TX);
            //return len;
        }
        fdatasync(file_descr_TX);
        sleep(2);
    } while(flag);

    return 0;
}
