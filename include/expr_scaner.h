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
#include <set>
#include "../include/location.h"
#include "../include/error_count.h"
#include "../include/trie.h"
#include "../include/trie_for_set.h"
#include "../include/expr_lexem_info.h"
#include "../include/aux_expr_scaner.h"
#include "../include/aux_expr_lexem.h"

class Expr_scaner{
public:
    Expr_scaner()                        = default;
    Expr_scaner(const Location_ptr&              location,
                const Errors_and_tries&          et,
                const Trie_for_set_of_char32ptr& trie_for_complement_of_set) :
        aux_scaner(std::make_unique<Aux_expr_scaner>(location, et)),
        compl_set_trie(trie_for_complement_of_set) {};
    Expr_scaner(const Expr_scaner& orig) = default;
    ~Expr_scaner()                       = default;

    Expr_lexem_info current_lexem();
private:
    Trie_for_set_of_char32ptr compl_set_trie;
    Aux_expr_scaner_ptr       aux_scaner;

    enum class State{
        Begin_class_complement, First_char,
        Body_chars,             End_class_complement
    };

/*
 * The lexeme 'character class complement' can be descripted as the following regular
 * expression:
 *          ab+c
 * where
 *      a is the lexeme 'Begin_char_class_complement',
 *      b is the lexeme 'Character',
 *      c is the lexeme 'End_char_class_complement'.
 *
 * If we construct a non-deterministic finite automaton by this regexp, next we build
 * a corresponding deterministic finite automaton, and, finally, we minimize the
 * deterministic automaton, then we obtain a finite automaton with the following
 * transition table:
 *
 * |-------|---|---|---|--------------|
 * | State | a | b | c |    Remark    |
 * |-------|---|---|---|--------------|
 * |   A   | B |   |   | Begin state. |
 * |-------|---|---|---|--------------|
 * |   B   |   | C |   |              |
 * |-------|---|---|---|--------------|
 * |   C   |   | C | E |              |
 * |-------|---|---|---|--------------|
 * |   E   |   |   |   | End state.   |
 * |-------|---|---|---|--------------|
 *
 * But for ease of writing, we need to introduce more meaningful names for states of
 * a finite automaton. The following table shows the matching state names from the
 * previous table and meaningful names. Meaningful names are collected in the enumeration
 * State.
 *
 * |---|------------------------|
 * |   |    Meaningful name     |
 * |---|------------------------|
 * | A | Begin_class_complement |
 * |---|------------------------|
 * | B | First_char             |
 * |---|------------------------|
 * | C | Body_chars             |
 * |---|------------------------|
 * | E | End_class_complement   |
 * |---|------------------------|
 *
 */
    size_t get_set_complement();

    using State_proc = size_t (Expr_scaner::*)();

    State state;

    std::set<char32_t> curr_set;

    static State_proc procs[];

    size_t begin_class_complement_proc(); size_t first_char_proc();
    size_t body_chars_proc();             size_t end_class_complement_proc();

    Aux_expr_lexem_info aeli;
    Aux_expr_lexem_code aelic;
};

using Expr_scaner_ptr = std::shared_ptr<Expr_scaner>;
#endif