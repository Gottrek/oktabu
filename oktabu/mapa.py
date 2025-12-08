import matplotlib.pyplot as plt
import matplotlib.cm as cm
import os

# --- PARSER INSTANCJI (SOLOMON) ---
def parse_solomon_file(filename):
    depot = None
    customers = []

    try:
        with open(filename, 'r') as f:
            lines = f.readlines()

        for line in lines:
            parts = line.strip().split()

            if not parts: continue
            if not parts[0].isdigit(): continue # Skip headers
            if len(parts) < 4: continue

            try:
                cust_no = int(parts[0])
                x_coord = float(parts[1])
                y_coord = float(parts[2])
                demand = float(parts[3])
                
                if cust_no == 0:
                    depot = {'id': cust_no, 'x': x_coord, 'y': y_coord, 'demand': demand}
                else:
                    customers.append({'id': cust_no, 'x': x_coord, 'y': y_coord, 'demand': demand})
            except ValueError:
                continue
                
    except Exception as e:
        print("Error reading instance file: " + str(e))
        return None, None

    return depot, customers

# --- PARSER ROZWIAZANIA ---
def parse_solution_file(filename):
    routes = []
    total_cost = 0
    
    try:
        with open(filename, 'r') as f:
            lines = f.readlines()
        
        # Pominiecie pustych linii
        lines = [l.strip() for l in lines if l.strip()]

        if not lines:
            print("Solution file is empty.")
            return None

        # Linia 1: Liczba tras i koszt (np. "11 1987.79952")
        header = lines[0].split()
        if len(header) >= 2:
            print("Solution info - Routes: " + header[0] + ", Cost: " + header[1])

        # Kolejne linie: Trasy
        for line in lines[1:]:
            parts = line.split()
            # Konwersja ID klientow na int
            try:
                route_ids = [int(p) for p in parts]
                routes.append(route_ids)
            except ValueError:
                print("Warning: Skipping invalid line in solution: " + line)
                continue
                
    except Exception as e:
        print("Error reading solution file: " + str(e))
        return None

    return routes

# --- RYSOWANIE ---
def plot_solution(depot, customers, routes, title):
    if not depot or not customers:
        print("No data to plot.")
        return

    # 1. Przesuniecie ukladu do (0,0) i mapa wspolrzednych
    depot_x_orig = depot['x']
    depot_y_orig = depot['y']
    
    # Slownik do szybkiego wyszukiwania wspolrzednych po ID
    # Depot (ID 0) jest teraz w (0,0)
    coords = {0: (0, 0)} 
    
    cust_x_list = []
    cust_y_list = []
    cust_demands = []
    cust_ids = []

    for c in customers:
        rel_x = c['x'] - depot_x_orig
        rel_y = c['y'] - depot_y_orig
        
        coords[c['id']] = (rel_x, rel_y)
        
        cust_x_list.append(rel_x)
        cust_y_list.append(rel_y)
        cust_demands.append(c['demand'])
        cust_ids.append(c['id'])

    # 2. Skalowanie wielkosci punktow
    min_size, max_size = 50, 200
    vals = cust_demands
    min_val = min(vals) if vals else 0
    max_val = max(vals) if vals else 1
    
    sizes = []
    for d in cust_demands:
        if max_val > min_val:
            norm = (d - min_val) / (max_val - min_val)
            s = min_size + norm * (max_size - min_size)
        else:
            s = (min_size + max_size) / 2
        sizes.append(s)

    plt.figure(figsize=(12, 10))

    # 3. RYSOWANIE TRAS (LINIE)
    if routes:
        # Generowanie kolorow (tyle ile tras)
        colors = cm.rainbow([i / len(routes) for i in range(len(routes))])
        
        for idx, route in enumerate(routes):
            # Dodajemy Depot (0) na poczatek i koniec trasy
            full_route = [0] + route + [0]
            
            rx = []
            ry = []
            for point_id in full_route:
                if point_id in coords:
                    rx.append(coords[point_id][0])
                    ry.append(coords[point_id][1])
            
            # Rysowanie linii
            plt.plot(rx, ry, color=colors[idx], linewidth=1.5, alpha=0.7, label=("Route " + str(idx+1)))

    # 4. RYSOWANIE PUNKTOW (NA WIERZCHU)
    # Klienci
    plt.scatter(cust_x_list, cust_y_list, c='blue', s=sizes, alpha=1.0, zorder=5)
    # Depot
    plt.scatter(0, 0, c='red', marker='s', s=250, edgecolors='black', zorder=10)

    # 5. Etykiety
    for i, txt in enumerate(cust_ids):
        plt.annotate(str(txt), (cust_x_list[i], cust_y_list[i]), xytext=(3, 3), textcoords='offset points', fontsize=8, fontweight='bold')
    
    plt.annotate("Depot", (0, 0), xytext=(5, -15), textcoords='offset points', fontweight='bold', color='red')

    plt.title(title)
    plt.xlabel("X (relative)")
    plt.ylabel("Y (relative)")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.axis('equal')
    
    # Legenda (jesli tras jest duzo, moze zaslaniac, wiec mozna ja wylaczyc)
    if routes and len(routes) <= 15:
        plt.legend(loc='best', fontsize='small')
    
    plt.show()

# --- MAIN ---
if __name__ == "__main__":
    print("--- CVRPTW Solution Visualizer ---")
    
    # 1. Sciezka do instancji (dane klientow)
    path_inst = input("Instance file path (e.g. ptest/R102.txt): ").strip().replace('"', '').replace("'", "")
    
    # 2. Sciezka do rozwiazania (trasy)
    path_sol = input("Solution file path (e.g. ptest/rozwiazanie.txt): ").strip().replace('"', '').replace("'", "")

    if os.path.exists(path_inst) and os.path.exists(path_sol):
        print("Loading data...")
        depot_data, cust_data = parse_solomon_file(path_inst)
        routes_data = parse_solution_file(path_sol)
        
        if depot_data and cust_data and routes_data:
            print("Drawing solution...")
            title = "Solution: " + os.path.basename(path_sol) + " (Instance: " + os.path.basename(path_inst) + ")"
            plot_solution(depot_data, cust_data, routes_data, title)
        else:
            print("Error: Could not parse files correctly.")
    else:
        print("Error: One or both files do not exist.")