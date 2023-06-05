#ifndef CHM_CPP
#define CHM_CPP

#include "HashMapConcurrente.hpp"

#include <pthread.h>

#include <fstream>
#include <iostream>

HashMapConcurrente::HashMapConcurrente()
{
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
        permisos_tabla[i] = new std::mutex;
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave)
{
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave)
{
    unsigned int index = hashIndex(clave);

    std::lock_guard<std::mutex> lock(*permisos_tabla[index]);
    for (auto it = tabla[index]->crearIt(); it.haySiguiente(); it.avanzar()) {
        if (it.siguiente().first == clave) {
            it.siguiente().second++;
            return;
        }
    }

    tabla[index]->insertar(hashMapPair(clave, 1));
    return;
}

std::vector<std::string> HashMapConcurrente::claves()
{
    std::vector<std::string> claves;

    for (unsigned int i = 0; i < cantLetras; i++) {
        for (auto it = tabla[i]->crearIt(); it.haySiguiente(); it.avanzar()) {
            claves.emplace_back(it.siguiente().first);
        }
    }

    return claves;
}

unsigned int HashMapConcurrente::valor(std::string clave)
{
    unsigned int index = hashIndex(clave);

    for (auto it = tabla[index]->crearIt(); it.haySiguiente(); it.avanzar()) {
        if (it.siguiente().first == clave) {
            return it.siguiente().second;
        }
    }

    return 0;
}

hashMapPair HashMapConcurrente::maximo()
{
    hashMapPair *max = new hashMapPair("", 0);

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        std::lock_guard<std::mutex> lock(*permisos_tabla[index]);
        for (auto it = tabla[index]->crearIt(); it.haySiguiente(); it.avanzar()) {
            if (it.siguiente().second > max->second) {
                max->first = it.siguiente().first;
                max->second = it.siguiente().second;
            }
        }
    }

    return *max;
}

hashMapPair HashMapConcurrente::maximoPorIndice(unsigned int index)
{
    hashMapPair *max = new hashMapPair("", 0);

    std::lock_guard<std::mutex> lock(*permisos_tabla[index]);
    for (auto it = tabla[index]->crearIt(); it.haySiguiente(); it.avanzar()) {
        if (it.siguiente().second > max->second) {
            max->first = it.siguiente().first;
            max->second = it.siguiente().second;
        }
    }

    return *max;
}

struct maximoThreadArgs {
    HashMapConcurrente *hash_map;
    std::atomic<unsigned int> *proximo_index;
    std::atomic<hashMapPair *> *maximo_global;
};

void *maximoThread(void *argv)
{
    maximoThreadArgs *args = (maximoThreadArgs *)argv;
    hashMapPair *thread_max = new hashMapPair("", 0);
    unsigned int index;

    while ((index = args->proximo_index->fetch_add(1)) < args->hash_map->cantLetras) {
        hashMapPair index_max = args->hash_map->maximoPorIndice(index);
        if (index_max.second > thread_max->second) {
            *thread_max = index_max;
        }
    }

    hashMapPair *maximo_expected = args->maximo_global->load();
    while (thread_max->second > maximo_expected->second && !args->maximo_global->compare_exchange_weak(maximo_expected, thread_max)) {
        ;
    }

    return nullptr;
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads)
{
    std::atomic<unsigned int> proximo_index(0);
    std::atomic<hashMapPair *> maximo(new hashMapPair("", 0));
    std::vector<pthread_t> threads(cantThreads);

    maximoThreadArgs args = (maximoThreadArgs){this, &proximo_index, &maximo};

    for (unsigned int i = 0; i < cantThreads; i++) {
        pthread_create(&threads[i], NULL, maximoThread, &args);
    }

    for (unsigned int i = 0; i < cantThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    return *maximo.load();
}

#endif
