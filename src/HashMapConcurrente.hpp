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
    hashMapPair max_();
    hashMapPair maximoParalelo(unsigned int cantThreads);

   private:
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];

    hashMapPair *curr_max;

    static unsigned int hashIndex(std::string clave);

    std::mutex permisos_incrementar[HashMapConcurrente::cantLetras];
    std::atomic<unsigned int> proxima_fila = 0;
    std::mutex *permiso_max;
};

#endif /* HMC_HPP */
