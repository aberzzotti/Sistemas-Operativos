#ifndef CHM_CPP
#define CHM_CPP

#include "HashMapConcurrente.hpp"

#include <pthread.h>

#include <fstream>
#include <iostream>
#include <thread>

HashMapConcurrente::HashMapConcurrente()
{
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }

    this->curr_max = new hashMapPair;
    this->curr_max->second = 0;

    this->permiso_max = new std::mutex;
}

unsigned int HashMapConcurrente::hashIndex(std::string clave)
{
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave)
{
    unsigned int index = this->hashIndex(clave);
    std::lock_guard<std::mutex> lock(this->permisos_incrementar[index]);
    for (auto it = tabla[index]->crearIt(); it.haySiguiente(); it.avanzar()) {
        if (it.siguiente().first == clave) {
            it.siguiente().second++;
            return;
        }
    }
    tabla[index]->insertar(make_pair(clave, 1));
    return;
}

std::vector<std::string> HashMapConcurrente::claves()
{
    std::vector<std::string> claves;
    for (unsigned int index = 0; index < this->cantLetras; index++) {
        for (auto it = tabla[index]->crearIt(); it.haySiguiente(); it.avanzar()) {
            claves.emplace_back(it.siguiente().first);
        }
    }
    return claves;
}

unsigned int HashMapConcurrente::valor(std::string clave)
{
    unsigned int index = this->hashIndex(clave);

    unsigned int valor = 0;
    for (auto it = tabla[index]->crearIt(); it.haySiguiente(); it.avanzar()) {
        if (it.siguiente().first == clave) {
            valor = it.siguiente().second;
        }
    }
    return valor;
}

hashMapPair HashMapConcurrente::maximo()
{
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        std::lock_guard<std::mutex> lock(this->permisos_incrementar[index]);
        for (
            auto it = tabla[index]->crearIt();
            it.haySiguiente();
            it.avanzar()) {
            if (it.siguiente().second > max->second) {
                max->first = it.siguiente().first;
                max->second = it.siguiente().second;
            }
        }
    }

    return *max;
}

hashMapPair HashMapConcurrente::max_() {
    hashMapPair *max = new hashMapPair();
    unsigned int index = this->proxima_fila.fetch_add(1);
    std::lock_guard<std::mutex> lock(this->permisos_incrementar[index]);
    for (auto it = tabla[index]->crearIt(); it.haySiguiente();it.avanzar()) {
        if (it.siguiente().second > max->second) {
            max->first = it.siguiente().first;
            max->second = it.siguiente().second;
        }
    }

    return *max;
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads)
{
    std::vector<std::thread> threads;
    std::vector<hashMapPair> maximos(this->cantLetras);
    // for (unsigned int i = 0; i < cantThreads; i++) {
    //     threads.emplace_back(max_);
    //     // threads.emplace_back(this-maximo, std::ref(), filePaths[i]);
    // }
}

#endif