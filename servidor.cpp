// AULA INCLUSIVA Y DIVERTIDA - versión de consola (C++17)
// Compilar:  g++ -std=c++17 -O2 -o aula consola.cpp
// Ejecutar:  ./aula      (en Windows: aula.exe)
//
// Cada ronda tiene 5 ejercicios y hasta 5 estrellas.
// Los errores no restan nada: se puede intentar otra vez o escribir 0 para pasar al siguiente.
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

mt19937 rng(random_device{}());
const int RONDA = 5;
int nivel = 1;  // 1 fácil, 2 medio, 3 difícil
map<string, int> sesion;  // estrellas ganadas en esta sesión (no se guarda en ningún archivo)

int azar(int a, int b) { return uniform_int_distribution<int>(a, b)(rng); }

struct Pregunta {
    string texto;
    vector<string> opciones;
    int correcta;  // posición (0, 1, 2...) dentro de opciones
};

// Arma una pregunta con la respuesta correcta y distractores tomados de "pool".
Pregunta armar(const string& texto, const string& ok, vector<string> pool, size_t n) {
    pool.erase(remove(pool.begin(), pool.end(), ok), pool.end());
    shuffle(pool.begin(), pool.end(), rng);
    if (pool.size() > n - 1) pool.resize(n - 1);
    pool.push_back(ok);
    shuffle(pool.begin(), pool.end(), rng);
    int idx = int(find(pool.begin(), pool.end(), ok) - pool.begin());
    return {texto, pool, idx};
}

size_t cuantas() { return size_t(nivel) + 2; }  // opciones por pregunta: 3, 4 o 5

// Lee un número entre 0 y max. Si se cierra la entrada, termina el programa.
int pedir(int max) {
    string linea;
    while (true) {
        cout << "  Tu respuesta: ";
        if (!getline(cin, linea)) exit(0);
        try {
            int v = stoi(linea);
            if (v >= 0 && v <= max) return v;
        } catch (...) {
        }
        cout << "  Escribe un número de la lista, por favor.\n";
    }
}

// ----- Generadores de ejercicios -----
Pregunta vocales() {
    static const vector<pair<char, string>> p = {
        {'A', "abeja"}, {'E', "elefante"}, {'I', "isla"}, {'O', "oso"}, {'U', "uvas"}};
    auto w = p[size_t(azar(0, 4))];
    return armar("¿Con qué vocal empieza «" + w.second + "»?", string(1, w.first),
                 {"A", "E", "I", "O", "U"}, min<size_t>(5, cuantas()));
}

Pregunta silabas() {
    static const vector<pair<string, string>> p = {
        {"ga", "to"}, {"ca", "sa"}, {"lu", "na"}, {"pa", "to"},
        {"va", "ca"}, {"so", "pa"}, {"ma", "no"}, {"pe", "ra"}};
    auto w = p[size_t(azar(0, int(p.size()) - 1))];
    vector<string> pool;
    for (auto& x : p) pool.push_back(x.first);
    return armar("Completa la palabra:  __ - " + w.second + "   (" + w.first + w.second + ")",
                 w.first, pool, min<size_t>(5, cuantas()));
}

vector<string> cercanos(int a) {
    vector<string> v;
    for (int i = max(0, a - 6); i <= a + 6; i++) v.push_back(to_string(i));
    return v;
}

Pregunta contar() {
    int n = azar(1, nivel == 1 ? 6 : nivel == 2 ? 10 : 15);
    string estrellas;
    for (int i = 0; i < n; i++) estrellas += "* ";
    return armar("¿Cuántas estrellas hay?\n     " + estrellas, to_string(n), cercanos(n), cuantas());
}

Pregunta sumas() {
    int m = nivel == 1 ? 5 : nivel == 2 ? 10 : 20;
    int a = azar(1, m), b = azar(1, m);
    bool suma = nivel == 1 || azar(0, 1) == 1;
    if (!suma && b > a) swap(a, b);
    int r = suma ? a + b : a - b;
    return armar(to_string(a) + (suma ? " + " : " - ") + to_string(b) + " = ?", to_string(r),
                 cercanos(r), cuantas());
}

Pregunta emociones() {
    static const vector<pair<string, string>> p = {
        {"feliz", "Sonrío y quiero jugar con todos."},
        {"triste", "Tengo ganas de llorar y de un abrazo."},
        {"enojado", "Siento calor en la cara y los puños apretados."},
        {"con miedo", "Mi corazón late fuerte y quiero esconderme."},
        {"tranquilo", "Respiro despacio y me siento en paz."}};
    auto w = p[size_t(azar(0, int(p.size()) - 1))];
    vector<string> pool;
    for (auto& x : p) pool.push_back(x.first);
    return armar("¿Cómo me siento?  " + w.second, w.first, pool, min<size_t>(5, cuantas()));
}

// ----- Una ronda de 5 ejercicios -----
void ronda(const string& titulo, const function<Pregunta()>& generar) {
    cout << "\n=== " << titulo << " ===\n";
    set<string> usadas;
    int estrellas = 0;
    for (int i = 1; i <= RONDA; i++) {
        Pregunta p = generar();
        for (int intento = 0; intento < 40 && usadas.count(p.texto); intento++) p = generar();
        usadas.insert(p.texto);
        cout << "\nEjercicio " << i << " de " << RONDA << "\n  " << p.texto << "\n";
        for (size_t k = 0; k < p.opciones.size(); k++) cout << "   " << k + 1 << ") " << p.opciones[k] << "\n";
        cout << "   (0 = pasar al siguiente)\n";
        while (true) {
            int r = pedir(int(p.opciones.size()));
            if (r == 0) {
                cout << "  Sin problema, seguimos.\n";
                break;
            }
            if (r - 1 == p.correcta) {
                cout << "  ¡Muy bien!  *\n";
                estrellas++;
                break;
            }
            cout << "  ¡Casi! Prueba otra vez.\n";
        }
    }
    sesion[titulo] += estrellas;
    cout << "\n¡Terminaste!  " << string(size_t(estrellas), '*') << string(size_t(RONDA - estrellas), '-')
         << "  (" << estrellas << " de " << RONDA << " estrellas)\n";
}

// ----- Rincón de calma: respiración guiada -----
void respirar() {
    cout << "\n=== Rincón de calma ===\nSigue las palabras. Inhala por la nariz, exhala por la boca.\n";
    for (int vez = 1; vez <= 4; vez++) {
        cout << "\nInhala... ";
        for (int s = 1; s <= 4; s++) {
            cout << s << ' ' << flush;
            this_thread::sleep_for(chrono::seconds(1));
        }
        cout << "\nExhala... ";
        for (int s = 4; s >= 1; s--) {
            cout << s << ' ' << flush;
            this_thread::sleep_for(chrono::seconds(1));
        }
        cout << '\n';
    }
    cout << "\n¡Qué bien lo hiciste!\n";
}

// ----- Hoja de trabajo para copiar o imprimir -----
void hoja() {
    cout << "\n=== Hoja de trabajo ===\n 1) Sumas\n 2) Restas\n 3) Números que faltan\n 4) Vocales que faltan\n 0) Volver\n";
    int t = pedir(4);
    if (t == 0) return;
    vector<string> pal = {"abeja", "elefante", "isla", "oso", "uvas", "gato", "luna", "pato", "casa", "vaca", "sol", "mano"};
    shuffle(pal.begin(), pal.end(), rng);
    cout << "\nNombre: ____________________   Fecha: ____________\n\n";
    for (int i = 1; i <= 10; i++) {
        cout << setw(2) << i << ")  ";
        if (t == 1) {
            cout << azar(1, 10) << " + " << azar(1, 10) << " = ______";
        } else if (t == 2) {
            int a = azar(2, 12);
            cout << a << " - " << azar(1, a) << " = ______";
        } else if (t == 3) {
            int ini = azar(1, 90), b1 = azar(0, 4), b2;
            do b2 = azar(0, 4); while (b2 == b1);
            for (int k = 0; k < 5; k++) {
                if (k == b1 || k == b2) cout << "____"; else cout << ini + k;
                if (k < 4) cout << ", ";
            }
        } else {
            string p = pal[size_t(i - 1) % pal.size()];
            vector<size_t> v;
            for (size_t k = 0; k < p.size(); k++)
                if (string("aeiou").find(p[k]) != string::npos) v.push_back(k);
            p.replace(v[size_t(azar(0, int(v.size()) - 1))], 1, "__");
            cout << p;
        }
        cout << "\n";
    }
    cout << "\nPuedes copiar esta hoja y pegarla en un documento para imprimirla.\n";
}

// ----- Resumen de la sesión -----
void resumen() {
    cout << "\n=== Resumen de esta sesión ===\n";
    if (sesion.empty()) {
        cout << "Todavía no hay rondas jugadas.\n";
        return;
    }
    for (auto& [nombre, e] : sesion) cout << "  " << nombre << ": " << e << " estrellas\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  // para ver tildes y signos de apertura
#endif
    cout << "\n*** AULA INCLUSIVA Y DIVERTIDA ***\n";
    cout << "Institución Dr. José María Vargas - San Juan de los Morros, Guárico\n";
    cout << "\"Todos somos diferentes, todos somos especiales, todos tenemos derecho a aprender\"\n";
    while (true) {
        cout << "\n--- MENÚ (nivel " << nivel << ") ---\n"
             << " 1) Vocales\n 2) Sílabas\n 3) Contar estrellas\n 4) Sumas y restas\n"
             << " 5) Emociones\n 6) Rincón de calma\n 7) Hoja de trabajo\n 8) Resumen de la sesión\n 9) Cambiar nivel (1 a 3)\n 0) Salir\n";
        int op = pedir(9);
        switch (op) {
            case 1: ronda("Vocales", vocales); break;
            case 2: ronda("Sílabas", silabas); break;
            case 3: ronda("Contar estrellas", contar); break;
            case 4: ronda("Sumas y restas", sumas); break;
            case 5: ronda("Emociones", emociones); break;
            case 6: respirar(); break;
            case 7: hoja(); break;
            case 8: resumen(); break;
            case 9:
                cout << "  ¿Qué nivel quieres (1 fácil, 2 medio, 3 difícil)?\n";
                nivel = max(1, pedir(3));
                break;
            default: cout << "\n¡Hasta pronto, explorador!\n"; return 0;
        }
    }
}