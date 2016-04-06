#include "console.h"
#include "parser.h"
#include "engine.h"

#include <iostream>

#include "mpc.h"

#ifdef UNIX
#include <signal.h>
#endif

static const char *SAND_CONSOLE_HEADER =
"\
Sand Version 0.0.0.1\n\
Press Ctrl+C to Quit\n\
\n";

static const char *SAND_PS = "sand> ";

static Console *s_instance = nullptr;
void _int_handler(int sig) {
    if (s_instance != 0) {
        s_instance->_running = false;
    }
}

Console::Console() : _running(false),
                     _prompt(SAND_PS)
 {
    s_instance = this;
    _parser = new Parser(Parser::Stdin);
}

Console::~Console() {
    delete _parser;
}

void Console::run() {
    _running = true;
    Engine engine;
#ifdef UNIX
    signal(SIGINT, _int_handler);
#endif
    std::cout << SAND_CONSOLE_HEADER;

    std::string input;
    while (_running) {
        std::cout << _prompt;
        std::getline(std::cin, input);
//        auto p = _parser->parse(input);
//        if (p.isRight()) {
//            std::cout << "parser: " << p.right()->printable() << std::endl;
//        } else {
//            LValRef lval = engine.eval(p.left());
//            std::cout << "-> " << lval->printable() << std::endl;
//        }

        auto it = input.cbegin();
        auto end = input.cend();
        switch (_parser->getNextToken(it, end)) {
            case tok_eof:
                return;
            case tok_def:
            {
                auto fast = _parser->parseDefinition(it, end);
                engine.eval(std::move(fast));
            }
                break;
            break;
            default:
            {
                auto expr = _parser->parseTopLevelExpr(it, end);
                if (expr != nullptr) {
                    auto value = engine.eval(std::move(expr));
                    value->dump();
                }
            }
                break;
        }
    }
}
