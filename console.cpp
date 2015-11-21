#include "console.h"
#include "parser.h"

#include <iostream>

#ifdef UNIX
#include <signal.h>
#endif

static const char *SAND_CONSOLE_HEADER =
" \
  Sand Version 0.0.0.1 \
  Press Ctrl+C to Quit \
";

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
#ifdef UNIX
    signal(SIGINT, _int_handler);
#endif
    std::cout << SAND_CONSOLE_HEADER;

    while (_running) {
        std::string input;
        std::cout << _prompt;
        std::getline(std::cin, input);
        std::shared_ptr<Printable> p = _parser->parse(input);
    }
}
