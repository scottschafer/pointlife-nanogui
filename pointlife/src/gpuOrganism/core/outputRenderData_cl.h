#define PUT(val) if (out_renderData<stop_renderData) *out_renderData++ = val;
#define TO_RGBA(r,g,b,a) ((r&255)<<24) | ((g&255)<<16) | ((b&255)<<8) | (a&255)
#define TO_IC(v) int(INT_MAX*(float(v)/WORLD_DIM))

#define SET_LINE_COLOR(color) PUT(RCMD_SET_LINE_COLOR); PUT(color); ++out_numCommands;
#define SET_FILL_COLOR(color) PUT(RCMD_SET_FILL_COLOR); PUT(color); ++out_numCommands;
#define CIRCLE(x,y,r) PUT(RCMD_CIRCLE); PUT(TO_IC(x)); PUT(TO_IC(y)); PUT(TO_IC(r)); ++out_numCommands;
#define LINE(x1,y1,x2,y2) PUT(RCMD_LINE); PUT(TO_IC(x1)); PUT(TO_IC(y1)); PUT(TO_IC(x2)); PUT(TO_IC(y2)); +out_numCommands;

kernel void outputRenderDataOrganisms(
                                      __global int* renderData,
                                      int renderDataSize,
                                      
                                      int focusedCritter,
                                      
                                      int numOrganisms,
                                      __global float* organismData,
                                      
                                      int numConnections,
                                      __global float* connectionData,
                                      
                                      int numCells,
                                      __global float* cellData
                                      ) {
  
  __global int* startRender = renderData;
  __global int* out_renderData = renderData + 3;
  __global int* out_focusCircle;
  __global int* stop_renderData = renderData + renderDataSize - 10;
  
  int out_numCommands = 0;
  
  CellVO * pCellVO = (CellVO*) (cellData);
  ConnectionVO * pConnections = (ConnectionVO*) (connectionData);
  OrganismVO * pOrganisms = (OrganismVO*) (organismData);
  
  SET_FILL_COLOR(TO_RGBA(128, 128, 255, 100));
  
  out_focusCircle = out_renderData;
  --out_numCommands;
  CIRCLE(0, 0, 0);
  
  pConnections;
  pOrganisms;
  
  float focusX = 0;
  float focusY = 0;
  float focusCellCount = 0;
  
  OrganismVO * pFocusedOrganism = & pOrganisms[focusedCritter];
  
  SET_FILL_COLOR(TO_RGBA(255,255,255,192));
  
  for (int i = 0; i < numCells; i++) {
    if (pCellVO->connectionCount < 0) break;
    
    if (pCellVO->organismIndex == focusedCritter) {
      focusX += pCellVO->x;
      focusY += pCellVO->y;
      ++focusCellCount;
    }
    
    //__global int* checkCircle = out_renderData;
    CIRCLE(pCellVO->x, pCellVO->y, CELL_SIZE/2.0f);
    //printf("%d, %d, %d, %d\n", checkCircle[0], checkCircle[1], checkCircle[2], checkCircle[3]);
    
    int iCoord = TO_IC(pCellVO->x);
    ++pCellVO;
  }
  
  if (focusCellCount) {
    focusX /= focusCellCount;
    focusY /= focusCellCount;
    
    out_renderData = out_focusCircle;
    CIRCLE(focusX, focusY, CELL_SIZE * 10);
  }
  
  startRender[0] = TO_IC(focusX);
  startRender[1] = TO_IC(focusY);
  startRender[2] = out_numCommands;
}


