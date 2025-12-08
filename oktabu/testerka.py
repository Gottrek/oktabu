#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import subprocess
import os
import csv
import datetime
import time
import sys
import platform

# --- KONFIGURACJA ---
INPUT_FILES = ["c101.txt", "r101.txt", "c102.txt"] 
INPUT_FOLDER = "ptest"       
OUTPUT_FOLDER_RAW = "wyniki_raw"
OUTPUT_FOLDER_CSV = "wyniki_csv"

TABU_TENURES = [10, 20]     
ITERATIONS = [1000]        
ALPHAS = [1.0]             

# Domyœlna nazwa, jeœli nie podasz innej
DEFAULT_PROG_NAME = "oktabu"

# --- LOGIKA SKRYPTU ---

def get_executable_path():
    # 1. SprawdŸ czy u¿ytkownik poda³ nazwê jako argument (np. python testerka.py moj_test)
    if len(sys.argv) > 1:
        target_name = sys.argv[1]
    else:
        target_name = DEFAULT_PROG_NAME

    # 2. Ustal system i ewentualne rozszerzenie
    is_windows = platform.system() == "Windows"
    expected_ext = ".exe" if is_windows else ""

    # Jeœli u¿ytkownik poda³ nazwê bez .exe na Windowsie, dodaj je
    if is_windows and not target_name.lower().endswith(".exe"):
        target_name += ".exe"
    
    # Lista miejsc do sprawdzenia
    search_paths = [
        ".",                  # Bie¿¹cy folder
        "x64/Release",        # Visual Studio
        "x64/Debug",          # Visual Studio
        "../x64/Release",     # VS (folder nadrzêdny)
        "../x64/Debug",       # VS (folder nadrzêdny)
        "build"               # CMake
    ]

    # Jeœli u¿ytkownik poda³ œcie¿kê absolutn¹ lub wzglêdn¹ (np. ./oktabu), sprawdŸ j¹ bezpoœrednio
    if os.path.sep in target_name or (os.path.altsep and os.path.altsep in target_name):
        if os.path.isfile(target_name):
            return os.path.abspath(target_name)

    # Szukanie w folderach
    for folder in search_paths:
        candidate = os.path.join(folder, target_name)
        if os.path.isfile(candidate):
            return os.path.abspath(candidate)
            
        # Fallback dla Linuxa: Czasami ktoœ skompiluje jako "oktabu.exe" mimo ¿e to Linux
        if not is_windows and os.path.isfile(candidate + ".exe"):
             return os.path.abspath(candidate + ".exe")

    return None

def main():
    exe_path = get_executable_path()
    
    if not exe_path:
        print(f"!!! BLAD: Nie znaleziono pliku wykonywalnego !!!")
        print(f"Szukalem domyslnie '{DEFAULT_PROG_NAME}' lub tego co podales w argumencie.")
        print("Skompiluj projekt i upewnij sie, ze plik jest w folderze.")
        print("Przyklad uzycia: python3 testerka.py nazwa_mojego_pliku")
        return

    print(f"Uzywam programu: {exe_path}")

    # Utwórz foldery
    for folder in [OUTPUT_FOLDER_RAW, OUTPUT_FOLDER_CSV]:
        if not os.path.exists(folder):
            os.makedirs(folder)

    timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    csv_filename = f"zestawienie_{timestamp}.csv"
    csv_full_path = os.path.join(OUTPUT_FOLDER_CSV, csv_filename)

    with open(csv_full_path, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f, delimiter=';')
        writer.writerow(["Plik", "Tabu", "Iter", "Alpha", "Klienci", "Trasy", "Dystans", "CzasTrasy", "CzasExe_ms"])

        total = len(INPUT_FILES) * len(TABU_TENURES) * len(ITERATIONS) * len(ALPHAS)
        count = 0

        for inp_name in INPUT_FILES:
            inp_path = os.path.join(INPUT_FOLDER, inp_name)
            if not os.path.exists(inp_path):
                print(f"Pomijam brak pliku: {inp_path}")
                continue

            for t in TABU_TENURES:
                for i in ITERATIONS:
                    for a in ALPHAS:
                        count += 1
                        print(f"[{count}/{total}] {inp_name} (T={t}, I={i})... ", end="")
                        
                        out_name = f"{os.path.splitext(inp_name)[0]}_t{t}_i{i}.txt"
                        out_path = os.path.join(OUTPUT_FOLDER_RAW, out_name)

                        cmd = [exe_path, inp_path, "-o", out_path, "-t", str(t), "-i", str(i), "-a", str(a)]

                        try:
                            result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
                            
                            if result.returncode != 0:
                                print("CRASH")
                                continue

                            found = False
                            for line in result.stdout.splitlines():
                                if line.startswith("STATS;"):
                                    parts = line.split(";")
                                    writer.writerow([inp_name, t, i, a, parts[1], parts[2], parts[3].replace('.', ','), parts[4].replace('.', ','), parts[5]])
                                    f.flush()
                                    print("OK")
                                    found = True
                                    break
                            if not found: print("BRAK DANYCH")

                        except Exception as e:
                            print(f"BLAD: {e}")

    print(f"\nGotowe. Wyniki: {csv_full_path}")

if __name__ == "__main__":
    main()