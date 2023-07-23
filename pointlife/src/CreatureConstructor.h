//
//  CreatureConstructor.h
//  PointLifeCpp
//
//  Created by Scott SSE on 7/6/15.
//  Copyright (c) 2015 Scott SSE. All rights reserved.
//

#ifndef __PointLifeCpp__CreatureConstructor__
#define __PointLifeCpp__CreatureConstructor__

#include <stdio.h>
#include <vector>
#include "WorldData.h"
#include "Genome.h"
#include "OrganismCTOR.h"
#include "constants.h"

using namespace cbtz;

typedef CellCTOR * CellPtr;

class CreatureConstructor {
public:
    CreatureConstructor(WorldData &, Genome &);
    CreatureConstructor(WorldData &, const char ** asciArt);
    
    CellCTOR * createCell(int genomeIndex, float angle, float x, float y, int level = 0);
    CellCTOR * addCell(float x, float y);
    CellCTOR * addCell(BYTE * cellType, float x, float y);
    void addCell(int cellType, float x, float y);
    
    
    void processGenome(int genomeIndex, CellPtr iCellA = 0, CellPtr iCellB = 0, int level = 0, int maxLevel = 0,
                       float elasticity = 0);
    
    void processAsciArt();
    
    OrganismCTOR * go(float x = -1, float y = -1);
  
    void deleteAddedCells();
    
    void go(CellCTOR * pFromCell);
    
    void go(CellCTOR * pFromCell, float angle);
    
    void connectCell(CellCTOR *pFrom, CellCTOR *pTo, int portIndex);
    
private:
    bool finalize();

    void prepareCollisionGrid();
    void freeCollisionGrid();
    bool canAdd(float x, float y);
  
private:
    WorldData & mWorldData;
    const char ** mAsciArt;
    Genome & mGenome;
    float mOffsetX, mOffsetY;
    int mNumCells;
    int mIndex;
    int mTurns;
    int mEntityIndex;
    float mAngle;
    std::vector<CellPtr> mAddedCells;
    OrganismCTOR * pOrganism;
    OrganismCTOR & organism;
};

#endif /* defined(__PointLifeCpp__CreatureConstructor__) */
