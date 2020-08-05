#include <iostream>
#include <thread>
#include <vector>
using namespace std;
#define UNICODE
#include <stdio.h>
#include <Windows.h>
#include "mingw.thread.h"

wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char *pField = nullptr;
int nScreenWidth = 80;
int nScreenHeight = 30;

int Rotate(int px, int py, int r){
    int pi = 0;
  switch(r % 4){
    case 0:
      pi = py * 4 + px; // 0 degrees
      break;
    case 1:
     pi = 12 + py - (px * 4); // 90 degrees
     break;
    case 2:
    pi = 15 - (py * 4) - px; // 90 degrees
    break;
    case 3:
    pi = 3 - py + (px * 4); // 90 degrees
    break;
  }
  return pi;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY){
  for(int px = 0; px < 4; px++){
    for(int py = 0; py < 4; py++){
      //Get index into piece
      int pi = Rotate(px,py,nRotation);
      //Get index into field
      int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

      if(nPosX + px >= 0 && nPosX + px < nFieldWidth){
        if(nPosX + px >= 0 && nPosX + px < nFieldWidth){
          if(tetromino[nTetromino][pi] == L'X' && pField[fi] != 0){
            return false; //Fail on first hit
          }
        }
      }
    }
  }

  return true;
}

int main(){

  //Create assets
  /*
    tetromino[0].append(L"..X...X...X...X."); // Tetronimos 4x4
  	tetromino[1].append(L"..X..XX...X.....");
  	tetromino[2].append(L".....XX..XX.....");
  	tetromino[3].append(L"..X..XX..X......");
  	tetromino[4].append(L".X...XX...X.....");
  	tetromino[5].append(L".X...X...XX.....");
  	tetromino[6].append(L"..X...X..XX.....");
*/
  tetromino[0].append(L"..X.");
  tetromino[0].append(L"..X.");
  tetromino[0].append(L"..X.");
  tetromino[0].append(L"..X.");

  tetromino[1].append(L"..X.");
  tetromino[1].append(L".XX.");
  tetromino[1].append(L".X..");
  tetromino[1].append(L"....");

  tetromino[2].append(L".X..");
  tetromino[2].append(L".XX.");
  tetromino[2].append(L"..X.");
  tetromino[2].append(L"....");

  tetromino[3].append(L"....");
  tetromino[3].append(L".XX.");
  tetromino[3].append(L".XX.");
  tetromino[3].append(L"....");

  tetromino[4].append(L"..X.");
  tetromino[4].append(L".XX.");
  tetromino[4].append(L"..X.");
  tetromino[4].append(L"....");

  tetromino[5].append(L"....");
  tetromino[5].append(L".XX.");
  tetromino[5].append(L"..X.");
  tetromino[5].append(L"..X.");

  tetromino[6].append(L"....");
  tetromino[6].append(L".XX.");
  tetromino[6].append(L".X..");
  tetromino[6].append(L".X..");

  pField = new unsigned char[nFieldWidth*nFieldHeight]; //Create play field buffer

  for(int x = 0; x < nFieldWidth; x++){ //Board boundary
    for(int y = 0; y < nFieldHeight; y++){
      pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0; // If the playing field is equal to 0, or field width or field height, set it to 9 otherwise set it to 0;
    }
  }

  // Create Screen Buffer
  wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
  for (int i = 0; i < nScreenWidth*nScreenHeight; i++){
    screen[i] = L' ';
  }
  HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
  SetConsoleActiveScreenBuffer(hConsole);
  DWORD dwBytesWritten = 0;


//GAME LOGIC STUFF============================================================================
bool bGameOver = false;
int nCurrentPiece = 1;
int nCurrentRotation = 0;
int nCurrentX = nFieldWidth / 2;
int nCurrentY = 0;

bool bKey[4];
bool bRotateHold = false;

int nSpeed = 20;
int nSpeedCounter = 0;
bool bForceDown = false;
int nPieceCount = 0;
int nScore = 0;

vector<int> vLines;

while(! bGameOver){
//GAME TIMING ============================================================================
this_thread::sleep_for(chrono::milliseconds(50)); // Game tick
nSpeedCounter++;
bForceDown = (nSpeedCounter == nSpeed);

//INPUT ============================================================================
for(int k = 0; k < 4; k++){                            //Right Left Z
  bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
}
//GAME LOGIC ============================================================================
nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1:0;
nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1:0;
nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1:0;
if(bKey[3]){
  nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1:0;
  bRotateHold = true;
}else{ bRotateHold = false;}

if(bForceDown){
  if(DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)){
    nCurrentY++;
  }else{
    //Lock piece in the Field
    for(int px = 0; px < 4; px++){
      for(int py = 0; py < 4; py++){
        if(tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X'){
          pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
        }
      }
    }

    nPieceCount++;
    if(nPieceCount % 10 == 0){
      if(nSpeed >= 10){
        nSpeed--;
      }
    }

    //Check have got any Lines
for(int py = 0; py < 4; py++){
  if(nCurrentY + py < nFieldHeight - 1){
    bool bLine = true;
    for(int px = 1; px<nFieldWidth-1; px++){
      bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;
    }
    if(bLine){
      //Remove Line, set to =
      for(int px = 1; px<nFieldWidth-1; px++){
        pField[(nCurrentY + py) * nFieldWidth + px] = 8;
      }
      vLines.push_back(nCurrentY + py);
    }
  }
}

nScore += 25;
if(!vLines.empty()){
  nScore += (1<<vLines.size()) * 100;
}
    //choose next piece
    nCurrentX = nFieldWidth / 2;
    nCurrentY = 0;
    nCurrentRotation = 0;
    nCurrentPiece = rand() % 7;
    //if piece does not fit
    bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
  }
nSpeedCounter = 0;
}
//RENDER OUTPUT ============================================================================

  //Draw field
  for(int x = 0; x < nFieldWidth; x++){
    for(int y = 0; y < nFieldHeight; y++){
      screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[(y * nFieldWidth) + x]]; // If the playing field is equal to 0, or field width or field height, set it to 9 otherwise set it to 0;
    }
  }

  //Draw current pieces
  for(int px = 0; px < 4; px++){
    for(int py = 0; py < 4; py++){
      if(tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X'){
        screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
      }
    }
  }
//Draw Score

snwprintf(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);
  if(!vLines.empty()){
    //Display Frame
    WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    this_thread::sleep_for(chrono::milliseconds(400));

    for(auto &v : vLines){
      for(int px = 1; px < nFieldWidth - 1; px++){
        for(int py = v; py>0; py--){
          pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
        }
        pField[px] = 0;
      }
    }

    vLines.clear();
  }
  //Display frame
  WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
}

//end
CloseHandle(hConsole);
cout<<"Game Over!! Score: "<< nScore<<endl;
system("pause");

  return 0;
}
