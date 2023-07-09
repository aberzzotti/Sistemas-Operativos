#include <iostream>

#include "CargarArchivos.hpp"
#include "HashMapConcurrente.hpp"

double timespecToMs(struct timespec *spec)
{
    return spec->tv_sec * 1000 + (spec->tv_nsec * 1e-6);
}

double diffMs(struct timespec *start, struct timespec *end)
{
    return timespecToMs(end) - timespecToMs(start);
}

int main(int argc, char **argv)
{
    if (argc < 4) {
        std::cout << "Error: faltan argumentos." << std::endl;
        std::cout << std::endl;
        std::cout << "Modo de uso: " << argv[0] << " <threads_lectura> <threads_maximo>" << std::endl;
        std::cout << "    "
                  << "<archivo1> [<archivo2>...]" << std::endl;
        std::cout << std::endl;
        std::cout << "    threads_lectura: "
                  << "Cantidad de threads a usar para leer archivos." << std::endl;
        std::cout << "    threads_maximo: "
                  << "Cantidad de threads a usar para computar máximo." << std::endl;
        std::cout << "    archivo1, archivo2...: "
                  << "Archivos a procesar." << std::endl;
        return 1;
    }
    int cantThreadsLectura = std::stoi(argv[1]);
    int cantThreadsMaximo = std::stoi(argv[2]);

    std::vector<std::string> filePaths = {};
    for (int i = 3; i < argc; i++) {
        filePaths.push_back(argv[i]);
    }

    HashMapConcurrente hashMap = HashMapConcurrente();

#ifdef __BENCHMARK__
    struct timespec cargar_start, cargar_end, maximo_end;
    clock_gettime(CLOCK_REALTIME, &cargar_start);
#endif

    cargarMultiplesArchivos(hashMap, cantThreadsLectura, filePaths);

#ifdef __BENCHMARK__
    clock_gettime(CLOCK_REALTIME, &cargar_end);
#endif

    auto maximo = hashMap.maximoParalelo(cantThreadsMaximo);

#ifdef __BENCHMARK__
    clock_gettime(CLOCK_REALTIME, &maximo_end);
    std::cout << diffMs(&cargar_start, &cargar_end) << "," << diffMs(&cargar_end, &maximo_end) << "\n";
#endif

#ifndef __BENCHMARK__
    std::cout << maximo.first << " " << maximo.second << std::endl;
#endif

    return 0;
}
