/*
    File:    test_expr_scaner.cpp
    Created: 30 July 2017 at 12:10 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <cstdio>
#include "../include/test_expr_scaner.h"
#include "../include/char_conv.h"
#include "../include/print_char32.h"
#include "../include/operations_with_sets.h"

static const char* lexem_names[] = {
    "Nothing",            "UnknownLexem",       "Action",
    "Opened_round_brack", "Closed_round_brack", "Or",
    "Kleene_closure",     "Positive_closure",   "Optional_member",
    "Character",          "Begin_expression",   "End_expression",
    "Class_Latin",        "Class_Letter",       "Class_Russian",
    "Class_bdigits",      "Class_digits",       "Class_latin",
    "Class_letter",       "Class_odigits",      "Class_russian",
    "Class_xdigits",      "Class_complement"
};

void print_expr_lexem(const Expr_lexem_info& li,
                      const Trie_for_set_of_char32ptr& t)
{
    using operations_with_sets::print_set;

    Expr_lexem_code lc = li.code;
    printf("%s ", lexem_names[static_cast<uint16_t>(lc)]);
    if(Expr_lexem_code::Character == lc){
        print_char32(li.c);
    }else if(Expr_lexem_code::Class_complement == lc){
        print_set(t->get_set(li.set_of_char_index), print_char32);
    }
    printf(" \n");

}

void test_expr_scaner(const std::shared_ptr<Expr_scaner>& sc,
                      const Trie_for_set_of_char32ptr&    t)
{
    Expr_lexem_info lexem;
    do{
        lexem = sc -> current_lexem();
        print_expr_lexem(lexem, t);
    }while(lexem.code != Expr_lexem_code::Nothing);
}