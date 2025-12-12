# CVRPTW Solver - Hybrid Tabu Search

Projekt jest implementacją algorytmu rozwiązującego **Problem Trasowania Pojazdów z Oknami Czasowymi** (CVRPTW - Capacitated Vehicle Routing Problem with Time Windows).

Celem optymalizacji jest minimalizacja całkowitego czasu tras, na który składają się: czas przejazdu, czas oczekiwania oraz czas obsługi klienta.

## Metodyka

Rozwiązanie opiera się na podejściu hybrydowym (dwufazowym):

1. **Algorytm Konstrukcyjny:**
   - Wykorzystano heurystykę wstawiania Solomona (I1).
   - Generuje ona poprawne (dopuszczalne) rozwiązanie początkowe, biorąc pod uwagę ograniczenia pojemności i okna czasowe.

2. **Metaheurystyka (Optymalizacja):**
   - Zastosowano algorytm Tabu Search (Przeszukiwanie z zabronieniami).
   - Operator sąsiedztwa: Relocate (przeniesienie klienta między trasami).
   - Mechanizm Tabu: Pamięć krótkotrwała (Tabu List) zapobiega cykleniu i pozwala algorytmowi uciec z minimów lokalnych.

## Wymagania

- Kompilator C++: Obsługujący standard C++17 (np. GCC, Clang, MSVC).
- System operacyjny: Windows, Linux lub macOS.
- Python 3.x: (Opcjonalnie) Wymagany wyłącznie do uruchomienia skryptów automatyzujących testy (testerka.py).

## Kompilacja i Uruchomienie

Projekt nie wymaga zewnętrznych bibliotek. Kod źródłowy jest samowystarczalny.

### Opcja 1: Linux / macOS / Windows (MinGW/Git Bash)

Dla użytkowników terminala. Upewnij się, że jesteś w głównym katalogu projektu.

1. Kompilacja:
   g++ *.cpp -o oktabu -O3 -std=c++17

   (Uwaga: Flaga -O3 włącza agresywną optymalizację kodu, co jest kluczowe dla wydajności algorytmu).

2. Uruchomienie:
   ./oktabu ptest/c101.txt

### Opcja 2: Windows (Visual Studio)

1. Otwórz plik rozwiązania .sln lub utwórz nowy projekt z istniejącego kodu.
2. Na górnym pasku zmień konfigurację z Debug na Release (krytyczne dla szybkości działania!).
3. Zbuduj projekt: Build -> Build Solution (Ctrl + Shift + B).
4. Plik wykonywalny oktabu.exe znajdzie się w katalogu x64/Release.

## Instrukcja użycia (CLI)

Program przyjmuje ścieżkę do pliku wejściowego jako pierwszy argument oraz opcjonalne flagi sterujące.

Składnia:
./oktabu <plik_wejsciowy> [opcje]

Dostępne opcje:

| Flaga | Opis                                               | Wartość domyślna |
|-------|----------------------------------------------------|------------------|
| -o    | Ścieżka do pliku wynikowego (np. wynik.txt)        | rozwiazanie.txt  |
| -t    | Długość kadencji Tabu (Tabu Tenure)                | 10               |
| -i    | Maksymalna liczba iteracji algorytmu               | 1000             |
| -a    | Współczynnik kary Alpha (eksperymentalny)          | 1.0              |

Przykłady:

1. Uruchomienie z domyślnymi parametrami:
   ./oktabu ptest/r101.txt

2. Uruchomienie ze strojeniem parametrów i własnym plikiem wyjściowym:
   ./oktabu ptest/c102.txt -t 20 -i 5000 -o wyniki/moj_wynik.txt

## Automatyzacja testów (Python)

W projekcie znajduje się skrypt `testerka.py`, który służy do przeprowadzania masowych testów na benchmarkach (instancje Solomona).

Skrypt automatycznie:
1. Wykrywa system operacyjny i plik wykonywalny.
2. Uruchamia solver dla zdefiniowanych kombinacji parametrów.
3. Agreguje wyniki w czytelnej tabeli CSV.
4. Zapisuje szczegółowe trasy w osobnych plikach tekstowych.

Uruchomienie:
python testerka.py

## Kluczowe pliki projektu

- main.cpp: Punkt wejścia, obsługa argumentów CLI.
- Solomon.cpp / .h: Implementacja algorytmu konstrukcyjnego.
- tabu.cpp / .h: Implementacja metaheurystyki Tabu Search.
- data.cpp / .h: Struktury danych (Klient, Trasa) i parser plików wejściowych.
- testerka.py: Skrypt do automatyzacji testów.
- ptest/: Katalog z przykładowymi instancjami testowymi.

## Szczegóły implementacyjne

- Delta Evaluation: Algorytm wykorzystuje ocenę przyrostową (delta) do szybkiego szacowania kosztu ruchu. Zamiast przeliczać całą trasę (O(N)), zmiana kosztu obliczana jest w czasie O(1), co pozwala na sprawdzenie tysięcy sąsiadów w ułamku sekundy.
- Reprezentacja: Rozwiązanie przechowywane jest jako wektor tras, gdzie każda trasa to wektor ID klientów.
- Format danych: Obsługiwany format zgodny ze standardem benchmarków Solomona (1987).

---
Projekt zrealizowany w ramach zajęć akademickich z optymalizacji kombinatorycznej.
