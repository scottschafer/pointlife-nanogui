#pragma once

#include "constants.h"
#include "Color.h"

#define PADDED_GENOME_LENGTH (GENOME_LENGTH+16)

class Genome {
    
public:
    Genome();
    
    Genome & operator = (const Genome & src);
    void randomize();
    Genome breed(const Genome &);
    void mutate();
    
    BYTE genome[PADDED_GENOME_LENGTH];
    
    Color color;
    
private:
    void determineColor();
};
