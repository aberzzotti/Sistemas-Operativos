#ifndef HMC_ARCHIVOS_HPP
#define HMC_ARCHIVOS_HPP

#include <ctype.h>
#include <pthread.h>

#include <string>
#include <vector>

#include "HashMapConcurrente.hpp"

int cargarArchivo(HashMapConcurrente &hashMap, std::string filePath);

void cargarMultiplesArchivos(HashMapConcurrente &hashMap, unsigned int cantThreads, std::vector<std::string> filePaths);

#endif /* HMC_ARCHIVOS_HPP */
