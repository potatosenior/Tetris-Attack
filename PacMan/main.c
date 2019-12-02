/*
Grupo JKL
Integrantes:
João Pedro de Oliveira Martins - 11921bcc017
Kaio Augusto de Souza - 11921bcc040
Luis Gustavo Seiji Tateishi - 11921bcc034
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

// Define as variaveis e funcoes que irao controlar o jogo
Cenario *cen;
StructGrade *gradee;

int iniciar = 0;//variavel pra definir quando iniciar o jogo
int reiniciar_pag = 1;//qual opcao vai escolhre, 1-reiniciar,2-sair
int musica = 0; //variavel pra pausar a musica | 0 - tocando | 1 - pausada
int pause = 0;  //variavel pra pausar o jogo
int konamiEasterEgg = 0; // Easter Egg, ao atingir 10 � ativado!
time_t lastTimer, actualTimer, startTimer = 0, auxTimer, timer_congelado;

void desenhaJogo();
void iniciaJogo();
void terminaJogo();
void desenhaFundo();
void desenhaPlacar();
void desenhaMaiorPlacar();
void maior_pontuacao_desenha();
void atualizarTempo();
void updateEasterEgg();
void updateCristais();
void reiniciaJogo();

//funcao que cria e configura a janela de desenho OpenGL
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

    // Chama a funcao que inicializa o jogo
    iniciaJogo();
    //startTimer = time(NULL);
    //lastTimer = time(NULL);

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
            Sleep(100);
            /* OpenGL animation code goes here */

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();
            // Esse laco controla cada frame do jogo.

            //printf("iniciar = %d, startTimer = %d\n", iniciar, startTimer);
            if(iniciar != 0 && startTimer == 0){
                startTimer = time(NULL);
                lastTimer = time(NULL);
                printf("Cronometro inciado!\n");
            }

            atualizarTempo();

            // Desenhar o jogo aqui
            desenhaJogo();

            glPopMatrix();

            SwapBuffers(hDC);
        }
    }
    // Saiu do laco que desenha os frames?
    // Entao o jogo acabou.
    terminaJogo();

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);
    fflush(stdin);

    return msg.wParam;
}

// Funcao que verifica se o teclado foi pressionado
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if(uMsg == WM_CLOSE)
        PostQuitMessage(0);
    else
        if(uMsg == WM_DESTROY)
            return 0;
        else
            if(uMsg == WM_KEYDOWN && grade_perdeu(gradee, cen) == 1){
                if(iniciar == 0){   //inicar o jogo
                    iniciar = 1;
                    printf("Pressionou para iniciar!\n");
                }else if(wParam == VK_ESCAPE){    //Pressionou ESC
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
                    adicionar_linha(cen, gradee);
                    //Alterar_score(gradee, 150); Alterar_high_score_grade(gradee, 150);
                }else if(wParam == 0x4D){
                    if(musica == 0){
                        PlaySound(NULL, NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
                        musica = 1;
                    }else{
                        PlaySound(TEXT("Songs/TitleTheme.wav"), NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
                        musica = 0;
                    }
                }
                updateEasterEgg(wParam);
            }else { //verifica se apertou pra sair mesmo se o jogo estiver pausado ou se o jogador perdeu
                if(wParam == VK_ESCAPE)    //Pressionou ESC
                    PostQuitMessage(0);
                if(wParam == VK_UP || wParam == 0x57){      //escolhe a opcao
                    if(grade_perdeu(gradee, cen) == 0)
                        if(reiniciar_pag == 2)
                            reiniciar_pag = 1;
                }
                if(wParam == VK_DOWN || wParam == 0x53){    //escolhe a opcao
                    if(grade_perdeu(gradee, cen) == 0)
                        if(reiniciar_pag == 1)
                            reiniciar_pag = 2;
                }
                if(wParam == VK_RETURN || wParam == VK_SPACE){  //seleciona a opcao
                    if(grade_perdeu(gradee, cen) == 0){
                        if(reiniciar_pag == 2){     //sair do jogo
                            terminaJogo();
                            PostQuitMessage(0);
                        }else{                  //reiniciar o jogo
                            printf("-----------------\nreiniciar o jogo!\n---------------\n");
                            reiniciaJogo();
                        }
                    }
                }

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
// Funcoes que controlam e desenham o jogo
// ===================================================================

// Funcao que desenha cada componente do jogo
void desenhaJogo() {
    if(iniciar == 0){       //faz a tela inicial
        cenario_desenha(cen);
        pagInicial_desenha();
    }else{
        //verifica se o jogador perdeu
        grade_perdeu(gradee, cen); //1 = nao perdeu, 0 = perdeu

        if(grade_perdeu(gradee, cen) == 1) {    //nao perdeu
            if(pause == 1){         //jogo pausado
                cenario_desenha(cen);
                pontuacao_desenha(gradee);
                maior_pontuacao_desenha(gradee);
                tempo_desenha(timer_congelado - startTimer);
            }else{  //jogo nao pausado
                descerBlocos(cen);
                updateCristais(cen, gradee);
                //desenha tudo
                cenario_desenha(cen);
                pontuacao_desenha(gradee);
                maior_pontuacao_desenha(gradee);
                tempo_desenha(timer_congelado - startTimer);
                grade_desenha(gradee);
            }
        }else{         //perdeu
            cenario_desenha(cen);
            pontuacao_desenha(gradee);
            maior_pontuacao_desenha(gradee);
            tempo_desenha(timer_congelado - startTimer);
            pagFinal_desenha(reiniciar_pag);
        }     
    }

}
// Funcao que inicia as variaveis do jogo
void iniciaJogo() {

    gradee = criar_grade(2, 9);
    Carregar_high_score(gradee);
    srand(time(NULL));
    cen = cenario_carrega();
    PlaySound(TEXT("Songs/TitleTheme.wav"), NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
    printf("jogo iniciado!\n");
}
// Funcao que libera os dados do jogo
void terminaJogo() {
    Salvar_high_score(gradee);
    cenario_destroy(cen);
    destruir_grade(gradee);
    printf("jogo terminado!");
}
//funcao que reinicia o jogo
void reiniciaJogo(){
    terminaJogo();
    iniciar = 0;
    reiniciar_pag = 1;
    musica = 0;
    pause = 0;
    startTimer = 0;
    lastTimer = 0;
    iniciaJogo();
    //printf("Variaveis reiniciadas:\niniciar = %d\nreiniciar_pag = %d\nstartTimer = %d\n", iniciar, reiniciar_pag, startTimer);
}
// ===================================================================
// Funcao relogio
// ===================================================================

void atualizarTempo(){
    actualTimer = time(NULL);
    if(actualTimer - lastTimer == TEMPO_SUBIR){ // Timer para adicionar uma nova linha
        if(grade_perdeu(gradee, cen) == 1 && pause == 0)
            adicionar_linha(cen, gradee);
            lastTimer = time(NULL);
        }
    if(grade_perdeu(gradee, cen) == 1 && pause == 0)
        timer_congelado = time(NULL);
}

// ===================================================================
// Funcoes easter egg
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
