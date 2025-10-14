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
#include "evaluator.hpp"

#include <stdexcept>
#include <sstream>
#include <ostream>
#include <unordered_set>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <random>

// Sprawdza czy wartosc jest "prawdziwa", np. w warunkach if/loop.
// 0 i pusty string to falsz, reszta to prawda.
bool is_truthy(const Value& val) {
    if (val.type == TYPE_NUMBER && get<int_fast64_t>(val.data) == 0) return false;
    if (val.type == TYPE_STRING && get<string>(val.data).empty()) return false;
    return true;
}

// Glowna funkcja wykonujaca kod
Value evaluate(const Expression& expr, Environment& env) {
    // Przypadek 1: Wyrazenie to pojedynczy token (atom)
    if (holds_alternative<Token>(expr.data)) {
        const Token& token = get<Token>(expr.data);
        if (token.type == TOKEN_NUMBER) return Value{stoll(token.text), TYPE_NUMBER}; // zwracamy wartosc liczbowa
        if (token.type == TOKEN_STRING) return Value{token.text, TYPE_STRING}; // zwracamy wartosc tekstowa
        if (token.type == TOKEN_IDENTIFIER) {
            // jesli to identyfikator, szukamy go w srodowisku
            if (env.count(token.text)) return env.at(token.text);
            throw runtime_error("Undefined variable: '" + token.text + "'.");
        }
    }

    // Przypadek 2: Wyrazenie to lista (wywolanie funkcji lub operatora)
    if (holds_alternative<ExpressionList>(expr.data)) {
        const ExpressionList& list = get<ExpressionList>(expr.data);
        if (list.empty()) return Value{}; // pusta lista zwraca nil

        // Sprawdzamy, czy pierwszy element listy to slowo kluczowe
        if (holds_alternative<Token>(list[0].data) && get<Token>(list[0].data).type == TOKEN_IDENTIFIER) {
            string const& keyword = get<Token>(list[0].data).text;

            // Zbiór slow kluczowych dla szybkiego sprawdzania
            static const unordered_set<string> keywords = {
                "def", "print", "if", "loop","do",
                "String", "Number", "typeof", "fun", "input",
                "len", "get", "set", "sys", "Random", "ord", "chr"
            };

            if (keywords.count(keyword)) {
                // obsluga 'def' - tworzenie nowej zmiennej w srodowisku
                if (keyword == "def") {
                    if (list.size() != 3) throw runtime_error("'def' requires 2 arguments (name, value), but received " + to_string(list.size() - 1) + ".");
                    const string& var_name = get<Token>(list[1].data).text;
                    Value var_value = evaluate(list[2], env);
                    env[var_name] = var_value;
                    return var_value;
                }
                // obsluga 'print' - wypisuje wartosci na ekran
                if (keyword == "print") {
                    stringstream ss;
                    for (size_t i = 1; i < list.size(); ++i) ss << value_to_string(evaluate(list[i], env));
                    cout << ss.str();
                    return Value{};
                }
                // obsluga 'if' - warunek, jesli prawda to wykonuje druga czesc
                if (keyword == "if") {
                    if (list.size() != 3) throw runtime_error("'if' requires 2 arguments (condition, body), but received " + to_string(list.size() - 1) + ".");
                    if (is_truthy(evaluate(list[1], env))) return evaluate(list[2], env);
                    return Value{};
                }
                // obsluga 'loop' - petla while, wykonuje cialo dopoki warunek jest prawdziwy
                if (keyword == "loop") {
                    if (list.size() != 3) throw runtime_error("'loop' requires 2 arguments (condition, body), but received " + to_string(list.size() - 1) + ".");
                    Value last_val = {};
                    while (is_truthy(evaluate(list[1], env))) last_val = evaluate(list[2], env);
                    return last_val;
                }
                // obsluga 'do' - wykonuje sekwencje wyrazen i zwraca wartosc ostatniego
                if (keyword == "do") {
                    Value last_val = {};
                    for (size_t i = 1; i < list.size(); ++i) last_val = evaluate(list[i], env);
                    return last_val;
                }
                // obsluga 'fun' - tworzenie nowej funkcji
                if (keyword == "fun") {
                    if (list.size() != 3) throw runtime_error("'fun' requires 2 arguments (parameters, body), but received " + to_string(list.size() - 1) + ".");
                    const ExpressionList& params_list = get<ExpressionList>(list[1].data);
                    vector<string> params;
                    for (const auto& param_expr : params_list) params.push_back(get<Token>(param_expr.data).text);
                    auto body_ptr = make_shared<Expression>(list[2]);
                    BraceFunction func = {params, body_ptr, make_shared<Environment>(env)};
                    return Value{func, TYPE_FUNCTION};
                }
                // obsluga 'input' - czyta linie z konsoli
                if (keyword == "input") {
                    if (list.size() > 2) throw runtime_error("'input' takes 0 or 1 arguments, but received " + to_string(list.size() - 1) + ".");
                    if (list.size() == 2) cout << value_to_string(evaluate(list[1], env)) << flush;
                    string line;
                    getline(cin, line);
                    if (!line.empty() && line.back() == '\r') line.pop_back();
                    return Value{line, TYPE_STRING};
                }
                // Konwersja na liczbe
                if (keyword == "Number") {
                    if (list.size() != 2) throw runtime_error("'Number' requires 1 argument, but received " + to_string(list.size() - 1) + ".");
                    Value val = evaluate(list[1], env);
                    if (val.type == TYPE_STRING) return Value{stoll(get<string>(val.data)), TYPE_NUMBER};
                    return val;
                }
                // Konwersja na string
                if (keyword == "String") {
                    if (list.size() != 2) throw runtime_error("'String' requires 1 argument, but received " + to_string(list.size() - 1) + ".");
                    return Value{value_to_string(evaluate(list[1], env)), TYPE_STRING};
                }
                // Sprawdzenie typu wartosci
                if (keyword == "typeof") {
                     if (list.size() != 2) throw runtime_error("'typeof' requires 1 argument, but received " + to_string(list.size() - 1) + ".");
                     Value val = evaluate(list[1], env);
                     if (val.type == TYPE_NUMBER) return Value{"number", TYPE_STRING};
                     if (val.type == TYPE_STRING) return Value{"string", TYPE_STRING};
                     if (val.type == TYPE_FUNCTION) return Value{"function", TYPE_STRING};
                     return Value{"nil", TYPE_STRING};
                }
                // Dlugosc stringa
                if (keyword == "len") {
                    if (list.size() != 2) throw runtime_error("'len' requires 1 argument (string), but received " + to_string(list.size() - 1) + ".");
                    Value val = evaluate(list[1], env);
                    if (val.type != TYPE_STRING) throw runtime_error("Type error: 'len' only operates on strings.");
                    return Value{(int_fast64_t)get<string>(val.data).length(), TYPE_NUMBER};
                }
                // Pobranie znaku ze stringa
                if (keyword == "get") {
                    if (list.size() != 3) throw runtime_error("'get' requires 2 arguments (string, index), but received " + to_string(list.size() - 1) + ".");
                    Value str_val = evaluate(list[1], env);
                    if (str_val.type != TYPE_STRING) throw runtime_error("Type error: The first argument to 'get' must be a string.");
                    Value idx_val = evaluate(list[2], env);
                    if (idx_val.type != TYPE_NUMBER) throw runtime_error("Type error: The second argument to 'get' must be a number (index).");
                    const string& str = get<string>(str_val.data);
                    int_fast64_t idx = get<int_fast64_t>(idx_val.data);
                    if (idx < 0 || idx >= str.length()) throw runtime_error("Index out of bounds.");
                    return Value{string(1, str[idx]), TYPE_STRING};
                }
                // Ustawienie znaku w stringu (modyfikuje zmienna!)
                if (keyword == "set") {
                    if (list.size() != 4) throw runtime_error("'set' requires 3 arguments (identifier, index, value), but received " + to_string(list.size() - 1) + ".");
                    if (!holds_alternative<Token>(list[1].data) || get<Token>(list[1].data).type != TOKEN_IDENTIFIER) throw runtime_error("Type error: The first argument to 'set' must be a variable identifier.");
                    const string& var_name = get<Token>(list[1].data).text;
                    if (env.find(var_name) == env.end() || env.at(var_name).type != TYPE_STRING) throw runtime_error("Type error: Variable for 'set' must exist and be a string.");
                    Value idx_val = evaluate(list[2], env);
                    if (idx_val.type != TYPE_NUMBER) throw runtime_error("Type error: The second argument to 'set' must be a number (index).");
                    Value new_char_val = evaluate(list[3], env);
                    if (new_char_val.type != TYPE_STRING || get<string>(new_char_val.data).length() != 1) throw runtime_error("Type error: The third argument to 'set' must be a single-character string.");
                    int_fast64_t idx = get<int_fast64_t>(idx_val.data);
                    string& original_str = get<string>(env.at(var_name).data);
                    if (idx < 0 || idx >= original_str.length()) throw runtime_error("Index for 'set' is out of bounds.");
                    original_str[idx] = get<string>(new_char_val.data)[0];
                    return env.at(var_name);
                }
                // Wykonanie komendy systemowej
                if (keyword == "sys") {
                    if (list.size() != 2) throw runtime_error("'sys' requires 1 argument (a command string), but received " + to_string(list.size() - 1) + ".");
                    Value cmd_val = evaluate(list[1], env);
                    if (cmd_val.type != TYPE_STRING) throw runtime_error("Type error: The argument for 'sys' must be a string.");

                    string command = get<string>(cmd_val.data);
                    string result = "";
                    char buffer[128];

                    FILE* pipe = popen(command.c_str(), "r"); // popen to funkcja C do takich rzeczy
                    if (!pipe) throw runtime_error("Failed to execute system command.");

                    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) result += buffer;

                    pclose(pipe);
                    return Value{result, TYPE_STRING};
                }
                // Generowanie liczby losowej z przedzialu
                if (keyword == "random") {
                    // Sprawdzamy, czy mamy dwa argumenty (min, max)
                    if (list.size() != 3) throw runtime_error("'random' requires 2 arguments (min, max), but received " + to_string(list.size() - 1) + ".");

                    Value min_arg = evaluate(list[1], env);
                    Value max_arg = evaluate(list[2], env);

                    // Sprawdzamy, czy argumenty sa liczbami
                    if (min_arg.type != TYPE_NUMBER || max_arg.type != TYPE_NUMBER) throw runtime_error("Type error: Arguments for 'random' must be numbers.");

                    int_fast64_t min_val = get<int_fast64_t>(min_arg.data);
                    int_fast64_t max_val = get<int_fast64_t>(max_arg.data);

                    if (min_val > max_val) throw runtime_error("First argument to 'random' cannot be greater than the second argument.");

                    // Uzywamy nowoczesnego C++ do generowania losowych liczb
                    // Silnik jest 'static', zeby nie byl tworzony i seedowany przy kazdym wywolaniu
                    static std::mt19937 gen(std::random_device{}());
                    std::uniform_int_distribution<int_fast64_t> distrib(min_val, max_val);

                    return Value{distrib(gen), TYPE_NUMBER};
                }
                // Zwraca kod ASCII pierwszego znaku w stringu
                if (keyword == "ord") {
                    if (list.size() != 2) throw runtime_error("'ord' requires 1 argument (string).");
                    Value val = evaluate(list[1], env);
                    if (val.type != TYPE_STRING || get<string>(val.data).empty()) {
                        throw runtime_error("Argument for 'ord' must be a non-empty string.");
                    }
                    // Zwracamy kod ASCII jako liczbe
                    return Value{(int_fast64_t)(get<string>(val.data)[0]), TYPE_NUMBER};
                }

                // Zwraca jednoznakowy string dla podanego kodu ASCII
                if (keyword == "chr") {
                    if (list.size() != 2) throw runtime_error("'chr' requires 1 argument (number).");
                    Value val = evaluate(list[1], env);
                    if (val.type != TYPE_NUMBER) {
                        throw runtime_error("Argument for 'chr' must be a number.");
                    }
                    // Tworzymy string z jednego znaku i go zwracamy
                    string s(1, (char)get<int_fast64_t>(val.data));
                    return Value{s, TYPE_STRING};
                }
            }
        }

        // Jesli to nie bylo slowo kluczowe, to pewnie wywolanie funkcji
        Value first_val = evaluate(list[0], env);

        if (first_val.type == TYPE_FUNCTION) {
            const BraceFunction& func = get<BraceFunction>(first_val.data);
            if (func.parameters.size() != list.size() - 1) throw runtime_error("Incorrect number of arguments for function call. Expected " + to_string(func.parameters.size()) + ", but got " + to_string(list.size() - 1) + ".");

            // Tworzymy nowe srodowisko dla wywolania funkcji, kopiujac te z momentu jej definicji
            Environment call_env = *func.closure_env;
            // Przypisujemy argumenty do parametrow w nowym srodowisku
            for (size_t i = 0; i < func.parameters.size(); ++i) call_env[func.parameters[i]] = evaluate(list[i + 1], env);
            // Wykonujemy cialo funkcji w jej wlasnym srodowisku
            return evaluate(*func.body, call_env);
        }

        // Jesli to nie funkcja, to musi byc operator jak + - * /
        Value result = first_val;
        for (size_t i = 1; i < list.size(); i += 2) {
            const Token& op = get<Token>(list[i].data);
            Value rhs = evaluate(list[i+1], env);

            // Operator '+' dziala na stringach i liczbach
            if (op.text == "+") {
                if (result.type == TYPE_STRING || rhs.type == TYPE_STRING) {
                    result.data = value_to_string(result) + value_to_string(rhs);
                    result.type = TYPE_STRING;
                } else {
                    result.data = get<int_fast64_t>(result.data) + get<int_fast64_t>(rhs.data);
                    result.type = TYPE_NUMBER;
                }
                continue;
            }

            // Operatory porownania dzialaja na wszystkim (po konwersji na string)
            if (op.text == "==") {
                result.data = (int_fast64_t)(value_to_string(result) == value_to_string(rhs));
                result.type = TYPE_NUMBER;
                continue;
            }
            if (op.text == "!=") {
                result.data = (int_fast64_t)(value_to_string(result) != value_to_string(rhs));
                result.type = TYPE_NUMBER;
                continue;
            }

            // Pozostale operatory wymagaja liczb
            if (result.type != TYPE_NUMBER || rhs.type != TYPE_NUMBER) throw runtime_error("Type error: Operator '" + op.text + "' requires numeric operands.");

            int_fast64_t left_num = get<int_fast64_t>(result.data);
            int_fast64_t right_num = get<int_fast64_t>(rhs.data);

            if (op.text == "-") result.data = left_num - right_num;
            else if (op.text == "*") result.data = left_num * right_num;
            else if (op.text == "/") { if(right_num == 0) throw runtime_error("Division by zero."); result.data = left_num / right_num; }
            else if (op.text == "%") { if(right_num == 0) throw runtime_error("Division by zero."); result.data = left_num % right_num; }
            else if (op.text == ">") result.data = (int_fast64_t)(left_num > right_num);
            else if (op.text == "<") result.data = (int_fast64_t)(left_num < right_num);
            else if (op.text == ">=") result.data = (int_fast64_t)(left_num >= right_num);
            else if (op.text == "<=") result.data = (int_fast64_t)(left_num <= right_num);
            else throw runtime_error("Unknown operator: " + op.text);

            result.type = TYPE_NUMBER;
        }
        return result;
    }

    // Jakis blad, nie powinno sie tu nigdy trafic
    throw runtime_error("Critical error: Failed to interpret expression.");
}