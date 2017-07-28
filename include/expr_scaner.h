/*
    File:    expr_scaner.h
    Created: 13 December 2015 at 09:05 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef EXPR_SCANER_H
#define EXPR_SCANER_H

#include <string>
#include <memory>
#include "../include/location.h"
#include "../include/error_count.h"
#include "../include/trie.h"
#include "../include/trie_for_set.h"
#include "../include/expr_lexem_info.h"

class Expr_scaner : public Scaner<Expr_lexem_info> {
public:
    Expr_scaner()                        = default;
    Expr_scaner(Location_ptr location, const Errors_and_tries& et) :
        Scaner<Expr_lexem_info>(location, et) {};
    Expr_scaner(const Expr_scaner& orig) = default;
    virtual ~Expr_scaner()               = default;
    virtual Expr_lexem_info current_lexem();
private:
//     enum Category : unsigned short {
//          Spaces,            Other,             Action_name_begin,
//          Action_name_body,  Delimiters,        Dollar,
//          Backslash,         Opened_square_br,  After_colon,
//          After_backslash,   Begin_expr,        End_expr
//     };
//
//     enum Automaton_name{
//         A_start,     A_unknown, A_action,
//         A_delimiter, A_class,   A_char
//     };
//     Automaton_name automaton; /* current automaton */
//     int            state;     /* current state of the current automaton */
//
//     typedef bool (Expr_scaner::*Automaton_proc)();
//     /* It is the type of the pointer on function-member that implements
//      * the state machine that handles the lexem. The function should
//      * return true if the token has not been read to the end,
//      * and false otherwise. */
//
//     typedef void (Expr_scaner::*Final_proc)();
//     /* It is the type of the pointer on function-member that performs
//      * the necessary actions in case of unexpected end of lexem. */
//
//     static Automaton_proc procs[];
//     static Final_proc     finals[];
//     /* functions for handling lexems: */
//     bool start_proc();     bool unknown_proc();
//     bool action_proc();    bool delimiter_proc();
//     bool classes_proc();   bool char_proc();
//     /* functions to perform actions in case of unexpected end of lexem */
//     void none_final_proc();      void unknown_final_proc();
//     void action_final_proc();    void delimiter_final_proc();
//     void classes_final_proc();   void char_final_proc();
//     /* If the lexem most likely is character class, then the following
//      * function corrects lexem code, and displays the needed diagnostic
//      * messsage. */
//     void correct_class();
//     /* The following function returns the set of character categories such that
//      * the character c belongs these categories. */
//     uint64_t get_categories_set(char32_t c);
};

using Expr_scaner_ptr = std::shared_ptr<Expr_scaner>;
#endif