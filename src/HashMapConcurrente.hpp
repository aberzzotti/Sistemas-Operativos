#ifndef HMC_HPP
#define HMC_HPP

#include <atomic>
#include <mutex>
#include <string>
#include <vector>

#include "ListaAtomica.hpp"

typedef std::pair<std::string, unsigned int> hashMapPair;

class HashMapConcurrente {
   public:
    static const unsigned int cantLetras = 26;

    HashMapConcurrente();

    void incrementar(std::string clave);
    std::vector<std::string> claves();
    unsigned int valor(std::string clave);

    hashMapPair maximo();
    hashMapPair maximoPorIndice(unsigned int indice);
    hashMapPair maximoParalelo(unsigned int cantThreads);

   private:
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];
    std::mutex *permisos_tabla[HashMapConcurrente::cantLetras];

    static unsigned int hashIndex(std::string clave);

    void bloquearListas() {
        for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
            permisos_tabla[index]->lock();
        }
    }
};

#endif /* HMC_HPP */
