/*
    File:    test_aux_expr_scaner.h
    Created: 20 July 2017 at 12:54 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <cstdio>
#include "../include/test_aux_expr_scaner.h"
#include "../include/char_conv.h"

/* Данный массив состоит из строковых литералов, представляющих собой заключённые
 * в кавычки идентификаторы из перечисления Lexem_code. Строки идут в том же
 * порядке, что и соответствующие идентификаторы перечисления Lexem_code. */
static const char* lexem_names[] = {
    "Nothing",                     "UnknownLexem",              "Action",
    "Opened_round_brack",          "Closed_round_brack",        "Or",
    "Kleene_closure",              "Positive_closure",          "Optional_member",
    "Character",                   "Begin_expression",          "End_expression",
    "Class_Latin",                 "Class_Letter",              "Class_Russian",
    "Class_bdigits",               "Class_digits",              "Class_latin",
    "Class_letter",                "Class_odigits",             "Class_russian",
    "Class_xdigits",               "Class_ndq",                 "Class_nsq",
    "Begin_char_class_complement", "End_char_class_complement", "M_Class_Latin",
    "M_Class_Letter",              "M_Class_Russian",           "M_Class_bdigits",
    "M_Class_digits",              "M_Class_latin",             "M_Class_letter",
    "M_Class_odigits",             "M_Class_russian",           "M_Class_xdigits",
    "M_Class_ndq",                 "M_Class_nsq"
};

void print_aux_lexem(Aux_expr_lexem_info li){
    Aux_expr_lexem_code lc = li.code;
    printf("%s", lexem_names[static_cast<uint16_t>(lc)]);
    if(Aux_expr_lexem_code::Character == lc){
        char32_t ch = li.c;
        if(ch < U' '){
            printf(" %u", ch);
        }else{
            std::string s = char32_to_utf8(ch);
            printf(" '%s'",s.c_str());
        }
    }
    printf(" \n");
}

void test_aux_scaner(std::shared_ptr<Aux_expr_scaner> sc)
{
    Aux_expr_lexem_info lexem;
    do{
        lexem = sc -> current_lexem();
        print_aux_lexem(lexem);
    }while(lexem.code != Aux_expr_lexem_code::Nothing);
}