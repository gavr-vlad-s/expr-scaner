/*
    File:    expr_scaner.cpp
    Created: 13 December 2015 at 09:05 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/expr_scaner.h"
#include "../include/aux_expr_lexem.h"
#include "../include/belongs.h"
#include "../include/sets_for_classes.h"
#include <cstdlib>
#include <cstdio>

// #include "../include/search_char.h"
// #include "../include/get_init_state.h"

// static const Expr_lexem_code convertation_table[] = {
//     Expr_lexem_code::Nothing,                     Expr_lexem_code::UnknownLexem,              Expr_lexem_code::Action,
//     Expr_lexem_code::Opened_round_brack,          Expr_lexem_code::Closed_round_brack,        Expr_lexem_code::Or,
//     Expr_lexem_code::Kleene_closure,              Expr_lexem_code::Positive_closure,          Expr_lexem_code::Optional_member,
//     Expr_lexem_code::Character,                   Expr_lexem_code::Begin_expression,          Expr_lexem_code::End_expression,
//     Expr_lexem_code::Class_Latin,                 Expr_lexem_code::Class_Letter,              Expr_lexem_code::Class_Russian,
//     Expr_lexem_code::Class_bdigits,               Expr_lexem_code::Class_digits,              Expr_lexem_code::Class_latin,
//     Expr_lexem_code::Class_letter,                Expr_lexem_code::Class_odigits,             Expr_lexem_code::Class_russian,
//     Expr_lexem_code::Class_xdigits
// };

//Aux_expr_lexem_code
// enum class Aux_expr_lexem_code : uint16_t {
//     Nothing,                     UnknownLexem,              Action,
//     Opened_round_brack,          Closed_round_brack,        Or,
//     Kleene_closure,              Positive_closure,          Optional_member,
//     Character,                   Begin_expression,          End_expression,
//     Class_Latin,                 Class_Letter,              Class_Russian,
//     Class_bdigits,               Class_digits,              Class_latin,
//     Class_letter,                Class_odigits,             Class_russian,
//     Class_xdigits,               Class_ndq,                 Class_nsq,
//     Begin_char_class_complement, End_char_class_complement, M_Class_Latin,
//     M_Class_Letter,              M_Class_Russian,           M_Class_bdigits,
//     M_Class_digits,              M_Class_latin,             M_Class_letter,
//     M_Class_odigits,             M_Class_russian,           M_Class_xdigits,
//     M_Class_ndq,                 M_Class_nsq
// };
//
// enum class Expr_lexem_code : uint16_t {
//     Nothing,             UnknownLexem,       Action,
//     Opened_round_brack,  Closed_round_brack, Or,
//     Kleene_closure,      Positive_closure,   Optional_member,
//     Character,           Begin_expression,   End_expression,
//     Class_Latin,         Class_Letter,       Class_Russian,
//     Class_bdigits,       Class_digits,       Class_latin,
//     Class_letter,        Class_odigits,      Class_russian,
//     Class_xdigits,       Class_complement
// };

template<typename T>
bool is_in_segment(T value, T lower, T upper)
{
    return (lower <= value) && (value <= upper);
}

inline uint64_t belongs(Aux_expr_lexem_code e, uint64_t s)
{
    return belongs(static_cast<uint64_t>(e), s);
}

static const std::set<char32_t> single_quote = {U'\''};
static const std::set<char32_t> double_quote = {U'\"'};

static const uint64_t classes_of_chars_without_complement =
    (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_Latin))   |
    (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_Letter))  |
    (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_Russian)) |
    (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_bdigits)) |
    (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_digits))  |
    (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_latin))   |
    (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_letter))  |
    (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_odigits)) |
    (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_russian)) |
    (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_xdigits));

static const uint64_t classes_of_chars_with_complement =
    (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_ndq)) |
    (1ULL << static_cast<uint64_t>(Aux_expr_lexem_code::Class_nsq));

static const char* not_admissible_nsq_ndq =
    "Error at line %zu: character classes [:ndq:] and [:nsq:] are not admissible "
    "in the character class complement.\n";

static const char* not_admissible_lexeme =
    "Error at line %zu: expected a character or character class, with the "
    "exception of [:nsq:] and [:ndq:].\n";

Expr_lexem_info Expr_scaner::convert_lexeme(const Aux_expr_lexem_info aeli){
    Expr_lexem_info     eli;
    Aux_expr_lexem_code aelic = aeli.code;

    if(is_in_segment(aelic, Aux_expr_lexem_code::Nothing,
                     Aux_expr_lexem_code::Class_xdigits))
    {
        eli.code = static_cast<Expr_lexem_code>(aelic);
        if(aelic == Aux_expr_lexem_code::Character)
        {
            eli.c = aeli.c;
        }else if(aelic == Aux_expr_lexem_code::Action)
        {
            eli.action_name_index = aeli.action_name_index;
        }
        return eli;
    }

    if(is_in_segment(aelic, Aux_expr_lexem_code::M_Class_Latin,
                     Aux_expr_lexem_code::M_Class_nsq))
    {
        int y = static_cast<int>(aelic) -
                static_cast<int>(Aux_expr_lexem_code::M_Class_Latin) +
                static_cast<int>(Aux_expr_lexem_code::Class_Latin);
        aelic = static_cast<Aux_expr_lexem_code>(y);
    }

    if(belongs(aelic, classes_of_chars_with_complement)){
        eli.code = Expr_lexem_code::Class_complement;
        eli.set_of_char_index = (Aux_expr_lexem_code::Class_ndq == aelic) ?
                                (compl_set_trie->insertSet(double_quote)):
                                (compl_set_trie->insertSet(single_quote));
    }else{
        eli.code = static_cast<Expr_lexem_code>(aelic);
    }

    return eli;
}

Expr_scaner::State_proc Expr_scaner::procs[] = {
    &Expr_scaner::begin_class_complement_proc,
    &Expr_scaner::first_char_proc,
    &Expr_scaner::body_chars_proc,
    &Expr_scaner::end_class_complement_proc
};

void Expr_scaner::begin_class_complement_proc(){
    if(Aux_expr_lexem_code::Begin_char_class_complement == aelic){
        state = State::First_char;
    }
}

static const size_t first_char_class =
    static_cast<size_t>(Aux_expr_lexem_code::Class_Latin);

inline size_t char_class_to_array_index(Aux_expr_lexem_code e)
{
    return static_cast<uint64_t>(e) - first_char_class;
}

void Expr_scaner::first_char_proc(){
    state = State::Body_chars;
    if(Aux_expr_lexem_code::Character == aelic){
        curr_set.insert(aeli.c);
    }else if(belongs(aelic, classes_of_chars_without_complement)){
        const auto& s = sets_for_char_classes[char_class_to_array_index(aelic)];
        curr_set.insert(s.begin(), s.end());
    }else if(belongs(aelic, classes_of_chars_with_complement)){
        printf(not_admissible_nsq_ndq, aux_scaner->lexem_begin_line_number());
        et_.ec->increment_number_of_errors();
    }else{
        printf(not_admissible_lexeme, aux_scaner->lexem_begin_line_number());
        et_.ec->increment_number_of_errors();
    }
}

void Expr_scaner::body_chars_proc(){
    state = State::Body_chars;
    if(Aux_expr_lexem_code::Character == aelic){
        curr_set.insert(aeli.c);
    }else if(belongs(aelic, classes_of_chars_without_complement)){
        const auto& s = sets_for_char_classes[char_class_to_array_index(aelic)];
        curr_set.insert(s.begin(), s.end());
    }else if(belongs(aelic, classes_of_chars_with_complement)){
        printf(not_admissible_nsq_ndq, aux_scaner->lexem_begin_line_number());
        et_.ec->increment_number_of_errors();
    }else if(Aux_expr_lexem_code::End_char_class_complement == aelic){
        set_idx = compl_set_trie->insertSet(curr_set);
    }else{
        printf(not_admissible_lexeme, aux_scaner->lexem_begin_line_number());
        et_.ec->increment_number_of_errors();
    }
}

void Expr_scaner::end_class_complement_proc(){}

size_t Expr_scaner::get_set_complement(){
    set_idx = 0;
    state   = State::Begin_class_complement;

    curr_set.clear();

    while((aelic = (aeli = aux_scaner-> current_lexem()).code) !=
          Aux_expr_lexem_code::Nothing)
    {
        (this->*procs[static_cast<size_t>(state)])();
        if(State::End_class_complement == state){
            break;
        }
    }
    return set_idx;
}

Expr_lexem_info Expr_scaner::current_lexem(){
    Expr_lexem_info     eli;

    aelic = (aeli = aux_scaner-> current_lexem()).code;
    switch(aelic){
        case Aux_expr_lexem_code::Nothing ... Aux_expr_lexem_code::Class_xdigits:
        case Aux_expr_lexem_code::Class_ndq:
        case Aux_expr_lexem_code::Class_nsq:
        case Aux_expr_lexem_code::M_Class_Latin ... Aux_expr_lexem_code::M_Class_nsq:
            eli = convert_lexeme(aeli);
            break;
        case Aux_expr_lexem_code::Begin_char_class_complement:
            aux_scaner->back();
            eli.code              = Expr_lexem_code::Class_complement;
            eli.set_of_char_index = get_set_complement();
            break;
        case Aux_expr_lexem_code::End_char_class_complement:
            eli.code = Expr_lexem_code::UnknownLexem;
    }
    return eli;
}