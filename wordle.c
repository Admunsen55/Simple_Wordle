#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#define BL_L 10
#define BL_H 5
#define NR_WORDS 6
#define NR_LETTERS 5
#define NR_ALL_LIST 30

#define ENTER 10
#define BACKSPACE 8
#define DELETE 127

enum colors {black = 1, yellow, green, white, red};

enum user_option {open_menu = 1, delete_letter, check_word, is_letter};

enum interactiv_block_option {game_goes_on = 0, exit_option, new_game};

enum game_result {user_lost = 0, user_wants_new_game, user_won, close_game};

//structura care cuprinde o litera si culoarea casutei careia ii apartine
typedef struct letter_and_block_color {
    char value;
    int color;
} letter;

//functie care converteste un numar de tip int intr-un sir de caractere
void int_to_array (int nr, char *array)
{
    int i, size = strlen(array);
    for (i = size - 1; i >= 0; i--) {
       array[i] = (char)(nr % 10 - 0 + '0'); 
       nr = nr / 10;
    }
}

//functie care verifica daca o litera se afla intr-un cuvant
int has_letter (char letter_to_check, char* word)
{
    int i;
    for (i = 0; i < NR_LETTERS; i++) {
        if (letter_to_check == word[i]) {
            return 1;
        }
    }
    return 0;
}

/* functie care verifica daca o litera se afla pe pozitia 
exacta intr-un cuvant */
int exact_letter (char letter_to_check, int poz, char *word)
{
    if (word[poz] == letter_to_check) {
        return 1;
    }
    return 0;
}

//functie care initializeaza o litera
void init_one_letter (letter *letter_to_init)
{
    letter_to_init->value = ' ';
    letter_to_init->color = black;
}

//functie care initializeaza literele care urmeaza sa fie introduse cu ' '
void init_all_letters (letter letters_to_init[NR_WORDS][NR_LETTERS])
{
    int i, j;
    for (i = 0; i < NR_WORDS; i++) {
        for (j = 0; j < NR_LETTERS; j++) {
            init_one_letter(&letters_to_init[i][j]);
        }
    }
    return;
}

// functie care "coloreaza" o casuta (fereastra)
void fill_block (WINDOW *block, int color)
{
    int i, j;
    int block_height, block_lenght;
    //detrminam dimensiunile maxime ale casutei
    getmaxyx(block, block_height, block_lenght);
    wattron(block, COLOR_PAIR(color));
    for (i = 1; i < block_height - 1; i++) {
        for (j = 1; j < block_lenght - 1; j++) {
            mvwprintw(block, i, j, " ");
        }
    }
    wattroff(block, COLOR_PAIR(color));
    return;
}

/* functie care afiseaza literele si casutele acestora in starea
curenta (practic afiseaza interfata jocului / "randeaza un frame") */
void print_all_words (WINDOW *letter_block[NR_WORDS][NR_LETTERS], 
letter letter_to_print[NR_WORDS][NR_LETTERS])
{
    int i, j, y_cntr_block, x_cntr_block;
    /* "mvwprintw" nu suporta decat siruri de caractere pentru 
    printare, asa ca folosim urmatorul sir de caracter ca 
    "intermediar" pentru un caracter " */
    char current_letter[1];
    //determinam coordonatele centrului unei casute
    y_cntr_block = BL_H / 2;  
    x_cntr_block = BL_L / 2 - 1;  
    //parcurgem toata matricea cu litere "in casute"
    for (i = 0; i < NR_WORDS; i++) {
        for (j = 0; j < NR_LETTERS; j++) {
            //desenam margini simple (tip 0, 0) pentru casute
            box(letter_block[i][j], 0, 0);
            //"transformam" litera in majuscula cu scopuri estetice
            current_letter[0] = letter_to_print[i][j].value + 'A' - 'a';
            //umplem intai casuta cu culoarea asociata literei
            fill_block(letter_block[i][j], letter_to_print[i][j].color);
            //afisam litera din casuta
            wattron(letter_block[i][j], COLOR_PAIR(letter_to_print[i][j].color));
            mvwprintw(letter_block[i][j], y_cntr_block, x_cntr_block, current_letter);
            wattroff(letter_block[i][j], COLOR_PAIR(letter_to_print[i][j].color));
            wrefresh(letter_block[i][j]);
        }
    }
    return;
}

/* functie care face in asa fel incat casutele (ferestrele)
nu mai sunt afisate pe ecran */
void clear_all_words(WINDOW *letter_block[NR_WORDS][NR_LETTERS])
{
    int i, j;
    for (i = 0; i < NR_WORDS; i++) {
        for (j=0; j < NR_LETTERS; j++) {
            wclear(letter_block[i][j]);
            wrefresh(letter_block[i][j]);
        }
    }
    return;
}

//functie care deschide casuta ("fereastra") meniului in timpul jocului
int open_interactive_block (WINDOW *interactive_block, int user_command)
{
    int block_height, block_lenght, starting_x, starting_y, i;
    char input_key;
    //desenam margini simple
    box(interactive_block, 0, 0);
    //determinam dimensiunea maxima a unei casute
    getmaxyx(interactive_block, block_height, block_lenght);
    //tratam cazul in care utilizatorul a apasat "enter" prea devreme
    if (user_command == check_word) {
        /* determinam coordonatele necesare astfel incat textul sa fie 
        afisat in mijloc */
        starting_y = block_height / 2 - 2;
        starting_x = block_lenght / 2 - 8;
        //coloram fereastra
        fill_block(interactive_block, red);
        //afisam textul
        wattron(interactive_block, COLOR_PAIR(white));
        mvwprintw(interactive_block, starting_y, starting_x, "CUVANT INVALID !!!");
        mvwprintw(interactive_block, starting_y + 1, starting_x - 3, "PREA PUTINE LITERE !!!");
        mvwprintw(interactive_block, starting_y + 2, starting_x - 5, "SUNT NECESARE 5 LITERE !!!");
        wattroff(interactive_block, COLOR_PAIR(white));
        wrefresh(interactive_block);
        getch();
        //returnam o valoare care semnifica faptul ca jocul continua
        return game_goes_on;
    }
    /* tratam cazul in care utilizatorul doreste sa deschida meniul,
    mai exact cazul in care user_command == open_menu */
    else {
        starting_y = block_height / 2 - 1;
        starting_x = block_lenght / 2 - 10;
        //afisam "instructiunile" pentru utilizarea meniului
        fill_block(interactive_block, yellow);
        wattron(interactive_block, COLOR_PAIR(green));
        mvwprintw(interactive_block, starting_y, starting_x, "NAVIGHEAZA PRIN MENIU ");
        mvwprintw(interactive_block, starting_y + 1, starting_x - 3, "FOLOSIND TASTELE 'w' SI 's'");
        wattroff(interactive_block, COLOR_PAIR(green));
        wrefresh(interactive_block);
        getch();   
        starting_y = block_height / 2 - 3;
        starting_x = block_lenght / 2 - 8;
        /* variabila "current_option" stocheaza optiunea la care se alfa utilizatorul
        (cea care este evidentiata) */
        int color_option[3] = {0}, current_option = 0;
        //cele 3 optiuni ale meniului
        char next_option[3][20] = {"CONTINUA", "IESI DIN APLICATIE", "INCEPE UN JOC NOU"};
        //bucla infinita care "asteapta" ca utilizatorul sa selecteze o optiune
        do {
            fill_block(interactive_block, yellow);
            //afisam optiunile
            for (i = 0; i < 3; i++) {
                wattron(interactive_block, COLOR_PAIR(yellow));
                /* daca utilizatorul de afla la optiunea curenta, atunci aceasta
                este colorata in verde si evidentiala cu '>' la inceput */
                if (current_option == i) {
                    wattron(interactive_block, COLOR_PAIR(green));
                    mvwprintw(interactive_block, starting_y + 2 * i, starting_x, ">");
                }
                mvwprintw(interactive_block, starting_y + 2 * i, starting_x + 1, next_option[i]);
                wattroff(interactive_block, COLOR_PAIR(green));
            }
            wattroff(interactive_block, COLOR_PAIR(yellow));
            wrefresh(interactive_block);
            //bucla infinita care "asteapta" o tasta valida
            do {
                input_key = getch();
            }
            while (input_key != 's' && input_key != 'w' && input_key != ENTER);
            /* "mutam" optiunea curenta in functie de tasta introdusa de
            utilizator */
            if (input_key == 's') {
                /* verificam sa nu depasim limitele meniului, deoarece
                current_option poate fi doar 0, 1 sau 2 */
                if (current_option <= 1) {
                    current_option++;
                }
            }
            else if (input_key == 'w') {
                if (current_option >= 1) {
                    current_option--;
                }
            //tratam cazul in care "input_key" == ENTER
            } else {
                return current_option;
            }
        } while (1);
    }
    return game_goes_on;
}

/* functie care returneaza o "comanda" in functie de
o functie introdusa de utilizator */
int get_user_input (char *character)
{
    *character = getch();
    if (*character == ':' || *character == ';') {
        return open_menu;
    }
    if (*character == BACKSPACE || *character == DELETE) {
        return delete_letter;
    }
    if (*character == ENTER) {
        return check_word;
    }
    if (*character >= 'a' && *character <= 'z') {
        return is_letter;
    }
    return 0;
}

/*functie care interpreteaza tastele introduse de utilizator
si verifica daca acesta castiga sau nu */
int play_game (WINDOW *letter_block[NR_WORDS][NR_LETTERS],
letter letter_to_read[NR_WORDS][NR_LETTERS], WINDOW *menu_block, char* word_to_guess)
{
    int i, j, k, user_command, next_option;
    char character, word[NR_LETTERS];
    print_all_words(letter_block, letter_to_read);
    /* fiecare iteratie a for-ului dublu reprezinta procesul
    de interpretare al unei taste/litere */
    for (i = 0; i < NR_WORDS; i++) {
        /* pasul j == NR_LETTERS "asteapta" tasta "ENTER", iar toate 
        celelalte asteapta litere */
        for (j = 0; j <= NR_LETTERS; j++) {
            //asteptam ca utilizatorul sa introduca o noua tasta valida
            do {
                user_command = get_user_input(&character); 
            }
            while(!user_command);
            //interpretam tasta introdusa
            if (user_command == open_menu) {
                clear_all_words(letter_block);
                //deschidem fereastra interactiva
                next_option = open_interactive_block(menu_block, open_menu); 
                //curatam interfata interactiva de pe ecran
                wclear(menu_block);
                wrefresh(menu_block);
                print_all_words(letter_block, letter_to_read);
                if (next_option == exit_option) {
                    return close_game;
                }
                else if (next_option == new_game) {
                    return user_wants_new_game;    
                }
                //ramanem la iteratia curenta 
                j--;
                continue;   
            } else if (user_command == check_word) {
                if (j == NR_LETTERS) {
                    //"coloram" literele
                    for (k = 0; k < NR_LETTERS; k++) {
                        if (exact_letter(word[k], k, word_to_guess)) {
                            letter_to_read[i][k].color = green;
                            continue;
                        }           
                        if (has_letter(word[k], word_to_guess)) {
                            letter_to_read[i][k].color = yellow;
                            continue;
                        }
                        letter_to_read[i][k].color = black;
                    }
                    print_all_words(letter_block, letter_to_read);
                    if (strcmp(word, word_to_guess) == 0) {
                        return user_won;
                    }
                /* tratam cazul in care j != NR_LETTERS, practic cazul in
                care utilizatorul a apasat ENTER prea devreme */
                } else {
                    clear_all_words(letter_block);
                    next_option = open_interactive_block(menu_block, check_word);
                    //"if" pus de "sigurnata", nu intra in el
                    if (next_option != game_goes_on) {
                        mvprintw(0, 0, "CEVA A MERS PROST !!!");
                        refresh();
                    }
                    wclear(menu_block);
                    wrefresh(menu_block);
                    print_all_words(letter_block, letter_to_read);
                    //ramanem la iteratia curenta
                    j--;
                }
                continue;
            } else if (user_command == delete_letter) {
                /* cazul in care utilizatorul doreste sa stearga o literea */
                if (j >= 1) {
                    //mergem la iteratia precedenta
                    j--;
                    init_one_letter(&letter_to_read[i][j]);
                    print_all_words(letter_block, letter_to_read);
                    j--;
                }
                //cazul in care nu exista nicio litera de sters
                else {
                    //ramanem la iteratia curenta
                    j--;
                }
                continue;
            } else {
            //urmeaza cazul in care a fost introdusa o litera
            if (j == NR_LETTERS) {
                j--;
                continue;
            }
            letter_to_read[i][j].value = character;
            word[j] = character;
            print_all_words(letter_block, letter_to_read);
            }
        }
    }
    //daca se iese din cele doua foruri, utilizatorul a pierdut
    return user_lost;
}

/* functie care deschide casuta ("fereastra") meniului de la finalul jocului,
asemenatoare cu una dintre functiile precedente (de aceea nu e descrisa in
amanunt) */
int open_final_block (WINDOW *final_block, int option, char* word_to_guess)
{
    int block_height, block_lenght, starting_x, starting_y, input_key, i;
    getmaxyx(final_block, block_height, block_lenght);
    if (option == user_lost) {
        box(final_block, 0, 0);
        starting_y = block_height / 2 - 2;
        starting_x = block_lenght / 2 - 7;
        fill_block(final_block, red);
        wattron(final_block, COLOR_PAIR(white));
        //afisam mesajul pentru joc pierdut
        mvwprintw(final_block, starting_y, starting_x, "AI PIERDUT :((");
        mvwprintw(final_block, starting_y + 1, starting_x - 4, "CUVANTUL CARE TREBUIA");
        mvwprintw(final_block, starting_y + 2, starting_x - 1, "GHICIT ESTE:");
        wattroff(final_block, COLOR_PAIR(white));
        wattron(final_block, COLOR_PAIR(green));
        mvwprintw(final_block, starting_y + 2, starting_x - 1 + 12, word_to_guess);
        wattroff(final_block, COLOR_PAIR(green));
        wrefresh(final_block);
        getch();   
        return close_game;
    }
    else if (option == user_won) {
        /* determinam coordonatele astfel incat textul sa fie 
        centrat */
        starting_y = block_height / 2 - 2;
        starting_x = block_lenght / 2 - 11;
        box(final_block, 0, 0);
        fill_block(final_block, yellow);
        wattron(final_block, COLOR_PAIR(green));
        mvwprintw(final_block, starting_y, starting_x, "FELICITARI! AI CASTIGAT");
        wattroff(final_block, COLOR_PAIR(green));
        wattron(final_block, COLOR_PAIR(white));
        //afisam instructiunile de utilizare ale meniului
        mvwprintw(final_block, starting_y + 1, starting_x + 1, "NAVIGHEAZA PRIN MENIU ");
        mvwprintw(final_block, starting_y + 2, starting_x - 2, "FOLOSIND TASTELE 'w' SI 's'");
        wattroff(final_block, COLOR_PAIR(white));
        wrefresh(final_block);
        getch();    
        //afisam meniul
        starting_y = block_height / 2 - 1;
        starting_x = block_lenght / 2 - 8;
        int color_option[2] = {0}, current_option = 0;
        char next_option[2][20] = {"IESI DIN APLICATIE", "INCEPE UN JOC NOU"};
        /* bucla infinita care asteapta ca utilizatorul sa aleaga o 
        optiune */
        do {
            fill_block(final_block, yellow);
            for (i = 0; i < 2; i++) {
                wattron(final_block, COLOR_PAIR(yellow));
                if (current_option == i) {
                    wattron(final_block, COLOR_PAIR(green));
                    mvwprintw(final_block, starting_y + 2 * i, starting_x, ">");
                }
                mvwprintw(final_block, starting_y + 2 * i, starting_x + 1, next_option[i]);
                wattroff(final_block, COLOR_PAIR(green));
            }
            wattroff(final_block, COLOR_PAIR(yellow));
            wrefresh(final_block);
            do {
                input_key = getch();
            }
            while (input_key != 'w' && input_key != 's' && input_key != ENTER);
            if (input_key == 'w') {
                if (current_option == 1) {
                    current_option--;
                }
            }
            else if (input_key == 's') {
                if (current_option == 0) {
                    current_option++;
                }
            //cazul in care a fost apasat ENTER
            } else {
                if (current_option == 0) {
                    return close_game;
                }
                else {
                    return new_game;
                }
            }
        } while (1);
    }
    return close_game;
}

// functie care afiseaza casuta (fereastra) cu numele jocului 
void open_name_block (WINDOW * name_block)
{
    box(name_block, '*', '*');
    fill_block(name_block, white);
    wattron(name_block, COLOR_PAIR(yellow));
    mvwprintw(name_block, 2, 3, "JOC WORDLE DE");
    mvwprintw(name_block, 4, 3, "ANDRONE ANDREI");
    wattroff(name_block, COLOR_PAIR(yellow));
    wrefresh(name_block);
    return;
}

//functie care afiseaza fereastra cu scorul (functionalitate "extra")
void open_score_block (WINDOW * score_block, int current_score)
{
    char score_array[5] = "0000";
    if (current_score <= 9998) {
        int_to_array(current_score, score_array);
    } else {
        strcpy(score_array, "9999");
    }
    box(score_block, '*', '*');
    fill_block(score_block, white);
    wattron(score_block, COLOR_PAIR(yellow));
    mvwprintw(score_block, 3, 5, "SCOR:");
    mvwprintw(score_block, 3, 11, score_array);
    wattroff(score_block, COLOR_PAIR(yellow));
    wrefresh(score_block);
    return;
}

int main(int argc, char **argv)
{
    int i, j, x_max, y_max, x_start, y_start;
    int blocks_length, blocks_height;
    int next_option, final_option;
    unsigned int current_score = 0;
    /* declaram matricea care stocheaza datele legate de literele introduse
    de utilizator */
    letter user_letters[NR_WORDS][NR_LETTERS];
    char all_possible_words[NR_ALL_LIST][NR_LETTERS + 1] = {"arici", "atent" , "baiat", 
    "ceata", "debut", "peste", "fixat", "hamac", "harta", "jalon", "jucam", "lacat",
    "magie", "nufar", "oaste", "perus", "rigle", "roman", "sanie", "scris", "sonda", 
    "texte", "tipar", "titan", "zebra", "vapor", "vatra", "copil", "carte", "pahar"};
    char word_to_guess[NR_LETTERS];
    //initializam ecranul
    initscr();
    //ascudem afisajul tastelor introduse de utilizator
    noecho();
    cbreak();
    //ascudem cursorul
    curs_set(0);
    //aflam dimensiunea totala ecranului
    getmaxyx(stdscr, y_max, x_max);
    //determinam inaltimea totala a tuturor casutelor cu litere
    blocks_height = NR_WORDS * (BL_H + 1);
    /* determinam coordonata y de inceput astfel incat toate 
    casutele cu litere sa fie centrate */
    if (blocks_height / 2 > y_max / 2) {
        y_start = 0;
    }
    else {
        y_start = y_max / 2 - blocks_height / 2;   
    }
    //determinam lungimea totala a tuturor casutelor cu litere
    blocks_length = NR_LETTERS * (BL_L + 1);
    /* determinam coordonata x de inceput astfel incat toate 
    casutele cu litere sa fie centrate */
    if (blocks_length / 2 > x_max / 2) {
        x_start = 0;
    }
    else {
        x_start = x_max / 2 - blocks_length / 2;   
    }
    //initializam casutele (ferestrele) in care vom afisa literele cuvintelor
    WINDOW *letter_block[NR_WORDS][NR_LETTERS];
    for (i = 0; i < NR_WORDS; i++) {
        for (j = 0; j < NR_LETTERS; j++) {
            letter_block[i][j] = newwin(BL_H, BL_L, y_start + (BL_H + 1) * i, 
            x_start + (BL_L + 1) * j);
        }
    }
    //initializam casuta (fereastra) in care vom afisa numele jocului
    WINDOW *name_box;
    blocks_height = 7;
    y_start = y_max / 2 - blocks_height / 2;
    blocks_length = 20;
    x_start = x_start / 2 - blocks_length / 2;
    name_box = newwin(blocks_height, blocks_length, y_start, x_start);
    refresh();
    //initializam casuta (fereastra) in care vom afisa scorul
    WINDOW *score_box;
    x_start = x_max - x_start - blocks_length;
    score_box = newwin(blocks_height, blocks_length, y_start, x_start);
    refresh();
    //initializam casuta (fereastra) destinata mesajelor pentru utilizator;
    WINDOW *menu_box;
    blocks_height = 15;
    if (blocks_height / 2 > y_max / 2) {
        y_start = 0;
    }
    else {
        y_start = y_max / 2 - blocks_height / 2;   
    }
    blocks_length = 30;
    if (blocks_length / 2 > x_max / 2) {
        x_start = 0;
    }
    else {
        x_start = x_max / 2 - blocks_length / 2;   
    }
    menu_box = newwin(blocks_height, blocks_length, y_start, x_start);
    //initializam culorile pentru litere
    start_color();
    init_pair(black, COLOR_WHITE, COLOR_BLACK);
    init_pair(yellow, COLOR_BLACK, COLOR_YELLOW);
    init_pair(green, COLOR_BLACK, COLOR_GREEN);
    init_pair(white, COLOR_BLACK, COLOR_WHITE);
    init_pair(red, COLOR_CYAN, COLOR_RED);
    //afisam casuta destinata numelui jocului
    open_name_block(name_box);
    //afisam casuta cu scorul
    open_score_block(score_box, current_score);
    //generam un "seed" aleaotoriu pentru functia "rand"
    srand(time(0));
    /* bucla infinita care se opreste doar daca utilizatorul pierde
    un joc sau decide sa iasa din joc */
    while(1) {
        //generam un cuvant de gasit total aleotoriu (din lista)
        strcpy(word_to_guess, all_possible_words[rand() % NR_ALL_LIST]);
        init_all_letters(user_letters);
        next_option = play_game(letter_block, user_letters, menu_box, word_to_guess);
        clear_all_words(letter_block);
        //interpretam urmatoarea etapa a jocului
        if (next_option == close_game) {
            endwin();
            return 0;
        }
        if (next_option == user_lost) {
            final_option = open_final_block(menu_box, user_lost, word_to_guess);
            if (final_option == close_game) {
                endwin();
                return 0;
            }
        }
        else if (next_option == user_wants_new_game) {
            continue;
        }
        /* tratam cazul in care utilizatorul a castigat, practic
        cazul in care next_option == user_won */
        else {
            //crestem scorul
            current_score++;
            //afisam dinnou scorul
            open_score_block(score_box, current_score);
            clear_all_words(letter_block);
            final_option = open_final_block(menu_box, user_won, word_to_guess);
            if (final_option == close_game) {
                endwin();
                return 0;
            }
            else {
                wclear(menu_box);
                wrefresh(menu_box);
                continue;
            }
        }
        refresh();
    }
    clear();
    endwin();
    return 0;
} 
