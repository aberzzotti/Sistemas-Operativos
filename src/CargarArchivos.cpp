#ifndef CHM_CPP
#define CHM_CPP

#include "CargarArchivos.hpp"

#include <pthread.h>

#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath)
{
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}

void cargarMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths)
{
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < cantThreads; i++) {
        threads.emplace_back(cargarArchivo, std::ref(hashMap), filePaths[i]);
    }

    for (unsigned int i = 0; i < cantThreads; i++) {
        threads[i].join();
    }
}

#endif
