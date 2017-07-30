/*
    File:    test_aux_expr_scaner.h
    Created: 20 July 2017 at 12:26 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef TEST_AUX_EXPR_SCANER_H
#define TEST_AUX_EXPR_SCANER_H
#include <memory>
#include "../include/aux_expr_scaner.h"
#include "../include/aux_expr_lexem.h"
void print_aux_lexem(Aux_expr_lexem_info li);
void test_aux_scaner(std::shared_ptr<Aux_expr_scaner>& sc);
#endif