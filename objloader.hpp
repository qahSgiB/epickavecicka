#include <istream>
#include <cctype>
#include <vector>
#include <filesystem>



class obj
{
    std::vector<float> indices;
    std::vector<float> vertices;

public:
    static obj from_file(std::file)
};



void eat_spaces(std::istream& is)
{
    while (!is.eof() && isspace(is.peek())) {
        is.get();
    }
}

bool is_eol(std::istream& is)
{
    return is.eof() || is.peek() == '\n';
}

void obj_load_line(std::istream& is)
{
    eat_spaces(is);

    if (is_eol(is)) {
        return;
    }
}

void obj_load(std::istream& is)
{
    while (!is.eof()) {
        obj_load_line(is);
    }
}



// bool getline_funkcny(std::istream& input, std::string& str)
// {
//     if (input.eof()) {
//         return false;
//     }

//     str.clear();

//     char c;
//     while (!input.eof()) {
//         input.get(c);
//         if (c == '\n') {
//             break;
//         }

//         str += c;
//     }

//     return true;
// }

// char eat_char(std::string_view& s)
// {
//     char c = s.front();
//     s.remove_prefix(1);
//     return c;
// }

// void eat_spaces(std::string_view& s)
// {
//     while (!s.empty() && std::isspace(s.front())) {
//         s.remove_prefix(1);
//     }
// }