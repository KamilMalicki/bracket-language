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

        // Ignorujemy biale znaki
        if (isspace(c)) { i++; continue; }

        // Proste tokeny jednoznakowe - nawiasy
        if (c == '(') { tokens.push_back({TOKEN_LPAREN, "("}); i++; continue; }
        if (c == ')') { tokens.push_back({TOKEN_RPAREN, ")"}); i++; continue; }

        // Operatory jednoznakowe
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%') {
            tokens.push_back({TOKEN_OPERATOR, string(1, c)}); i++; continue;
        }

        // Operatory ktore moga miec dwa znaki (np. ==, !=, >=, <=)
        if (c == '=' || c == '!' || c == '<' || c == '>') {
            string op_str;
            op_str += c;
            i++;
            // Sprawdzamy czy nastepny znak to '='
            if (i < source.length() && source[i] == '=') {
                op_str += source[i];
                i++;
            }
            tokens.push_back({TOKEN_OPERATOR, op_str});
            continue;
        }

        // Stringi w cudzyslowach
        if (c == '"') {
            string str_value; i++; // pomijamy cudzyslow
            while (i < source.length() && source[i] != '"') {
                // obsluga znakow specjalnych jak \n, \t
                if (source[i] == '\\' && i + 1 < source.length()) {
                    i++;
                    switch (source[i]) {
                        case 'n': str_value += '\n'; break; case 't': str_value += '\t'; break;
                        case 'r': str_value += '\r'; break; default: str_value += source[i]; break;
                    }
                } else  str_value += source[i];
                i++;
            }
            i++; // pomijamy cudzyslow zamykajacy
            tokens.push_back({TOKEN_STRING, str_value}); continue;
        }

        // Liczby
        if (isdigit(c)) {
            string num_str;
            // zbieramy wszystkie cyfry pod rzad
            while (i < source.length() && isdigit(source[i])) num_str += source[i++];

            // Specjalna skladnia dla operatora index' - np. 1' to to samo co (get text 1)
            if (i < source.length() && source[i] == '\'') {
                i++;
                tokens.push_back({TOKEN_INDEX_OP, num_str});
            } else tokens.push_back({TOKEN_NUMBER, num_str});
        }
        // Identyfikatory (nazwy zmiennych, funkcji)
        else if (isalpha(c)) {
            string identifier;
            // zbieramy wszystko co jest litera, cyfra lub podkreslnikiem
            while (i < source.length() && (isalnum(source[i]) || source[i] == '_')) identifier += source[i++];
            tokens.push_back({TOKEN_IDENTIFIER, identifier});
        } else i++; // jakis nieznany znak, po prostu go ignorujemy i idziemy dalej
    }
    return tokens;
}