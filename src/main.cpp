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
#include "types.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// Pomocnicza funkcja do zamiany naszej wartosci Value na string
string value_to_string(const Value& val) {
    if (val.type == TYPE_NUMBER) return to_string(get<int_fast64_t>(val.data));
    if (val.type == TYPE_STRING) return get<string>(val.data);
    return "nil";
}

// Druga pomocnicza funkcja, wypisuje wartosc na standardowe wyjscie
void print_value(const Value& val) {
    cout << value_to_string(val);
}

int main(int argc, char* argv[]) {
    // Sprawdzamy czy podano nazwe pliku jako argument
    if (argc != 2) {
        error_label:
        cout
            << endl
            << "########################################################################"
            << endl
            << "# --------- --------- Bracket Language interpreter --------- --------- #"
            << endl
            << "# Project was created by 2026 by Kamil Malicki                         #"
            << endl
            << "# Github: https://github.com/KamilMalicki/bracket-language             #"
            << endl
            << "########################################################################";
        cerr << endl << "Usage: " << argv[0] << " <filename.bl>" << endl;
        return 1;
    }

    // Sprawdzamy czy rozszerzenie pliku jest poprawne i czy jest to .bl
    string filename = argv[1];
    if (filename.size() < 3 || filename.substr(filename.size() - 3) != ".bl") {
        cerr << "Error: Only .bl files are allowed" << endl;
        goto error_label;
    }

    // Otwieramy plik i wczytujemy go do bufora
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file '" << filename << "'" << endl;
        goto error_label;
    }

    // Tworzymy bufor do wczytania całego pliku i zapisujemy go do stringa
    stringstream buffer;
    buffer << file.rdbuf();
    string source_code = buffer.str();

    // Glowny blok try-catch, zeby lapac wszystkie bledy z interpretera
    try {
        Environment global_env; // Tworzymy globalne srodowisko dla zmiennych
        // Krok 1: Tokenizacja kodu
        vector<Token> tokens = tokenize(source_code);
        // Krok 2: Parsowanie tokenow na wyrazenia
        ExpressionList expressions = parse(tokens);
        // Krok 3: Wykonanie kazdego wyrazenia z osobna
        for (const auto& expr : expressions) evaluate(expr, global_env);

    }
    // W przypadku wystapienia wyjatku, wypisujemy informacje o bledzie
    catch (const exception& e) {
        // Wypisujemy bledy na standardowe wyjscie
        cerr << "Execution error: " << e.what() << endl;
        return 1;
    }

    // Wypisujemy wartosci zmiennych globalnych na standardowe wyjscie
    return 0;
}
