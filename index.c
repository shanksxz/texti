#include <asm-generic/errno-base.h>
#include <errno.h>
// #include <ctype.h>
#include <stdio.h>7677
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

struct termios orig_termios;


#define CTRL_KEY(k) ((k) & 0x1f) // &'s with 00011111

void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J" ,4);
    write(STDOUT_FILENO, "\x1v[H" ,3);
    perror(s);
    exit(1);
}


void disableRawMode() {
    if(tcsetattr(STDERR_FILENO, TCIFLUSH, &orig_termios) == -1)
    die("tcseattr");
}


void enableRawMode() {

    if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | ICRNL | INPCK);
    raw.c_iflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcseattr");
}

char editorReadKey() {
    int nread;
    char c;
    while((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if(nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}


// input
void editorProcessKeypress() {
    char c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "x\1b[2J",4);
            write(STDOUT_FILENO, "\x1b[H", 3);

            exit(0);
            break;
    }
}

void editorRefreshScreen() {
    write(STDOUT_FILENO, "x\1b[2J",4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

int main() {
    enableRawMode();

    // char c;
    // while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
    //     if(iscntrl(c)) {
    //         printf("%d\r\n",c);
    //     } else {
    //         printf("%d ('%c')\r\n", c ,c );
    //     }
    // }

    // read(input ,pointer to the var where the data will be store, count(no. of bytes to read)) -> system call

    // STDIN_FILENO -> for taking input from the terminal

    // while(1) {
    //     char c = '\0';
    //     if(read(STDIN_FILENO, &c , 1) == -1 && errno != EAGAIN);
    //     if(iscntrl(c)) {
    //         printf("%d\r\n", c);
    //     } else {
    //         printf("%d ('%c)\r\n", c, c);
    //     }
    //     if(c == CTRL_KEY('q')) break;
    // }

    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }

    return 0;
}