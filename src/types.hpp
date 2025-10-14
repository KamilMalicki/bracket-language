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
#pragma once

#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <unordered_map>

using namespace std;

// Deklaracje z gory, zeby sie nie gryzlo pozniej
struct Value;
struct Expression;

// Srodowisko, czyli mapa trzymajaca nasze zmienne. Klucz to nazwa, wartosc to Value.
using Environment = unordered_map<string, Value>;

// Specjalna struktura dla funkcji, przechowuje parametry, cialo i srodowisko z momentu definicji
struct BraceFunction {
    vector<string> parameters;          // nazwy parametrow
    shared_ptr<Expression> body;        // cialo funkcji (kod do wykonania)
    shared_ptr<Environment> closure_env; // "domkniecie", czyli srodowisko w ktorym funkcja powstala
};

// Typy wartosci jakie moga istniec w naszym jezyku
enum ValueType { TYPE_NUMBER, TYPE_STRING, TYPE_NIL, TYPE_FUNCTION };

// Taka nasza uniwersalna wartosc, moze byc liczba, stringiem, funkcja albo niczym (nil).
// variant to fajna rzecz, przechowuje jeden z typow w danym momencie.
struct Value {
    variant<int_fast64_t, string, BraceFunction> data;
    ValueType type = TYPE_NIL; // domyslnie wszystko jest nil
};


// Typy tokenow, zeby bylo wiadomo co jest czym po pracy lexera
enum TokenType { TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_NUMBER, TOKEN_STRING, TOKEN_IDENTIFIER, TOKEN_OPERATOR, TOKEN_INDEX_OP };

// Token, czyli najmniejsza czastka kodu. Ma swoj typ i tekst.
struct Token { TokenType type; string text; };

// Lista wyrazen, przydatne do przechowywania ciala funkcji albo listy argumentow
using ExpressionList = vector<Expression>;

// Wyrazenie - moze byc albo pojedynczym tokenem (np. liczba) albo lista innych wyrazen (np. wywolanie funkcji)
struct Expression { variant<Token, ExpressionList> data; };

// Deklaracje funkcji z main.cpp, zeby mozna bylo z nich korzystac w evaluatorze
string value_to_string(const Value& val);
void print_value(const Value& val);