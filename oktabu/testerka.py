# -*- coding: utf-8 -*-
import subprocess
import os
import csv
import datetime
import time

# --- KONFIGURACJA ---
INPUT_FILES = ["c101.txt", "r101.txt", "c102.txt"] 
INPUT_FOLDER = "ptest"       
OUTPUT_FOLDER_RAW = "wyniki_raw"  # Tu trafi¹ pliki .txt
OUTPUT_FOLDER_CSV = "wyniki_csv"  # Tu trafi¹ pliki .csv

TABU_TENURES = [10, 20]     
ITERATIONS = [1000]        
ALPHAS = [1.0]             

# Gdzie szukaæ pliku EXE
EXE_PATHS = [
    r"..\x64\Debug\oktabu.exe", 
    r"x64\Debug\oktabu.exe",    
    r".\oktabu.exe"             
]

# --- SKRYPT ---
def main():
    # 1. ZnajdŸ plik exe
    exe_path = None
    for path in EXE_PATHS:
        if os.path.exists(path):
            exe_path = os.path.abspath(path)
            break
    
    if not exe_path:
        print("!!! BLAD: Nie znaleziono pliku oktabu.exe !!!")
        print("Skompiluj projekt (Ctrl+Shift+B).")
        return

    print(f"Uzywam programu: {exe_path}")

    # 2. Utwórz foldery na wyniki, jeœli nie istniej¹
    for folder in [OUTPUT_FOLDER_RAW, OUTPUT_FOLDER_CSV]:
        if not os.path.exists(folder):
            os.makedirs(folder)
            print(f"Utworzono folder: {folder}")

    # 3. Generuj nazwê pliku CSV z dat¹
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    csv_filename = f"zestawienie_{timestamp}.csv"
    csv_full_path = os.path.join(OUTPUT_FOLDER_CSV, csv_filename)

    # 4. Otwórz plik CSV do zapisu
    with open(csv_full_path, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f, delimiter=';')
        writer.writerow(["Plik", "Tabu", "Iter", "Alpha", "Klienci", "Trasy", "Dystans", "CzasTrasy", "CzasExe_ms"])

        total = len(INPUT_FILES) * len(TABU_TENURES) * len(ITERATIONS) * len(ALPHAS)
        count = 0

        for inp_name in INPUT_FILES:
            inp_path = os.path.join(INPUT_FOLDER, inp_name)

            if not os.path.exists(inp_path):
                print(f"Pomijam brakujacy plik: {inp_path}")
                continue

            for t in TABU_TENURES:
                for i in ITERATIONS:
                    for a in ALPHAS:
                        count += 1
                        print(f"[{count}/{total}] Testuje: {inp_name} (T={t}, I={i})... ", end="")

                        # Nazwa pliku RAW (szczegó³owego)
                        out_name = f"{os.path.splitext(inp_name)[0]}_t{t}_i{i}_a{int(a)}.txt"
                        out_path = os.path.join(OUTPUT_FOLDER_RAW, out_name)

                        cmd = [
                            exe_path, 
                            inp_path, 
                            "-o", out_path,
                            "-t", str(t), 
                            "-i", str(i), 
                            "-a", str(a)
                        ]

                        try:
                            result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
                            
                            if result.returncode != 0:
                                print("BLAD (Crash)")
                                continue

                            # Parsowanie wyjœcia STATS;
                            found = False
                            for line in result.stdout.splitlines():
                                if line.startswith("STATS;"):
                                    parts = line.split(";")
                                    # STATS;Klienci;Trasy;Dystans;Czas;CzasExe
                                    clients = parts[1]
                                    routes = parts[2]
                                    dist = parts[3].replace('.', ',')
                                    time_route = parts[4].replace('.', ',')
                                    duration = parts[5]

                                    writer.writerow([inp_name, t, i, a, clients, routes, dist, time_route, duration])
                                    f.flush()
                                    print("OK")
                                    found = True
                                    break
                            
                            if not found:
                                print("BLAD (Brak STATS)")

                        except Exception as e:
                            print(f"BLAD SKRYPTU: {e}")

    print(f"\nGotowe.")
    print(f"Tabela CSV: {csv_full_path}")
    print(f"Pliki tras: {OUTPUT_FOLDER_RAW}")

if __name__ == "__main__":
    main()