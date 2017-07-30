/*
    File:    test_expr_scaner.h
    Created: 30 July 2017 at 12:00 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef TEST_EXPR_SCANER_H
#define TEST_EXPR_SCANER_H
#include <memory>
#include "../include/expr_scaner.h"
#include "../include/expr_lexem_info.h"
#include "../include/trie_for_set.h"
void print_expr_lexem(const Expr_lexem_info& li,
                      const Trie_for_set_of_char32ptr& t);
void test_expr_scaner(const std::shared_ptr<Expr_scaner>& sc,
                      const Trie_for_set_of_char32ptr& t);
#endif