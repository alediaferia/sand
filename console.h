//
// Created by Alessandro Diaferia on 16/11/2015.
//

class Console {
public:
    Console();

    Parser *parser() const;

    void run();

private:
    bool volatile _running;

    friend void _int_handler(int);
};
