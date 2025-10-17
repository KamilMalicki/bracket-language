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
#include "parser.hpp"
#include <stdexcept>

// Globalna pozycja w wektorze tokenow, zeby nie przekazywac jej ciagle w argumentach funkcji
static int current_token_pos = 0;

// Deklaracja, bo funkcje wywoluja sie nawzajem (rekurencja)
static Expression parse_expression(vector<Token>& tokens);

// Glowna funkcja rekurencyjna parsera
static Expression parse_expression(vector<Token>& tokens) {
    // Zabezpieczenie przed wyjsciem poza wektor
    if (current_token_pos >= tokens.size()) throw runtime_error("Unexpected end of code.");
    Token token = tokens[current_token_pos++]; // Bierzemy token i przesuwamy wskaznik

    // Zamiast tworzyć specjalny operator `_index_op`, od razu tworzymy
    // standardowe wywołanie funkcji `get` w odpowiedniej kolejności argumentów.
    if (token.type == TOKEN_INDEX_OP) {
        ExpressionList get_call_list;

        // 1. Dodajemy nazwę funkcji "get"
        get_call_list.push_back(Expression{Token{TOKEN_IDENTIFIER, "get"}});

        // 2. Parsujemy następne wyrażenie (które powinno być stringiem lub zmienną) i dodajemy je jako PIERWSZY argument
        get_call_list.push_back(parse_expression(tokens));

        // 3. Dodajemy indeks z apostrofu jako DRUGI argument
        get_call_list.push_back(Expression{Token{TOKEN_NUMBER, token.text}});

        // Wynikiem jest poprawna lista, np. (get tekst 0), którą ewaluator rozumie bez żadnych modyfikacji.
        return Expression{get_call_list};
    }

    // Jesli token nie jest nawiasem otwierajacym, to jest to proste wyrazenie (atom) - np. liczba, string, nazwa zmiennej
    if (token.type != TOKEN_LPAREN) return Expression{token};

    // Jesli byl nawias otwierajacy, to tworzymy liste wyrazen
    ExpressionList list;
    // Parusjemy wszystko az do nawiasu zamykajacego
    while (current_token_pos < tokens.size() && tokens[current_token_pos].type != TOKEN_RPAREN) {
        list.push_back(parse_expression(tokens));
    }

    // Sprawdzamy, czy petla nie skonczyla sie z powodu konca pliku
    if (current_token_pos >= tokens.size() || tokens[current_token_pos].type != TOKEN_RPAREN) {
        throw runtime_error("Syntax error: Missing closing parenthesis ')'.");
    }

    current_token_pos++; // Przesuwamy sie za nawias zamykajacy
    return Expression{list}; // Zwracamy gotowa liste jako jedno wyrazenie
}

// Funkcja startowa dla parsera
ExpressionList parse(vector<Token>& tokens) {
    current_token_pos = 0; // Resetujemy pozycje przed kazdym parsowaniem
    ExpressionList top_level_expressions;
    // Parsujemy wyrazenia tak dlugo, az skoncza sie tokeny
    while (current_token_pos < tokens.size()) {
        top_level_expressions.push_back(parse_expression(tokens));
    }
    return top_level_expressions;
}
