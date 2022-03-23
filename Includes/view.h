#ifndef VIEW
#define VIEW

#include <cstring>
#include <list>
#include <map>
#include <vector>

class View{
 public:
    static View* Get(const std::string& type);
    virtual void Draw() = 0;
    virtual void DrawRabits(const std::vector<std::pair<int, int>>& rabits) = 0;
    virtual void CleanScreen() = 0;
    virtual void PrintSnake(const size_t length_x, const size_t length_y) = 0;
    virtual ~View() {}
    
 private:

    static View* obj;

};

#endif //VIEW