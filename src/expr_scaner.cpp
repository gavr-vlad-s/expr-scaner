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

static const std::set<char32_t> single_quote = {U'\''};
static const std::set<char32_t> double_quote = {U'\"'};

Expr_lexem_info convert_lexeme(const Aux_expr_lexem_info aeli){
    Expr_lexem_info     eli;
    Aux_expr_lexem_code aelic = aeli.code;

    if(is_in_segment(aelic, Aux_expr_lexem_code::Nothing,
                     Aux_expr_lexem_code::Class_xdigits)
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

    eli.code = Expr_lexem_code::Class_complement;
    eli.set_of_char_index = (Aux_expr_lexem_code::Class_ndq == aelic) ?
                            (compl_set_trie->insertSet(double_quote)):
                            (compl_set_trie->insertSet(single_quote));

    return eli;
}

Expr_scaner::State_proc Expr_scaner::procs[] = {
    &Expr_scaner::begin_class_complement_proc,
    &Expr_scaner::first_char_proc,
    &Expr_scaner::body_chars_proc,
    &Expr_scaner::end_class_complement_proc
};

size_t Expr_scaner::get_set_complement(){
    size_t ret_val = 0;
    state = Begin_class_complement::Begin_class_complement;

    Aux_expr_lexem_info aeli;
    Aux_expr_lexem_code aelic;

    while((aelic = (aeli = aux_scaner-> current_lexem()).code) !=
          Aux_expr_lexem_code::Nothing)
    {
        ret_val = (this->*procs[static_cast<size_t>(state)])();
        if(State::End_class_complement == state){
            break;
        }
    }
    return ret_val;
}

Expr_lexem_info Expr_scaner::current_lexem(){
    Expr_lexem_info     eli;
    Aux_expr_lexem_info aeli;
    Aux_expr_lexem_code aelic;

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

// enum Category : uint16_t {
//     Spaces,            Other,             Action_name_begin,
//     Action_name_body,  Delimiters,        Dollar,
//     Backslash,         Opened_square_br,  After_colon,
//     After_backslash,   Begin_expr,        End_expr
// };
//
// static const std::map<char32_t, uint16_t> categories_table = {
//     {   1,    1},  {   2,    1},  {   3,    1},  {   4,    1},
//     {   5,    1},  {   6,    1},  {   7,    1},  {   8,    1},
//     {   9,    1},  {  10,    1},  {  11,    1},  {  12,    1},
//     {  13,    1},  {  14,    1},  {  15,    1},  {  16,    1},
//     {  17,    1},  {  18,    1},  {  19,    1},  {  20,    1},
//     {  21,    1},  {  22,    1},  {  23,    1},  {  24,    1},
//     {  25,    1},  {  26,    1},  {  27,    1},  {  28,    1},
//     {  29,    1},  {  30,    1},  {  31,    1},  {  32,    1},
//     {U'$',  544},  {U'(',  528},  {U')',  528},  {U'*',  528},
//     {U'+',  528},  {U'0',    8},  {U'1',    8},  {U'2',    8},
//     {U'3',    8},  {U'4',    8},  {U'5',    8},  {U'6',    8},
//     {U'7',    8},  {U'8',    8},  {U'9',    8},  {U'?',  528},
//     {U'A',   12},  {U'B',   12},  {U'C',   12},  {U'D',   12},
//     {U'E',   12},  {U'F',   12},  {U'G',   12},  {U'H',   12},
//     {U'I',   12},  {U'J',   12},  {U'K',   12},  {U'L',  268},
//     {U'M',   12},  {U'N',   12},  {U'O',   12},  {U'P',   12},
//     {U'Q',   12},  {U'R',  268},  {U'S',   12},  {U'T',   12},
//     {U'U',   12},  {U'V',   12},  {U'W',   12},  {U'X',   12},
//     {U'Y',   12},  {U'Z',   12},  {U'[',  640},  {U'\\', 576},
//     {U']',  512},  {U'_',   12},  {U'a',   12},  {U'b',  268},
//     {U'c',   12},  {U'd',  268},  {U'e',   12},  {U'f',   12},
//     {U'g',   12},  {U'h',   12},  {U'i',   12},  {U'j',   12},
//     {U'k',   12},  {U'l',  268},  {U'm',   12},  {U'n',  780},
//     {U'o',  268},  {U'p',   12},  {U'q',   12},  {U'r',  268},
//     {U's',   12},  {U't',   12},  {U'u',   12},  {U'v',   12},
//     {U'w',   12},  {U'x',  268},  {U'y',   12},  {U'z',   12},
//     {U'{', 1552},  {U'|',  528},  {U'}', 2576},
// };
//
// uint64_t Expr_scaner::get_categories_set(char32_t c){
//     auto it = categories_table.find(c);
//     if(it != categories_table.end()){
//         return it->second;
//     }else{
//         return 1ULL << Other;
//     }
// }
//
//
// Expr_scaner::Automaton_proc Expr_scaner::procs[] = {
//     &Expr_scaner::start_proc,     &Expr_scaner::unknown_proc,
//     &Expr_scaner::action_proc,    &Expr_scaner::delimiter_proc,
//     &Expr_scaner::classes_proc,   &Expr_scaner::char_proc
// };
//
// Expr_scaner::Final_proc Expr_scaner::finals[] = {
//     &Expr_scaner::none_final_proc,
//     &Expr_scaner::unknown_final_proc,
//     &Expr_scaner::action_final_proc,
//     &Expr_scaner::delimiter_final_proc,
//     &Expr_scaner::classes_final_proc,
//     &Expr_scaner::char_final_proc
// };
//
// bool Expr_scaner::start_proc() {
//     bool t = true;
//     state = -1;
//     /* For an automaton that processes a lexeme, the state with the number (-1)
//      * is the state in which this machine is initialized. */
//     if(belongs(Spaces, char_categories)){
//         loc->current_line += U'\n' == ch;
//         return t;
//     }
//     lexem_begin_line = loc->current_line;
//     if(belongs(Delimiters, char_categories)){
//         automaton = A_delimiter; token.code = UnknownLexem;
//         (loc->pcurrent_char)--;
//     }else if(belongs(Dollar, char_categories)){
//         automaton = A_action;  token.code = Action;
//         buffer = U"";
//     }else if(belongs(Opened_square_br, char_categories)){
//         automaton = A_class, token.code = Character;
//         token.c = U'[';
//     }else if(belongs(Backslash, char_categories)){
//          automaton = A_char; token.code = Character;
//     }else if(belongs(Begin_expr, char_categories)){
//         token.code = Begin_expression; t = false;
//         (loc->pcurrent_char)++;
//     }else if(belongs(End_expr, char_categories)){
//         token.code = End_expression; t = false;
//         (loc->pcurrent_char)++;
//     }else{
//         token.code = Character; token.c = ch; t = false;
//         (loc->pcurrent_char)++;
//     }
//     return t;
// }
//
// static const char* class_strings[] = {
//     "[:Latin:]",    "[:Letter:]",       "[:Russian:]",
//     "[:bdigits:]",  "[:digits:]",       "[:latin:]",
//     "[:letter:]",   "[:ndq:]",          "[:nsq:]",
//     "[:odigits:]",  "[:russian:]",      "[:xdigits:]"
// };
//
// void Expr_scaner::correct_class(){
//     /* This function corrects the code of the token, most likely a character class,
//      * and displays the necessary diagnostics. */
//     if(token.code >= M_Class_Latin){
//         int y = token.code - M_Class_Latin;
//         printf("Line %zu expects %s.\n",
//                loc->current_line,class_strings[y]);
//         token.code = static_cast<Expr_lexem_code>(y + Class_Latin);
//         en -> increment_number_of_errors();
//     }
// }
//
// Expr_lexem_info Expr_scaner::current_lexem(){
//     automaton = A_start; token.code = Nothing;
//     lexem_begin = loc->pcurrent_char;
//     bool t = true;
//     while((ch = *(loc->pcurrent_char)++)){
//         char_categories = get_categories_set(ch); // categories_table[ch];
//         t = (this->*procs[automaton])();
//         if(!t){
//             /* We get here only if the lexeme has already been read. At the same time,
//              * the current automaton has already read the character that follows
//              * immediately after the end of the lexeme read, based on this symbol, it was
//              * decided that the lexeme was read and the transition to the next character
//              * was made. Therefore, in order to not miss the first character of the next
//              * lexeme, you need to decrease the pcurrent_char pointer by one. */
//             (loc->pcurrent_char)--;
//             if(Action == token.code){
//                 /* If the current lexeme is an identifier, then this identifier must be
//                  * written to the identifier table. */
//                 token.action_name_index = ids -> insert(buffer);
//             }else if(A_class == automaton){
//                 /* If you have finished processing the class of characters, you need to
//                  * adjust its code, and, possibly, output diagnostics. */
//                 correct_class();
//             }
//             return token;
//         }
//     }
//     /* Here we can be, only if we have already read all the processed text. In this
//      * case, the pointer to the current symbol points to a character that is immediately
//      * after the null character, which is a sign of the end of the text. To avoid entering
//      * subsequent calls outside the text, you need to go back to the null character.*/
//     (loc->pcurrent_char)--;
//     /* Further, since we are here, the end of the current token (perhaps unexpected) has
//      * not yet been processed. It is necessary to perform this processing, and, probably,
//      * to display some kind of diagnostics.*/
//     (this->*finals[automaton])();
//     return token;
// }
//
// bool Expr_scaner::unknown_proc(){
//     return belongs(Other, char_categories);
// }
//
// /* This array consists of pairs of the form (state, character) and is used to initialize
//  * the character class processing automaton. The sense of the element of the array is this:
//  * if the current character in the initialization state coincides with the second component
//  * of the element, the work begins with the state that is the first component of the element.
//  * Consider, for example, the element {54, U'n '}. If the current character coincides with
//  * the second component of this element, then work begins with the state being the first
//  * component, i.e. from state 54. The array must be sorted in ascending order of the
//  * second component.*/
// static const State_for_char init_table_for_classes[] = {
//     {0, U'L'},  {14, U'R'}, {23, U'b'}, {32, U'd'}, {40, U'l'},
//     {54, U'n'}, {63, U'o'}, {72, U'r'}, {81, U'x'}
// };
//
// bool Expr_scaner::classes_proc(){
//     bool t = false;
//     switch(state){
//         case -1:
//             if(U':' == ch){
//                 state = -2; t = true;
//             }
//             break;
//         case -2:
//             if(belongs(After_colon, char_categories)){
//                 state = get_init_state(ch, init_table_for_classes,
//                                        sizeof(init_table_for_classes)/
//                                        sizeof(State_for_char));
//                 token.code = a_classes_jump_table[state].code;
//                 t = true;
//             }else{
//                 printf("The line %zu expects one of the following characters: "
//                        "L, R, b, d, l, n, o, r, x.\n",
//                        loc->current_line);
//                 en -> increment_number_of_errors();
//             }
//             break;
//         default:
//             Elem elem = a_classes_jump_table[state];
//             token.code = elem.code;
//             int y = search_char(ch, elem.symbols);
//             if(y != THERE_IS_NO_CHAR){
//                 state = elem.first_state + y; t = true;
//             }
//     }
//     return t;
// }
//
// bool Expr_scaner::char_proc(){
//     if(belongs(After_backslash, char_categories)){
//         token.c = (U'n' == ch) ? U'\n' : ch;
//         (loc->pcurrent_char)++;
//     }else{
//         token.c = U'\\';
//     }
//     return false;
// }
//
// bool Expr_scaner::delimiter_proc(){
//     bool t = false;
//     switch((char)ch){
//         case U'{':
//             token.code = Begin_expression;
//             break;
//         case U'}':
//             token.code = End_expression;
//             break;
//         case U'(':
//             token.code = Opened_round_brack;
//             break;
//         case U')':
//             token.code = Closed_round_brack;
//             break;
//         case U'|':
//             token.code = Or;
//             break;
//         case U'*':
//             token.code = Kleene_closure;
//             break;
//         case U'+':
//             token.code = Positive_closure;
//             break;
//         case U'?':
//             token.code = Optional_member;
//             break;
//     }
//     (loc->pcurrent_char)++;
//     return t;
// }
//
// bool Expr_scaner::action_proc(){
//     bool t = true;
//     /* The variable t is true if the action name has not yet
//      * been fully read, and false otherwise. */
//     if(-1 == state){
//         if(belongs(Action_name_begin, char_categories)){
//             buffer += ch; state = 0;
//         }else{
//             printf("A Latin letter or an underscore is expected on the %zu line.\n",
//                     loc->current_line);
//             en -> increment_number_of_errors();
//             t = false;
//         }
//         return t;
//     }
//     t = belongs(Action_name_body, char_categories);
//     if(t){
//         buffer += ch;
//     }
//     return t;
// }
//
// void Expr_scaner::none_final_proc(){
//     /* This subroutine will be called if, after reading the input text, it turned out
//      * to be in the A_start automaton. Then you do not need to do anything. */
// }
//
// void Expr_scaner::unknown_final_proc(){
//     /* This subroutine will be called if, after reading the input text, it turned out
//      * to be in the A_unknown automaton. Then you do not need to do anything. */
// }
//
// void Expr_scaner::action_final_proc(){
//     /* This function will be called if, after reading the input stream, they were
//      * in the action names processing automaton, the A_action automaton. Then this
//      * name should be written in the prefix tree of identifiers. */
//     token.action_name_index = ids -> insert(buffer);
// }
//
// void Expr_scaner::delimiter_final_proc(){
// }
//
// void Expr_scaner::classes_final_proc(){
//     token.code = a_classes_jump_table[state].code;
//     correct_class();
// }
//
// void Expr_scaner::char_final_proc(){
//     token.c = U'\\';
// }