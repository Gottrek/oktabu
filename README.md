# CVRPTW Solver - Hybrid Tabu Search

## O projekcie

Projekt jest implementacją algorytmu rozwiązującego Problem Trasowania Pojazdów z Oknami Czasowymi (CVRPTW).

Rozwiązanie opiera się na podejściu hybrydowym:
1. Algorytm Konstrukcyjny: Wykorzystano heurystykę wstawiania Solomona (I1) do wygenerowania poprawnego rozwiązania początkowego.
2. Metaheurystyka: Zastosowano algorytm Tabu Search (Przeszukiwanie z Tabu) do optymalizacji wyniku. Algorytm wykorzystuje operator sąsiedztwa typu Relocate (przeniesienie klienta między trasami) oraz pamięć krótkotrwałą (Tabu List) do unikania minimów lokalnych.

Głównym celem optymalizacji jest minimalizacja całkowitego czasu tras (czas przejazdu + czas oczekiwania + czas obsługi).

## Wymagania

* Kompilator C++: Obsługujący standard C++17 (np. GCC, Clang, MSVC).
* Python 3.x: Wymagany wyłącznie do uruchomienia skryptów automatyzujących testy.
* System operacyjny: Windows, Linux lub macOS.

## Kompilacja i Uruchomienie

Projekt nie wymaga zewnętrznych bibliotek ani skomplikowanych systemów budowania. Kod źródłowy jest samowystarczalny.

### Opcja 1: Linux / macOS / Windows (MinGW/Git Bash)

Najprostsza metoda dla użytkowników terminala. Upewnij się, że jesteś w głównym katalogu projektu.

1. Kompilacja:
   g++ *.cpp -o oktabu -O3 -std=c++17

   (Flaga -O3 włącza maksymalną optymalizację kodu, co znacznie przyspiesza obliczenia).

2. Uruchomienie:
   ./oktabu ptest/c101.txt

### Opcja 2: Windows (Visual Studio)

Dla użytkowników korzystających z IDE Microsoft Visual Studio:

1. Otwórz plik rozwiązania .sln lub plik projektu .vcxproj.
2. Na górnym pasku zmień konfigurację z Debug na Release (kluczowe dla wydajności).
3. Zbuduj projekt: Build -> Build Solution (lub skrót Ctrl + Shift + B).
4. Plik wykonywalny oktabu.exe zostanie utworzony w folderze x64/Release.

Możesz uruchomić program przez wiersz poleceń (CMD/PowerShell) lub bezpośrednio z IDE.

## Instrukcja użycia (CLI)

Program przyjmuje ścieżkę do pliku wejściowego jako pierwszy argument oraz szereg opcjonalnych flag sterujących parametrami algorytmu.

Składnia:
./oktabu <plik_wejsciowy> [opcje]

Dostępne opcje:

| Flaga      | Opis                                                | Wartość domyślna |
| :---       | :---                                                | :---             |
| -o <plik>  | Ścieżka do pliku, w którym zapisany zostanie wynik  | rozwiazanie.txt  |
| -t <int>   | Długość listy Tabu (Tabu Tenure)                    | 10               |
| -i <int>   | Maksymalna liczba iteracji algorytmu                | 1000             |
| -a <double>| Współczynnik kary Alpha (dla eksperymentów)         | 1.0              |

Przykłady użycia:

1. Uruchomienie z domyślnymi parametrami:
   ./oktabu ptest/r101.txt

2. Uruchomienie z tuningiem parametrów i własnym plikiem wyjściowym:
   ./oktabu ptest/c102.txt -t 20 -i 5000 -o wyniki/moj_wynik.txt

## Automatyzacja testów (Python)

W projekcie znajduje się skrypt testerka.py, który służy do przeprowadzania masowych testów na wielu instancjach (Benchmark Solomon).

Skrypt automatycznie:
1. Wykrywa system operacyjny i plik wykonywalny.
2. Uruchamia solver dla zdefiniowanych kombinacji parametrów.
3. Agreguje wyniki w czytelnej tabeli CSV.
4. Zapisuje szczegółowe trasy w osobnych plikach tekstowych.

Uruchomienie:
python testerka.py

## Kluczowe pliki projektu

* main.cpp - Główny plik programu, obsługa argumentów.
* Solomon.cpp / .h - Implementacja algorytmu konstrukcyjnego.
* tabu.cpp / .h - Implementacja algorytmu Tabu Search.
* data.cpp / .h - Struktury danych i obsługa plików.
* testerka.py - Skrypt do automatyzacji testów.
* ptest/ - Katalog zawierający instancje testowe.

## Szczegóły implementacyjne

* Delta Evaluation: Algorytm wykorzystuje ocenę przyrostową (delta) do szybkiego szacowania kosztu ruchu O(1) zamiast przeliczania całej trasy O(N), co pozwala na sprawdzenie tysięcy sąsiadów w ułamku sekundy.
* Reprezentacja: Rozwiązanie przechowywane jest jako wektor tras, gdzie każda trasa to wektor ID klientów.
* Format danych: Obsługiwany format zgodny ze standardem benchmarków Solomona (1987).

Projekt zrealizowany w ramach zajęć akademickich z optymalizacji kombinatorycznej.
