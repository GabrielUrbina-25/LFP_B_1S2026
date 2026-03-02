#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <map>
#include <set>

using namespace std;

// Estructura de datos

struct Estudiante {
    int carnet;
    string nombre;
    string apellido;
    string carrera;
    int semestre;
};

struct Curso {
    int codigo;
    string nombre;
    int creditos;
    int semestre;
    string carrera;
};

struct Nota {
    int carnet;
    int codigo_curso;
    double nota;
    string ciclo;
    int anio;
};

// Variables globales 

vector<Estudiante> estudiantes;
vector<Curso> cursos;
vector<Nota> notas;

// Funciones

// Función split para separar strings por delimitador
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream ss(str);

    while (getline(ss, token, delimiter)) {
        // Eliminar espacios en blanco al inicio y final
        size_t start = token.find_first_not_of(" \t\r\n");
        size_t end = token.find_last_not_of(" \t\r\n");
        if (start != string::npos) {
            tokens.push_back(token.substr(start, end - start + 1));
        }
        else {
            tokens.push_back("");
        }
    }
    return tokens;
}

// Función para calcular la mediana
double calcularMediana(vector<double>& valores) {
    if (valores.empty()) return 0.0;

    sort(valores.begin(), valores.end());
    size_t n = valores.size();

    if (n % 2 == 0) {
        return (valores[n / 2 - 1] + valores[n / 2]) / 2.0;
    }
    else {
        return valores[n / 2];
    }
}

// Función para calcular la desviación estándar
double calcularDesviacionEstandar(vector<double>& valores, double media) {
    if (valores.size() < 2) return 0.0;

    double sumaCuadrados = 0.0;
    for (double val : valores) {
        sumaCuadrados += pow(val - media, 2);
    }
    return sqrt(sumaCuadrados / valores.size());
}

// Función para encontrar un estudiante por carnet
Estudiante* buscarEstudiante(int carnet) {
    for (auto& est : estudiantes) {
        if (est.carnet == carnet) return &est;
    }
    return nullptr;
}

// Función para encontrar un curso por código
Curso* buscarCurso(int codigo) {
    for (auto& cur : cursos) {
        if (cur.codigo == codigo) return &cur;
    }
    return nullptr;
}

// Carga de archivos

void cargarEstudiantes() {
    string filename;
    cout << "Ingrese el nombre del archivo de estudiantes: ";
    cin >> filename;

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: No se pudo abrir el archivo " << filename << endl;
        return;
    }

    estudiantes.clear();
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        vector<string> datos = split(line, ',');
        if (datos.size() >= 5) {
            Estudiante est;
            est.carnet = stoi(datos[0]);
            est.nombre = datos[1];
            est.apellido = datos[2];
            est.carrera = datos[3];
            est.semestre = stoi(datos[4]);
            estudiantes.push_back(est);
        }
    }

    file.close();
    cout << "Estudiantes cargados exitosamente: " << estudiantes.size() << " registros." << endl;
}

void cargarCursos() {
    string filename;
    cout << "Ingrese el nombre del archivo de cursos: ";
    cin >> filename;

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: No se pudo abrir el archivo " << filename << endl;
        return;
    }

    cursos.clear();
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        vector<string> datos = split(line, ',');
        if (datos.size() >= 5) {
            Curso cur;
            cur.codigo = stoi(datos[0]);
            cur.nombre = datos[1];
            cur.creditos = stoi(datos[2]);
            cur.semestre = stoi(datos[3]);
            cur.carrera = datos[4];
            cursos.push_back(cur);
        }
    }

    file.close();
    cout << "Cursos cargados exitosamente: " << cursos.size() << " registros." << endl;
}

void cargarNotas() {
    string filename;
    cout << "Ingrese el nombre del archivo de notas: ";
    cin >> filename;

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: No se pudo abrir el archivo " << filename << endl;
        return;
    }

    notas.clear();
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        vector<string> datos = split(line, ',');
        if (datos.size() >= 5) {
            Nota nota;
            nota.carnet = stoi(datos[0]);
            nota.codigo_curso = stoi(datos[1]);
            nota.nota = stod(datos[2]);
            nota.ciclo = datos[3];
            nota.anio = stoi(datos[4]);
            notas.push_back(nota);
        }
    }

    file.close();
    cout << "Notas cargadas exitosamente: " << notas.size() << " registros." << endl;
}

// Generación de reportes

void generarReporteEstadisticasCurso() {
    if (cursos.empty() || notas.empty()) {
        cout << "Error: Debe cargar cursos y notas primero." << endl;
        return;
    }

    ofstream html("reporte_estadisticas_curso.html");

    html << "<!DOCTYPE html>\n<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Estadísticas por Curso</title>\n";
    html << "<style>\n";
    html << "body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }\n";
    html << "h1 { color: #2c3e50; text-align: center; }\n";
    html << "table { width: 100%; border-collapse: collapse; margin: 20px 0; background: white; }\n";
    html << "th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }\n";
    html << "th { background: #3498db; color: white; }\n";
    html << "tr:hover { background: #f1f1f1; }\n";
    html << ".estadistica { font-weight: bold; color: #2980b9; }\n";
    html << "</style>\n</head>\n<body>\n";
    html << "<h1>📊 Reporte 1: Estadísticas Generales por Curso</h1>\n";
    html << "<table>\n";
    html << "<tr><th>Código</th><th>Curso</th><th>Estudiantes</th><th>Promedio</th>";
    html << "<th>Nota Máx</th><th>Nota Mín</th><th>Desv. Estándar</th><th>Mediana</th></tr>\n";

    for (const auto& curso : cursos) {
        vector<double> notasCurso;
        int count = 0;

        for (const auto& nota : notas) {
            if (nota.codigo_curso == curso.codigo) {
                notasCurso.push_back(nota.nota);
                count++;
            }
        }

        if (count > 0) {
            double promedio = 0, maxNota = 0, minNota = 100, desvEst = 0, mediana = 0;

            for (double n : notasCurso) {
                promedio += n;
                if (n > maxNota) maxNota = n;
                if (n < minNota) minNota = n;
            }
            promedio /= count;
            desvEst = calcularDesviacionEstandar(notasCurso, promedio);
            mediana = calcularMediana(notasCurso);

            html << "<tr>";
            html << "<td>" << curso.codigo << "</td>";
            html << "<td>" << curso.nombre << "</td>";
            html << "<td>" << count << "</td>";
            html << "<td class='estadistica'>" << fixed << setprecision(2) << promedio << "</td>";
            html << "<td>" << maxNota << "</td>";
            html << "<td>" << minNota << "</td>";
            html << "<td>" << desvEst << "</td>";
            html << "<td>" << mediana << "</td>";
            html << "</tr>\n";
        }
    }

    html << "</table>\n</body>\n</html>";
    html.close();

    cout << "✅ Reporte generado: reporte_estadisticas_curso.html" << endl;
}

void generarReporteRendimientoEstudiante() {
    if (estudiantes.empty() || notas.empty()) {
        cout << "Error: Debe cargar estudiantes y notas primero." << endl;
        return;
    }

    ofstream html("reporte_rendimiento_estudiante.html");

    html << "<!DOCTYPE html>\n<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Rendimiento por Estudiante</title>\n";
    html << "<style>\n";
    html << "body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }\n";
    html << "h1 { color: #2c3e50; text-align: center; }\n";
    html << "table { width: 100%; border-collapse: collapse; margin: 20px 0; background: white; }\n";
    html << "th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }\n";
    html << "th { background: #27ae60; color: white; }\n";
    html << "tr:hover { background: #f1f1f1; }\n";
    html << ".aprobado { color: green; font-weight: bold; }\n";
    html << ".reprobado { color: red; font-weight: bold; }\n";
    html << "</style>\n</head>\n<body>\n";
    html << "<h1>📈 Reporte 2: Rendimiento por Estudiante</h1>\n";
    html << "<table>\n";
    html << "<tr><th>Carnet</th><th>Nombre</th><th>Carrera</th><th>Semestre</th>";
    html << "<th>Promedio</th><th>Aprobados</th><th>Reprobados</th><th>Créditos</th></tr>\n";

    for (const auto& est : estudiantes) {
        double sumaNotas = 0;
        int count = 0, aprobados = 0, reprobados = 0, creditos = 0;

        for (const auto& nota : notas) {
            if (nota.carnet == est.carnet) {
                sumaNotas += nota.nota;
                count++;

                Curso* cur = buscarCurso(nota.codigo_curso);
                if (cur) {
                    if (nota.nota >= 61) {
                        aprobados++;
                        creditos += cur->creditos;
                    }
                    else {
                        reprobados++;
                    }
                }
            }
        }

        if (count > 0) {
            double promedio = sumaNotas / count;

            html << "<tr>";
            html << "<td>" << est.carnet << "</td>";
            html << "<td>" << est.nombre << " " << est.apellido << "</td>";
            html << "<td>" << est.carrera << "</td>";
            html << "<td>" << est.semestre << "</td>";
            html << "<td><b>" << fixed << setprecision(2) << promedio << "</b></td>";
            html << "<td class='aprobado'>" << aprobados << "</td>";
            html << "<td class='reprobado'>" << reprobados << "</td>";
            html << "<td>" << creditos << "</td>";
            html << "</tr>\n";
        }
    }

    html << "</table>\n</body>\n</html>";
    html.close();

    cout << "✅ Reporte generado: reporte_rendimiento_estudiante.html" << endl;
}

void generarReporteTop10() {
    if (estudiantes.empty() || notas.empty()) {
        cout << "Error: Debe cargar estudiantes y notas primero." << endl;
        return;
    }

    // Calcular promedios y ordenar
    vector<pair<double, Estudiante>> estudiantesConPromedio;

    for (const auto& est : estudiantes) {
        double suma = 0;
        int count = 0;

        for (const auto& nota : notas) {
            if (nota.carnet == est.carnet) {
                suma += nota.nota;
                count++;
            }
        }

        if (count > 0) {
            estudiantesConPromedio.push_back({ suma / count, est });
        }
    }

    // Ordenar de mayor a menor promedio
    sort(estudiantesConPromedio.begin(), estudiantesConPromedio.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });

    ofstream html("reporte_top10.html");

    html << "<!DOCTYPE html>\n<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Top 10 Mejores Estudiantes</title>\n";
    html << "<style>\n";
    html << "body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }\n";
    html << "h1 { color: #2c3e50; text-align: center; }\n";
    html << "table { width: 80%; margin: 0 auto; border-collapse: collapse; background: white; }\n";
    html << "th, td { padding: 15px; text-align: center; border-bottom: 1px solid #ddd; }\n";
    html << "th { background: #f39c12; color: white; font-size: 18px; }\n";
    html << "tr:nth-child(1) { background: gold; font-weight: bold; }\n";
    html << "tr:nth-child(2) { background: silver; }\n";
    html << "tr:nth-child(3) { background: #cd7f32; }\n";
    html << "tr:hover { background: #fff3cd; }\n";
    html << ".posicion { font-size: 24px; font-weight: bold; }\n";
    html << "</style>\n</head>\n<body>\n";
    html << "<h1>🏆 Reporte 3: Top 10 Mejores Estudiantes</h1>\n";
    html << "<table>\n";
    html << "<tr><th>Posición</th><th>Carnet</th><th>Nombre</th><th>Carrera</th>";
    html << "<th>Semestre</th><th>Promedio</th></tr>\n";

    int limite = min(10, (int)estudiantesConPromedio.size());
    for (int i = 0; i < limite; i++) {
        const auto& [promedio, est] = estudiantesConPromedio[i];

        html << "<tr>";
        html << "<td class='posicion'>" << (i + 1) << "</td>";
        html << "<td>" << est.carnet << "</td>";
        html << "<td><b>" << est.nombre << " " << est.apellido << "</b></td>";
        html << "<td>" << est.carrera << "</td>";
        html << "<td>" << est.semestre << "</td>";
        html << "<td><b>" << fixed << setprecision(2) << promedio << "</b></td>";
        html << "</tr>\n";
    }

    html << "</table>\n</body>\n</html>";
    html.close();

    cout << "✅ Reporte generado: reporte_top10.html" << endl;
}

void generarReporteReprobacion() {
    if (cursos.empty() || notas.empty()) {
        cout << "Error: Debe cargar cursos y notas primero." << endl;
        return;
    }

    // Calcular reprobación por curso
    vector<tuple<double, int, string, int, int, int>> cursosReprobacion; // %reprob, codigo, nombre, total, aprob, reprob

    for (const auto& curso : cursos) {
        int total = 0, aprobados = 0, reprobados = 0;

        for (const auto& nota : notas) {
            if (nota.codigo_curso == curso.codigo) {
                total++;
                if (nota.nota >= 61) {
                    aprobados++;
                }
                else {
                    reprobados++;
                }
            }
        }

        if (total > 0) {
            double porcentajeReprob = (reprobados * 100.0) / total;
            cursosReprobacion.push_back({ porcentajeReprob, curso.codigo, curso.nombre, total, aprobados, reprobados });
        }
    }

    // Ordenar por porcentaje de reprobación (mayor a menor)
    sort(cursosReprobacion.begin(), cursosReprobacion.end(),
        [](const auto& a, const auto& b) { return get<0>(a) > get<0>(b); });

    ofstream html("reporte_reprobacion.html");

    html << "<!DOCTYPE html>\n<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Cursos con Mayor Reprobación</title>\n";
    html << "<style>\n";
    html << "body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }\n";
    html << "h1 { color: #2c3e50; text-align: center; }\n";
    html << "table { width: 90%; margin: 0 auto; border-collapse: collapse; background: white; }\n";
    html << "th, td { padding: 12px; text-align: center; border-bottom: 1px solid #ddd; }\n";
    html << "th { background: #e74c3c; color: white; }\n";
    html << "tr:hover { background: #ffeaea; }\n";
    html << ".alta { background: #ffcccc; color: #c0392b; font-weight: bold; }\n";
    html << ".media { background: #fff3cd; color: #f39c12; }\n";
    html << ".baja { background: #d4edda; color: #27ae60; }\n";
    html << "</style>\n</head>\n<body>\n";
    html << "<h1>⚠️ Reporte 4: Cursos con Mayor Índice de Reprobación</h1>\n";
    html << "<table>\n";
    html << "<tr><th>Código</th><th>Curso</th><th>Total Est.</th><th>Aprobados</th>";
    html << "<th>Reprobados</th><th>% Reprobación</th></tr>\n";

    for (const auto& [porcentaje, codigo, nombre, total, aprob, reprob] : cursosReprobacion) {
        string clase;
        if (porcentaje >= 50) clase = "alta";
        else if (porcentaje >= 30) clase = "media";
        else clase = "baja";

        html << "<tr class='" << clase << "'>";
        html << "<td>" << codigo << "</td>";
        html << "<td><b>" << nombre << "</b></td>";
        html << "<td>" << total << "</td>";
        html << "<td>" << aprob << "</td>";
        html << "<td><b>" << reprob << "</b></td>";
        html << "<td><b>" << fixed << setprecision(2) << porcentaje << "%</b></td>";
        html << "</tr>\n";
    }

    html << "</table>\n</body>\n</html>";
    html.close();

    cout << "✅ Reporte generado: reporte_reprobacion.html" << endl;
}

void generarReporteAnalisisCarrera() {
    if (estudiantes.empty() || cursos.empty()) {
        cout << "Error: Debe cargar estudiantes y cursos primero." << endl;
        return;
    }

    // Agrupar por carrera
    map<string, vector<Estudiante>> estudiantesPorCarrera;
    map<string, vector<Curso>> cursosPorCarrera;
    map<string, vector<double>> notasPorCarrera;

    for (const auto& est : estudiantes) {
        estudiantesPorCarrera[est.carrera].push_back(est);
    }

    for (const auto& cur : cursos) {
        cursosPorCarrera[cur.carrera].push_back(cur);
    }

    // Calcular notas por carrera
    for (const auto& nota : notas) {
        Estudiante* est = buscarEstudiante(nota.carnet);
        if (est) {
            notasPorCarrera[est->carrera].push_back(nota.nota);
        }
    }

    ofstream html("reporte_analisis_carrera.html");

    html << "<!DOCTYPE html>\n<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>Análisis por Carrera</title>\n";
    html << "<style>\n";
    html << "body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }\n";
    html << "h1 { color: #2c3e50; text-align: center; }\n";
    html << ".carrera { background: white; margin: 20px 0; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n";
    html << "h2 { color: #8e44ad; border-bottom: 2px solid #8e44ad; padding-bottom: 10px; }\n";
    html << "table { width: 100%; border-collapse: collapse; margin: 10px 0; }\n";
    html << "th, td { padding: 8px; text-align: left; border: 1px solid #ddd; }\n";
    html << "th { background: #9b59b6; color: white; }\n";
    html << ".stat { display: inline-block; margin: 10px 20px; padding: 15px; background: #ecf0f1; border-radius: 5px; }\n";
    html << ".stat-value { font-size: 24px; font-weight: bold; color: #8e44ad; }\n";
    html << "</style>\n</head>\n<body>\n";
    html << "<h1>📚 Reporte 5: Análisis por Carrera</h1>\n";

    for (const auto& [carrera, ests] : estudiantesPorCarrera) {
        html << "<div class='carrera'>\n";
        html << "<h2>" << carrera << "</h2>\n";

        // Estadísticas generales
        double promedioCarrera = 0;
        if (!notasPorCarrera[carrera].empty()) {
            double suma = 0;
            for (double n : notasPorCarrera[carrera]) suma += n;
            promedioCarrera = suma / notasPorCarrera[carrera].size();
        }

        html << "<div class='stat'>Total Estudiantes<br><span class='stat-value'>" << ests.size() << "</span></div>\n";
        html << "<div class='stat'>Promedio General<br><span class='stat-value'>" << fixed << setprecision(2) << promedioCarrera << "</span></div>\n";
        html << "<div class='stat'>Cursos Disponibles<br><span class='stat-value'>" << cursosPorCarrera[carrera].size() << "</span></div>\n";

        // Distribución por semestre
        map<int, int> distribucionSemestre;
        for (const auto& est : ests) {
            distribucionSemestre[est.semestre]++;
        }

        html << "<h3>Distribución por Semestre:</h3>\n";
        html << "<table>\n<tr><th>Semestre</th><th>Cantidad de Estudiantes</th><th>Porcentaje</th></tr>\n";

        for (const auto& [sem, cant] : distribucionSemestre) {
            double porc = (cant * 100.0) / ests.size();
            html << "<tr><td>" << sem << "</td><td>" << cant << "</td>";
            html << "<td>" << fixed << setprecision(1) << porc << "%</td></tr>\n";
        }
        html << "</table>\n</div>\n";
    }

    html << "</body>\n</html>";
    html.close();

    cout << "✅ Reporte generado: reporte_analisis_carrera.html" << endl;
}

// Menú

void mostrarMenu() {
    cout << "\n========================================" << endl;
    cout << "   SISTEMA DE ANALISIS ACADEMICO" << endl;
    cout << "========================================" << endl;
    cout << "1. Cargar archivo de estudiantes" << endl;
    cout << "2. Cargar archivo de cursos" << endl;
    cout << "3. Cargar archivo de notas" << endl;
    cout << "4. Generar Reporte: Estadisticas por curso" << endl;
    cout << "5. Generar Reporte: Rendimiento por estudiantes" << endl;
    cout << "6. Generar Reporte: Top 10 mejores estudiantes" << endl;
    cout << "7. Generar Reporte: Cursos con mayor reprobacion" << endl;
    cout << "8. Generar Reporte: Analisis por carrera" << endl;
    cout << "9. Salir" << endl;
    cout << "========================================" << endl;
    cout << "Seleccione una opcion: ";
}

int main() {
    int opcion;
    bool salir = false;

    while (!salir) {
        mostrarMenu();
        cin >> opcion;

        switch (opcion) {
        case 1:
            cargarEstudiantes();
            break;
        case 2:
            cargarCursos();
            break;
        case 3:
            cargarNotas();
            break;
        case 4:
            generarReporteEstadisticasCurso();
            break;
        case 5:
            generarReporteRendimientoEstudiante();
            break;
        case 6:
            generarReporteTop10();
            break;
        case 7:
            generarReporteReprobacion();
            break;
        case 8:
            generarReporteAnalisisCarrera();
            break;
        case 9:
            cout << "¡Gracias por usar el sistema!" << endl;
            salir = true;
            break;
        default:
            cout << "Opción invalida. Intente de nuevo." << endl;
        }

        if (!salir) {
            cout << "\nPresione Enter para continuar...";
            cin.ignore();
            cin.get();
        }
    }

    return 0;
}