#include <iostream>
#include <fstream>
#include <iomanip>
#include <stack>
#include <vector>
#include <cmath>

using std::string;

namespace calc {
    using std::endl;
    using stack = std::stack<string>;

    void execute(stack &calc_stack, std::istream &in, string s);
    void operate(stack &calc_stack, const string op);
    void sqrt(stack &calc_stack);
    void repeat(stack &calc_stack, std::istream &in);
    void reverse(stack &calc_stack);
    inline bool isDouble(string s);
    inline bool isNumber(string s);
    string dtos(double val);

    namespace cmds {
        const string add = "add";
        const string sub = "sub";
        const string mult = "mult";
        const string div = "div";
        const string sqrt = "sqrt";
        const string pop = "pop";
        const string reverse = "reverse";
        const string repeat = "repeat";
        const string endrepeat = "endrepeat";
    
        bool isOp(string s) {
            return (
                s == add ||
                s == sub ||
                s == mult ||
                s == div
            );
        }
    }

    void execute(stack &calc_stack, std::istream &in, string s) {
        if (isNumber(s)) {
            calc_stack.push(s);
        } else if (s == cmds::repeat) {
            repeat(calc_stack, in);
        } else if (s == cmds::reverse) {
            reverse(calc_stack);
        } else if (s == cmds::pop) {
            calc_stack.pop();
        } else if (s == cmds::sqrt) {
            sqrt(calc_stack);
        } else if (cmds::isOp(s)) {
            operate(calc_stack, s);
        } else {
            return; // TODO: throw?
        }
    }
    
    void operate(stack &calc_stack, const string op) {
        const string x_str = calc_stack.top();
        calc_stack.pop();
        const string y_str = calc_stack.top();
        calc_stack.pop();

        const double x = stod(x_str);
        const double y = stod(y_str);
        double result_d;
        string symbol; // TODO: char?
        if (op == cmds::add) {
            result_d = x + y;
            symbol = "+";
        } else if (op == cmds::sub) {
            result_d = x - y;
            symbol = "-";
        } else if (op == cmds::mult) {
            result_d = x * y;
            symbol = "*";
        } else if (op == cmds::div) {
            result_d = x / y;
            symbol = "/";
        } else {
            return; // TODO: invalid operator, throw exception?
        }

        bool is_int = true;
        if (isDouble(x_str)) { // function for printing out number depending on wheteher is int or double?
            std::cout << x;
            is_int = false;
        } else {
            std::cout << static_cast<int>(x);
        }
        std::cout << " " << symbol << " ";
        if (isDouble(y_str)) {
            std::cout << y;
            is_int = false;
        } else {
            std::cout << static_cast<int>(y);
        }
        std::cout << " = ";
        string result_str;
        if (is_int) {
            const int result_i = static_cast<int>(result_d);
            std::cout << result_i;
            result_str = std::to_string(result_i);
        } else {
            std::cout << result_d;
            result_str = std::to_string(result_d); // TODO: precision?
        }
        std::cout << endl;

        calc_stack.push(result_str); 
    }
    
    void sqrt(stack &calc_stack) {
        string s = calc_stack.top();
        calc_stack.pop();
        string result_str;
        std::cout << "sqrt ";
        if (isDouble(s)) {
            const double d = stod(s);
            const double result = std::sqrt(d);
            std::cout << d << " = " << result;
            result_str = std::to_string(result); // result_str = blah // precision?
        } else {
            const int i = stoi(s);
            const int result = std::sqrt(i);
            std::cout << i << " = " << result;
            result_str = std::to_string(result);
        }
        std::cout << endl;
        calc_stack.push(result_str);
    }
    
    // void add(stack &calc_stack) {
    //     const string x = calc_stack.top();
    //     calc_stack.pop();
    //     const string y = calc_stack.top();
    //     calc_stack.pop();
    //     string result_str;
    //     if (isDouble(x) || isDouble(y)) {
    //         const double result = stod(x) + stod(y);
    //         result_str = dtos(result);
    //     } else {
    //         const int result = stoi(x) + stoi(y);
    //         result_str = std::to_string(result);
    //     }
    //     calc_stack.push(result_str);
    //     std::cout << x << " + " << y << " = " << result_str << endl;
    // }
    
    void repeat(stack &calc_stack, std::istream &in) {
        const int n = stoi(calc_stack.top()); // what if not int? would throw, but should we allow e.g. 3.00? or should truncate?
        calc_stack.pop();
        std::vector<string> cmds;
        string s;
        while (in >> s) {
            if (s == "endrepeat") {
                break;
            }
            cmds.push_back(s);
        }
        for (int i = 0; i < n; ++i) {
            for (string cmd : cmds) {
                execute(calc_stack, in, cmd);
            }
        }
    }
    
    void reverse(stack &calc_stack) {
        const int n = stoi(calc_stack.top()); // what if not int? would throw, but should we allow e.g. 3.00? or should truncate?
        // tutor just says should throw on invalid input...
        calc_stack.pop();
        std::vector<string> reverse_buffer;
        for (int i = 0; i < n; ++i) {
            reverse_buffer.push_back(calc_stack.top());
            calc_stack.pop();
        }
        for (const string s : reverse_buffer) {
            calc_stack.push(s);
        }
    }
    
    inline bool isDouble(string s) {
        return (s.find('.') != std::string::npos);
    }
    
    inline bool isNumber(string s) {
        return (isdigit(s[0]));
    }
    
    string dtos(double val) {
        return std::to_string(val);
        // std:to_string prints to 6 decimal points and doesnt print the dot if is an int
        // if really large or small will use scientific notation too...
    }
}

int main(int argc, char* argv[]) {
    // setup the print out format for the precision required.
    std::cout.setf(std::ios::fixed,std::ios::floatfield);
    std::cout.precision(3);

    // open the file for reading
    std::ifstream in;
    in.open(argv[1]);

    string s;
    calc::stack calc_stack;

    // read the file while we have input.
    while (in >> s) {
        // code style? snakecase vs camelcase?
        calc::execute(calc_stack, in, s);
    }
    in.close();
    return 0;
}