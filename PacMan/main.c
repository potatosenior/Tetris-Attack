/*
Grupo JKL
Integrantes:
João Pedro de Oliveira Martins - 11921BCC017
Kaio Augusto de Souza - matrícula
Luis Gustavo Seiji Tateish - matrícula
*/
#include "SOIL.h"
#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>
#include <time.h>
#include "pacman.h"

// tempo para os blocos subirem
#define TEMPO_SUBIR 6

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

// Define as vari�veis e fun��es que ir�o controlar o jogo
Cenario *cen;
StructGrade *gradee;

int musica = 0; //variavel pra pausar a musica | 0 - tocando | 1 - pausada
int pause = 0;  //variavel pra pausar o jogo
int konamiEasterEgg = 0; // Easter Egg, ao atingir 10 � ativado!
time_t lastTimer, actualTimer, startTimer, auxTimer, timer_congelado;

void desenhaJogo();
void iniciaJogo();
void terminaJogo();
void desenhaFundo();
void desenhaPlacar();
void desenhaMaiorPlacar();
void maior_pontuacao_desenha();
void updateEasterEgg();
int tempo();
void updateCristais();

//fun�ao que cria e configura a janela de desenho OpenGL
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;


    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "Tetris Attack",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          800,  //largura
                          800,  //altura
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    // Chama a fun��o que inicializa o jogo
    iniciaJogo();
    startTimer = time(NULL);
    lastTimer = time(NULL);

    /* program main loop */
    while (!bQuit) {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT) {
                bQuit = TRUE;
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else {
            /* OpenGL animation code goes here */

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();
            // Esse la�o controla cada frame do jogo.

            // Desenhar o jogo aqui
            desenhaJogo();

            glPopMatrix();

            SwapBuffers(hDC);
        }
        actualTimer = time(NULL);
        //printf("Actual timer - last timer -> %d - %d = %d\n", actualTimer, lastTimer, actualTimer-lastTimer);
        if(actualTimer - lastTimer == TEMPO_SUBIR){ // Timer para adicionar uma nova linha
            //printf("valor de pause = %d - ", pause);
            if(grade_perdeu(gradee, cen) == 1 && pause == 0)
                adicionar_linha(cen, gradee);
            lastTimer = time(NULL);
        }
        if(grade_perdeu(gradee, cen) == 1 && pause == 0)
            timer_congelado = time(NULL);
    }
    // Saiu do la�o que desenha os frames?
    // Ent�o o jogo acabou.
    terminaJogo();

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);
    fflush(stdin);

    return msg.wParam;
}

// Fun��o que verifica se o teclado foi pressionado
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if(uMsg == WM_CLOSE) 
        PostQuitMessage(0);
    else 
        if(uMsg == WM_DESTROY) 
            return 0;
        else 
            if(uMsg == WM_KEYDOWN && grade_perdeu(gradee, cen) == 1){
                if(wParam == VK_ESCAPE || wParam == VK_DELETE){    //Pressionou ESC
                    PostQuitMessage(0);
                }else if(wParam == 0x50){   //Pressionou P e pausa/despausa o jogo
                    if(pause == 0){
                        pause = 1; printf("jogo pausado!\n");
                    }
                    else{
                        pause = 0; printf("jogo retomado!\n");
                    }
                }else if(wParam == VK_LEFT || wParam == 0x41){
                    if(pause == 0)
                    grade_movimenta(gradee, cen, 0);
                }else if(wParam == VK_DOWN || wParam == 0x53){
                    if(pause == 0)
                    grade_movimenta(gradee, cen, 1);
                }else if(wParam == VK_RIGHT || wParam == 0x44){
                    if(pause == 0)
                    grade_movimenta(gradee, cen, 2);
                }else if(wParam == VK_UP || wParam == 0x57){
                    if(pause == 0)
                    grade_movimenta(gradee, cen, 3);
                }else if(wParam == VK_RETURN || wParam == VK_SPACE){
                    if(pause == 0)
                    grade_mudar(cen, gradee);
                }else if(wParam == VK_CONTROL){
                    Alterar_score(gradee, 150); Alterar_high_score_grade(gradee, 150);
                }else if(wParam == 0x4D){
                    if(musica == 0){ 
                        PlaySound(NULL, NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
                        musica = 1;
                    }else{ 
                        PlaySound(TEXT("Songs/TitleTheme.wav"), NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
                        musica = 0;
                    }
                }
            }else {
                if(wParam == VK_ESCAPE || wParam == VK_DELETE)    //Pressionou ESC
                    PostQuitMessage(0);
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }

    return 0;
}

// Fun��o que configura o OpenGL
void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC) {
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);

    // Chama a fun��o que carrega as texturas do jogo
    carregaTexturas();
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering


}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

// ===================================================================
// Fun��es que controlam e desenham o jogo
// ===================================================================

// Fun��o que desenha cada componente do jogo
void desenhaJogo() {
    descerBlocos(cen);
    updateCristais(cen, gradee); 
    cenario_desenha(cen);
    pontuacao_desenha(gradee);
    maior_pontuacao_desenha(gradee);
    //tempo_desenha(tempo());
    tempo_desenha(timer_congelado - startTimer);
    grade_desenha(gradee);
    grade_perdeu(gradee, cen);
    
    if(grade_perdeu(gradee, cen) == 1 && pause == 0) {

    }

}
// Fun��o que inicia o mapa do jogo e as posi��es iniciais dos personagens
void iniciaJogo() {

    gradee = criar_grade(2, 9);
    Carregar_high_score(gradee);
    srand(time(NULL));
    cen = cenario_carrega();
    PlaySound(TEXT("Songs/TitleTheme.wav"), NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
    printf("jogo iniciado!\n");
}
// Fun��o que libera os dados do jogo
void terminaJogo() {
    Salvar_high_score(gradee);
    cenario_destroy(cen);
    destruir_grade(gradee);
    printf("jogo terminado!");
}
// ===================================================================
// Fun�ao relogio
// ===================================================================

int tempo(){
    auxTimer = time(NULL);
    return auxTimer - startTimer;
}

// ===================================================================
// Fun��es easter egg
// ===================================================================

void updateEasterEgg(WPARAM key){
    if(key == VK_UP && (konamiEasterEgg == 0 || konamiEasterEgg == 1)){
        konamiEasterEgg++;
    }else if(key == VK_DOWN && (konamiEasterEgg == 2 || konamiEasterEgg == 3)){
        konamiEasterEgg++;
    }else if(key == VK_LEFT && (konamiEasterEgg == 4 || konamiEasterEgg == 6)){
        konamiEasterEgg++;
    }else if(key == VK_RIGHT && (konamiEasterEgg == 5 || konamiEasterEgg == 7)){
        konamiEasterEgg++;
    }else if(key == 0x41 && konamiEasterEgg == 8){
        konamiEasterEgg++;
    }else if(key == 0x42 && konamiEasterEgg == 9){
        konamiEasterEgg++;
    }else konamiEasterEgg = 0;

    if(konamiEasterEgg == 10){
        PlaySound(TEXT("Songs/chatuba.wav"), NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
    }
}
