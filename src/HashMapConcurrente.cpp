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

    for (unsigned int i = 0; i < cantLetras; i++) {
        lockIndice(i);
    }

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        hashMapPair max_local = maximoPorIndice(index);
        *max = (max_local.second > max->second) ? max_local : *max;
        unlockIndice(index);
    }

    return *max;
}

hashMapPair HashMapConcurrente::maximoPorIndice(unsigned int index)
{
    hashMapPair *max = new hashMapPair("", 0);
    for (auto it = tabla[index]->crearIt(); it.haySiguiente(); it.avanzar()) {
        *max = (it.siguiente().second > max->second) ? it.siguiente() : *max;
    }

    return *max;
}

struct maximoThreadArgs {
    HashMapConcurrente *hash_map;
    std::atomic<unsigned int> *proximo_index;
    hashMapPair *max;
};

void *maximoThread(void *argv)
{
    maximoThreadArgs *args = (maximoThreadArgs *)argv;

    unsigned int index;
    while ((index = args->proximo_index->fetch_add(1)) < args->hash_map->cantLetras) {
        hashMapPair max_local = args->hash_map->maximoPorIndice(index);
        *(args->max) = (max_local.second > args->max->second) ? max_local : *(args->max);

        // Desbloqueo lista ya recorrida
        args->hash_map->unlockIndice(index);
    }

    return nullptr;
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads)
{
    std::atomic<unsigned int> proximo_index(0);
    std::vector<pthread_t> threads(cantThreads);

    std::vector<hashMapPair> maximos(cantThreads);
    for (unsigned int i = 0; i < cantThreads; i++) {
        maximos.push_back(hashMapPair("", 0));
    }

    // Lockeamos el hashmap entero, luego a medida que van procesando los thread liberan la lista correspondiente
    for (unsigned int i = 0; i < cantLetras; i++) {
        lockIndice(i);
    }

    for (unsigned int i = 0; i < cantThreads; i++) {
        maximoThreadArgs args = (maximoThreadArgs){this, &proximo_index, &maximos[i]};
        pthread_create(&threads[i], NULL, maximoThread, &args);
    }

    for (unsigned int i = 0; i < cantThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    hashMapPair maximo("", 0);
    for (unsigned int i = 0; i < cantThreads; i++) {
        if (maximos[i].second > maximo.second) {
            maximo = maximos[i];
        }
    }

    return maximo;
}

#endif
