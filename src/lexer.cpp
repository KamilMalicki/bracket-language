/**
 * Copyright 2025 KamilMalicki
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "lexer.hpp"

// Glowna funkcja lexera, przelatuje po calym kodzie znak po znaku i dzieli go na tokeny
vector<Token> tokenize(const string& source) {
    vector<Token> tokens;
    int i = 0; // nasz aktualny wskaznik na znak w kodzie
    while (i < source.length()) {
        char c = source[i];

        // 1. Ignorujemy biale znaki (spacje, tabulatory, nowe linie)
        if (isspace(c)) {
            i++;
            continue;
        }

        // ***************************************************************
        // *** OTO POPRAWKA DLA KOMENTARZY                             ***
        // ***************************************************************
        // Jeśli napotkamy średnik, ignorujemy wszystko do końca linii.
        if (c == ';') {
            while (i < source.length() && source[i] != '\n') {
                i++;
            }
            continue; // Przechodzimy do następnej iteracji głównej pętli
        }

        // 2. Proste tokeny jednoznakowe - nawiasy
        if (c == '(') {
            tokens.push_back({TOKEN_LPAREN, "("});
            i++;
            continue;
        }
        if (c == ')') {
            tokens.push_back({TOKEN_RPAREN, ")"});
            i++;
            continue;
        }

        // 3. Operatory jednoznakowe
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%') {
            tokens.push_back({TOKEN_OPERATOR, string(1, c)});
            i++;
            continue;
        }

        // 4. Operatory, ktore moga miec dwa znaki (np. ==, !=, >=, <=)
        if (c == '=' || c == '!' || c == '<' || c == '>') {
            string op_str;
            op_str += c;
            i++;
            // Sprawdzamy, czy nastepny znak to '=', aby stworzyc operator dwuznakowy
            if (i < source.length() && source[i] == '=') {
                op_str += source[i];
                i++;
            }
            tokens.push_back({TOKEN_OPERATOR, op_str});
            continue;
        }

        // 5. Stringi w cudzyslowach
        if (c == '"') {
            string str_value;
            i++; // pomijamy cudzyslow otwierajacy
            while (i < source.length() && source[i] != '"') {
                // Obsluga znakow specjalnych jak \n, \t (tzw. escape characters)
                if (source[i] == '\\' && i + 1 < source.length()) {
                    i++; // pomijamy backslash
                    switch (source[i]) {
                        case 'n': str_value += '\n'; break;
                        case 't': str_value += '\t'; break;
                        case 'r': str_value += '\r'; break;
                        default: str_value += source[i]; break; // np. dla \"
                    }
                } else {
                    str_value += source[i];
                }
                i++;
            }
            i++; // pomijamy cudzyslow zamykajacy
            tokens.push_back({TOKEN_STRING, str_value});
            continue;
        }

        // 6. Liczby
        if (isdigit(c)) {
            string num_str;
            // Zbieramy wszystkie cyfry pod rzad, tworzac pelna liczbe
            while (i < source.length() && isdigit(source[i])) {
                num_str += source[i++];
            }

            // Specjalna skladnia dla operatora ' - np. 1' to to samo co (get text 1)
            if (i < source.length() && source[i] == '\'') {
                i++;
                tokens.push_back({TOKEN_INDEX_OP, num_str});
            } else {
                tokens.push_back({TOKEN_NUMBER, num_str});
            }
            continue;
        }

        // 7. Identyfikatory (nazwy zmiennych, funkcji)
        if (isalpha(c)) {
            string identifier;
            // zbieramy wszystko co jest litera, cyfra lub podkreslnikiem
            while (i < source.length() && (isalnum(source[i]) || source[i] == '_')) {
                identifier += source[i++];
            }
            tokens.push_back({TOKEN_IDENTIFIER, identifier});
            continue;
        }

        // 8. Jesli jakis znak nie pasuje do zadnej kategorii, po prostu go ignorujemy
        i++;
    }
    return tokens;
}
