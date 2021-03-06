#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ctime>
#include <termios.h>
#include <signal.h>
#include <sys/poll.h>
#include <string>
#include <random>
#include <algorithm>
//#include <curses.h>
// /#include <conio.h>

#include "tview.h"
#include "game.h"
#include "model.h" 

//Foreground Colors
#define FOREGROUND_COL_BLACK 30
#define FOREGROUND_COL_RED 31
#define FOREGROUND_COL_GREEN 32
#define FOREGROUND_COL_YELLOW 33
#define FOREGROUND_COL_BLUE 34
#define FOREGROUND_COL_MAGENTA 35
#define FOREGROUND_COL_CYAN 36
#define FOREGROUND_COL_WHITE 37

//Background Colors
#define BACKGROUND_COL_BLACK 40
#define BACKGROUND_COL_RED 41
#define BACKGROUND_COL_GREEN 42
#define BACKGROUND_COL_YELLOW 43
#define BACKGROUND_COL_BLUE 44
#define BACKGROUND_COL_MAGENTA 45
#define BACKGROUND_COL_CYAN 46
#define BACKGROUND_COL_WHITE 47

//Strelochki
#define LEFTBUTTON ^[[D
#define UPBUTTON '^[[A'
#define RIGHTBUTTON ^[[B
#define DOWNBUTTON ^[[C

const size_t stones_quan = 20;
const size_t rabits_quan = 40;
struct termios old_term;
bool final = false;

void SignHandler(int n) {
    final = true;
}

Tview::Tview() {
    struct termios term;
    tcgetattr(0, &term);
    
    old_term = term;

    cfmakeraw(&term);
    term.c_lflag |= ISIG; 
    tcsetattr(0, TCSANOW, &term);
    signal(SIGINT, SignHandler);
}

Tview::~Tview() {
    tcsetattr(0, TCSANOW, &old_term);
    CleanScreen();
}

void Tview::CleanScreen() {
    printf("\e[H\e[J");
}

void Tview::GoCoord(int x, int y) {
    printf("\e[%d;%dH", y, x);
}

void Tview::SetColor(int color) {
    printf("\033[0;%dm", color);
}

size_t Tview::Draw(){
    
    struct winsize w;
    ioctl(1, TIOCGWINSZ, &w);

    const size_t length_x  = w.ws_col;
    const size_t length_y  = w.ws_row;

    size_t start_x = length_x / 2;
    size_t start_y = length_y / 2;

    std::map<int, int> rabits;

    for(int i = 0; i < rabits_quan; ++i) {
        std::pair<int, int> tmp = View::RandCooord(length_x, length_y);
        rabits[tmp.first] = tmp.second;
    }
    std::map<int, int> stones;
    // for(int i = 0; i < stones_quan; ++i) {
    //     std::pair<int, int> tmp = View::RandCooord(length_x, 4);
    //     stones[tmp.first] = tmp.second;
    // }

    Snake snake(start_x, start_y, length_x, length_y);
    Snake snake_2(start_x, start_y + 1, length_x, length_y);

    while(!final) {

        struct pollfd arr;
        arr.fd = 0;
        arr.events = POLLIN;

        int n = poll(&arr, 1, 500);
        if( n == 1 ) {
            
            char c;
            scanf("%c", &c);
            
            if(c == 'q') {
                break;
            } else {
                switch (c) {
                    case 's' :
                        snake.ChangeDirection(Direction::UP);
                    break;

                    case 'w' :
                        snake.ChangeDirection(Direction::DOWN);
                    break;

                    case 'a' :
                        snake.ChangeDirection(Direction::LEFT);
                    break;

                    case 'd' :
                        snake.ChangeDirection(Direction::RIGHT);
                    break;


                    case 'i':
                        snake_2.ChangeDirection(Direction::DOWN);
                    break;

                    case 'j' :
                        snake_2.ChangeDirection(Direction::LEFT);
                    break;

                    case 'k' :
                        snake_2.ChangeDirection(Direction::UP);
                    break;

                    case 'l' :
                        snake_2.ChangeDirection(Direction::RIGHT);
                    break;
                    default:
                    break;
                }
            }
        }

        if(stones.empty()) {
            for(int i = 0; i < stones_quan; ++i) {
                std::pair<int, int> tmp = View::RandCooord(length_x, length_y / 2);
                stones[tmp.first] = tmp.second;
            }
        }

        CleanScreen();
        
        DrawBoundary(length_x, length_y);
        PrintSnake(snake.snake_body, snake.GetDirection());
        //PrintSnake(snake_2.snake_body, snake_2.GetDirection());
        DrawRabits(rabits);
        final = snake.Move(stones);    
        //final = snake_2.Move();
        IsGrow(rabits, snake);
        //IsGrow(rabits, snake_2);
        DrawStones(stones, length_x, length_y);
        fflush(stdout);
        //usleep(10);
    }

    return snake.GetScore();
}

std::pair<int, int> View::RandCooord(const size_t length_x, const size_t length_y) {
    
    std::pair<int, int> rand_coord;
    
    std::random_device random_device; 
    std::mt19937 generator(random_device());

    std::uniform_int_distribution<> distribution_x(2, length_x - 2);
    std::uniform_int_distribution<> distribution_y(2, length_y - 2);

    int x = distribution_x(generator);
    int y = distribution_y(generator); 

    rand_coord.first  = x;
    rand_coord.second = y;
    
    return rand_coord;
}

void Tview::DrawRabits(const std::map<int, int>& rabits) {
    for(const auto& coord: rabits) {
        
        GoCoord(coord.first, coord.second);
        SetColor(FOREGROUND_COL_WHITE);
        printf("R");
        fflush(stdout);
                
    }
}

void Tview::PrintSnake(const std::vector<std::pair<int, int>>& snake_body, Direction dir) {
    for(int i = 0; i < snake_body.size(); ++i) {
        GoCoord(snake_body.at(i).first, snake_body.at(i).second);
        //i == 0 ? SetColor(FOREGROUND_COL_RED) : SetColor(FOREGROUND_COL_WHITE);
        if(i == 0) {
            SetColor(FOREGROUND_COL_RED);
            switch (dir)
            {
            case Direction::DOWN :
                printf("^");
                break;
            
            case Direction::LEFT :
                printf("<");
                break;
            
            case Direction::RIGHT :
                printf(">");
                break;

            case Direction::UP :
                printf("v");
                break;

            default:
                break;
            }
        } else {
            SetColor(FOREGROUND_COL_WHITE);
            printf("@");
        }
        //printf("@");
    }
}

void Tview::DrawBoundary(const size_t length_x, const size_t length_y) {
    SetColor(FOREGROUND_COL_BLUE);
    GoCoord(1, 1);
    for(int i = 1; i < length_x; ++i) {
        printf("*");
    }

    GoCoord(1, length_y);
    for(int i = 1; i < length_x; ++i) {
        printf("*");
    }
        
    for(int i = 1; i < length_y; ++i) {
        GoCoord(1, i);
        printf("*");
        GoCoord(length_x, i);
        printf("*");
    }
}

void Tview::IsGrow(std::map<int, int>& rabits, Snake& snake) {
    if(rabits.count(snake.snake_body.at(0).first) && rabits.at(snake.snake_body.at(0).first) == snake.snake_body.at(0).second) {
        auto tmp = snake.snake_body.at(0).first;
        snake.SnakeGrow(std::pair<int, int>(snake.snake_body.at(0).first, rabits.at(snake.snake_body.at(0).first)));
        rabits.erase(tmp);
        printf("\a");
    }
    //snake.GetScore();
}

void Tview::DrawStones(std::map<int, int>& stones, const size_t length_x, const size_t length_y) {
    for(auto stone : stones) {
        GoCoord(stone.first, stone.second);
        SetColor(FOREGROUND_COL_YELLOW);
        printf("#");
        ++stones.at(stone.first);
        if(stones.at(stone.first) == length_y) {
            stones.at(stone.first) = View::RandCooord(length_x, 4).second;
        }
    }
}