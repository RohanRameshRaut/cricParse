#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define ubufsize 256
#define uwheight 5
#define uwwidth 64

int main(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(1);

    int my, mx;
    getmaxyx(stdscr, my, mx);

    WINDOW *mainwin = newwin(my, mx, 0, 0);
    box(mainwin, 0, 0);
    mvwprintw(mainwin, 1, 2, "Main Window");
    mvwprintw(mainwin, 2, 2, "Press 'j' to enter text, 'Esc' to exit");
    wrefresh(mainwin);

    // pointer for new window and user umsg
    // you can use this method to create multiple windows and manage them
    // e.g. nwin0, nwin1 etc
    WINDOW *nwin0 = NULL;
    char umsg[ubufsize] = "";
    int running = 1;

    while (running) {
        int ch = wgetch(mainwin);

        if (ch == 27) { // code for the Esc key
            running = 0;
        } 
        else if (ch == 'j' || ch == 'J') {
            int uwinx = (mx - uwwidth) / 2;
            int uwiny = (my - uwheight) / 2;

            // Create a new window for input
            nwin0 = newwin(uwheight, uwwidth, 
                              uwiny, uwinx);
            box(nwin0, 0, 0);
            mvwprintw(nwin0, 1, 2, "Enter text (press Enter to close this window):");
            wrefresh(nwin0);

            wmove(nwin0, 2, 2);
            wrefresh(nwin0);

            // Enable echo for input window
            echo();
            curs_set(2);

            // Get user input
            char umbuf[ubufsize];
            memset(umbuf, 0, ubufsize);
            wgetnstr(nwin0, umbuf, ubufsize - 1);

            // Disable echo to not show the keys pressed by the user
            noecho();
            curs_set(1);

            // save the user input string to another location
            strncpy(umsg, umbuf, ubufsize - 1);
            umsg[ubufsize - 1] = '\0'; // why???

            // destroy the input window now
            delwin(nwin0);
            nwin0 = NULL;

            // display the user msg in the main window
            wclear(mainwin);
            box(mainwin, 0, 0);
            mvwprintw(mainwin, 1, 2, "Main Window");
            mvwprintw(mainwin, 2, 2, "Press 'j' to enter text, 'Esc' to exit");

            if (strlen(umsg) > 0) {
                mvwprintw(mainwin, 4, 2, "Your message to the world: %s", umsg);
            }

            wrefresh(mainwin);
        }
    }

    if (nwin0 != NULL) {
        delwin(nwin0);
    }
    delwin(mainwin);
    endwin();

    return 0;
}

