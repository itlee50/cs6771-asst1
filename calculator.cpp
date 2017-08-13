#include <iostream>
#include <fstream>
#include <iomanip>
#include <stack>
#include <vector>
#include <cmath>
#include <limits>

using std::string;

namespace calc {
    using std::endl;
    using stack = std::stack<string>;

    void execute(stack &calc_stack, std::istream &in, const string s);
    void operate(stack &calc_stack, const string op);
    void sqrt(stack &calc_stack);
    void repeat(stack &calc_stack, std::istream &in);
    void reverse(stack &calc_stack);
    inline bool isDouble(const string s);
    inline bool isNumber(const string s);
    inline bool isInt(const string s);
    string dtos(const double val);

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
    
        bool isOp(const string s) {
            return (
                s == add ||
                s == sub ||
                s == mult ||
                s == div
            );
        }
    }

    /*
     * Handle a token.
     */
    void execute(stack &calc_stack, std::istream &in, const string s) {
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
            throw std::invalid_argument("Invalid token");
        }
    }
    
    /* 
     * Removes two numbers from the stack and applies a mathematical operation
     * to them. The result is pushed back onto the stack.
     */
    void operate(stack &calc_stack, const string op) {
        const string x_str = calc_stack.top();
        calc_stack.pop();
        const string y_str = calc_stack.top();
        calc_stack.pop();

        const double x = stod(x_str);
        const double y = stod(y_str);
        double result_d;
        char symbol;
        if (op == cmds::add) {
            result_d = x + y;
            symbol = '+';
        } else if (op == cmds::sub) {
            result_d = x - y;
            symbol = '-';
        } else if (op == cmds::mult) {
            result_d = x * y;
            symbol = '*';
        } else if (op == cmds::div) {
            result_d = x / y;
            symbol = '/';
        } else {
            throw std::invalid_argument("Invalid operator");
        }

        bool is_int = true;
        if (isDouble(x_str)) {
            std::cout << x;
            is_int = false;
        } else {
            std::cout << static_cast<int>(x);
        }
        std::cout << ' ' << symbol << ' ';
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
            result_str = dtos(result_d);
        }
        std::cout << endl;

        calc_stack.push(result_str); 
    }
    
    /*
     * Removes the top of the stack and calculates the sqrt of this number.
     * Pushes the result back onto the stack.
     */
    void sqrt(stack &calc_stack) {
        string s = calc_stack.top();
        calc_stack.pop();
        string result_str;
        std::cout << "sqrt ";
        if (isDouble(s)) {
            const double d = stod(s);
            const double result = std::sqrt(d);
            std::cout << d << " = " << result;
            result_str = dtos(result);
        } else {
            const int i = stoi(s);
            const int result = std::sqrt(i);
            std::cout << i << " = " << result;
            result_str = std::to_string(result);
        }
        std::cout << endl;
        calc_stack.push(result_str);
    }
    
    /*
     * Removes the top of the stack. Numbers and commands continue to be read
     * from the file but not acted on until an endrepeat command.
     */
    void repeat(stack &calc_stack, std::istream &in) {
        const string n_str = calc_stack.top();
        if (!isInt(n_str)) {
            throw std::invalid_argument("Invalid int"); 
        }
        const int n = stoi(n_str);
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
            for (const string cmd : cmds) {
                execute(calc_stack, in, cmd);
            }
        }
    }
    
    /*
     * Removes the top of the stack. The ordering of the next n (the number that
     * was at the top of the stack) elements are reversed on the stack. 
     */
    void reverse(stack &calc_stack) {
        const string n_str = calc_stack.top();
        if (!isInt(n_str)) {
            throw std::invalid_argument("Invalid int"); 
        }
        const int n = stoi(n_str);
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
    
    inline bool isDouble(const string s) {
        return (s.find('.') != std::string::npos);
    }
    
    inline bool isNumber(const string s) {
        return (isdigit(s[0]));
    }

    inline bool isInt(const string s) {
        return (isNumber(s) && !isDouble(s));
    }
    
    /*
     * Converts a double to string with full precision.
     */
    string dtos(double val) {
        std::stringstream s;
        s.precision(std::numeric_limits<double>::digits10);
        s.setf(std::ios::fixed,std::ios::floatfield);
        s << val;
        return s.str();
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
        calc::execute(calc_stack, in, s);
    }
    in.close();
    return 0;
}
