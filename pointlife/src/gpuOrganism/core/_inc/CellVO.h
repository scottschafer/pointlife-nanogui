#define CELL_VO

#define LOCAL_DATA_COUNT 4
#define GLOBAL_DATA_COUNT 8

enum {
  WHEEL_LEFT = 0,
  WHEEL_RIGHT = 1
};

typedef struct CellVO {
  float x;
  float y;
  float velX;
  float velY;
  int vertexId;
  int nextInGrid;
  int organismIndex;
  int connectionCount;
  int connections[MAX_CONNECTIONS]; // indices into connections
  
  float localData[LOCAL_DATA_COUNT];
  
//  
//  int vertexType;
//  
//  int actionType;
//  int actionData;
//  int activationDuration; // turns to keep applying this action
//  float actionX;          // if an action is directional (move, look), the vector
//  float actionY;
//  
//  int counter;            // used for logic n' stuff
//  
//  int signalResponses[MAX_CONNECTIONS]; // we can have different responses for signals received through different connections
  

  //float connectionMinDistance[MAX_CONNECTIONS];
  //float connectionMaxDistance[MAX_CONNECTIONS];
} CellVO;
