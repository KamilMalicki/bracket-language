# Język Programowania Bracket - Dokumentacja Techniczna

## 1\. Wprowadzenie

**Bracket Language** jest ezoterycznym językiem programowania (ang. *esolang*), zaprojektowanym w celu eksploracji nietypowych koncepcji składniowych. Jego główną cechą, która definiuje jego ezoteryczny charakter, jest unikalne połączenie dwóch paradygmatów:

1.  **Składnia oparta na S-wyrażeniach**, inspirowana rodziną języków LISP, gdzie logika jest zamykana w zagnieżdżonych nawiasach.
2.  **Tradycyjna notacja infiksowa** dla operatorów arytmetycznych i logicznych, typowa dla języków takich jak C++ czy Python.

Ta hybryda tworzy środowisko, które jest zarówno strukturalnie rygorystyczne, jak i znajome w zapisie matematycznym, co prowadzi do unikalnego doświadczenia programistycznego.

Niniejszy dokument stanowi kompletną referencję techniczną języka, przeznaczoną dla użytkowników posiadających plik wykonywalny interpretera.

## 2\. Uruchamianie Interpretera

Interpreter jest aplikacją konsolową, która wykonuje kod z plików źródłowych. Pliki te muszą posiadać rozszerzenie **`.bl`**.

#### **Windows**

W wierszu poleceń (CMD lub PowerShell) wykonaj komendę:

```bash
.\bracketLang.exe sciezka\do\skryptu.bl
```

#### **Linux / macOS**

W terminalu, po nadaniu uprawnień do uruchomienia (`chmod +x ./bracketLang`), wykonaj komendę:

```bash
./bracketLang sciezka/do/skryptu.bl
```

## 3\. Składnia i Podstawowe Koncepcje

### 3.1. S-wyrażenia (S-expressions)

Podstawową jednostką kodu jest S-wyrażenie: lista elementów zamknięta w nawiasach `()`. Pierwszy element listy jest zazwyczaj funkcją lub słowem kluczowym, a pozostałe to jego argumenty. Wyrażenia mogą być zagnieżdżane.

```lisp
(print "Wartość: " (+ 10 5))
```

### 3.2. Operatory Infiksowe

Kluczową cechą języka jest obsługa operatorów w notacji infiksowej wewnątrz S-wyrażeń. Wyrażenia arytmetyczne są przetwarzane od lewej do prawej, bez uwzględniania pierwszeństwa operatorów.

```lisp
; Poprawny zapis:
(100 - 20 + 5) ; Wynik: 85 (100-20=80, 80+5=85)

; Złożone wyrażenie:
(5 * (100 / 20)) ; Wynik: 25. Nawiasy wymuszają kolejność.
```

### 3.3. Typy Danych

Język obsługuje cztery podstawowe typy wartości:

  * `number`: 64-bitowa liczba całkowita ze znakiem.
  * `string`: Ciąg znaków.
  * `function`: Funkcja zdefiniowana przez użytkownika.
  * `nil`: Reprezentacja braku wartości.

### 3.4. Prawdziwość (Truthiness)

W kontekstach warunkowych (np. w `if` i `loop`), następujące wartości są traktowane jako **fałsz**:

  * Liczba `0`
  * Pusty napis `""`

Wszystkie pozostałe wartości są traktowane jako **prawda**.

## 4\. Referencja Języka

### 4.1. Zmienne i Środowisko

-----

**`def`**

  * **Składnia**: `(def nazwa wartość)`
  * **Opis**: Tworzy zmienną o podanej nazwie i przypisuje jej obliczoną `wartość`. Zmienna jest definiowana w bieżącym zasięgu.
  * **Przykład**: `(def wynik (10 * 2))`

### 4.2. Wejście / Wyjście

-----

**`print`**

  * **Składnia**: `(print arg1 arg2 ...)`
  * **Opis**: Konwertuje wszystkie argumenty na typ `string`, łączy je i wypisuje na standardowe wyjście.
  * **Przykład**: `(def x 10) (print "Wartość x wynosi: " x)`

**`input`**

  * **Składnia**: `(input)` lub `(input "prompt")`
  * **Opis**: Wczytuje jedną linię tekstu ze standardowego wejścia i zwraca ją jako `string`. Może opcjonalnie wyświetlić `prompt`.
  * **Przykład**: `(def imie (input "Podaj swoje imię: "))`

### 4.3. Struktury Kontrolne

-----

**`if`**

  * **Składnia**: `(if warunek wyrażenie)`
  * **Opis**: Ocenia `warunek`. Jeśli jest prawdziwy, wykonuje `wyrażenie` i zwraca jego wynik. W przeciwnym razie zwraca `nil`. Nie posiada klauzuli `else`.
  * **Przykład**: `(if (x > 10) (print "x jest większe od 10"))`

**`loop`**

  * **Składnia**: `(loop warunek wyrażenie)`
  * **Opis**: Wykonuje `wyrażenie` w pętli, dopóki `warunek` jest prawdziwy. Zwraca wartość ostatniego wykonania `wyrażenia`.
  * **Przykład**: `(loop (i < 10) (def i (i + 1)))`

**`do`**

  * **Składnia**: `(do wyr1 wyr2 ...)`
  * **Opis**: Wykonuje sekwencję wyrażeń i zwraca wynik ostatniego z nich. Przydatne do grupowania operacji w `if` lub `loop`.
  * **Przykład**: `(if (x == 1) (do (print "A") (print "B"))`

### 4.4. Funkcje Użytkownika

-----

**`fun`**

  * **Składnia**: `(fun (param1 param2 ...) ciało_funkcji)`
  * **Opis**: Tworzy anonimową funkcję (domknięcie), którą można przypisać do zmiennej. Funkcja "pamięta" zasięg, w którym została utworzona.
  * **Przykład**:
    ```lisp
    (def dodaj (fun (a b) (
      a + b
    )))
    (print (dodaj 5 3)) ; Wypisze 8
    ```

### 4.5. Operacje na Napisach

-----

  * **`(len napis)`**: Zwraca długość napisu jako `number`.
  * **`(get napis indeks)`**: Zwraca jednoznakowy `string` z podanego indeksu.
  * **`(set nazwa_zmiennej indeks znak)`**: Modyfikuje znak na podanym indeksie w zmiennej przechowującej napis.
  * **`(ord napis)`**: Zwraca kod ASCII pierwszego znaku napisu.
  * **`(chr kod_ascii)`**: Zwraca jednoznakowy `string` na podstawie kodu ASCII.

### 4.6. Konwersja i Inspekcja Typów

-----

  * **`(typeof wartość)`**: Zwraca typ wartości jako `string` ("number", "string", "function", "nil").
  * **`(Number napis)`**: Konwertuje `string` na `number`.
  * **`(String wartość)`**: Konwertuje dowolną wartość na `string`.

### 4.7. Funkcje Systemowe

-----

**`sys`**

  * **Składnia**: `(sys "komenda")`
  * **Opis**: Wykonuje komendę w powłoce systemowej i zwraca jej standardowe wyjście jako `string`.
  * **Przykład**: `(print (sys "date"))`

**`random`**

  * **Składnia**: `(random min max)`
  * **Opis**: Zwraca losową liczbę całkowitą z przedziału `[min, max]`.
  * **Przykład**: `(def rzut_kostka (random 1 6))`

## 5\. Kompletny Przykład: Gra w Zgadywanie Liczby

Poniższy skrypt implementuje prostą grę, demonstrując użycie wielu funkcji języka.

```lisp
; Ustawienie gry
(def sekretna_liczba (random 1 100))
(def proba 0)
(def licznik_prob 0)

(print "Zgadnij liczbę z przedziału od 1 do 100.")

; Główna pętla gry
(loop (proba != sekretna_liczba) (
  do
    (def proba_str (input "Podaj swoją propozycję: "))
    (def proba (Number proba_str))
    (def licznik_prob (licznik_prob + 1))

    (if (proba < sekretna_liczba) (
      print "Za mało!"
    ))

    (if (proba > sekretna_liczba) (
      print "Za dużo!"
    ))
))

; Koniec gry
(print "Gratulacje! Odgadłeś liczbę.")
(print "Udało Ci się w " + (String licznik_prob) + " próbach.")
```
## 6\. Rozszerzenie
Zachęcam do pobrania rozszerzenia do Visual Studio Code, które doda językowi kolorów i ładnego wyglądu.

## 7\. Autor
Ten Język programowania i interpreter w całości został napisany przez Kamila Malickiego.
